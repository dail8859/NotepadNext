/*
 * This file is part of Notepad Next.
 * Copyright 2022 Justin Dailey
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


#include "EditorPrintPreviewRenderer.h"
#include "ScintillaNext.h"

#include <QPrinter>
#include <QDebug>
#include <QPainter>

EditorPrintPreviewRenderer::EditorPrintPreviewRenderer(ScintillaNext *editor)
    : QObject(), editor(editor)
{
}

void EditorPrintPreviewRenderer::render(QPrinter *printer)
{
    QRectF printableArea(QPointF(0, 0), printer->pageRect(QPrinter::DevicePixel).size());

    qInfo() << "Print from page" << printer->fromPage() << "to" << printer->toPage();
    qInfo() << "Paper:" << printer->paperRect(QPrinter::DevicePixel);
    qInfo() << "Page:" << printer->pageRect(QPrinter::DevicePixel);
    qInfo() << "Printable Area:" << printableArea;

    const int fromPage = printer->fromPage();
    const int toPage = printer->toPage();

    // The printer starts with a valid page initially
    bool needsNewPage = false;

    int startPos = 0;
    int pageNum = 1;

    QPainter painter(printer);

    do {
        bool needsToDraw = (fromPage == 0 && toPage == 0) || (fromPage <= pageNum && pageNum <= toPage);

        if (needsToDraw) {
            if (needsNewPage) {
                printer->newPage();
            }
            else {
                needsNewPage = true;
            }
        }

        startPos = editor->formatRange(needsToDraw, printer, printer,
                                    printableArea.toRect(), printer->paperRect(QPrinter::DevicePixel).toRect(),
                                    startPos, editor->length());

        pageNum++;
    } while (startPos < editor->length());
}
