#ifndef CDLGFIRESYNERGY01_H
#define CDLGFIRESYNERGY01_H

#include <QWidget>
#include "LyDialog.h"

namespace Ui
{
class CDlgFireSynergy01;
}

class CDlgFireSynergy01 : public LyDialog
{
    Q_OBJECT

public:
    explicit CDlgFireSynergy01(QWidget *parent = nullptr);
    ~CDlgFireSynergy01();

public:
    void SetLineItemText(int index, const QString &text);

    void InitWindow();

private slots:
    void on_lineEdit_2_textChanged(const QString &arg1);

    void on_lineEdit_textChanged(const QString &arg1);

    void on_comboBox_currentTextChanged(const QString &arg1);

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::CDlgFireSynergy01 *ui;
private:
    QPixmap		m_Icon01;
    QPixmap		m_Icon02;
    QPixmap		m_Icon03;
};

#endif
