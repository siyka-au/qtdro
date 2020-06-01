#ifndef DEVICEMODEL_H
#define DEVICEMODEL_H

#include <QAbstractItemModel>

#include "deviceinfo.h"

class DeviceModel : public QAbstractTableModel
{
public:
    DeviceModel(QObject *parent = nullptr);

    void setCurrencyMap(const QMap<QString, double> &map);
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

private:
    QString currencyAt(int offset) const;

    QMap<QString, DeviceInfo*> deviceMap;
};

#endif // DEVICEMODEL_H
