## Summary

## Test plan

- [ ] `python scripts/lint_examples.py`
- [ ] `python scripts/generate_alias_index.py` (commit `docs/ALIAS_INDEX.md` if aliases changed)
- [ ] `python scripts/append_sensor_aliases.py` (if sensor docs should refresh)
- [ ] `arduino-cli compile --fqbn arduino:avr:uno --library . examples/Library_CompileSmoke`

## New SKU checklist (if applicable)

- [ ] Alias in `src/DiFinders.h`
- [ ] Entry in `scripts/alias_examples.yaml`
- [ ] Example folder or cross-reference to existing `*_BasicRead`
