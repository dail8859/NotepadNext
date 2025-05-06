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


#include "FindReplaceDialog.h"
#include "ApplicationSettings.h"
#include "ui_FindReplaceDialog.h"

#include <QStatusBar>
#include <QLineEdit>
#include <QKeyEvent>

#include "ScintillaNext.h"
#include "MainWindow.h"


static void convertToExtended(QString &str)
{
    str.replace("\\r", "\r");
    str.replace("\\n", "\n");
    str.replace("\\t", "\t");
    str.replace("\\0", "\0");
    str.replace("\\\\", "\\");
    // TODO: more
}

FindReplaceDialog::FindReplaceDialog(ISearchResultsHandler *searchResults, MainWindow *window) :
    QDialog(window, Qt::Dialog),
    ui(new Ui::FindReplaceDialog),
    searchResultsHandler(searchResults),
    finder(new Finder(window->currentEditor()))
{
    qInfo(Q_FUNC_INFO);

    // Turn off the help button on the dialog
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    ui->setupUi(this);

    // Get the current editor, and keep up the reference
    setEditor(window->currentEditor());
    connect(window, &MainWindow::editorActivated, this, &FindReplaceDialog::setEditor);

    tabBar = new QTabBar();
    tabBar->addTab(tr("Find"));
    tabBar->addTab(tr("Replace"));
    tabBar->setExpanding(false);
    qobject_cast<QVBoxLayout *>(layout())->insertWidget(0, tabBar);
    connect(tabBar, &QTabBar::currentChanged, this, &FindReplaceDialog::changeTab);

    statusBar = new QStatusBar();
    statusBar->setSizeGripEnabled(false); // the dialog has one already
    qobject_cast<QVBoxLayout *>(layout())->insertWidget(-1, statusBar);

    // Disable auto completion
    ui->comboFind->setCompleter(nullptr);
    ui->comboReplace->setCompleter(nullptr);

    // If the selection changes highlight the text
    connect(ui->comboFind, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), ui->comboFind->lineEdit(), &QLineEdit::selectAll);
    connect(ui->comboReplace, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), ui->comboReplace->lineEdit(), &QLineEdit::selectAll);

    // Force focus on the find text box
    connect(this, &FindReplaceDialog::windowActivated, [=]() {
        ui->comboFind->setFocus();
        ui->comboFind->lineEdit()->selectAll();
    });

    connect(this, &QDialog::rejected, [=]() {
        statusBar->clearMessage();
        savePosition();
    });

    connect(ui->radioRegexSearch, &QRadioButton::toggled, this, [=](bool checked) {
        ui->checkBoxBackwardsDirection->setDisabled(checked);
        ui->checkBoxMatchWholeWord->setDisabled(checked);
        ui->checkBoxRegexMatchesNewline->setEnabled(checked);
    });

    connect(ui->radioOnLosingFocus, &QRadioButton::toggled, this, &FindReplaceDialog::adjustOpacityWhenLosingFocus);
    connect(ui->radioAlways, &QRadioButton::toggled, this, &FindReplaceDialog::adjustOpacityAlways);
    connect(ui->transparency, &QGroupBox::toggled, this, &FindReplaceDialog::transparencyToggled);

    connect(ui->buttonFind, &QPushButton::clicked, this, &FindReplaceDialog::find);
    connect(ui->buttonCount, &QPushButton::clicked, this, &FindReplaceDialog::count);
    connect(ui->buttonFindAllInCurrent, &QPushButton::clicked, this, [=]() {
        prepareToPerformSearch();

        searchResultsHandler->newSearch(findString());

        findAllInCurrentDocument();

        searchResultsHandler->completeSearch();

        close();
    });
    connect(ui->buttonFindAllInDocuments, &QPushButton::clicked, this, [=]() {
        prepareToPerformSearch();

        searchResultsHandler->newSearch(findString());

        findAllInDocuments();

        searchResultsHandler->completeSearch();

        close();
    });
    connect(ui->buttonReplace, &QPushButton::clicked, this, &FindReplaceDialog::replace);
    connect(ui->buttonReplaceAll, &QPushButton::clicked, this, &FindReplaceDialog::replaceAll);
    connect(ui->buttonReplaceAllInDocuments, &QPushButton::clicked, this, [=]() {
        prepareToPerformSearch(true);

        QString replaceText = replaceString();

        if (ui->radioExtendedSearch->isChecked()) {
            convertToExtended(replaceText);
        }

        int count = 0;
        ScintillaNext *current_editor = editor;
        MainWindow *window = qobject_cast<MainWindow *>(parent());

        for(ScintillaNext *editor : window->editors()) {
            setEditor(editor);
            count += finder->replaceAll(replaceText);
        }

        setEditor(current_editor);

        showMessage(tr("Replaced %Ln matches", "", count), "green");
    });
    connect(ui->buttonClose, &QPushButton::clicked, this, &FindReplaceDialog::close);

    loadSettings();

    changeTab(tabBar->currentIndex());
}

FindReplaceDialog::~FindReplaceDialog()
{
    delete ui;
    delete finder;
}

void FindReplaceDialog::setFindString(const QString &string)
{
    ui->comboFind->setCurrentText(string);
    ui->comboFind->lineEdit()->selectAll();
}

void FindReplaceDialog::setTab(int tab)
{
    tabBar->setCurrentIndex(tab);
}

bool FindReplaceDialog::event(QEvent *event)
{
    if (event->type() == QEvent::WindowActivate) {
        emit windowActivated();
    }
    else if (event->type() == QEvent::WindowDeactivate) {
        emit windowDeactivated();
    }

    return QDialog::event(event);
}

void FindReplaceDialog::showEvent(QShowEvent *event)
{
    qInfo(Q_FUNC_INFO);

    if (!isFirstTime)
        restorePosition();

    isFirstTime = false;

    QDialog::showEvent(event);
}

void FindReplaceDialog::closeEvent(QCloseEvent *event)
{
    saveSettings();

    QDialog::closeEvent(event);
}

static void updateComboList(QComboBox *comboBox, const QString &text)
{
    // Block the signals while it is manipulated
    const QSignalBlocker blocker(comboBox);

    // Remove it if it is in the list, add it to beginning, and select it
    comboBox->removeItem(comboBox->findText(text));
    comboBox->insertItem(0, text);
    comboBox->setCurrentIndex(0);
}

void FindReplaceDialog::updateFindList(const QString &text)
{
    if (!text.isEmpty())
        updateComboList(ui->comboFind, text);
}

void FindReplaceDialog::updateReplaceList(const QString &text)
{
    updateComboList(ui->comboReplace, text);
}

void FindReplaceDialog::find()
{
    qInfo(Q_FUNC_INFO);

    prepareToPerformSearch();

    Sci_CharacterRange range;
    if(!ui->checkBoxBackwardsDirection->isChecked()) {
        range = finder->findNext();
    }
    else{
         range = finder->findPrev();
    }

    if (ScintillaNext::isRangeValid(range)) {
        if (finder->didLatestSearchWrapAround()) {
            showMessage(tr("The end of the document has been reached. Found 1st occurrence from the top."), "green");
        }

        // TODO: Handle zero length matches better
        if (range.cpMin == range.cpMax) {
            qWarning() << "0 length match at" << range.cpMin;
        }

        editor->goToRange(range);
    }
    else {
        showMessage(tr("No matches found."), "red");
    }
}

void FindReplaceDialog::findAllInCurrentDocument()
{
    qInfo(Q_FUNC_INFO);

    bool firstMatch = true;

    QString text = findString();

    finder->setSearchText(text);
    finder->forEachMatch([&](int start, int end){
        // Only add the file entry if there was a valid search result
        if (firstMatch) {
            searchResultsHandler->newFileEntry(editor);
            firstMatch = false;
        }

        const int line = editor->lineFromPosition(start);
        const int lineStartPosition = editor->positionFromLine(line);
        const int lineEndPosition = editor->lineEndPosition(line);
        const int startPositionFromBeginning = start - lineStartPosition;
        const int endPositionFromBeginning = end - lineStartPosition;
        QString lineText = editor->get_text_range(lineStartPosition, lineEndPosition);

        searchResultsHandler->newResultsEntry(lineText, line, startPositionFromBeginning, endPositionFromBeginning);

        return end;
    });
}

void FindReplaceDialog::findAllInDocuments()
{
    qInfo(Q_FUNC_INFO);

    ScintillaNext *current_editor = editor;
    MainWindow *window = qobject_cast<MainWindow *>(parent());

    for(ScintillaNext *editor : window->editors()) {
        setEditor(editor);
        findAllInCurrentDocument();
    }

    setEditor(current_editor);
}

void FindReplaceDialog::replace()
{
    qInfo(Q_FUNC_INFO);

    prepareToPerformSearch();

    QString replaceText = replaceString();

    if (ui->radioExtendedSearch->isChecked()) {
        convertToExtended(replaceText);
    }

    Sci_CharacterRange range = finder->replaceSelectionIfMatch(replaceText);

    if (ScintillaNext::isRangeValid(range)) {
        showMessage(tr("1 occurrence was replaced"), "blue");
    }

    Sci_CharacterRange next_match = finder->findNext();

    if (ScintillaNext::isRangeValid(next_match)) {
        editor->goToRange(next_match);
    }
    else {
        showMessage(tr("No more occurrences were found"), "red");
        ui->comboFind->setFocus();
        ui->comboFind->lineEdit()->selectAll();
    }
}

void FindReplaceDialog::replaceAll()
{
    qInfo(Q_FUNC_INFO);

    prepareToPerformSearch(true);

    QString replaceText = replaceString();

    if (ui->radioExtendedSearch->isChecked()) {
        convertToExtended(replaceText);
    }

    int count = finder->replaceAll(replaceText);
    showMessage(tr("Replaced %Ln matches", "", count), "green");
}

void FindReplaceDialog::count()
{
    qInfo(Q_FUNC_INFO);

    prepareToPerformSearch();

    int total = finder->count();

    showMessage(tr("Found %Ln matches", "", total), "green");
}

void FindReplaceDialog::setEditor(ScintillaNext *editor)
{
    this->editor = editor;

    finder->setEditor(editor);
}

void FindReplaceDialog::performLastSearch()
{
    editor->goToRange(finder->findNext());
}

void FindReplaceDialog::adjustOpacity(int value)
{
    qInfo(Q_FUNC_INFO);

    setWindowOpacity(value * .01);
}

void FindReplaceDialog::transparencyToggled(bool on)
{
    qInfo(Q_FUNC_INFO);

    if (on) {
        if (ui->radioOnLosingFocus->isChecked()) {
            adjustOpacityWhenLosingFocus(true);
            adjustOpacityAlways(false);
        }
        else {
            adjustOpacityWhenLosingFocus(false);
            adjustOpacityAlways(true);
        }
    }
    else {
        adjustOpacityWhenLosingFocus(false);
        adjustOpacityAlways(false);
        adjustOpacity(100);
    }
}

void FindReplaceDialog::adjustOpacityWhenLosingFocus(bool checked)
{
    qInfo(Q_FUNC_INFO);

    if (checked) {
        connect(this, &FindReplaceDialog::windowActivated, [=]() {
            this->adjustOpacity(100);
        });
        connect(this, &FindReplaceDialog::windowDeactivated, [=]() {
            this->adjustOpacity(ui->horizontalSlider->value());
        });
        adjustOpacity(100);
    }
    else {
        disconnect(this, &FindReplaceDialog::windowActivated, nullptr, nullptr);
        disconnect(this, &FindReplaceDialog::windowDeactivated, nullptr, nullptr);
    }
}

void FindReplaceDialog::adjustOpacityAlways(bool checked)
{
    qInfo(Q_FUNC_INFO);

    if (checked) {
        connect(ui->horizontalSlider, &QSlider::valueChanged, this, &FindReplaceDialog::adjustOpacity);
        adjustOpacity(ui->horizontalSlider->value());
    }
    else {
        disconnect(ui->horizontalSlider, &QSlider::valueChanged, this, &FindReplaceDialog::adjustOpacity);
    }
}

void FindReplaceDialog::changeTab(int index)
{
    if (index == 0) {
        ui->labelReplaceWith->setMaximumHeight(0);
        ui->comboReplace->setMaximumHeight(0);
        // The combo box isn't actually "hidden", so adjust the focus policy so it does not get tabbed to
        ui->comboReplace->setFocusPolicy(Qt::NoFocus);

        ui->buttonReplace->hide();
        ui->buttonReplaceAll->hide();
        ui->buttonReplaceAllInDocuments->hide();

        ui->buttonCount->show();
        ui->buttonFindAllInCurrent->show();
        ui->buttonFindAllInDocuments->show();
    }
    else if (index == 1) {
        ui->labelReplaceWith->setMaximumHeight(QWIDGETSIZE_MAX);
        ui->comboReplace->setMaximumHeight(QWIDGETSIZE_MAX);
        ui->comboReplace->setFocusPolicy(Qt::StrongFocus); // Reset its focus policy

        ui->buttonReplace->show();
        ui->buttonReplaceAll->show();
        ui->buttonReplaceAllInDocuments->show();

        ui->buttonCount->hide();
        ui->buttonFindAllInCurrent->hide();
        ui->buttonFindAllInDocuments->hide();
    }

    ui->comboFind->setFocus();
    ui->comboFind->lineEdit()->selectAll();
}

QString FindReplaceDialog::findString()
{
    return ui->comboFind->currentText();
}

QString FindReplaceDialog::replaceString()
{
    return ui->comboReplace->currentText();
}

void FindReplaceDialog::setSearchResultsHandler(ISearchResultsHandler *searchResults)
{
    this->searchResultsHandler = searchResults;
}

void FindReplaceDialog::prepareToPerformSearch(bool replace)
{
    qInfo(Q_FUNC_INFO);

    QString findText = findString();

    updateFindList(findText);
    if (replace) {
        QString replaceText = replaceString();
        updateReplaceList(replaceText);
    }

    statusBar->clearMessage();

    if (ui->radioExtendedSearch->isChecked()) {
        convertToExtended(findText);
        //convertToExtended(replaceText);
    }

    finder->setWrap(ui->checkBoxWrapAround->isChecked());
    finder->setSearchFlags(computeSearchFlags());
    finder->setSearchText(findText);
}

void FindReplaceDialog::loadSettings()
{
    qInfo(Q_FUNC_INFO);

    ApplicationSettings settings;

    settings.beginGroup("FindReplaceDialog");

    restoreGeometry(settings.value("geometry").toByteArray());

    ui->comboFind->addItems(settings.value("RecentSearchList").toStringList());
    ui->comboReplace->addItems(settings.value("RecentReplaceList").toStringList());

    ui->checkBoxBackwardsDirection->setChecked(settings.value("Backwards").toBool());
    ui->checkBoxMatchWholeWord->setChecked(settings.value("WholeWord").toBool());
    ui->checkBoxMatchCase->setChecked(settings.value("MatchCase").toBool());
    ui->checkBoxWrapAround->setChecked(settings.value("WrapAround", true).toBool());

    if (settings.contains("SearchMode")) {
        const QString searchMode = settings.value("SearchMode").toString();
        if (searchMode == "normal")
            ui->radioNormalSearch->setChecked(true);
        else if (searchMode == "extended")
            ui->radioExtendedSearch->setChecked(true);
        else
            ui->radioRegexSearch->setChecked(true);
    }
    ui->checkBoxRegexMatchesNewline->setChecked(settings.value("DotMatchesNewline").toBool());

    ui->transparency->setChecked(settings.value("TransparencyUsed").toBool());
    if (ui->transparency->isChecked()) {
        ui->horizontalSlider->setValue(settings.value("Transparency", 70).toInt());

        if (settings.value("TransparencyMode").toString() == "focus") {
            ui->radioOnLosingFocus->setChecked(true);
        }
        else {
            ui->radioAlways->setChecked(true);
        }
    }

    settings.endGroup();
}

void FindReplaceDialog::saveSettings()
{
    qInfo(Q_FUNC_INFO);

    ApplicationSettings settings;

    settings.beginGroup("FindReplaceDialog");
    settings.remove(""); // clear out any previous keys

    settings.setValue("geometry", saveGeometry());

    QStringList recentSearches;
    for (int i = 0; i < ui->comboFind->count(); ++i) {
        recentSearches << ui->comboFind->itemText(i);
    }
    settings.setValue("RecentSearchList", recentSearches);

    recentSearches.clear();
    for (int i = 0; i < ui->comboReplace->count(); ++i) {
        recentSearches << ui->comboReplace->itemText(i);
    }
    settings.setValue("RecentReplaceList", recentSearches);

    settings.setValue("Backwards", ui->checkBoxBackwardsDirection->isChecked());
    settings.setValue("WholeWord", ui->checkBoxMatchWholeWord->isChecked());
    settings.setValue("MatchCase", ui->checkBoxMatchCase->isChecked());
    settings.setValue("WrapAround", ui->checkBoxWrapAround->isChecked());

    if (ui->radioNormalSearch->isChecked())
        settings.setValue("SearchMode", "normal");
    else if (ui->radioExtendedSearch->isChecked())
        settings.setValue("SearchMode", "extended");
    else if (ui->radioRegexSearch->isChecked())
        settings.setValue("SearchMode", "regex");
    settings.setValue("DotMatchesNewline", ui->checkBoxRegexMatchesNewline->isChecked());

    settings.setValue("TransparencyUsed", ui->transparency->isChecked());
    if (ui->transparency->isChecked()) {
        settings.setValue("Transparency", ui->horizontalSlider->value());
        settings.setValue("TransparencyMode", ui->radioOnLosingFocus->isChecked() ? "focus" : "always");
    }

    settings.endGroup();
}

void FindReplaceDialog::savePosition()
{
    qInfo(Q_FUNC_INFO);

    lastClosedPosition = pos();
}

void FindReplaceDialog::restorePosition()
{
    qInfo(Q_FUNC_INFO);

    ApplicationSettings settings;

    if (settings.centerSearchDialog()) {
        const QPoint centerPoint = parentWidget()->geometry().center();
        move(centerPoint - rect().center());
    }
    else {
        move(lastClosedPosition);
    }
}

int FindReplaceDialog::computeSearchFlags()
{
    int flags = 0;

    if (ui->checkBoxMatchWholeWord->isChecked())
        flags |= SCFIND_WHOLEWORD;
    if (ui->checkBoxMatchCase->isChecked())
        flags |= SCFIND_MATCHCASE;
    if (ui->radioRegexSearch->isChecked())
        flags |= SCFIND_REGEXP;

    return flags;
}

void FindReplaceDialog::showMessage(const QString &message, const QString &color)
{
    statusBar->setStyleSheet(QStringLiteral("color: %1").arg(color));
    statusBar->showMessage(message);
}
