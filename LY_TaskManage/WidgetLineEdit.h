#ifndef _WIDGETLINEEDIT__
#define _WIDGETLINEEDIT__

#include <QWidget>
#include <QString>
#include <QCString.h>

#include "DataManage/NebulaTask/NebulaMissionLine.h"
#include "DataManage/NebulaTask/NebulaRoutePoint.h"

#include "DataManage/INebulaTaskManageService.h"
#include "DataManage/NebulaTask/NebulaMissionLine.h"

#include "ui_WidgetLineEdit.h"
#include "CoordinateTranfer.h"

#include "lY_TaskManage.h"

namespace Ui
{
class WidgetLineEdit;
}

class WidgetLineEdit : public QWidget
{
    Q_OBJECT

public:
    WidgetLineEdit(QWidget *parent = Q_NULLPTR);
    ~WidgetLineEdit();

    void setWindowName(QString Name)
    {
        widgetName = Name;
    }
    QString getWindowName()
    {
        return widgetName;
    }

    void setLineName(QString name);
    QString getLineName()
    {
        return LineName;
    }

    void setPlatName(const QString &platName);

    void setPlatType(QString _platType);
    QString getsetPlatType()
    {
        return ui->lineEditPlatType->text();
    }

    void setTaskLine(CNeubulaMissionLine _line);
    CNeubulaMissionLine getTaskLine()
    {
        return TaskLine;
    }

    QTableWidget *getTableWidget()
    {
        return ui->tableWidget;
    }

    int getLineID();

    void updataTableWidget();

    void UpdateListData_XY54();

    void updateTableHeader_XY54();
    void UpdatePoint_XY54(CNeubulaMissionLinePoint &stPoint, const int nNo);

    void tableWidgetInsertPoint(CNeubulaMissionLinePoint &stPoint, int nNo);

    void UpdateLoaclDistance(const int nRow, const int nColum);

    void updateLineDis();

    void initHeightCtr();

    void initLineIntoItems();

    void delPointN(int N);

    void clearPoints();

    void addPointN(int N, double lon, double lat, double alt = 30.0);

    void updateMissionLineID(int _nID);

    void updateLinePointLonLat(int pointIndex, double Lon, double Lat);

    bool GetShowInfoByLineID(const int nLineID, QString &strTaskID, QString &strPlatformID);

    bool getIsNewLine()
    {
        return isNewLine;
    }
    void setIsNewLine(bool _isNewLine)
    {
        isNewLine = _isNewLine;
    }

    void UpdateMarkCombobox();
    std::string GetMarkIDString();

    std::string GetPlatTypeString();

    void setHoveringTime(int _index, int _time);

private:
    QString		widgetName;
    QString		LineName;
    CNeubulaMissionLine		TaskLine;

    QStringList headerlabels_XY54;

    qnzkna::TaskManage::INebulaTaskManageService			*m_pITaskManageService = nullptr;
    qnzkna::MarkManage::IMarkManageService			*m_pIMarkManageService = nullptr;
    bool						isNewLine = false;

public slots:
    void LineEditLineName_changed(QString newName);

    void on_pB11_clicked();

    void on_pB12_clicked();

    void on_pB13_clicked();

    void on_pB14_clicked();

    void on_pB15_clicked();

    void on_LineName_Changed();

    void on_heigheCtrl_changed();

    void on_character_changed();

    void on_trans_changed();

    void on_itemChanged(QTableWidgetItem *_item);

    void on_MarkSet_Clicked();

private:
    Ui::WidgetLineEdit *ui;
};

#endif
