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
#include "DockAreaTitleBar.h"

#include "ScintillaNext.h"


DockedEditor::DockedEditor(QWidget *parent) : QObject(parent)
{
    ads::CDockManager::setConfigFlag(ads::CDockManager::AllTabsHaveCloseButton, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::AlwaysShowTabs, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::OpaqueSplitterResize, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::DragPreviewIsDynamic, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::DragPreviewShowsContentPixmap, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::DockAreaHasCloseButton, false);
    ads::CDockManager::setConfigFlag(ads::CDockManager::DockAreaHasUndockButton, false);
    ads::CDockManager::setConfigFlag(ads::CDockManager::DockAreaDynamicTabsMenuButtonVisibility, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::FocusHighlighting, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::EqualSplitOnInsertion, true);

    m_DockManager = new ads::CDockManager(parent);
    m_DockManager->setStyleSheet("");

    connect(m_DockManager, &ads::CDockManager::focusedDockWidgetChanged, [=] (ads::CDockWidget* old, ads::CDockWidget* now) {
        Q_UNUSED(old)

        ScintillaNext *editor = qobject_cast<ScintillaNext *>(now->widget());

        currentEditor = editor;
        editor->grabFocus();
        emit editorActivated(editor);
    });

    connect(m_DockManager, &ads::CDockManager::dockAreaCreated, [=](ads::CDockAreaWidget* DockArea) {
        // Disable the built in context menu for the title bar since it has options we don't want
        DockArea->titleBar()->setContextMenuPolicy(Qt::NoContextMenu);
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

QVector<ScintillaNext *> DockedEditor::editors() const
{
    QVector<ScintillaNext *> editors;
    foreach (ads::CDockWidget* dockWidget, m_DockManager->dockWidgetsMap()) {
        editors.append(qobject_cast<ScintillaNext *>(dockWidget->widget()));
    }

    return editors;
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

ads::CDockAreaWidget * DockedEditor::currentDockArea()
{
    foreach (ads::CDockWidget* dockWidget, m_DockManager->dockWidgetsMap()) {
        if (dockWidget->property("focused").toBool()) {
            return dockWidget->dockAreaWidget();
        }
    }

    return Q_NULLPTR;
}

void DockedEditor::addBuffer(ScintillaBuffer *buffer)
{
    qInfo(Q_FUNC_INFO);

    Q_ASSERT(buffer != Q_NULLPTR);

    // Need a new editor
    auto editor = new ScintillaNext(buffer);

    if (currentEditor == Q_NULLPTR) {
        currentEditor = editor;
    }

    emit editorCreated(editor);

    // Create the dock widget for the editor
    ads::CDockWidget* dw = new ads::CDockWidget(buffer->getName());
    dw->setWidget(editor);
    dw->setFeature(ads::CDockWidget::DockWidgetFeature::DockWidgetDeleteOnClose, true);
    dw->setFeature(ads::CDockWidget::DockWidgetFeature::CustomCloseHandling, true);
    dw->setFeature(ads::CDockWidget::DockWidgetFeature::DockWidgetFloatable, false);

    dw->tabWidget()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(dw->tabWidget(), &QWidget::customContextMenuRequested, [=](const QPoint &pos) {
        Q_UNUSED(pos)

        emit contextMenuRequestedForEditor(editor);
    });

    // Set the tooltip based on the buffer
    if (buffer->isFile()) {
        dw->tabWidget()->setToolTip(buffer->fileInfo.canonicalFilePath());
    }
    else {
        dw->tabWidget()->setToolTip(buffer->getName());
    }

    // Set the icon
    dw->tabWidget()->setIcon(QIcon(":/icons/saved.png"));
    connect(buffer, &ScintillaBuffer::save_point, [=](bool atSavePoint) {
        const QString iconPath = atSavePoint ? ":/icons/saved.png" : ":/icons/unsaved.png";
        dw->tabWidget()->setIcon(QIcon(iconPath));
    });

    connect(dw, &ads::CDockWidget::closeRequested, this, &DockedEditor::dockWidgetCloseRequested);

    m_DockManager->addDockWidget(ads::CenterDockWidgetArea, dw, currentDockArea());
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
