#include <QStandardPaths>
#include <QButtonGroup>
#include <QRadioButton>
#include <QApplication>
#include <QVBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QLineEdit>
#include <QGroupBox>
#include <QCheckBox>
#include <QLabel>

#include "NotepadNextApplication.h"
#include "PreferencesViewUtils.h"
#include "BehaviorCategoryItem.h"
#include "TranslationManager.h"
#include "ScintillaNext.h"

using namespace Preferences;

namespace
{
    const QLatin1StringView PathStyleValid("QLineEdit { border: 1px solid #2ecc71; }");
    const QLatin1StringView PathStyleInvalid("QLineEdit { border: 1px solid #e74c3c; }");

    inline QGroupBox *DefaultFolderView(ApplicationSettings *settings)
    {
        using BehaviourEnum = ApplicationSettings::DefaultDirectoryBehaviorEnum;

        const auto group = new QGroupBox(QObject::tr("Default directory"));

        const auto layout = new QVBoxLayout(group);

        const auto currentRadio  = new QRadioButton(QObject::tr("Current document directory"));
        const auto lastUsedRadio = new QRadioButton(QObject::tr("Last used directory"));
        const auto selectedRadio = new QRadioButton(QObject::tr("Selected directory:"));

        const auto buttonGroup = new QButtonGroup(group);
        buttonGroup->addButton(currentRadio,  ApplicationSettings::FollowCurrentDocument);
        buttonGroup->addButton(lastUsedRadio, ApplicationSettings::RememberLastUsed);
        buttonGroup->addButton(selectedRadio, ApplicationSettings::HardCoded);

        const auto selectedPathEdit = new QLineEdit;
        selectedPathEdit->setClearButtonEnabled(true);
        selectedPathEdit->setPlaceholderText(QObject::tr("Selected directory path here..."));
#if QT_CONFIG(completer)
        selectedPathEdit->setCompleter(
            FilesystemCompliter(
                selectedPathEdit,
                Qt::CaseInsensitive,
                QCompleter::PopupCompletion
            )
        );
#endif

        const auto pathBrowseButton = new QPushButton;
        pathBrowseButton->setIcon(qApp->style()->standardIcon(QStyle::SP_DirIcon));
        pathBrowseButton->setToolTip(QObject::tr("Open directory select dialog"));

        const auto setPathInputEnabled = [selectedPathEdit, pathBrowseButton](bool state) {
            selectedPathEdit->setEnabled(state);
            pathBrowseButton->setEnabled(state);
        };

        const auto selectedLayout = new QHBoxLayout;
        selectedLayout->addWidget(selectedRadio);
        selectedLayout->addWidget(selectedPathEdit, 1);
        selectedLayout->addWidget(pathBrowseButton);
        selectedLayout->setContentsMargins(MARGINS_0);
        selectedLayout->setSpacing(SPACING_3);

        layout->addWidget(currentRadio);
        layout->addWidget(lastUsedRadio);
        layout->addLayout(selectedLayout);
        layout->setContentsMargins(MARGINS_6);
        layout->setSpacing(SPACING_6);

        {// Load actual path
            const auto selectedPath = QDir::toNativeSeparators(settings->defaultDirectory());
            if (!selectedPath.isEmpty())
                selectedPathEdit->setText(selectedPath);
            else
                selectedPathEdit->setText(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
        }

        QObject::connect(selectedPathEdit, &QLineEdit::textChanged,
                         settings,         [selectedPathEdit, settings](const QString &text) {
            QFileInfo checkDir(text);
            QString toolTip;
            QString style;

            if (!text.isEmpty())
            {
                bool checks = checkDir.exists()
                              && checkDir.isDir()
                              && checkDir.isWritable();
                if (checks)
                {
                    style = PathStyleValid;
                    settings->setDefaultDirectory(text);
                }
                else
                {
                    QStringList whys;

                    if (!checkDir.exists())
                        whys += QObject::tr("Not exists");
                    if (!checkDir.isDir())
                        whys += QObject::tr("Not a directory");
                    if (!checkDir.isWritable())
                        whys += QObject::tr("No write access");

                    toolTip = whys.join(" | ");

                    style = PathStyleInvalid;
                }
            }
            else
                toolTip = QObject::tr("Please, use absolute path");

            selectedPathEdit->setStyleSheet(style);
            selectedPathEdit->setToolTip(toolTip);
        });
        QObject::connect(pathBrowseButton, &QPushButton::clicked,
                         group,            [group, selectedPathEdit]() {
            const auto selected = QFileDialog::getExistingDirectory(
                group,
                QObject::tr("Select default directory"),
                selectedPathEdit->text(),
                QFileDialog::ShowDirsOnly
                | QFileDialog::DontResolveSymlinks
            );
            if (!selected.isEmpty())
                selectedPathEdit->setText(selected);
        });

        QObject::connect(buttonGroup, &QButtonGroup::idClicked,
                         settings,    [settings, setPathInputEnabled](int id) {
            setPathInputEnabled(id == BehaviourEnum::HardCoded);
            settings->setDefaultDirectoryBehavior((BehaviourEnum)id);
        });
        QObject::connect(settings,    &ApplicationSettings::defaultDirectoryBehaviorChanged,
                         buttonGroup, [buttonGroup, setPathInputEnabled](BehaviourEnum value) {
            const auto button = buttonGroup->button(value);
            const QSignalBlocker block(buttonGroup); // Prevent ss-loop
            if (button) button->click();
            setPathInputEnabled(value == BehaviourEnum::HardCoded);
        });

        // Load current selection AFTER QObject::connect's
        const auto checkableRadio = buttonGroup->button(settings->defaultDirectoryBehavior());
        if (checkableRadio) checkableRadio->click();

        return group;
    }

    inline QGroupBox *PreviousSessionView(ApplicationSettings *settings)
    {
        const auto group = new QGroupBox(QObject::tr("Restore previous session"));
        group->setCheckable(true);

        const auto layout = new QVBoxLayout(group);
        layout->addWidget(CreateCheckBox(
            QObject::tr("Unsaved changes"),
            settings,
            PREFERENCES_BIND_PROPERTY(restoreUnsavedFiles, RestoreUnsavedFiles)
        ));
        layout->addWidget(CreateCheckBox(
            QObject::tr("Temporary files"),
            settings,
            PREFERENCES_BIND_PROPERTY(restoreTempFiles, RestoreTempFiles)
        ));
        layout->setContentsMargins(MARGINS_6);
        layout->setSpacing(SPACING_6);

        group->setChecked(settings->restorePreviousSession());

        QObject::connect(settings, &ApplicationSettings::restorePreviousSessionChanged,
                         group,    &QGroupBox::setChecked);
        QObject::connect(group,    &QGroupBox::toggled,
                         settings, &ApplicationSettings::setRestorePreviousSession);

        return group;
    }

    inline QGridLayout *AppLanguageView(ApplicationSettings *settings)
    {
        const auto app = qobject_cast<NotepadNextApplication*>(qApp);

        const auto languageCombo = new QComboBox;
        languageCombo->addItem(QObject::tr("Like in system"), QStringLiteral(""));
        for (const auto &languageData : app->getTranslationManager()->availableTranslations())
        {
            QLocale locale(languageData);
            const auto languageTitle = TranslationManager::FormatLocaleTerritoryAndLanguage(locale);
            languageCombo->addItem(languageTitle, languageData);
        }

        const auto wheelSuppressor = new WheelEventSuppressFilter(languageCombo);
        wheelSuppressor->setupFor(languageCombo);

        const auto restartNotification = new RestartRequiredLabel;

        const auto layout = new QGridLayout;
        layout->addWidget(new QLabel(QObject::tr("Language:")), 0, 0);
        layout->addWidget(languageCombo, 0, 1);
        layout->addWidget(restartNotification, 1, 0, 1, 2);
        layout->setColumnStretch(1, 1);
        layout->setContentsMargins(MARGINS_0);
        layout->setSpacing(SPACING_3);

        const auto setComboByData = [languageCombo](const QString &data) {
            const auto index = languageCombo->findData(data);
            languageCombo->setCurrentIndex(index >= 0 ? index : 0);
        };

        setComboByData(settings->translation());

        QObject::connect(languageCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                         settings,      [settings, languageCombo](int index) {
            settings->setTranslation(languageCombo->itemData(index).toString());
        });
        QObject::connect(settings,      &ApplicationSettings::translationChanged,
                         languageCombo, setComboByData);

        return layout;
    }

    inline QHBoxLayout *EOLTypeView(ApplicationSettings *settings)
    {
        const auto eolCombo = new QComboBox;
        eolCombo->addItem(QObject::tr("System default"),  QString(""));
        eolCombo->addItem(QObject::tr("Windows (CR LF)"), ScintillaNext::eolModeToString(SC_EOL_CRLF));
        eolCombo->addItem(QObject::tr("Unix (LF)"),       ScintillaNext::eolModeToString(SC_EOL_LF));
        eolCombo->addItem(QObject::tr("Macintosh (CR)"),  ScintillaNext::eolModeToString(SC_EOL_CR));

        const auto wheelSuppressor = new WheelEventSuppressFilter(eolCombo);
        wheelSuppressor->setupFor(eolCombo);

        const auto layout = new QHBoxLayout;
        layout->addWidget(new QLabel(QObject::tr("Default line endings:")));
        layout->addWidget(eolCombo, 1);
        layout->setContentsMargins(MARGINS_0);
        layout->setSpacing(SPACING_3);

        const auto setComboByData = [eolCombo](const QString &data) {
            const auto index = eolCombo->findData(data);
            eolCombo->setCurrentIndex(index >= 0 ? index : 0);
        };

        setComboByData(settings->defaultEOLMode());

        QObject::connect(eolCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                         settings, [settings, eolCombo](int index) {
            settings->setDefaultEOLMode(eolCombo->itemData(index).toString());
        });
        QObject::connect(settings, &ApplicationSettings::defaultEOLModeChanged,
                         eolCombo, setComboByData);

        return layout;
    }
}

QWidget *BehaviorCategoryItem::contentView(ApplicationSettings *settings) const
{
    const auto widget = new QWidget;
    const auto layout = new QVBoxLayout(widget);

    layout->addLayout(AppLanguageView(settings));
    layout->addWidget(PreviousSessionView(settings));
    layout->addWidget(DefaultFolderView(settings));
    layout->addLayout(EOLTypeView(settings));
    layout->addWidget(CreateCheckBox(
        QObject::tr("Recenter find/replace dialog when opened"),
        settings,
        PREFERENCES_BIND_PROPERTY(centerSearchDialog, CenterSearchDialog)
    ));
    layout->addWidget(CreateCheckBox(
        QObject::tr("Combine search results"),
        settings,
        PREFERENCES_BIND_PROPERTY(combineSearchResults, CombineSearchResults)
    ));
    layout->addWidget(CreateCheckBox(
        QObject::tr("Exit on last tab closed"),
        settings,
        PREFERENCES_BIND_PROPERTY(exitOnLastTabClosed, ExitOnLastTabClosed)
    ));
    layout->addStretch(1);
    layout->setContentsMargins(MARGINS_6);
    layout->setSpacing(SPACING_6);

    return widget;
}
