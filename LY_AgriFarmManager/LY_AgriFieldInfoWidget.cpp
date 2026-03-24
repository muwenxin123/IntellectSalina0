#include "LY_AgriFieldInfoWidget.h"
#include "ui_LY_AgriFieldInfoWidget.h"

LY_AgriFieldInfoWidget::LY_AgriFieldInfoWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LY_AgriFieldInfoWidget)
{
    ui->setupUi(this);


	//QPixmap bkgnd(":/images/AgriFarm/temp.png");
	//bkgnd = bkgnd.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	//QPalette palette;
	//palette.setBrush(QPalette::Background, bkgnd);
	//setPalette(palette);
	//setAutoFillBackground(true); // 关键点：启用自动填充背景

	setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
	setAttribute(Qt::WA_TranslucentBackground);
	setAttribute(Qt::WA_ShowWithoutActivating);


	//ui->widget->setAttribute(Qt::WA_TranslucentBackground);
	ui->widget->setObjectName("parentWidget");


	ui->widget->setStyleSheet("QWidget#parentWidget {background-image: url(:/images/AgriFarm/temp.png)};");



}

LY_AgriFieldInfoWidget::~LY_AgriFieldInfoWidget()
{
    delete ui;
}
