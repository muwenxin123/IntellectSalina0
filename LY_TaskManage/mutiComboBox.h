#ifndef MUTICOMBOBOX_H
#define MUTICOMBOBOX_H

#include <QWidget>
#include <QListWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QAbstractItemView>

class  MutiComboBox : public QComboBox
{
 Q_OBJECT

public:
 MutiComboBox(QWidget *parent = 0);
 MutiComboBox(const QMap<int,QString> &mapData,QWidget *parent = 0);
 ~MutiComboBox();

public:
 void setModelData(const QMap<int,QString> &mapData);
 void setCheckedItems(const QString &data);
 QString getSelectedItemDatas();
 void hidePopup();

private slots:
 void stateChanged(int state);
 void textChanged(const QString &text);

private:
 void clear();

signals:
 void hidePopuped();

private:
 QListWidget *pListWidget;
 QLineEdit *pLineEdit;
 QString strSelectedText;
 bool bSelected;
};

#endif

