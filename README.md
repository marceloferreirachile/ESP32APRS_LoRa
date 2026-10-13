# AR-LORA · Argentina LORA System

LoRa APRS firmware (iGate, digipeater, tracker) for the **Heltec WiFi LoRa 32 V3** and the **Heltec Wireless Tracker V1.1** (433 MHz), by LU6JMF (Marcelo, Concepción del Uruguay, Entre Ríos, Argentina).

It is built on **ESP32APRS_LoRa** by nakhonthai and carries the core of **AR-APRS** (the LU6JMF audio APRS firmware):
- TX queue with priorities and scheduler;
- listen-before-talk (CAD) with automatic retry;
- watchdog;
- `/txdebug` diagnostics with a "black box" of the last restart;
- web pages with the AR-LORA identity.

**LoRa APRS standard:** 433.775 MHz, SF12, BW 125 kHz, CR 4/5, sync word 0x12, TNC2 frames (`<\xFF\x01` header).
- **RX:** accepts both the TNC2 and the AX.25 LoRa formats.

## Install

**Current version: v1.1-lu6jmf**, which adds the Wireless Tracker, Bluetooth (BLE KISS TNC) and a button menu.

**New board / full reset (USB):** use the one-click browser installer: **https://marceloferreirachile.github.io/ESP32APRS_LoRa/**

**First access:**
1. Connect to the WiFi network `AR-LORA_AP` (no password).
2. Open `http://192.168.4.1` and log in with `admin` / `admin`.
3. Set your callsign, WiFi and position.

The board starts with callsign NOCALL and every mode off, so it does not transmit until configured.

**Existing board, no USB:** About tab → Manual Firmware Update → upload the file for your board (`HELTEC_V3_v1.1-lu6jmf.bin` or `HELTEC_TRACKER_v1.1-lu6jmf.bin`) from the [releases](https://github.com/marceloferreirachile/ESP32APRS_LoRa/releases). Your configuration is preserved. When a release changes the web files, upload them through the File tab. Never flash `littlefs.bin` on a configured board: it erases the configuration.

## License / source code

This firmware is based on ESP32APRS by Somkiat Nakhonthai (HS5TQA) and is distributed under the **GNU General Public License v3** (see [LICENSE](LICENSE)).

The source code is not published here. The complete corresponding source code of any released version is available on request, free of charge, for at least 3 years from its release date: **marcelobrachile@gmail.com** (please include the version number, e.g. v1.0-lu6jmf).

73 de LU6JMF
