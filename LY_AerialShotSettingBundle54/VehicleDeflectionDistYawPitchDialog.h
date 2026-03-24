#ifndef VEHICLEDEFLECTIONDISTYAWPITCHDIALOG_H
#define VEHICLEDEFLECTIONDISTYAWPITCHDIALOG_H

#include <QMenu>
#include <QContextMenuEvent>
#include <QWidget>

namespace Ui
{
class VehicleDeflectionDistYawPitchDialog;
}

class VehicleDeflectionDistYawPitchDialog : public QWidget
{
    Q_OBJECT

public:
    explicit VehicleDeflectionDistYawPitchDialog(QWidget *parent = nullptr);
    ~VehicleDeflectionDistYawPitchDialog();

    static void readVehicleDeflectionDistYawPitchData();
    static void writeVehicleDeflectionDistYawPitchData();

    void setVehicleIDAndName(const std::string &strVehicleID, const std::string &strVehicleName);
    void setVehicleDeflectionDistYawPitchMap(const std::vector< std::pair<unsigned int, std::list< std::pair<unsigned int, std::pair<double, double> > > > > &vehicleDeflectionDistYawPitchMap);

protected:
    virtual void changeEvent(QEvent *e) override;
#ifndef QT_NO_CONTEXTMENU
    virtual void contextMenuEvent(QContextMenuEvent *event) override;
#endif
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void closeEvent(QCloseEvent *event) override;

private slots:
    void on_pushButton_Save_clicked();

    void on_pushButton_Cancel_clicked();

private:
    void handleCustomContextMenuRequested(const QPoint &pos);

private:
    Ui::VehicleDeflectionDistYawPitchDialog *ui;
    QWidget                                 *m_ParentWidget;
    QMenu                                   *m_TableMenu;
    std::string                             m_currentComponentID;
    std::string                             m_currentComponentName;
};

#endif
