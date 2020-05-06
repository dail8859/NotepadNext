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


#include "DockedEditor.h"
#include "DockAreaWidget.h"
#include "DockWidgetTab.h"

#include "ScintillaNext.h"

#include <QVariant>

Q_DECLARE_METATYPE(ScintillaBuffer*)

DockedEditor::DockedEditor(QWidget *parent) : QObject(parent)
{
    ads::CDockManager::setConfigFlag(ads::CDockManager::AllTabsHaveCloseButton, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::DockAreaHasCloseButton, false);
    ads::CDockManager::setConfigFlag(ads::CDockManager::DockAreaDynamicTabsMenuButtonVisibility, true);

    m_DockManager = new ads::CDockManager(parent);

    connect(m_DockManager, &ads::CDockManager::dockAreaCreated, [this](ads::CDockAreaWidget* DockArea) {
        qInfo("Registering new DockArea");
        this->connect(DockArea, &ads::CDockAreaWidget::currentChanged, [DockArea, this](int index) {
            auto dockWidget = DockArea->dockWidget(index);
            auto editor = qobject_cast<ScintillaNext *>(dockWidget->widget());

            // Force it to be the active editor to receive input
            editor->grabFocus();

            currentEditor = editor;
            emit editorActivated(editor);
        });
    });
}


ScintillaNext *DockedEditor::getCurrentEditor() const
{
    return currentEditor;
}

ScintillaBuffer *DockedEditor::getCurrentBuffer() const
{
    return currentEditor->scintillaBuffer();
}

int DockedEditor::count() const
{
    int total = 0;

    for (int i = 0; i < m_DockManager->dockAreaCount(); ++i)
        total += m_DockManager->dockArea(i)->dockWidgetsCount();

    return total;
}

QVector<ScintillaBuffer *> DockedEditor::buffers() const
{
    QVector<ScintillaBuffer *> buffers;
    foreach (ads::CDockWidget* dockWidget, m_DockManager->dockWidgetsMap()) {
        auto editor = qobject_cast<ScintillaNext *>(dockWidget->widget());

        buffers.append(editor->scintillaBuffer());
    }

    return buffers;
}

void DockedEditor::switchToBuffer(const ScintillaBuffer *buffer)
{
    foreach (ads::CDockWidget* dockWidget, m_DockManager->dockWidgetsMap()) {
        auto editor = qobject_cast<ScintillaNext *>(dockWidget->widget());

        if (buffer == editor->scintillaBuffer()) {
            dockWidget->raise();
            return;
        }
    }
}

void DockedEditor::dockWidgetCloseRequested()
{
    auto dockWidget = qobject_cast<ads::CDockWidget *>(sender());
    auto editor = qobject_cast<ScintillaNext *>(dockWidget->widget());

    emit editorCloseRequested(editor);
}

void DockedEditor::addBuffer(ScintillaBuffer *buffer)
{
    qInfo(Q_FUNC_INFO);

    Q_ASSERT(buffer != Q_NULLPTR);

    //QString iconPath;
    //if (buffer->is_read_only())
    //    iconPath = ":/icons/readonly.png";
    //else
    //    iconPath = ":/icons/saved.png";

    auto editor = new ScintillaNext(buffer);

    if (currentEditor == Q_NULLPTR) {
        currentEditor = editor;
    }

    emit editorCreated(editor);

    // If the dock manager doesnt have any dock areas this will be the first one.
    // Since the
    //if (m_DockManager->dockAreaCount() == 0) {
    //    emit bufferSwitched(buffer);
    //}

    ads::CDockWidget* dw = new ads::CDockWidget(buffer->getName());
    dw->setWidget(editor);
    dw->setFeature(ads::CDockWidget::DockWidgetFeature::DockWidgetDeleteOnClose, true);
    dw->setFeature(ads::CDockWidget::DockWidgetFeature::CustomCloseHandling, true);
    ads::CDockAreaWidget *area = m_DockManager->addDockWidgetTab(ads::CenterDockWidgetArea, dw);



    connect(dw, &ads::CDockWidget::closeRequested, this, &DockedEditor::dockWidgetCloseRequested);

    //int newIndex = tabBar->addTab(QIcon(iconPath), buffer->getName());
    //tabBar->blockSignals(false);

    if (buffer->isFile()) {
        dw->tabWidget()->setToolTip(buffer->fileInfo.canonicalFilePath());
    }
    else {
        dw->tabWidget()->setToolTip(buffer->getName());
    }

    // NOTE:
    if (area->dockWidgetsCount() == 1) {
        emit area->currentChanged(0);
    }

    //connect(buffer, &ScintillaBuffer::save_point, this, &TabbedEditor::setBufferSaveIcon);
}

void DockedEditor::removeBuffer(ScintillaBuffer *buffer)
{
    foreach (ads::CDockWidget* dockWidget, m_DockManager->dockWidgetsMap()) {
        auto editor = qobject_cast<ScintillaNext *>(dockWidget->widget());

        if (buffer == editor->scintillaBuffer())
            dockWidget->closeDockWidget();
    }
}

void DockedEditor::renamedBuffer(ScintillaBuffer *buffer)
{
    Q_ASSERT(buffer != Q_NULLPTR);

    foreach (ads::CDockWidget* dockWidget, m_DockManager->dockWidgetsMap()) {
        auto editor = qobject_cast<ScintillaNext *>(dockWidget->widget());

        if (buffer == editor->scintillaBuffer()) {
            dockWidget->setObjectName(buffer->getName());

            if (buffer->isFile()) {
                dockWidget->tabWidget()->setToolTip(buffer->fileInfo.canonicalFilePath());
            }
            else {
                dockWidget->tabWidget()->setToolTip(buffer->getName());
            }
        }
    }
}
