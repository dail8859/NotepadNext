#ifndef PREFERENCESCATEGORYLISTMODEL_H
#define PREFERENCESCATEGORYLISTMODEL_H

#include <QAbstractListModel>
#include <QPointer>

#include "PreferencesCategoryItem.h"

class PreferencesCategoryListModel : public QAbstractListModel
{
public:
    explicit PreferencesCategoryListModel(QObject *parent = nullptr);
    virtual ~PreferencesCategoryListModel() = default;

    inline virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        return mItems.size();
    }

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    /**
     * @param item Specific item.
     * @param pos Item position. -1 will append to the end.
     * @note takes ownership of the item.
     */
    void addCategory(PreferencesCategoryItem *category, int row = -1);
    void removeCategory(int row);
    PreferencesCategoryItem *category(int row) const;

private:
    std::vector<std::unique_ptr<PreferencesCategoryItem>> mItems;
};

#endif // PREFERENCESCATEGORYLISTMODEL_H
