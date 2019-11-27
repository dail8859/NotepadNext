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


#include <QMenu>
#include <QPropertyAnimation>

#include "TabbedEditor.h"
#include "BufferView.h"
#include "NppTabBar.h"

Q_DECLARE_METATYPE(BufferView*)


TabbedEditor::TabbedEditor(QWidget *parent) :
    QWidget(parent),
    tabBar(new NppTabBar()),
    editor(new ScintillaNext())
{
    QVBoxLayout *verticalLayout = new QVBoxLayout();

    verticalLayout->setSpacing(0);
    verticalLayout->setContentsMargins(0, 0, 0, 0);
    verticalLayout->addWidget(tabBar);
    verticalLayout->addWidget(editor);

    setLayout(verticalLayout);

    connect(tabBar, &QTabBar::currentChanged, this, &TabbedEditor::forceSwitchToIndex);
    connect(tabBar, &QTabBar::tabMoved, [=](){ ignoreNextTabIndexChanged = true;});

    // The icons are 16x16 but Notepad++ squeezes them a bit to give a not-so-crisp look...
    tabBar->setIconSize(QSize(13, 13));

    // Force a switch the very first time a document is loaded
    forceSwitch = true;
}

TabbedEditor::~TabbedEditor()
{
    // Let the application take care of removing the buffers
    Q_ASSERT(tabBar->count() == 0);
}

int TabbedEditor::count() const
{
    return tabBar->count();
}

int TabbedEditor::currentIndex() const
{
    return tabBar->currentIndex();
}

ScintillaBuffer *TabbedEditor::getCurrentBuffer()
{
    return getBufferFromIndex(currentIndex());
}

void TabbedEditor::switchToIndex(int index)
{
    if (ignoreNextTabIndexChanged) {
        //qInfo("TabbedEditor::switchToIndex(%d) - Ignoring this index change", index);
        ignoreNextTabIndexChanged = false;
        previousIndex = index;
        return;
    }

    if (previousIndex != INVALID_INDEX) {
        tabBar->tabData(previousIndex).value<BufferView*>()->saveView(editor);
    }

    if (index == tabBar->currentIndex()) {
        if (!forceSwitch) {
            //qInfo("TabbedEditor::switchToIndex(%d) - not switching to already active doc", index);
            return;
        }
        forceSwitch = false;
    }

    ScintillaBuffer *buffer = Q_NULLPTR;
    if (index == INVALID_INDEX) {
        // All documents have been removed so give it a blank document
        editor->setDocPointer(0);
    }
    else {
        buffer = getBufferFromIndex(index);
        editor->setDocPointer(reinterpret_cast<sptr_t>(buffer->pointer()));
    }

    previousIndex = index;

    tabBar->blockSignals(true);
    tabBar->setCurrentIndex(index);
    tabBar->blockSignals(false);

    if (buffer != Q_NULLPTR) {
        tabBar->tabData(index).value<BufferView*>()->restoreView(editor);
        emit bufferSwitched(buffer);

        // Force a signal to get emitted from the editor
        emit editor->updateUi(SC_UPDATE_CONTENT | SC_UPDATE_SELECTION);
    }
}

void TabbedEditor::switchToBuffer(const ScintillaBuffer *buffer)
{
    Q_ASSERT(buffer != Q_NULLPTR);

    switchToIndex(getIndexFromBuffer(buffer));
}

ScintillaBuffer *TabbedEditor::getBufferFromIndex(int index)
{
    Q_ASSERT(index != INVALID_INDEX);

    QVariant var = tabBar->tabData(index);
    return var.value<BufferView*>()->getBuffer();
}

int TabbedEditor::getIndexFromBuffer(const ScintillaBuffer *buffer)
{
    Q_ASSERT(buffer != Q_NULLPTR);

    for (int i = 0; i < tabBar->count(); ++i) {
        // TODO: this is probably not the best way to compare them
        if (buffer == getBufferFromIndex(i)) {
            return i;
        }
    }

    // Something bad happend, trigger the assert
    Q_ASSERT(false);

    return INVALID_INDEX;
}

void TabbedEditor::addBuffer(ScintillaBuffer *buffer)
{
    qInfo(Q_FUNC_INFO);

    Q_ASSERT(buffer != Q_NULLPTR);

    // NOTE: turn off signals when a tab is created. This causes a QTabBar::currentChanged
    // signal, however the tab doesn't have any tabData() associated with it, so any slots
    // won't know what to do with it
    tabBar->blockSignals(true);

    QString iconPath;
    if (buffer->is_read_only())
        iconPath = ":/icons/readonly.png";
    else
        iconPath = ":/icons/saved.png";

    int newIndex = tabBar->addTab(QIcon(iconPath), buffer->getName());
    tabBar->blockSignals(false);


    QVariant var = QVariant::fromValue(new BufferView(buffer));
    tabBar->setTabData(newIndex, var);

    if (buffer->isFile()) {
        tabBar->setTabToolTip(newIndex, buffer->fileInfo.canonicalFilePath());
    }
    else {
        tabBar->setTabToolTip(newIndex, buffer->getName());
    }

    connect(buffer, &ScintillaBuffer::save_point, this, &TabbedEditor::setBufferSaveIcon);
}

void TabbedEditor::removeBuffer(ScintillaBuffer *buffer)
{
    Q_ASSERT(buffer != Q_NULLPTR);

    int index = getIndexFromBuffer(buffer);

    // If a document to the left is closed, the current index
    // will change but doesn't need switched to so ignore it
    if (index < currentIndex())
        ignoreNextTabIndexChanged = true;

    // If the current document is being removed, reset the previous
    // index so it is not referenced
    if (index == currentIndex())
        previousIndex = INVALID_INDEX;

    // Clean up the data that was originally associated with the tab
    delete tabBar->tabData(index).value<BufferView*>();

    tabBar->removeTab(index);

    if (tabBar->count() == 0) {
        // Force a switch the next time a buffer is switched to
        forceSwitch = true;
        emit allBuffersClosed();
    }

    /*
    QPropertyAnimation *prop = new QPropertyAnimation(tabBar, "maximumHeight");
    prop->setDuration(500);
    prop->setStartValue(tabBar->height());
    prop->setEndValue(0);
    prop->setEasingCurve(QEasingCurve::OutBounce);
    prop->start();
    */
}

void TabbedEditor::renamedBuffer(ScintillaBuffer *buffer)
{
    Q_ASSERT(buffer != Q_NULLPTR);

    int index = getIndexFromBuffer(buffer);
    tabBar->setTabText(index, buffer->getName());

    if (buffer->isFile()) {
        tabBar->setTabToolTip(index, buffer->fileInfo.canonicalFilePath());
    }
    else {
        tabBar->setTabToolTip(index, buffer->getName());
    }
}

void TabbedEditor::setBufferSaveIcon(bool atSavePoint)
{
    ScintillaBuffer *buffer = qobject_cast<ScintillaBuffer *>(sender());

    int index = getIndexFromBuffer(buffer);

    //if (buffer->is_read_only())
    //    tabBar->setTabIcon(index, QIcon(":/icons/readonly.png"));
    //else
    if (atSavePoint)
        tabBar->setTabIcon(index, QIcon(":/icons/saved.png"));
    else
        tabBar->setTabIcon(index, QIcon(":/icons/unsaved.png"));
}
