#ifndef QYZONEDOTPARADLG_H
#define	QYZONEDOTPARADLG_H

#include <QDialog>
#include "genstructdef.h"
#include "LyDialog.h"

namespace Ui
{
class QYZoneDotParaDialog;
}

class QYZoneDotParaDlg : public LyDialog
{
    Q_OBJECT
public:
    QYZoneDotParaDlg(int index, QUYU_Zone &zone, QWidget *parent = nullptr, bool isModal = false);
    ~QYZoneDotParaDlg();
    enum ZoneType { CIRCLE, PIE, ELLIPSE, POLY, RECTANGLE, CIRCLERING, PIERING, POINT, LINE };
    void initUI();
    double getValue(const QString &str);

    QUYU_Zone getQYZone()
    {
        return m_Zone;
    }

private slots:
    void applyParaToItem();

    void OnAdd();
    void OnDelete();

public:
    QUYU_Zone m_Zone;
    int m_iParaIndex;
protected:
private:
    Ui::QYZoneDotParaDialog *ui;

    int	m_iInnerRadius;
    int	m_iOuterRadius;
    int	m_iStartAngle;
    int m_iEndAngle;

    void resetParaCtrls();
};

#endif
