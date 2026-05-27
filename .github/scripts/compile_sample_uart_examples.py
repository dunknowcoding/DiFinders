#!/usr/bin/env python3
"""Compile a small ESP32 UART sample (Phase D CI smoke)."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
FQBN = "esp32:esp32:esp32"
SAMPLES = [
    "TFLuna_BasicRead",
    "A02YYUW_BasicRead",
    "HLK_LD2411_UART_BasicRead",
    "US100_BasicRead",
    "OpenProtocol_Mmwave_BasicRead",
]


def main() -> int:
    for name in SAMPLES:
        path = ROOT / "examples" / name
        print(f"Compiling {name} for {FQBN}...")
        cmd = [
            "arduino-cli",
            "compile",
            "--fqbn",
            FQBN,
            "--libraries",
            str(ROOT),
            str(path),
        ]
        if subprocess.run(cmd, cwd=ROOT).returncode != 0:
            return 1
    print("ESP32 UART sample builds passed.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
