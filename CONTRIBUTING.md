# Contributing to DiFinders

Thank you for helping improve [DiFinders](https://github.com/dunknowcoding/DiFinders). The project is maintained by **[dunknowcoding](https://github.com/dunknowcoding)** for the **[NiusRobotLab](https://www.youtube.com/@NiusRobotLab)** community.

## Before you open a PR

1. Match existing patterns under `src/families/` (header-only, minimal dependencies).
2. Add an example folder named `SensorName_Feature` (e.g. `HC_SR04_BasicRead`, `VL53L1X_Interrupt`).
3. For new marketplace SKUs: add the alias in `src/DiFinders.h` and map examples in [`.github/scripts/alias_examples.yaml`](.github/scripts/alias_examples.yaml), then run `python .github/scripts/generate_alias_index.py`.
4. Use **Arduino `Serial` / `Wire` / `SPI`** in examples — do **not** use removed helpers: `DF_PROBE`, `DF_READ`, `df_code`, `difinders_*`, `df_serial`, `df_uart`.
5. Use **numeric literals** for baud and `delay` in examples (e.g. `9600`, `120`).
6. Run:
   ```bash
   python scripts/lint_examples.py
   python scripts/generate_alias_index.py
   arduino-cli compile --fqbn arduino:avr:uno --library . examples/Library_CompileSmoke
   ```

## Code style

- **English** comments and user-facing strings in library and examples.
- Sketches: no `using namespace`, no ternary-heavy logic, block comments at the top for wiring.
- Public aliases live in `src/DiFinders.h`; sketch helpers in `src/core/DiFindersHelpers.h` (`df_adc`, `df_hit` only).
- `*_BasicRead` / `*_MotionRead` / `*_PresenceRead` sketches should print canonical keys: `distance_mm=`, `motion=`, `detected=`, `status=`, etc.

## Maintainer scripts (`.github/scripts/`)

Shipped in the git repository for CI; **not** required in the Arduino Library ZIP. Local copies may also live in gitignored `scripts/` or `extras/`.

| Script | Purpose |
| --- | --- |
| `generate_alias_index.py` | Regenerate `docs/ALIAS_INDEX.md` from headers + YAML |
| `alias_examples.yaml` | Authoritative alias → example folder mapping |
| `lint_examples.py` | Forbidden symbols + English + output key checks |

The `extras/` and `drivers/` trees are gitignored (maintainer-only tooling and third-party stacks).

## Roadmap phases (L59–L158)

Detailed status: [**docs/ROADMAP.md**](docs/ROADMAP.md).

| Phase | Target | Focus |
| --- | --- | --- |
| **A** | v0.1.0 tag | Release hygiene, lint, legacy API grep clean |
| **B** | v0.1.1 | ALIAS Bus + Primary/Also see, `check_alias_index.py`, sensor alias footers, `sync_keywords.py` |
| **C** | v0.2.0 | mmWave appendix, FAQ, `ready()` matrix, no `DF_MEDIAN_MM` / `namespace df` |
| **D** | ongoing | CI matrix (UNO R4, ESP32 UART, high-rate, CAN), Issue/PR templates, 3.3 V decision tree |

Regenerate docs after alias changes:

```bash
python .github/scripts/generate_alias_index.py
python .github/scripts/append_sensor_aliases.py
python .github/scripts/sync_keywords.py
```

## Hardware photos

Run `extras/fetch_hardware_assets.py` only with URLs you have rights to use. Do not commit secrets or seller API keys.

## Questions

Open a [GitHub issue](https://github.com/dunknowcoding/DiFinders/issues) with board FQBN, module photo, and wiring — especially for new AliExpress SKUs.
