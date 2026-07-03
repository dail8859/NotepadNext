from pathlib import Path
import re

root = Path("..") / "src"

pattern = re.compile(
    r'(<property name="shortcut">\s*<string)(?![^>]*\bnotr=)(>)',
    re.DOTALL,
)

for ui_file in root.rglob("*.ui"):
    text = ui_file.read_text(encoding="utf-8")

    new_text = pattern.sub(r'\1 notr="true"\2', text)

    if new_text != text:
        ui_file.write_text(new_text, encoding="utf-8")
        print(f"Updated {ui_file}")