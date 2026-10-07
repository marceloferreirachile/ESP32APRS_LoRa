// LU6JMF - Marcelo Ferreira: product identity, one place for both builds.
// Audio (AFSK) build = AR-APRS, LoRa build = AR-LORA. Same layout/rules,
// only the names, links and palette change.
#ifndef LU6JMF_BRAND_H
#define LU6JMF_BRAND_H

#ifdef RADIO_LORA
#define BRAND_SHORT "AR-LORA"
#define BRAND_LONG "Argentina LORA System"
#define BRAND_DEVICE_NAME "AR-LORA"                                  // default WiFi AP SSID / hostname / BT name
#define BRAND_PUBLIC_REPO "marceloferreirachile/ESP32APRS_LoRa"      // public repo: releases + installer (bins only)
#define BRAND_UPSTREAM_URL "https://github.com/nakhonthai/ESP32APRS_LoRa"
#define OPT_SIGNAL_LABEL "Signal Request (RSSI/SNR)"
#define BRAND_TXD_BG "#170b26"   // /txdebug page background
#define BRAND_TXD_CARD "#24133d" // /txdebug cards
#define BRAND_CSS "/* AR-LORA palette: violet / dark lavender (AR-APRS audio stays black) */.container{border-color:#2E1450}.header,.footer{background:#2E1450}h1{text-shadow:2px 2px #12051f}table{border-color:#4B2C7A}table th{background:#4B2C7A;text-shadow:1px 1px #1a0833}table td{border-color:#4B2C7A}.nav-tabs>button:hover{background-color:#B9A3E3;border-color:#8E6CC9;color:#2E1450}.nav-tabs>button.active,.nav-tabs>button.active:hover,.nav-tabs>button.active:focus{background-color:#7B4FC4;color:#fff}.button{border-color:#5B3A99;background-color:#7B4FC4}.button:hover{border-color:#7B4FC4;background-color:#9A72DB}"
#else
#define BRAND_SHORT "AR-APRS"
#define BRAND_LONG "Argentina APRS System"
#define BRAND_DEVICE_NAME "ESP32APRS_Audio"
#define BRAND_PUBLIC_REPO "marceloferreirachile/ESP32APRS_Audio"
#define BRAND_UPSTREAM_URL "https://github.com/nakhonthai/ESP32APRS_Audio"
#define BRAND_CSS ""
#define OPT_SIGNAL_LABEL "Audio Request"
#define BRAND_TXD_BG "#111"
#define BRAND_TXD_CARD "#151515"
#endif

#define BRAND_REPO_URL "https://github.com/" BRAND_PUBLIC_REPO
#endif
