#ifndef CDLGFIRINGDATA_H
#define CDLGFIRINGDATA_H

#include <QWidget>

namespace Ui
{
class CDlgFiringData;
}

class CDlgFiringData : public QWidget
{
    Q_OBJECT

public:
    explicit CDlgFiringData(QWidget *parent = nullptr);
    ~CDlgFiringData();

private:
    Ui::CDlgFiringData *ui;
};

#endif
