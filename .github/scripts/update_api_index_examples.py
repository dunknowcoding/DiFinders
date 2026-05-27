#!/usr/bin/env python3
"""Append representative example folders to API_INDEX sensor family bullets."""

from __future__ import annotations

import re
from collections import defaultdict
from pathlib import Path

import sys

ROOT = Path(__file__).resolve().parents[2]
INDEX = ROOT / "docs" / "API_INDEX.md"
sys.path.insert(0, str(ROOT / "scripts"))

from append_sensor_aliases import build_family_metadata  # noqa: E402

# API_INDEX link text -> family class name
LINK_TO_FAMILY = {
    "PulseUltrasonicSensor": "PulseUltrasonicSensor",
    "TFMiniSensor": "TFMiniSensor",
    "A02YYUWSensor": "A02YYUWSensor",
    "US100Sensor": "US100Sensor",
    "GyUs42I2cSensor": "GyUs42I2cSensor",
    "GyUs42PulseSensor": "GyUs42PulseSensor",
    "MaxBotixAnalogSensor": "MaxBotixAnalogSensor",
    "TF03CanSensor": "TF03CanSensor",
    "ModbusDistanceSensor": "ModbusDistanceSensor",
    "VL53L0XSensor": "VL53L0XSensor",
    "VL53L1XSensor": "VL53L1XSensor",
    "VL53L1XSpiSensor": "VL53L1XSpiSensor",
    "VL53L4CDSensor": "VL53L4CDSensor",
    "VL53L4CXSensor": "VL53L4CXSensor",
    "VL6180XSensor": "VL6180XSensor",
    "ToFSenseFI2CSensor": "ToFSenseFI2CSensor",
    "NoopLoopToFSensor": "NoopLoopToFSensor",
    "AsciiUartRangeSensor": "AsciiUartRangeSensor",
    "DigitalIrBarrierSensor": "DigitalIrBarrierSensor",
    "ReflectiveIrSensor": "ReflectiveIrSensor",
    "SharpAnalogDistanceSensor": "SharpAnalogDistanceSensor",
    "SharpDigitalDistanceSensor": "SharpDigitalDistanceSensor",
    "Gp2Y0E03Sensor": "Gp2Y0E03Sensor",
    "LD2410RadarSensor": "LD2410RadarSensor",
    "LD2411RadarSensor": "LD2411RadarSensor",
    "LD2412RadarSensor": "LD2412RadarSensor",
    "LD2420RadarSensor": "LD2420RadarSensor",
    "LD2450RadarSensor": "LD2450RadarSensor",
    "LD2461RadarSensor": "LD2461RadarSensor",
    "MmwavePresenceSensor": "MmwavePresenceSensor",
    "OpenProtocolMmwaveSensor": "OpenProtocolMmwaveSensor",
    "PirMotionSensor": "PirMotionSensor",
}


def main() -> None:
    _, examples_by_family, _ = build_family_metadata()
    text = INDEX.read_text(encoding="utf-8")

    def repl_line(match: re.Match) -> str:
        line = match.group(0)
        m = re.search(r"\[`(\w+)`\]", line)
        if not m:
            return line
        family = LINK_TO_FAMILY.get(m.group(1), m.group(1))
        ex = examples_by_family.get(family, [])
        if not ex or "Examples:" in line:
            return line
        ex_str = ", ".join(f"`{e}`" for e in ex[:4])
        if len(ex) > 4:
            ex_str += ", â€?
        return line.rstrip() + f" â€?Examples: {ex_str}"

    new_text = re.sub(r"^- \[`[^`]+`\]\([^)]+\)[^\n]*$", repl_line, text, flags=re.MULTILINE)
    INDEX.write_text(new_text, encoding="utf-8")
    print("updated", INDEX)


if __name__ == "__main__":
    main()
