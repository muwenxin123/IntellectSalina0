#ifndef CDRAWAXIS_H
#define CDRAWAXIS_H

#include <QWidget>
#include "Zone\AgroTaskInfo.h"
#include "qevent.h"
#include "AgriFarm\AgriFarmManagerDecl.h"

typedef struct CAgrosZZZSection
{
	QString			strName;
	int				nTime;
	CAgrosZZZSection()
	{
		nTime = 0;
		strName = "";
	}
};

typedef std::vector<CAgrosZZZSection> CAgrosZZZSectionVec;

typedef struct CAgrosZZSection
{
	QString					strName;
	int						nTime;
	CAgrosZZZSectionVec		stVec;
	CAgrosZZSection()
	{
		nTime = 0;
		strName = "";
	}
};

typedef std::vector<CAgrosZZSection> CAgrosZZSectionVec;

typedef struct CAgrosZSection
{
	QString					strName;
	int						nTime;
	CAgrosZZSectionVec		stVec;
	CAgrosZSection()
	{
		nTime = 0;
		strName = "";
	}
};

typedef std::vector<CAgrosZSection>		CAgrosZSectionVec;

typedef struct CDrugRatio
{
	int						nType;					//类型： 0：杀虫剂，1：杀菌剂，2：调节剂，3：生长肥，4：帮剂
	QString				stDrugName;		//药剂名称
	int						nRaito;					//计量

	CDrugRatio()
	{
		nType = 0;
		stDrugName = "";
		nRaito = 0;
	}
};
typedef std::vector<CDrugRatio>		CDrugRatioVec;

typedef struct CEquNYCSInfo
{
	QString					strName;						//显示名称
	int							nStartDay;					//开始天数
	int							nEndDay;						//结束天数
	QString					strNYCS;						//参数值
	int							nStates;						//状态,0:已完成，1：进行中，2：未进行
	int							nType;							//类型：数据类型：1：土地相关操作，2：浇水，3：施肥，4：除草，5：虫害，6：病害，7：巡查
	CDrugRatioVec		DrugRatioVec;
	QRect						drawRect;						//描画区域
	CEquNYCSInfo()
	{		
		strName = "";
		nStartDay = 0;
		nEndDay = 0;
		strNYCS = "";
		nType = 0;
		nStates = 0;
	}
};
typedef std::vector<CEquNYCSInfo> CEquNYCSInfoVec;


typedef struct CEquInfo
{
	int								nType;							//类型 1：农机，2：无人机，3：设施
	int								nSubType;					//车辆：1：平地机，2：深松机，3：播种机，4：收割机
																			//无人机：1：喷洒挂载，2：播撒挂载，3：多光谱
																			//设备：1：灌溉设施
	QString						strName;						//名称
	int								nCount;						//数量
	CEquNYCSInfoVec		InfoVec;
	CEquInfo()
	{
		nType = 0;
		nSubType = 0;
		strName = "";
		nCount = 0;
	}
};
typedef std::vector<CEquInfo> CEquInfoVec;
typedef std::map<int, CEquInfoVec> CEquInfoMap;

class CDrawAxis : public QWidget
{
    Q_OBJECT
public:
    explicit CDrawAxis(QWidget *parent = nullptr);

signals:

public slots:

protected:

	virtual void paintEvent(QPaintEvent *event) override;

	virtual void wheelEvent(QWheelEvent *event);

	virtual void resizeEvent(QResizeEvent* e);

	virtual void mousePressEvent(QMouseEvent* e);

	virtual void mouseMoveEvent(QMouseEvent* e);

	virtual void mouseReleaseEvent(QMouseEvent* e);

private:

	//画背景
	void DrawBackGround(QPainter& _Panter);

	//画坐标轴
	void DrawAxisX(QPainter& _Panter);

	//画图例
	void DrawLegend(QPainter& _Panter);

	//画当前位置
	//nDay : T0，播种时间
	void DrawCurrentPos(QPainter& _Panter, const int& nDay,const bool Detentioned);

	qnzkna::AgroTask::CAgroRefDataSubInfoVec GetAgroInfoVecByStates(const QString& strType1, const QString& strType2 = "", const QString& strType3 = "");

	//画甘特图
	void DrawGantt(QPainter& _Panter);

	//测试数据初始化
	void InitDebugData();

public:

	void CalaDrawData();

	void SetFarmlandAttributeInfo(qnzkna::AgroTask::FarmlandAttributeInfo& stInfo);

	void SetAgroRefDataInfoVec(qnzkna::AgroTask::CAgroRefDataInfoVec& InfoVec);

	void SetTimeShowType(const int& nType);

	void SetShowDetailFlag(const bool& bFlag);

	void SetShowMouseTipsFlag(const bool& bFlag);

	void ArrangeData();

	void SetScale(const int& nScale);

	void SetAgriFieldInfo(const AgriFieldInfo& stInfo);

private:

	QRect																		m_DrawRect;

	int																			m_nAxisWidth;

	int																			m_nStartPointX;

	int																			m_nStartPointY;

	int																			m_nGanTTStartY;

	int																			m_nCenterY;

	qnzkna::AgroTask::FarmlandAttributeInfo				m_FarmInfo;

	qnzkna::AgroTask::CAgroRefDataInfoVec				m_AgroInfoVec;

	int																			m_nTimeShowType;			//时间显示方式，0:T0,1:实际

	bool																			m_bShowDetail;				//是否显示细节	

	bool																			m_bShowMouseTips;			//是否显示鼠标提示

	CAgrosZSectionVec													m_AgrosZSectionVec;	

	int																			m_nScale = 0;				//缩放比例

	int																			m_nZoomStates = 0;			//1:放大，-1缩小，0

	int																			m_nScaleCenterX;			//缩放中心X

	int																			m_nSumDays;					//总天数

	bool																			m_bLeftDown = false;		//鼠标左键压下

	int																			m_nMousePressX;				//鼠标压下时X坐标

	CEquInfoMap															m_EquInfoMap;				//农场设备

	double																		m_OneDayStep;				//一天对应长度

	int																			m_nEquCount;				//对应数量

	bool																			m_bInitGanttRect = false;	//初始化甘特图描画区域

	AgriFieldInfo															m_FieldInfo;

};

#endif // CDRAWAXIS_H