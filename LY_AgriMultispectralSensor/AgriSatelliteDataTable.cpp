#include <QHeaderview>
#include <QCString.h>
#include <QProgressBar>
#include <QStyleOptionProgressBar>
#include <QStylePainter>
#include <QVBoxLayout>
#include <QTimer>
#include <QSettings>
#include <QPushButton>
#include <QDebug>
#include <QDir>

#include <gdal.h>
#include <gdal_priv.h>
#include <ogrsf_frmts.h>

#include "AgriSatelliteDataTable.h"
#include "AgriSatelliteDownloadManager.h"
#include "AgriMultispectralSensorUtil.h"

CustomProgressBar::CustomProgressBar(QProgressBar *parent /*= nullptr*/)
	: QProgressBar(parent), m_value(0.0), m_max(100.0)
{
	//setMinimumSize(200, 30);

	//QTimer *timer = new QTimer(this);
	//connect(timer, &QTimer::timeout, this, [=]() {
	//	updateValue(((int)m_value + 1) % ((int)m_max + 1));
	//	update(); // 触发 paintEvent
	//});
	//timer->start(100);
}

void CustomProgressBar::updateValue(float value)
{
	m_value = value * 100;

	setValue((int)m_value);
	setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	setFormat(QStringLiteral("%1%").arg(QString::number(m_value, 'f', 2)));

	//update(); // 触发 paintEvent
}

/*
void CustomProgressBar::paintEvent(QPaintEvent *)
{
	QStylePainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	QRect r = rect();

	// 背景样式
	painter.setBrush(QColor("#444"));
	painter.setPen(Qt::NoPen);
	painter.drawRoundedRect(r, 5, 5);
	painter.fillRect(r, QColor("#333333"));

	// 前景样式（进度部分）
	int w = r.width() * m_value / m_max;
	QRect progressRect = QRect(r.left(), r.top(), w, r.height());
	painter.setBrush(QColor("#00aaff"));
	painter.drawRoundedRect(progressRect, 5, 5);

	// 文字样式
	painter.setPen(Qt::blue);
	painter.drawText(r, Qt::AlignCenter, QString("%1%").arg(m_value));

	// 设置进度条的风格
	QStyleOptionProgressBar option;
	option.initFrom(this);
	option.rect = rect();
	option.minimum = 0;
	option.maximum = m_max;
	option.progress = m_value;
	option.text = QString::asprintf("%.2f", m_value);
	option.textVisible = true;
	option.textAlignment = Qt::AlignCenter;

	painter.drawControl(QStyle::CE_ProgressBar, option);
}
*/

AgriSatelliteDataTable::AgriSatelliteDataTable(QWidget *parent) : QTableWidget(parent)
{
    this->clear();

    //设置列数和列宽
    this->setColumnCount(6);
    this->setColumnWidth(0, 100);
    this->setColumnWidth(1, 120);
    this->setColumnWidth(2, 120);
    this->setColumnWidth(3, 120);
	this->setColumnWidth(4, 200);
	this->setColumnWidth(5, 100); 

    this->setStyleSheet("QTableWidget::item{padding:0px;}");

    QStringList headText;
    headText << tr2("名称") << tr2("分类") << tr2("描述") << tr2("文件大小") << tr2("下载进度") << tr2("操作按钮");

    this->setHorizontalHeaderLabels(headText);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setSelectionMode(QAbstractItemView::SingleSelection);
    this->verticalHeader()->setVisible(true);
    this->horizontalHeader()->setStretchLastSection(true);

	/*通过元对象系统调用成员函数的方式。跨线程调用、延迟调用或动态调用方法
	
	 1. 跨线程安全调用
		假设你在非 GUI 线程中想让主线程执行 load() 方法，使用 Qt::QueuedConnection 就能自动在线程之间切换执行上下文。

	 2. 延迟执行（推迟到事件循环）
		即使在主线程中，它也会将调用延迟到下一次事件循环，比如你在构造函数中不能立即执行某些操作，可以这样写,
		这可以避免在构造期间调用虚函数或涉及未完成初始化的成员。

	 3. 动态调用
	    不直接调用函数名，靠字符串反射机制 */
    QMetaObject::invokeMethod(this, "load", Qt::QueuedConnection);

	// 查询当前激活农场
	QSettings ini("../data/conf/ly.ini", QSettings::IniFormat);
	ini.beginGroup("Agri");
	m_sCurActiveFarm = ini.value("FarmID", "").toString();
}

void AgriSatelliteDataTable::load()
{
    //清空原有数据
    int row = this->rowCount();
    for (int i = 0; i < row; ++i) {
        this->removeRow(0);
    }

	m_vecFieldList.clear();

	AgriFarmManagerService()->GetFieldInfoList(m_vecFieldList);
	for (auto& fieldInfo : m_vecFieldList){
		if (m_sCurActiveFarm == fieldInfo.farm_id){  // 匹配激活农场
			insertSize(fieldInfo);
		}
	}
}

void AgriSatelliteDataTable::insertSize(const AgriFieldInfo& fieldInfo)
{
	QString name = fieldInfo.name;
	QString use = fieldInfo.category;
	QString free = fieldInfo.description;
	QString all = "";
	int percent = 0;

    int row = this->rowCount();
    this->insertRow(row);

    QTableWidgetItem *itemname = new QTableWidgetItem(name);
    QTableWidgetItem *itemuse = new QTableWidgetItem(use);
    itemuse->setTextAlignment(Qt::AlignCenter);
    QTableWidgetItem *itemfree = new QTableWidgetItem(free);
    itemfree->setTextAlignment(Qt::AlignCenter);
    QTableWidgetItem *itemall = new QTableWidgetItem(all);
    itemall->setTextAlignment(Qt::AlignCenter);

    this->setItem(row, 0, itemname);
    this->setItem(row, 1, itemuse);
    this->setItem(row, 2, itemfree);
    this->setItem(row, 3, itemall);

	CustomProgressBar *progressBar = new CustomProgressBar;

	progressBar->setStyleSheet(R"(
		QProgressBar {
			border: 1px solid #444444;
			border-radius: 5px;
			background-color: rgba(0, 0, 0, 51); /* 黑色背景，透明度20% */
			text-align: center;
			color: white;
		}

		QProgressBar::chunk {
			border-radius: 5px;
			background: qradialgradient(
				cx:0.5, cy:0.5, radius:0.8,
				fx:0.5, fy:0.5,
				stop:0 #8428f1,
				stop:1 #95a1f9
			);
		}
	)");

    this->setCellWidget(row, 4, progressBar);

	// 添加控制按钮
	QPushButton* downloadBtn = new QPushButton(tr2("下载"));
	QPushButton* cancelBtn = new QPushButton(tr2("取消"));

	QHBoxLayout* btnLayout = new QHBoxLayout();
	btnLayout->setContentsMargins(10, 0, 10, 0);
	btnLayout->setSpacing(15);
	//btnLayout->addStretch();
	btnLayout->addWidget(downloadBtn);
	btnLayout->addWidget(cancelBtn);

	QWidget* container = new QWidget();
	container->setLayout(btnLayout);

	connect(downloadBtn, &QPushButton::clicked, this, [&, progressBar]() {

		/*
		QPushButton* btn = qobject_cast<QPushButton*>(sender());
		if (!btn) return;

		// 遍历所有行
		for (int row = 0; row < this->rowCount(); ++row) {
			QWidget* container = this->cellWidget(row, 5); // 你的按钮所在列
			if (!container) continue;

			// 在 container 内查找该按钮
			if (container->findChild<QPushButton*>("", Qt::FindDirectChildrenOnly) == btn ||
				container->findChildren<QPushButton*>().contains(btn)) {
				//qDebug() << tr2("下载按钮点击，所在行：") << row;
				break;
			}
		}

		std::vector<osg::Vec3> polygonPoints = {
			osg::Vec3(0, 0, 0),
			osg::Vec3(100, 0, 0),
			osg::Vec3(100, 100, 0),
			osg::Vec3(0, 100, 0),
			osg::Vec3(0, 0, 0) // 可以不写，函数会自动闭合
		};

		writePolygonToShapefile(polygonPoints, "output_polygon.shp");

		QString clipPath = "output_polygon.shp";
		*/


		QString taskId = "task_" + fieldInfo.id;

		QString savePath = QDir::currentPath() + "/sentinel_image/output";

		DownloadTask task = { taskId, "", savePath };

		AgriSatelliteDownloadManager::instance().addTask(task);

		QObject::connect(&AgriSatelliteDownloadManager::instance(), &AgriSatelliteDownloadManager::taskProgress,
			[&](const QString& id, const QString& msg) {
			/*qDebug() << id << "Progress:" << msg;*/
			if (!id.isEmpty() && !msg.isEmpty()) {
				DownloadProgress tProgress = DownloadProgress::fromJson(AgriMultispectralSensorUtil::StringToJson(msg));
				// 更新进度展示
				if (tProgress.fileSize)
				{
					progressBar->updateValue((float)tProgress.downloadSize / tProgress.fileSize);
				}
			}
		});

		QObject::connect(&AgriSatelliteDownloadManager::instance(), &AgriSatelliteDownloadManager::taskFinished,
			[](const QString& id) {
			qDebug() << id << "Download finished.";
		});

	});

	connect(cancelBtn, &QPushButton::clicked, this, [&]() {

	});

	this->setCellWidget(row, 5, container);
}

bool AgriSatelliteDataTable::writePolygonToShapefile(const std::vector<osg::Vec3>& points, const std::string& shpPath)
{
	if (points.size() < 3) {
		std::cerr << "多边形至少需要 3 个点！\n";
		return false;
	}

	GDALAllRegister();
	GDALDriver* driver = GetGDALDriverManager()->GetDriverByName("ESRI Shapefile");
	if (!driver) {
		std::cerr << "未找到 Shapefile 驱动器！\n";
		return false;
	}

	GDALDataset* poDS = driver->Create(shpPath.c_str(), 0, 0, 0, GDT_Unknown, nullptr);
	if (!poDS) {
		std::cerr << "无法创建 shapefile 数据源！\n";
		return false;
	}

	OGRLayer* poLayer = poDS->CreateLayer("polygon", nullptr, wkbPolygon, nullptr);
	if (!poLayer) {
		std::cerr << "无法创建图层！\n";
		GDALClose(poDS);
		return false;
	}

	OGRFieldDefn idField("ID", OFTInteger);
	poLayer->CreateField(&idField);

	// 构造 LinearRing
	OGRLinearRing ring;
	for (const auto& p : points) {
		ring.addPoint(p.x(), p.y());
	}

	// 若首尾点不相同，则自动闭合
	if (!(points.front() == points.back())) {
		ring.addPoint(points.front().x(), points.front().y());
	}

	// 构造 Polygon
	OGRPolygon polygon;
	polygon.addRing(&ring);

	// 写入要素
	OGRFeature* poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());
	poFeature->SetField("ID", 1);
	poFeature->SetGeometry(&polygon);

	if (poLayer->CreateFeature(poFeature) != OGRERR_NONE) {
		std::cerr << "写入多边形要素失败！\n";
	}

	OGRFeature::DestroyFeature(poFeature);
	GDALClose(poDS);
	std::cout << "多边形 shapefile 成功写入: " << shpPath << "\n";
	return true;
}
