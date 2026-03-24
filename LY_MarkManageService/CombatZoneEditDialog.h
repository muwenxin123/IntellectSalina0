#ifndef COMBATZONEEDITDIALOG_H
#define COMBATZONEEDITDIALOG_H

#include <QWidget>
#include <QLineEdit>
#include <QMutex>
#include "DataManage/Mark/ZoneInfo.h"
#include "lyUI/LyDialog.h"

#define PICK_AS_POINT_NORMAL       1
#define PICK_AS_POINT_EDIT         2

typedef std::vector<ZonePoint>					ZoneWindowPointVec;
typedef ZoneWindowPointVec::iterator			ZoneWindowPointVecItr;

namespace Ui
{
class CombatZoneEditDialog;
}

class CombatZoneEditDialog : public LyDialog
{
    Q_OBJECT

public:
    explicit CombatZoneEditDialog(QWidget *parent = nullptr);
    ~CombatZoneEditDialog();

protected:
    void changeEvent(QEvent *e);
    virtual void closeEvent(QCloseEvent *event);
    virtual void showEvent(QShowEvent *event);

public:
    void InitWindowCtrl();
    void SetWindowInfo(CZoneInfo &stInfo);
    void SetDataType(int nType);
    void UpdataMarkCombobx();
    int GetPickPointStates();
    void SetPostion(double &dLon, double &dLat, double &dHeight);
    void SetWindowPoint(int &nNo, double &dLon, double &dLat, double &dHeight, double &dRadius);
    void AddWindowPoint(double &dLon, double &dLat, double &dHeight, double &dRadius);
    void RemoveWindowPoint(int &nNo);
    bool GetWindowPointMap(EZoneShape &eStates, ZoneWindowPointVec &wndMap);

    void CreateListHeading();
    void UpdateListData(const ZonePointVec &PointVec);
    void UpdateListData(int &nCount, const ZonePoint &stAirPoint);

private slots:

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_comboBox_CombatZone_Type_currentIndexChanged(int index);

    void on_comboBox_Mark_currentTextChanged(const QString &arg1);

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

private:
    void show_GroupBox_CombatZone(const int type);

private:
    Ui::CombatZoneEditDialog	*ui;

    CZoneInfo					m_ZoneInfo;
    ZoneWindowPointVec			m_ZonePointVec;

    int								m_nType = 1;

    int								m_PickPointStates;

    QMutex							sMutex;
};

#endif
