#ifndef CDLGZONESYNERGY_H
#define CDLGZONESYNERGY_H

#include <QWidget>
#include "LyDialog.h"

namespace Ui
{
class CDlgZoneSynergy;
}

class CDlgZoneSynergy : public LyDialog
{
    Q_OBJECT

public:
    explicit CDlgZoneSynergy(QWidget *parent = nullptr);
    ~CDlgZoneSynergy();

    void SetLineItemText(int index, const QString &text);

private slots:
    void on_comboBox_currentIndexChanged(const QString &arg1);

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::CDlgZoneSynergy *ui;
private:
    QPixmap		m_Icon01;
    QPixmap		m_Icon02;
    QPixmap		m_Icon03;

};

#endif
