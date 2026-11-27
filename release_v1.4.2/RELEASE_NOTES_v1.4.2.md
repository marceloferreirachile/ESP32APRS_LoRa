# AR-LORA v1.4.2-lu6jmf

Fixes and a smaller firmware. Upgrading from 1.4.1 keeps your configuration.
Boards on 1.4.1 show the green update notice on the DashBoard: one click installs 1.4.2.

## Fixes
- **Manual update refuses a file made for another board** (e.g. the Wireless Tracker file on a Heltec V3).
- "Check version" (About) could restart the board (watchdog). Fixed.
- Pages are built with much less memory (a section of a page could disappear on low memory).
- VPN: stays reachable from the Internet (keepalive); the board link opens correctly.
- AT commands and PPP over serial removed. The firmware is about 155 KB smaller than 1.4.1.

## Install
- **Board on 1.4.1:** DashBoard → green notice → "Install update now".
- **New board / full reset (USB):** https://marceloferreirachile.github.io/ESP32APRS_LoRa/ — choose the button for your board.
- **Existing board (keeps configuration):** About → Manual Firmware Update, or the "Update" button of the USB installer:
  - Heltec V3: `HELTEC_V3_v1.4.2-lu6jmf.bin`
  - Wireless Tracker: `HELTEC_TRACKER_v1.4.2-lu6jmf.bin` (built from the same code; tested on the Heltec V3)
- Web files did not change. Do not flash `littlefs.bin` on a configured board: it erases the configuration.

## Source code
GNU GPL v3, based on ESP32APRS by Somkiat Nakhonthai (HS5TQA). Source of this version: tag [v1.4.2-lu6jmf](https://github.com/marceloferreirachile/ESP32APRS_LoRa/tree/v1.4.2-lu6jmf).

73 de LU6JMF
