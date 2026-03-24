#include "UAVArmShotAloneListDialog.h"
#include "ui_UAVArmShotAloneListDialog.h"
#include "QCString.h"
UAVArmShotAloneListDialog::UAVArmShotAloneListDialog(QWidget *parent) 
	:QWidget(parent),
    ui(new Ui::UAVArmShotAloneListDialog)
{
    ui->setupUi(this);
}

UAVArmShotAloneListDialog::~UAVArmShotAloneListDialog()
{
    delete ui;
}

void UAVArmShotAloneListDialog::InitDialog(std::list<IUAVNavigationArmShotAlone54Dialog *> m_pList)
{
	ui->tabWidget->clear();
	if (m_pList.size() > 0)
	{
		for (auto &dlgItem : m_pList)
		{
			QString windowTitle = dlgItem->getCurrentComponentName() + ":";
			windowTitle += dlgItem->getTitleInfo();
			ui->tabWidget->addTab(dlgItem, windowTitle);
		}
	}
}

void UAVArmShotAloneListDialog::changeEvent(QEvent * event)
{
	QWidget::changeEvent(event);
}

void UAVArmShotAloneListDialog::showEvent(QShowEvent * event)
{
	m_show = true;
}

void UAVArmShotAloneListDialog::hideEvent(QHideEvent * event)
{
	m_show = false;
}
