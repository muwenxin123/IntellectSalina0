#ifndef CDLGDEBUG_H
#define CDLGDEBUG_H

#include <QWidget>

namespace Ui {
class CDlgDebug;
}

class CDlgDebug : public QWidget
{
    Q_OBJECT

public:
    explicit CDlgDebug(QWidget *parent = nullptr);
    ~CDlgDebug();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

	void on_pushButton_4_clicked();

private:
    Ui::CDlgDebug *ui;
};

#endif // CDLGDEBUG_H
