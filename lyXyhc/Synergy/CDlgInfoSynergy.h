#ifndef CDLGINFOSYNERGY_H
#define CDLGINFOSYNERGY_H

#include <QWidget>
#include "LyDialog.h"

namespace Ui
{
class CDlgInfoSynergy;
}

class CDlgInfoSynergy : public LyDialog
{
    Q_OBJECT

public:
    explicit CDlgInfoSynergy(QWidget *parent = nullptr);
    ~CDlgInfoSynergy();
public:
    void SetLineItemText(int index, const QString &text);
    void InitCombobox();

private slots:
    void on_comboBox_2_currentIndexChanged(const QString &arg1);

    void on_comboBox_currentIndexChanged(const QString &arg1);

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::CDlgInfoSynergy *ui;
private:
    QPixmap		m_Icon01;
    QPixmap		m_Icon02;
    QPixmap		m_Icon03;
};

#endif
