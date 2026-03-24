#include "ly_photolayoutwidget.h"
#include "ui_ly_photolayoutwidget.h"
#include <QString>
#include <QDebug>
#include "QCString.h"
#include "LyMessageBox.h"
#include "LY_AgricultureHomepage.h"
#define OPENCV
#include "darknet/yolo_v2_class.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <sstream>
#include <map>
#include <direct.h>
#include <io.h>
#include "PutTextZH.h"
#include <iosfwd>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <QThread>
#include "LY_AgricultureHomepage.h"
#include <QUuid>
#include <osgEarth/GeoMath>
#include <QDateTime>
#include <osg/Matrix>
#include <osg/Vec3>
#include <QBuffer>

LY_PhotoLayoutWidget::LY_PhotoLayoutWidget(QWidget *parent) :
    QWidget(parent),
	ui(new Ui::LY_PhotoLayoutWidget)
	, m_IsRightClick(false)
	, m_CurrentSelectWidget(1)
	, m_CurrentOneWidget(1)
	, m_FullScreenWidget(nullptr)
	, m_IsFullScreenShow(false)

{
	ui->setupUi(this);
	viewModel = LY_AgricultureHomepage::Get().GetViewModel();
	initUi();
}

LY_PhotoLayoutWidget::~LY_PhotoLayoutWidget()
{
	delete ui;
}

void LY_PhotoLayoutWidget::initUi()
{

	photoWidgetVec.clear();

	int number = 0;
	for (int i = 1; i <= 4; ++i) {
		for (int j = 1; j <= 4; ++j) {
			WidgetLayout wLayout;
			number++;
			QLabel* widget = new QLabel();
			widget->setObjectName(QString("Widget_%1").arg(number));
			widget->installEventFilter(this);
			widget->setStyleSheet("border-width: 1px; border-style: solid; border-color: red;");

			QLabel* label = new QLabel(QString(tr2("通道%1")).arg(number));
			label->setStyleSheet("border: none;");

			QVBoxLayout * layout = new QVBoxLayout();
			layout->addWidget(label);

			widget->setLayout(layout);

			ui->gridLayout->addWidget(widget, i, j);

			photoWidgetVec.append(widget);

			//保存所有控件
			wLayout.widget = widget;
			wLayout.label = label;
			wLayout.layout = layout;
			m_AllWidgetList.append(wLayout);
		}
	}
	QMenu* menuOne = new QMenu(tr2("切换1画面"));
	for (int i = 1; i <= 16; ++i) {
		QAction* action = new QAction(QString(tr2("切换%1通道")).arg(i));
		menuOne->addAction(action);
	}
	connect(menuOne, SIGNAL(triggered(QAction*)), this, SLOT(triggeredOne(QAction*)));

	QMenu* menuFour = new QMenu(tr2("切换4画面"));
	for (int i = 0; i < 4; ++i) {
		QAction* action = new QAction(QString(tr2("切换%1-%2通道")).arg(i * 4 + 1).arg((i + 1) * 4));
		menuFour->addAction(action);
	}
	connect(menuFour, SIGNAL(triggered(QAction*)), this, SLOT(triggeredFour(QAction*)));

	QMenu* menuEnght = new QMenu(tr2("切换8画面"));
	for (int i = 0; i < 2; ++i) {
		QAction* action = new QAction(QString(tr2("切换%1-%2通道")).arg(i * 8 + 1).arg((i + 1) * 8));
		menuEnght->addAction(action);
	}
	connect(menuEnght, SIGNAL(triggered(QAction*)), this, SLOT(triggeredEnght(QAction*)));

	QAction* actionSixTeen = new QAction(tr2("切换16画面"));
	connect(actionSixTeen, SIGNAL(triggered(bool)), this, SLOT(cutSixTeenWidget()));

	//QAction* actionFullScreen = new QAction(tr2("全屏显示"));
	//connect(actionFullScreen, SIGNAL(triggered(bool)), this, SLOT(fullScreenShowWidget()));

	m_Menu = new QMenu(this);
	//m_Menu->addAction(actionFullScreen);
	m_Menu->addMenu(menuOne);
	m_Menu->addMenu(menuFour);
	m_Menu->addMenu(menuEnght);
	m_Menu->addAction(actionSixTeen);

	//
	m_FullScreenWidget = new QWidget();
	m_FullScreenWidget->installEventFilter(this);
}

void LY_PhotoLayoutWidget::setPhoto(QVector<pair<QString , QImage>> pVec)
{

	if (pVec.size() == 0)
	{
		return;
	}
	m_pestPhotoVec.clear();

	m_photoVec.clear();

	std::string cfg_file = "../3rdParty/bin/yolo/nongye.cfg";
	std::string weight_file = "../3rdParty/bin/yolo/nongye.weights";
	// 创建检测对象
	Detector my_detector(cfg_file, weight_file, 0);
	float threshold = 0.1;

	FarmPestPhotoInfication photoInfication;
	LY_MointorViewModel *model = LY_AgricultureHomepage::Get().GetViewModel();

	std::map<QString, SensorListModelVec> sensorListMap = LY_AgricultureHomepageActivator::Get().getIAgricultureHomePageService()->GetSensorMap();
 	for (auto pair : pVec)
	{
		if (pair.second.isNull())
		{
			continue;
		}
	
		cv::Mat matImage = QImageToMat(pair.second);
	
		//cv::Mat matImage(image.height(), image.width(), CV_8UC4, (uchar*)image.bits(), image.bytesPerLine());

		if (matImage.empty())
		{
			continue;
		}
		//cv::resize(matImage, matImage, cv::Size(640, 360));

		//yolo检测视频帧
		std::vector<bbox_t> result_vec = my_detector.detect(matImage, threshold);

		for (auto iter = result_vec.begin();
			iter != result_vec.end(); ++iter) {
			cv::Rect rect(iter->x, iter->y, iter->w, iter->h);
			cv::rectangle(matImage, rect, cv::Scalar(0, 255, 0), 2);
			stringstream ss;
			ss << setiosflags(ios::fixed) << setprecision(2) << iter->prob;
			std::string label = name_map2[iter->obj_id] + ":" + ss.str();
		//	qDebug() << "test=====:" << QString::fromStdString(label);
			//cout << iter->x << ", " << iter->y << ": " << label << endl;


			putTextZH(matImage, label.c_str(),
				Point(iter->x, iter->y),
				Scalar(0, 0, 255), 10, "宋体");
		
			if (label.length() > 0)
			{
				for (auto it = sensorListMap.begin(); it != sensorListMap.end(); it++) {
					if (it->second.size() == 0)
					{
						continue;
					}
					for (auto m : it->second)
					{
						if (pair.first == m.mointID)
						{
							//double angle = tan(90.00 - std::abs(m.yj) * (M_PI / 180.0));
							double angle = tan(std::abs(m.yj)* (M_PI / 180.0));
							double distance = 5.00 / angle;

							double R = 6378137; // 球半径
							double sinLat = sin(osg::DegreesToRadians(m.latitude));
							double cosLat = cos(osg::DegreesToRadians(m.latitude));
							double cosDistR = cos(distance / R);
							double sinDistR = sin(distance / R);
							double lat2 = asin(sinLat * cosDistR + cosLat * sinDistR * cos(osg::DegreesToRadians(static_cast<double>(m.fw))));
							double lon2 = osg::DegreesToRadians(m.longitude) + atan2(sin(osg::DegreesToRadians(static_cast<double>(m.fw))) * sinDistR * cosLat, cosDistR - sinLat * sin(lat2));

							osgEarth::GeoMath::destination(osg::DegreesToRadians(m.latitude), osg::DegreesToRadians(m.longitude), osg::DegreesToRadians(static_cast<double>(m.fw)), distance,
								lat2, lon2);

							lon2 = osg::RadiansToDegrees(lon2);
							lon2 = lon2 > 180 ? lon2 - 360 : lon2 < -180 ? lon2 + 360 : lon2;
							lat2 = osg::RadiansToDegrees(lat2);

							LY_AgricultureHomepage::Get().GetViewModel()->draw3DWormImg(lon2, lat2, m.localHeight - 4.00);

							const QByteArray &ba = QUuid::createUuid().toRfc4122();
							QUuid uid = QUuid::fromRfc4122(QByteArray::fromRawData(ba, 16));
							photoInfication.photo_id = uid.toString(QUuid::WithoutBraces);

							QDateTime currentDateTime = QDateTime::currentDateTime();

							// 输出当前时间
							QString currentTime = currentDateTime.toString("yyyy-MM-dd");
							photoInfication.recode_time = currentTime;
							photoInfication.pest_type = QString::fromStdString(label);
							photoInfication.pest_stage = "2";
							photoInfication.localHeight = m.localHeight;
							photoInfication.longitude = m.longitude;
							photoInfication.latitude = m.latitude;
							photoInfication.pest_number = 2;
							photoInfication.recode_range = 10;
							photoInfication.pest_grade = "1";
							photoInfication.mointId = m.mointID;
							//photoInfication.origain_byteArray = original_byteArray;

							m_pestPhotoVec.push_back(photoInfication);
							QImage qImage = QImage((const unsigned char*)(matImage.data),
								matImage.cols,
								matImage.rows,
								matImage.step,
								QImage::Format_RGB888);
							QImage qImage1 = qImage.rgbSwapped().copy();

							m_photoVec.push_back(make_pair(pair.first, qImage1));

							QPixmap recognitionPixmap = QPixmap::fromImage(qImage1);
							QBuffer buffer;
							buffer.open(QIODevice::WriteOnly);
							recognitionPixmap.save(&buffer, "PNG");  // 使用PNG格式保存图像到buffer
							buffer.close();
							QByteArray byteArray = buffer.buffer();  // 获取完整的字节数组
							if (byteArray.size() > 0) {
								photoInfication.recongzine_byteArray = byteArray;
								LY_AgricultureHomepageActivator::Get().getIAgricultureHomePageService()->AddPestHarmSqlData(photoInfication);
								LyMessageBox::information(NULL, tr2("提示"), tr2("保存数据成功"));

							}
						}
					}
				}
			}
		}

		QImage qImage = QImage((const unsigned char*)(matImage.data),
			matImage.cols,
			matImage.rows,
			matImage.step,
			QImage::Format_RGB888);
		QImage qImage1 = qImage.rgbSwapped().copy();

		m_photoVec.push_back(make_pair(pair.first, qImage1));

	}

	this->update();
}


void LY_PhotoLayoutWidget::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);

	if (m_photoVec.size() > 0)
	{
		for (size_t i = 0; i < photoWidgetVec.size(); i++)
		{
			if (i < m_photoVec.size())
			{
				QImage image = m_photoVec.at(i).second;

				if (image.width() <= 0)
				{
					continue;
				}
				QLabel *label = photoWidgetVec.at(i);
				qDebug() << label->size();
				QImage img = image.scaled(label->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

				int x = label->geometry().x();
				int y = label->geometry().y();
				painter.drawImage(QPoint(x, y), img);
			}
		}
		//m_photoVec.clear();
	}
}

bool LY_PhotoLayoutWidget::eventFilter(QObject *watched, QEvent *event)
{
	for (int i = 0; i < m_AllWidgetList.size(); ++i) {
		if (watched == m_AllWidgetList.at(i).widget) {
			if (m_IsRightClick) {
				m_CurrentSelectWidget = i;
				qDebug() << "当前为" << m_CurrentSelectWidget;
				m_Menu->move(cursor().pos());
				m_Menu->show();

				m_IsRightClick = false;
				break;
			}
			if (event->type() == QEvent::MouseButtonDblClick && !m_IsFullScreenShow) {
				m_CurrentSelectWidget = i;
				qDebug() << "当前为" << m_CurrentSelectWidget;
				//fullScreenShowWidget();
			}
		}
	}
	if (watched == m_FullScreenWidget) {
		if (event->type() == QEvent::MouseButtonDblClick && m_IsFullScreenShow) {
			m_AllWidgetList[m_CurrentSelectWidget].widget->setLayout(m_FullScreenWidget->layout());

			m_IsFullScreenShow = false;
			m_FullScreenWidget->close();
		}
	}
	return QWidget::eventFilter(watched, event);
}

void LY_PhotoLayoutWidget::mousePressEvent(QMouseEvent *event)
{

}

void LY_PhotoLayoutWidget::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::RightButton) {
		m_IsRightClick = true;
	}
}

void LY_PhotoLayoutWidget::on_pushButton_clicked()
{
	cutOneWidget(m_CurrentOneWidget);
}

void LY_PhotoLayoutWidget::on_pushButton_2_clicked()
{
	cutFourWidget(m_CurrentOneWidget);
}

void LY_PhotoLayoutWidget::on_pushButton_3_clicked()
{
	cutEnghtWidget(m_CurrentOneWidget);
}

void LY_PhotoLayoutWidget::delAllWidget()
{
	for (int i = 0; i < m_AllWidgetList.size(); ++i) {
		QWidget* w = m_AllWidgetList[i].widget;
		ui->gridLayout->removeWidget(w);
		m_AllWidgetList[i].widget->setVisible(false);
	}
}

int LY_PhotoLayoutWidget::strCutNumber(QString str)
{
	QString number;
	for (int i = 0; i < str.length(); i++) {
		if (str[i] >= '0' && str[i] <= '9') {
			number.append(str[i]);
		}
	}
	return number.toInt();
}

int LY_PhotoLayoutWidget::strCutNumber(QString str, QString sep)
{
	QString number;
	for (int i = 0; i < str.length(); i++) {
		if (str[i] >= '0' && str[i] <= '9') {
			number.append(str[i]);
		}
		if (str[i] == sep) {
			return number.toInt();
		}
	}
	return 1;
}

void LY_PhotoLayoutWidget::triggeredOne(QAction *action)
{
	int number = strCutNumber(action->text());
	m_CurrentOneWidget = number;

	cutOneWidget(number);
}

void LY_PhotoLayoutWidget::triggeredFour(QAction *action)
{
	int number = strCutNumber(action->text(), "-");
	m_CurrentOneWidget = number;

	cutFourWidget(number);
}

void LY_PhotoLayoutWidget::triggeredEnght(QAction *action)
{
	int number = strCutNumber(action->text(), "-");
	m_CurrentOneWidget = number;

	cutEnghtWidget(number);
}

void LY_PhotoLayoutWidget::fullScreenShowWidget()
{
	m_FullScreenWidget->setLayout(m_AllWidgetList[m_CurrentSelectWidget].widget->layout());
	m_FullScreenWidget->showFullScreen();
	m_IsFullScreenShow = true;
}

void LY_PhotoLayoutWidget::cutOneWidget(int indexWidget)
{

	if (m_AllWidgetList.isEmpty() || m_AllWidgetList.size() < indexWidget) {
		return;
	}
	indexWidget--;
	if (indexWidget < 0)
	{
		return;
	}

	delAllWidget();

	ui->gridLayout->addWidget(m_AllWidgetList[indexWidget].widget);
	m_AllWidgetList[indexWidget].widget->setVisible(true);
}

void LY_PhotoLayoutWidget::cutFourWidget(int indexWidget)
{
	if (m_AllWidgetList.isEmpty()) {
		return;
	}
	delAllWidget();

	int number = indexWidget - 1;
	//如果切换布局第一个通道+需要切换的画面数大于总数就--
	while (number + 4 > m_AllWidgetList.size()) {
		number--;
	}

	for (int i = 0; i < 2; ++i) {
		for (int j = 0; j < 2; ++j) {
			ui->gridLayout->addWidget(m_AllWidgetList[number].widget, i, j);
			m_AllWidgetList[number].widget->setVisible(true);

			number++;
		}
	}
}

void LY_PhotoLayoutWidget::cutEnghtWidget(int indexWidget)
{
	if (m_AllWidgetList.isEmpty()) {
		return;
	}
	delAllWidget();

	int number = indexWidget - 1;
	//如果切换布局第一个通道+需要切换的画面数大于总数就--
	while (number + 8 > m_AllWidgetList.size()) {
		number--;
	}

	for (int i = 0; i < 2; ++i) {
		for (int j = 0; j < 4; ++j) {
			ui->gridLayout->addWidget(m_AllWidgetList[number].widget, i, j);
			m_AllWidgetList[number].widget->setVisible(true);

			number++;
		}
	}
}

void LY_PhotoLayoutWidget::cutOneBigSevenLitteWidget(int indexWidget)
{
	if (m_AllWidgetList.isEmpty()) {
		return;
	}
	delAllWidget();

	int number = indexWidget - 1;
	//如果切换布局第一个通道+需要切换的画面数大于总数就--
	while (number + 8 > m_AllWidgetList.size()) {
		number--;
	}

	m_AllWidgetList[number].widget->setVisible(true);
	m_AllWidgetList[number + 1].widget->setVisible(true);
	m_AllWidgetList[number + 2].widget->setVisible(true);
	m_AllWidgetList[number + 3].widget->setVisible(true);
	m_AllWidgetList[number + 4].widget->setVisible(true);
	m_AllWidgetList[number + 5].widget->setVisible(true);
	m_AllWidgetList[number + 6].widget->setVisible(true);
	m_AllWidgetList[number + 7].widget->setVisible(true);

	ui->gridLayout->addWidget(m_AllWidgetList[number].widget, 0, 0, 3, 3);
	ui->gridLayout->addWidget(m_AllWidgetList[number + 1].widget, 0, 3);
	ui->gridLayout->addWidget(m_AllWidgetList[number + 2].widget, 1, 3);
	ui->gridLayout->addWidget(m_AllWidgetList[number + 3].widget, 2, 3);
	ui->gridLayout->addWidget(m_AllWidgetList[number + 4].widget, 3, 3);
	ui->gridLayout->addWidget(m_AllWidgetList[number + 5].widget, 3, 2);
	ui->gridLayout->addWidget(m_AllWidgetList[number + 6].widget, 3, 1);
	ui->gridLayout->addWidget(m_AllWidgetList[number + 7].widget, 3, 0);

}

void LY_PhotoLayoutWidget::cutSixTeenWidget()
{
	if (m_AllWidgetList.isEmpty()) {
		return;
	}
	delAllWidget();

	m_CurrentOneWidget = 1;
	int number = 0;


	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			ui->gridLayout->addWidget(m_AllWidgetList[number].widget, i, j);
			m_AllWidgetList[number].widget->setVisible(true);

			number++;
		}
	}
}

void LY_PhotoLayoutWidget::on_pushButton_4_clicked()
{
	cutSixTeenWidget();
}

void LY_PhotoLayoutWidget::on_pushButton_5_clicked()
{
	cutOneBigSevenLitteWidget(m_CurrentOneWidget);
}
cv::Mat LY_PhotoLayoutWidget::QImageToMat(const QImage &image)
{

	Mat mat;
	switch (image.format())
	{
	case QImage::Format_ARGB32:
	case QImage::Format_ARGB32_Premultiplied:
		mat = Mat(image.height(), image.width(), CV_8UC4, const_cast<uchar*>(image.bits()), image.bytesPerLine());
		break;

	case QImage::Format_RGB32:
	

		mat = Mat(image.height(), image.width(), CV_8UC3);

		for (int y = 0; y < image.height(); ++y) {
			const uchar *srcLine = image.constScanLine(y);
			cv::Vec3b *dstLine = mat.ptr<cv::Vec3b>(y);
			for (int x = 0; x < image.width(); ++x) {
				QRgb pixel = reinterpret_cast<const QRgb*>(srcLine)[x];
				dstLine[x][0] = qBlue(pixel);  // B
				dstLine[x][1] = qGreen(pixel); // G
				dstLine[x][2] = qRed(pixel);   // R
			}
		}

		break;

	case QImage::Format_RGB888:
		mat = Mat(image.height(), image.width(), CV_8UC3, const_cast<uchar*>(image.bits()), image.bytesPerLine());
		cvtColor(mat, mat, COLOR_RGB2BGR);
		break;

	default:
		qWarning() << "Unsupported image format";
		break;
	}
	return  mat;

}
