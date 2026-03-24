#include "LY_ShowVideoData.h"
#include "ui_LY_ShowVideoData.h"

LY_ShowVideoData::LY_ShowVideoData(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LY_ShowVideoData)
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

LY_ShowVideoData::~LY_ShowVideoData()
{
    delete ui;
}

void LY_ShowVideoData::setWidgetPosition()
{
	// 获取所有屏幕信息
	QList<QScreen*> screens = QGuiApplication::screens();

	// 将控件移动到副屏（假设存在）
	if (screens.size() > 1) {
		QScreen* secondaryScreen = screens[1];
		QRect secondaryGeometry = secondaryScreen->geometry();
		// 计算靠右的x坐标（屏幕右边缘减去控件宽度）
		int x= secondaryGeometry.x() + secondaryGeometry.width() - width()-5;

		// 设置y坐标（这里设为屏幕顶部下方100像素）
		int y = secondaryGeometry.y() + 1;
		this->move(x, y);
	}
}

void LY_ShowVideoData::SetStyleSheetWidget()
{
	setStyleSheet("QWidget {"
		"background-color: #162130;"
		"color: #FEFEFE;"
		"font-size: 30px;"
		"border: 1px dashed white;"
		"}"
		"QWidget#widget_up {"
		"background-image: url(:/images/videobackgroundimage.png);"
		"background-size: cover;"
		"border: none;"
		"}"
		"QWidget#widget_left {"
		"background-image: url(:/images/videobackgroundimage1.png);"
		"background-size: contain;"
		"border: none;"
		"}"
		"QWidget#widget_reight {"
		"background-image: url(:/images/videobackgroundimage1.png);"
		"background-size: contain;"
		"border: none;"
		"}");
}

bool LY_ShowVideoData::eventFilter(QObject *obj, QEvent *event)
{
	// 如果不允许点击，过滤掉鼠标按下和释放事件
	if ((event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonRelease || event->type() == QEvent::MouseButtonDblClick)) {
		return true; // 事件被过滤，不会传递给目标对象
	}

	// 其他事件正常处理
	return QWidget::eventFilter(obj, event);
}
