#!/usr/bin/env python3
"""Lint DiFinders example sketches for API and output conventions."""

from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
EXAMPLES = ROOT / "examples"

FORBIDDEN = [
    (re.compile(r"\bDF_PROBE\b"), "DF_PROBE"),
    (re.compile(r"\bDF_READ\b"), "DF_READ"),
    (re.compile(r"\bdf_code\b"), "df_code"),
    (re.compile(r"\bdifinders_"), "difinders_"),
    (re.compile(r"\bDIFINDERS_PRINT_PROBE\b"), "DIFINDERS_PRINT_PROBE"),
]

CJK = re.compile(r"[\u4e00-\u9fff]")
CYRILLIC = re.compile(r"[\u0400-\u04FF]")

# Sketches that print serial telemetry should use at least one canonical key.
OUTPUT_KEYS = re.compile(
    r'F\("(distance_mm=|motion=|detected=|status=|proximity=|presence=|occupied=|targets=|mm=)'
)

SMOKE_SKIP_OUTPUT = {"Library_CompileSmoke", "ThirdParty_CoCompileSmoke"}


def requires_output_keys(folder: str) -> bool:
    """Basic/motion UART reads must use canonical Serial keys (L87)."""
    return (
        folder.endswith("_BasicRead")
        or folder.endswith("_MotionRead")
        or folder.endswith("_PresenceRead")
        or folder == "OpenProtocol_Mmwave_BasicRead"
    )


def lint_file(path: Path) -> list[str]:
    text = path.read_text(encoding="utf-8", errors="replace")
    rel = path.relative_to(ROOT)
    issues: list[str] = []
    folder = path.parent.name

    for pattern, name in FORBIDDEN:
        if pattern.search(text):
            issues.append(f"{rel}: forbidden symbol {name}")

    if CJK.search(text) or CYRILLIC.search(text):
        issues.append(f"{rel}: non-English user-visible text")

    if (
        folder not in SMOKE_SKIP_OUTPUT
        and requires_output_keys(folder)
        and "Serial.print" in text
        and "Serial.println" in text
    ):
        if not OUTPUT_KEYS.search(text):
            issues.append(
                f"{rel}: missing canonical output key "
                '(distance_mm=, motion=, detected=, status=, proximity=, presence=)'
            )

    return issues


def main() -> int:
    all_issues: list[str] = []
    for path in sorted(EXAMPLES.rglob("*.ino")):
        all_issues.extend(lint_file(path))

    if all_issues:
        for issue in all_issues:
            print(issue)
        print(f"lint failed: {len(all_issues)} issue(s)")
        return 1

    print(f"lint ok: {len(list(EXAMPLES.rglob('*.ino')))} sketch(s)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
