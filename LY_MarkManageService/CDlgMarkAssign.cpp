#include "CDlgMarkAssign.h"
#include "ui_CDlgMarkAssign.h"
#include "QPainter"
#include "QCString.h"

#define WND_COLOR_BACKGROUND	QColor(27, 34, 44)
#define LINE_COLOR				QColor(54, 59, 62)

CDlgMarkAssign::CDlgMarkAssign(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CDlgMarkAssign)
{
    ui->setupUi(this);
    InitUAVList();

    connect(this, &CDlgMarkAssign::RecUpdateWindow, this, &CDlgMarkAssign::UpdateWindow, Qt::ConnectionType::QueuedConnection);

    ui->pushButton->setEnabled(false);
    ui->pushButton_3->setEnabled(false);
    ui->groupBox->setStyleSheet("border-radius:10px;border-width:0px;");
}

CDlgMarkAssign::~CDlgMarkAssign()
{
    delete ui;
}

void CDlgMarkAssign::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QMutexLocker locker(&sMutex);

    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(WND_COLOR_BACKGROUND);
    QPoint pt = ui->groupBox->pos();
    QRect recGroupBox = ui->groupBox->rect();
    recGroupBox.moveTo(pt);
    painter.drawRect(recGroupBox);
    painter.setClipRect(recGroupBox);

    if (m_UAVMap.size() == 0)
    {
        return;
    }
    int nOneHeight = recGroupBox.height() / (m_UAVMap.size() + 1);

    QPoint startPoint, finishPoint;
    QPen Linepen(LINE_COLOR);
    Linepen.setWidth(2);
    QFont f1;
    f1.setPixelSize(16);
    f1.setWeight(QFont::Bold);

    painter.setPen(Linepen);
    int nSpace = recGroupBox.width() / 100;
    int nNo = 1;
    for (auto UAVData : m_UAVMap)
    {

        startPoint.setX(recGroupBox.x() + nSpace);
        startPoint.setY(recGroupBox.y() + nNo * nOneHeight);
        finishPoint.setX(recGroupBox.right() - nSpace);
        finishPoint.setY(recGroupBox.y() + nNo * nOneHeight);
        painter.setPen(Linepen);
        painter.drawLine(startPoint, finishPoint);

        painter.setFont(f1);
        painter.setPen(QColor(255, 255, 255, 230));
        int nWidthHalf = (finishPoint.x() - startPoint.x()) / 2;
        QRect rectText1, rectText2;
        rectText1.setX(startPoint.x());
        rectText1.setY(recGroupBox.y() + nNo * nOneHeight);
        rectText1.setWidth(nWidthHalf);
        rectText1.setHeight(nOneHeight);
        painter.drawText(rectText1, Qt::AlignLeft | Qt::AlignVCenter, QString::fromStdString(UAVData.second.getComponentName()));

        rectText2.setX(startPoint.x() + nWidthHalf);
        rectText2.setY(recGroupBox.y() + nNo * nOneHeight);
        rectText2.setWidth(nWidthHalf);
        rectText2.setHeight(nOneHeight);
        QString strMarkID = GetMarkID(UAVData.first);
        painter.drawText(rectText2, Qt::AlignLeft | Qt::AlignVCenter, strMarkID);

        if (nNo == 1)
        {
            rectText1.setX(startPoint.x());
            rectText1.setY(recGroupBox.y());
            rectText1.setWidth(finishPoint.x() - startPoint.x());
            rectText1.setHeight(nOneHeight);
            painter.drawText(rectText1, Qt::AlignLeft | Qt::AlignVCenter, tr("UAVName"));

            rectText2.setX(startPoint.x() + nWidthHalf);
            rectText2.setY(recGroupBox.y());
            rectText2.setWidth(nWidthHalf);
            rectText2.setHeight(nOneHeight);
            painter.drawText(rectText2, Qt::AlignLeft | Qt::AlignVCenter, tr("MarkID"));
        }
        nNo++;
    }

}

void CDlgMarkAssign::InitUAVList()
{
    auto *const pSystemMaintenanceService = IMarkManageServiceActivator::Get().getPSystemMaintenanceService();
    if (pSystemMaintenanceService == nullptr)
    {
        return;
    }

    if (!pSystemMaintenanceService->GetWeaponComponents_UAV(&m_UAVMap))
    {
        return;
    }
}

void CDlgMarkAssign::SetUAVMarkRelation(std::string &strUAVID, std::string &strMarkID)
{
    emit RecUpdateWindow(QString::fromStdString(strUAVID), QString::fromStdString(strMarkID));
}

QString CDlgMarkAssign::GetMarkID(std::string strUAVID)
{
    QString strID = "";
    auto itr = m_UAVMarkRelationMap.find(strUAVID);
    if (itr != m_UAVMarkRelationMap.end())
    {
        strID = QString::fromStdString(itr->second);
    }
    return strID;
}

void CDlgMarkAssign::UpdateWindow(QString strUAVID, QString strMarkID)
{
    QMutexLocker locker(&sMutex);

    auto itr = m_UAVMarkRelationMap.find(strUAVID.toStdString());
    if (itr == m_UAVMarkRelationMap.end())
    {
        m_UAVMarkRelationMap.insert(std::make_pair(strUAVID.toStdString(), strMarkID.toStdString()));
    }
    else
    {
        itr->second = strMarkID.toStdString();
    }
    this->update();

}

