#ifndef LY_AGRISPRAYPRESHOMEWIDGET_H
#define LY_AGRISPRAYPRESHOMEWIDGET_H

#include <QWidget>
#include "IProj.h"
#include "ly_sprayprescriptionuntil.h"
#include <Protocol/XygsJL/sharedFile.h>
#include "DataManage/AgriFarm/AgriFarmManagerDecl.h"
#include <QPainter>
#include "KrigingInterPolator.h"
#include <map3d/IEarth.h>
#include <map3d/EarthEvent.h>
#include "map3d\IGraphics.h"
#include <QMap>


#include <osgEarthAnnotation/ImageOverlay>
#include <osgEarthSymbology/Geometry>
#include <osgEarth/Map>
#include <osgEarth/MapNode>
#include <osgEarth/ImageLayer>
#include <osg/Plane>

#include <osgEarthFeatures/Feature>
#include <osgEarthFeatures/FeatureListSource>
#include <osgEarthFeatures/FeatureModelGraph>
#include <osgEarth/SpatialReference>
#include <osgEarth/Utils>
#include <osg/Vec3d>

#include <osgEarth/Utils>
#include <osg/Vec3d>
#include "AgriDataAnalysisPredictAlgorithm.h"
#include "AgrisprayComboBox.h"
#include <QMutex>

using namespace AgriDataAnalysisPredictAlgorithm;
using namespace AgriSprayPrescriptionDecl;
namespace Ui {
class LY_AgriSprayPresHomeWidget;
}

/* 数据采集方式 */
enum class PrescretionConcolType
{
   PrescretionConcolType_ADD =1,
	PrescretionConcolType_EDIT = 2, 
	PrescretionConcolType_DELETE = 3,
	PrescretionConcolType_NONE = 4,
};

class LY_AgriSprayPresHomeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LY_AgriSprayPresHomeWidget(QWidget *parent = nullptr);
    ~LY_AgriSprayPresHomeWidget();

    void drawDynamic(QPainter &painter, IProj *pProj);
	void updataCurActiveFarm();

	
	void draw3DGridInfo( pos3d *p3d);
	void draw2DGridInfo(QPainter &painter, IProj *pProj);

	HeatMap3D getHeatMap3D() const;


protected:
    virtual void changeEvent(QEvent *event);
    virtual void showEvent(QShowEvent *event);
    virtual void hideEvent(QHideEvent *event);

private:
	QMutex      sMutex;
    QString														m_sIconPath;
    AgriFieldInfoPtr											m_pFieldInfo;
    bool													         	m_show = false;
	//当前的数据源ID
	QString                                                       m_currentGuid;
	QString                                                         m_farmName;

	//选中的row
	int                                                               m_selectedCheckBoxRow = -1;

	PrescretionConcolType                            m_type = PrescretionConcolType::PrescretionConcolType_NONE;

	QString m_currentFieldID = nullptr;


	//虫害数据
	AgriPestDatasetVec m_pestDataVec;
	//处方图
	AgriSprayPrescriptionVec  m_listVec;
	//虫害分布图
	AgriPestMapInfoModel m_pestMaoInfo;

	//存等级颜色
	std::unordered_map<int, QList<QPair<QString, std::string>>>				m_mDiffusionLayerHexColor;
	std::unordered_map<int, std::vector<osgEarth::Symbology::Color>>			m_mDiffusionLayerOsgColor;

	std::vector<int> firstLeveVec;
	std::vector<int> secondLeveVec;
	std::vector<int> threeLeveVec;
	std::vector<int> fourLeveVec;
	std::vector<int> fiveLeveVec;

	//slider分段数组
	std::vector<std::vector<int>> pestLevVec;

	//每个阶段 ： 对应的喷药量
	std::map<std::vector<int>, QString> levpValueMap;
	std::map<int, std::pair<std::vector<int>, int>> psetLeveallDataMap;

	HeatMap3D  m_map3D;

	//每个阶段 ： 对应的多少亩地
	std::map<std::vector<int>, QString> tempMap;

	std::vector<IText*> grid3DInfoVec;

	AgrisprayComboBox* m_pAgrisprayComboBox = nullptr;

private:

	void setupUI();
	void setComboxStatus(bool isEnable);

	//设置列表数据
	void setListData();
	//设置单个iTem数据
	void setupItemData(int row);
	//添加数据集相关数据
	void addPestData(AgriPestDataset pest);
	//清除box数据
	void clearnBox();

	//等级
	int leveValue;
	//绘制3d处方图
	void  draw3DGridMap(AgriSprayPrescriptionModel &sprayModel);
	//绘制二维处方图
	//void create2DGridDrawing(std::vector<std::vector<double>>& grid,
	//	double minX, double minY,
	//	double spacing, QPainter& painter, IProj *pProj);

	bool isClickCompute = false;

	bool containsAll(const std::vector<int>& vec1, const std::vector<int>& vec2);
	osgEarth::Symbology::Color hexToColor(const std::string& hexColor, float rgb_a = 1.0f);

private slots:

    void on_ptn_add_dataset_clicked();
    void on_ptn_update_dataset_clicked();
    void on_ptn_delete_dataset_clicked();
    void on_ptn_export_dataset_clicked();
    void onCellClicked(int row, int column);
    void onCellDoubleClicked(int row, int column);
	//添加盐田
	void addField(QString &text, QString & field_id);
	void on_dataSourceComboBoxChanged(const int &);
	void onCheckboxChanged(int row, int col, int state);
	void on_mapComboBoxChanged(const int &);
    void on_sure_btn_clicked();//计算
	void on_show_hide_clicked();
	void on_sliderValuesChanged(const std::vector<std::pair<int, int>>& intervals);

private:
    Ui::LY_AgriSprayPresHomeWidget *ui;
};

#endif // LY_AGRISPRAYPRESHOMEWIDGET_H
