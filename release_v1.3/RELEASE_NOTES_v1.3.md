# AR-LORA v1.3-lu6jmf

Official APRS device identifier and iGate improvements. Same rules as AR-APRS 2.1.7.
Upgrading from 1.2 keeps your configuration.

## New
- **Own TOCALL `APJMF2`:** AR-LORA is now registered in the APRS device identification list, so maps and tools show the station as AR-LORA.
- **iGate** follows the aprsc iGate implementation hints: packets go to APRS-IS unmodified (cut only at the end of the line), RF packets are not filtered as duplicates (APRS-IS does that) and nothing is sent late after a lost APRS-IS connection.
- **Query replies** (`?VER`, `?APRSP`...) are sent twice on RF, about 10 s apart, because a reply without a message number has no ACK.

## Fixes
- NEWS bulletins: the schedule preview now shows the real send times.
- NEWS intervals saved by older versions are converted to minutes.

## Install
- **New board / full reset (USB):** https://marceloferreirachile.github.io/ESP32APRS_LoRa/ — choose the button for your board.
- **Existing board (keeps configuration):** About → Manual Firmware Update, or the "Update" button of the USB installer:
  - Heltec V3: `HELTEC_V3_v1.3-lu6jmf.bin`
  - Wireless Tracker: `HELTEC_TRACKER_v1.3-lu6jmf.bin`
- Web files did not change. Do not flash `littlefs.bin` on a configured board: it erases the configuration.

## Source code
GNU GPL v3, based on ESP32APRS by Somkiat Nakhonthai (HS5TQA). Source of this version: tag [v1.3-lu6jmf](https://github.com/marceloferreirachile/ESP32APRS_LoRa/tree/v1.3-lu6jmf).

73 de LU6JMF
