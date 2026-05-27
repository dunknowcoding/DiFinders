# DiFinders roadmap (L59–L158)

Phases after the **v0.1.0** bring-up. Status as of the Phase B–D implementation pass.

## Phase A — v0.1.0 release gate

| Layer | Item | Status |
| --- | --- | --- |
| L59–61 | `library.properties` 0.1.0, README first-release notes, `.gitignore` excludes `extras/`/`drivers/` | Done |
| L62 | MIT `LICENSE` | Done |
| L63–64 | `CONTRIBUTING.md`, Issue templates, PR template | Done |
| L65–66 | Release notes from CHANGELOG; `scripts/` in repo | Maintainer action |
| L86–87 | `lint_examples.py`, canonical Serial keys on BasicRead | Done |
| L105 | `check_docs_english.py` in CI | Done |

## Phase B — v0.1.1 docs pipeline

| Layer | Item | Status |
| --- | --- | --- |
| L69 | `alias_examples.yaml` authoritative | Done |
| L70 | `using` chain → family in generator | Done |
| L71 | ALIAS **Bus** column | Done |
| L72 | **Primary example** / **Also see** columns | Done |
| L73 | API_INDEX representative examples | Done (`update_api_index_examples.py`) |
| L74 | `GY_US42_UART` → `A02YYUWSensor` | Done |
| L75 | Sensor pages **Aliases** + examples | Done (`append_sensor_aliases.py`) |
| L76 | CI `check_alias_index.py` | Done |
| L77 | Sensor doc links exist | Manual / spot-check |
| L78 | `sync_keywords.py` | Done |

## Phase C — v0.2.0 examples & API

| Layer | Item | Status |
| --- | --- | --- |
| L79 | Every family has `*_BasicRead` or cross-ref | Partial (LD2450 uses LD2461 example) |
| L80–83 | Thin examples, mmWave table, header template | mmWave appendix + FAQ done; full header pass optional |
| L89–93 | `ready()` matrix, Modbus/CAN docs | USER_GUIDE §2.3 + §7 updated |
| L96 | No `DF_MEDIAN_MM` in examples | Done |
| L97 | No `namespace df` in examples | Done |

## Phase D — CI & long-term

| Layer | Item | Status |
| --- | --- | --- |
| L109–112 | UNO/UNO R4/ESP32 smoke; high-rate; CAN jobs | Done in `.github/workflows/arduino-ci.yml` |
| L118 | Third-party compile optional | `continue-on-error: true` |
| L119–128 | Install path, FAQ, 3.3 V tree | README + USER_GUIDE §4.1 + Appendix B |
| L137 | New SKU = `DiFinders.h` + YAML | CONTRIBUTING |
| L158 | Legacy grep zero + ALIAS correct + tag | Grep clean; tag `v0.1.0` pending release |

## Maintainer commands

```bash
python scripts/generate_alias_index.py
python scripts/append_sensor_aliases.py
python scripts/update_api_index_examples.py
python scripts/sync_keywords.py
python scripts/lint_examples.py
python scripts/check_alias_index.py
```
