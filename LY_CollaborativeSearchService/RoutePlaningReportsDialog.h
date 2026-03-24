#ifndef ROUTEPLANINGREPORTSDIALOG_H
#define ROUTEPLANINGREPORTSDIALOG_H

#include <QWidget>

namespace Ui {
class RoutePlaningReportsDialog;
}

class RoutePlaningReportsDialog : public QWidget
{
    Q_OBJECT

public:
    explicit RoutePlaningReportsDialog(QWidget *parent = nullptr);
    ~RoutePlaningReportsDialog();

private:
    Ui::RoutePlaningReportsDialog *ui;
};

#endif // ROUTEPLANINGREPORTSDIALOG_H
