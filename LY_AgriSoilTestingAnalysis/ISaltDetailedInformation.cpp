#include "ISaltDetailedInformation.h"
#include "ui_ISaltDetailedInformation.h"
#include "QCString.h"
#include <QGraphicsView>
#include <QToolBar>
#include <QStatusBar>
#include "LyMessageBox.h"
#include <QResizeEvent>

#define SCALE_MAX       50		//最大放大比例
#define ZOOMUP_VALUE    1.1		//单次放大比例
#define ZOOMDOWN_VALUE  0.9		//单次缩小比例

ISaltDetailedInformation::ISaltDetailedInformation(QWidget *parent) :
	LyDialog(tr2("详细信息"), parent),
	ui(new Ui::ISaltDetailedInformation)
{
	ui->setupUi(centralWidget());
	this->setResizable(true);
	scene = new QGraphicsScene();
	ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

}

ISaltDetailedInformation::~ISaltDetailedInformation()
{
    delete ui;
}

void ISaltDetailedInformation::Init(double width, double height)
{
	scene->clear();
	QPixmap pixmap("D:\\work\\video\\images1\\01.jpg");
	if (pixmap.isNull()) {
		QMessageBox::warning(this, "错误", "无法加载图片文件！");
		return;
	}

	scene->addPixmap(pixmap);
	ui->graphicsView->setScene(scene);
	ui->graphicsView->resize(QSize(width, height));
	this->resize(QSize(width + 1, height + 1));
}

void ISaltDetailedInformation::resizeEvent(QResizeEvent * event)
{
	scene->clear();

	QPixmap pixmap("D:\\work\\video\\images1\\01.jpg");
	if (pixmap.isNull()) {
		QMessageBox::warning(this, "错误", "无法加载图片文件！");
		return;
	}
	double scale = 1.0;
	double scaleX = static_cast<double>(ui->graphicsView->width()) / (pixmap.width() + 1);
	double scaleY = static_cast<double>(ui->graphicsView->height()) / (pixmap.height() + 1);
	if (scaleX > scaleY)
	{
		scale = scaleY;
	}
	else
	{
		scale = scaleX;
	}

	// 缩放
	pixmap = pixmap.scaled(pixmap.width() * scale, pixmap.height() * scale, Qt::KeepAspectRatio);
	scene->addPixmap(pixmap);
	ui->graphicsView->setScene(scene);
}

void ISaltDetailedInformation::LeftButtonDown(double x, double y)
{
}

void ISaltDetailedInformation::RightButtonDown(double x, double y)
{
}

void ISaltDetailedInformation::MouseRolling(double multiple)
{
}

void ISaltDetailedInformation::SetGraphicsSceneUI()
{
	ui->graphicsView->setRenderHint(QPainter::SmoothPixmapTransform);
	ui->graphicsView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
	// 放大
	zoomInAction = new QAction("放大", this);
	zoomInAction->setShortcut(QKeySequence::ZoomIn);
	connect(zoomInAction, &QAction::triggered, this, [=]() {
		if (!backgroundPixmapItem) return;

		ui->graphicsView->scale(1.2, 1.2);
		currentScale = ui->graphicsView->transform().m11();

	});

	// 缩小
	zoomOutAction = new QAction("缩小", this);
	zoomOutAction->setShortcut(QKeySequence::ZoomOut);
	connect(zoomOutAction, &QAction::triggered, this, [=]() {

		if (!backgroundPixmapItem) return;

		ui->graphicsView->scale(1 / 1.2, 1 / 1.2);
		currentScale = ui->graphicsView->transform().m11();
	});

	// 重置视图
	resetAction = new QAction("重置", this);
	resetAction->setShortcut(Qt::Key_R);
	connect(resetAction, &QAction::triggered, this, [=]() {
		if (!backgroundPixmapItem) return;

		ui->graphicsView->resetTransform();
		ui->graphicsView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
		currentScale = ui->graphicsView->transform().m11();
	});

	// 	statusBar()->addWidget(statusLabel, 1);
	// 	statusBar()->addPermanentWidget(zoomLabel);

	QToolBar *toolbar = new QToolBar;
	toolbar->addAction(zoomInAction);
	toolbar->addAction(zoomOutAction);
	toolbar->addAction(resetAction);
}