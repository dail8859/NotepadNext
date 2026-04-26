#include <QFontComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QComboBox>
#include <QLabel>

#include "AppearanceCategoryItem.h"
#include "PreferencesViewUtils.h"
#include "ApplicationSettings.h"

using namespace Preferences;

namespace
{
    inline QGroupBox *WindowAppearanceView(ApplicationSettings *settings)
    {
        const auto group = new QGroupBox(QObject::tr("Window"));

        const auto layout = new QVBoxLayout(group);
        layout->addWidget(CreateCheckBox(
            QObject::tr("Show menu bar"),
            settings,
            PREFERENCES_BIND_PROPERTY(showMenuBar, ShowMenuBar)
        ));
        layout->addWidget(CreateCheckBox(
            QObject::tr("Show toolbar"),
            settings,
            PREFERENCES_BIND_PROPERTY(showToolBar, ShowToolBar)
        ));
        layout->addWidget(CreateCheckBox(
            QObject::tr("Show status bar"),
            settings,
            PREFERENCES_BIND_PROPERTY(showStatusBar, ShowStatusBar)
        ));
        layout->setContentsMargins(MARGINS_6);
        layout->setSpacing(SPACING_6);

        return group;
    }

    inline QGroupBox *FontAppearanceView(ApplicationSettings *settings)
    {
        const auto group = new QGroupBox(QObject::tr("Font"));

        const auto familyCombo = new QFontComboBox;

        const auto sizeCombo = new QComboBox;
        sizeCombo->addItems({
             "4",  "6",  "8",  "9", "10",
            "11", "12", "14", "16", "18",
            "20", "22", "24", "26", "28",
            "36", "48", "72"
        });
        sizeCombo->setValidator(new QIntValidator(4, 200, sizeCombo));
        sizeCombo->setEditable(true);

        const auto wheelSuppressor = new WheelEventSuppressFilter(group);
        wheelSuppressor->setupFor(familyCombo);
        wheelSuppressor->setupFor(sizeCombo);

        const auto layout = new QHBoxLayout(group);
        layout->addWidget(new QLabel(QObject::tr("Family:")));
        layout->addWidget(familyCombo, 1);
        layout->addSpacing(3);
        layout->addWidget(new QLabel(QObject::tr("Size:")));
        layout->addWidget(sizeCombo);
        layout->setContentsMargins(MARGINS_6);
        layout->setSpacing(3);

        familyCombo->setCurrentFont(QFont(settings->fontName()));
        sizeCombo->setCurrentText(QString("%1").arg(settings->fontSize()));

        QObject::connect(familyCombo, &QFontComboBox::currentFontChanged,
                         settings,    [settings](const QFont &font) {
            settings->setFontName(font.family());
        });
        QObject::connect(settings,    &ApplicationSettings::fontNameChanged,
                         familyCombo, [familyCombo](const QString &fontName){
            familyCombo->setCurrentFont(QFont(fontName));
        });

        QObject::connect(sizeCombo, &QComboBox::currentTextChanged,
                         settings,  [settings](const QString &value) {
            settings->setFontSize(value.toInt());
        });
        QObject::connect(settings,  &ApplicationSettings::fontSizeChanged,
                         sizeCombo, [sizeCombo](int size) {
            sizeCombo->setCurrentText(QString("%1").arg(size));
        });

        return group;
    }

    inline QGroupBox *EditorAppearanceView(ApplicationSettings *settings)
    {
        const auto group = new QGroupBox(QObject::tr("Editor"));

        const auto layout = new QVBoxLayout(group);
        layout->addWidget(FontAppearanceView(settings));
        layout->addWidget(CreateCheckBox(
            QObject::tr("Highlight URLs"),
            settings,
            PREFERENCES_BIND_PROPERTY(urlHighlighting, URLHighlighting)
        ));
        layout->addWidget(CreateCheckBox(
            QObject::tr("Show Line Numbers"),
            settings,
            PREFERENCES_BIND_PROPERTY(showLineNumbers, ShowLineNumbers)
        ));
        layout->setContentsMargins(MARGINS_6);
        layout->setSpacing(SPACING_6);

        return group;
    }
}

QWidget *AppearanceCategoryItem::contentView(ApplicationSettings *settings) const
{
    const auto widget = new QWidget;
    const auto layout = new QVBoxLayout(widget);

    layout->addWidget(WindowAppearanceView(settings));
    layout->addWidget(EditorAppearanceView(settings));
    layout->addStretch(1);
    layout->setContentsMargins(MARGINS_6);
    layout->setSpacing(SPACING_6);

    return widget;
}
