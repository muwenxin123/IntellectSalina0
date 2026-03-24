#ifndef CMD_H
#define CMD_H

#include <QWidget>

namespace Ui
{
class Cmd;
}

class Cmd : public QWidget
{
    Q_OBJECT

public:
    explicit Cmd(QWidget *parent = nullptr);
    ~Cmd();

private:
    Ui::Cmd *ui;
};

#endif
