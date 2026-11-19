#include <Arduino.h>
#include "TFT_eSPI.h"  // Header supplied by Seeed_GFX

#ifndef EPAPER_ENABLE
  #error "EPAPER_ENABLE is not defined. Check src/driver.h and the Seeed_GFX library configuration."
#endif

EPaper epaper;

// EE05 user buttons: external 10k pull-ups; pressed = LOW.
static constexpr uint8_t KEY1_PIN = 2;  // D1 / GPIO2
static constexpr uint8_t KEY2_PIN = 3;  // D2 / GPIO3
static constexpr uint8_t KEY3_PIN = 8;  // D9 / GPIO8

static const uint8_t keyPins[3] = {KEY1_PIN, KEY2_PIN, KEY3_PIN};
static uint32_t pressCount[3] = {0, 0, 0};
static int lastKey = 0;

static void drawScreen() {
  const int width = epaper.width();
  const int height = epaper.height();

  epaper.fillScreen(TFT_WHITE);
  epaper.setTextColor(TFT_BLACK, TFT_WHITE);

  // Full-panel border and corner markers.
  epaper.drawRect(0, 0, width, height, TFT_BLACK);
  epaper.fillRect(0, 0, 8, 8, TFT_BLACK);
  epaper.fillRect(width - 8, 0, 8, 8, TFT_BLACK);
  epaper.fillRect(0, height - 8, 8, 8, TFT_BLACK);
  epaper.fillRect(width - 8, height - 8, 8, 8, TFT_BLACK);

  // Shape test.
  epaper.fillCircle(width - 26, 24, 10, TFT_BLACK);

  // Text test.
  epaper.drawString("EE05 OK", 14, 14, 4);

  char line[64];
  snprintf(line, sizeof(line), "Panel: %d x %d", width, height);
  epaper.drawString(line, 14, 46, 2);

  if (lastKey == 0) {
    epaper.drawString("Press Key1/Key2/Key3", 14, 64, 2);
  } else {
    snprintf(line, sizeof(line), "Last: Key%d", lastKey);
    epaper.drawString(line, 14, 64, 2);
  }

  snprintf(line, sizeof(line), "Count: %lu/%lu/%lu",
           static_cast<unsigned long>(pressCount[0]),
           static_cast<unsigned long>(pressCount[1]),
           static_cast<unsigned long>(pressCount[2]));
  epaper.drawString(line, 14, 82, 2);

  Serial.println("Refreshing ePaper...");
  epaper.update();
  Serial.println("ePaper refresh command completed.");
}

void setup() {
  Serial.begin(115200);

  const uint32_t serialStart = millis();
  while (!Serial && (millis() - serialStart < 2000)) {
    delay(10);
  }

  Serial.println();
  Serial.println("=== XIAO ePaper Display Board EE05 - 4.2 inch test ===");

  const size_t psramBytes = ESP.getPsramSize();
  Serial.printf("PSRAM: %u bytes %s\n",
                static_cast<unsigned>(psramBytes),
                psramBytes ? "(OK)" : "(NOT DETECTED)");

  for (uint8_t i = 0; i < 3; ++i) {
    pinMode(keyPins[i], INPUT_PULLUP);
  }

  Serial.println("Initializing ePaper...");
  epaper.begin();
  Serial.printf("Panel size: %d x %d\n", epaper.width(), epaper.height());

  drawScreen();
  Serial.println("Initial screen draw completed. Press Key1 / Key2 / Key3.");
}

void loop() {
  for (uint8_t i = 0; i < 3; ++i) {
    if (digitalRead(keyPins[i]) != LOW) {
      continue;
    }

    delay(30);  // Debounce.
    if (digitalRead(keyPins[i]) != LOW) {
      continue;
    }

    ++pressCount[i];
    lastKey = static_cast<int>(i) + 1;

    Serial.printf("Key%d (GPIO%u) pressed, count=%lu\n",
                  i + 1,
                  keyPins[i],
                  static_cast<unsigned long>(pressCount[i]));

    drawScreen();

    // Wait for release so one hold registers as one press.
    while (digitalRead(keyPins[i]) == LOW) {
      delay(10);
    }
    delay(30);
  }
}
