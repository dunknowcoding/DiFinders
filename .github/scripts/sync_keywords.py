#!/usr/bin/env python3
"""Regenerate keywords.txt from DiFinders.h sketch types (classes + using aliases)."""

from __future__ import annotations

import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
HEADER = ROOT / "src" / "DiFinders.h"
USER_API = ROOT / "src" / "DiFindersUserAPI.h"
OUT = ROOT / "keywords.txt"

SKIP = {
    "Ultrasonic",
    "Reflective",
    "Motion",
    "Presence",
    "MmwaveOpenProtocol",
    "ToF",
    "UltrasonicRadarRangingSensor",
}


def collect_symbols() -> list[str]:
    text = HEADER.read_text(encoding="utf-8", errors="replace")
    names: set[str] = set()
    for m in re.finditer(r"class\s+(\w+)\s*:", text):
        names.add(m.group(1))
    for m in re.finditer(r"using\s+(\w+)\s*=", text):
        names.add(m.group(1))
    names -= SKIP
    # Global helpers exported to sketches
    api = USER_API.read_text(encoding="utf-8", errors="replace")
    for m in re.finditer(r"using DiFinders::(\w+);", api):
        name = m.group(1)
        if name[0].isupper() or name.startswith("df_"):
            names.add(name)
    return sorted(names)


def main() -> None:
    symbols = collect_symbols()
    lines = ["DiFinders\tKEYWORD1", ""]
    for sym in symbols:
        lines.append(f"{sym}\tKEYWORD2")
    lines.append("")
    OUT.write_text("\n".join(lines), encoding="utf-8")
    print("wrote", OUT, "entries", len(symbols))


if __name__ == "__main__":
    main()
