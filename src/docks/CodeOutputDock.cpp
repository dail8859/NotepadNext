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

#include "CodeOutputDock.h"
#include "ui_CodeOutputDock.h"

#include "ScintillaNext.h"

#include <QVBoxLayout>
#include <string>


CodeOutputDock::CodeOutputDock(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::CodeOutputDock)
{
    ui->setupUi(this);

    codeOutput = new ScintillaNext(Q_NULLPTR, this);

    QFrame *line;
    line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    QVBoxLayout *layout;
	layout = new QVBoxLayout;
    layout->addWidget(codeOutput);
    layout->addWidget(line);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    QWidget *widget = new QWidget(this);
    widget->setLayout(layout);
    setWidget(widget);

    codeOutput->setUndoCollection(false);
    codeOutput->setReadOnly(true);
    // codeOutput->setILexer(reinterpret_cast<sptr_t>(CreateLexer("lua")));
    codeOutput->usePopUp(SC_POPUP_NEVER);
    codeOutput->setMarginWidthN(1, codeOutput->textWidth(STYLE_DEFAULT, ">") * 2);
    codeOutput->setMarginTypeN(1, SC_MARGIN_RTEXT);
    codeOutput->styleSetBold(STYLE_LINENUMBER, true);
    codeOutput->setScrollWidth(1);

    setupStyle(codeOutput);

    //codeOutput->styleSetFore(39, 0x0000FF); // For error messages

}

CodeOutputDock::~CodeOutputDock()
{
    delete ui;
}

void CodeOutputDock::executeCommand(const QString cmd)
{
	qInfo("Entered Function");
	std::array<char, 128> buff;
	QString cmdOutput;
	
#ifdef _WIN32
	auto pipe = _popen(cmd.toUtf8().constData(), "r");
#else
	auto pipe = popen(cmd.toUtf8().constData(), "r");
#endif

	if(!pipe) {
		throw std::runtime_error("command execution failed");
	}
	
	while(fgets(buff.data(), buff.size(), pipe)!= nullptr){
		cmdOutput += buff.data();
	}
#ifdef _WIN32
	_pclose(pipe);
#else
	pclose(pipe);
#endif

	writeOutputToDock(cmdOutput);
}

void CodeOutputDock::writeOutputToDock(const QString cmdOutput){
	codeOutput->setReadOnly(false);
	codeOutput->clearAll();
    codeOutput->appendText(cmdOutput.size(), cmdOutput.toUtf8().constData());
	qInfo(cmdOutput.toUtf8().constData());
    codeOutput->setReadOnly(true);
    codeOutput->documentEnd();
}

bool CodeOutputDock::eventFilter(QObject *obj, QEvent *event) {
	return QObject::eventFilter(obj, event);
}


void CodeOutputDock::setupStyle(ScintillaNext *editor)
{
    editor->setEOLMode(SC_EOL_CRLF);

    editor->styleSetFore(STYLE_DEFAULT, 0x000000);
    editor->styleSetBack(STYLE_DEFAULT, 0xFFFFFF);
    editor->styleSetFont(STYLE_DEFAULT, "Courier New");
    editor->styleSetSize(STYLE_DEFAULT, 10);
    editor->styleClearAll();

    // Setup the margins
    editor->setMarginWidthN(0, 0);
    editor->setMarginWidthN(2, 0);
    editor->setMarginWidthN(3, 0);
    editor->setMarginWidthN(4, 0);
}