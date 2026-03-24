#include "IUAVFlyMessage_DJI_WayLine_Up.h"
#include "QDoubleSpinBox"
#include "QCString.h"
#include <QComboBox>
#include <QVector3D>
IUAVFlyMessage_DJI_WayLine_Up::IUAVFlyMessage_DJI_WayLine_Up(QWidget *parent)
	:  QTableWidget(parent)
{
	connect(this, &QTableWidget::customContextMenuRequested, this, &IUAVFlyMessage_DJI_WayLine_Up::on_customContextMenuRequested);

	actionList << tr("-") << tr("takePhoto") << tr("startRecord") << tr("stopRecord") << tr("focus")
	 << tr("hover") << tr("panoShot");

	actionTriggerType << tr("-") << tr("reachPoint") << tr("betweenAdjacentPoints") << tr("multipleTiming") << tr("multipleDistance");

	headers << tr2("序号") << tr2("经度") << tr2("纬度") << tr2("高度") << tr2("速度") << tr("param1") << tr("param2") << tr("param3")
		<< tr2("param4") << tr2("param5") << tr2("param6") << tr2("param7") << tr2("动作触发类型") << tr2("time/distance");// size 14

	 // 设置上下文菜单
	setupContextMenu();

	// 创建动作
	createActions();
}
IUAVFlyMessage_DJI_WayLine_Up::~IUAVFlyMessage_DJI_WayLine_Up()
{
}

// 查找函数
boost::optional<wce::ActionActuatorFunc> IUAVFlyMessage_DJI_WayLine_Up::GetActionActuatorFuncFromString(const std::string& str) {
	auto it = wce::kStringToActionActuatorFunc.find(str);
	if (it != wce::kStringToActionActuatorFunc.end()) {
		return it->second;
	}
	return boost::none;
}

// 查找函数
boost::optional<wce::ActionTriggerType> IUAVFlyMessage_DJI_WayLine_Up::GetActionTriggerTypeFromString(const std::string& str) {
	auto it = wce::kStringToAActionTriggerType.find(str);
	if (it != wce::kStringToAActionTriggerType.end()) {
		return it->second;
	}
	return boost::none;
}

std::shared_ptr<wcs::ActionActuatorFuncParam> IUAVFlyMessage_DJI_WayLine_Up::GetActionActuatorFuncParam(const std::string & str, const int& hovertime)
{
	if (str == "-") {
		return std::shared_ptr<wcs::ActionActuatorFuncParam>();
	}
	else if (str == "takePhoto" ||
		str == "startRecord") {
		wcs::TakePhotoParam param;
		param.payloadPositionIndex = 0;
		param.useGlobalPayloadLensIndex = 1;
		param.fileSuffix = "png";
		std::vector<wce::ImageFormat> imageVec;
		imageVec.push_back(wce::ImageFormat::visable);
		imageVec.push_back(wce::ImageFormat::ir);
		param.payloadLensIndex = imageVec;
		return std::make_shared<wcs::TakePhotoParam>(param);
	}
	else if (str == "stopRecord")
	{
		wcs::StopRecordParam param;
		param.payloadPositionIndex = 0;
		std::vector<wce::ImageFormat> imageVec;
		imageVec.push_back(wce::ImageFormat::visable);
		imageVec.push_back(wce::ImageFormat::ir);
		param.payloadLensIndex = imageVec;
		return std::make_shared<wcs::StopRecordParam>(param);
	}
	else if (str == "focus")
	{
		wcs::FocusParam param;
		param.payloadPositionIndex = 0;
		param.isPointFocus = 1;
		param.focusX = 0.5;
		param.focusY = 0.5;
		param.isInfiniteFocus = 0;
		return std::make_shared<wcs::FocusParam>(param);
	}
	else if (str == "hover")
	{
		wcs::HoverParam param;
		param.hoverTime = hovertime;
		return std::make_shared<wcs::HoverParam>(param);
	}
	else if (str == "panoShot")
	{
		wcs::PanoShotParams param;
		param.payloadPositionIndex = 0;
		std::vector<wce::ImageFormat> imageVec;
		imageVec.push_back(wce::ImageFormat::visable);
		imageVec.push_back(wce::ImageFormat::ir);
		param.useGlobalPayloadLensIndex = 1;
		param.panoShotSubMode = wce::PanoShotSubMode::panoShot_360;
		return std::make_shared<wcs::PanoShotParams>(param);
	}
	
}

bool IUAVFlyMessage_DJI_WayLine_Up::bIsEnable()
{
	return bIsTableEnable;
}

std::vector<wcs::WPMLPlacemark> IUAVFlyMessage_DJI_WayLine_Up::getWaypointInfoKmlPlacemark()
{
	m_placemarkVec.clear();
	for (int row = 0;row < rowCount();++row)
	{
		wcs::WPMLPlacemark placemark;
		QTableWidgetItem *widgetitem = item(row, 0);
		if (widgetitem) {
			double nindex = widgetitem->text().toInt();
			placemark.index = nindex;
		}
		widgetitem = item(row, 1);
		wcs::CPoint itemPoint;
		if (widgetitem) {
			double dlon = widgetitem->text().toDouble();
			itemPoint.lon = dlon;
		}
		widgetitem = item(row, 2);
		if (widgetitem) {
			double dlat = widgetitem->text().toDouble();
			itemPoint.lat = dlat;
		}

		QWidget *widget = cellWidget(row, 3);
		if (widget)
		{
			if (QDoubleSpinBox *spin = qobject_cast<QDoubleSpinBox*>(widget))
			{
				double alt = spin->value();
				placemark.executeHeight = alt;
			}
		}
		placemark.point = itemPoint;

		widget = cellWidget(row, 4);
		if (widget)
		{
			if (QDoubleSpinBox *spin = qobject_cast<QDoubleSpinBox*>(widget))
			{
				double speed = spin->value();
				placemark.waypointSpeed = speed;
			}
		}
		boost::optional<wcs::ActionGroup> ActionGroup = setActionGroupParam(row);

		placemark.useStraightLine = 1;
		placemark.waypointHeadingParam.waypointHeadingMode = wce::WaypointHeadingMode::followWayline;
		placemark.waypointHeadingParam.waypointHeadingPathMode = wce::WaypointHeadingPathMode::followBadArc;
		placemark.waypointTurnParam.waypointTurnMode = wce::WaypointTurnMode::toPointAndStopWithDiscontinuityCurvature;
		placemark.waypointTurnParam.waypointTurnDampingDist = 1;
		placemark.startActionGroup = ActionGroup;
		m_placemarkVec.push_back(placemark);
	}

	return m_placemarkVec;
}

boost::optional<wcs::ActionGroup> IUAVFlyMessage_DJI_WayLine_Up::setActionGroupParam(const int& row)
{
	//获取最后一列时间
	QWidget* timeWidget = cellWidget(row, headers.size() - 1);
	int hoverTime = 5;//默认悬停5秒
	if (QSpinBox *spinbox = qobject_cast<QSpinBox*>(timeWidget))
	{
		hoverTime = spinbox->value();
	}
	boost::optional<wcs::ActionGroup> ActionGroup;

	QWidget* actionType = cellWidget(row, headers.size() - 2);
	if (QComboBox* actionTypeCombo = qobject_cast<QComboBox*>(actionType))
	{
		QString typecombo = actionTypeCombo->currentText();
		boost::optional<wce::ActionTriggerType> triggerType = GetActionTriggerTypeFromString(typecombo.toStdString());
		if (triggerType) {
			ActionGroup->actionTrigger.actionTriggerType = triggerType.value();
		}
	}

	ActionGroup->actionGroupId = row;
	ActionGroup->actionGroupStartIndex = row; //需要修改录像模式 开始执行和结束执行参数
	ActionGroup->actionGroupEndIndex = row;//需要修改录像模式 开始执行和结束执行参数
	ActionGroup->actionGroupMode = wce::ActionGroupMode::sequence;
	ActionGroup->actionTrigger.actionTriggerParam = 1;/* 注：当“actionTriggerType”为“multipleTiming”时，该元素表示间隔时间，单位是s。当“actionTriggerType”为“multipleDistance”时，该元素表示间隔距离，单位是m。*/
	std::vector<wcs::Action> actionVec;

	auto createActionFromCombo = [this](QWidget* widget, int actionId) -> wcs::Action {
		wcs::Action action;
		action.actionId = actionId;
		if (QComboBox *combo = qobject_cast<QComboBox*>(widget))
		{
			QString text = combo->currentText();
			boost::optional<wce::ActionActuatorFunc> actionFunc = GetActionActuatorFuncFromString(text.toStdString());

			if (actionFunc) {
				action.actionActuatorFunc = actionFunc.value();  // 安全获取值
			}

			std::shared_ptr<wcs::ActionActuatorFuncParam> actionFuncParam = GetActionActuatorFuncParam(text.toStdString());
			action.actionActuatorFuncParam = actionFuncParam;

			return action;
		}
	};

	QWidget* widget = cellWidget(row, 5);
	if (widget) {
		wcs::Action actionParam = createActionFromCombo(widget, 0);
		actionVec.push_back(actionParam);
	}
	widget = cellWidget(row, 6);
	if (widget) {
		wcs::Action actionParam = createActionFromCombo(widget, 1);
		actionVec.push_back(actionParam);
	}
	widget = cellWidget(row, 7);
	if (widget) {
		wcs::Action actionParam = createActionFromCombo(widget, 2);
		actionVec.push_back(actionParam);
	}
	widget = cellWidget(row, 8);
	if (widget) {
		wcs::Action actionParam = createActionFromCombo(widget, 3);
		actionVec.push_back(actionParam);
	}
	widget = cellWidget(row, 9);
	if (widget) {
		wcs::Action actionParam = createActionFromCombo(widget, 4);
		actionVec.push_back(actionParam);
	}
	widget = cellWidget(row, 10);
	if (widget) {
		wcs::Action actionParam = createActionFromCombo(widget, 5);
		actionVec.push_back(actionParam);
	}
	widget = cellWidget(row, 11);
	if (widget) {
		wcs::Action actionParam = createActionFromCombo(widget, 6);
		actionVec.push_back(actionParam);
	}
	ActionGroup->action = actionVec;

	return ActionGroup;
}

int IUAVFlyMessage_DJI_WayLine_Up::rowCount_wayline()
{
	return rowCount();
}

void IUAVFlyMessage_DJI_WayLine_Up::contextMenuEvent(QContextMenuEvent * event)
{
}

bool IUAVFlyMessage_DJI_WayLine_Up::setAltitudeAll(const double& altitude)
{
	if (rowCount_wayline() <= 0) {
		return false;
	}
	if (altitude < 2) {
		return false;
	}
	for (int row = 0; row < rowCount_wayline(); ++row) {
		QWidget *widget = cellWidget(row, 3);
		if (QDoubleSpinBox *spin = qobject_cast<QDoubleSpinBox*>(widget)) {
			spin->setValue(altitude);
		}
	}
	return true;
}

bool IUAVFlyMessage_DJI_WayLine_Up::setSpeed(const double& speed)
{
	if (rowCount_wayline() <= 0) {
		return false;
	}
	if (speed < 1) {
		return false;
	}
	for (int row = 0; row < rowCount_wayline(); ++row) {
		QWidget *widget = cellWidget(row, 4);
		if (QDoubleSpinBox *spin = qobject_cast<QDoubleSpinBox*>(widget)) {
			spin->setValue(speed);
		}
	}
	return true;
}

double IUAVFlyMessage_DJI_WayLine_Up::setCurrentItem_distance(const int& curRow)
{
	if (curRow <= 0 || rowCount_wayline() < curRow + 1) {
		return 0;
	}
	QWidget *widget_lon = cellWidget(curRow, 3);
	double dlon = 0, dlat = 0, dalt = 0, dlon2 = 0, dlat2 = 0, dalt2 = 0;
	if (QDoubleSpinBox *spin = qobject_cast<QDoubleSpinBox*>(cellWidget(curRow, 3))) {
		dlon = spin->value();
	}
	if (QDoubleSpinBox *spin = qobject_cast<QDoubleSpinBox*>(cellWidget(curRow, 4))) {
		dlat = spin->value();
	}
	if (QDoubleSpinBox *spin = qobject_cast<QDoubleSpinBox*>(cellWidget(curRow, 5))) {
		dalt = spin->value();
	}
	if (QDoubleSpinBox *spin = qobject_cast<QDoubleSpinBox*>(cellWidget(curRow - 1, 3))) {
		dlon2 = spin->value();
	}
	if (QDoubleSpinBox *spin = qobject_cast<QDoubleSpinBox*>(cellWidget(curRow - 1, 4))) {
		dlat2 = spin->value();
	}
	if (QDoubleSpinBox *spin = qobject_cast<QDoubleSpinBox*>(cellWidget(curRow - 1, 5))) {
		dalt2 = spin->value();
	}
	QVector3D v_pos(dlon, dlat, dalt);
	float distanceTopoint = v_pos.distanceToPoint(QVector3D(dlon2, dlat2, dalt2));
	return distanceTopoint;

}

void IUAVFlyMessage_DJI_WayLine_Up::startEditTable()
{
	for (int row = 0; row < rowCount_wayline(); ++row) {

		QTableWidgetItem *widgetitem = item(row, 0);
		if (widgetitem) {
			widgetitem->setFlags(widgetitem->flags() | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		}
		widgetitem = item(row, 1);
		if (widgetitem) {
			widgetitem->setFlags(widgetitem->flags() | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		}
		widgetitem = item(row, 2);
		if (widgetitem) {
			widgetitem->setFlags(widgetitem->flags() | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		}

		QWidget *widget = cellWidget(row, 3);
		if (widget)
		{
			if (QDoubleSpinBox *spin = qobject_cast<QDoubleSpinBox*>(widget))
			{
				spin->setEnabled(true);
			}
		}
		widget = cellWidget(row, 4);
		if (widget)
		{
			if (QDoubleSpinBox *spin = qobject_cast<QDoubleSpinBox*>(widget))
			{
				spin->setEnabled(true);
			}
		}
		widget = cellWidget(row, 5);
		if (widget)
		{
			if (QComboBox *combo = qobject_cast<QComboBox*>(widget))
			{
				combo->setEnabled(true);
			}
		}
		widget = cellWidget(row, 6);
		if (widget)
		{
			if (QComboBox *combo = qobject_cast<QComboBox*>(widget))
			{
				combo->setEnabled(true);
			}
		}
		widget = cellWidget(row, 7);
		if (widget)
		{
			if (QComboBox *combo = qobject_cast<QComboBox*>(widget))
			{
				combo->setEnabled(true);
			}
		}
		widget = cellWidget(row, 8);
		if (widget)
		{
			if (QComboBox *combo = qobject_cast<QComboBox*>(widget))
			{
				combo->setEnabled(true);
			}
		}
		widget = cellWidget(row, 9);
		if (widget)
		{
			if (QComboBox *combo = qobject_cast<QComboBox*>(widget))
			{
				combo->setEnabled(true);
			}
		}
		widget = cellWidget(row, 10);
		if (widget)
		{
			if (QComboBox *combo = qobject_cast<QComboBox*>(widget))
			{
				combo->setEnabled(true);  // 禁用
			}
		}
		widget = cellWidget(row, 11);
		if (widget)
		{
			if (QComboBox *combo = qobject_cast<QComboBox*>(widget))
			{
				combo->setEnabled(true);  // 禁用
			}
		}
		widget = cellWidget(row, 12);
		if (widget)
		{
			if (QComboBox *combo = qobject_cast<QComboBox*>(widget))
			{
				combo->setEnabled(true);  // 禁用
			}
		}
		widget = cellWidget(row, 13);
		if (widget)
		{
			if (QSpinBox *spin = qobject_cast<QSpinBox*>(widget))
			{
				spin->setEnabled(true);  // 禁用
			}
		}
	}
	bIsTableEnable = true;
}

void IUAVFlyMessage_DJI_WayLine_Up::stopEditTable()
{
	for (int row = 0; row < rowCount_wayline(); ++row) {

		QTableWidgetItem *widgetitem = item(row, 0);
		if (widgetitem) {
			widgetitem->setFlags(widgetitem->flags() & ~Qt::ItemIsEnabled& ~Qt::ItemIsSelectable);
		}
		widgetitem = item(row, 1);
		if (widgetitem) {
			widgetitem->setFlags(widgetitem->flags() & ~Qt::ItemIsEnabled& ~Qt::ItemIsSelectable);
		}
		widgetitem = item(row, 2);
		if (widgetitem) {
			widgetitem->setFlags(widgetitem->flags() & ~Qt::ItemIsEnabled& ~Qt::ItemIsSelectable);
		}

		QWidget *widget = cellWidget(row, 3);
		if (widget)
		{
			if (QDoubleSpinBox *spin = qobject_cast<QDoubleSpinBox*>(widget))
			{
				spin->setEnabled(false);  // 禁用
			}
		}
		widget = cellWidget(row, 4);
		if (widget)
		{
			if (QDoubleSpinBox *spin = qobject_cast<QDoubleSpinBox*>(widget))
			{
				spin->setEnabled(false);  // 禁用
			}
		}
		widget = cellWidget(row, 5);
		if (widget)
		{
			if (QComboBox *combo = qobject_cast<QComboBox*>(widget))
			{
				combo->setEnabled(false);  // 禁用
			}
		}
		widget = cellWidget(row, 6);
		if (widget)
		{
			if (QComboBox *combo = qobject_cast<QComboBox*>(widget))
			{
				combo->setEnabled(false);  // 禁用
			}
		}
		widget = cellWidget(row, 7);
		if (widget)
		{
			if (QComboBox *combo = qobject_cast<QComboBox*>(widget))
			{
				combo->setEnabled(false);  // 禁用
			}
		}
		widget = cellWidget(row, 8);
		if (widget)
		{
			if (QComboBox *combo = qobject_cast<QComboBox*>(widget))
			{
				combo->setEnabled(false);  // 禁用
			}
		}
		widget = cellWidget(row, 9);
		if (widget)
		{
			if (QComboBox *combo = qobject_cast<QComboBox*>(widget))
			{
				combo->setEnabled(false);  // 禁用
			}
		}
		widget = cellWidget(row, 10);
		if (widget)
		{
			if (QComboBox *combo = qobject_cast<QComboBox*>(widget))
			{
				combo->setEnabled(false);  // 禁用
			}
		}
		widget = cellWidget(row, 11);
		if (widget)
		{
			if (QComboBox *combo = qobject_cast<QComboBox*>(widget))
			{
				combo->setEnabled(false);  // 禁用
			}
		}
		widget = cellWidget(row, 12);
		if (widget)
		{
			if (QComboBox *combo = qobject_cast<QComboBox*>(widget))
			{
				combo->setEnabled(false);  // 禁用
			}
		}
		widget = cellWidget(row,13);
		if (widget)
		{
			if (QComboBox *combo = qobject_cast<QComboBox*>(widget))
			{
				combo->setEnabled(false);  // 禁用
			}
		}

	}
	bIsTableEnable = false;
}

void IUAVFlyMessage_DJI_WayLine_Up::on_customContextMenuRequested(const QPoint & pos)
{
	contextMenu->exec(QCursor::pos());
}

void IUAVFlyMessage_DJI_WayLine_Up::menu_uploadRoute()
{
	emit signal_uploadRoute();
}

void IUAVFlyMessage_DJI_WayLine_Up::menu_downloadRoute()
{
	emit signal_downloadRoute();
}

void IUAVFlyMessage_DJI_WayLine_Up::addNewRow(const double& dlon,const double& dlat)
{
	if (!bIsTableEnable)
		return;
	int row = rowCount_wayline();
	insertRow(row);

	// 创建第一列的复选框项目
	QTableWidgetItem* nIndexItem = new QTableWidgetItem(QString::number(row));
	nIndexItem->setFlags(nIndexItem->flags() ^ Qt::ItemIsEditable);
	setItem(row, 0, nIndexItem);

	QTableWidgetItem* nLonItem = new QTableWidgetItem(QString::number(dlon,'f',6));
	nLonItem->setFlags(nLonItem->flags() ^ Qt::ItemIsEditable);
	setItem(row, 1, nLonItem);

	QTableWidgetItem* nLatItem = new QTableWidgetItem(QString::number(dlat, 'f', 6));
	nLatItem->setFlags(nLatItem->flags() ^ Qt::ItemIsEditable);
	setItem(row, 2, nLatItem);

	QDoubleSpinBox *spinBox_alt = new QDoubleSpinBox();
	// 配置 spinBox 的参数
	spinBox_alt->setRange(2.0, 1500.0);
	spinBox_alt->setDecimals(1);
	spinBox_alt->setValue(20);
	spinBox_alt->setSuffix(" m");
	setCellWidget(row, 3, spinBox_alt);

	QDoubleSpinBox *spinBox_speed = new QDoubleSpinBox();
	// 配置 spinBox 的参数
	spinBox_speed->setRange(1, 15);
	spinBox_speed->setDecimals(0);
	spinBox_speed->setValue(5);
	spinBox_speed->setSuffix(" m/s");
	setCellWidget(row, 4, spinBox_speed);

	QComboBox *combo_param1 = new QComboBox();
	combo_param1->addItems(actionList);
	combo_param1->setCurrentIndex(0); // 设置一个初始值
	setCellWidget(row, 5, combo_param1);

	QComboBox *combo_param2 = new QComboBox();
	combo_param2->addItems(actionList);
	combo_param2->setCurrentIndex(0); // 设置一个初始值
	setCellWidget(row,6, combo_param2);

	QComboBox *combo_param3 = new QComboBox();
	combo_param3->addItems(actionList);
	combo_param3->setCurrentIndex(0); // 设置一个初始值
	setCellWidget(row, 7, combo_param3);

	QComboBox *combo_param4 = new QComboBox();
	combo_param4->addItems(actionList);
	combo_param4->setCurrentIndex(0); // 设置一个初始值
	setCellWidget(row, 8, combo_param4);

	QComboBox *combo_param5 = new QComboBox();
	combo_param5->addItems(actionList);
	combo_param5->setCurrentIndex(0); // 设置一个初始值
	setCellWidget(row, 9, combo_param5);

	QComboBox *combo_param6 = new QComboBox();
	combo_param6->addItems(actionList);
	combo_param6->setCurrentIndex(0); // 设置一个初始值
	setCellWidget(row, 10, combo_param6);

	QComboBox *combo_param7 = new QComboBox();
	combo_param7->addItems(actionList);
	combo_param7->setCurrentIndex(0); // 设置一个初始值
	setCellWidget(row, 11, combo_param7);

	QComboBox *combo_param8 = new QComboBox();
	combo_param8->addItems(actionList);
	combo_param8->setCurrentIndex(0); // 设置一个初始值
	setCellWidget(row, 12, combo_param8);

	QSpinBox *spin_param9 = new QSpinBox();
	spin_param9->setValue(0);
	setCellWidget(row, 13, spin_param9);

	v_point_vector.push_back(QPair<int, QPointF>(row, QPointF(dlon, dlat)));
}

QVector<QPair<int, QPointF>> IUAVFlyMessage_DJI_WayLine_Up::getRoutePointVector()
{
	if (rowCount_wayline() <= 0)
		return QVector<QPair<int, QPointF>>();

	if (bIsTableEnable)
	{
		return v_point_vector;
	}
	else
	{
		return QVector<QPair<int, QPointF>>();
	}
}

void IUAVFlyMessage_DJI_WayLine_Up::createActions()
{
	deleteAction = new QAction(tr2("删除航点"), this);
	startEdit = new QAction(tr2("开始编辑"), this);
	stopEdit = new QAction(tr2("停止编辑"), this);
	uploadRoute = new QAction(tr2("上传航线并获取签名"));
	downloadRoute = new QAction(tr2("下载航线并获取签名"));
	contextMenu = new QMenu(this);

	contextMenu->addAction(startEdit);
	contextMenu->addAction(stopEdit);
	contextMenu->addAction(uploadRoute);
	contextMenu->addAction(downloadRoute);
	contextMenu->addAction(deleteAction);
	// 设置快捷键
	deleteAction->setShortcut(QKeySequence::Delete);

	connect(deleteAction, &QAction::triggered, this, &IUAVFlyMessage_DJI_WayLine_Up::deleteSelectedRows);
	connect(startEdit, &QAction::triggered, this, &IUAVFlyMessage_DJI_WayLine_Up::startEditTable);
	connect(stopEdit, &QAction::triggered, this, &IUAVFlyMessage_DJI_WayLine_Up::stopEditTable);
	connect(uploadRoute, &QAction::triggered, this, &IUAVFlyMessage_DJI_WayLine_Up::menu_uploadRoute);
	connect(downloadRoute, &QAction::trigger, this, &IUAVFlyMessage_DJI_WayLine_Up::menu_downloadRoute);

}

void IUAVFlyMessage_DJI_WayLine_Up::setupContextMenu()
{
	setColumnCount(headers.size());
	setHorizontalHeaderLabels(headers);

	// 设置表格属性
	setSelectionBehavior(QAbstractItemView::SelectRows); 
	setSelectionMode(QAbstractItemView::ExtendedSelection); 
	setAlternatingRowColors(true); // 交替行颜色

	horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	horizontalHeader()->setStretchLastSection(true); 

	setContextMenuPolicy(Qt::CustomContextMenu);

	verticalHeader()->hide();
}

QList<int> IUAVFlyMessage_DJI_WayLine_Up::getSelectedRows() const
{
	QList<int> rows;
	// 获取所有选中的项目
	QList<QTableWidgetItem*> selectedItems = this->selectedItems();
	for (QTableWidgetItem *item : selectedItems) {
		int row = item->row();
		// 避免重复添加同一行
		if (!rows.contains(row)) {
			rows.append(row);
		}
	}
	return rows;
}

void IUAVFlyMessage_DJI_WayLine_Up::deleteSelectedRows()
{
	QList<int> rowsToDelete = getSelectedRows();
	if (rowsToDelete.isEmpty()) return;

	std::vector<int> pointvec;
	for (int i = 0;i < v_point_vector.size();++i)
	{
		int n = v_point_vector[i].first;
		pointvec.push_back(n);
	}

	for (int i = 0;i < rowsToDelete.size();++i)
	{
		v_point_vector.erase(
			std::remove_if(v_point_vector.begin(), v_point_vector.end(),
				[=](const QPair<int, QPointF>& pair)
		{	
			return rowsToDelete[i] == pair.first;
		}),
			v_point_vector.end()
			);
	}
	

	for (int i = 0;i < v_point_vector.size();++i)
	{
		v_point_vector[i].first = i;
	}

	for (int row : rowsToDelete) {
		removeRow(row);
	}
	
	for (int i = 0;i < v_point_vector.size();++i)
	{
		QTableWidgetItem *itemindex = this->item(i, 0);
		if (itemindex)
		{
			itemindex->setText(QString::number(v_point_vector[i].first + 1));
			itemindex->setTextAlignment(Qt::AlignCenter);
		}
	}
}