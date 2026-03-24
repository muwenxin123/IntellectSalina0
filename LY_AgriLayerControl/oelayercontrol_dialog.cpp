#include <QPainter>
#include <QGraphicsDropShadowEffect>
#include <QVBoxLayout>
#include <QLabel>

#include <osgEarthFeatures/Feature>
#include <osgEarthFeatures/FeatureSource>
#include <osgEarthFeatures/FeatureCursor>
#include <osgEarthDrivers/feature_ogr/OGRFeatureOptions>
#include <osgEarthSymbology/Geometry>
#include <osgEarth/GeoData>

#include "QCString.h"
#include "oelayercontrol_dialog.h"
#include "ui_oelayercontrol_dialog.h"
#include "AgriLayerControlUtil.h"

using namespace osgEarth;
using namespace osgEarth::Features;
using namespace osgEarth::Symbology;
using namespace osgEarth::Drivers;


struct FeatureInfo_shp
{
	FeatureInfo_shp()
		:id("")
		, crop_type("none")
		, area_mu(0.0)
		, area_ha(0.0)
		, center_point(0.0, 0.0)
	{}
	std::string id;
	std::string crop_type;
	double area_mu;
	double area_ha;
	QPointF center_point;
	std::vector<QPointF> point_list;
};

struct oeLayerControl_DialogPrivate
{
	oeLayerControl_DialogPrivate()
	{
	};
	std::vector<FeatureInfo_shp> p_ShpInfo;
};

oeLayerControl_Dialog::oeLayerControl_Dialog(QWidget *parent) :
    QWidget(parent),
	ui(new Ui::oeLayerControl_Dialog),
	_p(new oeLayerControl_DialogPrivate)
{
    ui->setupUi(this);

	setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
	setAttribute(Qt::WA_TranslucentBackground);
	setAttribute(Qt::WA_ShowWithoutActivating);

	auto *shadow = new QGraphicsDropShadowEffect(this);
	shadow->setBlurRadius(5.0);
	shadow->setOffset(0, 0);
	shadow->setColor(QColor(0, 0, 0, 160));
	setGraphicsEffect(shadow);

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setContentsMargins(20, 20, 20, 20);
	layout->setSpacing(10);

	resize(286, 627); // 适当尺寸

	setStyleSheet("border-image:url(:/images/LayerControl/layer_3D_bkgd.png)");

    initTreeWidget();
}

oeLayerControl_Dialog::~oeLayerControl_Dialog()
{
    delete ui;
}

void oeLayerControl_Dialog::OnCreate()
{
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
			if (t.first == "area_mu"){
				shpInfo.area_mu = t.second.getDouble();
			}else if (t.first == "area_ha"){
				shpInfo.area_ha = t.second.getDouble();
			}else if (t.first == "crop_type"){
				shpInfo.crop_type = Utf8_Gbk(t.second.getString().c_str());
			}
		}

		if (geom.valid())
		{
			for (unsigned i = 0; i < geom->size(); ++i)
			{
				const osg::Vec3d& pt = geom->at(i);
				shpInfo.point_list.emplace_back(QPointF(pt.x(), pt.y()));
			}

			if (shpInfo.id == "152")
			{
				shpInfo.center_point.setX(116.196667);
				shpInfo.center_point.setY(40.264444);
			}
			else if (shpInfo.id == "180")
			{
				shpInfo.center_point.setX(116.198611);
				shpInfo.center_point.setY(40.264167);
			}
			else if (shpInfo.id == "182")
			{
				shpInfo.center_point.setX(116.198889);
				shpInfo.center_point.setY(40.265833);
			}
			else
			{
				AgriDataAnalysisPredictUtil::CalcPolygonCenterPoint(shpInfo.point_list, shpInfo.center_point);
			}
		}

		_p->p_ShpInfo.emplace_back(shpInfo);
	}
}

void oeLayerControl_Dialog::setMapNode(osgEarth::MapNode *_mapNode)
{
    mapNode = _mapNode;
    layerControl = new oeLayerControl;
    layerControl->setMapNode(mapNode);
    if (layerControl->loadLayerFromMpaNode())
    {
        imageLayerNames.clear();
        elevationLayerNames.clear();
		modelLayerNames.clear();
		fieldLayerNames.clear();

        imageLayerNames = layerControl->getImageLayerOrder();
        elevationLayerNames = layerControl->getElevationLayerOrder();
        modelLayerNames = layerControl->getModelLyaerOrder();

		QString fieldName = tr2("十三陵田块识别");
		fieldLayerNames.emplace_back(std::make_pair(fieldName.toStdString(), false));

        updateTreeWidget();
    }
}

void oeLayerControl_Dialog::drawDynamic(QPainter &painter, IProj *pProj)
{
	QFont originFont = painter.font();

	QPen pen(Qt::darkRed);
	pen.setWidth(2);

	QBrush brush;
	brush.setStyle(Qt::BrushStyle::SolidPattern);
	brush.setColor(QColor(67, 228, 255, 45));
	painter.setBrush(brush);

	// 设置颜色（带透明度（0-255，128 代表 50% 透明度））
	QColor textColor(67, 228, 255);
	textColor.setAlpha(200);
	pen.setColor(textColor);
	painter.setPen(pen);

	// 采集点绘制
	int pointSize = 9;
	QFont font("Arial", pointSize);
	painter.setFont(font);

	// 获取文本尺寸
	QFontMetrics fm(font);
	int textHeight = fm.ascent();

	painter.setRenderHint(QPainter::TextAntialiasing);

	for (const auto& field : fieldLayerNames)
	{
		if (field.second)
		{
			for (unsigned i = 0; i < _p->p_ShpInfo.size(); ++i)
			{
				auto& shpInfo = _p->p_ShpInfo[i];
				QVector<QPoint> points;

				std::vector<QPointF>& tmpVec = shpInfo.point_list;
				for (unsigned j = 0; j < tmpVec.size(); ++j)
				{
					const QPointF& pt = tmpVec[j];
					QPoint point = pProj->Jwd2Screen(pt.x(), pt.y());
					points.push_back(point);
				}

				QPolygon poly(points);
				painter.drawPolygon(poly);
				QString text = "#" + QString::fromStdString(shpInfo.id) + "\n"
					+ tr2(shpInfo.crop_type.c_str()) + "\n"
					+ QString::number(shpInfo.area_mu) + tr2("亩");

				QPoint screenPoint = pProj->Jwd2Screen(shpInfo.center_point.x(), shpInfo.center_point.y());
				int x = screenPoint.x();
				int y = screenPoint.y();

				QStringList lines = text.split('\n');
				for (const QString& line : lines) {
					painter.drawText(QPoint(x, y), line);
					y += painter.fontMetrics().height();
				}
			}
		}
	}

	painter.setFont(originFont);
}

void oeLayerControl_Dialog::updateParentItem(QTreeWidgetItem *item)
{
	QTreeWidgetItem* parent = item->parent();
	if (parent == nullptr)
	{
		return;
	}

	int selectCount = 0;
	int nPartiallCount = 0;
	int childCount = parent->childCount();
	for (int i = 0; i < childCount; i++)
	{
		QTreeWidgetItem* childItem = parent->child(i);
		if (childItem->checkState(0) == Qt::Checked)
		{
			selectCount++;
		}
		else if (childItem->checkState(0) == Qt::PartiallyChecked)
		{
			nPartiallCount++;
		}
	}

	if (selectCount == childCount)
	{
		parent->setCheckState(0, Qt::Checked);
	}
	else  if (selectCount > 0 && selectCount < childCount || selectCount == 0 && nPartiallCount > 0)
	{
		parent->setCheckState(0, Qt::PartiallyChecked);
	}
	else if (selectCount == 0 && nPartiallCount == 0)
	{
		parent->setCheckState(0, Qt::Unchecked);
	}
}

bool oeLayerControl_Dialog::setLayerShowState(std::string LayerName, bool isShow)
{
	auto iter = std::find_if(fieldLayerNames.begin(), fieldLayerNames.end(), [&](const std::pair<std::string, bool>& poi) { return LayerName == poi.first; });
	if (iter != fieldLayerNames.end()) {
		iter->second = isShow;
		return true;
	}
	else if (layerControl)
	{
		return layerControl->setLayerShowOrDis(LayerName, isShow);
	}
	return false;
}

void oeLayerControl_Dialog::initTreeWidget()
{
    ui->treeWidget->clear();
    ui->treeWidget->setHeaderLabel(tr2("图层控制_三维"));
	ui->treeWidget->setHeaderHidden(true);
	ui->treeWidget->setStyleSheet(R"(
        QTreeView {
            border: 1px solid #1da2f1;
			padding: 35 15 15 15;
			outline: 0px;
			font-size: 12px;
			background: transparent;
			background-color: transparent;
			selection-background-color: transparent;
			alternate-background-color: transparent; 
			font-family: "Microsoft YaHei";
			show-decoration-selected: 0; 
			qproperty-indentation: 12;
        }
        QTreeView::item {
            height: 24px;
			border: none; 
			background: transparent;
			border-radius: 6px;
			padding: 0 6;
			/*margin-left: -6px;*/
        }
        QTreeView::item:has-children {
			margin-left: 6px;
        }

		QTreeView::item:hover { background-color: rgba(255, 255, 255, 30); }
		QTreeView::item:selected { background-color: rgba(90, 150, 255, 80); }
		QTreeView::item:selected { background-color: rgba(90, 150, 255, 100); }
		QTreeView::item:hover { background-color: rgba(255, 255, 255, 30); } 

		QTreeView[alternatingRowColors="false"]::item { border-image: none }
		QTreeView[alternatingRowColors="false"]::item:hover { border-image: none; }
		QTreeView[alternatingRowColors="false"]::item:selected { border-image: none; color: #ffffff; }
		QTreeView[alternatingRowColors="false"]::item:!closed:has-children { border-image: none; }
		QTreeView[alternatingRowColors="false"]::item:!closed:has-children:hover { border-image: none; }
		QTreeView[alternatingRowColors="false"]::item:!closed:has-children:selected { border-image: none; }

		QTreeView::branch:has-children:!has-siblings:closed,QTreeView::branch:closed:has-children:has-siblings { border-image: none;image: url(:/images/LayerControl/arrow_right.png); }
		QTreeView::branch:open:has-children:!has-siblings,QTreeView::branch:open:has-children:has-siblings { border-image: none; image: url(:/images/LayerControl/arrow_down.png); }
		
		QTreeView::branch:has-siblings:!adjoins-item { image: none;}
		QTreeView::branch:has-siblings:adjoins-item { image: none;}
		QTreeView::branch:!has-children:!has-siblings:adjoins-item { image: none;}
    )");

	connect(ui->treeWidget, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(on_treeWidget_itemClicked(QTreeWidgetItem *, int)));
}

void oeLayerControl_Dialog::updateTreeWidget()
{
    ui->treeWidget->clear();
	ui->treeWidget->setColumnCount(1);

    QTreeWidgetItem *imageRoot = new QTreeWidgetItem(ui->treeWidget);
    imageRoot->setText(0, tr2("卫星影像图"));
	imageRoot->setFlags(imageRoot->flags() | Qt::ItemIsTristate | Qt::ItemIsUserCheckable);
	imageRoot->setFont(0, QFont("Microsoft YaHei", 10, QFont::Bold));

    for (auto imageInfo : imageLayerNames)
    {
        QTreeWidgetItem *imageLayer = new QTreeWidgetItem(imageRoot);
        imageLayer->setText(0, tr3((imageInfo.first.data())));
		imageLayer->setIcon(0, QIcon(":/images/LayerControl/map.png"));
        imageLayer->setCheckState(0, imageInfo.second ? Qt::Checked : Qt::Unchecked);
		updateParentItem(imageLayer);
    }

    QTreeWidgetItem *eleRoot = new QTreeWidgetItem(ui->treeWidget);
    eleRoot->setText(0, tr2("高程地形图"));
	eleRoot->setFlags(eleRoot->flags() | Qt::ItemIsTristate | Qt::ItemIsUserCheckable);
	eleRoot->setFont(0, QFont("Microsoft YaHei", 10, QFont::Bold));

    for (auto eleInfo : elevationLayerNames)
    {
        QTreeWidgetItem *eleLayer = new QTreeWidgetItem(eleRoot);
        eleLayer->setText(0, tr3(eleInfo.first.data()));
		eleLayer->setIcon(0, QIcon(":/images/LayerControl/elevation.png"));
        eleLayer->setCheckState(0, eleInfo.second ? Qt::Checked : Qt::Unchecked);
		updateParentItem(eleLayer);
    }

	QTreeWidgetItem * fieldRoot = new QTreeWidgetItem(ui->treeWidget);
	fieldRoot->setText(0, tr2("田块识别图"));
	fieldRoot->setFlags(fieldRoot->flags() | Qt::ItemIsTristate | Qt::ItemIsUserCheckable);
	fieldRoot->setFont(0, QFont("Microsoft YaHei", 10, QFont::Bold));

	for (auto fieldInfo : fieldLayerNames)
	{
		QTreeWidgetItem *fieldLayer = new QTreeWidgetItem(fieldRoot);
		fieldLayer->setText(0, tr3(fieldInfo.first.data()));
		fieldLayer->setIcon(0, QIcon(":/images/LayerControl/map.png"));
		fieldLayer->setCheckState(0, fieldInfo.second ? Qt::Checked : Qt::Unchecked);
		updateParentItem(fieldLayer);
	}

    QTreeWidgetItem *modelRoot = new QTreeWidgetItem(ui->treeWidget);
    modelRoot->setText(0, tr2("三维模型"));
	modelRoot->setFlags(modelRoot->flags() | Qt::ItemIsTristate | Qt::ItemIsUserCheckable);
	modelRoot->setFont(0, QFont("Microsoft YaHei", 10, QFont::Bold));

    for (auto modelInfo : modelLayerNames)
    {
        QTreeWidgetItem *modelLayer = new QTreeWidgetItem(modelRoot);
        modelLayer->setText(0, tr3(modelInfo.first.data()));
		modelLayer->setIcon(0, QIcon(":/images/LayerControl/model.png"));
        modelLayer->setCheckState(0, modelInfo.second ? Qt::Checked : Qt::Unchecked);
		updateParentItem(modelLayer);
    }
}

void oeLayerControl_Dialog::changeEvent(QEvent *event)
{

}

void oeLayerControl_Dialog::showEvent(QShowEvent *event)
{

}

void oeLayerControl_Dialog::hideEvent(QHideEvent *event)
{

}

void oeLayerControl_Dialog::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event);
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	QColor backgroundColor = QColor(10, 30, 80, 0); // 深蓝透明
	painter.setBrush(backgroundColor);
	painter.setPen(Qt::NoPen);
	painter.drawRoundedRect(rect(), 5, 5); // 圆角矩形
}

void oeLayerControl_Dialog::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton) {
		QApplication::setOverrideCursor(Qt::OpenHandCursor);
		m_dragPosition = event->globalPos() - frameGeometry().topLeft();
		m_dragging = true;
	}
}

void oeLayerControl_Dialog::mouseMoveEvent(QMouseEvent *event)
{
	if (m_dragging && (event->buttons() & Qt::LeftButton)) {
		QApplication::setOverrideCursor(Qt::ClosedHandCursor);
		move(event->globalPos() - m_dragPosition);
	}
}

void oeLayerControl_Dialog::mouseReleaseEvent(QMouseEvent *event)
{
	Q_UNUSED(event);
	QApplication::setOverrideCursor(Qt::ArrowCursor);
	m_dragging = false;
}

void oeLayerControl_Dialog::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
	if (item == nullptr)
		return;

	int nCount = item->childCount();

	ui->treeWidget->disconnect(ui->treeWidget, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(on_treeWidget_itemClicked(QTreeWidgetItem *, int)));

	bool bCheck = false;
	if (item->checkState(0) == Qt::Checked)
	{
		bCheck = true;
		if (nCount > 0)
		{
			for (int i = 0; i < nCount; i++)
			{
				QTreeWidgetItem* itemChild = item->child(i);
				itemChild->setCheckState(0, Qt::Checked);
				setLayerShowState(tr2(itemChild->text(0).toLocal8Bit().data()).toStdString(), true);
			}
			updateParentItem(item);
		}
		else
		{
			updateParentItem(item);
			setLayerShowState(tr2(item->text(0).toLocal8Bit().data()).toStdString(), true);
		}
	}
	else if (Qt::Unchecked == item->checkState(0))
	{
		if (nCount > 0)
		{
			for (int i = 0; i < nCount; i++)
			{
				QTreeWidgetItem* itemChild = item->child(i);
				itemChild->setCheckState(0, Qt::Unchecked);
				setLayerShowState(tr2(itemChild->text(0).toLocal8Bit().data()).toStdString(), false);
			}
			updateParentItem(item);
		}
		else
		{
			updateParentItem(item);
			setLayerShowState(tr2(item->text(0).toLocal8Bit().data()).toStdString(), false);
		}
	}

	connect(ui->treeWidget, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(on_treeWidget_itemClicked(QTreeWidgetItem *, int)));
}
