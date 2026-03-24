#ifndef LY_SHOWWEATHERDATAINFO_H
#define LY_SHOWWEATHERDATAINFO_H

#include <QWidget>
#include <QGuiApplication>
#include "DataManage/AgriSoilTestingAnalysis/AgriSoilTestingAnalysisDecl.h"
namespace Ui {
class LY_ShowWeatherDataInfo;
}
using namespace AgriSoilTestingFertilizer;
class LY_ShowWeatherDataInfo : public QWidget
{
    Q_OBJECT

public:
    explicit LY_ShowWeatherDataInfo(QWidget *parent = nullptr);
    ~LY_ShowWeatherDataInfo();
	//设置窗口显示位置
	void setWidgetPosition();
	//设置窗口中控件样式
	void SetStyleSheetWidget();
protected:
	// 重写事件过滤器
	bool eventFilter(QObject *obj, QEvent *event) override;

public:


	//气象信息设置接口////////////////////////////////
	//气象信息文本名称
	void SetMeteorologicalInfoText(const MeteorologicalInfoText& data);
	//气象信息文本对应值
	void SetMeteorologicalInfoValue(const MeteorologicalInfoValue& data);
public:

private:
    Ui::LY_ShowWeatherDataInfo *ui;
};

#endif // LY_SHOWWEATHERDATAINFO_H
