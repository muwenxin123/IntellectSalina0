#ifndef DISPOSE_TARGETANDUAVSROUTE_DIALOG_H
#define DISPOSE_TARGETANDUAVSROUTE_DIALOG_H

#include <QDialog>

namespace Ui {
class Dispose_targetAndUAVSRoute_Dialog;
}

class Dispose_targetAndUAVSRoute_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dispose_targetAndUAVSRoute_Dialog(QWidget *parent = nullptr);
    ~Dispose_targetAndUAVSRoute_Dialog();

private:
    Ui::Dispose_targetAndUAVSRoute_Dialog *ui;
};

#endif
