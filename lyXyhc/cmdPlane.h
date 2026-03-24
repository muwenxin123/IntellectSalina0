#ifndef CMDPLANE_H
#define CMDPLANE_H

#include <QWidget>

namespace Ui
{
class CmdPlane;
}

class CmdPlane : public QWidget
{
    Q_OBJECT

public:
    explicit CmdPlane(QWidget *parent = nullptr);
    ~CmdPlane();

private slots:
    void on_pushButtonSend_clicked();

private:
    Ui::CmdPlane *ui;
};

#endif
