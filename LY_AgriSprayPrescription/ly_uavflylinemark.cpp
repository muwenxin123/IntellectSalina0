#include "ly_uavflylinemark.h"
#include "ui_ly_uavflylinemark.h"
#include <QPainter>
LY_UAVFlyLineMark::LY_UAVFlyLineMark(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LY_UAVFlyLineMark)
{
    ui->setupUi(this);



	setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
	setAttribute(Qt::WA_TranslucentBackground);
	setAttribute(Qt::WA_ShowWithoutActivating);
	

	ui->widget_2->setObjectName("parentWidget");


	ui->widget_2->setStyleSheet("QWidget#parentWidget {background-image: url(:/images/AgriFarm/line_show.png)};");


	ui->label_2->setStyleSheet(" font-size: 20px;");
	ui->label->setStyleSheet(" font-size: 20px;");

	ui->label_2->setAlignment(Qt::AlignRight);
	ui->label->setAlignment(Qt::AlignRight);
	
}

LY_UAVFlyLineMark::~LY_UAVFlyLineMark()
{
    delete ui;
}
