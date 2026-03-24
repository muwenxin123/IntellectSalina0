#include "VedioRealTimeConfigureDlg.h"
#include "ui_VedioRealTimeConfigureDlg.h"
#include <QPainter>
#include<QStyleOption>
#include<QResizeEvent>
#include<QMouseEvent>
#include <QMutexLocker>
#include "LyMessageBox.h"

#include  "Ly_VideoRealTimePreview.h"
#include "IVideoRealTimePreviewActivator.h"
#include <QCString.h>

VedioRealTimeConfigureDlg::VedioRealTimeConfigureDlg(QWidget *parent) :
	LyDialog(tr2("实时视频配置"), parent)
   , ui(new Ui::VedioRealTimeConfigureDlg)
{
    ui->setupUi(centralWidget());
	this->setResizable(true);
	this->resize(500, 800);

	ensureUavCbb();

	ui->OccupyCbb->addItem("0.25");
	ui->OccupyCbb->addItem("0.5");
	ui->OccupyCbb->addItem("0.80");
	ui->OccupyCbb->addItem("1");

	connect(ui->topRadioButton, &QRadioButton::clicked, this, &VedioRealTimeConfigureDlg::onViewLocationRadioBtnClicked);
	connect(ui->bottomRadioButton, &QRadioButton::clicked, this, &VedioRealTimeConfigureDlg::onViewLocationRadioBtnClicked);
	connect(ui->leftRadioButton, &QRadioButton::clicked, this, &VedioRealTimeConfigureDlg::onViewLocationRadioBtnClicked);
	connect(ui->rightRadioButton, &QRadioButton::clicked, this, &VedioRealTimeConfigureDlg::onViewLocationRadioBtnClicked);
	connect(ui->showCheckBox, &QCheckBox::toggled, this, &VedioRealTimeConfigureDlg::onShowFlagCheckBoxToggled);
	connect(ui->useCheckBox, &QCheckBox::toggled, this, &VedioRealTimeConfigureDlg::onUseSortCheckBoxToggled);
	connect(ui->yawDoubleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(onDspValueChanged(double)));
	connect(ui->lonDoubleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(onDspValueChanged(double)));
	connect(ui->latDoubleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(onDspValueChanged(double)));
	connect(ui->sureBtn, &QPushButton::clicked, this, &VedioRealTimeConfigureDlg::onSureBtnClicked);
	connect(ui->quitBtn, &QPushButton::clicked, this, &VedioRealTimeConfigureDlg::onQuitBtnClicked);
	connect(ui->useFriendTrackCheckBox, &QCheckBox::toggled, this, &VedioRealTimeConfigureDlg::onUseFriendTrackCheckBoxToggled);
	connect(ui->ObeserveComboBox, &QComboBox::currentTextChanged, this, &VedioRealTimeConfigureDlg::onObeserveComboBoxCurrentTextChanged);
	connect(ui->OccupyCbb, &QComboBox::currentTextChanged, this, &VedioRealTimeConfigureDlg::onOccupyCbbChanged);
}

VedioRealTimeConfigureDlg::~VedioRealTimeConfigureDlg()
{
    delete ui;
}


void VedioRealTimeConfigureDlg::Init()
{
	ensureUavCbb();
	{
		QMutexLocker locker(&(Ly_VideoRealTimePreview::Get().getConfigMutex()));
		auto pConfigure = Ly_VideoRealTimePreview::Get().getConfig();
		switch (pConfigure->viewLocation)
		{
		case VideoRealTime::ViewTop:
		{
			ui->topRadioButton->setChecked(true);
			ui->bottomRadioButton->setChecked(false);
			ui->leftRadioButton->setChecked(false);
			ui->rightRadioButton->setChecked(false);
		}
		break;
		case VideoRealTime::ViewBottom:
		{
			ui->topRadioButton->setChecked(false);
			ui->bottomRadioButton->setChecked(true);
			ui->leftRadioButton->setChecked(false);
			ui->rightRadioButton->setChecked(false);
		}
		break;
		case VideoRealTime::ViewRight:
		{
			ui->topRadioButton->setChecked(false);
			ui->bottomRadioButton->setChecked(false);
			ui->leftRadioButton->setChecked(false);
			ui->rightRadioButton->setChecked(true);
		}
		break;
		case VideoRealTime::ViewLeft:
		{
			ui->topRadioButton->setChecked(false);
			ui->bottomRadioButton->setChecked(false);
			ui->leftRadioButton->setChecked(true);
			ui->rightRadioButton->setChecked(false);
		}
		break;
		default:
			break;
		}

		ui->showCheckBox->setChecked(pConfigure->showFlag);

		ui->useCheckBox->setChecked(pConfigure->useSortByPosAndYawFlag);
		ui->yawDoubleSpinBox->setEnabled(pConfigure->useSortByPosAndYawFlag);
		ui->lonDoubleSpinBox->setEnabled(pConfigure->useSortByPosAndYawFlag);
		ui->latDoubleSpinBox->setEnabled(pConfigure->useSortByPosAndYawFlag);

		ui->yawDoubleSpinBox->setValue(pConfigure->dYaw);
		ui->lonDoubleSpinBox->setValue(pConfigure->dLon);
		ui->latDoubleSpinBox->setValue(pConfigure->dLat);

		ui->useFriendTrackCheckBox->setChecked(pConfigure->useFriendTrackFlag);
		ui->ObeserveComboBox->setEnabled(pConfigure->useFriendTrackFlag);
		ui->ObeserveComboBox->setCurrentIndex(0);
		auto iter_observer = m_identity2NameMap.find(pConfigure->lObserverIdentity);
		if (iter_observer != m_identity2NameMap.end())
		{
			ui->ObeserveComboBox->setCurrentText(iter_observer.value());
		}
		ui->OccupyCbb->setCurrentText(QString::number(pConfigure->detailOccupyValue));
		m_isChanged = false;
	}
}

void VedioRealTimeConfigureDlg::paintEvent(QPaintEvent *event)
{
	QWidget::paintEvent(event);
}

void VedioRealTimeConfigureDlg::onSureBtnClicked()
{
	bool isSave = false;
	if (m_isChanged)
	{
		QMutexLocker locker(&(Ly_VideoRealTimePreview::Get().getConfigMutex()));
		auto pConfigure = Ly_VideoRealTimePreview::Get().getConfig();
		if (ui->topRadioButton->isChecked())
		{
			pConfigure->viewLocation = VideoRealTime::ViewTop;
		}
		else if (ui->bottomRadioButton->isChecked())
		{
			pConfigure->viewLocation = VideoRealTime::ViewBottom;
		}
		else if (ui->leftRadioButton->isChecked())
		{
			pConfigure->viewLocation = VideoRealTime::ViewLeft;
		}
		else if (ui->rightRadioButton->isChecked())
		{
			pConfigure->viewLocation = VideoRealTime::ViewRight;
		}

		pConfigure->showFlag = ui->showCheckBox->isChecked();
		pConfigure->useSortByPosAndYawFlag = ui->useCheckBox->isChecked();
		pConfigure->dYaw = ui->yawDoubleSpinBox->value();
		pConfigure->dLon = ui->lonDoubleSpinBox->value();
		pConfigure->dLat = ui->latDoubleSpinBox->value();
		pConfigure->useFriendTrackFlag = ui->useFriendTrackCheckBox->isChecked();
		pConfigure->lObserverIdentity = ui->ObeserveComboBox->currentData().toLongLong();
		pConfigure->detailOccupyValue = ui->OccupyCbb->currentText().toDouble();
		pConfigure->Save();
		m_isChanged = false;
		isSave = true;
	}

	LyMessageBox::information(this, tr2("提示"), tr2("保存成功"));

	if (isSave) {
		Ly_VideoRealTimePreview::Get().NotifyConfigureChanged();
	}
	close();
}

void VedioRealTimeConfigureDlg::onQuitBtnClicked()
{
	if (m_isChanged)
	{
		auto retValue = LyMessageBox::question(this, tr2("提示"), tr2("检测到数据发生改变，是否保存更改"),
			QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
		switch (retValue)
		{
		case QMessageBox::Yes:
			onSureBtnClicked();
			break;
		case QMessageBox::No:
			Init();
			break;
		case QMessageBox::Cancel:
			return;
			break;
		default:
			break;
		}
	}
	close();
}

void VedioRealTimeConfigureDlg::onViewLocationRadioBtnClicked(bool checked)
{
	m_isChanged = true;
}

void VedioRealTimeConfigureDlg::onShowFlagCheckBoxToggled(bool checked)
{
	m_isChanged = true;
}

void VedioRealTimeConfigureDlg::onUseSortCheckBoxToggled(bool checked)
{
	ui->yawDoubleSpinBox->setEnabled(checked);
	ui->lonDoubleSpinBox->setEnabled(checked);
	ui->latDoubleSpinBox->setEnabled(checked);
	m_isChanged = true;
}

void VedioRealTimeConfigureDlg::onDspValueChanged(double value)
{
	m_isChanged = true;
}

void VedioRealTimeConfigureDlg::onUseFriendTrackCheckBoxToggled(bool checked)
{
	ui->ObeserveComboBox->setEnabled(checked);
	m_isChanged = true;
}

void VedioRealTimeConfigureDlg::onObeserveComboBoxCurrentTextChanged(const QString &text)
{
	m_isChanged = true;
}

void VedioRealTimeConfigureDlg::onOccupyCbbChanged(const QString &text)
{
	m_isChanged = true;
}

void VedioRealTimeConfigureDlg::closeEvent(QCloseEvent * event)
{
	if (m_isChanged)
	{
		auto retValue = LyMessageBox::question(this, tr2("提示"), tr2("检测到数据发生改变，是否保存更改"),
			QMessageBox::Yes | QMessageBox::No);
		switch (retValue)
		{
		case QMessageBox::Yes:
			onSureBtnClicked();
			break;
		case QMessageBox::No:
			Init();
			break;
		default:
			break;
		}
	}

	return QWidget::closeEvent(event);
}

void VedioRealTimeConfigureDlg::showEvent(QShowEvent * event)
{
	ensureUavCbb();
	return QWidget::showEvent(event);
}

void VedioRealTimeConfigureDlg::getUavList(QMap<long long, QString>& idMap)
{

	idMap.clear();
	auto pISystemMaintenanceService = IVideoRealTimePreviewActivator::Get().getISystemMaintenanceService();
	auto pService = IVideoRealTimePreviewActivator::Get().getPWeaponManageService();
	if (pService == nullptr || nullptr == pISystemMaintenanceService)
		return;

	QSet<long long> idSet;

	qnzkna::WeaponManage::WeaponSystemMap weaponSystemMap;
	if (pISystemMaintenanceService->GetWeaponSystems(&weaponSystemMap))
	{
		for (const auto &weaponSystemIter : weaponSystemMap)
		{
			const auto &mapWeaponComponent = weaponSystemIter.second.getMapWeaponComponent();
			for (const auto &mapWeaponComponentIter : mapWeaponComponent)
			{
				const qnzkna::WeaponManage::WeaponComponent &weaponComponent = (mapWeaponComponentIter.second);
				bool ok = false;
				const unsigned long long componentID = QString::fromStdString(weaponComponent.getComponentID()).toULongLong(&ok);
				if (ok)
				{
					if (weaponSystemIter.second.getSystemType() == qnzkna::WeaponManage::WeaponSystem_Type::WeaponSystem_Type_UAV)
					{
						idSet.insert(componentID);
					}
				}
			}
		}
	}

	std::string weaponName;
	for (auto& id : idSet)
	{
		if (pService->GetWeaponComponentName(weaponName, QString::number(id).toStdString()))
		{
			idMap.insert(id, QString::fromStdString(weaponName));
		}
		else 
		{
			idMap.insert(id, QString::number(id));
		}
	}
}

void VedioRealTimeConfigureDlg::ensureUavCbb()
{
	if (m_identity2NameMap.isEmpty())
	{
		getUavList(m_identity2NameMap);
		for (auto iter = m_identity2NameMap.begin(); iter != m_identity2NameMap.end(); iter++)
		{
			ui->ObeserveComboBox->addItem(iter.value(), iter.key());
		}
	}
}
