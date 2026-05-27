#!/usr/bin/env python3
"""Compile high-throughput example sketches (CI; mirrors extras/compile_high_rate_examples.py)."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

TARGETS = [
    ("esp32:esp32:esp32", "VL53L1X_SpiBurstRead"),
    ("esp32:esp32:esp32", "HLK_LD2410_UartHighRateRead"),
    ("esp32:esp32:esp32", "VL53L4CD_HighRateRead"),
    ("arduino:renesas_uno:minima", "VL53L4CD_HighRateRead"),
    ("arduino:renesas_uno:minima", "VL53L0X_HighRateRead"),
]


def main() -> int:
    for fqbn, example in TARGETS:
        path = ROOT / "examples" / example
        print(f"Compiling {example} for {fqbn}...")
        cmd = [
            "arduino-cli",
            "compile",
            "--fqbn",
            fqbn,
            "--libraries",
            str(ROOT),
            str(path),
        ]
        result = subprocess.run(cmd, cwd=ROOT)
        if result.returncode != 0:
            return result.returncode
    print("All high-rate example builds passed.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
