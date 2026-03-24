#include "qyzonedlg.h"
#include "ZoneManager.h"
#include "ZoneManagerInterface.h"
#include "qyzonerectparadlg.h"
#include "qyzonedotparadlg.h"
#include "AcmGeometry.h"
#include "LyMessageBox.h"
#include "IProj.h"
#include <QPainter>
#include <map3d/IEarth.h>
#include <map3d/EarthEvent.h>
#include <map3d/IGraphics.h>
#include <CreateShape.h>
#include <ShapeClip.h>

CQYZoneDlg::CQYZoneDlg(QWidget *parent): LyDialog(tr2("自建区域"), parent), ui(new Ui::QYZoneDialog())
{
    ui->setupUi((QDialog *)centralWidget());

    initUI();
    setupConnection();
}

CQYZoneDlg::~CQYZoneDlg()
{
    delete ui;
}

void CQYZoneDlg::initUI()
{
    m_bModifyFlag = m_bCreateFlag = m_bMoveInsert = false;
    m_strQYZoneName = QLatin1String("");
    m_iQYZoneType = CIRCLE;
    m_iQYZoneTopHeight = 8000;
    m_iQYZoneBotHeight = 6000;
    m_DwFlashFlag = 0;
    m_bQYZoneSmooth = false;
    m_iCurSelDotIdx = -1;

    ui->splitter->setStretchFactor(0, 2);
    ui->splitter->setStretchFactor(1, 4);
    ui->splitter->setStretchFactor(2, 2);

    ui->tableWidget->setColumnWidth(0, 40);
    ui->tableWidget->setColumnWidth(1, 90);
    ui->tableWidget->setColumnWidth(2, 90);

    m_QYZoneTree = ui->treeWidget;
    m_listCtrlQYZoneParas = ui->tableWidget;

    ui->treeWidget->InitTree();
    m_QYZoneTree->m_pQYZoneDlg = this;

    m_buttonGroup = new QButtonGroup(this);
    m_buttonGroup->addButton(ui->radioButton, 0);
    m_buttonGroup->addButton(ui->radioButton_2, 1);
    m_buttonGroup->addButton(ui->radioButton_3, 2);
    m_buttonGroup->addButton(ui->radioButton_4, 3);
    m_buttonGroup->addButton(ui->radioButton_5, 4);
    m_buttonGroup->addButton(ui->radioButton_6, 5);
    m_buttonGroup->addButton(ui->radioButton_7, 6);
    m_buttonGroup->addButton(ui->radioButton_8, 7);
    m_buttonGroup->addButton(ui->radioButton_9, 8);

    SetModifyState(false);

    ChangeListColumn(m_iQYZoneType);
}

void CQYZoneDlg::setupConnection()
{
    connect(ui->pushButton_refresh, &QPushButton::clicked, this, &CQYZoneDlg::OnBnClickedBtnReset);
    connect(ui->pushButton_new, &QPushButton::clicked, this, &CQYZoneDlg::OnBtnNew);
    connect(ui->pushButton_modify, &QPushButton::clicked, this, &CQYZoneDlg::OnBtnModify);
    connect(ui->pushButton_quit, &QPushButton::clicked, this, &CQYZoneDlg::OnBtnNosave);
    connect(ui->pushButton_save, &QPushButton::clicked, this, &CQYZoneDlg::OnBtsavezone);
    connect(ui->pushButton_clear, &QPushButton::clicked, this, &CQYZoneDlg::OnBnClickedClearItems);
    connect(ui->pushButton, &QPushButton::clicked, this, &CQYZoneDlg::close);
    connect(ui->pushButton_3, &QPushButton::clicked, this, &CQYZoneDlg::OnBnClickedBtnOutputParasToWord);

    connect(ui->radioButton, &QRadioButton::clicked, this, &CQYZoneDlg::OnRadioClickedSlot);
    connect(ui->radioButton_2, &QRadioButton::clicked, this, &CQYZoneDlg::OnRadioClickedSlot);
    connect(ui->radioButton_3, &QRadioButton::clicked, this, &CQYZoneDlg::OnRadioClickedSlot);
    connect(ui->radioButton_4, &QRadioButton::clicked, this, &CQYZoneDlg::OnRadioClickedSlot);
    connect(ui->radioButton_5, &QRadioButton::clicked, this, &CQYZoneDlg::OnRadioClickedSlot);
    connect(ui->radioButton_6, &QRadioButton::clicked, this, &CQYZoneDlg::OnRadioClickedSlot);
    connect(ui->radioButton_7, &QRadioButton::clicked, this, &CQYZoneDlg::OnRadioClickedSlot);
    connect(ui->radioButton_8, &QRadioButton::clicked, this, &CQYZoneDlg::OnRadioClickedSlot);
    connect(ui->radioButton_9, &QRadioButton::clicked, this, &CQYZoneDlg::OnRadioClickedSlot);

    connect(ui->tableWidget, &QTableWidget::itemPressed, this, &CQYZoneDlg::OnTableItemClick);
}

void CQYZoneDlg::Refresh()
{
    OnBnClickedBtnReset();
}

void CQYZoneDlg::WriteKyList(QUYU_Zone &zone, bool InitFlg )
{
    ui->comboBox->setCurrentIndex(zone.attr.type);
    ui->cboBorderStyle->setPenStyle((Qt::PenStyle)zone.xianxing);
    m_iQYZoneType = zone.type;

    setRadioState(m_iQYZoneType);

    ui->lineEdit->setText(QString::number(zone.toph));
    ui->lineEdit_2->setText(QString::number(zone.both));
    ui->cboFillStyle->setBrushStyle((Qt::BrushStyle)zone.tianchong);
    unsigned int uiV = zone.kycolor;
    QColor c = QColor(uiV & 0xFF, (uiV >> 8) & 0xFF, (uiV >> 16) & 0xFF);
    ui->mFontColorButton->setColor(c);

    if (InitFlg)
    {
        ChangeListColumn(m_iQYZoneType);
    }

    QString str;
    int jwd;
    int count;

    removeTableRow();

    if (zone.data.n == 0)
    {
        str = tr2("%1").arg(tr2(zone.name));
        ui->groupBox_2->setTitle(str);
        return;
    }

    for (int k = 0; k < zone.data.n; k++)
    {
        count = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(count);

        str = QStringLiteral("%1").arg(k + 1);
        QTableWidgetItem *item1 = new QTableWidgetItem;
        item1->setText(str);
        ui->tableWidget->setItem(count, 0, item1);

        jwd = (int)(zone.data.h[k].x * 3600 + 0.5);
        str = QStringLiteral("%1.%2'%3\"").arg(jwd / 3600, 3, 10, QChar('0')).arg(jwd % 3600 / 60, 2, 10, QChar('0')).arg(jwd % 3600 % 60, 2, 10, QChar('0'));
        QTableWidgetItem *item2 = new QTableWidgetItem;
        item2->setText(str);
        ui->tableWidget->setItem(count, 1, item2);

        jwd = (int)(zone.data.h[k].y * 3600 + 0.5);
        str = QStringLiteral("%1.%2'%3\"").arg(jwd / 3600, 2, 10, QChar('0')).arg(jwd % 3600 / 60, 2, 10, QChar('0')).arg(jwd % 3600 % 60, 2, 10, QChar('0'));
        QTableWidgetItem *item3 = new QTableWidgetItem;
        item3->setText(str);
        ui->tableWidget->setItem(count, 2, item3);

        if (m_iQYZoneType == CIRCLE || m_iQYZoneType == PIE)
        {
            jwd = (int)zone.data.h[2].x;
            str = QStringLiteral("%1").arg(jwd);
            QTableWidgetItem *item4 = new QTableWidgetItem;
            item4->setText(str);
            ui->tableWidget->setItem(count, 3, item4);
        }
        if ((m_iQYZoneType == POLY || m_iQYZoneType == LINE) && k != 0)
        {
            str = ZoneManager::Get().RtnDir_and_Dis_str(zone.data.h[k - 1].x, zone.data.h[k - 1].y, zone.data.h[k].x, zone.data.h[k].y);

            QTableWidgetItem *item5 = new QTableWidgetItem;
            item5->setText(str);
            ui->tableWidget->setItem(count - 1, 3, item5);

            if (k == zone.data.n - 1)
            {
                str = ZoneManager::Get().RtnDir_and_Dis_str(zone.data.h[k].x, zone.data.h[k].y, zone.data.h[0].x, zone.data.h[0].y);
                QTableWidgetItem *item51 = new QTableWidgetItem;
                item51->setText(str);
                ui->tableWidget->setItem(count, 3, item51);
            }
        }
        if (m_iQYZoneType == RECTANGLE)
        {
            jwd = (int)zone.data.h[1].x;
            str = QStringLiteral("%1").arg(jwd);
            QTableWidgetItem *item6 = new QTableWidgetItem;
            item6->setText(str);
            ui->tableWidget->setItem(count, 3, item6);

            jwd = (int)zone.data.h[1].y;
            str = QStringLiteral("%1").arg(jwd);
            QTableWidgetItem *item7 = new QTableWidgetItem;
            item7->setText(str);
            ui->tableWidget->setItem(count, 4, item7);

            jwd = (int)zone.data.h[2].x;
            str = QStringLiteral("%1").arg(jwd);
            QTableWidgetItem *item8 = new QTableWidgetItem;
            item8->setText(str);
            ui->tableWidget->setItem(count, 5, item8);

            jwd = (int)zone.data.h[2].y;
            str = QStringLiteral("%1").arg(jwd);
            QTableWidgetItem *item9 = new QTableWidgetItem;
            item9->setText(str);
            ui->tableWidget->setItem(count, 6, item9);
        }
        if (m_iQYZoneType == PIE)
        {
            jwd = (int)zone.data.h[1].x;
            str = QStringLiteral("%1").arg(jwd);
            QTableWidgetItem *item10 = new QTableWidgetItem;
            item10->setText(str);
            ui->tableWidget->setItem(0, 4, item10);

            jwd = (int)zone.data.h[1].y;
            str = QStringLiteral("%1").arg(jwd);
            QTableWidgetItem *item11 = new QTableWidgetItem;
            item11->setText(str);
            ui->tableWidget->setItem(0, 5, item11);
        }
        if (m_iQYZoneType == ELLIPSE)
        {
            jwd = (int)zone.data.h[2].x;
            str = QStringLiteral("%1").arg(jwd);
            QTableWidgetItem *item12 = new QTableWidgetItem;
            item12->setText(str);
            ui->tableWidget->setItem(count, 3, item12);

            jwd = (int)zone.data.h[2].y;
            str = QStringLiteral("%1").arg(jwd);
            QTableWidgetItem *item13 = new QTableWidgetItem;
            item13->setText(str);
            ui->tableWidget->setItem(count, 4, item13);

            jwd = (int)zone.data.h[1].x;
            str = QStringLiteral("%1").arg(jwd);
            QTableWidgetItem *item14 = new QTableWidgetItem;
            item14->setText(str);
            ui->tableWidget->setItem(count, 5, item14);
        }
        if (m_iQYZoneType == CIRCLERING || m_iQYZoneType == PIERING)
        {
            jwd = (int)zone.data.h[2].x;
            str = QStringLiteral("%1").arg(jwd);
            QTableWidgetItem *item15 = new QTableWidgetItem;
            item15->setText(str);
            ui->tableWidget->setItem(count, 3, item15);

            jwd = (int)zone.data.h[2].y;
            str = QStringLiteral("%1").arg(jwd);
            QTableWidgetItem *item16 = new QTableWidgetItem;
            item16->setText(str);
            ui->tableWidget->setItem(count, 4, item16);
        }
        if (m_iQYZoneType == PIERING)
        {
            jwd = (int)zone.data.h[1].x;

            str = QStringLiteral("%1").arg(jwd);
            QTableWidgetItem *item17 = new QTableWidgetItem;
            item17->setText(str);
            ui->tableWidget->setItem(0, 5, item17);

            jwd = (int)zone.data.h[1].y;
            str = QStringLiteral("%1").arg(jwd);
            QTableWidgetItem *item18 = new QTableWidgetItem;
            item18->setText(str);
            ui->tableWidget->setItem(0, 6, item18);
        }
    }

    str = tr2("%1 区域参数列表 ").arg(tr2(zone.name));
    ui->groupBox_2->setTitle(str);
}

void CQYZoneDlg::insertJWDpt(double ll, double bb, bool bEndflg )
{
    if (m_bModifyFlag == false)
    {
        return;
    }
    if (m_QYZoneTree->m_CurInd < 0 && m_QYZoneTree->m_CurInd >= m_QYZoneTree->m_ZoneArray.size())
    {
        return;
    }
    QUYU_Zone &zone = m_QYZoneTree->m_ZoneArray[m_QYZoneTree->m_CurInd];
    if (bEndflg && zone.data.n < 3)
    {
        return;
    }

    QString str, strjd, strwd;
    double a, b;
    a = ll * 3600 + 0.5;
    b = bb * 3600 + 0.5;
    strjd = QStringLiteral("%1.%2'%3\"").arg((int)a / 3600, 3, 10, QChar('0')).arg((int)a % 3600 / 60, 2, 10, QChar('0')).arg((int)a % 3600 % 60, 2, 10, QChar('0'));
    strwd = QStringLiteral("%1.%2'%3\"").arg((int)b / 3600, 2, 10, QChar('0')).arg((int)b % 3600 / 60, 2, 10, QChar('0')).arg((int)b % 3600 % 60, 2, 10, QChar('0'));

    int count = ui->tableWidget->rowCount();
    if (count == 0)
    {
        if (m_iQYZoneType == RECTANGLE)
        {
            InsertDotList(0, "1", strjd, strwd);
            zone.data.h[0].x = ll;
            zone.data.h[0].y = bb;
            zone.data.h[1].x = 100;
            zone.data.h[1].y = 50;
            zone.data.h[2].x = 0;
            zone.data.h[2].y = 0;
            QYZoneRectParaDlg dlg(zone, this, true);
            if (dlg.exec() == QDialog::Accepted)
            {
                a = dlg.m_zone.data.h[0].x * 3600 + 0.5;
                b = dlg.m_zone.data.h[0].y * 3600 + 0.5;
                strjd = QStringLiteral("%1.%2'%3\"").arg((int)a / 3600, 3, 10, QChar('0')).arg((int)a % 3600 / 60, 2, 10, QChar('0')).arg((int)a % 3600 % 60, 2, 10, QChar('0'));
                strwd = QStringLiteral("%1.%2'%3\"").arg((int)b / 3600, 2, 10, QChar('0')).arg((int)b % 3600 / 60, 2, 10, QChar('0')).arg((int)b % 3600 % 60, 2, 10, QChar('0'));

                QTableWidgetItem *item = ui->tableWidget->item(0, 1);
                item->setText(strjd);

                item = ui->tableWidget->item(0, 2);
                item->setText(strwd);

                str = QStringLiteral("%1").arg(dlg.m_zone.data.h[1].x);
                item = ui->tableWidget->item(0, 3);
                item->setText(str);

                str = QStringLiteral("%1").arg(dlg.m_zone.data.h[1].y);
                item = ui->tableWidget->item(0, 4);
                item->setText(str);

                str = QStringLiteral("%1").arg(dlg.m_zone.data.h[2].x);
                item = ui->tableWidget->item(0, 5);
                item->setText(str);

                str = QStringLiteral("%1").arg(dlg.m_zone.data.h[2].y);
                item = ui->tableWidget->item(0, 6);
                item->setText(str);
            }
            else
            {
                removeTableRow();
            }
        }
        else
        {
            InsertDotList(0, "1", strjd, strwd);
        }
    }
    else if (m_iCurSelDotIdx < 0 || m_iCurSelDotIdx >= count)
    {
        if (m_iQYZoneType == RECTANGLE)
        {
            return;
        }
        if (count >= MAX_ky_point + 2)
        {
            LyMessageBox::warning(this, tr2("警告"), tr2("区域最多100个端点!"));
            return;
        }
        if (
            ((m_iQYZoneType == CIRCLE || m_iQYZoneType == PIE || m_iQYZoneType == ELLIPSE ||
              m_iQYZoneType == CIRCLERING || m_iQYZoneType == PIERING || m_iQYZoneType == POINT) && count < 1))
        {
            str = tr2("%1").arg(count + 1);
            InsertDotList(count, str, strjd, strwd);
        }
        if (m_iQYZoneType == POLY || m_iQYZoneType == LINE)
        {
            ui->tableWidget->item(count - 1, 1)->setText(strjd);
            ui->tableWidget->item(count - 1, 2)->setText(strwd);
            int num = zone.data.n;
            str = ZoneManager::Get().RtnDir_and_Dis_str(zone.data.h[num - 2].x, zone.data.h[num - 2].y, ll, bb);
            if (ui->tableWidget->item(count - 2, 3))
            {
                ui->tableWidget->item(count - 2, 3)->setText(str);
            }
            if (bEndflg && zone.data.n > 2)
            {
                str = ZoneManager::Get().RtnDir_and_Dis_str(ll, bb, zone.data.h[0].x, zone.data.h[0].y);
                ui->tableWidget->item(count - 1, 3)->setText(str);
            }
        }
        if ((m_iQYZoneType == CIRCLE || m_iQYZoneType == PIE || m_iQYZoneType == ELLIPSE
                || m_iQYZoneType == CIRCLERING || m_iQYZoneType == PIERING || m_iQYZoneType == POINT) && count >= 1)
        {
            ui->tableWidget->item(0, 1)->setText(strjd);
            ui->tableWidget->item(0, 2)->setText(strwd);
        }
    }
    else if (m_iCurSelDotIdx >= 0 && m_iCurSelDotIdx < count)
    {
        ui->tableWidget->item(m_iCurSelDotIdx, 1)->setText(strjd);
        ui->tableWidget->item(m_iCurSelDotIdx, 2)->setText(strwd);
    }
    if (m_QYZoneTree->m_CurInd >= 0 && m_QYZoneTree->m_CurInd < m_QYZoneTree->m_ZoneArray.size())
    {
        m_QYZoneTree->m_ZoneArray[ui->treeWidget->m_CurInd] = GetCurrentValue();
    }
    m_bMoveInsert = true;
    if (bEndflg && zone.data.n > 2)
    {
        SetModifyState(false);
        OnBtsavezone();
    }
}

void CQYZoneDlg::InsertMouseMoveJWD(double ll, double bb)
{
    if (m_iQYZoneType != POLY && m_iQYZoneType != LINE)
    {
        return;
    }
    int count = ui->tableWidget->rowCount();
    if (count == 0)
    {
        return;
    }

    QString str, strjd, strwd;
    double a, b;
    a = ll * 3600 + 0.5;
    b = bb * 3600 + 0.5;
    strjd = QStringLiteral("%1.%2'%3\"").arg((int)a / 3600, 3, 10, QChar('0')).arg((int)a % 3600 / 60, 2, 10, QChar('0')).arg((int)a % 3600 % 60, 2, 10, QChar('0'));
    strwd = QStringLiteral("%1.%2'%3\"").arg((int)b / 3600, 2, 10, QChar('0')).arg((int)b % 3600 / 60, 2, 10, QChar('0')).arg((int)b % 3600 % 60, 2, 10, QChar('0'));

    QUYU_Zone zone;
    if (m_QYZoneTree->m_CurInd >= 0 && m_QYZoneTree->m_CurInd < m_QYZoneTree->m_ZoneArray.size())
    {
        zone = m_QYZoneTree->m_ZoneArray[m_QYZoneTree->m_CurInd];
    }

    int num = zone.data.n;
    if (m_bMoveInsert)
    {
        str = QStringLiteral("%1").arg(count + 1);
        InsertDotList(count, str, strjd, strwd);
        str = ZoneManager::Get().RtnDir_and_Dis_str(zone.data.h[num - 2].x, zone.data.h[num - 2].y, ll, bb);
        ui->tableWidget->item(count - 1, 3)->setText(str);
        m_bMoveInsert = false;
    }
    else
    {
        str = ZoneManager::Get().RtnDir_and_Dis_str(zone.data.h[num - 2].x, zone.data.h[num - 2].y, ll, bb);
        ui->tableWidget->item(count - 2, 3)->setText(str);
        ui->tableWidget->item(count - 1, 1)->setText(strjd);
        ui->tableWidget->item(count - 1, 2)->setText(strwd);
    }

    if (m_QYZoneTree->m_CurInd >= 0 && m_QYZoneTree->m_CurInd < m_QYZoneTree->m_ZoneArray.size())
    {
        m_QYZoneTree->m_ZoneArray[m_QYZoneTree->m_CurInd] = GetCurrentValue();
    }
}

void CQYZoneDlg::SetModifyState(bool flag, bool Typeflag)
{
    ui->treeWidget->setEnabled(!flag);
    m_bModifyFlag = flag;
    if (!flag)
    {
        m_bCreateFlag = flag;
        ZoneManager::Get().m_bRoatedZone = flag;
    }
    ui->lineEdit_3->setEnabled(!flag);
    ui->pushButton_refresh->setEnabled(!flag);
    ui->pushButton_new->setEnabled(!flag);
    ui->pushButton_modify->setEnabled(!flag);
    ui->pushButton_clear->setEnabled(flag);
    ui->pushButton_save->setEnabled(flag);
    ui->pushButton_quit->setEnabled(flag);

    ui->comboBox->setEnabled(flag);
    ui->cboBorderStyle->setEnabled(flag);
    ui->cboFillStyle->setEnabled(flag);
    ui->mFontColorButton->setEnabled(flag);
    ui->lineEdit->setEnabled(flag);
    ui->lineEdit_2->setEnabled(flag);

    ui->radioButton->setEnabled(flag && Typeflag);
    ui->radioButton_2->setEnabled(flag && Typeflag);
    ui->radioButton_3->setEnabled(flag && Typeflag);
    ui->radioButton_4->setEnabled(flag && Typeflag);
    ui->radioButton_5->setEnabled(flag && Typeflag);
    ui->radioButton_6->setEnabled(flag && Typeflag);
    ui->radioButton_7->setEnabled(flag && Typeflag);
    ui->radioButton_8->setEnabled(flag && Typeflag);
    ui->radioButton_9->setEnabled(flag && Typeflag);
}

void CQYZoneDlg::ChangeListColumn(int m_zoneType)
{
    removeTableRow();
    for (int i = ui->tableWidget->columnCount() - 1; i >= 3; --i)
    {
        ui->tableWidget->removeColumn(i);
    }
    switch (m_zoneType)
    {
    case CIRCLE:
    {
        ui->tableWidget->insertColumn(3);
        QTableWidgetItem *item = new QTableWidgetItem(tr2("半径"));
        ui->tableWidget->setHorizontalHeaderItem(3, item);
        ui->tableWidget->setColumnWidth(3, 50);
        break;
    }
    case PIE:
    {
        ui->tableWidget->insertColumn(3);
        QTableWidgetItem *item = new QTableWidgetItem(tr2("半径"));
        ui->tableWidget->setHorizontalHeaderItem(3, item);
        ui->tableWidget->setColumnWidth(3, 50);

        ui->tableWidget->insertColumn(4);
        QTableWidgetItem *item1 = new QTableWidgetItem(tr2("起始角"));
        ui->tableWidget->setHorizontalHeaderItem(4, item1);
        ui->tableWidget->setColumnWidth(4, 50);

        ui->tableWidget->insertColumn(5);
        QTableWidgetItem *item2 = new QTableWidgetItem(tr2("终止角"));
        ui->tableWidget->setHorizontalHeaderItem(5, item2);
        ui->tableWidget->setColumnWidth(5, 50);
        break;
    }
    case ELLIPSE:
    {
        ui->tableWidget->insertColumn(3);
        QTableWidgetItem *item = new QTableWidgetItem(tr2("长半轴"));
        ui->tableWidget->setHorizontalHeaderItem(3, item);
        ui->tableWidget->setColumnWidth(3, 55);

        ui->tableWidget->insertColumn(4);
        QTableWidgetItem *item1 = new QTableWidgetItem(tr2("短半轴"));
        ui->tableWidget->setHorizontalHeaderItem(4, item1);
        ui->tableWidget->setColumnWidth(4, 55);

        ui->tableWidget->insertColumn(5);
        QTableWidgetItem *item2 = new QTableWidgetItem(tr2("长轴与正北方向夹角"));
        ui->tableWidget->setHorizontalHeaderItem(5, item2);
        ui->tableWidget->setColumnWidth(5, 110);

        break;
    }
    case POLY:
    case LINE:
    {
        ui->tableWidget->insertColumn(3);
        QTableWidgetItem *item = new QTableWidgetItem(tr2("方向距离(单位：°、Km)"));
        ui->tableWidget->setHorizontalHeaderItem(3, item);
        ui->tableWidget->setColumnWidth(3, 160);
        break;
    }
    case RECTANGLE:
    {
        ui->tableWidget->insertColumn(3);
        QTableWidgetItem *item = new QTableWidgetItem(tr2("长"));
        ui->tableWidget->setHorizontalHeaderItem(3, item);
        ui->tableWidget->setColumnWidth(3, 40);

        ui->tableWidget->insertColumn(4);
        QTableWidgetItem *item1 = new QTableWidgetItem(tr2("宽"));
        ui->tableWidget->setHorizontalHeaderItem(4, item1);
        ui->tableWidget->setColumnWidth(4, 40);

        ui->tableWidget->insertColumn(5);
        QTableWidgetItem *item2 = new QTableWidgetItem(tr2("方向"));
        ui->tableWidget->setHorizontalHeaderItem(5, item2);
        ui->tableWidget->setColumnWidth(5, 50);

        ui->tableWidget->insertColumn(6);
        QTableWidgetItem *item3 = new QTableWidgetItem(tr2("圆角半径"));
        ui->tableWidget->setHorizontalHeaderItem(6, item3);
        ui->tableWidget->setColumnWidth(6, 60);
        break;
    }
    case CIRCLERING:
    {
        ui->tableWidget->insertColumn(3);
        QTableWidgetItem *item = new QTableWidgetItem(tr2("外圈半径"));
        ui->tableWidget->setHorizontalHeaderItem(3, item);
        ui->tableWidget->setColumnWidth(3, 60);

        ui->tableWidget->insertColumn(4);
        QTableWidgetItem *item1 = new QTableWidgetItem(tr2("内圈半径"));
        ui->tableWidget->setHorizontalHeaderItem(4, item1);
        ui->tableWidget->setColumnWidth(4, 60);
        break;
    }
    case PIERING:
    {
        ui->tableWidget->insertColumn(3);
        QTableWidgetItem *item = new QTableWidgetItem(tr2("外圈半径"));
        ui->tableWidget->setHorizontalHeaderItem(3, item);
        ui->tableWidget->setColumnWidth(3, 60);

        ui->tableWidget->insertColumn(4);
        QTableWidgetItem *item1 = new QTableWidgetItem(tr2("内圈半径"));
        ui->tableWidget->setHorizontalHeaderItem(4, item1);
        ui->tableWidget->setColumnWidth(4, 60);

        ui->tableWidget->insertColumn(5);
        QTableWidgetItem *item2 = new QTableWidgetItem(tr2("起始角"));
        ui->tableWidget->setHorizontalHeaderItem(5, item2);
        ui->tableWidget->setColumnWidth(5, 50);

        ui->tableWidget->insertColumn(6);
        QTableWidgetItem *item3 = new QTableWidgetItem(tr2("终止角"));
        ui->tableWidget->setHorizontalHeaderItem(6, item3);
        ui->tableWidget->setColumnWidth(6, 50);
        break;
    }
    }
}

void CQYZoneDlg::removeTableRow()
{
    int rowSum = ui->tableWidget->rowCount();
    for (int i = rowSum; i >= 0; i--)
    {
        ui->tableWidget->removeRow(i);
    }
}

void CQYZoneDlg::setRadioState(int state)
{
    switch (state)
    {
    case 0:
        ui->radioButton->setChecked(true);
        break;
    case 1:
        ui->radioButton_2->setChecked(true);
        break;
    case 2:
        ui->radioButton_3->setChecked(true);
        break;
    case 3:
        ui->radioButton_4->setChecked(true);
        break;
    case 4:
        ui->radioButton_5->setChecked(true);
        break;
    case 5:
        ui->radioButton_6->setChecked(true);
        break;
    case 6:
        ui->radioButton_7->setChecked(true);
        break;
    case 7:
        ui->radioButton_8->setChecked(true);
        break;
    case 8:
        ui->radioButton_9->setChecked(true);
        break;
    default:
        break;
    }
}

void CQYZoneDlg::OnBnClickedBtnReset()
{
    m_bModifyFlag = false;
    m_bCreateFlag = false;
    m_bMoveInsert = true;

    m_iQYZoneType = CIRCLE;
    m_iCurSelDotIdx = -1;
    ui->treeWidget->RemberSetcheck();
    QCoreApplication::processEvents();
    ui->treeWidget->InitTree();
    removeTableRow();
    ui->treeWidget->setItemsCheck();
    ui->mFontColorButton->setColor(Qt::red);
    ui->comboBox->setCurrentIndex(0);
    ui->cboBorderStyle->setPenStyle((Qt::PenStyle)1);
    ui->cboFillStyle->setBrushStyle((Qt::BrushStyle)1);
    ui->groupBox_2->setTitle(tr2("区域参数"));

}

void CQYZoneDlg::OnBtnNew()
{
    QUYU_Zone zone;
    memset(&zone, 0, sizeof(zone));
    m_strQYZoneName = ui->lineEdit_3->text().trimmed();
    if (isNewNameValidate(m_strQYZoneName, zone.name) == false)
    {
        ui->lineEdit_3->setFocus();
        return;
    }

    zone.show[0] = 1;
    if (ui->treeWidget->m_dbx.InsertNewZoneTab(zone))
    {
        ui->treeWidget->m_CurItem = ui->treeWidget->NewLineItem(zone.name);
        ui->treeWidget->setCurrentItem(ui->treeWidget->m_CurItem);
        ui->treeWidget->m_ZoneArray.append(zone);
        ui->treeWidget->m_CurInd = ui->treeWidget->m_ZoneArray.size() - 1;
        ui->treeWidget->m_CurItem->setCheckState(0, Qt::Checked);
        removeTableRow();
    }

    QString str = tr2("%1 区域参数列表 ").arg(tr2(zone.name));
    ui->groupBox_2->setTitle(str);

    m_iCurSelDotIdx = -1;
    m_QYZoneBackup.data.n = 0;
    SetModifyState(true);
    ui->radioButton->click();
    m_bCreateFlag = true;
    m_bMoveInsert = false;

    ui->cboBorderStyle->setPenStyle((Qt::PenStyle)1);
    ui->cboFillStyle->setBrushStyle((Qt::BrushStyle)1);
    ui->mFontColorButton->setColor(Qt::red);
}

void CQYZoneDlg::OnBtnNosave()
{
    SetModifyState(false);

    if (m_QYZoneTree->m_CurInd < 0 || m_QYZoneTree->m_CurInd >= m_QYZoneTree->m_ZoneArray.size() || m_QYZoneTree->m_CurItem == nullptr)
    {
        return;
    }

    if (m_QYZoneBackup.data.n >= 1)
    {
        ui->treeWidget->m_ZoneArray[ui->treeWidget->m_CurInd] = m_QYZoneBackup;
        WriteKyList(ui->treeWidget->m_ZoneArray[ui->treeWidget->m_CurInd]);
    }
    else
    {
        ui->treeWidget->DeletOneItem(ui->treeWidget->m_CurItem);
        removeTableRow();
    }
    m_iCurSelDotIdx = -1;
    m_QYZoneBackup.data.n = 0;
}

void CQYZoneDlg::OnBtnModify()
{
    if (ui->treeWidget->m_CurInd < 0 || ui->treeWidget->m_CurInd >= ui->treeWidget->m_ZoneArray.size() || ui->treeWidget->m_ZoneArray[ui->treeWidget->m_CurInd].data.n == 0)
    {
        LyMessageBox::warning(this, tr2("警告"), tr2("请选择一个区域"));
        return;
    }

    ui->treeWidget->currentItem()->setCheckState(0, Qt::Checked);
    ui->treeWidget->m_ZoneArray[ui->treeWidget->m_CurInd].show[0] = 1;
    m_QYZoneBackup = ui->treeWidget->m_ZoneArray[ui->treeWidget->m_CurInd];
    SetModifyState(true, false);

    if (m_QYZoneBackup.type == POLY || m_QYZoneBackup.type == RECTANGLE)
    {

    }
}

void CQYZoneDlg::OnBtsavezone()
{
    if (ui->treeWidget->m_CurInd < 0 || ui->treeWidget->m_CurInd >= ui->treeWidget->m_ZoneArray.size())
    {
        return;
    }

    const QUYU_Zone &zone = GetCurrentValue();
    if (isZoneParaValidate(zone) == false)
    {
        return;
    }
    ui->treeWidget->m_ZoneArray[ui->treeWidget->m_CurInd] = zone;
    ui->treeWidget->m_dbx.DeleteZoneDatabase(tr2(zone.name));
    ui->treeWidget->m_dbx.InsertNewZoneTab(zone);
    SetModifyState(false);

    m_iCurSelDotIdx = -1;
    m_QYZoneBackup.data.n = 0;

}

void CQYZoneDlg::OnBnClickedClearItems()
{
    removeTableRow();
    if (m_QYZoneTree->m_CurInd < 0 || m_QYZoneTree->m_CurInd > m_QYZoneTree->m_ZoneArray.size())
    {
        return;
    }

    m_QYZoneTree->m_ZoneArray[m_QYZoneTree->m_CurInd] = GetCurrentValue();
    m_iCurSelDotIdx = -1;
    m_bCreateFlag = true;
}

void CQYZoneDlg::OnRadioClickedSlot()
{
    m_iQYZoneType = m_buttonGroup->checkedId();
    ChangeListColumn(m_buttonGroup->checkedId());
}

void CQYZoneDlg::OnTableItemClick(QTableWidgetItem *item)
{
    if (m_bModifyFlag == false)
    {
        return;
    }

    int index = item->row();
    if (index < 0 || index >= ui->tableWidget->rowCount())
    {
        return;
    }
    if (ui->treeWidget->m_CurInd < 0 || ui->treeWidget->m_CurInd >= ui->treeWidget->m_ZoneArray.size())
    {
        return;
    }
    if (m_bModifyFlag && !m_bCreateFlag)
    {
        m_iCurSelDotIdx = index;
    }

    if (QGuiApplication::mouseButtons() == Qt::RightButton)
    {
        if (index == 0 && m_iQYZoneType == RECTANGLE)
        {
            QYZoneRectParaDlg dlg(ui->treeWidget->m_ZoneArray[ui->treeWidget->m_CurInd], this, true);
            if (dlg.exec() == QDialog::Accepted)
            {
                ui->treeWidget->m_ZoneArray[ui->treeWidget->m_CurInd] = dlg.m_zone;
                WriteKyList(ui->treeWidget->m_ZoneArray[ui->treeWidget->m_CurInd]);
            }

        }
        else
        {
            QYZoneDotParaDlg dlg(index, ui->treeWidget->m_ZoneArray[ui->treeWidget->m_CurInd], this, true);
            if (dlg.exec() == QDialog::Accepted)
            {
                ui->treeWidget->m_ZoneArray[ui->treeWidget->m_CurInd] = dlg.getQYZone();
                WriteKyList(ui->treeWidget->m_ZoneArray[ui->treeWidget->m_CurInd]);
            }
        }
    }

}

void CQYZoneDlg::closeEvent(QCloseEvent *e)
{
    if (m_bModifyFlag)
    {
        int msgbox = LyMessageBox::question(this, tr2("保存询问"), tr2("你需要保存区域并退出吗?"), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        if (msgbox == QMessageBox::No)
        {
            OnBtnNosave();
        }
        else if (msgbox == QMessageBox::Yes)
        {
            OnBtsavezone();
            if (m_bModifyFlag)
            {
                return;
            }
        }
        else if (msgbox == QMessageBox::Cancel)
        {
            return;
        }
    }
    QDialog::closeEvent(e);
}

bool CQYZoneDlg::isNewNameValidate(const QString &name, char zname[])
{
    if (!ZoneManager::Get().CheckNameStr(name, 19))
    {
        return false;
    }
    strcpy(zname, name.toLocal8Bit().data());

    if (m_QYZoneTree->m_strSelFolderName != "" && m_QYZoneTree->m_strSelFolderName.length() + name.length() < MAX_ky_name)
    {
        sprintf(zname, "%s/%s", m_QYZoneTree->m_strSelFolderName.toLocal8Bit().data(), name.toLocal8Bit().data());
    }

    for (int i = 0; i < ui->treeWidget->m_ZoneArray.size(); i++)
    {
        if (strcmp(zname, ui->treeWidget->m_ZoneArray[i].name) == 0)
        {
            LyMessageBox::warning(this, tr2("警告"), tr2("区域名称重复!"));
            return false;
        }
    }

    return true;
}

QUYU_Zone CQYZoneDlg::GetCurrentValue()
{
    QUYU_Zone zone;
    if (ui->treeWidget->m_CurInd >= 0 && ui->treeWidget->m_CurInd < ui->treeWidget->m_ZoneArray.size())
    {
        zone = ui->treeWidget->m_ZoneArray[ui->treeWidget->m_CurInd];
    }

    QString str;
    zone.attr.attr = 0;

    zone.xianxing = ui->cboBorderStyle->penStyle();

    zone.tianchong = ui->cboFillStyle->brushStyle();

    QColor d = ui->mFontColorButton->color();
    unsigned int uiV = (d.blue() & 0xFF) << 16 | (d.green() & 0xFF) << 8 | (d.red() & 0xFF);
    zone.kycolor = uiV;

    zone.both = ui->lineEdit_2->text().toFloat();
    zone.toph = ui->lineEdit->text().toFloat();

    zone.attr.type = ui->comboBox->currentIndex();
    zone.attr.threat = 100;

    zone.type = m_iQYZoneType;
    zone.data.n = ui->tableWidget->rowCount();
    for (int k = 0; k < zone.data.n; k++)
    {
        str = ui->tableWidget->item(k, 1)->text();
        zone.data.h[k].x = ZoneManager::Get().StrToJWD(str);
        str = ui->tableWidget->item(k, 2)->text();
        zone.data.h[k].y = ZoneManager::Get().StrToJWD(str);

        if ((m_iQYZoneType == PIE))
        {
            str = ui->tableWidget->item(0, 3)->text();
            zone.data.h[2].x = str.toDouble();

            str = ui->tableWidget->item(0, 4)->text();
            zone.data.h[1].x = str.toDouble();

            str = ui->tableWidget->item(0, 5)->text();
            zone.data.h[1].y = str.toDouble();
        }
        else if (m_iQYZoneType == CIRCLE)
        {
            str = ui->tableWidget->item(0, 3)->text();
            zone.data.h[2].x = str.toDouble();

            zone.data.h[1].x = 0;
            zone.data.h[1].y = 360;
        }
        else if (m_iQYZoneType == RECTANGLE && k == 0)
        {
            str = ui->tableWidget->item(k, 3)->text();
            zone.data.h[1].x = str.toDouble();
            str = ui->tableWidget->item(k, 4)->text();
            zone.data.h[1].y = str.toDouble();
            str = ui->tableWidget->item(k, 5)->text();
            zone.data.h[2].x = str.toDouble();
            str = ui->tableWidget->item(k, 6)->text();

            zone.data.h[2].y = str.toDouble();
        }
        else if (m_iQYZoneType == ELLIPSE)
        {
            str = ui->tableWidget->item(0, 3)->text();
            zone.data.h[2].x = str.toDouble();
            str = ui->tableWidget->item(0, 4)->text();
            zone.data.h[2].y = str.toDouble();
            str = ui->tableWidget->item(0, 5)->text();
            zone.data.h[1].x = str.toDouble();
            zone.data.h[1].y = 0;
        }
        else if (m_iQYZoneType == PIERING)
        {
            str = ui->tableWidget->item(0, 3)->text();
            zone.data.h[2].x = str.toDouble();
            str = ui->tableWidget->item(0, 4)->text();
            zone.data.h[2].y = str.toDouble();
            str = ui->tableWidget->item(0, 5)->text();
            zone.data.h[1].x = str.toDouble();
            str = ui->tableWidget->item(0, 6)->text();
            zone.data.h[1].y = str.toDouble();

        }
        else if (m_iQYZoneType == CIRCLERING)
        {
            str = ui->tableWidget->item(0, 3)->text();
            zone.data.h[2].x = str.toDouble();
            str = ui->tableWidget->item(0, 4)->text();
            zone.data.h[2].y = str.toDouble();

            zone.data.h[1].x = 0;
            zone.data.h[1].y = 360;

        }
    }

    return zone;
}

bool CQYZoneDlg::isZoneParaValidate(const QUYU_Zone &zone)
{
    if ((zone.type == CIRCLE && zone.data.n < 1) || (zone.type == PIE && zone.data.n < 1) ||
            (zone.type == ELLIPSE && zone.data.n != 1) ||
            (zone.type == POLY && zone.data.n < 3) || (zone.type == RECTANGLE && zone.data.n != 1))
    {
        LyMessageBox::warning(this, tr2("警告"), tr2("区域端点数不合理!"));
        return false;
    }

    if (zone.toph <= 100 || zone.toph > 40000)
    {
        LyMessageBox::warning(this, tr2("警告"), tr2("区域顶高100--40000米!"));
        QString str;
        str = QStringLiteral("%1").arg(40000);
        ui->lineEdit->setText(str);
        ui->lineEdit->setFocus();
        return false;
    }

    if (zone.both < 100 || zone.both >= 40000)
    {
        LyMessageBox::warning(this, tr2("警告"), tr2("区域底高100--40000米!"));
        QString str;
        str = QStringLiteral("%1").arg(100);
        ui->lineEdit_2->setText(str);
        ui->lineEdit_2->setFocus();
        return false;
    }

    if ((zone.both + 50) > zone.toph)
    {
        LyMessageBox::warning(this, tr2("警告"), tr2("区域顶高必须大于底高一定程度."));
        QString str;
        str = QStringLiteral("%1").arg(50 + (int)(zone.both));
        ui->lineEdit->setText(str);
        ui->lineEdit->setFocus();
        return false;
    }

    return true;
}

void CQYZoneDlg::InsertDotList(int index, const QString &str, const QString &strJD, const QString &strWD)
{
    ui->tableWidget->insertRow(index);

    QTableWidgetItem *item1 = new QTableWidgetItem;
    item1->setText(str);
    ui->tableWidget->setItem(index, 0, item1);

    QTableWidgetItem *item2 = new QTableWidgetItem;
    item2->setText(strJD);
    ui->tableWidget->setItem(index, 1, item2);

    QTableWidgetItem *item3 = new QTableWidgetItem;
    item3->setText(strWD);
    ui->tableWidget->setItem(index, 2, item3);

    if (str == tr2("进") || str == tr2("退"))
    {
        return;
    }

    if (m_iQYZoneType == POLY || m_iQYZoneType == LINE)
    {
        QTableWidgetItem *item31 = new QTableWidgetItem;
        ui->tableWidget->setItem(index, 3, item31);

        if (index != 0)
        {
            double jd = ZoneManager::Get().StrToJWD(ui->tableWidget->item(index - 1, 1)->text());
            double wd = ZoneManager::Get().StrToJWD(ui->tableWidget->item(index - 1, 2)->text());
            double jd1 = ZoneManager::Get().StrToJWD(strJD);
            double wd1 = ZoneManager::Get().StrToJWD(strWD);

            QString strRtnDis = ZoneManager::Get().RtnDir_and_Dis_str(jd, wd, jd1, wd1);
            ui->tableWidget->item(index - 1, 3)->setText(strRtnDis);
        }
    }

    if (m_iQYZoneType == CIRCLE || m_iQYZoneType == PIE || (m_iQYZoneType == RECTANGLE && index == 0))
    {
        QTableWidgetItem *item4 = new QTableWidgetItem;
        item4->setText("30");
        ui->tableWidget->setItem(index, 3, item4);
    }
    if (m_iQYZoneType == PIE)
    {
        QTableWidgetItem *item5 = new QTableWidgetItem;
        item5->setText("0");
        ui->tableWidget->setItem(index, 4, item5);

        QTableWidgetItem *item6 = new QTableWidgetItem;
        item6->setText("120");
        ui->tableWidget->setItem(index, 5, item6);
    }
    else if (m_iQYZoneType == ELLIPSE)
    {
        QTableWidgetItem *item7 = new QTableWidgetItem;
        item7->setText("50");
        ui->tableWidget->setItem(index, 3, item7);

        QTableWidgetItem *item8 = new QTableWidgetItem;
        item8->setText("25");
        ui->tableWidget->setItem(index, 4, item8);

        QTableWidgetItem *item9 = new QTableWidgetItem;
        item9->setText("37");
        ui->tableWidget->setItem(index, 5, item9);
    }
    else if (m_iQYZoneType == CIRCLERING)
    {
        QTableWidgetItem *item10 = new QTableWidgetItem;
        item10->setText("50");
        ui->tableWidget->setItem(index, 3, item10);

        QTableWidgetItem *item11 = new QTableWidgetItem;
        item11->setText("25");
        ui->tableWidget->setItem(index, 4, item11);
    }
    else if (m_iQYZoneType == PIERING)
    {
        QTableWidgetItem *item12 = new QTableWidgetItem;
        item12->setText("80");
        ui->tableWidget->setItem(index, 3, item12);

        QTableWidgetItem *item13 = new QTableWidgetItem;
        item13->setText("65");
        ui->tableWidget->setItem(index, 4, item13);

        QTableWidgetItem *item14 = new QTableWidgetItem;
        item14->setText("0");
        ui->tableWidget->setItem(index, 5, item14);

        QTableWidgetItem *item15 = new QTableWidgetItem;
        item15->setText("120");
        ui->tableWidget->setItem(index, 6, item15);
    }
    else if (m_iQYZoneType == RECTANGLE)
    {
        QTableWidgetItem *item16 = new QTableWidgetItem;
        item16->setText("50");
        ui->tableWidget->setItem(index, 4, item16);

        QTableWidgetItem *item17 = new QTableWidgetItem;
        item17->setText("0");
        ui->tableWidget->setItem(index, 5, item17);

        QTableWidgetItem *item18 = new QTableWidgetItem;
        item18->setText("0");
        ui->tableWidget->setItem(index, 6, item18);
    }
}

void CQYZoneDlg::DrawQYZoneData(QPainter *painter)
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);

    for (int i = 0; i < ui->treeWidget->m_ZoneArray.size(); i++)
    {
        QUYU_Zone &zone = ui->treeWidget->m_ZoneArray[i];
        if (zone.show[0] == 0)
        {
            continue;
        }

        if (m_QYZoneTree->m_CurInd == i)
        {
            if (m_DwFlashFlag != 0)
            {
                m_DwFlashFlag++;
            }
            if (m_DwFlashFlag != 0 && m_DwFlashFlag % 2 != 0)
            {
                continue;
            }
            if (m_DwFlashFlag >= 6)
            {
                m_DwFlashFlag = 0;
            }
            if (m_bCreateFlag)
            {
                DrawLyJLPrompt(painter, zone);
            }
        }

        QPen m_penCur;
		m_penCur.setStyle((Qt::PenStyle)zone.xianxing);
		m_penCur.setWidth(2);

        unsigned int uiV = zone.kycolor;
        QColor color = QColor(uiV & 0xFF, (uiV >> 8) & 0xFF, (uiV >> 16) & 0xFF);
        color.setAlpha(100);
        m_penCur.setColor(color);

        QBrush brush;
        brush.setStyle((Qt::BrushStyle)zone.tianchong);
        brush.setColor(color);
        painter->setBrush(brush);
        painter->setFont(QFont("MS Serif", 11, QFont::Bold));
        painter->setPen(m_penCur);

        bool modifyFlg = false;
        if (m_QYZoneTree->m_CurInd == i)
        {
            modifyFlg = true;
        }

        DrawOneQuYu(painter, zone, modifyFlg);
    }

    painter->restore();
}

void CQYZoneDlg::DrawOneQuYu(QPainter *painter, QUYU_Zone &zone, bool bModify )
{
    double x, y;
    QString str, jwdstr;
    float  l, b;
    double centerX = 0, centerY = 0;
    QVector<QPointF> m_spoints;
    m_spoints.clear();
    for (int j = 0; j < zone.data.n; j++)
    {
        ZoneManager::Get().L_X(zone.data.h[j].x, zone.data.h[j].y, &x, &y);
        ZoneManager::Get().X_S(x, y, &l, &b);

        if (zone.type == POLY || zone.type == LINE)
        {
            str = QStringLiteral("%1").arg(j + 1);
            int jj, ww;
            jj = zone.data.h[j].x * 3600;
            ww = zone.data.h[j].y * 3600;
            jwdstr = QStringLiteral("%1.%2'%3\"  %4.%5'%6\"").arg((int)jj / 3600, 3, 10, QChar('0'))
                     .arg((int)jj % 3600 / 60, 2, 10, QChar('0'))
                     .arg((int)jj % 3600 % 60, 2, 10, QChar('0'))
                     .arg((int)ww / 3600, 2, 10, QChar('0'))
                     .arg((int)ww % 3600 / 60, 2, 10, QChar('0'))
                     .arg((int)ww % 3600 % 60, 2, 10, QChar('0'));
            if (bModify && !ui->pushButton_modify->isEnabled())
            {
                if (ZoneManager::Get().m_nModifyZone == 2)
                {
                    painter->drawText(QPoint((int)l + 5, (int)b + 5), jwdstr);
                }
                painter->drawText(QPoint((int)l, (int)b), str);
                DrawFG(painter, l - 3, b - 3, l + 3, b + 3);
            }

            m_spoints.push_back(QPointF(l, b));
        }
        if (zone.type == CIRCLE)
        {
            DrawKongYuCircle(painter, x, y, zone.data.h[2].x * 1000, zone.data.h[1].x, zone.data.h[1].y, zone.type, zone.tianchong);
        }
        if (zone.type == PIE)
        {
            QPainterPath path;
            DrawKongYuPie(painter, x, y, zone.data.h[2].x * 1000, zone.data.h[1].x, zone.data.h[1].y, zone.type, zone.tianchong, path);
            painter->drawPath(path);
        }
        if (zone.type == ELLIPSE)
        {
            DrawQYEllipse(painter, x, y, zone.data.h[2].x * 1000, zone.data.h[2].y * 1000, zone.data.h[1].x, zone.tianchong);
        }
        if (zone.type == RECTANGLE)
        {
            if (bModify && !ui->pushButton_modify->isEnabled())
            {
                DrawKongYuRect(painter, &zone, x, y, 1);
            }
            else
            {
                DrawKongYuRect(painter, &zone, x, y);
            }

        }
        if (zone.type == PIERING)
        {
            DrawArcRing(painter, x, y, zone);
        }
        if (zone.type == CIRCLERING)
        {
            DrawCircleRing(painter, x, y, zone);
        }

        centerX = centerX + l;
        centerY = centerY + b;
    }
    if (zone.type == POLY)
    {
        painter->drawPolygon(m_spoints);
    }
    if (zone.type == LINE)
    {
        painter->drawPolyline(m_spoints);
    }

    if (zone.type != RECTANGLE && zone.data.n > 0)
    {
        centerX = centerX / zone.data.n;
        centerY = centerY / zone.data.n;
    }
    else
    {
        ZoneManager::Get().L_X(zone.data.h[0].x, zone.data.h[0].y, &x, &y);
        ZoneManager::Get().X_S(x, y, &l, &b);
        centerX = l;
        centerY = b;
    }
    if (zone.show[1])
    {
        DrawZhuYuan(painter, zone, centerX, centerY);
    }
    if (zone.show[2])
    {
        painter->save();
        QPen pen;
        pen.setColor(Qt::red);
        painter->setPen(pen);
        char nm[MAX_ky_name], fold[MAX_ky_name];
        m_QYZoneTree->GetFolder_Name(zone.name, fold, nm);
        painter->drawText((int)centerX - 20, (int)centerY - 10, tr2(nm));
        painter->restore();
    }
    DrawFG(painter, centerX - 3, centerY - 3, centerX + 3, centerY + 3);
}

void CQYZoneDlg::DingWeiqy(const QUYU_Zone &zone)
{
    m_DwFlashFlag = 1;
    int width = 0;
    int height = 0;
    double zbx, zby;
    ZoneManager::Get().m_pProj->GetCurDevSize(width, height);
    int i = 0;
    for (i = 0; i < zone.data.n && i < MAX_ky_point; i++)
    {
        float sx, sy;
        ZoneManager::Get().L_X(zone.data.h[i].x, zone.data.h[i].y, &zbx, &zby);
        ZoneManager::Get().X_S(zbx, zby, &sx, &sy);
        if (sx > -0.00001 && sx < width + 0.0001 && sy > -0.00001 && sy < height + 0.00001)
        {
            break;
        }
    }
    if (i == zone.data.n)
    {
        char buf[64];
        sprintf(buf, "CenterAt(%f,%f)", zone.data.h[0].x, zone.data.h[0].y);
        ZoneManagerInterface::Get().Script(buf);
        ZoneManagerInterface::Get().Refresh(0);
    }
}

bool CQYZoneDlg::On2d3dChanged(int wParam)
{
	typedef std::vector<IUserMesh*> VUM;
	typedef std::map<std::string, VUM> MapSVUM;
	static MapSVUM smSVUM;
	CCreateShape cs;

	IEarth *pEarth = IEarth::GetEarth();
	if (pEarth->IsVisible())
	{
		IGraphics *pGrp = pEarth->GetGraphics();
		for (int i = 0; i < ui->treeWidget->m_ZoneArray.size(); i++)
		{
			QUYU_Zone &zone = ui->treeWidget->m_ZoneArray[i];
			std::string s = zone.name;
			MapSVUM::iterator it = smSVUM.find(s);
			if (zone.show[0] == 0)
			{
				continue;
			}
			if (it == smSVUM.end())
			{
				smSVUM[s];
				it = smSVUM.find(s);
			}
			((Qt::PenStyle)zone.xianxing);
			unsigned int uiV = zone.kycolor;
			unsigned int color1 = RGBA(uiV & 0xFF, (uiV >> 8) & 0xFF, (uiV >> 16) & 0xFF, 32);
			unsigned int color2 = RGBA(uiV & 0xFF, (uiV >> 8) & 0xFF, (uiV >> 16) & 0xFF, 32);
			((Qt::BrushStyle)zone.tianchong);

			bool modifyFlg = false;
			if (m_QYZoneTree->m_CurInd == i)
			{
				modifyFlg = true;
			}

			float  l, b;
			double centerX = 0, centerY = 0;
			double x, y;
			double dZ = zone.both;
			double dM = zone.toph - zone.both;
			SHPObject *pShp;
			QString str, jwdstr;
			switch (zone.type)
			{
			case CIRCLE:
			{
				ZoneManager::Get().L_X(zone.data.h[0].x, zone.data.h[0].y, &x, &y);
				pShp = cs.CreateCircle(x, y, zone.data.h[2].x * 1000);
				std::vector<double> vX(pShp->nVertices);
				std::vector<double> vY(pShp->nVertices);
				for (int k = 0; k < pShp->nVertices; k++)
				{
					int ii = pShp->nVertices - k - 1;
					ZoneManager::Get().X_L(pShp->padfX[k], pShp->padfY[k], &vX[ii], &vY[ii]);
				}
				int nVertices = pShp->nVertices;
				SHPDestroyObject(pShp);
				pShp = SHPCreateSimpleObject(SHPT_POLYGON, nVertices, vX.data(), vY.data(), NULL);
				SHPObject *pShp1 = cs.AddZM2SHPObject(true, pShp, &dZ, &dM);
				IUserMesh1 *pUM = pGrp->CreateUserMesh1(pShp1);
				pUM->SetColor(color1);
				it->second.push_back(pUM);
				pUM = pGrp->CreateUserMesh1(pShp1);
				pUM->SetColor(color2);
				pUM->SetState(IUserMesh::POLYGONMODE, IUserMesh::USERMESH_LINE);
				it->second.push_back(pUM);
				SHPDestroyObject(pShp1);
			}
			break;
			case PIE:
			{
				ZoneManager::Get().L_X(zone.data.h[0].x, zone.data.h[0].y, &x, &y);
				pShp = cs.CreatePie(x, y, zone.data.h[2].x * 1000, fmod(450-zone.data.h[1].y,360), fmod(450-zone.data.h[1].x,360), 100, zone.tianchong);
				for (int k = 0; k < pShp->nVertices; k++)
				{
					ZoneManager::Get().X_L(pShp->padfX[k], pShp->padfY[k], &pShp->padfX[k], &pShp->padfY[k]);
				}
				SHPObject *pShp1 = cs.AddZM2SHPObject(true, pShp, &dZ, &dM);
				IUserMesh1 *pUM = pGrp->CreateUserMesh1(pShp1);
				pUM->SetColor(color1);
				it->second.push_back(pUM);
				pUM = pGrp->CreateUserMesh1(pShp1);
				pUM->SetColor(color2);
				pUM->SetState(IUserMesh::POLYGONMODE, IUserMesh::USERMESH_LINE);
				it->second.push_back(pUM);
				SHPDestroyObject(pShp1);
			}
			break;
			case ELLIPSE:
			{
				pShp = cs.CreateEllipse(0, 0, zone.data.h[2].x * 1000, zone.data.h[2].y * 1000, 100, zone.tianchong);
				SHPObject *pShp1 = cs.AddZM2SHPObject(true, pShp, &dZ, &dM);
				IUserMesh2 *pUM = pGrp->CreateUserMesh2(pShp1);
				pUM->SetColor(color1);
				pUM->SetPosition(90 - zone.data.h[0].x, zone.data.h[0].y, dZ);
				pUM->SetYawPitchRoll(zone.data.h[1].x, 0, 0);
				it->second.push_back(pUM);
				pUM = pGrp->CreateUserMesh2(pShp1);
				pUM->SetColor(color2);
				pUM->SetState(IUserMesh::POLYGONMODE, IUserMesh::USERMESH_LINE);
				pUM->SetPosition(90 - zone.data.h[0].x, zone.data.h[0].y, dZ);
				pUM->SetYawPitchRoll(zone.data.h[1].x, 0, 0);
				it->second.push_back(pUM);
				SHPDestroyObject(pShp1);
			}
			break;
			case RECTANGLE:
			{
				double dX[5], dY[5];
				double x1 = zone.data.h[1].y * 500;
				double y1 = zone.data.h[1].x * 500;
				double x2 = zone.data.h[1].y * 1000;
				double y2 = zone.data.h[1].x * 1000;
				if (zone.data.h[2].y == 0)
				{
					dX[4] = -x1;
					dY[4] = -y1;
					dX[0] = -x1;
					dY[0] = -y1;
					dX[1] = x1;
					dY[1] = -y1;
					dX[2] = x1;
					dY[2] = y1;
					dX[3] = -x1;
					dY[3] = y1;
				}
				else
				{
					dX[4] = 0;
					dY[4] = -y1;
					dX[0] = 0;
					dY[0] = -y1;
					dX[1] = x2;
					dY[1] = -y1;
					dX[2] = x2;
					dY[2] = y1;
					dX[3] = 0;
					dY[3] = y1;
				}
				pShp = cs.CreatePolygon(dX, dY, 5);
				SHPObject *pShp1 = cs.AddZM2SHPObject(true, pShp, &dZ, &dM);
				IUserMesh2 *pUM = pGrp->CreateUserMesh2(pShp1);
				pUM->SetColor(color1);
				pUM->SetPosition(zone.data.h[0].x, zone.data.h[0].y, dZ);
				pUM->SetYawPitchRoll(zone.data.h[2].x, 0, 0);
				it->second.push_back(pUM);
				pUM = pGrp->CreateUserMesh2(pShp1);
				pUM->SetColor(color2);
				pUM->SetState(IUserMesh::POLYGONMODE, IUserMesh::USERMESH_LINE);
				pUM->SetPosition(zone.data.h[0].x, zone.data.h[0].y, dZ);
				pUM->SetYawPitchRoll(zone.data.h[2].x, 0, 0);
				it->second.push_back(pUM);
				SHPDestroyObject(pShp1);
			}
			break;
			case PIERING:
			{
				CShapeClip sc;
				sc.SetOperateType(SHAPE_DIFF);

				SHPObject *pShp1 = cs.CreatePie(0, 0, zone.data.h[2].x * 1000, fmod(450 - zone.data.h[1].y, 360), fmod(450 - zone.data.h[1].x, 360));
				SHPObject *pShp2 = cs.CreatePie(0, 0, zone.data.h[2].y * 1000, fmod(450 - zone.data.h[1].y, 360), fmod(450 - zone.data.h[1].x, 360));
				SHPObject *pShp3 = sc.Clip(pShp1, pShp2);
				SHPObject *pShp4 = cs.AddZM2SHPObject(true, pShp3, &dZ, &dM);
				IUserMesh2 *pUM = pGrp->CreateUserMesh2(pShp4);
				pUM->SetColor(color1);
				pUM->SetPosition(zone.data.h[0].x, zone.data.h[0].y, dZ);
				it->second.push_back(pUM);
				pUM = pGrp->CreateUserMesh2(pShp4);
				pUM->SetColor(color2);
				pUM->SetPosition(zone.data.h[0].x, zone.data.h[0].y, dZ);
				pUM->SetState(IUserMesh::POLYGONMODE, IUserMesh::USERMESH_LINE);
				it->second.push_back(pUM);
				SHPDestroyObject(pShp1);
				SHPDestroyObject(pShp2);
				SHPDestroyObject(pShp4);
			}
			break;
			case CIRCLERING:
			{
				CShapeClip sc;
				sc.SetOperateType(SHAPE_DIFF);
				SHPObject *pShp1 = cs.CreateCircle(0, 0, zone.data.h[2].x * 1000);
				SHPObject *pShp2 = cs.CreateCircle(0, 0, zone.data.h[2].y * 1000);
				SHPObject *pShp3 = sc.Clip(pShp1, pShp2);
				SHPObject *pShp4 = cs.AddZM2SHPObject(true, pShp3, &dZ, &dM);
				IUserMesh2 *pUM = pGrp->CreateUserMesh2(pShp4);
				pUM->SetColor(color1);
				pUM->SetPosition(zone.data.h[0].x, zone.data.h[0].y, dZ);
				it->second.push_back(pUM);
				pUM = pGrp->CreateUserMesh2(pShp4);
				pUM->SetColor(color2);
				pUM->SetPosition(zone.data.h[0].x, zone.data.h[0].y, dZ);
				pUM->SetState(IUserMesh::POLYGONMODE, IUserMesh::USERMESH_LINE);
				it->second.push_back(pUM);
				SHPDestroyObject(pShp1);
				SHPDestroyObject(pShp2);
				SHPDestroyObject(pShp4);
			}
			break;
			case POLY:
			{
				bool bConterClock = isconterclock(zone.data.n, (DPOINT*)zone.data.h);
				std::vector<double> vX;
				std::vector<double> vY;
				vX.reserve(zone.data.n);
				vY.reserve(zone.data.n);
				if (bConterClock)
				{
					for (int j = 0; j < zone.data.n; j++)
					{
						vX.push_back(zone.data.h[j].x);
						vY.push_back(zone.data.h[j].y);
					}
				}
				else
				{
					for (int j = zone.data.n - 1; j >= 0; j--)
					{
						vX.push_back(zone.data.h[j].x);
						vY.push_back(zone.data.h[j].y);
					}
				}

				pShp = SHPCreateSimpleObject(SHPT_POLYGON, zone.data.n,
					vX.data(), vY.data(), NULL);
				SHPObject *pShp1 = cs.AddZM2SHPObject(true, pShp, &dZ, &dM);

				IUserMesh1 *pUM = pGrp->CreateUserMesh1(pShp1);
				pUM->SetColor(color1);
				it->second.push_back(pUM);

				pUM = pGrp->CreateUserMesh1(pShp1);
				pUM->SetColor(color2);
				pUM->SetState(IUserMesh::POLYGONMODE, IUserMesh::USERMESH_LINE);
				it->second.push_back(pUM);
				SHPDestroyObject(pShp1);
			}
			break;
			case LINE:
			{
				std::vector<double> vX;
				std::vector<double> vY;
				for (int j = 0; j < zone.data.n; j++)
				{
					vX.push_back(zone.data.h[j].x);
					vY.push_back(zone.data.h[j].y);
				}
				pShp = SHPCreateSimpleObject(SHPT_ARC, zone.data.n,
					vX.data(), vY.data(), NULL);
				SHPObject *pShp1 = cs.AddZM2SHPObject(false, pShp, &dZ, &dM);
				IUserMesh1 *pUM = pGrp->CreateUserMesh1(pShp1);
				pUM->SetColor(color2);
				it->second.push_back(pUM);
				SHPDestroyObject(pShp1);
			}
			break;
			default:
				break;
			}

		}
	}
	return false;
}

void CQYZoneDlg::DrawFG(QPainter *pDC, int x1, int y1, int x2, int y2)
{
    pDC->save();
    QPen pen;
    pen.setStyle(Qt::NoPen);
    QColor color(Qt::green);
    color.setAlpha(130);
    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(color);
    pDC->setBrush(brush);
    pDC->setPen(pen);

    QRect rect(QPoint(x1, y1), QPoint(x2, y2));
    pDC->drawRect(rect);
    pDC->restore();
}

void CQYZoneDlg::DrawLyJLPrompt(QPainter *p, const QUYU_Zone &zone)
{
    int num = zone.data.n;
    if (num < 2)
    {
        return;
    }
    QString str = ZoneManager::Get().RtnDir_and_Dis_str(zone.data.h[num - 2].x, zone.data.h[num - 2].y, zone.data.h[num - 1].x, zone.data.h[num - 1].y, 1);
    int x, y;
    ZoneManager::Get().L_S(zone.data.h[num - 1].x, zone.data.h[num - 1].y, &x, &y);

    p->save();
    QPen pen;
    pen.setColor(Qt::red);
    p->setPen(pen);
    p->drawText(x + 20, y - 10, str);
    p->restore();
}

void CQYZoneDlg::DrawKongYuCircle(QPainter *painter, double x, double y, float radius, float startangle, float endangle, int type, int filltype)
{
    double yx, yy;
    yx = x + radius * sin(startangle * PI / 180.);
    yy = y + radius * cos(startangle * PI / 180.);

    float l, b, ll, bb;
    ZoneManager::Get().X_S(x, y, &l, &b);
    ZoneManager::Get().X_S(yx, yy, &ll, &bb);

    double m_fRadius = sqrt(float((ll - l) * (ll - l) + (bb - b) * (bb - b)));

    painter->drawEllipse(QPointF(l, b), m_fRadius, m_fRadius);
}

void CQYZoneDlg::DrawKongYuPie(QPainter *painter, double x, double y, float radius, float startangle, float endangle, int type, int filltype, QPainterPath &path)
{
    double yx, yy, xx, xy;
    yx = x + radius * sin(startangle * PI / 180.);
    yy = y + radius * cos(startangle * PI / 180.);
    xx = x + radius * sin(endangle * PI / 180.);
    xy = y + radius * cos(endangle * PI / 180.);

    float l, b, ll, bb, lll, bbb;
    ZoneManager::Get().X_S(x, y, &l, &b);
    ZoneManager::Get().X_S(yx, yy, &ll, &bb);
    ZoneManager::Get().X_S(xx, xy, &lll, &bbb);

    double rd = sqrt(float((ll - l) * (ll - l) + (bb - b) * (bb - b)));

    QPointF pcenter(l, b);
    QPointF m_StartPoint(ll, bb);
    QPointF m_EndPoint(lll, bbb);

    double angleStart = atan2((double)(m_StartPoint.y() - pcenter.y()),
                              (double)(m_StartPoint.x() - pcenter.x())) * 180.0 / 3.1415926;
    double angleEnd = atan2((double)(m_EndPoint.y() - pcenter.y()),
                            (double)(m_EndPoint.x() - pcenter.x())) * 180.0 / 3.1415926;

    angleStart = abs(angleStart);
    angleEnd = abs(angleEnd);

    if (m_StartPoint.y() - pcenter.y() >= 0 && m_StartPoint.x() - pcenter.x() > 0)
    {
        angleStart = -angleStart;
    };
    if (m_StartPoint.y() - pcenter.y() > 0 && m_StartPoint.x() - pcenter.x() <= 0)
    {
        angleStart = -angleStart;
    }
    if (m_StartPoint.y() - pcenter.y() <= 0 && m_StartPoint.x() - pcenter.x() < 0)
    {
        angleStart = angleStart;
    }
    if (m_StartPoint.y() - pcenter.y() < 0 && m_StartPoint.x() - pcenter.x() >= 0)
    {
        angleStart = angleStart;
    }

    if (m_EndPoint.y() - pcenter.y() >= 0 && m_EndPoint.x() - pcenter.x() > 0)
    {
        angleEnd = -angleEnd;
    }
    if (m_EndPoint.y() - pcenter.y() > 0 && m_EndPoint.x() - pcenter.x() <= 0)
    {
        angleEnd = -angleEnd;
    }
    if (m_EndPoint.y() - pcenter.y() <= 0 && m_EndPoint.x() - pcenter.x() < 0)
    {
        angleEnd = angleEnd;
    }
    if (m_EndPoint.y() - pcenter.y() < 0 && m_EndPoint.x() - pcenter.x() >= 0)
    {
        angleEnd = angleEnd;
    }

    double sweepAngle = angleEnd - angleStart;

    if (sweepAngle > 0)
    {
        sweepAngle = 360 - sweepAngle;
    }
    else
    {
        sweepAngle = 0 - sweepAngle;
    }

    QRectF rectangle(QPointF(pcenter.x() - rd, pcenter.y() - rd), QPointF(pcenter.x() + rd, pcenter.y() + rd));

    path.moveTo(pcenter);
    path.arcTo(rectangle, angleStart, (0 - sweepAngle));

}

void CQYZoneDlg::calcPoint(double x1, double y1, double l, double h, double angle, double type, double &centerx, double &centery, QVector<QPointF> &m_mPoints)
{
    m_mPoints.clear();
    double dis;
    double j1, j2, j3, j4;
    double dx1, dy1, dx2, dy2, dx3, dy3, dx4, dy4;
    if (type == 0)
    {
        dis = sqrt(l * l + h * h) * 1000 / 2;
        if (ZoneManager::Get().m_pProj != nullptr)
        {
            j1 = angle - atan(h / l) * 180 / PI;
            ZoneManager::Get().m_pProj->A_X(x1, y1, dis, j1, dx1, dy1);

            j2 = angle + atan(h / l) * 180 / PI;
            ZoneManager::Get().m_pProj->A_X(x1, y1, dis, j2, dx2, dy2);

            j3 = j1 + 180;
            ZoneManager::Get().m_pProj->A_X(x1, y1, dis, j3, dx3, dy3);

            j4 = j2 + 180;
            ZoneManager::Get().m_pProj->A_X(x1, y1, dis, j4, dx4, dy4);

            int px, py;
            ZoneManager::Get().m_pProj->xyLtoD(x1, y1, &px, &py);
            centerx = (double)px;
            centery = (double)py;
        }
    }

    else
    {
        if (ZoneManager::Get().m_pProj != nullptr)
        {
            dis = h * 1000 / 2;
            j1 = angle - 90;
            ZoneManager::Get().m_pProj->A_X(x1, y1, dis, j1, dx1, dy1);
            j4 = angle + 90;
            ZoneManager::Get().m_pProj->A_X(x1, y1, dis, j4, dx4, dy4);

            dis = l * 1000;
            j2 = j3 = angle;
            ZoneManager::Get().m_pProj->A_X(dx1, dy1, dis, j2, dx2, dy2);
            ZoneManager::Get().m_pProj->A_X(dx4, dy4, dis, j3, dx3, dy3);

            double dx5, dy5;
            int px, py;
            ZoneManager::Get().m_pProj->A_X(x1, y1, dis / 2, angle, dx5, dy5);
            ZoneManager::Get().m_pProj->xyLtoD(dx5, dy5, &px, &py);
            centerx = (double)px;
            centery = (double)py;
        }
    }
    float ll, bb;
    ZoneManager::Get().X_S(dx1, dy1, &ll, &bb);
    m_mPoints.push_back(QPointF(ll, bb));

    ZoneManager::Get().X_S(dx2, dy2, &ll, &bb);
    m_mPoints.push_back(QPointF(ll, bb));

    ZoneManager::Get().X_S(dx3, dy3, &ll, &bb);
    m_mPoints.push_back(QPointF(ll, bb));

    ZoneManager::Get().X_S(dx4, dy4, &ll, &bb);
    m_mPoints.push_back(QPointF(ll, bb));
}

void CQYZoneDlg::DrawKongYuRect(QPainter *painter, QUYU_Zone *pZone, double x1, double y1, bool flag )
{
    double centerX = 0, centerY = 0;
    QVector<QPointF> m_mPoints;
    calcPoint(x1, y1, pZone->data.h[1].x, pZone->data.h[1].y, pZone->data.h[2].x, pZone->data.h[2].y, centerX, centerY, m_mPoints);

    painter->drawPolygon(m_mPoints);
}

void CQYZoneDlg::DrawQYEllipse(QPainter *painter, double x, double y, float fLRadius, float fSRadius, float fRotateAngle, int filltype)
{
    painter->save();

    float l, b, ll, bb;
    ZoneManager::Get().X_S(x, y, &l, &b);

    ll = ZoneManager::Get().m_pProj->Map2ScreenDistance(fLRadius);
    bb = ZoneManager::Get().m_pProj->Map2ScreenDistance(fSRadius);

    painter->translate(l, b);
    painter->rotate(fRotateAngle - 90);
    painter->drawEllipse(QPointF(0, 0), ll, bb);
    painter->restore();
}

void CQYZoneDlg::DrawCircleRing(QPainter *painter, double x, double y, QUYU_Zone &zone)
{
    painter->save();

    double yx, yy, dx, dy;
    yx = x + zone.data.h[2].x * 1000 * sin(0 * PI / 180.);
    yy = y + zone.data.h[2].x * 1000 * cos(0 * PI / 180.);

    dx = x + zone.data.h[2].y * 1000 * sin(0 * PI / 180.);
    dy = y + zone.data.h[2].y * 1000 * cos(0 * PI / 180.);

    float l, b, ll, bb, lll, bbb;
    ZoneManager::Get().X_S(x, y, &l, &b);
    ZoneManager::Get().X_S(yx, yy, &ll, &bb);
    ZoneManager::Get().X_S(dx, dy, &lll, &bbb);

    double m_OutRadius = sqrt(float((ll - l) * (ll - l) + (bb - b) * (bb - b)));
    double m_inRadius = sqrt(float((lll - l) * (lll - l) + (bbb - b) * (bbb - b)));

    QPainterPath path;
    path.addEllipse(QPointF(l, b), m_OutRadius, m_OutRadius);
    path.addEllipse(QPointF(l, b), m_inRadius, m_inRadius);
    path.setFillRule(Qt::OddEvenFill);

    painter->drawPath(path);
    painter->restore();
}

void CQYZoneDlg::DrawArcRing(QPainter *painter, float x, float y, QUYU_Zone &zone)
{
    double yx, yy, xx, xy;
    yx = x + zone.data.h[2].x * 1000 * sin(zone.data.h[1].x * PI / 180.);
    yy = y + zone.data.h[2].x * 1000 * cos(zone.data.h[1].x * PI / 180.);
    xx = x + zone.data.h[2].x * 1000 * sin(zone.data.h[1].y * PI / 180.);
    xy = y + zone.data.h[2].x * 1000 * cos(zone.data.h[1].y * PI / 180.);

    float l, b, ll, bb, lll, bbb;
    ZoneManager::Get().X_S(x, y, &l, &b);
    ZoneManager::Get().X_S(yx, yy, &ll, &bb);
    ZoneManager::Get().X_S(xx, xy, &lll, &bbb);

    double rd = sqrt(float((ll - l) * (ll - l) + (bb - b) * (bb - b)));

    double dx, dy;
    dx = x + zone.data.h[2].y * 1000 * sin(zone.data.h[1].x * PI / 180.);
    dy = y + zone.data.h[2].y * 1000 * cos(zone.data.h[1].x * PI / 180.);

    float ldx, ldy;
    ZoneManager::Get().X_S(dx, dy, &ldx, &ldy);
    double inrd = sqrt(float((ldy - l) * (ldx - l) + (ldy - b) * (ldy - b)));

    QPointF pcenter(l, b);
    QPointF m_StartPoint(ll, bb);
    QPointF m_EndPoint(lll, bbb);

    double angleStart = atan2((double)(m_StartPoint.y() - pcenter.y()),
                              (double)(m_StartPoint.x() - pcenter.x())) * 180.0 / 3.1415926;
    double angleEnd = atan2((double)(m_EndPoint.y() - pcenter.y()),
                            (double)(m_EndPoint.x() - pcenter.x())) * 180.0 / 3.1415926;

    angleStart = abs(angleStart);
    angleEnd = abs(angleEnd);

    if (m_StartPoint.y() - pcenter.y() >= 0 && m_StartPoint.x() - pcenter.x() > 0)
    {
        angleStart = -angleStart;
    };
    if (m_StartPoint.y() - pcenter.y() > 0 && m_StartPoint.x() - pcenter.x() <= 0)
    {
        angleStart = -angleStart;
    }
    if (m_StartPoint.y() - pcenter.y() <= 0 && m_StartPoint.x() - pcenter.x() < 0)
    {
        angleStart = angleStart;
    }
    if (m_StartPoint.y() - pcenter.y() < 0 && m_StartPoint.x() - pcenter.x() >= 0)
    {
        angleStart = angleStart;
    }

    if (m_EndPoint.y() - pcenter.y() >= 0 && m_EndPoint.x() - pcenter.x() > 0)
    {
        angleEnd = -angleEnd;
    }
    if (m_EndPoint.y() - pcenter.y() > 0 && m_EndPoint.x() - pcenter.x() <= 0)
    {
        angleEnd = -angleEnd;
    }
    if (m_EndPoint.y() - pcenter.y() <= 0 && m_EndPoint.x() - pcenter.x() < 0)
    {
        angleEnd = angleEnd;
    }
    if (m_EndPoint.y() - pcenter.y() < 0 && m_EndPoint.x() - pcenter.x() >= 0)
    {
        angleEnd = angleEnd;
    }

    double sweepAngle = angleEnd - angleStart;

    if (sweepAngle > 0)
    {
        sweepAngle = 360 - sweepAngle;
    }
    else
    {
        sweepAngle = 0 - sweepAngle;
    }

    QRectF rectangle(QPointF(pcenter.x() - rd, pcenter.y() - rd), QPointF(pcenter.x() + rd, pcenter.y() + rd));
    QRectF rectangle1(QPointF(pcenter.x() - inrd, pcenter.y() - inrd), QPointF(pcenter.x() + inrd, pcenter.y() + inrd));

    painter->save();
    painter->setPen(Qt::NoPen);
    QPainterPath path;

    path.moveTo(pcenter);
    path.arcTo(rectangle, angleStart, (0 - sweepAngle));
    path.closeSubpath();

    path.moveTo(pcenter);
    path.arcTo(rectangle1, angleStart, (0 - sweepAngle));
    path.closeSubpath();

    path.setFillRule(Qt::OddEvenFill);
    painter->drawPath(path);

    painter->restore();
}

void CQYZoneDlg::DrawZhuYuan(QPainter *p, const QUYU_Zone &zone, double centerX, double centerY)
{
    p->save();
    QPen pen;
    pen.setColor(Qt::red);
    p->setPen(pen);

    QString str, strmj;
    if (zone.attr.type >= 0 && zone.attr.type <= 4)
    {
        str = ui->comboBox->itemText(zone.attr.type);
    }
    p->drawText(QPoint((int)centerX - 20, (int)centerY + 10), str);
    str = tr2("顶%1").arg(zone.toph, 0, 'f', 0);
    p->drawText(QPoint((int)centerX - 20, (int)centerY + 30), str);
    str = tr2("底%1").arg(zone.both, 0, 'f', 0);
    p->drawText(QPoint((int)centerX - 20, (int)centerY + 50), str);

    DPOINT  polygon[MAX_ky_point + 1];
    double x, y;
    if (zone.type == POLY)
    {
        for (int k = 0; k < zone.data.n; k++)
        {
            ZoneManager::Get().L_X(zone.data.h[k].x, zone.data.h[k].y, &x, &y);
            polygon[k].x = x;
            polygon[k].y = y;
        }
        polygon[zone.data.n] = polygon[0];
        double mj = area_of_polygon(zone.data.n + 1, polygon);
        double val = fabs(mj);
        float m = val / 1000000;
        strmj = tr2("面积:%1平方公里").arg(m, 0, 'f', 1);
        p->drawText(QPoint((int)centerX - 20, (int)centerY + 70), strmj);
    }
    if (zone.type == CIRCLE)
    {
        float val = PI * zone.data.h[2].x * zone.data.h[2].x;
        strmj = tr2("面积:%1平方公里").arg(val, 0, 'f', 1);
        p->drawText(QPoint((int)centerX - 20, (int)centerY + 90), strmj);
    }
    if (zone.type == PIE)
    {
        float jiajiao = zone.data.h[1].y - zone.data.h[1].x;
        if (jiajiao < 0)
        {
            jiajiao += 360;
        }
        float xs = jiajiao / 360.0;
        float val = PI * zone.data.h[2].x * zone.data.h[2].x * xs;
        strmj = tr2("面积:%1平方公里").arg(val, 0, 'f', 1);
        p->drawText(QPoint((int)centerX - 20, (int)centerY + 110), strmj);
    }

    if (zone.type == RECTANGLE)
    {
        float val = zone.data.h[1].x * zone.data.h[1].y;
        strmj = tr2("面积:%1平方公里").arg(val, 0, 'f', 1);
        p->drawText(QPoint((int)centerX - 20, (int)centerY + 130), strmj);
    }

    if (zone.type == CIRCLE || zone.type == PIE)
    {
        str = tr2("半径%1公里").arg(zone.data.h[2].x, 0, 'f', 0);
        p->drawText(QPoint((int)centerX - 20, (int)centerY + 70), str);
    }
    if (zone.type == RECTANGLE)
    {
        str = tr2("长%1公里").arg(zone.data.h[1].x);
        p->drawText(QPoint((int)centerX - 20, (int)centerY + 70), str);
        str = tr2("宽%1公里").arg(zone.data.h[1].y);
        p->drawText(QPoint((int)centerX - 20, (int)centerY + 90), str);
        str = tr2("方向%1度").arg((int)zone.data.h[2].x);
        p->drawText(QPoint((int)centerX - 20, (int)centerY + 110), str);
    }
    if (zone.type == PIE)
    {
        str = tr2("起始角%1,终止角%2").arg(zone.data.h[1].x).arg(zone.data.h[1].y);
        p->drawText(QPoint((int)centerX - 20, (int)centerY + 90), str);
    }
    p->restore();
}

void CQYZoneDlg::OnBnClickedBtnOutputParasToWord()
{
}
