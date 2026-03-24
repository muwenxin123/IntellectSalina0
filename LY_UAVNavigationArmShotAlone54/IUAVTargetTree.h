#ifndef IUAVTARGETTREE_H
#define IUAVTARGETTREE_H

#include <QWidget>
#include <QMenu>
#include "LyDialog.h"
#include "LY_UAVNavigationArmShotAlone54.h"
#include <QTimer>

namespace Ui
{
class IUAVTargetTree;
}
class QTreeWidget;
class QTreeWidgetItem;

class IUAVTargetTree : public LyDialog
{
    Q_OBJECT

public:
    explicit IUAVTargetTree(QWidget *parent = nullptr);
    ~IUAVTargetTree();

    void Init();
    void SetData(std::map<int, QVector<XygsZC_Mark_01>> xygsZC_Taregt, int nChangeVal);
    void ClearTargetList();

    void checkItemStatus(QTreeWidgetItem *item, QStringList &selectedItems);

    void SetMainUAVCommandID(int commandID);

    int GetUnUsedTreeWidget(int value);

    bool GetUAVCommandID_P(QString &strUAV_P);

    bool CheckExistOfCommand_P(const QString &strCommand);

    std::map<QString, QVector<QString>> GetTargetBleadMap()
    {
        return m_TargetBleadMap;
    }

	bool on_MessageBoxWidgetShow();

    void closeEvent(QCloseEvent *event);
protected slots:
    void on_pushButton_clicked();
    void on_treeWidget_2_customContextMenuRequested();
    void on_Menu_Targetblead_Delete();

    void on_pushButton_2_clicked();

    void on_treeWidget_2_currentItemChanged();

    void on_pushButton_3_clicked();

	bool slot_GetIsTargetOnMap();

public:
    QVector<XygsZC_Mark_01> m_AchieveTarget_P;

	bool isTargetOnMap = false;
signals:
    void SignalsClearTree();

	bool MessageBoxWidgetShow();

	bool sig_MessageBoxTarget();

public:
	
private:
    QMenu *m_TreeMenuRight;
    Ui::IUAVTargetTree *ui;
    QVector<QTreeWidget *> m_pTreeWidgetVec;

    std::map<int, int> m_pUsedTreeWidgetMap;

    QTimer *m_Timer;
    bool b_SendMsg = false;

    std::map<int, QVector<XygsZC_Mark_01>> m_OldxygsZC_TaregtMap;

    std::map<QString, QVector<QString>>				m_TargetBleadMap;

    QStringList	m_strUAVUsedTarkID;

    QStringList m_strUAVTarkIDList;

    int nMain_UAVCommandID = -1;

    bool b_SetMainCommand = false;
};

#endif
