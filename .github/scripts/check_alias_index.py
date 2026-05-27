#!/usr/bin/env python3
"""Regenerate ALIAS_INDEX.md and fail if git would change it (CI guard)."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
INDEX = ROOT / "docs" / "ALIAS_INDEX.md"
GENERATOR = ROOT / ".github" / "scripts" / "generate_alias_index.py"


def main() -> int:
    def normalize(text: str) -> str:
        return text.replace("\r\n", "\n").replace("\r", "\n")

    before = normalize(INDEX.read_text(encoding="utf-8") if INDEX.exists() else "")
    subprocess.run([sys.executable, str(GENERATOR)], check=True, cwd=ROOT)
    after = normalize(INDEX.read_text(encoding="utf-8"))
    if before != after:
        print("ALIAS_INDEX.md is out of date. Run: python scripts/generate_alias_index.py")
        return 1
    print("ALIAS_INDEX.md is up to date.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
