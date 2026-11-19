# EE05 + 4.2-inch ePaper test (PlatformIO)

PlatformIO project for:

- Board: Seeed Studio XIAO ePaper Display Board EE05 (XIAO ESP32S3 Plus)
- Display: 4.2-inch monochrome ePaper, 400 x 300, SSD1683
- Buttons: Key1 = GPIO2, Key2 = GPIO3, Key3 = GPIO8 (active LOW)

![4.2 inch Epaper Testing](.images/4.2%20inch%20Epaper%20Testing.jpg)

## Important library choice

This project deliberately uses **Seeed_GFX** (the legacy `TFT_eSPI.h` / `EPaper` API), not Seeed_GFX2. The sketch uses the `EPaper` class, and Seeed's current EE0x PlatformIO instructions use the `Seeed_GFX` library with `driver.h`. Do not add `TFT_eSPI` or `Seeed_GFX2` as additional display libraries in this project.

## Project files

```text
EE05_4.2_PlatformIO/
├── platformio.ini
├── README.md
└── src/
    ├── driver.h
    └── main.cpp
```

## Build and upload

1. Install Visual Studio Code and the PlatformIO IDE extension.
2. Extract this project ZIP.
3. In VS Code, use **File -> Open Folder** and open the extracted `EE05_4.2_PlatformIO` folder (the folder containing `platformio.ini`).
4. Connect the EE05 using a USB data cable. Make sure the display's 24-pin FPC cable is fully inserted and locked.
5. In PlatformIO, select environment `ee05_4in2`.
6. Run **Build** first. PlatformIO downloads Seeed's board platform, the Arduino framework, and Seeed_GFX on the first build.
7. Select the board's upload port, then run **Upload**.
8. Open **Monitor** at 115200 baud.

The PlatformIO board ID `seeed-xiao-esp32-s3-sense` is the board ID used by Seeed's EE0x PlatformIO instructions; the project configuration includes the `qio_opi` memory setting and `BOARD_HAS_PSRAM` flag.

## Expected serial output

```text
=== XIAO ePaper Display Board EE05 - 4.2 inch test ===
PSRAM: ... bytes (OK)
Initializing ePaper...
Panel size: 400 x 300
Refreshing ePaper...
ePaper refresh command completed.
Initial screen draw completed. Press Key1 / Key2 / Key3.
```

Pressing each button increments its counter and triggers another full-screen refresh. ePaper full refreshes are slow; this is expected.

## Display configuration

`src/driver.h` selects combo `507` (4.2-inch monochrome SSD1683) and the EE05 driver-board define. These values must match the connected screen and board.

## Troubleshooting

- If compilation reports `EPAPER_ENABLE` is not defined, confirm that `src/driver.h` exists in the project and has not been moved. The `-I src` build flag is required so Seeed_GFX can find it.
- If the panel size is not 400 x 300, check the selected screen combo in `src/driver.h`.
- If the upload succeeds but the panel remains blank, power off before reseating the FPC cable, then check its orientation and locking tab.
- Keep only Seeed_GFX as the display library in this project; do not mix it with TFT_eSPI or Seeed_GFX2.

## References

- Seeed EE0x PlatformIO guide: https://wiki.seeedstudio.com/epaper_work_with_platformio/
- EE05 Getting Started: https://wiki.seeedstudio.com/epaper_ee05/
- Seeed GFX configuration generator: https://seeed-studio.github.io/Seeed_GFX/

### ESP32-S3 bootloader `--flash-mode` error

This project explicitly sets `board_build.flash_mode = qio` in `platformio.ini`.
This ensures the board flash-mode value is present when PlatformIO generates
the bootloader image. The Seeed board platform manages its own `tool-esptoolpy`
dependency; do not add a separate `platform_packages` override unless you are
intentionally testing a different esptool build.
