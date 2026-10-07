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
#else
#define BRAND_SHORT "AR-APRS"
#define BRAND_LONG "Argentina APRS System"
#define BRAND_DEVICE_NAME "ESP32APRS_Audio"
#define BRAND_PUBLIC_REPO "marceloferreirachile/ESP32APRS_Audio"
#define BRAND_UPSTREAM_URL "https://github.com/nakhonthai/ESP32APRS_Audio"
#endif

#define BRAND_REPO_URL "https://github.com/" BRAND_PUBLIC_REPO
#endif
