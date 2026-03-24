#include <QDebug>

#include "AgriSatelliteDataDialog.h"
#include "ui_AgriSatelliteDataDialog.h"
#include "AgriSatelliteDataTable.h"
//#include "AgriSatelliteDownloadManager.h"
//#include "AgriMultispectralSensorUtil.h"


AgriSatelliteDataDialog::AgriSatelliteDataDialog(QWidget *parent /*= nullptr*/)
	: LyDialog("", parent),
	ui(new Ui::AgriSatelliteDataDialog)
{
	ui->setupUi(centralWidget());
	this->setResizable(true);

	Init();
}

AgriSatelliteDataDialog::~AgriSatelliteDataDialog()
{
	delete ui;
}

void AgriSatelliteDataDialog::Init()
{
	/*
	for (int i = 0; i < 3; ++i) { 

		QString clipPath = "F:/wuchunrui/Others/ZS/data/shp/plot_84.shp";
		QString savePath = "D:/sentinel_ndvi/output_" + QString::number(i+1);

		DownloadTask task = { ("task" + QString::number(i + 1)), "", savePath};

		AgriSatelliteDownloadManager::instance().addTask(task);

		QObject::connect(&AgriSatelliteDownloadManager::instance(), &AgriSatelliteDownloadManager::taskProgress,
			[](const QString& id, const QString& msg) {
			// qDebug() << id << "Progress:" << msg;
			if (!id.isEmpty() && !msg.isEmpty()){
				DownloadProgress tProgress = DownloadProgress::fromJson(AgriMultispectralSensorUtil::StringToJson(msg));
				// 更新进度展示

			}
		});

		QObject::connect(&AgriSatelliteDownloadManager::instance(), &AgriSatelliteDownloadManager::taskFinished,
			[](const QString& id) {
			qDebug() << id << "Download finished.";
		});
	}
	*/
}
