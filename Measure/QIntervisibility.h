#ifndef QINTERVISIBILITY_H
#define QINTERVISIBILITY_H

#include <QDialog>

namespace Ui
{
class QIntervisibility;
}
typedef struct SHPObject SHPObject;

class QIntervisibility : public QDialog
{
    Q_OBJECT

public:
    explicit QIntervisibility(bool bBiIntervisibility = false, QWidget *parent = nullptr);
    ~QIntervisibility();

    void setLonLat(const QLineF &line);

private slots:
    void on_pushButtonOk_clicked();

public:
    SHPObject *m_pShp[2];

private:
    bool m_bBiIntervisibility;
    Ui::QIntervisibility *ui;
};

#endif
