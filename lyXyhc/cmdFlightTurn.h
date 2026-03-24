#ifndef CMDFLIGHTTURN_H
#define CMDFLIGHTTURN_H

#include <QWidget>

namespace Ui
{
class CmdFlightTurn;
}

class CmdFlightTurn : public QWidget
{
    Q_OBJECT

public:
    explicit CmdFlightTurn(QWidget *parent = nullptr);
    ~CmdFlightTurn();

private slots:
    void on_pushButtonSend_clicked();

private:
    Ui::CmdFlightTurn *ui;
};

#endif
