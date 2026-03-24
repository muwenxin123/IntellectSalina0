#ifndef CMDTRACK_H
#define CMDTRACK_H

#include <QWidget>

namespace Ui
{
class CmdTrack;
}

class CmdTrack : public QWidget
{
    Q_OBJECT

public:
    explicit CmdTrack(QWidget *parent = nullptr);
    ~CmdTrack();

private:
    Ui::CmdTrack *ui;
};

#endif
