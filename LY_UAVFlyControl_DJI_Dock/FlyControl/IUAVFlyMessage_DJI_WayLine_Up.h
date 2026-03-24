#ifndef IUAVFLYMESSAGE_DJI_WAYLINE_UP_H
#define IUAVFLYMESSAGE_DJI_WAYLINE_UP_H

#include <QTableWidget>
#include <QMenu>
#include <QContextMenuEvent>
#include <QAction>
#include <QMessageBox>
#include <QContextMenuEvent>
#include <QHeaderView>
#include <QGeoCoordinate>
#include "wpml_codec/wpml_codec.h"
#include <QStringList>
namespace Ui {
	class IUAVFlyMessage_DJI_WayLine_Up;
}

class IUAVFlyMessage_DJI_WayLine_Up : public QTableWidget
{
	Q_OBJECT

public:
	explicit IUAVFlyMessage_DJI_WayLine_Up(QWidget *parent = nullptr);

	~IUAVFlyMessage_DJI_WayLine_Up();

	void addNewRow(const double& dlon, const double& dlat);

	QVector<QPair<int, QPointF>> getRoutePointVector();

	std::vector<wcs::WPMLPlacemark> getWaypointInfoKmlPlacemark();

	bool bIsEnable();

	int rowCount_wayline();

	bool setAltitudeAll(const double& altitude);

	bool setSpeed(const double& speed);

	double setCurrentItem_distance(const int& curRow);

	boost::optional<wcs::ActionGroup> setActionGroupParam(const int& row);		//设置航点动作类型

	//wce::ActionActuatorFunc	getActionActuatorFunc(const QString& strAction);

	boost::optional<wce::ActionActuatorFunc> GetActionActuatorFuncFromString(const std::string& str);//param动作类型转换

	boost::optional<wce::ActionTriggerType> GetActionTriggerTypeFromString(const std::string& str);	//param动作触发类型转换

	std::shared_ptr<wcs::ActionActuatorFuncParam> GetActionActuatorFuncParam(const std::string& str,const int& hovertime = 5);//枚举类型配置参数
	
private slots:
	void deleteSelectedRows();
	
	void startEditTable();

	void stopEditTable();

	void on_customContextMenuRequested(const QPoint &pos);

	void menu_uploadRoute();

	void menu_downloadRoute();

signals:
	void signal_uploadRoute();

	void signal_downloadRoute();
private:
	QList<int> getSelectedRows() const;

	void createActions();

	void setupContextMenu();

	void contextMenuEvent(QContextMenuEvent *event) override;
	
private:
	QVector<QPair<int, QPointF>> v_point_vector; //航点序号，航点

	bool bIsTableEnable = false;
	
	QAction *deleteAction;
	QAction* startEdit;
	QAction* stopEdit;
	QAction* uploadRoute;
	QAction* downloadRoute;

	QMenu *contextMenu;

	std::vector<wcs::WPMLPlacemark> m_placemarkVec;

	QStringList actionList;
	QStringList headers;
	QStringList actionTriggerType;
};

#endif // 
