# AR-LORA v1.4.1-lu6jmf

SOS for a tracker that is out of reach. Upgrading from 1.4 or 1.3 keeps your configuration.
Boards already on 1.4 show the green update notice on the DashBoard: one click installs 1.4.1.

## New
- **SOS without touching the board** (tracker hidden in a car, a box...):
  - **"SOS Emergency" button on the DashBoard** (Tracker enabled). While active, a red "SOS ACTIVE" bar with "Cancel SOS".
  - **APRS message** `SOS` (or `SOS ON`) to the station's message callsign activates it, `SOS OFF` (or `SOS CANCEL`) cancels it. Accepted only from your own callsign (any SSID) or from the SOS contact; the station replies to confirm.
- The PRG menu SOS works as in 1.4. Everything else is the same as 1.4 (see its notes).

## Install
- **Board on 1.4:** DashBoard → green notice → "Install update now".
- **New board / full reset (USB):** https://marceloferreirachile.github.io/ESP32APRS_LoRa/ — choose the button for your board.
- **Existing board (keeps configuration):** About → Manual Firmware Update, or the "Update" button of the USB installer:
  - Heltec V3: `HELTEC_V3_v1.4.1-lu6jmf.bin`
  - Wireless Tracker: `HELTEC_TRACKER_v1.4.1-lu6jmf.bin` (built from the same code; tested on the Heltec V3)
- Web files did not change. Do not flash `littlefs.bin` on a configured board: it erases the configuration.

## Source code
GNU GPL v3, based on ESP32APRS by Somkiat Nakhonthai (HS5TQA). Source of this version: tag [v1.4.1-lu6jmf](https://github.com/marceloferreirachile/ESP32APRS_LoRa/tree/v1.4.1-lu6jmf).

73 de LU6JMF
