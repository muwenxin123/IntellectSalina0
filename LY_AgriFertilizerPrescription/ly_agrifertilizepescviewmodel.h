#ifndef LY_AGRIFERTILIZEPESCVIEWMODEL_H
#define LY_AGRIFERTILIZEPESCVIEWMODEL_H

#include <QObject>
#include <QPainter>
#include "LY_AgriFertilizerPrescriptionActivator.h"
#include "KrigingInterPolator.h"
#include <map3d/IEarth.h>
#include <map3d/EarthEvent.h>
#include "map3d\IGraphics.h"
#include <QPointF>
#include <QMutex>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include "QSqlQuery"
#include <map>
#include <osgEarthSymbology/Geometry>
#include <osgEarthSymbology/Color>
#include "IProj.h"
#include <QUuid>
#include "LY_AgriFertilizerPrescription_global.h"
#include "DataManage/AgriFarm/AgriSprayPrescriptDecl.h"

using namespace AgriSprayPrescriptionDecl;

class LY_AGRIFERTILIZERPRESCRIPTION_EXPORT LY_AgriFertilizePescViewModel : public QObject
{
    Q_OBJECT
public:
    explicit LY_AgriFertilizePescViewModel(QObject *parent = nullptr);

	QString getFertilizeName(int &valid_identify);

	std::vector<AgriSoilTestingFertilityPoint> m_flyPoints;
	//画3d处方图
	void draw3DGridMap_Update(double space_width, double degress, QString Area_id = "");

	void draw3DGridMap(AgriSoilTestingFertilityPointPtrVec soilFertilityVec, double space_width, QString area_id = "");
	//画2d处方图
	void draw2DGridMap(QPainter &painter, IProj *pProj);
	//处方图显隐
	void showAndHideMap(bool isShow);
	//清理处方图
	void clear3dMap();
	//生成航线
	void generateFertilizerDiagramFlyLine(double pyfk = 4.0, double flyHeight = 5.0);
	//清理航线数据
	void clearFlyLine();
	//模拟飞行
	void staringFly(double flyHeight);
	//模拟指令播放();
	void staringInstructionFly(double isPlayInstruction = true);
	std::vector< AirInstructionPoi> generateInstructionPoint(double pyfk);
	

	void setFieldId(QString field_id, QString field_name);
	//获取部分处方图信息
	FertilizerPrescriptionGridInfoVec	 getMapInfoData() const;
	HeatMap3D  * getHeatMap3D(); 
	//肥力等级对应的 格子信息
	std::map<double, AgriSoilTestingFertilityPointPtr> soilFertilityMap;
	//相邻数字排序
	std::vector<std::vector<int>> groupConsecutiveNumbers(const std::vector<int>& nums, bool isLeft);

public:

	QString GenerateUuid()
	{
		const QByteArray &ba = QUuid::createUuid().toRfc4122();
		QUuid uid = QUuid::fromRfc4122(QByteArray::fromRawData(ba, 16));
		return uid.toString(QUuid::WithoutBraces);
	};

	void InitDataBase() ;
	//添加处方图
	 bool AddFertilizePrescription(FertilizerPrescription &model) ;
	//获取处方图
	 FertilizerPrescriptionVec GetFertilizePrescription(const QString field_id) ;
	//修改处方图
	 bool UpdateFertilizePrescription(FertilizerPrescription &model) ;
	//删除处方图
	 bool DeleteFertilizePrescription(QString &guid);

	 //添加处方图info
	 bool AddFertilizePrescriptionInfo(FertilizerPrescriptionGridInfo &model);
	 //获取处方图info
	 FertilizerPrescriptionGridInfoVec GetFertilizePrescriptionInfo(const QString map_id);
	 //修改处方图info
	 bool UpdateFertilizePrescriptionInfo(FertilizerPrescriptionGridInfo &model);
	 //删除处方图info
	 bool DeleteFertilizePrescriptionInfo(QString &guid);

	 //添加指令
	 bool AddTaskInstruction(AgriTaskInstruction &Instruction) const;
	 //删除指令
	 bool DeleteTaskInstruction() const;

private:

	QSqlDatabase				db;
	bool			isOpen;
	mutable QMutex      sMutex;

	std::vector<QPointF> m_boundPoings;
	double  m_space_width; //喷药幅宽
	FertilizerPrescriptionVec  m_fertiliPresVec;
	QString m_currentFieldID = "";
	QString m_fieldName = "";
	HeatMap3D  *m_map3D = nullptr;
	//存等级颜色
	std::unordered_map<int, QList<QPair<QString, std::string>>>				m_mDiffusionLayerHexColor;
	std::unordered_map<int, std::vector<osgEarth::Symbology::Color>>			m_mDiffusionLayerOsgColor;
	std::vector<QString>		m_colorVec;
	osgEarth::Symbology::Color hexToColor(const std::string& hexColor, float rgb_a  = 1.0f);

	QString parseValidBit(int valid_identify);
	bool isValidIdentify(int valid_identify) {
		// 检查是否只有一个位被设置为1
		return valid_identify != 0 && (valid_identify & (valid_identify - 1)) == 0;
	}

signals:

public slots:
};

#endif // LY_AGRIFERTILIZEPESCVIEWMODEL_H