#!/usr/bin/env python3
"""
Build the web-installer files (ESP Web Tools) for the LoRa firmware.

Usage (from the project root, after building):
    pio run -e heltec_V3_GPS
    pio run -e heltec_V3_GPS -t buildfs        # only if you want a fresh LittleFS image
    python3 tools/make_installer.py heltec_V3_GPS 0.9b-lu6jmf

It copies the binaries into docs/bin and writes two manifests:
  docs/manifest.json         fresh install  (bootloader + partitions + app + littlefs, erase offered)
  docs/manifest-update.json  update only    (app only, NO erase, NO littlefs -> configuration is kept,
                                              because the configuration lives in LittleFS)
Flash offsets are read from the real partitions.bin of the build, never hard-coded.
"""
import json
import os
import shutil
import struct
import sys

CHIP = {"esp32s3": "ESP32-S3", "esp32": "ESP32", "esp32c3": "ESP32-C3"}


def read_partitions(path):
    """Parse partitions.bin -> {label: (type, subtype, offset, size)}."""
    parts = {}
    with open(path, "rb") as f:
        data = f.read()
    for i in range(0, len(data) - 32, 32):
        magic, ptype, subtype, offset, size = struct.unpack("<HBBII", data[i:i + 12])
        if magic != 0x50AA:  # entries are stored little-endian: bytes AA 50
            if data[i:i + 2] == b"\xff\xff":
                break
            continue
        label = data[i + 12:i + 28].split(b"\0")[0].decode("ascii", "ignore")
        parts[label] = (ptype, subtype, offset, size)
    return parts


def main():
    if len(sys.argv) < 3:
        print(__doc__)
        sys.exit(1)
    env_name, version = sys.argv[1], sys.argv[2]
    chip = CHIP.get(sys.argv[3] if len(sys.argv) > 3 else "esp32s3", "ESP32-S3")
    root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    build = os.path.join(root, ".pio", "build", env_name)
    out = os.path.join(root, "docs", "bin")
    os.makedirs(out, exist_ok=True)

    need = ["bootloader.bin", "partitions.bin", "firmware.bin"]
    for n in need:
        if not os.path.exists(os.path.join(build, n)):
            sys.exit("missing %s - run: pio run -e %s" % (os.path.join(build, n), env_name))
        shutil.copy2(os.path.join(build, n), os.path.join(out, n))

    parts = read_partitions(os.path.join(build, "partitions.bin"))
    app0 = next((v for k, v in parts.items() if v[0] == 0 and v[1] == 0x10), None)
    fs = next((v for k, v in parts.items() if v[0] == 1 and v[1] == 0x82), None)
    if not app0:
        sys.exit("no ota_0 app partition found in partitions.bin")
    print("partitions:", {k: (hex(v[2]), hex(v[3])) for k, v in parts.items()})

    boot_off = 0 if chip in ("ESP32-S3", "ESP32-C3") else 0x1000
    fresh = [
        {"path": "./bin/bootloader.bin", "offset": boot_off},
        {"path": "./bin/partitions.bin", "offset": 0x8000},
        {"path": "./bin/firmware.bin", "offset": app0[2]},
    ]
    have_fs = os.path.exists(os.path.join(build, "littlefs.bin"))
    if have_fs and fs:
        shutil.copy2(os.path.join(build, "littlefs.bin"), os.path.join(out, "littlefs.bin"))
        fresh.append({"path": "./bin/littlefs.bin", "offset": fs[2]})
    elif os.path.exists(os.path.join(out, "littlefs.bin")) and fs:
        fresh.append({"path": "./bin/littlefs.bin", "offset": fs[2]})
    else:
        print("WARNING: no littlefs.bin; fresh install will boot with an empty filesystem")

    name = "ESP32APRS LoRa (LU6JMF) %s" % version
    with open(os.path.join(root, "docs", "manifest.json"), "w") as f:
        json.dump({"name": name, "version": version, "new_install_prompt_erase": True,
                   "builds": [{"chipFamily": chip, "parts": fresh}]}, f, indent=2)
    with open(os.path.join(root, "docs", "manifest-update.json"), "w") as f:
        json.dump({"name": name + " (update, keeps configuration)", "version": version,
                   "new_install_prompt_erase": False,
                   "builds": [{"chipFamily": chip,
                               "parts": [{"path": "./bin/firmware.bin", "offset": app0[2]}]}]}, f, indent=2)
    print("OK: docs/manifest.json + docs/manifest-update.json written for", name)


if __name__ == "__main__":
    main()
