#ifndef CMDTAKEBACKCONTROL_H
#define CMDTAKEBACKCONTROL_H

#include <LyDialog.h>

namespace Ui
{
class CmdTakeBackControl;
}

class CmdTakeBackControl : public LyDialog
{
    Q_OBJECT

public:
    explicit CmdTakeBackControl(QWidget *parent = nullptr);
    ~CmdTakeBackControl();

private slots:
    void on_pushButtonOk_clicked();

private:
    Ui::CmdTakeBackControl *ui;
};

#endif
