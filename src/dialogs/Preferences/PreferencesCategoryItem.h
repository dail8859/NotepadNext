#ifndef PREFERENCESCATEGORYITEM_H
#define PREFERENCESCATEGORYITEM_H

#include <QWidget>

class PreferencesCategoryItem
{
public:
    PreferencesCategoryItem() = default;
    virtual ~PreferencesCategoryItem() = default;

    virtual QString title() const = 0;
    virtual QString icon() const = 0;
    virtual QWidget *view() const = 0;
};

#endif // PREFERENCESCATEGORYITEM_H
