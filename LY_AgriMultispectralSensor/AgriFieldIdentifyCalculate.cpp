#include <QScrollBar>
#include <QProcess>
#include <QTimer>
#include <QDebug>
#include <QFileDialog>
#include <QQueue>
#include <QRandomGenerator>

#include <osgEarthFeatures/Feature>
#include <osgEarthFeatures/FeatureSource>
#include <osgEarthFeatures/FeatureCursor>
#include <osgEarthDrivers/feature_ogr/OGRFeatureOptions>
#include <osgEarthSymbology/Geometry>
#include <osgEarth/GeoData>

#include "LyMessageBox.h"
#include "AgriFieldIdentifyCalculate.h"
#include "ui_AgriFieldIdentifyCalculate.h"
#include "AgriMultispectralSensorUtil.h"
#include "LY_AgriMultispectralSensor.h"
#include "AgriFieldIdentifyDialog.h"

using namespace osgEarth;
using namespace osgEarth::Features;
using namespace osgEarth::Symbology;
using namespace osgEarth::Drivers;


struct AgriFieldIdentifyCalculatePrivate
{
	AgriFieldIdentifyCalculatePrivate()
	{
	};
	std::vector<FeatureInfo_shp> p_ShpInfo;
};

AgriFieldIdentifyCalculate::AgriFieldIdentifyCalculate(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::AgriFieldIdentifyCalculate),
	_p(new AgriFieldIdentifyCalculatePrivate)
{
	ui->setupUi(this);

	ui->progressBar->setValue(0);
}

AgriFieldIdentifyCalculate::~AgriFieldIdentifyCalculate()
{
    delete ui;
}

void AgriFieldIdentifyCalculate::GetIndentifyAreaList(std::vector<FeatureInfo_shp> & areaList)
{
	for (int i = 0; i < _p->p_ShpInfo.size(); i++) {
		areaList.emplace_back(_p->p_ShpInfo[i]);
	}
}

bool AgriFieldIdentifyCalculate::CheckAreaUsable()
{
	if (AgriMultispectralAnalysis* analysis = LY_AgriMultispectralSensor::Get().GetpAgriMultispectralAnalysis())
	{
		if (AgriFieldIdentifyDialog* identify = analysis->getFieldIdentifyDialog())
		{
			m_areaPoints.clear();
			identify->getSelectArea(m_areaPoints);
			if (m_areaPoints.size() >= 3)
			{
				return true;
			}
		}
	}
	return false;
}

void AgriFieldIdentifyCalculate::AppendLogMessage(const QString& msg, const QString& color, LineWrappingType optType)
{
	// 格式化当前时间
	//QDateTime dt = QDateTime::currentDateTime();
	//QString strTime = dt.toString("yyyyMMdd HHmmss.zzz");
	//QString ts = QString("<font color='gray'>[%1] </font>").arg(strTime);
	//QString coloredMessage = QString("<font color='%1'>%2</font>").arg(color).arg(msg);

	switch (optType)
	{
	case ADD_TYPE_NOLINE:
	{
		ui->textBrowser_2->moveCursor(QTextCursor::End);
		ui->textBrowser_2->insertHtml(msg);
	}
	break;
	case ADD_TYPE_LINE:
	{
		ui->textBrowser_2->append(msg);
	}
	break;
	case ADD_TYPE_REPLACE_LINE:
	{
		// 获取当前的QTextCursor  
		QTextCursor cursor = ui->textBrowser_2->textCursor();

		// 检查是否有选中的文本或光标是否在有效位置  
		if (!cursor.hasSelection() && cursor.position() != -1) {
			// 定位到当前行的开始  
			cursor.movePosition(QTextCursor::StartOfLine);
			// 保存当前行的开始位置  
			int start = cursor.position();

			// 定位到当前行的结束（下一行的开始或文档末尾）  
			cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
			// 删除当前行  
			cursor.removeSelectedText();

			// 将修改后的cursor设置回QTextBrowser  
			ui->textBrowser_2->setTextCursor(cursor);

			// 设置新的行文本
			ui->textBrowser_2->insertHtml(msg);
		}
	}
	break;
	default:
		break;
	}

	// Scroll to the end.
	QScrollBar* verticalScrollBar = ui->textBrowser_2->verticalScrollBar();
	verticalScrollBar->setValue(verticalScrollBar->maximum());
}

void AgriFieldIdentifyCalculate::SetProcess(double dVal)
{
	ui->progressBar->setValue(dVal);
	ui->progressBar->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	ui->progressBar->setFormat(QStringLiteral("%1%").arg(QString::number(dVal, 'f', 2)));
}

void AgriFieldIdentifyCalculate::on_BT_Play_clicked()
{
	// 校验区域可用性
	if (!CheckAreaUsable())
	{
		LyMessageBox::warning(nullptr, tr2("错误"), tr2("规划区域不可用"), QMessageBox::Ok);
		return;
	}

	ui->textBrowser_2->clear();
	_p->p_ShpInfo.clear();

	// 1. 设置 shapefile 数据源
	OGRFeatureOptions ogrOptions;
	ogrOptions.url() = "../data/maps_beijing_changping_shisanling/AreaRecognize/shisanling_84.shp";

	// 2. 创建 FeatureSource 对象
	FeatureSource* featureSource = FeatureSourceFactory::create(ogrOptions);
	featureSource->initialize();

	// 3. 创建一个查询（可设条件）
	Symbology::Query query;
	//query.expression() = "1=1"; // A simple query that matches all features

	if (query.expression().isSet())
	{
		std::cout << "Query expression: " << query.expression().value() << std::endl;
	}

	FeatureCursor* cursor = featureSource->createFeatureCursor(query);

	int filterId = 1;
	// 4. 遍历每个 Feature
	while (cursor && cursor->hasMore())
	{
		FeatureInfo_shp shpInfo;

		osg::ref_ptr<Feature> feature = cursor->nextFeature();
		osg::ref_ptr<Geometry> geom = feature->getGeometry();
		//const SpatialReference* ref = feature->getSRS();
		shpInfo.id = std::to_string(feature->getFID());
		AttributeTable attr = feature->getAttrs();
		for (auto &t : attr) {
			if (t.first == "area_mu") {
				shpInfo.area_mu = t.second.getDouble();
			}
			else if (t.first == "area_ha") {
				shpInfo.area_ha = t.second.getDouble();
			}
			else if (t.first == "crop_type") {
				shpInfo.crop_type = Utf8_Gbk(t.second.getString().c_str());
			}
		}

		if (geom.valid())
		{
			for (unsigned i = 0; i < geom->size(); ++i)
			{
				const osg::Vec3d& pt = geom->at(i);
				shpInfo.point_list.push_back(QPointF(pt.x(), pt.y()));
			}
		}

		bool isValidArea = false;

		// 校验识别田块是否在所选区域内
		if (AgriMultispectralSensorUtil::IsPolygonCompletelyInside(m_areaPoints, shpInfo.point_list)) {
			shpInfo.id = std::to_string(filterId++);
			isValidArea = true;
		}else{
			QPolygonF intersection = AgriMultispectralSensorUtil::CalculateIntersection(m_areaPoints, shpInfo.point_list);
			if (!intersection.isEmpty()) {
				shpInfo.id = std::to_string(filterId++);
				shpInfo.point_list = intersection;

				double area = 0.0;
				AgriMultispectralSensorUtil::CalcPolygonArea(intersection, area);
				shpInfo.area_mu = AgriMultispectralSensorUtil::sqmToMu(area);
				shpInfo.area_ha = shpInfo.area_mu / 15;

				isValidArea = true;
			}
		}

		AgriMultispectralSensorUtil::CalcPolygonCenterPoint(shpInfo.point_list, shpInfo.center_point);

		if (isValidArea)
		{
			_p->p_ShpInfo.emplace_back(shpInfo);
		}
	}

	QQueue<QString> textQueue;
	int totalNum = _p->p_ShpInfo.size();
	for (int i = 0 ; i< _p->p_ShpInfo.size(); i++)
	{
		auto &tInfo = _p->p_ShpInfo[i];
		// 更新显示
		QString lineText = tr2("已识别: ") + QString("ID: %1: %2: %3%4: %5")
			.arg(QString::fromStdString(tInfo.id) + tr2(" 作物类型"))
			.arg(tr2(tInfo.crop_type.c_str()) + tr2(" 面积（亩）"))
			.arg(tInfo.area_mu, 0, 'f', 2)
			.arg(tr2(" 面积（公顷）"))
			.arg(tInfo.area_ha, 0, 'f', 4);

		textQueue.push_back(lineText);
	}

	int counter = 0;
	QTimer* timer = new QTimer();
	QObject::connect(timer, &QTimer::timeout, [=]() mutable {
		if (textQueue.isEmpty()) {
			timer->stop();
			timer->deleteLater();
			return;
		}
		counter++;

		int currentInterval = timer->interval();
		QString lineText = textQueue.dequeue() + tr2(" 耗时:") + QString::number(currentInterval) + "ms";
		AppendLogMessage(lineText, "white", LineWrappingType::ADD_TYPE_LINE);
		int per = ((double)counter / totalNum) * 100;
		SetProcess(per);

		// 设置下次触发的随机间隔
		int randomInterval = QRandomGenerator::global()->bounded(0,20);
		timer->start(randomInterval);

		if (per == 100){
			AppendLogMessage((tr2("田块识别结束，成功识别%1块田块!")).arg(QString::number(_p->p_ShpInfo.size())), "white", LineWrappingType::ADD_TYPE_LINE);

			emit AreaIdentifyFinishied();
		}
	});

	// 初次启动，设置一个随机间隔
	AppendLogMessage(tr2("正在准备识别田块数据..."), "white", LineWrappingType::ADD_TYPE_LINE);
	timer->start(1000);
}

void AgriFieldIdentifyCalculate::on_BT_Stop_clicked()
{

}
