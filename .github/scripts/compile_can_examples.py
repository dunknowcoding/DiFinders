#!/usr/bin/env python3
"""Compile Benewake_TF03_CAN_* examples (requires autowp-mcp2515 for MCP2515 sketches)."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

TARGETS = [
    ("esp32:esp32:esp32", "Benewake_TF03_CAN_SN65HVD230_ESP32"),
    ("esp32:esp32:esp32", "Benewake_TF03_CAN_MCP2515_ESP32"),
    ("arduino:renesas_uno:minima", "Benewake_TF03_CAN_MCP2515_UnoR4"),
]


def ensure_mcp2515_lib() -> None:
    subprocess.run(
        ["arduino-cli", "lib", "install", "autowp-mcp2515"],
        check=False,
        stdout=subprocess.DEVNULL,
    )


def main() -> int:
    ensure_mcp2515_lib()
    failures: list[tuple[str, str]] = []
    for fqbn, sketch in TARGETS:
        path = ROOT / "examples" / sketch
        cmd = [
            "arduino-cli",
            "compile",
            "--fqbn",
            fqbn,
            "--libraries",
            str(ROOT),
            str(path),
        ]
        print("+", " ".join(cmd))
        if subprocess.run(cmd, cwd=ROOT).returncode != 0:
            failures.append((fqbn, sketch))
    if failures:
        print("FAILED:", failures)
        return 1
    print("All CAN example builds passed.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
