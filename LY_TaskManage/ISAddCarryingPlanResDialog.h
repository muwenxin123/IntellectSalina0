#ifndef ISADDCARRYINGPLANRESDIALOG_H
#define ISADDCARRYINGPLANRESDIALOG_H

#include <QWidget>
#include "LyDialog.h"
namespace Ui
{
class ISAddCarryingPlanResDialog;
}
class QTableWidget;

class ISAddCarryingPlanResDialog : public LyDialog
{
    Q_OBJECT

public:
    explicit ISAddCarryingPlanResDialog(QWidget *parent = nullptr);
    ~ISAddCarryingPlanResDialog();

    void Init(int value, QTableWidget *widget);

    void SetAmendInit(QString strName, int  nNum, int nSurPlus, QString strRemark);
private slots:
    void on_pbtnOK_clicked();
    void on_pbtnCancel_clicked();
private:
    QTableWidget *m_tableWidget;

    Ui::ISAddCarryingPlanResDialog *ui;
};

#endif
