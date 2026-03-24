#include "LY_ShowWeatherDataInfo.h"
#include "ui_LY_ShowWeatherDataInfo.h"
#include "LY_ShowTaskDataInfo.h"
#include <QScreen>

LY_ShowWeatherDataInfo::LY_ShowWeatherDataInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LY_ShowWeatherDataInfo)
{
    ui->setupUi(this);
	setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
	setWidgetPosition();
	this->installEventFilter(this);  // 过滤窗口本身
									 // 为所有子控件安装事件过滤器
	foreach(QObject *obj, this->children()) {
		obj->installEventFilter(this);
	}
}

LY_ShowWeatherDataInfo::~LY_ShowWeatherDataInfo()
{
    delete ui;
}

void LY_ShowWeatherDataInfo::setWidgetPosition()
{
	// 获取所有屏幕信息
	QList<QScreen*> screens = QGuiApplication::screens();
	// 将控件移动到副屏（假设存在）
	if (screens.size() > 1) {
		QScreen* secondaryScreen = screens[1];
		QRect secondaryGeometry = secondaryScreen->geometry();
		 //计算靠右的x坐标（屏幕右边缘减去控件宽度）
		int x = secondaryGeometry.x() + secondaryGeometry.width() - width() - 5;

		 //设置y坐标（这里设为屏幕顶部下方100像素）
		int y = secondaryGeometry.y() + 1 + 667+1118;
		this->move(x, y);
	}
}

void LY_ShowWeatherDataInfo::SetStyleSheetWidget()
{
	setStyleSheet("QWidget {"
		"background-color: #162130;"
		"color: #A1ADBF;"
		"font-size: 26px;"
		"border: 1px dashed white;"
		"}");
	ui->label->setStyleSheet("QLabel {"
		"font-size: 30px;"
		"color: #FEFEFE;"
		"}");

	ui->widget->setStyleSheet("QWidget#widget {"
		"border: none;"
		"background-image: url(:/images/WhiteBorder.png);"
		"}"
		"QWidget#widget QLabel {"
		"border: none;"
		"background: transparent;"
		"}");
	ui->widget_2->setStyleSheet("QWidget#widget_2 {"
		"border: none;"
		"background-image: url(:/images/WhiteBorder.png);"
		"}"
		"QWidget#widget_2 QLabel {"
		"border: none;"
		"background: transparent;"
		"}");
	ui->widget_3->setStyleSheet("QWidget#widget_3 {"
		"border: none;"
		"background-image: url(:/images/WhiteBorder.png);"
		"}"
		"QWidget#widget_3 QLabel {"
		"border: none;"
		"background: transparent;"
		"}");

	ui->widget_4->setStyleSheet("QWidget#widget_4 {"
		"border: none;"
		"background-image: url(:/images/GreenBorder.png);"
		"}"
		"QWidget#widget_4 QLabel {"
		"border: none;"
		"background: transparent;"
		"color: #1AF9C5;"
		"}");
	ui->widget_5->setStyleSheet("QWidget#widget_5 {"
		"border: none;"
		"background-image: url(:/images/GreenBorder.png);"
		"}"
		"QWidget#widget_5 QLabel {"
		"border: none;"
		"background: transparent;"
		"color: #1AF9C5;"
		"}");
	ui->widget_6->setStyleSheet("QWidget#widget_6 {"
		"border: none;"
		"background-image: url(:/images/GreenBorder.png);"
		"}"
		"QWidget#widget_6 QLabel {"
		"border: none;"
		"background: transparent;"
		"color: #1AF9C5;"
		"}");
}

bool LY_ShowWeatherDataInfo::eventFilter(QObject *obj, QEvent *event)
{
	// 如果不允许点击，过滤掉鼠标按下和释放事件
	if ((event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonRelease || event->type() == QEvent::MouseButtonDblClick)) {
		return true; // 事件被过滤，不会传递给目标对象
	}

	// 其他事件正常处理
	return QWidget::eventFilter(obj, event);
}

void LY_ShowWeatherDataInfo::SetMeteorologicalInfoText(const MeteorologicalInfoText& data)
{
	ui->label->setText(data.title);
	ui->label_2->setText(data.text11);
	ui->label_6->setText(data.text12);
	ui->label_8->setText(data.text13);
	ui->label_4->setText(data.text21);
	ui->label_10->setText(data.text22);
	ui->label_12->setText(data.text23);
}

void LY_ShowWeatherDataInfo::SetMeteorologicalInfoValue(const MeteorologicalInfoValue& data)
{
	QString wd = data.value11 + QString("℃");
	ui->label_wd->setText(wd);
	QString sd = data.value12 + QString("%");
	ui->label_sd->setText(sd);
	QString fs = data.value13 + QString("m/s");
	ui->label_fs->setText(fs);
	QString fx = data.value21 + QString("°");
	ui->label_fx->setText(fx);
	QString qy = data.value22 + QString("mpa");
	ui->label_qy->setText(qy);
	QString gz = data.value23 + QString("Lux");
	ui->label_fushe->setText(gz);
}
