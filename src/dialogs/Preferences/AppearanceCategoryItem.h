#ifndef APPEARANCECATEGORYITEM_H
#define APPEARANCECATEGORYITEM_H

#include "PreferencesCategoryItem.h"

class AppearanceCategoryItem : public PreferencesCategoryItem
{
public:
    AppearanceCategoryItem() = default;
    virtual ~AppearanceCategoryItem() = default;

    virtual QString title() const override { return QObject::tr("Appearance"); }
    virtual QString iconPath() const override { return "://icons/paintbrush.svg"; }
    virtual QWidget *contentView(ApplicationSettings *settings) const override;
};

#endif // APPEARANCECATEGORYITEM_H
