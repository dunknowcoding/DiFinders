#!/usr/bin/env python3
"""Generate docs/ALIAS_INDEX.md from DiFinders.h + scripts/alias_examples.yaml."""

from __future__ import annotations

import re
from pathlib import Path

try:
    import yaml
except ImportError:
    yaml = None  # type: ignore

ROOT = Path(__file__).resolve().parents[2]
HEADER = ROOT / "src" / "DiFinders.h"
YAML_PATH = Path(__file__).resolve().parent / "alias_examples.yaml"
BUS_PATH = Path(__file__).resolve().parent / "family_bus.yaml"
OUT = ROOT / "docs" / "ALIAS_INDEX.md"
EXAMPLES = ROOT / "examples"
SENSORS_DIR = ROOT / "docs" / "sensors"

SKIP_ALIASES = {
    "Ultrasonic",
    "Reflective",
    "Motion",
    "Presence",
    "MmwaveOpenProtocol",
    "ToF",
    "UltrasonicRadarRangingSensor",
}


def load_yaml_overrides() -> dict:
    if yaml is None or not YAML_PATH.exists():
        return {}
    data = yaml.safe_load(YAML_PATH.read_text(encoding="utf-8")) or {}
    return data.get("overrides", {})


def load_family_bus() -> dict[str, str]:
    if yaml is None or not BUS_PATH.exists():
        return {}
    data = yaml.safe_load(BUS_PATH.read_text(encoding="utf-8")) or {}
    return dict(data.get("families") or {})


def parse_aliases(text: str) -> tuple[dict[str, str], dict[str, str]]:
    class_map: dict[str, str] = {}
    using_map: dict[str, str] = {}
    for m in re.finditer(r"class\s+(\w+)\s*:\s*public\s+(\w+)", text):
        class_map[m.group(1)] = m.group(2)
    for m in re.finditer(r"using\s+(\w+)\s*=\s*(\w+)\s*;", text):
        using_map[m.group(1)] = m.group(2)
    return class_map, using_map


def resolve_family_class(
    name: str, class_map: dict[str, str], using_map: dict[str, str], seen: set | None = None
) -> str:
    if seen is None:
        seen = set()
    if name in seen:
        return name
    seen.add(name)
    if name in class_map:
        base = class_map[name]
        if base.endswith("Sensor"):
            return base
        return resolve_family_class(base, class_map, using_map, seen)
    if name in using_map:
        return resolve_family_class(using_map[name], class_map, using_map, seen)
    if name.endswith("Sensor"):
        return name
    return name


def list_example_folders() -> list[str]:
    """Only git-tracked example folders (ignores local untracked dirs)."""
    import subprocess

    try:
        out = subprocess.check_output(
            ["git", "ls-files", "examples"],
            cwd=ROOT,
            text=True,
            stderr=subprocess.DEVNULL,
        )
    except (subprocess.CalledProcessError, FileNotFoundError):
        return sorted(d.name for d in EXAMPLES.iterdir() if d.is_dir())

    folders: set[str] = set()
    for line in out.splitlines():
        parts = Path(line).parts
        if len(parts) >= 2 and parts[0] == "examples":
            folders.add(parts[1])
    return sorted(folders)


def match_examples_heuristic(alias: str, folders: list[str]) -> list[str]:
    alias_u = alias.upper()
    candidates: list[tuple[int, str]] = []
    for folder in folders:
        fu = folder.upper()
        if fu == alias_u or fu.startswith(alias_u + "_") or alias_u in fu:
            score = len(folder) if fu.startswith(alias_u) else len(alias)
            candidates.append((score, folder))
    if not candidates:
        return []
    candidates.sort(key=lambda x: (-x[0], x[1]))
    return [candidates[0][1]]


def format_one(example: str) -> str:
    return f"`{example}`"


def collect_alias_names(class_map: dict[str, str], using_map: dict[str, str]) -> list[str]:
    names = set(class_map.keys()) | set(using_map.keys())
    names -= SKIP_ALIASES
    return sorted(names)


def build_rows() -> list[tuple[str, str, str, str, str, str]]:
    text = HEADER.read_text(encoding="utf-8", errors="replace")
    class_map, using_map = parse_aliases(text)
    overrides = load_yaml_overrides()
    family_bus = load_family_bus()
    folders = list_example_folders()
    rows: list[tuple[str, str, str, str, str, str]] = []

    all_names = set(collect_alias_names(class_map, using_map)) | set(overrides.keys())
    for alias in sorted(all_names):
        family = resolve_family_class(alias, class_map, using_map)
        doc_name = family if family.endswith("Sensor") else alias
        if not (SENSORS_DIR / f"{doc_name}.md").exists():
            doc_name = family
        family_link = f"[`{doc_name}`](sensors/{doc_name}.md)"

        ex: list[str] = []
        bus = family_bus.get(family, "-")
        if alias in overrides:
            ov = overrides[alias]
            if isinstance(ov, dict):
                if "family_class" in ov:
                    fn = ov["family_class"]
                    family = fn
                    family_link = f"[`{fn}`](sensors/{fn}.md)"
                    bus = family_bus.get(fn, bus)
                if "bus" in ov:
                    bus = ov["bus"]
                ex = list(ov.get("examples") or [])
        if not ex:
            ex = match_examples_heuristic(alias, folders)

        primary = format_one(ex[0]) if ex else "?"
        also = ", ".join(format_one(e) for e in ex[1:]) if len(ex) > 1 else "?"
        rows.append((alias, bus, family_link, primary, also))
    return rows


def render_markdown(rows: list[tuple[str, str, str, str, str, str]]) -> str:
    lines = [
        "# DiFinders SKU alias index",
        "",
        "Marketplace names (sketch types) map to family drivers. Include `#include <DiFinders.h>` only.",
        "",
        "Example folders are authoritative when listed in "
        "[`.github/scripts/alias_examples.yaml`](../.github/scripts/alias_examples.yaml); "
        "otherwise the generator picks the best name match under `examples/`.",
        "",
        "| Alias (sketch type) | Bus | Family driver | Primary example | Also see |",
        "| --- | --- | --- | --- | --- |",
    ]
    for alias, bus, link, primary, also in rows:
        lines.append(f"| `{alias}` | {bus} | {link} | {primary} | {also} |")
    lines.extend(
        [
            "",
            "See [API_INDEX.md](API_INDEX.md) for global helpers.",
            "",
        ]
    )
    return "\n".join(lines)


def main() -> None:
    rows = build_rows()
    with OUT.open("w", encoding="utf-8", newline="\n") as f:
        f.write(render_markdown(rows))
    print("wrote", OUT, "rows", len(rows))
    missing = sum(1 for _, _, _, primary, _ in rows if primary == "?")
    print("aliases without examples:", missing)


if __name__ == "__main__":
    main()
