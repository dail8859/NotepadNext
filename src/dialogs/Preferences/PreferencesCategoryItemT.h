#ifndef PREFERENCESCATEGORYITEMT_H
#define PREFERENCESCATEGORYITEMT_H

#include "PreferencesCategoryItem.h"

template<typename T>
class PreferencesCategoryItemT : public PreferencesCategoryItem
{
public:
    PreferencesCategoryItemT(const QString &title, const QString &icon)
        : PreferencesCategoryItem(), mTitle(title), mIcon(icon)
    { }
    virtual ~PreferencesCategoryItemT() = default;

    inline virtual QString title() const override { return mTitle; }
    inline virtual QString iconPath() const override { return mIcon; }
    inline virtual T *contentView(ApplicationSettings *settings) const override {
        return new T(settings);
    }

private:
    QString mTitle;
    QString mIcon;
};

#endif // PREFERENCESCATEGORYITEMT_H
