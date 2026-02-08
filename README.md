# BLE Scanner (ESP32-C3)

Сканер BLE-устройств для **ESP32-C3 Super Mini**: выводит в Serial все объявления из эфира без фильтров (адрес, имя, RSSI, UUID, manufacturer data, тип адреса, Legacy/Extended). Подходит для отладки и анализа BLE-трафика.

- **Плата:** ESP32-C3 Super Mini (в PlatformIO: `esp32-c3-devkitm-1`)  
- **Стек:** NimBLE (Extended Scan, 1M + Coded PHY)  
- **Лицензия:** [MIT](LICENSE)

## Требования

- [PlatformIO](https://platformio.org/) (IDE или CLI)
- Плата ESP32-C3 Super Mini (или совместимая ESP32-C3)

## Сборка и загрузка

```bash
# Клонировать репозиторий (подставьте свой логин вместо YOUR_USERNAME)
git clone https://github.com/BLE-SCAN/BLE-SCAN.git
cd BLE-SCAN

# Сборка
pio run

# Загрузка прошивки (порт по умолчанию COM6, см. platformio.ini)
pio run -t upload

# Монитор (115200)
pio device monitor
```

В `platformio.ini` при необходимости укажите свой COM-порт. Вывод в Serial по **USB CDC** (115200): после прошивки откройте монитор на том COM-порте, который появляется при подключении платы по USB.

## Поведение

- Непрерывное сканирование без фильтра по типу адреса (PUBLIC/RANDOM) и без whitelist.
- Extended Scan: видны Legacy и Extended объявления (BLE 4.x и 5).
- Раз в 25 секунд скан перезапускается для «освежения» приёма.
- В Serial выводится каждая запись: `[N] адрес type=… имя RSSI conn UUID Mfr TX (Legacy)/(Ext)`.

## Структура проекта

```
BLE-SCAN/
├── src/
│   └── main.cpp      # Весь код сканера
├── platformio.ini    # Окружение ESP32-C3 + NimBLE
├── PLAN.md           # План и заметки (Legacy/Extended, PHY, тип адреса)
├── LICENSE           # MIT
└── README.md
```
для связи с автором https://t.me/+Q2HRrdfc-hpjM2Ri

## Лицензия

MIT License — см. [LICENSE](LICENSE).
