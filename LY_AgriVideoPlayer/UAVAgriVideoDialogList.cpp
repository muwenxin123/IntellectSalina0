#include "UAVAgriVideoDialogList.h"
#include "ui_UAVAgriVideoDialogList.h"
#include "QCString.h"
#include "LY_AgriVideoPlayer.h"
#include "ISelectYoloModelDialog.h"
UAVAgriVideoDialogList::UAVAgriVideoDialogList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UAVAgriVideoDialogList)
{
    ui->setupUi(this);
	connect(ui->tabWidget, &QTabWidget::currentChanged, this, &UAVAgriVideoDialogList::tabWidgetChanged);
}

UAVAgriVideoDialogList::~UAVAgriVideoDialogList()
{
    delete ui;
}

void UAVAgriVideoDialogList::InitDialog(std::vector<CameraWidget*> m_pList)
{
	ui->tabWidget->clear();
	if (m_pList.size() > 0)
	{
		int index = 1;
		for (auto &dlgItem : m_pList)
		{
			QString windowTitle = tr2("»ú³¡:") + QString::number(index);
			ui->tabWidget->addTab(dlgItem, windowTitle);
			m_ViewCameraVec.push_back(dlgItem);
			++index;
		}
	}
}

bool UAVAgriVideoDialogList::getCurrentTab(int& index)
{
	int currentTabIdx = ui->tabWidget->currentIndex();
	if (m_ViewCameraVec.size() > currentTabIdx + 1)
	{
		//strComponent = m_ViewCameraVec[currentTabIdx]->getCurrentComponentName().toStdString();
		index = currentTabIdx;
	}
	else {
		return false;
	}
	return true;
}

void UAVAgriVideoDialogList::tabWidgetChanged()
{
	int nIndex = ui->tabWidget->currentIndex();
	if (nIndex >= m_ViewCameraVec.size()) {
		return;
	}
	int number = m_ViewCameraVec[nIndex]->getVideoID();
	if (LY_AgriVideoPlayer::Get().GetISelectYoloModelDialog()) {
		LY_AgriVideoPlayer::Get().GetISelectYoloModelDialog()->setcurrSelectCamera(QString::number(number));
	}
}
	

void UAVAgriVideoDialogList::changeEvent(QEvent * event)
{
	QWidget::changeEvent(event);
}

void UAVAgriVideoDialogList::showEvent(QShowEvent * event)
{
	m_show = true;
}

void UAVAgriVideoDialogList::hideEvent(QHideEvent * event)
{
	m_show = false;
}
