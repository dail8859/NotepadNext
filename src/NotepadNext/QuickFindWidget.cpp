#include "QuickFindWidget.h"
#include "ScintillaNext.h"
#include "ui_QuickFindWidget.h"

#include <QLineEdit>
#include <QShortcut>


QuickFindWidget::QuickFindWidget(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::QuickFindWidget)
{
    ui->setupUi(this);

    this->setFocusProxy(ui->lineEdit);

    auto a = new FocusOutCatcher();
    ui->lineEdit->installEventFilter(a);

    connect(a, &FocusOutCatcher::focusLeft, this, &QuickFindWidget::hide);
    connect(ui->lineEdit, &QLineEdit::textEdited, [=](const QString &) { this->performSearch(); });
    connect(ui->lineEdit, &QLineEdit::returnPressed, this, &QuickFindWidget::performSearch);
    connect(ui->buttonRegexp, &QToolButton::toggled, this, &QuickFindWidget::performSearch);
    connect(ui->buttonMatchCase, &QToolButton::toggled, this, &QuickFindWidget::performSearch);
}

QuickFindWidget::~QuickFindWidget()
{
    delete ui;
}

void QuickFindWidget::performSearch()
{
    ScintillaNext *editor =  qobject_cast<ScintillaNext *>(this->parentWidget());

    // Clear any previously highlighted areas
    editor->setIndicatorCurrent(29);
    editor->indicatorClearRange(0, editor->length());

    QString text = ui->lineEdit->text();

    if (text.length() > 0) {
        int searchFlags = 0;

        if (ui->buttonRegexp->isChecked()) {
            searchFlags |= SCFIND_REGEXP;
        }

        if (ui->buttonMatchCase->isChecked()) {
            searchFlags |= SCFIND_MATCHCASE;
        }

        editor->setSearchFlags(searchFlags);
        editor->forEachMatch(text, [=](int start, int end) {
            editor->indicatorFillRange(start, end - start);
            return end;
        });
    }
}
