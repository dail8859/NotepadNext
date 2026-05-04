#ifndef BEHAVIORCATEGORYITEM_H
#define BEHAVIORCATEGORYITEM_H

#include "PreferencesCategoryItem.h"

class BehaviorCategoryItem : public PreferencesCategoryItem
{
public:
    BehaviorCategoryItem() = default;
    virtual ~BehaviorCategoryItem() = default;

    virtual QString title() const override { return QObject::tr("Behavior"); }
    virtual QString iconPath() const override { return "://icons/audio-waveform.svg"; }
    virtual QWidget *contentView(ApplicationSettings *settings) const override;
};

#endif // BEHAVIORCATEGORYITEM_H
