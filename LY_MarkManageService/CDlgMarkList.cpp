#include "CDlgMarkList.h"
#include "ui_CDlgMarkList.h"
#include "QCString.h"
#include "LY_MarkManageService.h"
#include "IMarkManageImpl.h"
#include "DataManage/IMarkManageService.h"
#include "lyUI/LyMessageBox.h"
#include "lyUI/LyWidget.h"
#include "CombatZoneEditDialog.h"
#include "IMarkManageServiceActivator.h"
#include "Bag_XygsZCNEO_Markp.h"
#include <QCheckBox>

CDlgMarkList::CDlgMarkList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CDlgMarkList)
{
    ui->setupUi(this);

    m_headerlabels << tr2("序号") << tr2("ID") << tr2("名称") << tr2("类型");
    m_headerlabels << tr2("威胁值") ;
    m_headerlabels << tr2("大小") << tr2("经度") << tr2("维度");
    m_headerlabels << tr2("高度") << tr2("发现时间");
    m_headerlabels << tr2("简述");

	m_SearchCenterPoint.setLongitude(0);
	m_SearchCenterPoint.setLatitude(0);
	m_SearchCenterPoint.setAltitude(0);

	m_TakeOffPoint.setLongitude(0);
	m_TakeOffPoint.setLatitude(0);
	m_TakeOffPoint.setAltitude(0);

    UpdateTrackInfoToListHeading();

    CreateTableMenu();
    TabWidgetConnet();

    connect(ui->tableWidget_5, SIGNAL(currentChanged(int)), this, SLOT(tabWidget_currentChanged(int)));

    m_bUpdateFlg = false;

    m_DBMarkType.push_back(tr("Tank"));
    m_DBMarkType.push_back(tr("Vehicle"));
    m_DBMarkType.push_back(tr("Army"));
    m_DBMarkType.push_back(tr("Radar"));
    m_DBMarkType.push_back(tr("Airport"));
    m_DBMarkType.push_back(tr("Bridge"));
    m_DBMarkType.push_back(tr("MilitaryFactory"));

    m_pIWeaponManageService = IMarkManageServiceActivator::Get().getPIWeaponManageService();
	if (m_pIWeaponManageService == nullptr)
		return;

	localWeaponNameBindToId();

    ui->comboBox->insertItem(0, tr2("未选择"));
    ui->comboBox->setItemData(0, ThreatenByNone);
    ui->comboBox->insertItem(1, tr2("搜索区域中心点距离"));
    ui->comboBox->setItemData(1, ThreatenByAreaCenterPoint);
    ui->comboBox->insertItem(2, tr2("起飞点距离"));
    ui->comboBox->setItemData(2, ThreatenByTakeOffPoint);
}

CDlgMarkList::~CDlgMarkList()
{
    delete ui;
}

void CDlgMarkList::TabWidgetConnet()
{
    connect(ui->tableWidget_5, SIGNAL(customContextMenuRequested(const QPoint)), this, SLOT(customContextMenuRequested(const QPoint)));
	connect(ui->tableWidget_5, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(tableWidget_cellDoubleClicked(int, int)));

}

void CDlgMarkList::CreateTableMenu()
{
    m_TableMenu = new QMenu(this);
    m_MenuAdd = m_TableMenu->addAction(tr2("增加"));
    m_MenuEdit = m_TableMenu->addAction(tr2("修改"));
    m_MenuRemove = m_TableMenu->addAction(tr2("删除"));
    m_MenuUpdate = m_TableMenu->addAction(tr2("更新"));
    m_MenuImg = m_TableMenu->addAction(tr2("图片"));
    m_MenuVideo = m_TableMenu->addAction(tr2("视频"));
    m_MenuMerge = m_TableMenu->addAction(tr2("合并"));
    m_MenuMarkState = m_TableMenu->addAction(tr2("状态"));

    m_MarkState = new QMenu();
    m_MenuDestroy = m_MarkState->addAction(tr2("完成摧毁"));
    m_MenuPressing = m_MarkState->addAction(tr2("完成压制"));
    m_MenuAttack = m_MarkState->addAction(tr2("正在攻击"));
    m_MenuUndestructed = m_MarkState->addAction(tr2("未完成摧毁"));
    m_MenuUnsuppressed = m_MarkState->addAction(tr2("未完成压制"));
    m_MenuDamage = m_MarkState->addAction(tr2("目标损伤"));
    m_MenuDisapper = m_MarkState->addAction(tr2("目标消失"));
    m_MenuMarkState->setMenu(m_MarkState);

    connect(m_MenuAdd, SIGNAL(triggered()), this, SLOT(on_Menu_Add()));
    connect(m_MenuEdit, SIGNAL(triggered()), this, SLOT(on_Menu_Edit()));
    connect(m_MenuRemove, SIGNAL(triggered()), this, SLOT(on_Menu_Remove()));
    connect(m_MenuUpdate, SIGNAL(triggered()), this, SLOT(on_Menu_Update()));
	connect(m_MenuImg, SIGNAL(triggered()), this, SLOT(on_Menu_Image()));
	connect(m_MenuVideo, SIGNAL(triggered()), this, SLOT(on_Menu_Video()));
    connect(m_MenuMerge, SIGNAL(triggered()), this, SLOT(on_Menu_Merge()));

	connect(m_MenuDestroy, SIGNAL(triggered()), this, SLOT(on_Menu_Destroy()));
	connect(m_MenuPressing, SIGNAL(triggered()), this, SLOT(on_Menu_Pressing()));
	connect(m_MenuAttack, SIGNAL(triggered()), this, SLOT(on_Menu_Attack()));
	connect(m_MenuUndestructed, SIGNAL(triggered()), this, SLOT(on_Menu_Undestructed()));
	connect(m_MenuUnsuppressed, SIGNAL(triggered()), this, SLOT(on_Menu_Unsuppressed()));
	connect(m_MenuDamage, SIGNAL(triggered()), this, SLOT(on_Menu_Damage()));
	connect(m_MenuDisapper, SIGNAL(triggered()), this, SLOT(on_Menu_Disapper()));
}

void CDlgMarkList::UpdateTrackInfoToListHeading()
{
# if 0

    ui->tableWidget->setColumnCount(m_headerlabels.size());
    ui->tableWidget->setHorizontalHeaderLabels(m_headerlabels);

    ui->tableWidget->verticalHeader()->setHidden(true);
    ui->tableWidget->setSelectionBehavior(QTableWidget::SelectRows);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setAlternatingRowColors(true);
    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tableWidget->setSortingEnabled(true);
    ui->tableWidget->sortItems(0);
#else
	ui->tableWidget_5->setColumnCount(m_headerlabels.size());
	ui->tableWidget_5->setHorizontalHeaderLabels(m_headerlabels);
	ui->tableWidget_5->verticalHeader()->setHidden(true);
	ui->tableWidget_5->setSelectionBehavior(QTableWidget::SelectRows);
	ui->tableWidget_5->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->tableWidget_5->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui->tableWidget_5->setAlternatingRowColors(true);
	ui->tableWidget_5->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->tableWidget_5->setSortingEnabled(true);
	ui->tableWidget_5->sortItems(0);
#endif
}

void CDlgMarkList::updateTableNo(QTableWidget * pTable)
{
	if (pTable != nullptr)
	{
		int count = pTable->rowCount();
		for (int i = 0; i < count; i++)
		{
			pTable->item(i, 0)->setText(QString::number(i + 1));
		}
	}
}

void CDlgMarkList::UpdataWindowData()
{
    m_MarkInfoVec.clear();
    m_MarkInfoVec = qnzkna::MarkManage::IMarkManageImpl::Get().GetMarkList();
}

void CDlgMarkList::UpdataWindowDataByAction()
{
    if (IMarkManageServiceActivator::Get().getPTaskManageService() == nullptr)
	{
		return;
	}
    int nActionID = IMarkManageServiceActivator::Get().getPTaskManageService()->GetActivatingActionID();

    m_MarkInfoVec.clear();

    CRelationActionMarkVec m_RelationActionMarkVec = IMarkManageServiceActivator::Get().getPTaskManageService()->GetRelationMark(nActionID);

    for ( auto data :m_RelationActionMarkVec)
	{
        SystemMarkInfo stinfo = qnzkna::MarkManage::IMarkManageImpl::Get().GetMark(data.GetMarkID());
		if (stinfo.GetID() != "")
		{
            m_MarkInfoVec.push_back(stinfo);
		}
	}
}

void CDlgMarkList::UpdateListData()
{
    m_nActionID = IMarkManageServiceActivator::Get().getPTaskManageService()->GetActivatingActionID();
    ui->tableWidget_5->clearContents();
    ui->tableWidget_5->setRowCount(m_MarkInfoVec.size());
    ui->tableWidget_5->setSortingEnabled(false);

    if (m_MarkInfoVec.size() == 0)
		return;

    int nNo = 0;
    if (m_ThreatenMarkMap.size() == 0)
	{
        for (auto data : m_MarkInfoVec)
		{
			UpdataListData(ui->tableWidget_5, data, nNo);
			nNo++;
		}
	}
	else
	{
        auto itrEnd = m_ThreatenMarkMap.rbegin();
		double dMaxDis = itrEnd->first;
        if (m_ThreatenMarkMap.size() == 1)
		{
			dMaxDis = 2 * dMaxDis;
		}
		else
		{
			dMaxDis += 1e3;
		}
        for (auto itr:m_ThreatenMarkMap)
		{
			double dTh = (1-itr.first / dMaxDis) *1e3;
			UpdataListData(ui->tableWidget_5, itr.second, nNo, dTh);
			nNo++;
		}
	}

    ui->tableWidget_5->setSortingEnabled(true);
    ui->tableWidget_5->sortItems(0);
}

void CDlgMarkList::UpdataListData(QTableWidget *pQTable, SystemMarkInfo& stMarkInfo, int nNo, double dThreaten)
{
    pQTable->setRowCount(nNo + 1);

    QString strNo, strID, strName, strType, strAttribute, strMaterial, strLon, strLat, strHeight, strSpeed, strPitch,
		strMissionType, strDirections, strThreatenDegree, strFindTime, strFindEqu,
		 strSizeX, strSizeY, strSizeZ, strDectRequire, strSize,strState;
    if (m_MarkInfoVec.size() >= 10)
    {
        strNo = QString::number(nNo).sprintf("%02d", nNo + 1);
    }
    else
    {
        strNo = QString::number(nNo + 1);
    }
    strID = QString::fromStdString(stMarkInfo.GetID());
    strName = QString::fromStdString(stMarkInfo.GetName());
    strType = QString::fromLocal8Bit(ESystemMarkTypeToString(stMarkInfo.GetType()));
    strAttribute = GetAttributeString(stMarkInfo.GetAttribute());
    strMaterial = GetMaterialString(stMarkInfo.GetMaterial());
    strLon = QString::number(stMarkInfo.GetLon(), 'f', 7);
    strLat = QString::number(stMarkInfo.GetLat(), 'f', 7);
    strHeight = QString::number(stMarkInfo.GetHeight(), 'f', 2);
    strSpeed = QString::number(stMarkInfo.GetSpeed(), 'f', 2);
    strPitch = QString::number(stMarkInfo.GetPitch(), 'f', 2);
    strMissionType = GetMissionTypeString(stMarkInfo.GetMissionType());
    strDirections = QString::fromStdString(stMarkInfo.GetDirections());
	if (dThreaten == 0)
	{
		strThreatenDegree = "-";
	}
	else
	{
		strThreatenDegree = QString::number(dThreaten, 'f', 2);
	}
    strFindTime = QString::fromStdString(stMarkInfo.GetFindTime());
    strFindEqu = QString::fromStdString(stMarkInfo.GetFindEqu());

    strSizeX = QString::number(stMarkInfo.GetSizeX(), 'f', 2);
    strSizeY = QString::number(stMarkInfo.GetSizeY(), 'f', 2);
    strSizeZ = QString::number(stMarkInfo.GetSizeZ(), 'f', 2);
    strSize = strSizeX + " x " + strSizeY + " x " + strSizeZ;
    strDectRequire = QString::fromStdString(stMarkInfo.GetDetectRequest());
    strState = GetMarkState(stMarkInfo.GetStats());

    if (stMarkInfo.GetType() == ESystemMarkType_Building || stMarkInfo.GetType() == ESystemMarkType_Unknow)
    {
        strSpeed = "-";
        strPitch = "-";
    }

    QTableWidgetItem* item0 = new QTableWidgetItem(strNo);
    QTableWidgetItem* item1 = new QTableWidgetItem(strID);
    QTableWidgetItem* item2 = new QTableWidgetItem(strName);
    QTableWidgetItem* item3 = new QTableWidgetItem(strType);
    QTableWidgetItem* item4 = new QTableWidgetItem(strThreatenDegree);

    QTableWidgetItem* item8 = new QTableWidgetItem(strSize);
    QTableWidgetItem* item9 = new QTableWidgetItem(strLon);
    QTableWidgetItem* item10 = new QTableWidgetItem(strLat);
    QTableWidgetItem* item11 = new QTableWidgetItem(strHeight);

    QTableWidgetItem* item14 = new QTableWidgetItem(strFindTime);

    QTableWidgetItem* item18 = new QTableWidgetItem(strDirections);

    pQTable->setItem(nNo, 0, item0);
    pQTable->setItem(nNo, 1, item1);
    pQTable->setItem(nNo, 2, item2);
    pQTable->setItem(nNo, 3, item3);
    pQTable->setItem(nNo, 4, item4);

    pQTable->setItem(nNo, 5, item8);
    pQTable->setItem(nNo, 6, item9);
    pQTable->setItem(nNo, 7, item10);
    pQTable->setItem(nNo, 8, item11);

    pQTable->setItem(nNo, 9, item14);

    pQTable->setItem(nNo, 10, item18);

	for (int i=0;i<m_headerlabels.size();i++)
	{
		pQTable->item(nNo, i)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	}

}

QString CDlgMarkList::GetTypeString(ESystemMarkType eType)
{
    switch (eType)
    {
    case ESystemMarkType_Unknow:
        return tr2("未知");
    case ESystemMarkType_UAV:
        return tr2("飞机");
    case ESystemMarkType_Person:
        return tr2("生物");
    case ESystemMarkType_Vehicle:
        return tr2("车辆");
    case ESystemMarkType_Building:
        return tr2("建筑");
    case ESystemMarkType_Fire:
        return tr2("普通火");
    default:
        return tr2("未知");
    }
    return tr2("未知");
}

QString CDlgMarkList::GetAttributeString(ESystemMarkAttribute eType)
{
    switch (eType)
    {
    case ESystemMarkAttribute_Unknow:
        return tr2("未知");
    case ESystemMarkAttribute_Enemy:
        return tr2("严重危险");
    case ESystemMarkAttribute_Me:
        return tr2("危险");
    case ESystemMarkAttribute_Friend:
        return tr2("紧急");
    case ESystemMarkAttribute_Neutral:
        return tr2("一般");
    case ESystemMarkAttribute_Civil:
        return tr2("普通");
    default:
        return tr2("未知");
    }
    return tr2("未知");
}

QString CDlgMarkList::GetMaterialString(ESystemMarkMaterial eType)
{
    switch (eType)
    {
    case ESystemMarkMaterial_Unknow:
        return tr2("未知");
    case ESystemMarkMaterial_Metal:
        return tr2("金属材料");
    case ESystemMarkMaterial_Organic:
        return tr2("有机合成材料");
    case ESystemMarkMaterial_Inorganic:
        return tr2("玻璃-陶瓷类");
    case ESystemMarkMaterial_Fixture:
        return tr2("钢筋混凝土");
    case ESystemMarkMaterial_Tile:
        return tr2("砖石");
    case ESystemMarkMaterial_Dust:
        return tr2("土方");
    case ESystemMarkMaterial_Anima:
        return tr2("生物体");
    case ESystemMarkMaterial_UnSpot:
        return tr2("未识别");
    default:
        return tr2("未知");
    }
    return tr2("未知");
}

QString CDlgMarkList::GetMissionTypeString(EMissionType eType)
{
    switch (eType)
    {
    case EMissionType_None:
        return tr2("未知");
    case EMissionType_Attack:
        return tr2("打击");
    case EMissionType_Detect:
        return tr2("监测");
    case EMissionType_Protect:
        return tr2("防护");
    default:
        return tr2("未知");
    }
    return tr2("未知");
}

QString CDlgMarkList::GetThreatenDegreeString(EThreatenDegree eType)
{
    switch (eType)
    {
    case EThreatenDegree_ONE:
        return tr("1");
    case EThreatenDegree_TWO:
        return tr("2");
    case EThreatenDegree_THREE:
        return tr("3");
    case EThreatenDegree_FORE:
        return tr("4");
    case EThreatenDegree_FIVE:
        return tr("5");
    case EThreatenDegree_NONE:
        return tr2("未知");
    default:
        return tr2("未知");
    }
    return tr2("未知");
}

QString CDlgMarkList::GetZoneAttributeString(EZoneAttribute eType)
{
	switch (eType)
	{
    case EZoneAttribute_Unknow:
        tr2("未知");
    case EZoneAttribute_Mark:
        return tr2("标记区域");
    case EZoneAttribute_Attack:
        return tr2("攻击区域");
    case EZoneAttribute_Danger:
        return tr2("危险区域");
    case EZoneAttribute_Safe:
        return tr2("安全区域");
    case EZoneAttribute_Action:
        return tr2("行动区域");
	default:
        return tr2("未知");
	}
    return tr2("未知");
}

QString CDlgMarkList::GetZoneShapeString(EZoneShape eType)
{
	switch (eType)
	{
    case EZoneShape_Polygon:
        return tr2("多边形");
    case EZoneShape_Circle:
        return tr2("圆形");
    case EZoneShape_Sector:
		return tr2("扇形");
	default:
        return tr2("多边形");
	}
}

void CDlgMarkList::Update(qnzkna::TaskManage::INebulaTaskManageService* sub)
{
	on_Menu_Update();

}

void CDlgMarkList::showEvent(QShowEvent *event)
{

	UpdataWindowDataByAction();
    UpdateListData();
}

void CDlgMarkList::on_Menu_Add()
{
    if (LY_MarkManageService::Get().GetDlgMark()->isVisible())
    {
        LY_MarkManageService::Get().GetDlgMark()->hide();
    }
    LY_MarkManageService::Get().GetDlgMark()->SetDataType(1);
    LY_MarkManageService::Get().GetDlgMark()->InitWindowCtrl();
    LY_MarkManageService::Get().GetDlgMark()->ShowTop();
}

void CDlgMarkList::on_Menu_Edit()
{
    int nHitrow = ui->tableWidget_5->currentRow();

	if (nHitrow == -1)
	{

        LyMessageBox::information(NULL, tr2("提示"), tr2("请选择需要编辑的目标!"));
        return ;
	}

	QTableWidgetItem* pItem = ui->tableWidget_5->item(nHitrow, 1);
	QString strID = pItem->text();

    for (auto data : m_MarkInfoVec)
	{
		if (0 == QString::compare(strID, QString::fromStdString(data.GetID())))
		{
            LY_MarkManageService::Get().GetDlgMark()->SetDataType(2);
            LY_MarkManageService::Get().GetDlgMark()->SetWindowInfo(data);
            if (LY_MarkManageService::Get().GetDlgMark()->isVisible())
			{
                LY_MarkManageService::Get().GetDlgMark()->hide();
			}
            LY_MarkManageService::Get().GetDlgMark()->ShowTop();

			break;
		}
	}
}

void CDlgMarkList::on_Menu_Remove()
{
    int nHitrow = ui->tableWidget_5->currentRow();
	if (nHitrow == -1)
	{

        LyMessageBox::information(NULL, tr2("提示"), tr2("请选择需要删除的目标!"));
        return ;
	}

	QTableWidgetItem* pItem = ui->tableWidget_5->item(nHitrow, 1);

	if (pItem == nullptr)
	{
		return;
	}

	QString strID = pItem->text();

    QString strTmp = tr2("请确认是否删除目标(ID:") + strID + ")";
    if (QMessageBox::StandardButton::Yes == LyMessageBox::question(this, tr2("提示"), strTmp, QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel))
	{
		std::string strTartget = strID.toStdString();
        bool bresut = qnzkna::MarkManage::IMarkManageImpl::Get().RmoveTartget(strTartget);
		if (bresut == true)
		{

            LyMessageBox::information(NULL, tr2("提示"), tr2("删除成功"));

			UpdataWindowDataByAction();
			UpdateListData();
		}
		else
		{

            LyMessageBox::information(NULL, tr2("提示"), tr2("删除失败"));
		}
	}

}

void CDlgMarkList::on_Menu_Update()
{
    qnzkna::MarkManage::IMarkManageImpl::Get().RmoveAllTartget();
    qnzkna::MarkManage::IMarkManageImpl::Get().LoadDBData();
    IMarkManageServiceActivator::Get().getPTaskManageService()->ReLoadRelationMarkDB();
    qnzkna::MarkManage::IMarkManageImpl::Get().LoadMissionTypeDBData();
    qnzkna::MarkManage::IMarkManageImpl::Get().InitMapIcon();

	UpdataWindowDataByAction();
	UpdateListData();
}

void CDlgMarkList::on_Menu_Image()
{
	int nHitrow = -1;

	nHitrow = ui->tableWidget_5->currentRow();

	if (nHitrow == -1)
	{

        LyMessageBox::information(NULL, tr2("提示"), tr2("请选择需要编辑的目标图像!"));
        return ;
	}

	QTableWidgetItem* pItem = ui->tableWidget_5->item(nHitrow, 1);
	QString strID = pItem->text();
    LY_MarkManageService::Get().GetDlgMarkImage()->InitImgList(strID.toStdString());
    if (LY_MarkManageService::Get().GetDlgMarkImage()->isVisible()) {
        LY_MarkManageService::Get().GetDlgMarkImage()->hide();
	}
    LY_MarkManageService::Get().GetDlgMarkImage()->show();
}

void CDlgMarkList::on_Menu_Video()
{
	int nHitrow = -1;
	nHitrow = ui->tableWidget_5->currentRow();

	if (nHitrow == -1)
	{

        LyMessageBox::information(NULL, tr2("提示"), tr2("请选择需要边界的目标视频!"));
        return ;
	}

	QTableWidgetItem* pItem = ui->tableWidget_5->item(nHitrow, 1);
	QString strID = pItem->text();
}

void CDlgMarkList::on_Menu_Merge()
{
	int nHitrow = -1;
	nHitrow = ui->tableWidget_5->currentRow();
	int rowCount = ui->tableWidget_5->rowCount();

	if (rowCount == 0)
	{

        LyMessageBox::warning(this, tr2("提示"), tr2("请选择需要合并的目标!"), QMessageBox::Ok);
		return;
	}

	int selectedRow = nHitrow == -1 ? 0 : nHitrow;

    std::vector<QString> MarkNames;
    MarkNames.clear();
	for (int rowIndex = 0; rowIndex < rowCount; rowIndex++)
	{
		QTableWidgetItem* pItem = ui->tableWidget_5->item(rowIndex, 2);
        MarkNames.push_back(pItem->text());
	}
    LY_MarkManageService::Get().getDlgMarkMerge()->InitComBox(MarkNames);
    LY_MarkManageService::Get().getDlgMarkMerge()->setMergeCurIndex(selectedRow);
    LY_MarkManageService::Get().getDlgMarkMerge()->setMergedCurIndex(0);

    if (LY_MarkManageService::Get().getDlgMarkMerge()->isVisible())
	{
        LY_MarkManageService::Get().getDlgMarkMerge()->hide();
	}
    LY_MarkManageService::Get().getDlgMarkMerge()->ShowTop();
}

void CDlgMarkList::customContextMenuRequested(const QPoint &pos)
{
    m_TableMenu->exec(QCursor::pos());
}

void CDlgMarkList::tableWidget_cellDoubleClicked(int row, int column)
{
    int nHitrow = ui->tableWidget_5->currentRow();

	if (nHitrow == -1)
	{

        LyMessageBox::information(NULL, tr2("提示"), tr2("请选择需要编辑的目标!"));
        return ;
	}

	QTableWidgetItem* pItem = ui->tableWidget_5->item(nHitrow, 1);
	QString strID = pItem->text();

    for (auto data : m_MarkInfoVec)
	{
		if (0 == QString::compare(strID, QString::fromStdString(data.GetID())))
		{
			double dLon = data.GetLon();
			double dLat = data.GetLat();
            LY_MarkManageService::Get().SetScreenCenter(dLon, dLat);
			break;
		}
	}

}

void CDlgMarkList::execMergeAction(int mergeIndex, QString mergeName, int mergedIndex, QString mergedName)
{
	if(ui->tableWidget_5 != nullptr)
	{
		QString mergeId = " ", mergedId = " ";
		QString merName = ui->tableWidget_5->item(mergeIndex, 2)->text();
		QString merdName = ui->tableWidget_5->item(mergedIndex, 2)->text();
		if (0 == QString::compare(mergeName, ui->tableWidget_5->item(mergeIndex, 2)->text()))
		{
			mergeId = ui->tableWidget_5->item(mergeIndex, 1)->text();
		}
		if (mergedName == ui->tableWidget_5->item(mergedIndex, 2)->text())
		{
			mergedId = ui->tableWidget_5->item(mergedIndex, 1)->text();
		}

        SystemMarkInfo mergeInfo;
        SystemMarkInfo mergedInfo;
		int dataMerge = 0;
        auto data = m_MarkInfoVec.begin();
		if (mergeId != " " && mergedId != " ")
		{
			bool isSkip = false;
            while (data != m_MarkInfoVec.end())
			{
				if (dataMerge >= 2)
					break;
				if (0 == QString::compare(mergeId, QString::fromStdString(data->GetID())))
				{
					mergeInfo = *data;
					dataMerge++;
					data++;
					continue;
				}
				if (0 == QString::compare(mergedId, QString::fromStdString(data->GetID())))
				{
					mergedInfo = *data;
					dataMerge++;

					continue;
				}
				data++;
			}
		}

        bool bresut = qnzkna::MarkManage::IMarkManageImpl::Get().MergeMarkAToB(IMarkManageServiceActivator::Get().getPTaskManageService()->GetActivatingActionID(),
																					mergedId.toStdString(), mergeId.toStdString());
		if (bresut == true)
		{
			LyMessageBox::information(NULL, tr("Prompt"), tr("Remove successful"));

			UpdataWindowDataByAction();
			UpdateListData();
		}
		else
		{
			LyMessageBox::information(NULL, tr("Prompt"), tr("Remove failed"));
		}
	}
}

void CDlgMarkList::AddDBMark(const TeleMsg_FB_04& dbData)
{
    SystemMarkInfo stInfo;
	auto itr = m_SystemSysNameAndID.find(QString::number(dbData.head.SendId).toStdString());
	QString strDBID;
	if (itr != m_SystemSysNameAndID.end())
	{
		strDBID = QString::fromStdString(itr->second);
	}
	else
	{
		strDBID = QString::number(dbData.head.SendId);
	}

    QString strType = GetTypeString(stInfo.GetTypeENum(dbData.targetType));

    QString strTitle = tr("Individual device '% 1' reports Mark '% 2', do you agree? \n Mark type:% 3, Mark location：%4，%5，%6").arg(strDBID).arg(dbData.targetId).arg(strType).arg(dbData.targetLng).arg(dbData.targetLat).arg(dbData.targetHeight);
    QMessageBox::StandardButton btresule = LyMessageBox::warning(nullptr, tr("Mark addition"), strTitle, QMessageBox::Yes | QMessageBox::No | QMessageBox::Open);

    stInfo.SetID(QString::fromLocal8Bit(dbData.targetId).toStdString());
    stInfo.SetName(QString::fromLocal8Bit(dbData.targetName).toStdString());
    stInfo.SetType(stInfo.GetTypeENum(dbData.targetType));
	stInfo.SetAttribute(stInfo.GetAttributeENum(dbData.sides));
    stInfo.SetLon(dbData.targetLng);
    stInfo.SetLat(dbData.targetLat);
    stInfo.SetHeight(dbData.targetHeight);
	stInfo.SetSpeed(dbData.speed);
	stInfo.SetDirections(QString::fromLocal8Bit(dbData.remark).toStdString());

	if (btresule == QMessageBox::Yes)
	{
        bool bresut = qnzkna::MarkManage::IMarkManageImpl::Get().SetTaregt(stInfo);
		if (bresut == true)
		{

			UpdataWindowDataByAction();
			UpdateListData();
		}
		else
		{
            LyMessageBox::information(nullptr, tr("Prompt"), tr("Individual Mark addition failed"));
		}
	}
	else if ( btresule == QMessageBox::Open)
	{

        if (LY_MarkManageService::Get().GetDlgMark()->isVisible())
		{
            LY_MarkManageService::Get().GetDlgMark()->hide();
		}
        LY_MarkManageService::Get().GetDlgMark()->SetDataType(1);
        LY_MarkManageService::Get().GetDlgMark()->InitWindowCtrl();
        LY_MarkManageService::Get().GetDlgMark()->SetWindowInfo(stInfo);
        LY_MarkManageService::Get().GetDlgMark()->ShowTop();
	}
}

void CDlgMarkList::tableWidget_cellClicked()
{
    CNeubulaActionVec ActionVec = IMarkManageServiceActivator::Get().getPTaskManageService()->GetAllNebulaAction();
	for (auto data:ActionVec)
	{
		if (data.GetActivateStates() == true)
		{
			if (data.GetID() != m_nActionID)
			{
				if (data.GetID()!=currActionID)
				{
					m_bPressEvect = true;
				}
				if (m_bPressEvect)
				{
					QString strTmp = tr("Do you want to switch to the current activation action？");
					if (QMessageBox::StandardButton::Yes != LyMessageBox::question(this, tr("Prompt"), strTmp, QMessageBox::Yes | QMessageBox::No))
					{
						m_bPressEvect = false;
						currActionID = data.GetID();
					}
					else
					{
                        Update(IMarkManageServiceActivator::Get().getPTaskManageService());
						m_nActionID = data.GetID();
                        IMarkManageServiceActivator::Get().getPTaskManageService()->SetActivatingActionID(m_nActionID);
					}
				}
			}
			break;
		}
	}
}

void CDlgMarkList::localWeaponNameBindToId()
{
	if (m_pIWeaponManageService == nullptr)
	{
		return;
	}

	m_SystemSysNameAndID.clear();
	qnzkna::WeaponManage::WeaponSystemMap weaponSystemMap;
	m_pIWeaponManageService->GetWeaponSystemMap(weaponSystemMap);
	for (auto sysItr : weaponSystemMap)
	{
		if (sysItr.second.getSystemType() == qnzkna::WeaponManage::WeaponSystem_Type::WeaponSystem_Type_SOLDIER)
		{
			auto weaponComPonentMap = sysItr.second.GetWeaponComponentMapPtr();
			for (auto weapon : *weaponComPonentMap)
			{
				m_SystemSysNameAndID.insert(std::make_pair(weapon.second.getComponentID(), weapon.second.getComponentName()));
			}
		}
	}
}

void CDlgMarkList::TableWidgetAddCheckBox(QTableWidget *tableWidget, int x, int y, QString text, Qt::CheckState CheckState)
{
	QWidget *widget = new QWidget();
	QHBoxLayout *layout = new QHBoxLayout();
	QCheckBox *checkbox = new QCheckBox();

	checkbox->setText(text);
	checkbox->setCheckState(CheckState);
	layout->addWidget(checkbox, 0, Qt::AlignCenter);
	layout->setMargin(0);
	widget->setLayout(layout);
	tableWidget->setCellWidget(x, y, widget);
}

void CDlgMarkList::UpdateDataByDB()
{
	on_Menu_Update();
}

void CDlgMarkList::updateMenu()
{
	on_Menu_Update();
}

QString CDlgMarkList::GetMarkState(EMarkState eState)
{
	switch (eState)
	{
    case EMarkState_DESTROY:
        return tr2("完成摧毁");
    case EMarkState_PRESSING:
        return tr2("完成压制");
    case EMarkState_ATTACK:
        return tr2("正在攻击");
    case EMarkState_UNDESTRUCTED:
        return tr2("未完成摧毁");
    case EMarkState_UNSUPPRESSED:
        return tr2("未完成压制");
    case EMarkState_Damage:
        return tr2("目标损伤");
    case EMarkState_Disapper:
        return tr2("目标消失");
	default:
        return tr2("完成摧毁");
	}
    return tr2("完成摧毁");
}

void CDlgMarkList::UpdateMarkState(int Num)
{
	int nHitrow = ui->tableWidget_5->currentRow();
	if (nHitrow == -1)
	{
		LyMessageBox::information(NULL, tr("tip"), tr("Please select the task you want to edit!"));
		return;
	}

	QTableWidgetItem* pItem = ui->tableWidget_5->item(nHitrow, 1);
	QString strID = pItem->text();

    for (auto data : m_MarkInfoVec)
	{
		if (0 == QString::compare(strID, QString::fromStdString(data.GetID())))
		{
			data.SetState(data.GetStateENum(Num));
            qnzkna::MarkManage::IMarkManageImpl::Get().UpdateTaregtState(data);
			break;
		}
	}

	on_Menu_Update();
}

void CDlgMarkList::on_Menu_Destroy()
{
    UpdateMarkState(1);
}

void CDlgMarkList::on_Menu_Pressing()
{
    UpdateMarkState(2);
}

void CDlgMarkList::on_Menu_Attack()
{
    UpdateMarkState(3);
}

void CDlgMarkList::on_Menu_Undestructed()
{
    UpdateMarkState(4);
}

void CDlgMarkList::on_Menu_Unsuppressed()
{
    UpdateMarkState(5);
}

void CDlgMarkList::on_Menu_Damage()
{
    UpdateMarkState(6);
}

void CDlgMarkList::on_Menu_Disapper()
{
    UpdateMarkState(7);
}

void CDlgMarkList::SetSearchZoneCenterPoint(const double& dLon, const double& dLat, const double& dAlt)
{
	if (dLon == 0 || dLat == 0)
	{
		return;
	}

	m_SearchCenterPoint.setLongitude(dLon);
	m_SearchCenterPoint.setLatitude(dLat);
	m_SearchCenterPoint.setAltitude(dLat);

	UpdateListDataByThreaten();
}

void CDlgMarkList::SetTakeOffPoint(const double& dLon, const double& dLat, const double& dAlt)
{
	if (dLon == 0 || dLat == 0)
	{
		return;
	}

	m_TakeOffPoint.setLongitude(dLon);
	m_TakeOffPoint.setLatitude(dLat);
	m_TakeOffPoint.setAltitude(dLat);

	UpdateListDataByThreaten();
}

void CDlgMarkList::UpdateListDataByThreaten()
{
    m_ThreatenMarkMap.clear();
    QVariant  qv = ui->comboBox->itemData(ui->comboBox->currentIndex());
    int nIndex = qv.toInt();
    if (nIndex == ThreatenByNone)
    {
        QStringList strIDlist;
        qnzkna::MarkManage::IMarkManageImpl::Get().SetThreatenSequenceIDList(strIDlist);
    }
    else if (nIndex == ThreatenByAreaCenterPoint)
    {
        if (m_SearchCenterPoint.longitude() == 0 || m_SearchCenterPoint.latitude() == 0)
        {
            return;
        }

        UpdataWindowDataByAction();
        for (auto MarkItr : m_MarkInfoVec)
        {
            QGeoCoordinate qGeoPoint2(MarkItr.GetLat(),MarkItr.GetLon());
            double ddis = m_SearchCenterPoint.distanceTo(qGeoPoint2);
            m_ThreatenMarkMap.insert(std::make_pair(ddis, MarkItr));
        }

        QStringList strIDlist;
        for ( auto itr:m_ThreatenMarkMap)
        {
            strIDlist.push_back(QString::fromStdString(itr.second.GetID()));
        }
        qnzkna::MarkManage::IMarkManageImpl::Get().SetThreatenSequenceIDList(strIDlist);
    }
    else if ( nIndex == ThreatenByTakeOffPoint)
    {
        if (m_TakeOffPoint.longitude() == 0 || m_TakeOffPoint.latitude() == 0)
        {
            return;
        }

        UpdataWindowDataByAction();
        for (auto MarkItr : m_MarkInfoVec)
        {
            QGeoCoordinate qGeoPoint2(MarkItr.GetLat(), MarkItr.GetLon());
            double ddis = m_TakeOffPoint.distanceTo(qGeoPoint2);
            m_ThreatenMarkMap.insert(std::make_pair(ddis, MarkItr));
        }

        QStringList strIDlist;
        for (auto itr : m_ThreatenMarkMap)
        {
            strIDlist.push_back(QString::fromStdString(itr.second.GetID()));
        }
        qnzkna::MarkManage::IMarkManageImpl::Get().SetThreatenSequenceIDList(strIDlist);
    }
    UpdateListData();
}

void CDlgMarkList::on_comboBox_currentIndexChanged(int index)
{
	UpdateListDataByThreaten();
}