#ifndef CDLGPLANNEDSYNERGY_H
#define CDLGPLANNEDSYNERGY_H

#include <QWidget>
#include "LyDialog.h"

namespace Ui
{
class CDlgPlannedSynergy;
}

class CDlgPlannedSynergy : public LyDialog
{
    Q_OBJECT

public:
    explicit CDlgPlannedSynergy(QWidget *parent = nullptr);
    ~CDlgPlannedSynergy();

public:
    void SetLineItemText(int index, const QString &text);
private slots:
    void on_comboBox_currentIndexChanged(const QString &arg1);

    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

private:
    Ui::CDlgPlannedSynergy *ui;
private:
    QPixmap		m_Icon01;
    QPixmap		m_Icon02;
    QPixmap		m_Icon03;
};

#endif
