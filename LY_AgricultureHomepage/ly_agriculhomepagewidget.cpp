#include "ly_agriculhomepagewidget.h"
#include "ui_ly_agriculhomepagewidget.h"
#include "LY_AgricultureHomepage.h"

LY_AgriculHomePageWidget::LY_AgriculHomePageWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LY_AgriculHomePageWidget)
{

    ui->setupUi(this);

	//QLabel *backgroundLabel = new QLabel(ui->widget);
	//QPixmap pixmap("../data/images/Rectangle_3.png");
	//backgroundLabel->setPixmap(pixmap.scaled(ui->widget->size())); // 调整大小以适应窗口大小
	////backgroundLabel->setScaledContents(true); // 保持图片比例
	//backgroundLabel->setFixedSize(ui->widget->size());
	//backgroundLabel->raise(); // 确保标签在其他内容之上
	//backgroundLabel->show(); // 显示标签和图片


	LY_AgricultureHomepage::Get().setupWidgetLocal(this, "LY_AgriculHomePageWidget", "Home");
	//ui->widget->setStyleSheet("background-image: url(:/images/mointor/rectangle_1.png);");
	//ui->widget->setStyleSheet("background-image: url(:../data/images/Rectangle_3.png;");
	ui->widget->setStyleSheet(
		"background-image: url(:/images/mointor/rectangle_1.png); "
		//"background-repeat: no-repeat; "
		//"background-position: center; "
	);
	ui->widget_11->setStyleSheet(
		//"background-image: url(:/images/mointor/rectangle_2.png); "
	    "border: 1px solid #1E6CA1;"
		"border-radius: 15px;"
	);

}

LY_AgriculHomePageWidget::~LY_AgriculHomePageWidget()
{
    delete ui;
}
