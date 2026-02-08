/**
 * BLE Scanner — без фильтров: каждое объявление из эфира, вся информация.
 * ESP32-C3 Super Mini, стек NimBLE.
 */

#include <Arduino.h>
#include <NimBLEDevice.h>

#define SERIAL_BAUD 115200
#define SCAN_INTERVAL_MS 800   // дольше на канале = лучше ловим редкие объявления
#define SCAN_WINDOW_MS 800
#define MFR_HEX_MAX 64
#define NIMBLE_SCAN_RESTART_MS 25000

static uint32_t packetCount = 0;
static NimBLEScan* pScan = nullptr;
static uint32_t lastRestart = 0;

static void printHex(const uint8_t* data, size_t len, size_t maxBytes) {
  if (len == 0) return;
  if (len > maxBytes) len = maxBytes;
  for (size_t i = 0; i < len; i++) {
    if (data[i] < 16) Serial.print("0");
    Serial.print(data[i], HEX);
  }
  if (len < (size_t)maxBytes && len < 16) Serial.print(" ");
}

class ScanCallbacks : public NimBLEScanCallbacks {
  void onResult(const NimBLEAdvertisedDevice* d) override {
    packetCount++;
    Serial.printf("\n[%u] %s  ", packetCount, d->getAddress().toString().c_str());
    Serial.printf("type=%u  ", d->getAddressType());
    Serial.printf("%s  ", d->getName().length() ? d->getName().c_str() : "-");
    Serial.printf("RSSI %d  ", d->getRSSI());
    Serial.printf("%s  ", d->isConnectable() ? "conn" : "-");

    if (d->haveServiceUUID()) {
      Serial.print("UUID:");
      for (uint8_t i = 0; i < d->getServiceUUIDCount(); i++) {
        if (i) Serial.print(",");
        Serial.print(d->getServiceUUID(i).toString().c_str());
      }
      Serial.print("  ");
    }
    if (d->haveManufacturerData()) {
      std::string mfr = d->getManufacturerData();
      Serial.print("Mfr:");
      if (mfr.length() > 0)
        printHex((const uint8_t*)mfr.data(), mfr.length(), MFR_HEX_MAX);
      Serial.print("  ");
    }
    if (d->haveTXPower()) Serial.printf("TX%d  ", d->getTXPower());
#if CONFIG_BT_NIMBLE_EXT_ADV
    Serial.print(d->isLegacyAdvertisement() ? " (Legacy)" : " (Ext)");
#endif
  }
  void onScanEnd(const NimBLEScanResults&, int) override {}
};

static ScanCallbacks scanCb;

void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(1500);
  Serial.println("\n--- BLE Scanner: без фильтров, вся информация из эфира (NimBLE) ---");
  Serial.printf("  Интервал/окно скана %d ms. Фильтры по адресу/дубликатам отключены.\n", SCAN_INTERVAL_MS);
#if CONFIG_BT_NIMBLE_EXT_ADV
  Serial.println("  PHY: 1M + Coded (оба диапазона). Legacy и Extended объявления.");
#endif
  Serial.println("  [N]=счётчик  type/name/conn/UUID/Mfr/TX  RSSI\n");

  NimBLEDevice::init("");
  pScan = NimBLEDevice::getScan();
  pScan->setScanCallbacks(&scanCb, true);
  pScan->setActiveScan(true);
  pScan->setDuplicateFilter(0);
  pScan->setFilterPolicy(0);
  pScan->setLimitedOnly(false);
  pScan->setInterval(SCAN_INTERVAL_MS);
  pScan->setWindow(SCAN_WINDOW_MS);
  pScan->setMaxResults(0);
#if CONFIG_BT_NIMBLE_EXT_ADV
  pScan->setPhy(NimBLEScan::SCAN_ALL);
#endif
  pScan->start(0, false, true);
}

void loop() {
  delay(200);
  if (!pScan) return;
  if (!pScan->isScanning()) {
    pScan->start(0, false, true);
    lastRestart = millis();
    return;
  }
  if (millis() - lastRestart >= NIMBLE_SCAN_RESTART_MS) {
    pScan->stop();
    delay(50);
    pScan->clearResults();
    pScan->start(0, false, true);
    lastRestart = millis();
    Serial.println("\n--- перезапуск скана ---");
  }
}
