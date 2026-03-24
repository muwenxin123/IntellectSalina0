#ifndef QVIEWSHED_H
#define QVIEWSHED_H

#include <lyUI/LyDialog.h>

namespace Ui
{
class QViewShed;
}
typedef struct SHPObject SHPObject;

class QViewShed : public LyDialog
{
    Q_OBJECT

public:
    explicit QViewShed(QWidget *parent = nullptr);
    ~QViewShed();

    void setLonLat(double dLon, double dLat);

private slots:
    void on_pushButtonOk_clicked();

public:
    SHPObject *m_pShp;

private:
    Ui::QViewShed *ui;
};

#endif
