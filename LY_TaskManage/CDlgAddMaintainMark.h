#pragma once

#include <QWidget>
#include "ui_CDlgAddMaintainMark.h"
#include"LyDialog.h"
using namespace std;
class CDlgAddMaintainMark : public LyDialog
{
    Q_OBJECT

public:
    CDlgAddMaintainMark(QWidget *parent = Q_NULLPTR);
    ~CDlgAddMaintainMark();

    void Init(int index, string strID = "");
    void InitData();
    void SetMarkBaseNature(string strID);
    void SetMarkLocation(double lon, double lat);
private slots:
    void on_btnGetPoint_clicked();
    void on_btnConfirm_clicked();
    void on_btnCancel_clicked();

public:
    int m_DataType = 0;
    bool bScreenPoint = false;
private:
    Ui::CDlgAddMaintainMark *ui;
};
