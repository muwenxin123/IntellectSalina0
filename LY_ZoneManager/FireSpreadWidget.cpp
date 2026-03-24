#include "FireSpreadWidget.h"
#include "ui_FireSpreadWidget.h"
#include "IZoneManagerActivator.h"
#include "qglobal.h"
#include "QPolygon"
#include "Zone\ZoneManagerInfo.h"

using namespace qnzkna::ZoneManage;

FireSpreadWidget::FireSpreadWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FireSpreadWidget)
{
    ui->setupUi(this);

	m_pIFireSpreadService = IZoneManagerActivator::Get().getPIFireSpreadService();
	m_pIMarkManageService = IZoneManagerActivator::Get().getPIMarkManageService();
	m_pINebulaTaskManageService = IZoneManagerActivator::Get().getPTaskManageService();
	m_pIZoneManageService = IZoneManagerActivator::Get().getPIZoneManageService();

	if (m_pIMarkManageService && m_pINebulaTaskManageService) {
		int nActionID = IZoneManagerActivator::Get().getPTaskManageService()->GetActivatingActionID();
		m_MarkInfoVec.clear();
		CRelationActionMarkVec m_RelationActionMarkVec = IZoneManagerActivator::Get().getPTaskManageService()->GetRelationMark(nActionID);

		for (auto data : m_RelationActionMarkVec) {
			SystemMarkInfo stinfo = m_pIMarkManageService->GetMark(data.GetMarkID());
			if (stinfo.GetID() != "") {
				m_MarkInfoVec.push_back(stinfo);
				ui->cb_markIDlist->addItem(QString::fromStdString(stinfo.GetID()));
			}
		}
		if (m_MarkInfoVec.size() > 0) {
			ui->le_markname->setText(QString::fromStdString(m_MarkInfoVec[0].GetName()));
			ui->le_firelon->setText(QString::number(m_MarkInfoVec[0].GetLon(), 'f', 7));
			ui->le_firelat->setText(QString::number(m_MarkInfoVec[0].GetLat(), 'f', 7));
		}
	}

	connect(ui->cb_markIDlist, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [&](int index) {
		if (m_MarkInfoVec.size() > index) {
			ui->le_markname->setText(QString::fromStdString(m_MarkInfoVec[index].GetName()));
			ui->le_firelon->setText(QString::number(m_MarkInfoVec[index].GetLon(), 'f', 7));
			ui->le_firelat->setText(QString::number(m_MarkInfoVec[index].GetLat(), 'f', 7));
		}
	});
}

FireSpreadWidget::~FireSpreadWidget()
{
    delete ui;
}

void FireSpreadWidget::on_pb_forecast_clicked()
{
	if(m_pIZoneManageService)
		m_pIZoneManageService->RemoveCoverZoneBase();

	if (m_pIFireSpreadService) {
		double fire_lon = ui->le_firelon->text().toDouble();
		double fire_lat = ui->le_firelat->text().toDouble();
		double fire_chang = ui->le_chang->text().toDouble();
		double fire_kuan = ui->le_kuan->text().toDouble();
		m_pIFireSpreadService->setInitFireInfo(fire_lon, fire_lat, fire_chang, fire_kuan);

		double spreadtimeSec = ui->le_totaltime->text().toInt() * 60;
		m_pIFireSpreadService->setSpreadTime(spreadtimeSec);
		double timeStepSec = ui->le_timestep->text().toInt() * 60;
		m_pIFireSpreadService->setTimeStep(timeStepSec);

		QVector<int> weatherConditionVec;
		double tempo = ui->le_wendu->text().toInt();
		weatherConditionVec.push_back(tempo);
		double shidu = ui->le_shidu->text().toInt();
		weatherConditionVec.push_back(shidu);
		double fengxiang = ui->cb_fengxiang->currentIndex();
		weatherConditionVec.push_back(fengxiang);
		double fengsu = ui->le_fengsu->text().toInt();
		weatherConditionVec.push_back(fengsu);

		outQueueFireSpread.clear();
		outQueueFireSpread = m_pIFireSpreadService->spreadFire(weatherConditionVec);

		m_pIFireSpreadService->setFireSpreadQueue(outQueueFireSpread);

		int num = 0;
		int tou = 255;
		int red = 255;
		int green = 0;
		int queueSize = outQueueFireSpread.size();
		for (const auto& poiVec : outQueueFireSpread) {
			CZoneBase zone;
			for (const auto& poi : poiVec) {
				zone.stZonePointVec.push_back(qnzkna::ZoneManage::ZonePoint(poi.x(), poi.y()));
			}

			num++;
			QString tempID = QString("Fire") + QString::number(num);
			strcpy(zone.strID, tempID.toLocal8Bit().data());
			zone.nZoneType = CZoneBase::ZoneBaseType::POLY;
			zone.nProperty = 0;
			zone.nLineType = 1;
			zone.nFillType = 1;
			QColor col(red, green, 0, tou);
			unsigned int uiV = (col.blue() & 0xFF) << 16 | (col.green() & 0xFF) << 8 | (col.red() & 0xFF);
			zone.nLineColor = uiV;
			zone.nAboveAlt = 8000;
			zone.nBottomhAlt = 6000;

			m_pIZoneManageService->AddCoverZoneBase(zone);

			tou -= (255 - 20) / queueSize;
			red -= (255 - 100) / queueSize;
			green += 150 / queueSize;
		}
	}
}

void FireSpreadWidget::on_pb_clear_clicked()
{
	if(m_pIFireSpreadService)
		m_pIFireSpreadService->clearFireSpreadQueue();

	if (m_pIZoneManageService)
		m_pIZoneManageService->RemoveCoverZoneBase();
}
