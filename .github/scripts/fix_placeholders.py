from pathlib import Path

root = Path(__file__).resolve().parents[2] / "examples"
for p in root.rglob("*.ino"):
    t = p.read_text(encoding="utf-8")
    n = t
    n = n.replace('Serial.println(F("mm"));', 'Serial.println(F("ready"));')
    n = n.replace('Serial.println(F("msg"));', 'Serial.println(F("ready"));')
    n = n.replace('Serial.print(F("mm"));', 'Serial.print(F("distance_mm="));')
    n = n.replace('Serial.print(F("msg"));', 'Serial.print(F("value="));')
    if n != t:
        p.write_text(n, encoding="utf-8")
        print(p.parent.name)
