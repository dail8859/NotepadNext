# Enhancement: Font Style Selection for Qt 6

## Problem

After building NotepadNext with Qt 6, some fonts are missing from the Preferences font picker — specifically font variants like "Sarasa Fixed SC Nerd Font Light". Older builds (Qt 5) showed these variants as separate entries.

## Root Cause

Qt 6 changed how font families are enumerated. In Qt 5, each font variant (Regular, Light, Bold, etc.) appeared as a separate family name. In Qt 6, variants are merged under a single family, with the variant becoming a style/weight attribute.

`QFontComboBox` (used in the original implementation) only shows the merged family names. There is no built-in way to select a specific style (Light, Thin, SemiBold, etc.) through `QFontComboBox` — it lacks a style/weight selector.

| | Qt 5 | Qt 6 |
|---|---|---|
| "Sarasa Fixed SC Nerd Font Light" | Separate family ✅ | Merged into "Sarasa Fixed SC Nerd Font" ❌ |
| `QFontDatabase::families()` | Returns all variant names | Returns merged family names |
| `QFontDatabase::styles(family)` | N/A | Returns "Light", "Regular", "Bold", etc. |

## Fix

**Commit:** `10ba404`
**Files changed:** `src/ApplicationSettings.h`, `src/ApplicationSettings.cpp`, `src/dialogs/PreferencesDialog.ui`, `src/dialogs/PreferencesDialog.cpp`, `src/EditorManager.cpp`

### 1. New `FontStyle` setting (`ApplicationSettings`)

Added a persisted `FontStyle` setting (stored as the style name string, e.g. "Light", "Bold Italic"):

```cpp
// ApplicationSettings.h
DEFINE_SETTING(FontStyle, fontStyle, QString);

// ApplicationSettings.cpp
CREATE_SETTING(Editor, FontStyle, fontStyle, QString, QStringLiteral(""))
```

Empty string defaults to Regular / the first available style.

### 2. UI: Font family + Style combo boxes (`PreferencesDialog.ui`)

Replaced the single `QFontComboBox` with two `QComboBox` widgets in the "Default Font" group:

```
Row 1: Font      [QComboBox - font family]      ← was QFontComboBox
Row 2: Style     [QComboBox - font style]       ← new
Row 3: Font Size [QSpinBox]                     ← unchanged
```

### 3. Font enumeration logic (`PreferencesDialog.cpp`)

- Font family list populated via `QFontDatabase::families()` (shows all system font families)
- When user selects a family, the style combo is repopulated via `QFontDatabase::styles(family)`
- Style selection falls back to saved style → "Regular" → first available
- `blockSignals()` used during style repopulation to prevent spurious settings updates
- Two-way bindings: settings changes update widgets, widget changes update settings

```cpp
auto populateFontStyles = [this](const QString &family) {
    ui->comboBoxFontStyle->blockSignals(true);
    ui->comboBoxFontStyle->clear();
    for (const QString &style : QFontDatabase::styles(family)) {
        ui->comboBoxFontStyle->addItem(style);
    }
    // Try saved style → "Regular" → first item
    QString savedStyle = settings->fontStyle();
    int idx = ui->comboBoxFontStyle->findText(savedStyle);
    if (idx == -1) idx = ui->comboBoxFontStyle->findText("Regular");
    ui->comboBoxFontStyle->setCurrentIndex(qMax(idx, 0));
    ui->comboBoxFontStyle->blockSignals(false);
};
```

### 4. Apply font weight/italic to editors (`EditorManager.cpp`)

The selected style is mapped to Scintilla weight and italic via `QFontDatabase::font()`:

**Initial editor setup** (between `styleSetFont` and `styleClearAll`):
```cpp
if (!settings->fontStyle().isEmpty()) {
    QFont selectedFont = QFontDatabase::font(settings->fontName(), settings->fontStyle(), settings->fontSize());
    editor->styleSetWeight(STYLE_DEFAULT, selectedFont.weight());
    editor->styleSetItalic(STYLE_DEFAULT, selectedFont.italic());
}
```

**Live update handler** (follows existing `fontNameChanged` / `fontSizeChanged` pattern):
```cpp
connect(settings, &ApplicationSettings::fontStyleChanged, this, [=](QString fontStyle){
    QFont selectedFont = QFontDatabase::font(settings->fontName(), fontStyle, settings->fontSize());
    for (auto &editor : getEditors()) {
        for (int i = 0; i <= STYLE_MAX; ++i) {
            editor->styleSetWeight(i, selectedFont.weight());
            editor->styleSetItalic(i, selectedFont.italic());
        }
    }
});
```

### Scintilla API used

- `styleSetWeight(style, weight)` — fine-grained weight (1-999: 400=normal, 600=semibold, 700=bold)
- `styleSetItalic(style, italic)` — italic on/off
- Qt 6 `QFont::weight()` returns 0-1000, directly compatible with Scintilla's range

## Diff summary

```
 src/ApplicationSettings.cpp       |  1+
 src/ApplicationSettings.h         |  1+
 src/EditorManager.cpp             | 19+
 src/dialogs/PreferencesDialog.cpp | 53+ / 5-
 src/dialogs/PreferencesDialog.ui  | 12+ / 1-
 Total: 5 files, +80 / -6
```
