#ifndef PREFERENCESCATEGORYITEM_H
#define PREFERENCESCATEGORYITEM_H

#include <QWidget>

class ApplicationSettings;

class PreferencesCategoryItem
{
public:
    PreferencesCategoryItem() = default;
    virtual ~PreferencesCategoryItem() = default;

    virtual QString title() const = 0;
    virtual QString iconPath() const = 0;
    /// @warning Caller becomes owner.
    virtual QWidget *contentView(ApplicationSettings *settings) const = 0;
};

#endif // PREFERENCESCATEGORYITEM_H
