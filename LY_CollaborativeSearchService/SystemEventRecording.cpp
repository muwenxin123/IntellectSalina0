#include "SystemEventRecording.h"
#include "LY_CollaborativeSearchService.h"
#include <QVBoxLayout>
#include <QHeaderView>
#include <QCString.h>
#include <QTableWidgetItem>

SystemEventRecording::SystemEventRecording(QWidget *parent) : QDialog(parent)
{
	initUI();
}

SystemEventRecording::~SystemEventRecording()
{

}

void SystemEventRecording::Update(qnzkna::TaskManage::INebulaTaskManageService * sub)
{

}

void SystemEventRecording::initUI()
{
	auto mainVlayout = new QVBoxLayout(this);
	this->setLayout(mainVlayout);

	m_pTableWidget = new QTableWidget(this);
	mainVlayout->addWidget(m_pTableWidget);

	m_pTableWidget->verticalHeader()->setVisible(false);
	m_pTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	m_pTableWidget->setCornerButtonEnabled(false);
	m_pTableWidget->setFocusPolicy(Qt::NoFocus);
	m_pTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_pTableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);

	QStringList hheaderList;
	hheaderList << tr2("时间") << tr2("事件类型") << tr2("详情");
	m_pTableWidget->setColumnCount(hheaderList.size());
	m_pTableWidget->setHorizontalHeaderLabels(hheaderList);
	m_pTableWidget->setHidden(false);
}

void SystemEventRecording::sltRecvEvent(EventInfo info)
{
	if (m_pTableWidget != nullptr) {

		m_pTableWidget->insertRow(0);
		auto it1 = new QTableWidgetItem(info.strTime);
		auto it2 = new QTableWidgetItem(info.strTitle);
		auto it3 = new QTableWidgetItem(info.strDetails);
		it3->setToolTip(info.strDetails);
		m_pTableWidget->setItem(0, 0, it1);
		m_pTableWidget->setItem(0, 1, it2);
		m_pTableWidget->setItem(0, 2, it3);
		if (info.strTitle == tr2("告警事件"))
		{
			it1->setBackground(QBrush(QColor(255, 0, 0)));
			it2->setBackground(QBrush(QColor(255, 0, 0)));
			it3->setBackground(QBrush(QColor(255, 0, 0)));
		}

	}
}

