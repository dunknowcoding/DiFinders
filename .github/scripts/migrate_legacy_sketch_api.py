#!/usr/bin/env python3
"""Remove legacy DF_PROBE / DF_READ / df_code from example sketches."""

from __future__ import annotations

import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
EXAMPLES = ROOT / "examples"

DF_PROBE_LINE = re.compile(r"^\s*DF_PROBE\s*\([^)]*\)\s*;\s*\n", re.MULTILINE)
DF_READ = re.compile(r"\bDF_READ\s*\(\s*([^)]+)\s*\)")
DF_CODE = re.compile(r"\bdf_code\s*\(\s*([^)]+)\s*\)")
LEGACY_COMMENT = re.compile(
    r"\s*\*\s*Sketch API:.*(?:df_serial|DF_READ|DF_PROBE).*",
    re.IGNORECASE,
)


def migrate(text: str) -> tuple[str, list[str]]:
    changes: list[str] = []
    new = text

    if DF_PROBE_LINE.search(new):
        new = DF_PROBE_LINE.sub("", new)
        changes.append("removed DF_PROBE")

    if DF_READ.search(new):
        new = DF_READ.sub(r"\1.read()", new)
        changes.append("DF_READ -> .read()")

    if DF_CODE.search(new):
        new = DF_CODE.sub(r"static_cast<uint8_t>(\1)", new)
        changes.append("df_code -> static_cast")

    if LEGACY_COMMENT.search(new):
        new = LEGACY_COMMENT.sub("", new)
        changes.append("removed legacy comment")

    return new, changes


def main() -> None:
    touched = 0
    for path in sorted(EXAMPLES.rglob("*.ino")):
        original = path.read_text(encoding="utf-8", errors="replace")
        updated, changes = migrate(original)
        if updated != original:
            path.write_text(updated, encoding="utf-8")
            print(f"updated {path.relative_to(ROOT)}: {', '.join(changes)}")
            touched += 1
    print(f"done: {touched} file(s)")


if __name__ == "__main__":
    main()
