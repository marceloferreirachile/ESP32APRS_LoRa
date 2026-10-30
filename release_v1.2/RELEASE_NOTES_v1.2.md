# AR-LORA v1.2-lu6jmf

APRS 1.2 behaviour, more APRS queries, our own VPN server and stability fixes. Same rules as AR-APRS 2.1.6.
Upgrading from 1.1 keeps your configuration.

## New
- **APRS 1.2:** the digipeater inserts its own callsign on WIDEn-N, ignores repeated copies for 30 s and never repeats its own packets. Query replies follow the spec (no message number); reply-ack supported; general queries `?APRS?` and `?IGATE?` heard on RF are answered.
- **APRS queries** (message to the station): `?APRS` (list), `?VER`, `?APRSP` (position now), `?APRSS` (status), `?APRST` / `?PING?` (path), `?APRSD` (stations heard direct), `?APRSO` (objects again), `?IGATE`, `?DX`.
- **Messages without a message number** are shown in the chat and answered.
- **VPN:** new server `vpn.tracktotal.ar`. The private key is generated on the board and never leaves it. Saving the VPN page restarts the board.
- **Header** shows the Host Name, enabled modes and the LoRa frequency.

## Fixes
- VPN restart loop on the ESP32-S3.
- Reboot with I2C sensors after a WiFi reconnect.
- A digipeated packet could cancel another packet waiting in the TX queue.
- A message repeated by the sender gets the ACK again but is not answered twice.

## Install
- **New board / full reset (USB):** https://marceloferreirachile.github.io/ESP32APRS_LoRa/ — choose the button for your board.
- **Existing board (keeps configuration):** About → Manual Firmware Update:
  - Heltec V3: `HELTEC_V3_v1.2-lu6jmf.bin`
  - Wireless Tracker: `HELTEC_TRACKER_v1.2-lu6jmf.bin`
- Do not flash `littlefs.bin` on a configured board: it erases the configuration.

## Source code
GNU GPL v3, based on ESP32APRS by Somkiat Nakhonthai (HS5TQA). Source of this version: tag [v1.2-lu6jmf](https://github.com/marceloferreirachile/ESP32APRS_LoRa/tree/v1.2-lu6jmf).

73 de LU6JMF
