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


#include "FileDialogHelpers.h"


/*
 * NOTE: This code is nearly identical to the QFileDialog implementation of several static methods.
 * Since the QDir filter options were not exposed, there was no way to specify QDir::Hidden.
 */


constexpr QDir::Filters default_filters = QDir::AllEntries | QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Files | QDir::Hidden;


QList<QUrl> FileDialogHelpers::getOpenFileUrls(QWidget *parent, const QString &caption, const QString &dir, const QString &filter, QString *selectedFilter, QFileDialog::Options options, const QStringList &supportedSchemes)
{
    QFileDialog dialog(parent, caption, dir, filter);

    dialog.setSupportedSchemes(supportedSchemes);
    dialog.setFilter(default_filters);
    dialog.setOptions(options);
    dialog.setFileMode(QFileDialog::ExistingFiles);

    if (selectedFilter && !selectedFilter->isEmpty())
        dialog.selectNameFilter(*selectedFilter);
    if (dialog.exec() == QDialog::Accepted) {
        if (selectedFilter)
            *selectedFilter = dialog.selectedNameFilter();
        return dialog.selectedUrls();
    }
    return QList<QUrl>();
}

QStringList FileDialogHelpers::getOpenFileNames(QWidget *parent, const QString &caption, const QString &dir, const QString &filter, QString *selectedFilter, QFileDialog::Options options)
{
    const QStringList schemes = QStringList(QStringLiteral("file"));
    const QList<QUrl> selectedUrls = getOpenFileUrls(parent, caption, dir, filter, selectedFilter, options, schemes);
    QStringList fileNames;

    fileNames.reserve(selectedUrls.size());
    for (const QUrl &url : selectedUrls) {
        if (url.isLocalFile() || url.isEmpty())
            fileNames << url.toLocalFile();
        else
            fileNames << url.toString();
    }
    return fileNames;
}

QUrl FileDialogHelpers::getSaveFileUrl(QWidget *parent, const QString &caption, const QString &dir, const QString &filter, QString *selectedFilter, QFileDialog::Options options, const QStringList &supportedSchemes)
{
    QFileDialog dialog(parent, caption, dir, filter);

    dialog.setSupportedSchemes(supportedSchemes);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setFilter(default_filters);
    dialog.setOptions(options);

    if (selectedFilter && !selectedFilter->isEmpty())
        dialog.selectNameFilter(*selectedFilter);
    if (dialog.exec() == QDialog::Accepted) {
        if (selectedFilter)
            *selectedFilter = dialog.selectedNameFilter();
        return dialog.selectedUrls().value(0);
    }
    return QUrl();
}

QString FileDialogHelpers::getSaveFileName(QWidget *parent, const QString &caption, const QString &dir, const QString &filter, QString *selectedFilter, QFileDialog::Options options)
{
    const QStringList schemes = QStringList(QStringLiteral("file"));
    const QUrl selectedUrl = getSaveFileUrl(parent, caption, dir, filter, selectedFilter, options, schemes);
    if (selectedUrl.isLocalFile() || selectedUrl.isEmpty())
        return selectedUrl.toLocalFile();
    else
        return selectedUrl.toString();
}
