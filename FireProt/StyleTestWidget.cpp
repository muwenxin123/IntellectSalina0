#include "StyleTestWidget.h"
#include <QFileSystemModel>
#include <QTreeView>
#include <QTreeWidget>
#include <QCString.h>
#include <QSplashScreen>
#include <QTimer>
#include "LyCalendarDialog.h"
#include "LyColorDialog.h"
#include "LyFileDialog.h"
#include "LyMessageBox.h"
#include "LyInputDialog.h"
#include "LyColorSelector.h"
#include "LyColorSelectDialog.h"
#include "ui_StyleTestWidget.h"
#include "LyWidget.h"
#include "LyCatalogueWidget.h"
#include "VMarksWidget.h"
#include "LyLoadingDialog.h"

#include "LyWaiting.h"

StyleTestWidget::StyleTestWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::StyleTestWidget())
	, m_pWidget(new LyWidget(tr("Normal Window"), this))
	, m_pTableItemDelegate(NULL)
{
	ui->setupUi(this);
	ui->comboBox->addTreeItem(tr("Grade 1"), "100");
	ui->comboBox->addTreeItem(tr("Class 101"), "101", "100");
	ui->comboBox->addTreeItem(tr("Class 102"), "102", "100");
	ui->comboBox->addTreeItem(tr("Class 103"), "103", "100");
	ui->comboBox->addTreeItem(tr("Class 104"), "104", "100");
	ui->comboBox->addTreeItem(tr("Class 105"), "105", "100");
	ui->comboBox->addTreeItem(tr("Grade 2"), "200");
	ui->comboBox->addTreeItem(tr("Class 201"), "201", "200");
	ui->comboBox->addTreeItem(tr("Class 202"), "202", "200");
	ui->comboBox->addTreeItem(tr("Class 203"), "203", "200");
	ui->comboBox->addTreeItem(tr("Class 204"), "204", "200");
	ui->comboBox->addTreeItem(tr("Class 205"), "205", "200");
	ui->comboBox->addTreeItem(tr("Grade 3"), "300");
	ui->comboBox->addTreeItem(tr("Class 301"), "301", "300");
	ui->comboBox->addTreeItem(tr("Class 302"), "302", "300");
	ui->comboBox->addTreeItem(tr("Class 303"), "303", "300");
	ui->comboBox->addTreeItem(tr("Class 304"), "304", "300");
	ui->comboBox->addTreeItem(tr("Class 305"), "305", "300");
	ui->comboBox->addTreeItem(tr("Grade 4"), "400");
	ui->comboBox->addTreeItem(tr("Class 401"), "401", "400");
	ui->comboBox->addTreeItem(tr("Class 402"), "402", "400");
	ui->comboBox->addTreeItem(tr("Class 403"), "403", "400");
	ui->comboBox->addTreeItem(tr("Class 404"), "404", "400");
	ui->comboBox->addTreeItem(tr("Class 405"), "405", "400");
	ui->comboBox->addTreeItem(tr("Grade 5"), "500");
	ui->comboBox->addTreeItem(tr("Class 501"), "501", "500");
	ui->comboBox->addTreeItem(tr("Class 502"), "502", "500");
	ui->comboBox->addTreeItem(tr("Class 503"), "503", "500");
	ui->comboBox->addTreeItem(tr("Class 504"), "504", "500");
	ui->comboBox->addTreeItem(tr("Class 505"), "505", "500");
	ui->comboBox->addTreeItem(tr("Grade 6"), "600");
	ui->comboBox->addTreeItem(tr("Shanghai"), "601", "600");
	ui->comboBox->addTreeItem(tr("Wuhan"), "602", "600");
	ui->comboBox->addTreeItem(tr("Shijiazhuang"), "603", "600");
	ui->comboBox->addTreeItem(tr("Beijing"), "604", "600");
	ui->comboBox->addTreeItem(tr("Tianjin"), "605", "600");
	ui->comboBox->setOnlySelectChild(true);
	ui->comboBox->setSearchStrategy(LyTreeComboBox::StrategyText);
	QStringList texts = ui->comboBox->treeItemParentText("601");

	ui->comboBox->expandAll();
	ui->lonInputWidget->setCoordinateType(LyLonLatInputWidget::Longitude);
	ui->latInputWidget->setCoordinateType(LyLonLatInputWidget::Latitude);
	ui->lonInputWidget->setCoordinateFormat(LyLonLatInputWidget::DegreesMinutesSeconds);
	ui->latInputWidget->setCoordinateFormat(LyLonLatInputWidget::DegreesMinutesSeconds);
	connect(ui->comboBox_3, SIGNAL(currentTextChanged(const QString &)), ui->comboBox, SLOT(setCurrentText(const QString &)));
	ui->customTabWidget->addTab(new QLabel("first page"), tr("first page"));
	ui->customTabWidget->addTab(new QLabel("second page"), tr("second page"));
	ui->customTabWidget->addTab(new QLabel("third page"), tr("third page"));
	LyIconToolButton *btn = new LyIconToolButton(ui->customTabWidget);
	btn->setObjectName("addBtn");
	ui->customTabWidget->addRightCornerBtn(btn);
	btn = new LyIconToolButton(ui->customTabWidget);
	btn->setObjectName("deleteBtn");
	ui->customTabWidget->addRightCornerBtn(btn);
	m_pWidget->setResizable(true);
	m_pWidget->setTransparent(false);
	LyCatalogueWidget *catalogue = new LyCatalogueWidget;
	ui->tabWidget->addTab(catalogue, "CatalogueWidget");
	QGroupBox *gb = NULL;
	catalogue->addCatalogue(tr("Article 1"), tr("Article 1"), gb = new QGroupBox(tr("Article 1")));
	gb->setMinimumHeight(300);
	catalogue->addCatalogue(tr("What is\nArticle 2 for"), tr("What is Article 2 for"), gb = new QGroupBox(tr("Article 2")));
	gb->setMinimumHeight(400);
	catalogue->addCatalogue(tr("Article 3"), tr("Article 3 doesn't do anything"), gb = new QGroupBox(tr("Article 3")));
	gb->setMinimumHeight(500);
	catalogue->addCatalogue(tr("Article 4"), tr("Article 4 doesn't do anything"), gb = new QGroupBox(tr("Article 4")));
	gb->setMinimumHeight(600);
	catalogue->setCatalogueState(2, LY_CS_Fault);
	ui->tableWidget->setSpan(3, 3, 2, 1);
	ui->tableWidget->setSpan(3, 5, 3, 1);
	ui->tableWidget->setEditTriggers(QAbstractItemView::CurrentChanged);
	m_pTableItemDelegate = new TableItemDelegate(ui->tableWidget);
	for(int i = 0; i < ui->tableWidget->rowCount(); ++i)
	{
		QTableWidgetItem *item = new QTableWidgetItem();
		item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
		ui->tableWidget->setItem(i, 0, item);
		ui->tableWidget->setItem(i, 1, new QTableWidgetItem());
	}
	ui->treeWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerItem);
	ui->lineEdit_2->setCoordinateType(LyLonLatLineEdit::Latitude);
	ui->lineEdit->setCoordinateFormat(LyLonLatLineEdit::EWSN);
	ui->lineEdit_2->setCoordinateFormat(LyLonLatLineEdit::EWSN);

	connect(ui->doubleSpinBox, SIGNAL(valueChanged(double)), ui->lineEdit, SLOT(setValue(double)));
	VMarksWidget *markWidget = new VMarksWidget(this);
	markWidget->setMarks(QStringList() << "10000" << "9000" << "8000" << "7000" << "6000" << "5000" << "4000" << "3000" << "2000" << "1000");
	ui->tabWidget->addTab(markWidget, "MarkWidget");
	ui->lineEdit_4->setDaySign(tr("Day"));
}

StyleTestWidget::~StyleTestWidget()
{
}

QString StyleTestWidget::iconPath() const
{
	if(m_pTableItemDelegate)
	{
		return m_pTableItemDelegate->iconPath();
	}
	return QString();
}

void StyleTestWidget::setIconPath(const QString &iconPath)
{
	if(m_pTableItemDelegate)
	{
		m_pTableItemDelegate->setIconPath(iconPath);
	}
}

void StyleTestWidget::on_pushButton_clicked()
{
	m_pWidget->show();
}

void StyleTestWidget::on_selectFileTBtn_clicked()
{
	QString fileName = LyFileDialog::getOpenFileName(this, tr("Select Files"), "../data/conf");
	ui->fileLabel->setText(fileName);
}

void StyleTestWidget::on_selectColorTBtn_clicked()
{
	QColor color = LyColorDialog::getColor(Qt::red, this, tr("Choose color"), QColorDialog::ShowAlphaChannel);
	ui->colorLabel->setStyleSheet(QString("border: none; background: %1;").arg(color.name()));
}

void StyleTestWidget::on_selectColorTBtn_2_clicked()
{
	LyColorSelectDialog colorSelectDialog(this);
	if(colorSelectDialog.exec())
	{
		QColor color = colorSelectDialog.currentColor();
		ui->colorLabel_2->setStyleSheet(QString("border: none; background: %1;").arg(color.name()));
	}
}

void StyleTestWidget::on_selectDateTBtn_clicked()
{
	bool ok = false;
	QDate date = LyCalendarDialog::getDate(this, QDate::currentDate(), &ok);
	if(ok)
	{
		ui->dateLabel->setText(date.toString("yyyy-MM-dd"));
	}
}

void StyleTestWidget::on_selectTimeTBtn_clicked()
{
	bool ok = false;
	QTime time = LyCalendarDialog::getTime(this, QTime::currentTime(), &ok);
	if(ok)
	{
		ui->timeLabel->setText(time.toString("hh:mm:ss"));
	}
}

void StyleTestWidget::on_selectDateTimeTBtn_clicked()
{
	bool ok = false;
	QDateTime dateTime = LyCalendarDialog::getDateTime(this, QDateTime::currentDateTime(), &ok);
	if(ok)
	{
		ui->dateTimeLabel->setText(dateTime.toString("yyyy-MM-dd"));
	}
}

void StyleTestWidget::on_selectDateRangeTBtn_clicked()
{
	QDate begin, end;
	bool flag = LyCalendarDialog::getDateRange(&begin, &end, this, QDate::currentDate(), QDate::currentDate().addDays(5));
	if(flag)
	{
		ui->dateRangeLabel->setText(begin.toString("yyyy-MM-dd|") + end.toString("yyyy-MM-dd"));
	}
}

void StyleTestWidget::on_selectTimeRangeTBtn_clicked()
{
	QTime begin, end;
	bool flag = LyCalendarDialog::getTimeRange(&begin, &end, this, QTime::currentTime(), QTime::currentTime().addSecs(3600));
	if(flag)
	{
		ui->timeRangeLabel->setText(begin.toString("hh:mm:ss|") + end.toString("hh:mm:ss"));
	}
}

void StyleTestWidget::on_selectDateTimeRangeTBtn_clicked()
{
	QDateTime begin, end;
	bool flag = LyCalendarDialog::getDateTimeRange(&begin, &end, this, QDateTime::currentDateTime(), QDateTime::currentDateTime().addDays(5));
	if(flag)
	{
		ui->dateTimeRangeLabel->setText(begin.toString("yyyy-MM-dd hh:mm:ss|") + end.toString("yyyy-MM-dd hh:mm:ss"));
	}
}

void StyleTestWidget::on_informationTBtn_clicked()
{
	LyMessageBox::information(this, tr("Message Prompt"), tr("This is a message alert!"));
}

void StyleTestWidget::on_questionTBtn_clicked()
{
	LyMessageBox::question(this, tr("Inquiry Prompt"), tr("Is this an inquiry prompt?"), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
}

void StyleTestWidget::on_warningTBtn_clicked()
{
	LyMessageBox::warning(this, tr("Alarm Prompt"), tr("This is a alert box!"), QMessageBox::Ok | QMessageBox::Cancel);
}

void StyleTestWidget::on_criticalTBtn_clicked()
{
	LyMessageBox::critical(this, tr("Error Prompt"), tr("This is an error box!"));
}

void StyleTestWidget::on_comboBox_2_currentTextChanged(const QString &id)
{
	ui->comboBox->setCurrentItem(id);
}

void StyleTestWidget::on_tableWidget_cellClicked(int row, int column)
{
	if(column == 0 && ui->tableWidget->item(row, column))
	{
		ui->tableWidget->item(row, column)->setCheckState(ui->tableWidget->item(row, column)->checkState() == Qt::Unchecked ? Qt::Checked : Qt::Unchecked);
	}
}

void StyleTestWidget::on_pushButton_4_clicked(bool checked)
{

	static bool initial = false;
	static QLabel *load = new QLabel(this, Qt::FramelessWindowHint | Qt::Window);
	if(!initial)
	{
		QMovie *movie = new QMovie(":/images/load.gif");
		movie->start();
		load->setMovie(movie);
		load->setAttribute(Qt::WA_TranslucentBackground);
		load->setObjectName("loadLabel");
		load->setAlignment(Qt::AlignCenter);
		initial = true;
	}
	if(checked)
	{
		load->showFullScreen();
	}
	else
	{
		load->hide();
	}
}

void StyleTestWidget::on_toolButton_11_clicked()
{
	LyLoadingDialog *ldd = new LyLoadingDialog("10000 msec to close");
	QTimer::singleShot(10000, ldd, SLOT(close()));
	ldd->show();
}

void StyleTestWidget::on_pushButton_5_clicked()
{

}

void StyleTestWidget::on_pushButton_6_clicked()
{

}

void StyleTestWidget::on_pushButton_7_clicked()
{

}

void StyleTestWidget::on_pushButton_8_clicked()
{
#if 0
	airspace_data dC;
	dC.center = osg::Vec2d(10, 10);
	dC.r = 100000;
	dC.bottomH = 10000;
	dC.topH = 50000;
	dC.topColor = osg::Vec4(1.0, 0.0, 0.0, 0.5);
	dC.bottomColor = osg::Vec4(1.0, 0.0, 0.0, 0.5);
	dC.surfaceColor = osg::Vec4(1.0, 0.0, 0.0, 0.5);
	dC.lineVisible = true;
	dC.lineWidth = 1;
	dC.lineType = 0xFFFF;
	dC.lineColor = osg::Vec4(0.0, 1.0, 0.0, 0.5);
	dC.sideline = false;
	space_Point dP;
	dP.point = osg::Vec2d(0, -0.2);
	dP.bottomH = 10000;
	dP.topH = 50000;
	dP.topColor = osg::Vec4(1.0, 0.0, 0.0, 0.5);
	dP.bottomColor = osg::Vec4(1.0, 0.0, 0.0, 0.5);
	dP.surfaceColor = osg::Vec4(1.0, 0.0, 0.0, 0.5);
	dP.lineVisible = true;
	dP.lineWidth = 1;
	dP.lineType = 0xFFFF;
	dP.lineColor = osg::Vec4(0.0, 1.0, 0.0, 0.5);
	dP.sideline = true;
	space_Polygon dPg;
	dPg.points.push_back(osg::Vec3d(0, -2, 0));
	dPg.points.push_back(osg::Vec3d(0, -1.9, 0));
	dPg.points.push_back(osg::Vec3d(0.1, -1.8, 0));
	dPg.points.push_back(osg::Vec3d(-0.05, -1.7, 0));
	dPg.points.push_back(osg::Vec3d(0.5, -1.6, 0));
	dPg.points.push_back(osg::Vec3d(1.0, -1.4, 0));
	dPg.points.push_back(osg::Vec3d(-0.1, -2.1, 0));
	dPg.bottomH = 10000;
	dPg.topH = 50000;
	dPg.topColor = osg::Vec4(1.0, 0.0, 0.0, 0.5);
	dPg.bottomColor = osg::Vec4(1.0, 0.0, 0.0, 0.5);
	dPg.surfaceColor = osg::Vec4(1.0, 0.0, 0.0, 0.5);
	dPg.lineVisible = true;
	dPg.lineWidth = 1;
	dPg.lineType = 0xFFFF;
	dPg.lineColor = osg::Vec4(0.0, 1.0, 0.0, 0.5);
	dPg.sideline = false;
	m_asmag->createPolygon(dPg);
	space_Line dL;
	dL.points.push_back(osg::Vec3d(0, -1, 0));
	dL.points.push_back(osg::Vec3d(0, -0.9, 0));
	dL.points.push_back(osg::Vec3d(0.1, -0.8, 0));
	dL.points.push_back(osg::Vec3d(-0.05, -0.7, 0));
	dL.points.push_back(osg::Vec3d(0.5, -0.6, 0));
	dL.points.push_back(osg::Vec3d(1.0, -0.4, 0));
	dL.points.push_back(osg::Vec3d(-0.1, -1.1, 0));
	dL.bottomH = 10000;
	dL.topH = 50000;
	dL.topColor = osg::Vec4(1.0, 0.0, 0.0, 0.5);
	dL.bottomColor = osg::Vec4(1.0, 0.0, 0.0, 0.5);
	dL.surfaceColor = osg::Vec4(1.0, 0.0, 0.0, 0.5);
	dL.lineVisible = true;
	dL.lineWidth = 1;
	dL.lineType = 0xFFFF;
	dL.lineColor = osg::Vec4(0.0, 1.0, 0.0, 0.5);
	dL.sideline = false;
	space_Arc dA;
	dA.center = osg::Vec3d(-10, -10, 0);
	dA.startAg = 130;
	dA.endAg = 270;
	dA.r = 500000;
	dA.bottomH = 10000;
	dA.topH = 50000;
	dA.topColor = osg::Vec4(1.0, 0.0, 0.0, 0.5);
	dA.bottomColor = osg::Vec4(1.0, 0.0, 0.0, 0.5);
	dA.surfaceColor = osg::Vec4(1.0, 0.0, 0.0, 0.5);
	dA.lineVisible = true;
	dA.lineWidth = 1;
	dA.lineType = 0xFFFF;
	dA.lineColor = osg::Vec4(0.0, 1.0, 0.0, 0.5);
	dA.sideline = false;
	space_RadArc dR;
	dR.center = osg::Vec3d(10, -10, 0);
	dR.startAg = 60;
	dR.endAg = 270;
	dR.rMax = 500000;
	dR.rMin = 300000;
	dR.bottomH = 10000;
	dR.topH = 50000;
	dR.topColor = osg::Vec4(1.0, 0.0, 0.0, 0.5);
	dR.bottomColor = osg::Vec4(1.0, 0.0, 0.0, 0.5);
	dR.surfaceColor = osg::Vec4(1.0, 0.0, 0.0, 0.5);
	dR.lineVisible = true;
	dR.lineWidth = 1;
	dR.lineType = 0xFFFF;
	dR.lineColor = osg::Vec4(0.0, 1.0, 0.0, 0.5);
	dR.sideline = false;
	space_PolyArc dPA;
	dPA.points.push_back(osg::Vec3d(-4.0, -6.0, 0));
	dPA.points.push_back(osg::Vec3d(-6.5, -4.0, 0));
	dPA.center = osg::Vec3d(-5, -5, 0);
	dPA.startAg = -10;
	dPA.endAg = 170;
	dPA.r = 500000;
	dPA.bottomH = 10000;
	dPA.topH = 50000;
	dPA.topColor = osg::Vec4(1.0, 0.0, 0.0, 0.5);
	dPA.bottomColor = osg::Vec4(1.0, 0.0, 0.0, 0.5);
	dPA.surfaceColor = osg::Vec4(1.0, 0.0, 0.0, 0.5);
	dPA.lineVisible = true;
	dPA.lineWidth = 1;
	dPA.lineType = 0xFFFF;
	dPA.lineColor = osg::Vec4(0.0, 1.0, 0.0, 0.5);
	dPA.sideline = false;
	space_Orbit dO;
	dO.leftCenter = osg::Vec3d(13.0, 13.0, 0.0);
	dO.rightCenter = osg::Vec3d(18.0, 18.0, 0.0);
	dO.r = 50000;
	dO.bottomH = 10000;
	dO.topH = 50000;
	dO.topColor = osg::Vec4(1.0, 0.0, 0.0, 0.5);
	dO.bottomColor = osg::Vec4(1.0, 0.0, 0.0, 0.5);
	dO.surfaceColor = osg::Vec4(1.0, 0.0, 0.0, 0.5);
	dO.lineVisible = true;
	dO.lineWidth = 1;
	dO.lineType = 0xFFFF;
	dO.lineColor = osg::Vec4(0.0, 1.0, 0.0, 0.5);
	dO.sideline = false;
	space_Corridor dCr;
	dCr.points.push_back(osg::Vec3d(0.0, 5.0, 0));
	dCr.points.push_back(osg::Vec3d(0.0, 6.0, 0));
	dCr.points.push_back(osg::Vec3d(1.0, 6.5, 0));
	dCr.points.push_back(osg::Vec3d(2.5, 5.4, 0));
	dCr.points.push_back(osg::Vec3d(3.0, 6.8, 0));
	dCr.points.push_back(osg::Vec3d(2.5, 7.5, 0));
	dCr.points.push_back(osg::Vec3d(2.0, 9.0, 0));
	dCr.points.push_back(osg::Vec3d(1.0, 8.5, 0));
	dCr.points.push_back(osg::Vec3d(-2.5, 7.5, 0));
	dCr.points.push_back(osg::Vec3d(-5.0, 8.5, 0));
	dCr.points.push_back(osg::Vec3d(-6.0, 5.5, 0));
	dCr.width = 50000;
	dCr.bottomH = 10000;
	dCr.topH = 50000;
	dCr.topColor = osg::Vec4(1.0, 0.0, 0.0, 0.5);
	dCr.bottomColor = osg::Vec4(1.0, 0.0, 0.0, 0.5);
	dCr.surfaceColor = osg::Vec4(1.0, 0.0, 0.0, 0.5);
	dCr.lineVisible = true;
	dCr.lineWidth = 1;
	dCr.lineType = 0xFFFF;
	dCr.lineColor = osg::Vec4(0.0, 1.0, 0.0, 0.5);
	dCr.sideline = false;
	space_Track dTo;
	dTo.id = "114";
	dTo.points.push_back(osg::Vec3d(0.0, 0.0, 0));
	dTo.points.push_back(osg::Vec3d(0.0, 1.0, 0));
	dTo.points.push_back(osg::Vec3d(1.0, 1.5, 0));
	dTo.points.push_back(osg::Vec3d(2.5, 0.4, 0));
	dTo.points.push_back(osg::Vec3d(3.0, 1.8, 0));
	dTo.width.push_back(osg::Vec2d(4000, 4000));
	dTo.width.push_back(osg::Vec2d(4000, 4000));
	dTo.width.push_back(osg::Vec2d(4000, 4000));
	dTo.width.push_back(osg::Vec2d(4000, 4000));
	dTo.height.push_back(osg::Vec2d(10000, 4000));
	dTo.height.push_back(osg::Vec2d(11000, 5000));
	dTo.height.push_back(osg::Vec2d(12000, 6000));
	dTo.height.push_back(osg::Vec2d(9000, 3000));
	dTo.topColor = osg::Vec4(1.0, 0.0, 0.0, 0.7);
	dTo.bottomColor = osg::Vec4(0.0, 0.0, 1.0, 0.7);
	dTo.surfaceColor = osg::Vec4(0.5, 0.0, 0.5, 0.5);
	dTo.lineVisible = true;
	dTo.lineWidth = 1;
	dTo.lineType = 0xFFFF;
	dTo.lineColor = osg::Vec4(0.0, 1.0, 0.0, 0.5);
	dTo.sideline = true;
	space_Track dT;
	dT.id = "514";
	dT.points.push_back(osg::Vec3d(72.90244, 17.73498, 0));
	dT.points.push_back(osg::Vec3d(76.41987, 20.16210, 0));
	dT.points.push_back(osg::Vec3d(78.71614, 16.64238, 0));
	dT.points.push_back(osg::Vec3d(75.06159, 14.71636, 0));
	dT.width.push_back(osg::Vec2d(26628, 26628));
	dT.width.push_back(osg::Vec2d(26628, 26628));
	dT.width.push_back(osg::Vec2d(26628, 26628));
	dT.height.push_back(osg::Vec2d(50000, 10000));
	dT.height.push_back(osg::Vec2d(60000, 20000));
	dT.height.push_back(osg::Vec2d(40000, 30000));
	dT.topColor = osg::Vec4(1.0, 0.0, 0.0, 0.7);
	dT.bottomColor = osg::Vec4(1.0, 0.0, 0.0, 0.7);
	dT.surfaceColor = osg::Vec4(1.0, 0.0, 0.0, 0.5);
	dT.lineVisible = true;
	dT.lineWidth = 1;
	dT.lineType = 0xFFFF;
	dT.lineColor = osg::Vec4(0.0, 1.0, 0.0, 0.5);
	dT.sideline = true;
#endif
}

void StyleTestWidget::on_pushButton_9_clicked()
{
}

void StyleTestWidget::on_pushButton_10_clicked()
{
}

void StyleTestWidget::on_pushButton_11_clicked()
{
}

void StyleTestWidget::on_pushButton_12_clicked()
{
}

void StyleTestWidget::on_pushButton_13_clicked()
{
}

void StyleTestWidget::on_pushButton_14_clicked()
{

}

void StyleTestWidget::on_pushButton_15_clicked()
{

}

void StyleTestWidget::on_pushButton_16_clicked()
{

}

void StyleTestWidget::on_pushButton_17_clicked()
{

}

void StyleTestWidget::on_pushButton_18_clicked()
{
	double ar[6][13][3] =
	{
		{ { 0, 25210, 25153 }, { 30, 25953, 25546 }, { 60, 28597, 26637 }, { 90, 33209, 28315 }, { 120, 39174, 29994 }, { 150, 44770, 31924 }, { 180, 47230, 33015 }, { 210, 44770, 31924 }, { 240, 39174, 29994 }, { 270, 33209, 28315 }, { 300, 28597, 26637 }, { 330, 25953, 25546 }, { 360, 25210, 25153 } },
		{ { 0, 28586, 28576 }, { 30, 29524, 29005 }, { 60, 32523, 30552 }, { 90, 37709, 33043 }, { 120, 44458, 35792 }, { 150, 50706, 38111 }, { 180, 53455, 39314 }, { 210, 50706, 38111 }, { 240, 44458, 35792 }, { 270, 37709, 33043 }, { 300, 32523, 30552 }, { 330, 29524, 29005 }, { 360, 28586, 28576 } },
		{ { 0, 32513, 32456 }, { 30, 33552, 32926 }, { 60, 36962, 34663 }, { 90, 42868, 38055 }, { 120, 50722, 41859 }, { 150, 58090, 44920 }, { 180, 61378, 46078 }, { 210, 58090, 44920 }, { 240, 50722, 41859 }, { 270, 42868, 38055 }, { 300, 36962, 34663 }, { 330, 33552, 32926 }, { 360, 32513, 32456 } },
		{ { 0, 35714, 35700 }, { 30, 36859, 36085 }, { 60, 40565, 38022 }, { 90, 46965, 41708 }, { 120, 55453, 46457 }, { 150, 63537, 50206 }, { 180, 67175, 50456 }, { 210, 63537, 50206 }, { 240, 55453, 46457 }, { 270, 46965, 41708 }, { 300, 40565, 38022 }, { 330, 36859, 36085 }, { 360, 35714, 35700 } },
		{ { 0, 39543, 39506 }, { 30, 40834, 39727 }, { 60, 44883, 41326 }, { 90, 51985, 45262 }, { 120, 61676, 51042 }, { 150, 70636, 55224 }, { 180, 74486, 53809 }, { 210, 70636, 55224 }, { 240, 61676, 51042 }, { 270, 51985, 45262 }, { 300, 44883, 41326 }, { 330, 40834, 39727 }, { 360, 39543, 39506 } },
		{ { 0, 42221, 42209 }, { 30, 43615, 42725 }, { 60, 48042, 43984 }, { 90, 55490, 48264 }, { 120, 65046, 54181 }, { 150, 73268, 57642 }, { 180, 76500, 54999 }, { 210, 73268, 57642 }, { 240, 65046, 54181 }, { 270, 55490, 48264 }, { 300, 48042, 43984 }, { 330, 43615, 42725 }, { 360, 42221, 42209 } }
	};
	QVector<QVector<QPointF>> res0, res1;
	QVector<double> helist0, helist1;
	for(int i = 0; i < 6; i++)
	{
		QVector<QPointF> tmp, tmp1;
		for(int j = 0; j < 13; j++)
		{
			tmp.push_back(QPointF(ar[i][j][0], ar[i][j][1]));
			tmp1.push_back(QPointF(ar[i][j][0], ar[i][j][2]));
		}
		res0.push_back(tmp);
		res1.push_back(tmp1);
		helist0.push_back(i * 2000 + 6000);
		helist1.push_back(i * 2000 + 6000);
	}

}

void StyleTestWidget::on_pushButton_19_clicked()
{
}

void StyleTestWidget::on_toolButton_12_clicked()
{
	LyWaiting::start(tr("Waiting for tests..."), LyWaiting::CIRCLE);
	QTimer::singleShot(5000, this, []()
	{
		LyWaiting::finish();
	});
}

void StyleTestWidget::on_pushButton_20_clicked()
{

}

void StyleTestWidget::on_pushButton_21_clicked()
{

}

void StyleTestWidget::on_pushButton_22_clicked()
{

}

void StyleTestWidget::on_pushButton_23_clicked()
{

}

void StyleTestWidget::on_pushButton_24_clicked()
{

}

void StyleTestWidget::on_pushButton_25_clicked()
{

}

void StyleTestWidget::on_pushButton_26_clicked()
{

}

void StyleTestWidget::on_pushButton_27_clicked()
{

}

void StyleTestWidget::on_pushButton_28_clicked()
{
}

void StyleTestWidget::on_pushButton_29_clicked()
{
}

void StyleTestWidget::on_pushButton_30_clicked()
{
}

void StyleTestWidget::on_pushButton_31_clicked()
{

}

void StyleTestWidget::on_pushButton_32_clicked()
{

}

void StyleTestWidget::on_pushButton_33_clicked()
{

}

void StyleTestWidget::on_pushButton_34_clicked()
{

}

void StyleTestWidget::on_pushButton_35_clicked()
{

}

void StyleTestWidget::on_pushButton_36_clicked()
{

}

void StyleTestWidget::on_pushButton_37_clicked()
{

}

void StyleTestWidget::on_pushButton_38_clicked()
{

}

void StyleTestWidget::on_pushButton_39_clicked()
{

}

void StyleTestWidget::on_pushButton_40_clicked()
{

}

void StyleTestWidget::on_pushButton_41_clicked()
{

}

MonthSelector::MonthSelector(QWidget *parent)
	: QWidget(parent)
{
	init();
}

void MonthSelector::setRange(int begin, int end)
{
	m_pBeginSP->setValue(begin);
	m_pEndSP->setValue(end);
}

int MonthSelector::begin()
{
	return m_pBeginSP->value();
}

int MonthSelector::end()
{
	return m_pEndSP->value();
}

void MonthSelector::init()
{
	QHBoxLayout *hLayout = new QHBoxLayout(this);
	hLayout->setMargin(0);
	hLayout->setSpacing(0);
	hLayout->addWidget(m_pBeginSP = new QSpinBox());
	QLabel *label = new QLabel("-");
	label->setAlignment(Qt::AlignCenter);
	label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
	hLayout->addWidget(label);
	hLayout->addWidget(m_pEndSP = new QSpinBox());
	m_pBeginSP->setRange(1, 12);
	m_pEndSP->setRange(1, 12);
}

TableItemDelegate::TableItemDelegate(QAbstractItemView *view)
	: LyStyledItemDelegate(view)
	, m_strIconPath(":/images/1k/black/icon/Tool/18/")
{
}

void TableItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	LyStyledItemDelegate::paint(painter, option, index);
	QPixmap pixmap;
	if(index.column() == 0)
	{
		Qt::CheckState state = static_cast<Qt::CheckState>(index.data(Qt::CheckStateRole).toInt());
		if(state == Qt::Checked)
		{
			pixmap.load(m_strIconPath + "ignore_checked.png");
		}
		else
		{
			pixmap.load(m_strIconPath + "ignore_normal.png");
		}
		painter->save();
		painter->setPen(Qt::NoPen);
		if(option.features & QStyleOptionViewItem::Alternate)
		{
			painter->setBrush(option.palette.alternateBase().color());
		}
		else
		{
			painter->setBrush(option.palette.background().color());
		}
		painter->drawRect(option.rect.adjusted(1, 1, -1, -1));
		painter->restore();
	}
	else if(index.column() == 1)
	{
		if(option.state & QStyle::State_MouseOver)
		{
			pixmap.load(m_strIconPath + "refresh conflict_hover.png");
		}
		else
		{
			pixmap.load(m_strIconPath + "refresh conflict_normal.png");
		}
	}
	if(!pixmap.isNull())
	{
		painter->drawPixmap(pixmap.rect().translated(option.rect.center() - pixmap.rect().center()), pixmap);
	}
}

QWidget *TableItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	if(index.column() == 2)
	{

		MonthSelector *ms = new MonthSelector(parent);
		return ms;
	}
	return QStyledItemDelegate::createEditor(parent, option, index);
}

void TableItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	if(index.column() == 2)
	{

		MonthSelector *ms = static_cast<MonthSelector *>(editor);
		QStringList months = index.model()->data(index, Qt::EditRole).toString().split("/");
		if(months.size() == 2)
		{
			ms->setRange(months.first().toInt(), months.last().toInt());
		}
		else
		{
			ms->setRange(1, 12);
		}
	}
	else
	{
		QStyledItemDelegate::setEditorData(editor, index);
	}
}

void TableItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	if(index.column() == 2)
	{

		MonthSelector *ms = static_cast<MonthSelector *>(editor);
		model->setData(index, QString("%1/%2").arg(ms->begin()).arg(ms->end()), Qt::EditRole);
	}
	else
	{
		QStyledItemDelegate::setModelData(editor, model, index);
	}
}

void TableItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	editor->setGeometry(option.rect.adjusted(1, 0, -1, 0));
}

void TableItemDelegate::setIconPath(const QString &iconPath)
{
	m_strIconPath = iconPath;
}
