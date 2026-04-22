#include <QVBoxLayout>
#include <QLabel>

#include "BehaviorCategoryItem.h"

QWidget *BehaviorCategoryItem::view() const
{
    const auto widget = new QWidget;
    const auto layout = new QVBoxLayout(widget);

    for (int i = 0; i < 100; ++i)
        layout->addWidget(new QLabel(QString("LABEL %1").arg(i)));

    return widget;
}
