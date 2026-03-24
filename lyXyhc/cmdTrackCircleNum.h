#ifndef CMDTrackCircleNum_H
#define CMDTrackCircleNum_H

#include <QWidget>

namespace Ui
{
class CmdTrackCircleNum;
}

class CmdTrackCircleNum : public QWidget
{
    Q_OBJECT

public:
    explicit CmdTrackCircleNum(QWidget *parent = nullptr);
    ~CmdTrackCircleNum();

private slots:
    void on_pushButtonSend_clicked();

private:
    Ui::CmdTrackCircleNum *ui;
};

#endif
