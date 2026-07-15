/*
 * This file is part of Notepad Next.
 * Copyright 2019 Justin Dailey
 *
 * Notepad Next is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Notepad Next is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Notepad Next.  If not, see <https://www.gnu.org/licenses/>.
 */


#include "PreferencesDialog.h"
#include "NotepadNextApplication.h"
#include "TranslationManager.h"
#include "ui_PreferencesDialog.h"
#include "ScintillaNext.h"

#include <QButtonGroup>
#include <QFileDialog>
#include <QFontDatabase>
#include <QMessageBox>


PreferencesDialog::PreferencesDialog(ApplicationSettings *settings, QWidget *parent) :
    QDialog(parent, Qt::Tool),
    ui(new Ui::PreferencesDialog),
    settings(settings)
{
    ui->setupUi(this);

    QIcon icon = style()->standardIcon(QStyle::SP_MessageBoxInformation);
    QPixmap pixmap = icon.pixmap(QSize(16, 16));
    ui->labelAppRestartIcon->setPixmap(pixmap);
    ui->labelAppRestartIcon->hide();
    ui->labelAppRestart->hide();

    MapSettingToCheckBox(ui->checkBoxMenuBar, &ApplicationSettings::showMenuBar, &ApplicationSettings::setShowMenuBar, &ApplicationSettings::showMenuBarChanged);
    MapSettingToCheckBox(ui->checkBoxToolBar, &ApplicationSettings::showToolBar, &ApplicationSettings::setShowToolBar, &ApplicationSettings::showToolBarChanged);
    MapSettingToCheckBox(ui->checkBoxStatusBar, &ApplicationSettings::showStatusBar, &ApplicationSettings::setShowStatusBar, &ApplicationSettings::showStatusBarChanged);
    MapSettingToCheckBox(ui->checkBoxRecenterSearchDialog, &ApplicationSettings::centerSearchDialog, &ApplicationSettings::setCenterSearchDialog, &ApplicationSettings::centerSearchDialogChanged);

    MapSettingToGroupBox(ui->gbxRestorePreviousSession, &ApplicationSettings::restorePreviousSession, &ApplicationSettings::setRestorePreviousSession, &ApplicationSettings::restorePreviousSessionChanged);
    connect(ui->gbxRestorePreviousSession, &QGroupBox::toggled, this, [=](bool checked) {
        if (!checked) {
            ui->checkBoxUnsavedFiles->setChecked(false);
            ui->checkBoxRestoreTempFiles->setChecked(false);
        }
        else {
            QMessageBox::warning(this, tr("Warning"), tr("This feature is experimental and it should not be considered safe for critically important work. It may lead to possible data loss. Use at your own risk."));
        }
    });

    MapSettingToCheckBox(ui->checkBoxUnsavedFiles, &ApplicationSettings::restoreUnsavedFiles, &ApplicationSettings::setRestoreUnsavedFiles, &ApplicationSettings::restoreUnsavedFilesChanged);
    MapSettingToCheckBox(ui->checkBoxRestoreTempFiles, &ApplicationSettings::restoreTempFiles, &ApplicationSettings::setRestoreTempFiles, &ApplicationSettings::restoreTempFilesChanged);

    MapSettingToCheckBox(ui->checkBoxCombineSearchResults, &ApplicationSettings::combineSearchResults, &ApplicationSettings::setCombineSearchResults, &ApplicationSettings::combineSearchResultsChanged);

    populateTranslationComboBox();
    connect(ui->comboBoxTranslation, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index) {
        settings->setTranslation(ui->comboBoxTranslation->itemData(index).toString());
        showApplicationRestartRequired();
    });

    MapSettingToCheckBox(ui->checkBoxExitOnLastTabClosed, &ApplicationSettings::exitOnLastTabClosed, &ApplicationSettings::setExitOnLastTabClosed, &ApplicationSettings::exitOnLastTabClosedChanged);

    // Populate font family combo box with all system fonts
    for (const QString &family : QFontDatabase::families()) {
        ui->comboBoxFont->addItem(family);
    }

    // Helper to populate the font style combo for the currently selected family
    auto populateFontStyles = [this](const QString &family) {
        ui->comboBoxFontStyle->blockSignals(true);
        ui->comboBoxFontStyle->clear();
        for (const QString &style : QFontDatabase::styles(family)) {
            ui->comboBoxFontStyle->addItem(style);
        }
        // Try to select the saved style, fall back to "Regular", then first item
        QString savedStyle = this->settings->fontStyle();
        int idx = ui->comboBoxFontStyle->findText(savedStyle);
        if (idx == -1) {
            idx = ui->comboBoxFontStyle->findText(QStringLiteral("Regular"));
        }
        ui->comboBoxFontStyle->setCurrentIndex(qMax(idx, 0));
        ui->comboBoxFontStyle->blockSignals(false);
    };

    // Set initial font family selection and populate styles
    ui->comboBoxFont->setCurrentText(settings->fontName());
    populateFontStyles(settings->fontName());

    // When the user picks a different font family
    connect(ui->comboBoxFont, &QComboBox::currentTextChanged, this, [this, populateFontStyles](const QString &family) {
        this->settings->setFontName(family);
        populateFontStyles(family);
        // Update font style setting to the newly selected style
        this->settings->setFontStyle(ui->comboBoxFontStyle->currentText());
    });

    // When the user picks a different font style
    connect(ui->comboBoxFontStyle, &QComboBox::currentTextChanged, this, [this](const QString &style) {
        this->settings->setFontStyle(style);
    });

    // Two-way bindings: settings → widgets
    connect(settings, &ApplicationSettings::fontNameChanged, this, [this](QString fontName) {
        ui->comboBoxFont->setCurrentText(fontName);
    });
    connect(settings, &ApplicationSettings::fontStyleChanged, this, [this](QString fontStyle) {
        int idx = ui->comboBoxFontStyle->findText(fontStyle);
        if (idx != -1) {
            ui->comboBoxFontStyle->setCurrentIndex(idx);
        }
    });

    ui->spbDefaultFontSize->setValue(settings->fontSize());
    connect(ui->spbDefaultFontSize, QOverload<int>::of(&QSpinBox::valueChanged), settings, &ApplicationSettings::setFontSize);
    connect(settings, &ApplicationSettings::fontSizeChanged, ui->spbDefaultFontSize, &QSpinBox::setValue);

    ui->comboBoxLineEndings->addItem(tr("System Default"), QString(""));
    ui->comboBoxLineEndings->addItem(tr("Windows (CR LF)"), ScintillaNext::eolModeToString(SC_EOL_CRLF));
    ui->comboBoxLineEndings->addItem(tr("Linux (LF)"), ScintillaNext::eolModeToString(SC_EOL_LF));
    ui->comboBoxLineEndings->addItem(tr("Macintosh (CR)"), ScintillaNext::eolModeToString(SC_EOL_CR));

    // Select the current one
    int index = ui->comboBoxLineEndings->findData(settings->defaultEOLMode());
    ui->comboBoxLineEndings->setCurrentIndex(index == -1 ? 0 : index);

    connect(ui->comboBoxLineEndings, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index) {
        settings->setDefaultEOLMode(ui->comboBoxLineEndings->itemData(index).toString());
    });
    connect(settings, &ApplicationSettings::defaultEOLModeChanged, this, [=](QString defaultEOLMode) {
        int index = ui->comboBoxLineEndings->findData(defaultEOLMode);
        ui->comboBoxLineEndings->setCurrentIndex(index == -1 ? 0 : index);
    });

    MapSettingToCheckBox(ui->checkBoxHighlightURLs, &ApplicationSettings::urlHighlighting, &ApplicationSettings::setURLHighlighting, &ApplicationSettings::urlHighlightingChanged);
    MapSettingToCheckBox(ui->checkBoxShowLineNumbers, &ApplicationSettings::showLineNumbers, &ApplicationSettings::setShowLineNumbers, &ApplicationSettings::showLineNumbersChanged);
    MapSettingToCheckBox(ui->checkBoxAutoCompletion, &ApplicationSettings::autoCompletion, &ApplicationSettings::setAutoCompletion, &ApplicationSettings::autoCompletionChanged);

    QButtonGroup *buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(ui->radioFollowCurrentDirectory, ApplicationSettings::FollowCurrentDocument);
    buttonGroup->addButton(ui->radioLastUsedDirectory, ApplicationSettings::RememberLastUsed);
    buttonGroup->addButton(ui->radioHardCoded, ApplicationSettings::HardCoded);

    connect(buttonGroup, &QButtonGroup::idClicked, this, [=](int id) {
        ApplicationSettings::DefaultDirectoryBehaviorEnum e = static_cast<ApplicationSettings::DefaultDirectoryBehaviorEnum>(id);
        settings->setDefaultDirectoryBehavior(e);
    });

    connect(ui->radioHardCoded, &QRadioButton::toggled, this, [=](bool checked){
        ui->btnSelectHardCodedPath->setEnabled(checked);
        ui->txtHardCodedPath->setEnabled(checked);
    });

    connect(ui->btnSelectHardCodedPath, &QToolButton::clicked, this, [=]() {
        QString dir = QFileDialog::getExistingDirectory(this, tr("Default Directory"));
        if (dir.isEmpty()) return; // user cancelled

        settings->setDefaultDirectory(QDir::fromNativeSeparators(dir));
        ui->txtHardCodedPath->setText(QDir::toNativeSeparators(dir));
    });

    connect(ui->txtHardCodedPath, &QLineEdit::editingFinished, this, [=]() {
        QString dir = ui->txtHardCodedPath->text();
        settings->setDefaultDirectory(QDir::fromNativeSeparators(dir));
        ui->txtHardCodedPath->setText(QDir::toNativeSeparators(dir));
    });

    if (auto b = buttonGroup->button(settings->defaultDirectoryBehavior())) {
        b->setChecked(true);
    }

    if (settings->defaultDirectoryBehavior() == ApplicationSettings::HardCoded) {
        ui->txtHardCodedPath->setText((QDir::toNativeSeparators(settings->defaultDirectory())));
    }
    else {
        ui->txtHardCodedPath->setText(QString());
    }
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

void PreferencesDialog::showApplicationRestartRequired() const
{
    ui->labelAppRestartIcon->show();
    ui->labelAppRestart->show();
}

template<typename Func1, typename Func2, typename Func3>
void PreferencesDialog::MapSettingToCheckBox(QCheckBox *checkBox, Func1 getter, Func2 setter, Func3 notifier) const
{
    // Get the value and set the checkbox state
    checkBox->setChecked(std::bind(getter, settings)());

    // Set up two way connection
    connect(settings, notifier, checkBox, &QCheckBox::setChecked);
    connect(checkBox, &QCheckBox::toggled, settings, setter);
}

template<typename Func1, typename Func2, typename Func3>
void PreferencesDialog::MapSettingToGroupBox(QGroupBox *groupBox, Func1 getter, Func2 setter, Func3 notifier) const
{
    // Get the value and set the checkbox state
    groupBox->setChecked(std::bind(getter, settings)());

    // Set up two way connection
    connect(settings, notifier, groupBox, &QGroupBox::setChecked);
    connect(groupBox, &QGroupBox::toggled, settings, setter);
}

void PreferencesDialog::populateTranslationComboBox()
{
    NotepadNextApplication *app = qobject_cast<NotepadNextApplication *>(qApp);

    // Add the system default at the top
    ui->comboBoxTranslation->addItem(tr("<System Default>"), QStringLiteral(""));

    // TODO: sort this list and keep the system default at the top
    for (const auto &localeName : app->getTranslationManager()->availableTranslations())
    {
        QLocale locale(localeName);
        const QString localeDisplay = TranslationManager::FormatLocaleTerritoryAndLanguage(locale);
        ui->comboBoxTranslation->addItem(localeDisplay, localeName);
    }

    // Select the current one
    int index = ui->comboBoxTranslation->findData(settings->translation());
    if (index != -1) {
        ui->comboBoxTranslation->setCurrentIndex(index);
    }
}
