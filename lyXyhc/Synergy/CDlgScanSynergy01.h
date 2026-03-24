#ifndef CDLGSCANSYNERGY01_H
#define CDLGSCANSYNERGY01_H

#include <QWidget>
#include "LyDialog.h"

namespace Ui
{
class CDlgScanSynergy01;
}

class CDlgScanSynergy01 : public LyDialog
{
    Q_OBJECT

public:
    explicit CDlgScanSynergy01(QWidget *parent = nullptr);
    ~CDlgScanSynergy01();
public:
    void SetLineItemText(int index, const QString &text);
    void InitEdit();

private slots:
    void on_lineEdit_3_textChanged(const QString &arg1);

    void on_lineEdit_textChanged(const QString &arg1);

    void on_lineEdit_2_textChanged(const QString &arg1);

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::CDlgScanSynergy01 *ui;
private:
    QPixmap		m_Icon01;
    QPixmap		m_Icon02;
    QPixmap		m_Icon03;
};

#endif
