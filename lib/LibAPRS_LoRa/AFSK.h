// LU6JMF - LoRa build: compatibility header.
// The 2.1.5 core includes "AFSK.h" (audio modem). On LoRa these entry points are
// implemented by lora_compat.cpp on top of the LoRa radio (see that file).
#ifndef LU6JMF_LORA_AFSK_COMPAT_H
#define LU6JMF_LORA_AFSK_COMPAT_H
#include <Arduino.h>
#include "AX25.h"

#ifndef LED_RX_PIN
#define LED_RX_PIN -1
#endif
#ifndef LED_TX_PIN
#define LED_TX_PIN -1
#endif

void AFSK_init(int8_t adc_pin, int8_t dac_pin, int8_t ptt_pin, int8_t sql_pin, int8_t pwr_pin, int8_t led_tx_pin, int8_t led_rx_pin, int8_t led_strip_pin, bool ptt_act, bool sql_act, bool pwr_act);
void AFSK_deinit();
void AFSK_Poll(bool SA818, bool RFPower);
void AFSK_TimerEnable(bool sts);
void DAC_TimerEnable(bool sts);
void afskSetHPF(bool val);
void afskSetBPF(bool val);
void afskSetADCAtten(uint8_t val);
void afskSetPTT(int8_t val, bool act);
void afskSetPWR(int8_t val, bool act);
void afskSetSQL(int8_t val, bool act);
bool getTransmit();
bool getReceive();
void afskSetModem(uint8_t val, bool bpf, uint16_t timeSlot, uint16_t preamble, uint8_t fx25Mode);
void setPtt(bool state);
void afskSetTxGain(uint8_t gainPercent);
void afskSetTxPreemphasis(bool enable, uint8_t tiltPercent);
bool AFSK_StartTxCalibration(uint8_t toneType);
bool AFSK_IsTxCalibrationActive(void);
bool AFSK_DacTimerRunning(void);
void AFSK_RequestRxSoftReset(void);

// AX.25 ring API of the audio library
bool Ax25NewRxFrames(void);
bool Ax25ReadNextRxFrame(uint8_t **dst, uint16_t *size, int8_t *peak, int8_t *valley, uint8_t *level, uint8_t *corrected, uint16_t *mV);
void Ax25TransmitBuffer(void);
void Ax25TransmitCheck(void);
void Ax25GetTxDiag(uint8_t *head, uint8_t *tail, bool *bufferFull, uint8_t *initStage);
void Ax25TxAbort(bool keyed);

// LoRa listen-before-talk: transmit() was not attempted because the channel stayed busy
// (CAD) for 10 s. The core puts the frame back into the TX queue (main.cpp).
#define LORA_TX_CHANNEL_BUSY (-32000)
void loraRememberFrame(const uint8_t *raw, size_t len);
const char *loraLastFrame(size_t *len);
uint32_t loraGetTxBusy();
void loraSetOnAir(bool on);

// LoRa-only diagnostics (lora_compat.cpp)
int16_t loraGetLastTxState();
uint32_t loraGetTxOk();
uint32_t loraGetTxErr();
uint32_t loraGetRxDropped();
extern uint32_t loraRecoverCount;
#endif
