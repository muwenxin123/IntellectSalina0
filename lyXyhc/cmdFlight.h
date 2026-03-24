#ifndef CMDFLIGHT_H
#define CMDFLIGHT_H

#include <QWidget>

namespace Ui
{
class CmdFlight;
}

class CmdFlight : public QWidget
{
    Q_OBJECT

public:
    explicit CmdFlight(QWidget *parent = nullptr);
    ~CmdFlight();

private:
    Ui::CmdFlight *ui;
};

#endif
