#include "LY_ShowTaskDataInfo.h"
#include "ui_LY_ShowTaskDataInfo.h"
#include "LY_ShowVideoData.h"
#include "QTimer"
#include <QHBoxLayout>
#include <QCString.h>

LY_ShowTaskDataInfo::LY_ShowTaskDataInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LY_ShowTaskDataInfo)
{
    ui->setupUi(this);
	setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
	setWidgetPosition();
	InitCircleProgressBar();
	InitWaveformWidget();
	InitGradientProgressBar();
	InitGradientProgressBarLine();
	this->installEventFilter(this);  // 过滤窗口本身
	// 为所有子控件安装事件过滤器
	foreach(QObject *obj, this->children()) {
		obj->installEventFilter(this);
	}
}

LY_ShowTaskDataInfo::~LY_ShowTaskDataInfo()
{
	if (m_circleProgressNum1)
	{
		delete m_circleProgressNum1;
	}
	if (m_circleProgressNum2)
	{
		delete m_circleProgressNum2;
	}
	if (m_circleProgressNum3)
	{
		delete m_circleProgressNum3;
	}
    delete ui;
}

void LY_ShowTaskDataInfo::setWidgetPosition()
{
	// 获取所有屏幕信息
	QList<QScreen*> screens = QGuiApplication::screens();
	// 将控件移动到副屏（假设存在）
	if (screens.size() > 1) {
		QScreen* secondaryScreen = screens[1];
		QRect secondaryGeometry = secondaryScreen->geometry();
		// 计算靠右的x坐标（屏幕右边缘减去控件宽度）
		int x = secondaryGeometry.x() + secondaryGeometry.width() - width() - 5;

		// 设置y坐标（这里设为屏幕顶部下方100像素）
		int y = secondaryGeometry.y() + 1 + 667;
		this->move(x, y);
	}
}

void LY_ShowTaskDataInfo::SetStyleSheetWidget()
{
	setStyleSheet("QWidget {"
		"background-color: #162130;"
		"color: #A1ADBF;"
		"font-size: 20px;"
		"border: 1px dashed white;"
		"}");
	ui->label->setStyleSheet("QLabel {"
		"font-size: 30px;"
		"color: #FEFEFE;"
		"}");
	ui->label_2->setStyleSheet("QLabel {"
		"font-size: 24px;"
		"}");
	ui->label_5->setStyleSheet("QLabel {"
		"font-size: 24px;"
		"}");
	ui->label_rwjd->setStyleSheet("QLabel {"
		"font-size: 24px;"
		"background-color: #4F5866;"
		"}");
	ui->label_rwhs->setStyleSheet("QLabel {"
		"font-size: 24px;"
		"background-color: #4F5866;"
		"}");

	ui->label_6->setStyleSheet("QLabel {"
		"font-size: 24px;"
		"background-color: #254458;"
		//"border: 3px solid #254458"
		"}");
	ui->label_ahlx->setStyleSheet("QLabel {"
		"font-size: 24px;"
		"background-color: #131926;"
		//"border: 3px solid #254458"
		"}");
	ui->label_8->setStyleSheet("QLabel {"
		"font-size: 24px;"
		"background-color: #254458;"
		//"border: 3px solid #254458"
		"}");
	ui->label_bdsl->setStyleSheet("QLabel {"
		"font-size: 24px;"
		"background-color: #131926;"
		//"border: 3px solid #254458"
		"}");
	ui->label_10->setStyleSheet("QLabel {"
		"font-size: 24px;"
		"background-color: #254458;"
		//"border: 3px solid #254458"
		"}");
	ui->label_hxcdd->setStyleSheet("QLabel {"
		"font-size: 24px;"
		"background-color: #131926;"
		//"border: 3px solid #254458"
		"}");
	ui->label_12->setStyleSheet("QLabel {"
		"font-size: 24px;"
		"background-color: #254458;"
		//"border: 3px solid #254458"
		"}");
	ui->label_pxcdd->setStyleSheet("QLabel {"
		"font-size: 24px;"
		"background-color: #131926;"
		//"border: 3px solid #254458"
		"}");
	ui->label_16->setStyleSheet("QLabel {"
		"font-size: 24px;"
		"background-color: #254458;"
		//"border: 3px solid #254458"
		"}");
	ui->label_fxgd->setStyleSheet("QLabel {"
		"font-size: 24px;"
		"background-color: #131926;"
		//"border: 3px solid #254458"
		"}");
	ui->label_17->setStyleSheet("QLabel {"
		"font-size: 24px;"
		"background-color: #254458;"
		//"border: 3px solid #254458"
		"}");
	ui->label_gdrc->setStyleSheet("QLabel {"
		"font-size: 24px;"
		"background-color: #131926;"
		//"border: 3px solid #254458"
		"}");
		
	ui->label_18->setStyleSheet("QLabel {"
		"font-size: 24px;"
		"color: #FEFEFE;"
		"}");

	ui->widget_sjtj1->setStyleSheet("QWidget {"
		"background-color: #162130;"
		"border: none"	
		"}");
	ui->widget_sjtj2->setStyleSheet("QWidget {"
		"background-color: #162130;"
		"border: none"
		"}");
	ui->widget_sjtj3->setStyleSheet("QWidget {"
		"background-color: #162130;"
		"border: none"
		"}");

	ui->label_wrjname->setStyleSheet("QLabel {"
		"font-size: 24px;"
		"color: #FEFEFE;"
		"}");
	ui->label_wrjname1->setStyleSheet("QLabel {"
		"background-color:#1C3748;"
		"font-size: 24px;"
		"color: #1AF9C5;"
		"}");

	ui->label_24->setStyleSheet("QLabel {"
		"background-color:#1C3748;"
		"}");
	ui->label_29->setStyleSheet("QLabel {"
		"background-color:#1C3748;"
		"}");
	ui->label_27->setStyleSheet("QLabel {"
		"background-color:#1C3748;"
		"}");
	ui->label_31->setStyleSheet("QLabel {"
		"background-color:#1C3748;"
		"}");

	ui->label_dl->setStyleSheet("QLabel {"
		"background-color:#131926;"
		"}");
	ui->label_kzms->setStyleSheet("QLabel {"
		"background-color:#131926;"
		"}");
	ui->label_dy->setStyleSheet("QLabel {"
		"background-color:#131926;"
		"}");
	ui->label_fxjd->setStyleSheet("QLabel {"
		"background-color:#131926;"
		"}");

	ui->label_32->setStyleSheet("QLabel {"
		"font-size: 24px;"
		"}");
	ui->label_35->setStyleSheet("QLabel {"
		"font-size: 24px;"
		"}");
	ui->label_36->setStyleSheet("QLabel {"
		"font-size: 24px;"
		"}");


	//背景图片
	ui->widget_5->setStyleSheet("QWidget#widget_5 {"
		"border: none;"
		"background-image: url(:/images/Position1.png);"
		"}");
	ui->widget_6->setStyleSheet("QWidget#widget_6 {"
		"border: none;"
		"background-image: url(:/images/Position1.png);"
		"}");

	ui->label_lon->setStyleSheet("QLabel {"
		"font-size: 20px;"
		"border: none;"
		"background: transparent;"
		"}");
	ui->label_lat->setStyleSheet("QLabel {"
		"font-size: 20px;"
		"border: none;"
		"background: transparent;"
		"}");

	ui->label_sd->setStyleSheet("QLabel {"
		"color: #1AF9C5;"
		"}");
}

void LY_ShowTaskDataInfo::InitCircleProgressBar()
{
	// 创建 CircleProgressBar 对象
	m_circleProgressNum1 = new CircleProgressBar(this);
	// 将进度条添加到主窗口的某个布局中（例如 centralWidget）
	QHBoxLayout *layoutNum1 = new QHBoxLayout(ui->widget_sjtj1);
	layoutNum1->addWidget(m_circleProgressNum1, Qt::AlignCenter);  // 居中显示
	m_circleProgressNum1->setProgress(46);  // 设置初始进度为 46%

	// 创建 CircleProgressBar 对象
	m_circleProgressNum2 = new CircleProgressBar(this);
	// 将进度条添加到主窗口的某个布局中（例如 centralWidget）
	QHBoxLayout *layoutNum2 = new QHBoxLayout(ui->widget_sjtj2);
	layoutNum2->addWidget(m_circleProgressNum2, Qt::AlignCenter);  // 居中显示
	m_circleProgressNum2->setProgress(50);  // 设置初始进度为 46%

	// 创建 CircleProgressBar 对象
	m_circleProgressNum3 = new CircleProgressBar(this);
	// 将进度条添加到主窗口的某个布局中（例如 centralWidget）
	QHBoxLayout *layoutNum3 = new QHBoxLayout(ui->widget_sjtj3);
	layoutNum3->addWidget(m_circleProgressNum3, Qt::AlignCenter);  // 居中显示
	m_circleProgressNum3->setProgress(0);  // 设置初始进度为 46%
}

void LY_ShowTaskDataInfo::InitWaveformWidget()
{
	// 创建 WaveformWidget 对象
	m_WaveformWidget = new WaveformWidget(this);
	// 将波形图添加到主窗口的某个布局中（例如 centralWidget）
	QHBoxLayout *layout = new QHBoxLayout(ui->widget_gd);
	layout->addWidget(m_WaveformWidget,Qt::AlignCenter);  // 添加到布局
}

void LY_ShowTaskDataInfo::InitGradientProgressBar()
{
	// 创建自定义进度条
	m_GradientProgressBar = new GradientProgressBar(this);
	QHBoxLayout *layout = new QHBoxLayout(ui->widget_sd);
	layout->addWidget(m_GradientProgressBar,Qt::AlignCenter);
	m_GradientProgressBar->setGradientColors(QColor(0, 180, 150), QColor(0, 255, 200)); // 设置渐变颜色
	m_GradientProgressBar->setGradientSize(10);
	m_GradientProgressBar->setProgress(0);
}

void LY_ShowTaskDataInfo::InitGradientProgressBarLine()
{
	// 创建自定义进度条
	m_GradientProgressBarLine = new CustomProgressBarLine(this);
	QHBoxLayout *layout = new QHBoxLayout(ui->widget_rwjd);
	layout->addWidget(m_GradientProgressBarLine, Qt::AlignCenter);
	m_GradientProgressBarLine->setGradientColors(QColor(0, 180, 150), QColor(0, 255, 200)); // 设置渐变颜色
	//m_GradientProgressBarLine->setGradientSize(100);
	m_GradientProgressBarLine->setProgress(0.25);
}

void LY_ShowTaskDataInfo::setProgress(const int& num, const int& value)
{
	switch (num)
	{
	case 1:
		m_circleProgressNum1->setProgress(value);
		break;
	case 2:
		m_circleProgressNum1->setProgress(value);
		break;
	case 3:
		m_circleProgressNum1->setProgress(value);
		break;
	}
}

bool LY_ShowTaskDataInfo::eventFilter(QObject *obj, QEvent *event)
{
	// 如果不允许点击，过滤掉鼠标按下和释放事件
	if ((event->type() == QEvent::MouseButtonPress ||event->type() == QEvent::MouseButtonRelease ||event->type() == QEvent::MouseButtonDblClick)) {
		return true; // 事件被过滤，不会传递给目标对象
	}

	// 其他事件正常处理
	return QWidget::eventFilter(obj, event);
}

void LY_ShowTaskDataInfo::SetTaskProgress(const TaskProgress& data)
{
	QString taskprogress = data.progress+QString("%");
	ui->label_rwjd->setText(taskprogress);
	QString time = data.time + QString("S");
	ui->label_rwhs->setText(time);
	QString progress = data.progress;
	qreal num = progress.toDouble() / 100;
	m_GradientProgressBarLine->setProgress(num);
}

void LY_ShowTaskDataInfo::SetTaskTextInfo(const TaskTextInfo& data)
{
	ui->label_6->setText(data.text11);
	ui->label_ahlx->setText(data.text12);
	ui->label_8->setText(data.text13);
	ui->label_bdsl->setText(data.text14);
	ui->label_10->setText(data.text21);
	ui->label_hxcdd->setText(data.text22);
	ui->label_12->setText(data.text23);
	ui->label_pxcdd->setText(data.text24);
	ui->label_16->setText(data.text31);
	ui->label_fxgd->setText(data.text32);
	ui->label_17->setText(data.text33);
	ui->label_gdrc->setText(data.text34);

	update();
}

void LY_ShowTaskDataInfo::SetCircularprogressbarText(const CircularprogressbarText &data)
{
	ui->label_19->setText(data.text1);
	ui->label_20->setText(data.text2);
	ui->label_21->setText(data.text3);
}

void LY_ShowTaskDataInfo::SetCircularprogressbarValue(const CircularprogressbarValue &data)
{
	int num1 = data.value1.toInt();
	int num2 = data.value2.toInt();
	int num3 = data.value3.toInt();
	m_circleProgressNum1->setProgress(num1);
	m_circleProgressNum2->setProgress(num2);
	m_circleProgressNum3->setProgress(num3);
}

void LY_ShowTaskDataInfo::SetUAVinformation(const UAVinformation &data)
{
	QString uavname = tr2("无人机") + data.UAVnum;
	ui->label_wrjname->setText(uavname);
	ui->label_wrjname1->setText(data.UAVnum);
	QString quantity = data.quantity + QString("A");
	ui->label_dy->setText(quantity);
	ui->label_kzms->setText(data.controlmodel);
	QString voltage = data.voltage + QString("V");
	ui->label_dl->setText(voltage);
	ui->label_fxjd->setText(data.flightphase);
	ui->label_lon->setText(data.lon);
	ui->label_lat->setText(data.lat);
	m_WaveformWidget->setWaveformData(data.alt);
	int speedMax = data.speedMax.toInt();
	m_GradientProgressBar->setGradientSize(speedMax);
	double speed = data.speed.toDouble();
	m_GradientProgressBar->setProgress(speed);
	QString strpeed = data.speed + QString("m/s");
	ui->label_sd->setText(strpeed);

}

