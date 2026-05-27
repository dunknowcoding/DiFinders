#!/usr/bin/env python3
"""Fail if user-facing docs/examples contain CJK or Cyrillic (L105)."""

from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
PATHS = [
    ROOT / "examples",
    ROOT / "docs" / "USER_GUIDE.md",
    ROOT / "docs" / "API_INDEX.md",
    ROOT / "docs" / "ALIAS_INDEX.md",
    ROOT / "README.md",
    ROOT / "CONTRIBUTING.md",
]
CJK = re.compile(r"[\u4e00-\u9fff]")
CYR = re.compile(r"[\u0400-\u04FF]")


def main() -> int:
    issues: list[str] = []
    for base in PATHS:
        if base.is_file():
            files = [base]
        else:
            files = list(base.rglob("*.ino")) + list(base.rglob("*.md"))
        for path in files:
            text = path.read_text(encoding="utf-8", errors="replace")
            if CJK.search(text) or CYR.search(text):
                issues.append(str(path.relative_to(ROOT)))
    if issues:
        for item in issues:
            print(item)
        print(f"non-English text in {len(issues)} file(s)")
        return 1
    print("English check passed.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
