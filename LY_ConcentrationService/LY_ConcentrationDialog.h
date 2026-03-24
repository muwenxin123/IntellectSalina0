#ifndef LY_CONCENTRATIONDIALOG_H
#define LY_CONCENTRATIONDIALOG_H

#include <QWidget>

namespace Ui {
class LY_ConcentrationDialog;
}

class LY_ConcentrationDialog : public QWidget
{
    Q_OBJECT

public:
    explicit LY_ConcentrationDialog(QWidget *parent = nullptr);
    ~LY_ConcentrationDialog();

private:
    Ui::LY_ConcentrationDialog *ui;
};

#endif // LY_CONCENTRATIONDIALOG_H
