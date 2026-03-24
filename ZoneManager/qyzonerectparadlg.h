#ifndef QYZONERECTPARADLG_H
#define QYZONERECTPARADLG_H
#pragma once

#include <QDialog>
#include <QButtonGroup>
#include "ui_qyzonerectparadialog.h"

#include "LyDialog.h"

#include "genstructdef.h"

class QYZoneRectParaDlg: public LyDialog
{
    Q_OBJECT
public:
    QYZoneRectParaDlg(QUYU_Zone &zone, QWidget *parent = nullptr, bool isModal = false);
    ~QYZoneRectParaDlg();

    double getValue(const QString &str);

private slots:
    void applyParaToItem();
    void setCurrentParaToGui();
    void OnRadioClickedSlot();

public:
    QUYU_Zone m_zone;
protected:
private:
    Ui::QYZoneRectParaDialog *ui;
    QButtonGroup *m_buttonGroup;
    int m_type;

    int     m_iRoundRadius;
    int		m_iLength;
    int		m_iWidth;
};
#endif

