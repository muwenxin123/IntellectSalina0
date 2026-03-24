#ifndef CDLGSCANSYNERGY02_H
#define CDLGSCANSYNERGY02_H

#include <QWidget>
#include "LyDialog.h"

namespace Ui
{
class CDlgScanSynergy02;
}

class CDlgScanSynergy02 : public LyDialog
{
    Q_OBJECT

public:
    explicit CDlgScanSynergy02(QWidget *parent = nullptr);
    ~CDlgScanSynergy02();
public:
    void SetLineItemText(int index, const QString &text);
    void InitWindow();
private slots:
    void on_comboBox_currentIndexChanged(const QString &arg1);

    void on_lineEdit_textChanged(const QString &arg1);

    void on_lineEdit_2_textChanged(const QString &arg1);

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::CDlgScanSynergy02 *ui;
private:
    QPixmap		m_Icon01;
    QPixmap		m_Icon02;
    QPixmap		m_Icon03;
};

#endif
