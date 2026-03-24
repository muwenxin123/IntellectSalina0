#include "ly_photoanalysisdetailwidget.h"
#include "ui_ly_photoanalysisdetailwidget.h"
#include "QCString.h"


LY_PhotoAnalysisDetailWidget::LY_PhotoAnalysisDetailWidget(QWidget *parent) :
	LyDialog(tr2("虫情分析"), parent),
	ui(new Ui::LY_PhotoAnalysisDetailWidget)
{
	ui->setupUi(centralWidget());
	this->setResizable(true);
	this->setMinimumSize(970, 670);

	networkManager = new QNetworkAccessManager(this);

	connect(ui->pushButton, &QPushButton::clicked, [=]() {
		loadImageFromUrl(QString::fromStdString(m_model.analyseCoordUrl));
	});
}

void LY_PhotoAnalysisDetailWidget::setData(WormRecord model)
{
	m_model = model;
	ui->creat_time->setText(QString::fromStdString(model.createTime));
	ui->device_name->setText(QString::fromStdString(model.deviceAddr));
	ui->analysis_time->setText(QString::fromStdString(model.verifyTime));
	ui->label_6->setText(tr2("备注:") + QString::fromStdString(model.remark));
	ui->label_7->setText(tr2("分析员:") + QString::fromStdString(model.analyst));
	loadImageFromUrl(QString::fromStdString(model.imagesUrl));
}
void LY_PhotoAnalysisDetailWidget::loadImageFromUrl(const QString &url)
{
	QNetworkRequest request(url);
	QNetworkReply *reply = networkManager->get(request);

	connect(reply, &QNetworkReply::finished, [this, reply]() {
		if (reply->error() == QNetworkReply::NoError) {
			QByteArray imageData = reply->readAll();
			QPixmap pixmap;
			pixmap.loadFromData(imageData);

			// 假设你的QLabel名为label
			ui->photo->setPixmap(pixmap.scaled(
				ui->photo->size(),
				Qt::KeepAspectRatio,
				Qt::SmoothTransformation
			));
			ui->photo->setAlignment(Qt::AlignCenter);
		}
		else {
			qDebug() << "Error loading image:" << reply->errorString();
		}
		reply->deleteLater();
	});
}
LY_PhotoAnalysisDetailWidget::~LY_PhotoAnalysisDetailWidget()
{
    delete ui;
}
