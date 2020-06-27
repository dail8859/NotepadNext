#ifndef QUICKFINDWIDGET_H
#define QUICKFINDWIDGET_H

#include <QEvent>
#include <QFrame>
#include <QObject>

namespace Ui {
class QuickFindWidget;
}

class FocusOutCatcher : public QObject {
    Q_OBJECT

signals:
    void focusLeft();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override {
        if (event->type() == QEvent::FocusOut) {
            emit focusLeft();
        }
        return QObject::eventFilter(obj, event);
    }
};


class QuickFindWidget : public QFrame
{
    Q_OBJECT

public:
    explicit QuickFindWidget(QWidget *parent = nullptr);
    ~QuickFindWidget();

public slots:
    void performSearch();

private:
    Ui::QuickFindWidget *ui;
};

#endif // QUICKFINDWIDGET_H
