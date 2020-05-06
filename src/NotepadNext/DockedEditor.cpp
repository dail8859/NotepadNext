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
            auto editorobj = qobject_cast<QObject *>(dockWidget->widget());
            auto var = editorobj->property("ScintillaBufferPointer");

            // Force it to be the active editor to receive input
            editor->grabFocus();

            currentEditor = editor; //var.value<ScintillaBuffer*>();
            emit bufferSwitched(var.value<ScintillaBuffer*>());
        });
    });
}


ScintillaNext *DockedEditor::getCurrentEditor()
{
    return currentEditor;
}

ScintillaBuffer *DockedEditor::getCurrentBuffer()
{
    QObject *editorobj = currentEditor;
    auto var = editorobj->property("ScintillaBufferPointer");
    return var.value<ScintillaBuffer*>();
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
        auto editor = qobject_cast<QObject *>(dockWidget->widget());
        auto var = editor->property("ScintillaBufferPointer");

        buffers.append(var.value<ScintillaBuffer*>());
    }

    return buffers;
}

void DockedEditor::switchToBuffer(const ScintillaBuffer *buffer)
{
    foreach (ads::CDockWidget* dockWidget, m_DockManager->dockWidgetsMap()) {
        auto editor = qobject_cast<QObject *>(dockWidget->widget());
        auto var = editor->property("ScintillaBufferPointer");

        if (buffer == var.value<ScintillaBuffer*>()) {
            dockWidget->raise();
            return;
        }
    }
}

void DockedEditor::dockWidgetCloseRequested()
{
    auto dockWidget = qobject_cast<ads::CDockWidget *>(sender());
    auto editor = qobject_cast<QObject *>(dockWidget->widget());
    auto var = editor->property("ScintillaBufferPointer");

    emit bufferCloseRequested(var.value<ScintillaBuffer*>());
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

    auto editor = new ScintillaNext();
    editor->setDocPointer(reinterpret_cast<sptr_t>(buffer->pointer()));
    QObject *obj = editor;
    QVariant var = QVariant::fromValue(buffer);
    obj->setProperty("ScintillaBufferPointer", var);

    if (currentEditor == Q_NULLPTR) {
        currentEditor = editor;
    }

    emit editorCreated(editor);

    ads::CDockWidget* dw = new ads::CDockWidget(buffer->getName());
    dw->setWidget(editor);
    dw->setFeature(ads::CDockWidget::DockWidgetFeature::DockWidgetDeleteOnClose, true);
    dw->setFeature(ads::CDockWidget::DockWidgetFeature::CustomCloseHandling, true);
    m_DockManager->addDockWidgetTab(ads::CenterDockWidgetArea, dw);

    connect(dw, &ads::CDockWidget::closeRequested, this, &DockedEditor::dockWidgetCloseRequested);

    //int newIndex = tabBar->addTab(QIcon(iconPath), buffer->getName());
    //tabBar->blockSignals(false);

    if (buffer->isFile()) {
        dw->tabWidget()->setToolTip(buffer->fileInfo.canonicalFilePath());
    }
    else {
        dw->tabWidget()->setToolTip(buffer->getName());
    }

    //connect(buffer, &ScintillaBuffer::save_point, this, &TabbedEditor::setBufferSaveIcon);
}

void DockedEditor::removeBuffer(ScintillaBuffer *buffer)
{
    foreach (ads::CDockWidget* dockWidget, m_DockManager->dockWidgetsMap()) {
        auto editor = qobject_cast<QObject *>(dockWidget->widget());
        auto var = editor->property("ScintillaBufferPointer");

        if (buffer == var.value<ScintillaBuffer*>())
            dockWidget->closeDockWidget();
    }
}

void DockedEditor::renamedBuffer(ScintillaBuffer *buffer)
{
    Q_ASSERT(buffer != Q_NULLPTR);

    foreach (ads::CDockWidget* dockWidget, m_DockManager->dockWidgetsMap()) {
        auto editor = qobject_cast<QObject *>(dockWidget->widget());
        auto var = editor->property("ScintillaBufferPointer");

        if (buffer == var.value<ScintillaBuffer*>()) {
            // TODO: ADS needs this feature
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
