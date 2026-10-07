// -----------------------------------------------------------------------------
// LU6JMF - Marcelo Ferreira (Oct/2026)
// lora_compat.cpp - LoRa radio behind the ESP32APRS_Audio v2.1.5 core API
//
// The 2.1.5 core (TX scheduler with job/priority, watchdog, /txdebug, black box,
// RX processing in taskAPRS, KISS, PRIME...) was written against the audio
// modem library (LibAPRS_ESP32: AFSK + AX.25 rings + PTT). This file provides
// the same entry points for the LoRa radio (LibAPRS_LoRa + RadioLib), so the
// core runs unchanged:
//
//   audio concept                 LoRa equivalent
//   ---------------------------   ---------------------------------------------
//   PTT ON  (getTransmit())       a frame was handed to the radio (ax25_stateTx)
//   PTT OFF (pttOff = true)       radioHal->transmit() returned (loraTxDone())
//   AX.25 RX ring                 frames decoded by the radio (loraRxPushFrame)
//   Ax25TxAbort / RX soft reset   drop the pending frame + re-init the radio
//   AFSK timers / tone / gains    not applicable -> no-ops
//
// Nothing here touches the radio from the caller's task except through
// flags: the radio itself is only driven by taskAPRSPoll (APRS_poll()).
// -----------------------------------------------------------------------------
#include <Arduino.h>
#include "AX25.h"
#include "FIFO.h"
#include "LibAPRSesp.h"

extern FIFOBuffer txFifo;
extern float rssi;
extern float snr;
extern float freqErr;
extern Configuration config;

// ---- globals the 2.1.5 core declares as extern (owned by AFSK.cpp in the audio build)
volatile int8_t adcEn = 0;
volatile int8_t dacEn = 0;
volatile bool pttOff = false;
volatile uint32_t adcIsrCount = 0;
volatile int fifoSampleCount = 0;
volatile bool hw_afsk_dac_isr = false;
int offset = 0; // audio ADC DC offset (shown on the MOD page); no meaning on LoRa

// ---- LoRa TX lifecycle -----------------------------------------------------
static volatile int16_t loraLastTxState = 0;
static volatile uint32_t loraTxOk = 0;
static volatile uint32_t loraTxErr = 0;
static volatile bool loraRecoverReq = false;

// Called by LibAPRS.cpp (taskAPRSPoll context) right after radioHal->transmit()
// returned and RX was restarted. This is the LoRa "PTT OFF".
static volatile uint32_t loraTxBusy = 0;
uint32_t loraGetTxBusy() { return loraTxBusy; }

// TNC2 text of the frame currently handed to the radio (to re-queue it if the
// channel was busy).
static char loraFrameBuf[300];
static size_t loraFrameLen = 0;
void loraRememberFrame(const uint8_t *raw, size_t len)
{
    if (len >= sizeof(loraFrameBuf))
        len = sizeof(loraFrameBuf) - 1;
    memcpy(loraFrameBuf, raw, len);
    loraFrameBuf[len] = 0;
    loraFrameLen = len;
    loraLastTxState = 0; // a new lifecycle starts: forget the previous result
}
const char *loraLastFrame(size_t *len)
{
    if (len)
        *len = loraFrameLen;
    return loraFrameBuf;
}

void loraTxDone(int16_t state)
{
    loraLastTxState = state;
    if (state == 0)
        loraTxOk++;
    else if (state == LORA_TX_CHANNEL_BUSY)
        loraTxBusy++;
    else
        loraTxErr++;
    pttOff = true;
}

int16_t loraGetLastTxState() { return loraLastTxState; }
uint32_t loraGetTxOk() { return loraTxOk; }
uint32_t loraGetTxErr() { return loraTxErr; }

// Radio recovery request (watchdog). Consumed by AFSK_Poll() below, which runs
// in taskAPRSPoll - the only task that owns the radio.
bool loraRecoverPending() { return loraRecoverReq; }

// ---- RX ring: raw AX.25 frames (without FCS) + signal report ----------------
#define LORA_RX_SLOTS 8
typedef struct
{
    uint8_t data[AX25_MAX_FRAME_LEN];
    uint16_t len;
    float rssi;
    float snr;
    float freqErr;
} LoraRxSlot;

static LoraRxSlot loraRx[LORA_RX_SLOTS];
static LoraRxSlot loraRxCurrent; // frame handed to the core (stays valid until the next read)
static volatile uint8_t loraRxHead = 0;
static volatile uint8_t loraRxTail = 0;
static volatile uint32_t loraRxDropped = 0;
static portMUX_TYPE loraRxMux = portMUX_INITIALIZER_UNLOCKED;

uint32_t loraGetRxDropped() { return loraRxDropped; }

bool loraRxPushFrame(const uint8_t *frame, size_t len)
{
    if (frame == NULL || len < 15 || len > AX25_MAX_FRAME_LEN)
        return false;
    bool ok = false;
    portENTER_CRITICAL(&loraRxMux);
    uint8_t next = (loraRxHead + 1) % LORA_RX_SLOTS;
    if (next != loraRxTail)
    {
        LoraRxSlot *s = &loraRx[loraRxHead];
        memcpy(s->data, frame, len);
        s->len = len;
        s->rssi = rssi;
        s->snr = snr;
        s->freqErr = freqErr;
        loraRxHead = next;
        ok = true;
    }
    else
    {
        loraRxDropped++;
    }
    portEXIT_CRITICAL(&loraRxMux);
    return ok;
}

bool Ax25NewRxFrames(void)
{
    return loraRxHead != loraRxTail;
}

// Same signature as the audio library. peak/valley/level/corrected have no
// LoRa meaning (0); mV stays 0. The frame's RSSI/SNR/freq error are copied to
// the core globals rssi/snr/freqErr so the code that processes this frame
// (last heard list, web socket, logs) sees the values of THIS packet.
bool Ax25ReadNextRxFrame(uint8_t **dst, uint16_t *size, int8_t *peak, int8_t *valley,
                         uint8_t *level, uint8_t *corrected, uint16_t *mV)
{
    bool ok = false;
    portENTER_CRITICAL(&loraRxMux);
    if (loraRxHead != loraRxTail)
    {
        memcpy(&loraRxCurrent, &loraRx[loraRxTail], sizeof(LoraRxSlot));
        loraRxTail = (loraRxTail + 1) % LORA_RX_SLOTS;
        ok = true;
    }
    portEXIT_CRITICAL(&loraRxMux);
    if (!ok)
        return false;

    rssi = loraRxCurrent.rssi;
    snr = loraRxCurrent.snr;
    freqErr = loraRxCurrent.freqErr;
    if (dst) *dst = loraRxCurrent.data;
    if (size) *size = loraRxCurrent.len;
    if (peak) *peak = 0;
    if (valley) *valley = 0;
    if (level) *level = 0;
    if (corrected) *corrected = 0;
    if (mV) *mV = 0;
    return true;
}

// ---- TX side ------------------------------------------------------------------
// "PTT" on LoRa = the radio is really on the air (set by LibAPRS around
// radioHal->transmit(), after listen-before-talk). So /txdebug "wait to PTT" is
// the queue + channel wait and the learned airtime is the real airtime.
static volatile bool loraOnAir = false;
void loraSetOnAir(bool on) { loraOnAir = on; }
bool getTransmit() { return loraOnAir; }
bool getReceive() { return false; }
uint8_t ModemDcdState(void) { return 0; }
void setPtt(bool state) { (void)state; }

// TX is done by APRS_poll() in taskAPRSPoll; nothing to pump here.
void Ax25TransmitBuffer(void) {}
void Ax25TransmitCheck(void) {}

void Ax25GetTxDiag(uint8_t *head, uint8_t *tail, bool *bufferFull, uint8_t *initStage)
{
    if (head) *head = 0;
    if (tail) *tail = 0;
    if (bufferFull) *bufferFull = false;
    if (initStage) *initStage = ax25_stateTx ? 2 : 0; // 2 = TRANSMITTING (same code as audio)
}

// Watchdog recovery: drop the pending frame and ask taskAPRSPoll to re-init
// the radio. The core raises pttOff itself right after calling this.
void Ax25TxAbort(bool keyed)
{
    (void)keyed;
    fifo_flush(&txFifo);
    ax25_stateTx = false;
    loraRecoverReq = true;
}

void AFSK_RequestRxSoftReset(void) { loraRecoverReq = true; }

// ---- audio-only controls: no LoRa meaning ------------------------------------
void AFSK_TimerEnable(bool sts) { (void)sts; }
void DAC_TimerEnable(bool sts) { (void)sts; }
bool AFSK_DacTimerRunning(void) { return false; }
bool AFSK_IsTxCalibrationActive(void) { return false; }
bool AFSK_StartTxCalibration(uint8_t toneType) { (void)toneType; return false; }
void afskSetHPF(bool val) { (void)val; }
void afskSetBPF(bool val) { (void)val; }
void afskSetADCAtten(uint8_t val) { (void)val; }
void afskSetPTT(int8_t val, bool act) { (void)val; (void)act; }
void afskSetPWR(int8_t val, bool act) { (void)val; (void)act; }
void afskSetSQL(int8_t val, bool act) { (void)val; (void)act; }
void afskSetModem(uint8_t val, bool bpf, uint16_t timeSlot, uint16_t preamble, uint8_t fx25Mode) {}
void afskSetTxGain(uint8_t gainPercent) { (void)gainPercent; }
void afskSetTxPreemphasis(bool enable, uint8_t tiltPercent) { (void)enable; (void)tiltPercent; }
void APRS_setCallsign(char *call, int ssid) { (void)call; (void)ssid; }
void APRS_setPreamble(unsigned long pre) { (void)pre; }

// ---- radio task entry points used by taskAPRSPoll of the 2.1.5 core -----------
static bool loraRadioUp = false;

// Audio signature kept; LoRa ignores the pins (they come from config.rf_*).
void AFSK_init(int8_t adc_pin, int8_t dac_pin, int8_t ptt_pin, int8_t sql_pin, int8_t pwr_pin,
               int8_t led_tx_pin, int8_t led_rx_pin, int8_t led_strip_pin,
               bool ptt_act, bool sql_act, bool pwr_act)
{
    loraRadioUp = APRS_init(&config);
    // Always printed (not log_*): the release build has logging off, and this is
    // the first thing to check on a new board / after an update.
    Serial.printf("[LoRa] radio init %s: type=%d %.3f MHz SF%d BW%.1f CR4/%d sync=0x%02X pwr=%d dBm TX=%s\r\n",
                  loraRadioUp ? "OK" : "FAILED", config.rf_type, config.rf_freq, config.rf_sf,
                  config.rf_bw, config.rf_cr, config.rf_sync, config.rf_power,
                  config.rf_ax25 ? "AX.25" : "TNC2(<\\xFF\\x01)");
}

void AFSK_deinit() {}

uint32_t loraRecoverCount = 0;

void AFSK_Poll(bool SA818, bool RFPower)
{
    if (loraRecoverReq)
    {
        // Radio recovery without reboot: re-run the same init sequence.
        loraRecoverReq = false;
        loraRecoverCount++;
        ax25_stateTx = false;
        fifo_flush(&txFifo);
        loraRadioUp = APRS_init(&config);
        log_e("[LoRa] radio re-init after watchdog (%lu): %s",
              (unsigned long)loraRecoverCount, loraRadioUp ? "OK" : "FAILED");
        return;
    }
    APRS_poll();
}
