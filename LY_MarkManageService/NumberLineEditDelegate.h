#ifndef NUMBERLINEEDITDELEGATE_H
#define NUMBERLINEEDITDELEGATE_H

#include <QStyledItemDelegate>
#include <QLineEdit>

class NumberLineEditDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit NumberLineEditDelegate(QObject *parent = nullptr);

signals:

public slots:

public:

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    void setEditorData(QWidget *editor, const QModelIndex &index) const override;

    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif
