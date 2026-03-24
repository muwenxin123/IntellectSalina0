#ifndef CDLGTIMESYNERGY_H
#define CDLGTIMESYNERGY_H

#include <QWidget>
#include "LyDialog.h"

namespace Ui
{
class CDlgTimeSynergy;
}

class CDlgTimeSynergy : public LyDialog
{
    Q_OBJECT

public:
    explicit CDlgTimeSynergy(QWidget *parent = nullptr);
    ~CDlgTimeSynergy();

    void SetLineItemText(int index, const QString &text);

private slots:
    void on_comboBox_currentIndexChanged(const QString &arg1);

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::CDlgTimeSynergy *ui;

private:
    QPixmap		m_Icon01;
    QPixmap		m_Icon02;
    QPixmap		m_Icon03;
};

#endif
