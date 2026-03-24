#include <QApplication>
#include <QHeaderView>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QMenu>
#include <QPainter>
#include <QSettings>
#include <QCheckBox>

#include "CreateShape.h"
#include "ShapeClip.h"
#include "DrawShape.h"
#include "LyMessageBox.h"
#include "SaltFieldEventAnalysisDetailWgt.h"
#include "ui_SaltFieldEventAnalysisDetailWgt.h"
#include "LY_AgriSoilAnalysisPredict.h"
#include "AgriSoilTestingDataSetDialog.h"
#include "AgriSoilAnalysisPredictUtil.h"
#include "AgriSoilAnalysisPredictAlgorithm.h"
#include "LY_AgriSoilAnalysisPredict.h"

SaltFieldEventAnalysisDetailWgt::SaltFieldEventAnalysisDetailWgt(QWidget *parent) 
	: QWidget(parent),
    ui(new Ui::SaltFieldEventAnalysisDetailWgt)
{
    ui->setupUi(this);

	// 初始化参数
	auto eventTypeMap = LY_AgriSoilAnalysisPredict::Get().GetpDbHandler()->GetSaltFieldEventTypeIDMap();
	QList<QPair<QString, QVariant>> optionList;
	for (auto& data : eventTypeMap)
		m_eventID2DataMap.insert(data.id, data);

	ui->lbl_picData->setScaledContents(true);
}

SaltFieldEventAnalysisDetailWgt::~SaltFieldEventAnalysisDetailWgt()
{
    delete ui;
}

void SaltFieldEventAnalysisDetailWgt::setData(const QString& saltFieldName, SaltFieldEventInfoPtr soilTestingDataset)
{
	ui->let_eventName->setText(soilTestingDataset->name);
	ui->let_eventType->setText(m_eventID2DataMap[soilTestingDataset->eventType].name);
	ui->let_eventPos->setText(GetWeaponName(soilTestingDataset->sourceFieldID));
	ui->let_eventLon->setText(QString::number(soilTestingDataset->lon));
	ui->let_eventLat->setText(QString::number(soilTestingDataset->lat));
	ui->textEdit->setText(soilTestingDataset->desc);

	QPixmap image;
	image.loadFromData(soilTestingDataset->picData, "JPEG");
	ui->lbl_picData->setPixmap(image);
}

QString SaltFieldEventAnalysisDetailWgt::GetWeaponName(const QString& reporterID)
{
	QString name;
	std::string WeaponComponentName;
	bool isGetWeaponComponentName = IAgriSoilAnalysisPredictActivator::Get().getPIWeaponManageService()->
		GetWeaponComponentName(WeaponComponentName, reporterID.toStdString());
	if (isGetWeaponComponentName)
	{
		name = QString::fromStdString(WeaponComponentName);
	}
	else
	{
		name = reporterID;
	}
	return name;
}

