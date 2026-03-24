#include "ZoneManageWidget.h"
#include "ui_ZoneManageWidget.h"
#include "LY_ZoneManager.h"
#include "PtInPloy.h"
#include "AcmGeometry.h"
#include "IZoneManagerActivator.h"
#include "LyMessageBox.h"
#include "LY_ZoneManagerInterface.h"
#include "map3d/IEarth.h"
#include "QGeoCoordinate"
#include <QCheckBox>
#include <QPainter>

ZoneManageWidget::ZoneManageWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ZoneManageWidget)
{
    ui->setupUi(this);

	if (m_pChildDlg == nullptr)
	{
		m_pChildDlg = new CDlgZoneParameterEdit(this);
		ui->horizontalLayout->addWidget(m_pChildDlg);
		qnzkna::ZoneManage::CZoneBase stZoneBase;
		m_pChildDlg->SetZonebase(stZoneBase,false);

	}

	m_pIZoneManageService = IZoneManagerActivator::Get().getPIZoneManageService();

	m_pINebulaTaskManageService = IZoneManagerActivator::Get().getPTaskManageService();

	InitZoneData();

	SetEditStates(false);

	CreateTree();

	int colorIndex = 0;
	m_nColor[colorIndex] = 0xffff00;
	m_nColor[++colorIndex] = 0xffff00;
	m_nColor[++colorIndex] = 0x00ff00;
	m_nColor[++colorIndex] = 0x008000;
	m_nColor[++colorIndex] = 0x0000ff;
	m_nColor[++colorIndex] = 0x800000;
	m_nColor[++colorIndex] = 0x000000;
	m_nColor[++colorIndex] = 0x404040;
	m_nColor[++colorIndex] = 0xc000c0;
	m_nColor[++colorIndex] = 0x00ffff;
	m_nColor[++colorIndex] = 0xaa5500;
	m_nColor[++colorIndex] = 0xffffff;
	m_nColor[++colorIndex] = 0xff0000;

	ui->pb_ghhl->hide();
	ui->pbtnAreaName->hide();
	ui->tableWidget_2->hide();
	m_strDataDir = tr2(LY_ZoneManager::Get().GetDataDir().data());

	GetLableColor();

}

ZoneManageWidget::~ZoneManageWidget()
{
    delete ui;
}

void ZoneManageWidget::DrawQYZoneData(QPainter *painter)
{
	if (m_pZoneAdditionDrawWidget && m_pZoneAdditionDrawWidget->isVisible())
		m_pZoneAdditionDrawWidget->DrawQYZoneData(painter);

}

void ZoneManageWidget::insertJWDpt(double ll, double bb, bool bEndflg )
{

	m_pZoneAdditionDrawWidget->insertJWDpt(ll, bb, bEndflg);
}

void ZoneManageWidget::EditMouseMoveJWD(double ll, double bb)
{
	if (m_pZoneAdditionDrawWidget)
		m_pZoneAdditionDrawWidget->EditMouseMoveJWD(ll, bb);
}

void ZoneManageWidget::WriteKyList(QUYU_Zone &zone, bool InitFlg )
{

}

void ZoneManageWidget::On2d3dChanged(int wParam)
{

}

void ZoneManageWidget::DingWeiqy(const QUYU_Zone &zone)
{

}

void ZoneManageWidget::isZoneSelected(bool isSelected)
{
	if (m_pZoneAdditionDrawWidget)
		m_pZoneAdditionDrawWidget->isZoneSelected(isSelected);
}

void ZoneManageWidget::InitZoneData()
{
	if (m_pINebulaTaskManageService == nullptr || m_pIZoneManageService == nullptr)
	{
		return;
	}
	m_pIZoneManageService->SetActivatedAciton(m_pINebulaTaskManageService->GetActivatingActionID());
}

void ZoneManageWidget::UpdateWindowData()
{
	m_ZoneBaseVec.clear();
	if (m_pIZoneManageService == nullptr)
	{
		return;
	}
	m_ZoneBaseVec = m_pIZoneManageService->GetAllZoneBase();
}

void ZoneManageWidget::SetEditStates(bool bEdited)
{
	if (bEdited == true)
	{
		ui->pushButton->setVisible(false);
		ui->pushButton_2->setVisible(false);
		ui->pb_add->setVisible(false);
		ui->pb_edit->setVisible(false);
		ui->pb_delete->setVisible(false);

		ui->pushButton_3->setVisible(false);

		ui->pb_save->setVisible(true);
		ui->pushButton_4->setVisible(true);
	}
	else
	{
		ui->pushButton->setVisible(true);
		ui->pushButton_2->setVisible(true);
		ui->pb_add->setVisible(true);
		ui->pb_edit->setVisible(true);
		ui->pb_delete->setVisible(true);

		ui->pushButton_3->setVisible(true);

		ui->pb_save->setVisible(false);
		ui->pushButton_4->setVisible(false);
	}
}

void ZoneManageWidget::CreateTree()
{
	ui->treeWidget->clear();

	m_TreeHeader.clear();

	m_TreeHeader << tr2("区域类型") << tr2("区域名称") << tr2("区域形状");
	ui->treeWidget->setHeaderLabels(m_TreeHeader);
	ui->treeWidget->setColumnCount(m_TreeHeader.size());

	for (int i = 0; i < m_TreeHeader.size(); i++)
	{
		ui->treeWidget->header()->setSectionResizeMode(i, QHeaderView::Fixed);
	}

	ui->treeWidget->header()->setDefaultAlignment(Qt::AlignCenter);
}

void ZoneManageWidget::UpdateTreeData()
{
	ui->treeWidget->clear();

	if (m_ZoneBaseVec.size() == 0)
	{
		return;
	}

	QTreeWidgetItem* pRootItem = new QTreeWidgetItem(ui->treeWidget);
	pRootItem->setText(0, tr2("高盐度盐田(>3.5%)"));

	pRootItem->setTextAlignment(0, Qt::AlignCenter);
	pRootItem->setCheckState(0,Qt::Checked);
	AddZoneTreeItem(7, pRootItem);

	QTreeWidgetItem* pRootItem1 = new QTreeWidgetItem(ui->treeWidget);
	pRootItem1->setText(0, tr2("中盐度盐田(2.5%-3.5%)"));

	pRootItem1->setTextAlignment(0, Qt::AlignCenter);
	pRootItem1->setCheckState(0, Qt::Checked);
	AddZoneTreeItem(8, pRootItem1);

	QTreeWidgetItem* pRootItem2 = new QTreeWidgetItem(ui->treeWidget);
	pRootItem2->setText(0, tr2("低盐度盐田(<2.5%)"));

	pRootItem2->setTextAlignment(0, Qt::AlignCenter);
	pRootItem2->setCheckState(0, Qt::Checked);
	AddZoneTreeItem(9, pRootItem2);

	QTreeWidgetItem* pRootItem6 = new QTreeWidgetItem(ui->treeWidget);
	pRootItem6->setText(0, tr2("其他"));

	pRootItem6->setTextAlignment(0, Qt::AlignCenter);
	pRootItem6->setCheckState(0, Qt::Checked);
	AddZoneTreeItem(0, pRootItem6);

	QTreeWidgetItem* pRootItem3 = new QTreeWidgetItem(ui->treeWidget);
	pRootItem3->setText(0, tr2("禁飞区"));

	pRootItem3->setTextAlignment(0, Qt::AlignCenter);
	pRootItem3->setCheckState(0, Qt::Checked);
	AddZoneTreeItem(1, pRootItem3);

	QTreeWidgetItem* pRootItem4 = new QTreeWidgetItem(ui->treeWidget);
	pRootItem4->setText(0, tr2("限飞区"));

	pRootItem4->setTextAlignment(0, Qt::AlignCenter);
	pRootItem4->setCheckState(0, Qt::Checked);
	AddZoneTreeItem(2, pRootItem4);

	QTreeWidgetItem* pRootItem5 = new QTreeWidgetItem(ui->treeWidget);
	pRootItem5->setText(0, tr2("危险区"));

	pRootItem5->setTextAlignment(0, Qt::AlignCenter);
	pRootItem5->setCheckState(0, Qt::Checked);
	AddZoneTreeItem(3, pRootItem5);

	QTreeWidgetItem* pRootItem7 = new QTreeWidgetItem(ui->treeWidget);
	pRootItem7->setText(0, tr2("起降区"));

	pRootItem7->setTextAlignment(0, Qt::AlignCenter);
	pRootItem7->setCheckState(0, Qt::Checked);
	AddZoneTreeItem(4, pRootItem7);

	QTreeWidgetItem* pRootItem8 = new QTreeWidgetItem(ui->treeWidget);
	pRootItem8->setText(0, tr2("作业区"));

	pRootItem8->setTextAlignment(0, Qt::AlignCenter);
	pRootItem8->setCheckState(0, Qt::Checked);
	AddZoneTreeItem(5, pRootItem8);

	QTreeWidgetItem* pRootItem9 = new QTreeWidgetItem(ui->treeWidget);
	pRootItem9->setText(0, tr2("航路"));

	pRootItem9->setTextAlignment(0, Qt::AlignCenter);
	pRootItem9->setCheckState(0, Qt::Checked);
	AddZoneTreeItem(6, pRootItem9);

	ui->treeWidget->expandAll();

	TransmitShowData();
}

void ZoneManageWidget::AddZoneTreeItem(const int& nType, QTreeWidgetItem* pRootItem)
{
	if (m_ZoneBaseVec.size() == 0)
	{
		return;
	}

	for (auto itrZone : m_ZoneBaseVec)
	{
		if (itrZone.nProperty == nType)
		{

			QTreeWidgetItem* CarChildItem = new QTreeWidgetItem(pRootItem);

			if (itrZone.bShow == true)
			{
				CarChildItem->setCheckState(0, Qt::Checked);
			}
			else
			{
				CarChildItem->setCheckState(0, Qt::Unchecked);
			}

			CarChildItem->setText(1, QString::fromLocal8Bit(itrZone.strID));
			CarChildItem->setTextAlignment(1, Qt::AlignCenter);
			CarChildItem->setData(1, Qt::UserRole, QString::fromLocal8Bit(itrZone.strID));

			CarChildItem->setText(2, QString::fromLocal8Bit(qnzkna::ZoneManage::ZoneTypeToString(itrZone.nZoneType)));
			CarChildItem->setTextAlignment(2, Qt::AlignCenter);

			UpdateParentItem(CarChildItem);
		}
	}
}

void ZoneManageWidget::UpdateParentItem(QTreeWidgetItem* item)
{
	QTreeWidgetItem* parent = item->parent();
	if (parent == nullptr)
	{
		return;
	}

	int selectCount = 0;
	int nPartiallCount = 0;
	int childCount = parent->childCount();
	for (int i = 0; i < childCount; i++)
	{
		QTreeWidgetItem* childItem = parent->child(i);
		if (childItem->checkState(0) == Qt::Checked)
		{
			selectCount++;
		}
		else if (childItem->checkState(0) == Qt::PartiallyChecked)
		{
			nPartiallCount++;
		}
	}

	if (selectCount == childCount)
	{
		parent->setCheckState(0, Qt::Checked);
	}
	else  if (selectCount > 0 && selectCount < childCount || selectCount == 0 && nPartiallCount > 0)
	{
		parent->setCheckState(0, Qt::PartiallyChecked);
	}
	else if (selectCount == 0 && nPartiallCount == 0)
	{
		parent->setCheckState(0, Qt::Unchecked);
	}
}

void ZoneManageWidget::UpdateShowStates()
{
	if (m_pIZoneManageService == nullptr)
	{
		return;
	}

	QTreeWidgetItemIterator it(ui->treeWidget);
	while (*it)
	{
		QString strID = (*it)->data(1, Qt::UserRole).toString();
		if (strID != "")
		{
			bool bChecked = false;
			if ((*it)->checkState(0) == Qt::Checked)
			{
				bChecked = true;
			}

			for ( auto& itrZone :m_ZoneBaseVec)
			{
				if (0 == strID.compare(QString::fromLocal8Bit(itrZone.strID)))
				{
					if ( itrZone.bShow != bChecked)
					{
						m_pIZoneManageService->SetZoneBaseShowStates(strID, bChecked);
						itrZone.bShow = bChecked;

						break;
					}
				}
			}
		}
		++it;
	}

	TransmitShowData();

}

void ZoneManageWidget::TableWidgetAddCheckBox(QTableWidget *tableWidget, int x, int y, QString text, Qt::CheckState CheckState)
{
	QWidget *widget = new QWidget();
	QHBoxLayout *layout = new QHBoxLayout();
	QCheckBox *checkbox = new QCheckBox();

	checkbox->setText("");
	checkbox->setCheckState(CheckState);
	layout->addWidget(checkbox, 0, Qt::AlignCenter);
	layout->setMargin(0);
	widget->setLayout(layout);

	tableWidget->setCellWidget(x, y, checkbox);
}

void ZoneManageWidget::SetScreenCenter(double &dLon, double &dLat)
{
	QString str = "CenterAt(" + QString::number(dLon, 'f', 8) + "," + QString::number(dLat, 'f', 8) + ")";
	LY_ZoneManagerInterface::Get().Script(str.toLocal8Bit());
}

void ZoneManageWidget::TransmitShowData()
{

	m_CurAreaShowItemMap.clear();
	for (auto itr:m_ZoneBaseVec)
	{
		if ( itr.bShow == true)
		{
			std::string str = tr3(QString::fromLocal8Bit(itr.strID));
			m_CurAreaShowItemMap[str] = Qt::Checked;
		}
	}

	Draw3DTakeOffAndLandImg();

}

void ZoneManageWidget::DrawTextFunctionTwo(QPainter& painter, IProj* pProj)
{
	QColor DifColor(255, 255, 255);
	for (QMap<std::string,Qt::CheckState>::const_iterator it = m_CurAreaShowItemMap.cbegin();it != m_CurAreaShowItemMap.cend(); ++it)
	{
		for (auto name : m_ZoneBaseVec)
		{
			if(name.strID != it.key())
				continue;
			double dlon, dlat;
			unsigned int uiV = name.nLineColor;
			QColor color = QColor(uiV & 0xFF, (uiV >> 8) & 0xFF, (uiV >> 16) & 0xFF);
			color.setAlpha(100);
			if (name.nZoneType == 3)
			{
				CalculateLocation(name.stZonePointVec, dlon, dlat);

				QPoint point = pProj->Jwd2Screen(dlon, dlat);
				painter.setPen(DifColor);
				QPoint ptTmp(point.x() - 5, point.y() - 5);
				painter.drawText(ptTmp, tr2(it.key().c_str()));
			}
			else if (name.nZoneType == 8 && name.stZonePointVec.size() >= 0)
			{
				QPoint point = pProj->Jwd2Screen(name.stZonePointVec[0].dLon, name.stZonePointVec[0].dLat);
				painter.setPen(color);
				QPoint ptTmp(point.x() - 10, point.y() - 10);
				painter.drawText(ptTmp, tr2(it.key().c_str()));
			}
			else
			{
				QPoint point = pProj->Jwd2Screen(name.stRoundParameter.dLon, name.stRoundParameter.dLat);
				painter.setPen(DifColor);
				QPoint ptTmp(point.x() - 5, point.y() - 5);
				painter.drawText(ptTmp, tr2(it.key().c_str()));
			}

		}
	}

}
void ZoneManageWidget::DrawTextFunctionThree()
{
	vector<double> vLon;
	vector<double> vLat;
	vector<double> vAlt;
	IEarth *pEarth = IEarth::GetEarth();
	if (pEarth == nullptr)
		return;
	auto itr2 = m_3DLocPositionMesh2Vec.begin();
	for (; itr2 != m_3DLocPositionMesh2Vec.end(); itr2++)
	{
		IEarth *pEarth = IEarth::GetEarth();
		pEarth->GetGraphics()->RemoveObject(*itr2);
		*itr2 = nullptr;
		delete *itr2;
	}
	m_3DLocPositionMesh2Vec.clear();

	QColor colorBombTrack2((m_nColor[5]));
	colorBombTrack2.setAlpha(125);
	QColor colorTemp2;
	colorTemp2.setRgb(colorBombTrack2.blue(), colorBombTrack2.green(), colorBombTrack2.red(), colorBombTrack2.alpha());

	struct ZoneStart
	{
		std::string name;
		QGeoCoordinate point;
		int color;
	}ZonePoint;

	std::vector<ZoneStart> ZoneData;
	for (QMap<std::string, Qt::CheckState>::const_iterator it = m_CurAreaShowItemMap.cbegin(); it != m_CurAreaShowItemMap.cend(); ++it)
	{
		for (auto name : m_ZoneBaseVec)
		{
			if (name.strID != it.key())
				continue;
			double dlon, dlat;

			ZoneStart data;
			data.color = name.nLineColor;
			data.name = name.strID;
			if (name.nZoneType == 3)
			{
				CalculateLocation(name.stZonePointVec, dlon, dlat);
				data.point = QGeoCoordinate(dlat, dlon, name.nAboveAlt);
				ZoneData.push_back(data);
				break;
			}
			else if (name.nZoneType == 8 && name.stZonePointVec.size() >=0)
			{
				data.point = QGeoCoordinate(name.stZonePointVec[0].dLat, name.stZonePointVec[0].dLon, name.nAboveAlt);
				ZoneData.push_back(data);
				break;
			}
			data.point = QGeoCoordinate(name.stRoundParameter.dLat, name.stRoundParameter.dLon, name.nAboveAlt);
			ZoneData.push_back(data);
			break;
		}
	}

	for (int i = 0; i < ZoneData.size(); i++)
	{
		QGeoCoordinate mpos = ZoneData[i].point;
		createUserMesh2Text(pEarth->GetGraphics(), m_3DLocPositionMesh2Vec, ZoneData[i].color,
			ZoneData[i].name.c_str(), mpos.longitude(), mpos.latitude(), mpos.altitude());
	}
}

void ZoneManageWidget::ClearTextFunctionThree()
{
	IEarth *pEarth = IEarth::GetEarth();
	if (pEarth == nullptr)
		return;
	auto itr2 = m_3DLocPositionMesh2Vec.begin();
	for (; itr2 != m_3DLocPositionMesh2Vec.end(); itr2++)
	{
		IEarth *pEarth = IEarth::GetEarth();
		pEarth->GetGraphics()->RemoveObject(*itr2);
		*itr2 = nullptr;
		delete *itr2;
	}
	m_3DLocPositionMesh2Vec.clear();
}

void ZoneManageWidget::CalculateLocation(qnzkna::ZoneManage::ZonePointVec ZoneVec, double & dlon, double & dlat)
{
	double signedArea = 0;
	dlon = 0, dlat = 0;
	for (size_t i = 0; i < ZoneVec.size(); i++)
	{
		const QPointF& current = QPointF(ZoneVec[i].dLon, ZoneVec[i].dLat);
		const QPointF& next = QPointF(ZoneVec[(i + 1) % ZoneVec.size()].dLon, ZoneVec[(i + 1) % ZoneVec.size()].dLat);
		double step = current.x() * next.y() - next.x() * current.y();
		signedArea += step;
		dlon += (current.x() + next.x()) * step;
		dlat += (current.y() + next.y()) * step;
	}
	signedArea *= 0.5;
	dlon /= (6.0 * signedArea);
	dlat /= (6.0 * signedArea);
}

void ZoneManageWidget::createUserMesh2Text(IGraphics * pGraphics, std::vector<IText*>& wndVec, ARGB ar, const char * lpszText, double dLng, double dLat, double dAlt)
{
	IEarth *pEarth = IEarth::GetEarth();
	unsigned int uiV = ar;
	QColor color = QColor(uiV & 0xFF, (uiV >> 8) & 0xFF, (uiV >> 16) & 0xFF);
	color.setAlpha(100);

	std::vector<std::vector<int>> setTRGB;
	auto itr = m_SetColorMap.begin();
	for (;itr!= m_SetColorMap.end();)
	{
		if (itr->second)
		{
			setTRGB = itr->first;
		}
		itr++;
	}
	IText* mshpText = pEarth->GetGraphics()->CreateText(lpszText, dLng, dLat, dAlt, NULL, setTRGB[3][0],
		TRGB(setTRGB[1][0], setTRGB[1][1], setTRGB[1][2], setTRGB[1][3]), TRGB(setTRGB[0][0], setTRGB[0][1], setTRGB[0][2], setTRGB[0][3]), TRGB(setTRGB[2][0], setTRGB[2][1], setTRGB[2][2], setTRGB[2][3]),7);

	if (mshpText != nullptr)
	{
		m_3DLocPositionMesh2Vec.push_back(mshpText);
	}

}

void ZoneManageWidget::Draw3DTakeOffAndLandImg()
{
	IEarth *pEarth = IEarth::GetEarth();
	if (pEarth == nullptr)
		return;
	auto itr2 = m_3DTakeOffMeshVec.begin();
	for (; itr2 != m_3DTakeOffMeshVec.end(); itr2++)
	{
		IEarth *pEarth = IEarth::GetEarth();
		pEarth->GetGraphics()->RemoveObject(*itr2);
		*itr2 = nullptr;
		delete *itr2;
	}
	m_3DTakeOffMeshVec.clear();

	struct ZoneStart
	{
		std::string name;
		QGeoCoordinate point;
		int color;
	}ZonePoint;

	std::vector<ZoneStart> ZoneData;
	for (QMap<std::string, Qt::CheckState>::const_iterator it = m_CurAreaShowItemMap.cbegin(); it != m_CurAreaShowItemMap.cend(); ++it)
	{
		for (auto name : m_ZoneBaseVec)
		{
			if (name.strID != it.key())
				continue;
			double dlon, dlat;

			ZoneStart data;
			data.color = name.nLineColor;
			data.name = name.strID;
			if (name.nProperty == qnzkna::ZoneManage::CZoneBase::ZoneProperty::QJ)
			{
				if (name.nZoneType == qnzkna::ZoneManage::CZoneBase::ZoneBaseType::RECTANGLE)
				{
					qnzkna::ZoneManage::ZonePointVec pointlist;
					IZoneManagerActivator::Get().getPIZoneManageService()->TransformRectangleToPoints(tr2(name.strID), pointlist);
					name.stZonePointVec = pointlist;
					CalculateLocation(name.stZonePointVec, dlon, dlat);
					data.point = QGeoCoordinate(dlat, dlon, name.nAboveAlt);
					ZoneData.push_back(data);
					break;
				}
				else
				{
					dlon = name.stRoundParameter.dLon;
					dlat = name.stRoundParameter.dLat;
					data.point = QGeoCoordinate(dlat, dlon, name.nAboveAlt);
					ZoneData.push_back(data);
					break;
				}
			}

		}
	}

	QColor col(255, 0, 0, 255);
	QUuid uid = QUuid::fromRfc4122(QByteArray::fromRawData(QUuid::createUuid().toRfc4122(), 16));
	QString strLable = "";
	if (m_mapLabel.find(uid) == m_mapLabel.end())
	{
		std::string strImg;
		auto it = m_mapLabelPath.find(uid);
		if (it == m_mapLabelPath.end())
		{
			QString strPath = QObject::tr(m_strDataDir);
			strPath += QLatin1String("Temp_Line/toala1.png");
			QImage img = QImage(strPath);
			strPath = QObject::tr(m_strDataDir);
			strPath += QLatin1String("/temp/");
			strPath += uid.toString(QUuid::WithoutBraces);
			strPath += QLatin1String(".png");
			QFile::remove(strPath);
			img.save(strPath);
			strImg = tr3(strPath).data();
			m_mapLabelPath[uid] = strImg;
		}
		else
		{
			strImg = it->second;
		}
		for (int i = 0; i < ZoneData.size(); i++)
		{
			QGeoCoordinate mpos = ZoneData[i].point;
			double alt = mpos.altitude();
			short int elevationTemp = LY_ZoneManager::Get().s_CGetHeight.GetHeight
			(mpos.longitude(), mpos.latitude());
			if (elevationTemp < -10000)
			{
				alt = mpos.altitude();
			}
			else
			{
				alt = elevationTemp;
			}
			ILabel *pLabel = pEarth->GetGraphics()->CreateLabel(strLable.toLocal8Bit(), strImg.c_str(),
				mpos.longitude(), mpos.latitude(), alt, 64, 64, 0);
			if (pLabel != nullptr) {
				m_3DTakeOffMeshVec.push_back(pLabel);
				m_mapLabel[uid] = pLabel;
			}
		}

	}

}

void ZoneManageWidget::Clear3DTakeOffAndLandImg()
{
	IEarth *pEarth = IEarth::GetEarth();
	if (pEarth == nullptr)
		return;
	auto itr2 = m_3DTakeOffMeshVec.begin();
	for (; itr2 != m_3DTakeOffMeshVec.end(); itr2++)
	{
		IEarth *pEarth = IEarth::GetEarth();
		pEarth->GetGraphics()->RemoveObject(*itr2);
		*itr2 = nullptr;
		delete *itr2;
	}
	m_3DTakeOffMeshVec.clear();
}

void ZoneManageWidget::UpdateLableColorShow()
{
	if (ui->checkBox->checkState()==Qt::Unchecked)
	{
		ui->checkBox->setCheckState(Qt::Checked);
	}
	else
	{
		on_checkBox_stateChanged(1);
	}
}

void ZoneManageWidget::GetLableColor()
{
	std::vector<int> DBLableColor = m_pIZoneManageService->GetLableColor();
	std::vector<int> trgbbrackground{ 128, 63, 72, 204 };
	std::vector<int> trgbscript{ 200, 255, 255, 255 };
	std::vector<int> trgbframe{ 128, 63, 72, 204 };
	std::vector<int> scriptSize{ 16 };
	m_vDefaultTRGB.push_back(trgbbrackground);
	m_vDefaultTRGB.push_back(trgbscript);
	m_vDefaultTRGB.push_back(trgbframe);
	m_vDefaultTRGB.push_back(scriptSize);
	if (!DBLableColor.empty())
	{

		m_SetColorMap[m_vDefaultTRGB] = false;

		unsigned int uiV = DBLableColor[0];
		QColor brackgroundColor = QColor(uiV & 0xFF, (uiV >> 8) & 0xFF, (uiV >> 16) & 0xFF);
		unsigned int uiV1 = DBLableColor[1];
		QColor scriptColor = QColor(uiV1 & 0xFF, (uiV1 >> 8) & 0xFF, (uiV1 >> 16) & 0xFF);
		unsigned int uiV2 = DBLableColor[2];
		QColor frameColor = QColor(uiV2 & 0xFF, (uiV2 >> 8) & 0xFF, (uiV2 >> 16) & 0xFF);
		std::vector<std::vector<int>> InitTRGBDB;
		std::vector<int> trgbbrackgroundDB{ DBLableColor[3], brackgroundColor.red(), brackgroundColor.green(), brackgroundColor.blue() };
		std::vector<int> trgbscriptDB{ DBLableColor[4], scriptColor.red(), scriptColor.green(), scriptColor.blue() };
		std::vector<int> trgbframeDB{ DBLableColor[5], frameColor.red(), frameColor.green(), frameColor.blue() };
		std::vector<int> scriptSizeDB{ DBLableColor[6] };
		InitTRGBDB.push_back(trgbbrackgroundDB);
		InitTRGBDB.push_back(trgbscriptDB);
		InitTRGBDB.push_back(trgbframeDB);
		InitTRGBDB.push_back(scriptSizeDB);
		m_SetColorMap[InitTRGBDB] = true;
	}
	else
	{
		m_SetColorMap[m_vDefaultTRGB] = true;
	}

}

qnzkna::ZoneManage::CZoneBase & ZoneManageWidget::getCurZoneBase()
{
	return m_curZoneBase;
}

void ZoneManageWidget::setCurZoneBase(qnzkna::ZoneManage::CZoneBase & _curZoneBase)
{
	m_curZoneBase = _curZoneBase;
	if (m_pChildDlg != nullptr)
	{
		for ( auto& itr :m_ZoneBaseVec)
		{
			if ( 0 == strcmp(m_curZoneBase.strID,itr.strID))
			{
				itr = m_curZoneBase;
				m_pChildDlg->SetZonebase(m_curZoneBase, true);
				break;
			}
		}
	}
}

void ZoneManageWidget::showEvent(QShowEvent *event)
{
	UpdateWindowData();

	UpdateTreeData();
}



void ZoneManageWidget::closeEvent(QCloseEvent *event)
{
	m_bCreateFlag = false;
}

void ZoneManageWidget::paintEvent(QPaintEvent *event)
{
	//如果不写此代码,会导致画面在地图上透明
	QPainter painter(this);
	QStyleOption opt;
	opt.init(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
}

void ZoneManageWidget::on_pushButton_clicked()
{
	if (LY_ZoneManager::Get().GetPDlgZoneFileManage() == nullptr)
	{
		return;
	}

	if (LY_ZoneManager::Get().GetPDlgZoneFileManage()->isHidden() == true)
	{
		LY_ZoneManager::Get().GetPDlgZoneFileManage()->show();
	}
	else
	{
		LY_ZoneManager::Get().GetPDlgZoneFileManage()->hide();
	}

}

void ZoneManageWidget::on_pushButton_2_clicked()
{
	if (LY_ZoneManager::Get().GetPDlgZoneRuleManage() == nullptr)
	{
		return;
	}

	if (LY_ZoneManager::Get().GetPDlgZoneRuleManage()->isHidden() == true)
	{
		LY_ZoneManager::Get().GetPDlgZoneRuleManage()->show();
	}
	else
	{
		LY_ZoneManager::Get().GetPDlgZoneRuleManage()->hide();
	}
}

void ZoneManageWidget::on_pb_add_clicked()
{
	if (m_pZoneAdditionDrawWidget) {
		delete m_pZoneAdditionDrawWidget;
		m_pZoneAdditionDrawWidget = nullptr;
	}
	m_pZoneAdditionDrawWidget = new ZoneAdditionDrawWidget(this);

	connect(m_pZoneAdditionDrawWidget, &ZoneAdditionDrawWidget::sig_cur_zone, this, [&](qnzkna::ZoneManage::CZoneBase zone)
	{
		bool success = m_pIZoneManageService->AddZoneBase(zone);
		QStringList strIDlist;
		QString strID = QString::fromLocal8Bit(zone.strID);
		success = m_pIZoneManageService->JudgeZoneIntersect(strID, strIDlist);
		if (success == true)
		{
			bool bLine = false;
			if (zone.nZoneType == qnzkna::ZoneManage::CZoneBase::LINE)
			{
				if (strIDlist.size() == 1)
				{
					success = m_pIZoneManageService->ReplanAirway_2(strID,strIDlist[0]);
					if (success == false)
					{
						success = m_pIZoneManageService->RemoveZoneBase(strID);
					}
					bLine = true;
				}
			}

			if (bLine == false)
			{
				QString strTips = tr2("当前区域与\n");
				for (auto itr : strIDlist)
				{
					qnzkna::ZoneManage::CZoneBase stData;
					success = m_pIZoneManageService->GetZoneBase(itr, stData);
					strTips += tr2("区域(ID:") + itr + tr2(", 属性:") + QString::fromLocal8Bit(qnzkna::ZoneManage::ZonePropertyToString(stData.nProperty)) + tr2(")\n");
				}
				strTips += tr2("有交集，是否创建？");

				if (LyMessageBox::question(nullptr, tr2("提示"), strTips, QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
				{
					success = m_pIZoneManageService->RemoveZoneBase(strID);
					return;
				}
			}
		}
		UpdateWindowData();
		UpdateTreeData();

	});

	m_pZoneAdditionDrawWidget->raise();
	m_pZoneAdditionDrawWidget->show();

	m_bCreateFlag = true;
	m_iCurSelDotIdx = -1;
}

void ZoneManageWidget::on_pb_edit_clicked()
{
	QTreeWidgetItem* pSelectedItem = ui->treeWidget->currentItem();
	if (pSelectedItem == nullptr)
	{
		return;
	}
	QString strID = pSelectedItem->data(1, Qt::UserRole).toString();

	if (strID == "")
	{
		LyMessageBox::warning(nullptr, tr2("提示"), tr2("请选中一个区域"), QMessageBox::Ok);
		return;
	}

	if (m_pChildDlg)
	{
		m_pChildDlg->SetEdited(true);
	}

	SetEditStates(true);

	auto zoneMangetServicePtr = IZoneManagerActivator::Get().getPIZoneManageService();
	if (zoneMangetServicePtr != nullptr)
	{
		zoneMangetServicePtr->addEditor_forDrawAble_3d(m_curZoneBase);
	}
}

void ZoneManageWidget::on_pb_save_clicked()
{
	m_curZoneBase = m_pChildDlg->GetZonneBase();
	QString strID = QString::fromLocal8Bit(m_curZoneBase.strID);
	qnzkna::ZoneManage::CZoneBase stDataOld;
	bool success = m_pIZoneManageService->GetZoneBase(strID, stDataOld);
	success = m_pIZoneManageService->EditZoneBase(m_curZoneBase);

	QStringList strIDlist;
	success = m_pIZoneManageService->JudgeZoneIntersect(strID, strIDlist);
	if (success == true)
	{
		bool bLine = false;
		if (m_curZoneBase.nZoneType == qnzkna::ZoneManage::CZoneBase::LINE)
		{
			if (strIDlist.size() == 1)
			{
				success = m_pIZoneManageService->ReplanAirway_2(strID, strIDlist[0]);
				if (success == false)
				{
					success = m_pIZoneManageService->RemoveZoneBase(strID);
				}
				bLine = true;
			}
		}

		if (bLine == false)
		{
			QString strTips = tr2("当前区域与\n");
			for (auto itr : strIDlist)
			{
				qnzkna::ZoneManage::CZoneBase stDataTemp;
				success = m_pIZoneManageService->GetZoneBase(strID, stDataTemp);
				strTips += tr2("区域(ID:") + itr + tr2(", 属性:") + QString::fromLocal8Bit(qnzkna::ZoneManage::ZonePropertyToString(stDataTemp.nProperty)) + tr2(")\n");
			}
			strTips += tr2("有交集，是否修改？");

			if (LyMessageBox::question(nullptr, tr2("提示"), strTips, QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
			{
				success = m_pIZoneManageService->EditZoneBase(stDataOld);
				return;
			}
		}
	}

	UpdateWindowData();

	UpdateTreeData();

	if (m_pChildDlg)
	{
		m_pChildDlg->SetEdited(false);
	}
	SetEditStates(false);
	IZoneManagerActivator::Get().getPIZoneManageService()->endEditing_forDrawAble_3d();
}

void ZoneManageWidget::on_pushButton_4_clicked()
{
	SetEditStates(false);

	IZoneManagerActivator::Get().getPIZoneManageService()->endEditing_forDrawAble_3d();
}

void ZoneManageWidget::on_pb_delete_clicked()
{
	QTreeWidgetItem* pSelectedItem = ui->treeWidget->currentItem();
	if (pSelectedItem == nullptr)
	{
		return;
	}
	QString strID = pSelectedItem->data(1, Qt::UserRole).toString();

	if (strID == "")
	{
		LyMessageBox::warning(nullptr, tr2("提示"), tr2("请选中一个区域"), QMessageBox::Ok);
		return;
	}

	if (LyMessageBox::question(nullptr, tr2("警告"), tr2("确定删除此区域？"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
		return;
	}

	if (m_pIZoneManageService)
	{
		m_pIZoneManageService->RemoveZoneBase(QString::fromLocal8Bit(m_curZoneBase.strID));

		m_pIZoneManageService->delADrawAble_3d(m_curZoneBase.strID);
	}

	UpdateWindowData();
	UpdateTreeData();

}

void ZoneManageWidget::on_pb_ghhl_clicked()
{
	QString hlID = "hanglu";
	QString quyu = "quyu";

	bool isreplan = false;
	if (m_pIZoneManageService)
		isreplan = m_pIZoneManageService->ReplanAirway_2(hlID, quyu);

	if (isreplan) {
		UpdateWindowData();
		UpdateTreeData();

	}

}

void ZoneManageWidget::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
	SetEditStates(false);

	if (item == nullptr)
		return;

	QString strID = item->data(1, Qt::UserRole).toString();
	if (strID != "")
	{
		for (auto itr : m_ZoneBaseVec)
		{
			if (0 == strID.compare(QString::fromLocal8Bit(itr.strID)))
			{
				m_curZoneBase = itr;
				if (m_pChildDlg != nullptr)
				{
					m_pChildDlg->SetZonebase(itr, false);
				}
			}
		}
	}

	int nCount = item->childCount();

	ui->treeWidget->disconnect(ui->treeWidget, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(on_treeWidget_itemClicked(QTreeWidgetItem *, int)));

	bool bCheck = false;
	if (item->checkState(0) == Qt::Checked)
	{
		bCheck = true;
		if (nCount > 0)
		{
			for (int i = 0; i < nCount; i++)
			{

				QTreeWidgetItem* itemChild = item->child(i);
				itemChild->setCheckState(0, Qt::Checked);
			}
			UpdateParentItem(item);
		}
		else
		{
			UpdateParentItem(item);
		}
	}
	else if (Qt::Unchecked == item->checkState(0))
	{
		if (nCount > 0)
		{
			for (int i = 0; i < nCount; i++)
			{

				QTreeWidgetItem* itemChild = item->child(i);
				itemChild->setCheckState(0, Qt::Unchecked);
			}
			UpdateParentItem(item);
		}
		else
		{
			UpdateParentItem(item);
		}
	}

	UpdateShowStates();

	connect(ui->treeWidget, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(on_treeWidget_itemClicked(QTreeWidgetItem *, int)));
}

void ZoneManageWidget::on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
	double curLon = m_curZoneBase.stRoundParameter.dLon;
	double curLat = m_curZoneBase.stRoundParameter.dLat;

	if (m_curZoneBase.nZoneType == qnzkna::ZoneManage::CZoneBase::POLY ||
		m_curZoneBase.nZoneType == qnzkna::ZoneManage::CZoneBase::LINE) {
		if (m_curZoneBase.stZonePointVec.size() > 0) {
			curLon = m_curZoneBase.stZonePointVec[0].dLon;
			curLat = m_curZoneBase.stZonePointVec[0].dLat;
		}
	}

	SetScreenCenter(curLon, curLat);
}

void ZoneManageWidget::on_pbtnAreaName_clicked()
{
	b_IsShowTextAll = !b_IsShowTextAll;
	if (b_IsShowTextAll)
	{
		DrawTextFunctionThree();

	}
	else
	{
		ClearTextFunctionThree();

	}
}

void ZoneManageWidget::on_pushButton_3_clicked()
{
	LY_ZoneManagerInterface::Get().SendMsgToAll(IInterface::MSG_Create, 51003, nullptr);
}

void ZoneManageWidget::on_checkBox_stateChanged(int nStates)
{
	if (nStates == 0)
	{
		b_IsShowTextAll = false;
		ClearTextFunctionThree();

	}
	else
	{
		b_IsShowTextAll = true;
		DrawTextFunctionThree();

	}
}

void ZoneManageWidget::on_checkBox_2_stateChanged(int nStates)
{
	LY_ZoneManagerInterface::Get().SetLayerVisible("经纬网格10002", nStates);
	LY_ZoneManagerInterface::Get().SetLayerVisible("经纬网格1000", nStates);
}

void ZoneManageWidget::on_btnlablecolor_clicked()
{
	if (LY_ZoneManager::Get().GetCDlgSet3DLableColor() == nullptr)
	{
		return;
	}

	if (LY_ZoneManager::Get().GetCDlgSet3DLableColor()->isHidden())
	{
		LY_ZoneManager::Get().GetCDlgSet3DLableColor()->Init();
		LY_ZoneManager::Get().GetCDlgSet3DLableColor()->show();
	}
	else
	{
		LY_ZoneManager::Get().GetCDlgSet3DLableColor()->hide();
	}
}

void ZoneManageWidget::on_btnlandIcon_clicked()
{
	isDrowIcon = true;
}
