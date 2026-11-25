# AR-LORA v1.4-lu6jmf

Safe one-click updates, SOS emergency for the tracker, project logo and messaging improvements. Same base as AR-APRS 2.1.8.
Upgrading from 1.3 keeps your configuration.

## New
- **SOS emergency (tracker):** first item of the PRG menu, confirmed by holding PRG 2 s. Uses the APRS convention: Mic-E status "Emergency", emergency symbol `\!` and "EMERGENCY - SOS" in the comment, repeated every minute on RF and APRS-IS. Optional APRS message to a trusted callsign (TRACKER page, "SOS message to"). "Cancel SOS" sends "SOS cancelled - all OK" and returns to the normal beacon. The SOS stays active after a restart.
- **Safe update from the DashBoard:** the board checks for new versions once a day and shows a green notice "vX → vY". One button installs the file for your board (Heltec V3 or Wireless Tracker): the board restarts, downloads and checks the new version, and restarts again (about 3 minutes). Your configuration is kept. If anything fails, nothing changes and the board keeps running the current version.
- **Station logo:** the AR-APRS project mark is the default logo in the page header; click it to use your own image (resized automatically, kept after updates).
- **Header:** callsign, active modes and LoRa frequency on the right, next to [LOG OUT].
- **Reply button** for received messages (MSG page).
- **Browser tab icon** with the project mark.

## Fixes
- Message numbers no longer restart at 1 after a reboot (the other station could drop a new message as a repeat).
- INET→RF uses the standard third-party format (no APRS-IS path or q constructs on RF), and stations without SSID are listed and gated too.
- Manual Firmware Update shows the real reason when it fails.
- Long digipeater paths in LAST HEARD no longer stretch the page.
- About page: a section could be missing on low memory.

## Install
- **New board / full reset (USB):** https://marceloferreirachile.github.io/ESP32APRS_LoRa/ — choose the button for your board.
- **Existing board (keeps configuration):** About → Manual Firmware Update, or the "Update" button of the USB installer:
  - Heltec V3: `HELTEC_V3_v1.4-lu6jmf.bin`
  - Wireless Tracker: `HELTEC_TRACKER_v1.4-lu6jmf.bin` (built from the same code; tested on the Heltec V3)
- Web files did not change. Do not flash `littlefs.bin` on a configured board: it erases the configuration.

## Source code
GNU GPL v3, based on ESP32APRS by Somkiat Nakhonthai (HS5TQA). Source of this version: tag [v1.4-lu6jmf](https://github.com/marceloferreirachile/ESP32APRS_LoRa/tree/v1.4-lu6jmf).

73 de LU6JMF
