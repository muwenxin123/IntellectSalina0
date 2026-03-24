#include <QTimer>

#include "LyMessageBox.h"
#include "AgriMultispectralSensorWidget.h"
#include "ui_AgriMultispectralSensorWidget.h"
#include "AgriMultispectralSensorController.h"


AgriMultispectralSensorWidget::AgriMultispectralSensorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AgriMultispectralSensorWidget),
    m_show(false)
{
    ui->setupUi(this);

	Init();
}

AgriMultispectralSensorWidget::~AgriMultispectralSensorWidget()
{
    delete ui;
}

void AgriMultispectralSensorWidget::changeEvent(QEvent *event)
{
	QWidget::changeEvent(event);
}

void AgriMultispectralSensorWidget::showEvent(QShowEvent *event)
{
    m_show = true;
}

void AgriMultispectralSensorWidget::hideEvent(QHideEvent *event)
{
    m_show = false;
}

void AgriMultispectralSensorWidget::Init()
{
	// 设置轮询状态查询
	m_statusTimer = new QTimer(this);
	m_statusTimer->stop();

	connect(m_statusTimer, &QTimer::timeout, this, &AgriMultispectralSensorWidget::on_timeout_status);
	m_statusTimer->setInterval(1000 * 10);
	m_statusTimer->start();
}

void AgriMultispectralSensorWidget::HandleMSIStatus(const AgriMSICameraStatusInfoPtr & status)
{
	// 更新界面展示
}

void AgriMultispectralSensorWidget::on_timeout_status()
{
	if (m_show)
	{
		// 查询摄像头状态
		MSIController().asyncGetRequest_status(std::bind(&AgriMultispectralSensorWidget::HandleMSIStatus, this, std::placeholders::_1));
	}
}
