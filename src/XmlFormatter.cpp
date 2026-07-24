/*
 * This file is part of Notepad Next.
 * Copyright 2026 Justin Dailey
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

#include "XmlFormatter.h"

#include <QXmlStreamReader>
#include <QXmlStreamWriter>

bool XmlFormatter::format(const QByteArray &input, QByteArray &output, QString &errorString, int indent)
{
    QXmlStreamReader reader(input);

    QByteArray prolog;
    QByteArray body;
    QXmlStreamWriter writer(&body);
    writer.setAutoFormatting(true);
    writer.setAutoFormattingIndent(indent);

    while (!reader.atEnd()) {
        reader.readNext();

        switch (reader.tokenType()) {
        case QXmlStreamReader::StartDocument: {
            const QString version = reader.documentVersion().isEmpty() ? QStringLiteral("1.0") : reader.documentVersion().toString();

            prolog += QStringLiteral("<?xml version=\"%1\"").arg(version).toUtf8();
            if (!reader.documentEncoding().isEmpty()) {
                prolog += " encoding=\"UTF-8\"";
            }
            if (reader.isStandaloneDocument()) {
                prolog += " standalone=\"yes\"";
            }
            prolog += "?>\n";
            break;
        }
        case QXmlStreamReader::EndDocument:
            break;
        case QXmlStreamReader::StartElement:
            writer.writeStartElement(reader.qualifiedName().toString());
            for (const QXmlStreamNamespaceDeclaration &ns : reader.namespaceDeclarations()) {
                const QString qualifiedName = ns.prefix().isEmpty() ? QStringLiteral("xmlns") : QStringLiteral("xmlns:%1").arg(ns.prefix());
                writer.writeAttribute(qualifiedName, ns.namespaceUri().toString());
            }
            for (const QXmlStreamAttribute &attr : reader.attributes()) {
                writer.writeAttribute(attr.qualifiedName().toString(), attr.value().toString());
            }
            break;
        case QXmlStreamReader::EndElement:
            writer.writeEndElement();
            break;
        case QXmlStreamReader::Characters:
            if (reader.isCDATA()) {
                writer.writeCDATA(reader.text().toString());
            }
            else if (!reader.isWhitespace()) {
                writer.writeCharacters(reader.text().toString());
            }
            break;
        case QXmlStreamReader::Comment:
            writer.writeComment(reader.text().toString());
            break;
        case QXmlStreamReader::ProcessingInstruction:
            writer.writeProcessingInstruction(reader.processingInstructionTarget().toString(), reader.processingInstructionData().toString());
            break;
        case QXmlStreamReader::DTD:
            writer.writeDTD(reader.text().toString());
            break;
        case QXmlStreamReader::EntityReference:
            writer.writeEntityReference(reader.name().toString());
            break;
        default:
            break;
        }
    }

    if (reader.hasError()) {
        errorString = QStringLiteral("%1 (line %2, column %3)")
            .arg(reader.errorString())
            .arg(reader.lineNumber())
            .arg(reader.columnNumber());
        return false;
    }

    if (body.startsWith('\n')) {
        body.remove(0, 1);
    }

    output = prolog + body;
    return true;
}
