# DiFinders v0.1.0 — first public release

## Highlights

- **Arduino library** for distance, ToF, IR, mmWave, PIR, Modbus, and CAN modules sold under many marketplace aliases.
- **74 English examples** using `read()` + `Serial.print` and `SensorStatus` checks.
- **Documentation:** [User Guide](docs/USER_GUIDE.md), [API Index](docs/API_INDEX.md), [SKU alias index](docs/ALIAS_INDEX.md) (Bus + primary example columns), per-sensor pages under [docs/sensors/](docs/sensors/).
- **Multi-MCU CI:** AVR UNO, UNO R4, ESP32, RP2040 compile smoke; ESP32 UART samples; high-rate and CAN example jobs.

## Install

Arduino IDE: **Sketch → Include Library → Add .ZIP Library** → select the `DiFinders` folder, or clone into `Arduino/libraries/DiFinders`.

## Supported buses

GPIO pulse ultrasonic, UART (TFMini, A02, US-100, HLK-LD24xx), I2C/SPI ST ToF, RS485 Modbus, CAN TF03, analog Sharp IR, PIR, open-protocol mmWave.

## Not included in the library ZIP

Maintainer-only trees (`extras/`, `drivers/`, local `scripts/`) are gitignored. Regeneration tooling lives in `.github/scripts/` in the repository.

## Links

- [README](README.md) — install and quick start
- [API Index](docs/API_INDEX.md) — types, helpers, driver catalog
- [Issues](https://github.com/dunknowcoding/DiFinders/issues)
