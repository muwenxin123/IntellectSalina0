#include "ly_taskheaderwidget.h"
#include "ui_ly_taskheaderwidget.h"
#include <QCString.h>
#include <QFont>

LY_TaskHeaderWidget::LY_TaskHeaderWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LY_TaskHeaderWidget)
{
    ui->setupUi(this);

	setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
	setAttribute(Qt::WA_TranslucentBackground);
	setAttribute(Qt::WA_ShowWithoutActivating);

	ui->widget->setObjectName("parentWidget");
	ui->widget_2->setObjectName("parentWidget");
	ui->widget_3->setObjectName("parentWidget");
	ui->widget_4->setObjectName("parentWidget");

	ui->widget->setStyleSheet("QWidget#parentWidget {background-image: url(:/images/AgriFarm/task_type_header_bg.png)};");
	ui->widget_2->setStyleSheet("QWidget#parentWidget {background-image: url(:/images/AgriFarm/task_header_bg.png)};");
	ui->widget_3->setStyleSheet("QWidget#parentWidget {background-image: url(:/images/AgriFarm/farm_area_bg.png)};");
	ui->widget_4->setStyleSheet("QWidget#parentWidget {background-image: url(:/images/AgriFarm/field_header_bg.png)};");


	ui->task_type_name->setStyleSheet(" font-size: 41px; color: #00eaff; ");
	ui->task_process->setStyleSheet("font-size: 42px; color: #00eaff; ");
	ui->work_area->setStyleSheet(" font-size: 42px; color: #00eaff; ");
	ui->field_area->setStyleSheet(" font-size: 42px; color: #00eaff; ");

	ui->label_2->setStyleSheet(" font-size: 24px;");
	ui->label_4->setStyleSheet(" font-size: 24px;");
	ui->label_6->setStyleSheet(" font-size: 24px;");
	ui->label_8->setStyleSheet(" font-size: 24px;");

	ui->label_9->setStyleSheet(" font-size: 21px;");
	ui->label_10->setStyleSheet(" font-size: 21px;");
	//ui->label_9->setStyleSheet("QLabel { font-size: 12px; color: #ffffff; }");
	//ui->label_10->setStyleSheet("QLabel { font-size: 12px; color: #ffffff; }");

	ui->label_10->setAlignment(Qt::AlignLeading | Qt::AlignBottom);
	ui->label_9->setAlignment(Qt::AlignLeading | Qt::AlignBottom);

}
void  LY_TaskHeaderWidget::setTaskTypeName(const QString &name)
{
	ui->task_type_name->setText(name);
}
void  LY_TaskHeaderWidget::setTaskProcess(const QString &name)
{
	ui->task_process->setText(name);
}
void  LY_TaskHeaderWidget::setWorkArea(const QString &name)
{
	ui->work_area->setText(name);
}
void  LY_TaskHeaderWidget::setFieldArea(const QString &name)
{
	ui->field_area->setText(name);
}
LY_TaskHeaderWidget::~LY_TaskHeaderWidget()
{
    delete ui;
}
