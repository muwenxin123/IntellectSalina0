#pragma once
#ifndef CXTREEWIDGET_H
#define CXTREEWIDGET_H

#include <QTreeWidget>
#include <QTreeWidgetItem>

#include "genstructdef.h"

class CXTreeWidget: public QTreeWidget
{
    Q_OBJECT
public:
    CXTreeWidget(QWidget *parent = nullptr);
    virtual ~CXTreeWidget();

    virtual QTreeWidgetItem *NewLineItem(char name[]);
    QTreeWidgetItem *NewFolderItem(char name[]);
    QTreeWidgetItem *GetFolderParent(char name[]);
    bool GetFolder_Name(const char all[], char fold[], char name[]);

    virtual void InitTree();

    void setChildCheckState(QTreeWidgetItem *item, Qt::CheckState cs);
    void setParentCheckState(QTreeWidgetItem *item);
    bool isTopItem(QTreeWidgetItem *item);

    bool		IsRember(const QString &name);
    void		setItemsCheck();
protected:
    QList<QString> m_RemberArray;
    QList<QTreeWidgetItem * > m_listItemCheck;
private:

};
#endif
