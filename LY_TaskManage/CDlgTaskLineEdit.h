#ifndef _CDLGTASKLINEEDIT__
#define _CDLGTASKLINEEDIT__

#include <QWidget>
#include "LyDialog.h"
#include <QCString.h>
#include <QPointF>

#include "DataManage/INebulaTaskManageService.h"
#include "DataManage/NebulaTask/NebulaMissionLine.h"

#include "WidgetLineEdit.h"

#include "ui_CDlgTaskLineEdit.h"

class WidgetLineEdit;

namespace Ui
{
class CDlgTaskLineEdit;
}

class CDlgTaskLineEdit : public LyDialog
{
    Q_OBJECT

public:
    CDlgTaskLineEdit(QWidget *parent = Q_NULLPTR);
    ~CDlgTaskLineEdit();

    void initMissionData();
    void initWidgetLines();

    void update();

    void clearTabWidget();

    void AddPointInLine(double lon, double lat, double alt = 30.0);

    bool getIsAddPointStatus()
    {
        return isAddPoint;
    }

    bool getIsDragStatus()
    {
        return isDrag;
    }

    int getActionID()
    {
        return selectedActionID;
    }

    void setComBoxCurText(QString curText);

    QString getComBoxCurText()
    {
        return ui->comboBoxMissionName->currentText();
    }

    void setTabWidgetCurLine(int nLineID);

    WidgetLineEdit *getTabWidgetLine(int nLineID);

    void updateLinePointLL(int nLineID, int nPointID, double dLon, double dLat);

    void showWarmSelectedPoint();

    WidgetLineEdit *getCurWidget();

    void SanveMisssionPlatHXRelation();

    void initFormPlane();

    void initFormSelectPlanes(int curIndex);

    void initForm();

protected:
    void closeEvent(QCloseEvent *event);

public slots:
    void comboBoxCurrentIndexChanged();

    void on_clicked_pBSetScreenCenter();

    void on_clicked_pBDelPoint();

    void on_clicked_pBInsertPoint();

    void on_clicked_pBAddPoint();

    void on_clicked_pBClearPoint();

    void on_clicked_pBCreatOrder();

    void on_clicked_pBSaveInfo();

    void on_clicked_pBDragPoint();

    void tabWidgetChanged(int curIndex);

    void on_clicked_pBEnsure();

    void on_clicked_pBShowOrClose();

    void on_startFormPlane_clicked();
    void on_genFormPlane_clicked();
    void on_isRetainPoints_clicked();

private:
    Ui::CDlgTaskLineEdit *ui;

    CNeubulaMissionVec										m_MissionVec;
    qnzkna::TaskManage::INebulaTaskManageService			*m_pITaskManageService = nullptr;
    qnzkna::AmmunitionManage::IAmmunitionManageService     *m_pAmmunitionManageService = nullptr;

    std::vector<WidgetLineEdit *>							widgetLines;

    int														selectedActionID;

    bool													isAddPoint = false;
    bool													isDrag = false;

    int														AddPointType = 0;

    bool													isForingPlane = false;
    bool													isRetainPoints = false;
    std::vector<int>										havingRoutes;
    std::vector<QPointF>									formPoints;
    std::vector<std::vector<QPointF>>						forms;
};

#endif
