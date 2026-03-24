#ifndef CXQYZONETREEWIDGET_H
#define CXQYZONETREEWIDGET_H

#include "cxtreewidget.h"
#include "qyzonedbmgr.h"

class ZoneManageWidget;

class CXQYZoneTreeWidget: public CXTreeWidget
{
    Q_OBJECT
public:
    CXQYZoneTreeWidget(QWidget *parent = nullptr);
    ~CXQYZoneTreeWidget();

    void setupConnection();
    void initActions();

    virtual void InitTree() override;
    void InsertAllTree();

    bool DeletOneItem(QTreeWidgetItem *item);
    int	 GetAirLineArrID_ByItem(QTreeWidgetItem *item);

    bool DeleteCurItem();
    bool IsRepeat(const QString &str, int *pInd = NULL);
    void RemberSetcheck();

private slots:
    void OnCustomContextMenuRequested(const QPoint &pos);
    void OnItemChangedSlot(QTreeWidgetItem *, int);
    void OnTreeSelectionSlot();

    void displayPara();
    void OnDeleteSlot();
    void NewFolderSlot();
    void ReNamed();
    bool SaveAs();

public:
	ZoneManageWidget *m_pZoneManageWidget;
    QList<QUYU_Zone> m_ZoneArray;
    CQYZoneDBMgr m_dbx;

    QTreeWidgetItem *m_CurItem;
    int m_CurInd;

    QString m_strSelFolderName;
protected:
    void DispAirLinePara(int flag);

    void startDrag(Qt::DropActions supportedActions);
    void dropEvent(QDropEvent *event);

private:
    QMenu *itemPopMenu;
    QMenu *itemFolderPopMenu;
    QMenu *treePopMenu;

    QTreeWidgetItem *m_hItemDragS;

};
#endif
