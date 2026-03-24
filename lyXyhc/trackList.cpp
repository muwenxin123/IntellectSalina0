#include "trackList.h"
#include <QCString.h>
#include <QDateTime>
#include <QTableWidget>
#include <QHeaderView>
#include <QVBoxLayout>
#include "QTrack.h"
#include "lyXyhc.h"
#include "lyXyhcInterface.h"

inline int WhichList(unsigned char cIFF)
{
    switch (cIFF)
    {
    case 2:
    case 3:
        return 0;
    default:
        return 1;
    }
}

inline void RemoveTrack(QTableWidget *pTable, const QString &strZ, const QString &strP)
{
    if (pTable == nullptr)
    {
        return ;
    }

    for (int i = pTable->rowCount() - 1; i >= 0; i--)
    {
        if (pTable->item(i, 0)->text().compare(strZ) == 0
                && pTable->item(i, 1)->text().compare(strP) == 0)
        {
            pTable->removeRow(i);
            return;
        }
    }
}

inline void RemoveTrackByType(QTableWidget *pTable, const QString &strType)
{
    if (pTable == nullptr)
    {
        return ;
    }

    for (int i = pTable->rowCount() - 1; i >= 0; i--)
    {
        if (pTable->item(i, 0)->text().compare(strType) == 0)
        {
            pTable->removeRow(i);
        }
    }
}

QTableWidget *CreateTable(const QStringList &labels)
{
    QTableWidget *pTable = new QTableWidget(0, labels.size());
    pTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    pTable->setHorizontalHeaderLabels(labels);

    pTable->verticalHeader()->hide();
    pTable->setShowGrid(true);
    pTable->setColumnWidth(0, 44);
    pTable->setColumnWidth(1, 44);
    pTable->setColumnWidth(2, 64);
    pTable->setColumnWidth(3, 64);
    pTable->setColumnWidth(4, 64);
    pTable->setColumnWidth(5, 64);
    pTable->setColumnWidth(6, 64);
    pTable->setColumnWidth(7, 64);
    pTable->setColumnWidth(8, 52);
    pTable->setColumnWidth(9, 48);
    pTable->setColumnWidth(10, 52);
    pTable->setColumnWidth(11, 72);
    pTable->setColumnWidth(12, 90);
    pTable->setColumnWidth(13, 82);

    return pTable;
}

TrackList::TrackList()
{
    QStringList labels;
    labels << tr("station number") << tr("batch number") << tr("type") << tr("attribute") << tr("model")  << tr("task") << tr("time")
           << tr("longitude") << tr("latitude") << tr("state") << tr("height") << tr("formation") << tr("speed") << tr("course") ;

    m_pListW = CreateTable(labels);
    m_pListD = CreateTable(labels);
    m_pListX = CreateTable(labels);

    connect(m_pListW, &QTableWidget::cellDoubleClicked, this, &TrackList::OnCellDoubleClicked);
    connect(m_pListD, &QTableWidget::cellDoubleClicked, this, &TrackList::OnCellDoubleClicked);

    QVBoxLayout *loyout = new QVBoxLayout(this);
    loyout->setSpacing(0);
    loyout->setObjectName(QStringLiteral("verticalLayout"));
    loyout->setContentsMargins(0, 0, 0, 0);
    loyout->addWidget(m_pListW);
    loyout->addWidget(m_pListD);
    loyout->addWidget(m_pListX);
}

QTableWidget *TrackList::GetTableWidget(const QString &strZ, const QString &strP, const QTrack *pTrack)
{
    if (pTrack == nullptr)
    {
        return nullptr;
    }

    if (pTrack->m_status.XiaoShi)
    {
        RemoveTrack(m_pListW, strZ, strP);
        RemoveTrack(m_pListD, strZ, strP);
        return m_pListX;
    }

    switch (WhichList(pTrack->getIFF()))
    {
    case 0:
    {
        RemoveTrack(m_pListD, strZ, strP);
        RemoveTrack(m_pListX, strZ, strP);
        return m_pListW;
    }
    break;
    default:
    {
        RemoveTrack(m_pListW, strZ, strP);
        RemoveTrack(m_pListX, strZ, strP);
        return m_pListD;
    }
    break;
    }

    return nullptr;
}

void TrackList::OnCellDoubleClicked(int row, int column)
{
    QTableWidget *pTable = (QTableWidget *)sender();
    if (row < pTable->rowCount())
    {
        unsigned char ucZ = pTable->item(row, 0)->text().toInt();
        unsigned char ucP = pTable->item(row, 1)->text().toInt();
        QTrack *pTtk = LyXyhc::Get().getTrackMgr().FindTrack(ucZ, ucP);
        const TrackPt &tp = pTtk->m_lsPoints.back();
        if (&tp != NULL)
        {
            char szT[64];
            sprintf(szT, "CenterAt(%.8f,%.8f)", tp.dX, tp.dY);
            LyXyhcInterface::Get().Script(szT);
        }
    }
}

inline int GetRow(QTableWidget *pTable, const QString &strZ, const QString &strP)
{
    if (pTable == nullptr)
    {
        return -1;
    }

    for (int i = pTable->rowCount() - 1; i >= 0; i--)
    {
        if (pTable->item(i, 0)->text().compare(strZ) == 0
                && pTable->item(i, 1)->text().compare(strP) == 0)
        {
            return i;
        }
    }
    int nRow = pTable->rowCount();
    pTable->insertRow(nRow);
    QTableWidgetItem *pItem = new QTableWidgetItem(strZ);
    pItem->setFlags(pItem->flags() ^ Qt::ItemIsEditable);
    pTable->setItem(nRow, 0, pItem);
    pTable->setRowHeight(nRow, 15);
    return nRow;
}

#define UPDATE_ITEM(r, c, t)\
{\
    pItem = pTable->item(r, c);\
    if (pItem == nullptr)\
    {\
        pItem = new QTableWidgetItem(t);\
        pItem->setFlags(pItem->flags() ^ Qt::ItemIsEditable);\
        pTable->setItem(r, c, pItem);\
    }\
    else\
    {\
        pItem->setText(t);\
    }\
}

const QString &GetAttr(int i, unsigned int k)
{
    const QStringList &sl = LyXyhc::Get().m_vslAttributeItem[i];
    if (k > 0 && k <= sl.size())
    {
        return sl[k - 1];
    }
    static QString s;
    return s;
}

void TrackList::Update(int nID, const QTrack *pTrack)
{
    if (pTrack == nullptr)
    {
        return ;
    }

    char szBuf[128];
    const QString &strZ = QString::asprintf("%d", (nID >> 8) & 0xFF);
    const QString &strP = QString::asprintf("%d", nID & 0xFF);
    QTableWidget *pTable = GetTableWidget(strZ, strP, pTrack);
    if (pTable == nullptr)
    {
        return ;
    }
    int nRow = GetRow(pTable, strZ, strP);
    if (nRow < 0)
    {
        return ;
    }

    QTableWidgetItem *pItem;

    UPDATE_ITEM(nRow, 1, strP);
    UPDATE_ITEM(nRow, 2, GetAttr(0, pTrack->getType()));
    UPDATE_ITEM(nRow, 3, GetAttr(2, pTrack->getIFF()));
    UPDATE_ITEM(nRow, 4, GetAttr(1, pTrack->getModel()));
    UPDATE_ITEM(nRow, 5, GetAttr(4, pTrack->getTask()));

    TrackPtList::const_reverse_iterator it = pTrack->m_lsPoints.rbegin();
    if (it != pTrack->m_lsPoints.rend())
    {
        double dX, dY;
        LyXyhc::Get().GetProj()->xyjw(it->dX, it->dY, &dX, &dY);
        UPDATE_ITEM(nRow, 6, QDateTime::fromSecsSinceEpoch(it->nT).time().toString("HH:mm:ss"));
        int nTmp = dX * 360000 + 0.5;
        sprintf(szBuf, "%03d¡ã%02d¡ä%04d", nTmp / 360000, nTmp / 6000 % 60, nTmp % 6000);
        UPDATE_ITEM(nRow, 7, tr(szBuf));
        nTmp = dY * 360000 + 0.5;
        sprintf(szBuf, "%02d¡ã%02d¡ä%04d", nTmp / 360000, nTmp / 6000 % 60, nTmp % 6000);
        UPDATE_ITEM(nRow, 8, tr(szBuf));
    }
    UPDATE_ITEM(nRow, 9, GetAttr(3, pTrack->getState()));
    UPDATE_ITEM(nRow, 10, QString::asprintf("%.2f", pTrack->m_nGaoDu));
    UPDATE_ITEM(nRow, 11, GetAttr(5, pTrack->getFormation()));
    UPDATE_ITEM(nRow, 12, QString::asprintf("%.2f", pTrack->m_nSuDu));
    UPDATE_ITEM(nRow, 13, QString::asprintf("%.2f", pTrack->m_nHangXiang));
}

void TrackList::Remove(int nID)
{
    const QString &strZ = QString::asprintf("%d", (nID >> 8) & 0xFF);
    const QString &strP = QString::asprintf("%d", nID & 0xFF);
    RemoveTrack(m_pListD, strZ, strP);
    RemoveTrack(m_pListW, strZ, strP);
    RemoveTrack(m_pListX, strZ, strP);
}

void TrackList::RemoveByType(int nID)
{
    const QString &strType = QString::asprintf("%d", (nID >> 8) & 0xFF);
    RemoveTrackByType(m_pListD, strType);
    RemoveTrackByType(m_pListW, strType);
    RemoveTrackByType(m_pListX, strType);
}

