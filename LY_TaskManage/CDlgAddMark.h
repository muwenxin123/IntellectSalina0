#ifndef CDLGADDMARK_H
#define CDLGADDMARK_H

#include <QWidget>
#include <QString>
#include"LyDialog.h"
#include "DataManage/Mark/Mark.h"
#include "CDlgActionEdit.h"
namespace Ui
{
class CDlgAddMark;
}

class CDlgAddMark : public LyDialog
{
    Q_OBJECT

public:
    explicit CDlgAddMark(QWidget *parent = nullptr);
    ~CDlgAddMark();
    void Init();

    std::vector<SystemMarkInfo> GetMarkList();
    std::vector<SystemMarkInfo> targetInfoVec;

public slots:

    void on_btnOk_clicked();
    void on_btnCencel_clicked();

    void on_tbwMark_doubleclicked();
private:
    Ui::CDlgAddMark *ui;

};

#endif
