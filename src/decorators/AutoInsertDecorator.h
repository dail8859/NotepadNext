/*
 * This file is part of Notepad Next.
 * Copyright 2026 Justin Dailey
 *
 * Notepad Next is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#pragma once

#include <QHash>
#include <QSet>

#include "EditorDecorator.h"

class AutoInsertDecorator : public EditorDecorator
{
    Q_OBJECT

public:
    explicit AutoInsertDecorator(ScintillaNext *editor);

    void setParenthesesEnabled(bool enabled);
    void setSquareBracketsEnabled(bool enabled);
    void setCurlyBracesEnabled(bool enabled);
    void setSingleQuotesEnabled(bool enabled);
    void setDoubleQuotesEnabled(bool enabled);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

public slots:
    void notify(const Scintilla::NotificationData *pscn) override;

private:
    struct TrackingState {
        QSet<int> before;
        QSet<int> after;
    };

    int addTrackingUndoAction();
    bool isOpeningCharacterEnabled(char ch) const;
    bool isClosingCharacterEnabled(char ch) const;
    static char matchingClose(char ch);
    static char matchingOpen(char ch);
    void updateEnabled();

    bool parenthesesEnabled = false;
    bool squareBracketsEnabled = false;
    bool curlyBracesEnabled = false;
    bool singleQuotesEnabled = false;
    bool doubleQuotesEnabled = false;
    QSet<int> generatedClosers;
    QHash<int, TrackingState> trackingStates;
    QSet<int> pendingRedoState;
    bool hasPendingRedoState = false;
    int nextTrackingToken = 0x41490000;
};
