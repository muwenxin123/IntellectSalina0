#ifndef CDLGMARKEDIT_H
#define CDLGMARKEDIT_H

#include <QWidget>
#include "lyUI/LyDialog.h"
#include "DataManage/Mark/Mark.h"
#include "Protocol/XygsZC/Bag_XygsZCDefines.h"

#define PICK_POINT_NORMAL       1
#define PICK_POINT_EDIT         2

namespace Ui
{
class CDlgMarkEdit;
}

class CDlgMarkEdit : public LyDialog
{
    Q_OBJECT

public:
    explicit CDlgMarkEdit(QWidget *parent = nullptr);
    ~CDlgMarkEdit();

signals:
    void SetWindowInfoSig(const XygsZC_Mark_01 xygsZC_Mark_01);

private slots:
    void SetWindowInfoSlot(const XygsZC_Mark_01 xygsZC_Mark_01);

private:
    Ui::CDlgMarkEdit *ui;

    int         m_DataType;

    int         m_PickPointStates;

    SystemMarkInfo m_SystemMarkInfo;
public:
    void InitData();
    void InitWindowCtrl();
    void SetWindowInfo(SystemMarkInfo &stInfo);
    void ShowTop();
    void SetDataType(int nType);
    int GetPickPointStates();
    void SetPostion(double &dLon, double &dLat, double &dHeight);
    void InitTimeEdit();
    void UpdateMissionCombobox();
private:
    void showEvent(QShowEvent *event);
    void closeEvent(QCloseEvent *event);

private slots:

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();
};

#endif
