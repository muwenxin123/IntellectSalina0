#ifndef LY_SHOWTASKDATAINFO_H
#define LY_SHOWTASKDATAINFO_H

#include <QWidget>
#include <QTimer>
#include "CircleProgressBar.h"
#include "WaveformWidget.h"
#include "GradientProgressBar.h"
#include "DataManage/AgriSoilTestingAnalysis/AgriSoilTestingAnalysisDecl.h"
namespace Ui {
class LY_ShowTaskDataInfo;
}
class GradientProgressBarLine;
using  namespace AgriSoilTestingFertilizer;
class LY_ShowTaskDataInfo : public QWidget
{
    Q_OBJECT

public:
    explicit LY_ShowTaskDataInfo(QWidget *parent = nullptr);
    ~LY_ShowTaskDataInfo();

	//设置窗口显示位置
	void setWidgetPosition();
public:
	//设置窗口中控件样式
	void SetStyleSheetWidget();
	//初始化环形进度条
	void InitCircleProgressBar();

	//初始化高度波形图
	void InitWaveformWidget();

	//初始化速度进度条
	void InitGradientProgressBar();

	//初始化任务进度进度条
	void InitGradientProgressBarLine();

	//对外接口设置环形的百分比
	//从左往右num依次是1,2,3
	void setProgress(const int& num, const int& value);

protected:
	// 重写事件过滤器
	bool eventFilter(QObject *obj, QEvent *event) override;

public:
	//////////////////数据设置接口////////////////////////////////////////////////////////
	//设置任务进度
	void SetTaskProgress(const TaskProgress& data);
	//任务12个text文本信息
	void SetTaskTextInfo(const TaskTextInfo& data);
	////环形进度条下方文本
	void SetCircularprogressbarText(const CircularprogressbarText &data);
	//环形进度条显示值
	void SetCircularprogressbarValue(const CircularprogressbarValue &data);
	//无人机信息
	void SetUAVinformation(const UAVinformation& data);

private:
    Ui::LY_ShowTaskDataInfo *ui;
	//自定以环形进度条
	//从左到右第一个
	CircleProgressBar *m_circleProgressNum1; 
	//从左到右第二个
	CircleProgressBar *m_circleProgressNum2;
	//从左到右第三个
	CircleProgressBar *m_circleProgressNum3;

	//自定义波形绘制
	WaveformWidget *m_WaveformWidget;
	//自定义速度进度条
	GradientProgressBar *m_GradientProgressBar;
	//自定义任务进度进度条
	CustomProgressBarLine *m_GradientProgressBarLine;
};

#endif // LY_SHOWTASKDATAINFO_H
