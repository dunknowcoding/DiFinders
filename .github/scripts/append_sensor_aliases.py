#!/usr/bin/env python3
"""Update docs/sensors/*.md Overview with aliases and representative examples from YAML."""

from __future__ import annotations

import re
from collections import defaultdict
from pathlib import Path

try:
    import yaml
except ImportError:
    yaml = None  # type: ignore

ROOT = Path(__file__).resolve().parents[2]
HEADER = ROOT / "src" / "DiFinders.h"
YAML_PATH = ROOT / "scripts" / "alias_examples.yaml"
BUS_PATH = ROOT / "scripts" / "family_bus.yaml"
SENSORS_DIR = ROOT / "docs" / "sensors"

# Reuse generator logic
import sys

sys.path.insert(0, str(ROOT / "scripts"))
from generate_alias_index import (  # noqa: E402
    HEADER,
    load_family_bus,
    load_yaml_overrides,
    parse_aliases,
    list_example_folders,
    match_examples_heuristic,
    resolve_family_class,
    collect_alias_names,
    SKIP_ALIASES,
)


def bus_label_for_family(family: str, family_bus: dict[str, str]) -> str:
    bus = family_bus.get(family, "Unknown")
    labels = {
        "GPIO": "GPIO pulse or digital OUT",
        "UART": "UART",
        "I2C": "I2C",
        "SPI": "SPI",
        "RS485": "RS485 Modbus",
        "CAN": "CAN",
        "Analog": "Analog",
    }
    return labels.get(bus, bus)


def build_family_metadata() -> tuple[dict[str, list[str]], dict[str, list[str]], dict[str, str]]:
    text = HEADER.read_text(encoding="utf-8", errors="replace")
    class_map, using_map = parse_aliases(text)
    overrides = load_yaml_overrides()
    family_bus = load_family_bus()
    folders = list_example_folders()

    aliases_by_family: dict[str, list[str]] = defaultdict(list)
    examples_by_family: dict[str, list[str]] = defaultdict(list)
    bus_by_family: dict[str, str] = dict(family_bus)

    all_names = set(collect_alias_names(class_map, using_map)) | set(overrides.keys())
    for alias in sorted(all_names):
        if alias in SKIP_ALIASES:
            continue
        family = resolve_family_class(alias, class_map, using_map)
        ex: list[str] = []
        if alias in overrides and isinstance(overrides[alias], dict):
            ov = overrides[alias]
            if "family_class" in ov:
                family = ov["family_class"]
            if "bus" in ov:
                bus_by_family[family] = ov["bus"]
            ex = list(ov.get("examples") or [])
        if not ex:
            ex = match_examples_heuristic(alias, folders)
        aliases_by_family[family].append(alias)
        for folder in ex:
            if folder not in examples_by_family[family]:
                examples_by_family[family].append(folder)

    return aliases_by_family, examples_by_family, bus_by_family


def patch_overview(text: str, class_name: str, aliases: list[str], examples: list[str], bus_line: str) -> str:
    alias_str = ", ".join(f"`{a}`" for a in sorted(set(aliases))[:24])
    if len(aliases) > 24:
        alias_str += ", â€?
    if not alias_str:
        alias_str = "â€?
    ex_str = ", ".join(f"`{e}`" for e in examples[:6]) if examples else "â€?
    if len(examples) > 6:
        ex_str += ", â€?

    overview_block = (
        "## Overview\n\n"
        f"- **Bus:** {bus_line}\n"
        f"- **Sketch aliases:** {alias_str}\n"
        f"- **Representative examples:** {ex_str}\n"
    )

    if "## Overview" in text:
        text = re.sub(r"## Overview\n.*?(?=\n## |\Z)", overview_block + "\n", text, count=1, flags=re.DOTALL)
    else:
        # Insert after header block
        m = re.search(r"(# `[^`]+`\n\nHeader:[^\n]+\n\n)", text)
        if m:
            text = text[: m.end()] + overview_block + "\n" + text[m.end() :]
    return text


def main() -> None:
    aliases_by_family, examples_by_family, bus_by_family = build_family_metadata()
    family_bus = load_family_bus()
    updated = 0
    for path in sorted(SENSORS_DIR.glob("*.md")):
        class_name = path.stem
        aliases = aliases_by_family.get(class_name, [])
        examples = examples_by_family.get(class_name, [])
        bus = bus_by_family.get(class_name) or family_bus.get(class_name, "Unknown")
        bus_line = bus_label_for_family(class_name, {class_name: bus, **family_bus})
        new_text = patch_overview(path.read_text(encoding="utf-8"), class_name, aliases, examples, bus_line)
        if new_text != path.read_text(encoding="utf-8"):
            path.write_text(new_text, encoding="utf-8")
            print("updated", path.name)
            updated += 1
    print("done:", updated, "sensor page(s)")


if __name__ == "__main__":
    main()
