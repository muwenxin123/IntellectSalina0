#include "DrawVehicleIcon.h"
#include "ui_DrawVehicleIcon.h"
#include "QPen"
#include "QPainter"
#include "QCString.h"
#include <QMouseEvent>

#define SIZE_FONE_COMMTYPE			0x08
#define TYPE_FONT_COMMTYPE			L"Arial"
#define COLOR_TXT_COMMTYPE			QColor(255, 0, 255, 0)
#define COLOR_TXT_FLYID				QColor(255, 255, 255, 255)
#define COLOR_TXT_FLYID1			QColor(0,0,0)
#define OPER_MOVE_NONE				0
#define OPER_MOVE_VEHICLE			1
#define OPER_MOVE_VEHICLE_SELECTED	2

DrawVehicleIcon::DrawVehicleIcon(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DrawVehicleIcon)
{
    ui->setupUi(this);
    InitData();
}

DrawVehicleIcon::~DrawVehicleIcon()
{
    delete ui;
}

void DrawVehicleIcon::paintEvent(QPaintEvent *event)
{
    QPainter p(this);

    QRect	m_rectView = rect();
    QColor qcBack(11, 29, 47);

    QPen pen1(qcBack, 1, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin);
    p.setPen(pen1);
    QBrush brushStar(qcBack, Qt::SolidPattern);
    QColor qcBack1(0, 0, 0);
    p.setBrush(brushStar);
    p.drawRect(m_rectView);

    m_rectInfoView.setLeft(m_rectView.left() + m_rectView.width() * 3 / 4);
    m_rectInfoView.setTop(m_rectView.top());
    m_rectInfoView.setRight(m_rectView.right());
    m_rectInfoView.setBottom(m_rectView.bottom());

    m_rectView.setLeft(m_rectView.left());
    m_rectView.setTop(m_rectView.top());
    m_rectView.setRight(m_rectView.left() + m_rectView.width() * 3 / 4);
    m_rectView.setBottom(m_rectView.bottom());

    m_nW = m_rectView.width();
    m_nH = m_rectView.height();
    m_nCXOri = m_nCX = m_nW / 2;
    m_nCYOri = m_nCY = m_nH / 2;

    if (m_UAVnumber > 1 && m_line_kind != 1)
    {
        drawSelectedVehicles(p);
        drawVehicleBorder(p, false);

    }
    else
    {

    }
    if (m_UAVnumber > 0)
    {

    }
    else
    {

    }

    drawVehicleInfoShow(p);

}

void DrawVehicleIcon::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
    {
        return;
    }

    if (m_EBtnStates != BTN_EDIT && m_EBtnStates != BTN_ADD)
    {
        return;
    }

    if (m_UAVnumber < 2 || m_line_kind == 1)
    {
        return;
    }

    int nDesSize = getIconDescSize();

    QPoint mousePt = event->pos();
    SelectVehicleID = getMouseHoverVehicleID(mousePt);
    getMouseHoverVehicleIDLegend(mousePt);

    for (auto &itr : m_VehicleShowInfoVec)
    {
        if (itr.Flag_Selected != false)
        {
            itr.eOperType = OPER_MOVE_VEHICLE;
            m_Btn_Flag = 1;
        }
        else if ((SelectVehicleID == itr.SelectVehicleID) && (itr.Flag == true) && (SelectVehicleID != -1))
        {
            itr.eOperType = OPER_MOVE_VEHICLE_SELECTED;
            if (SelectVehicleID != -1)
            {

                if (SelectVehicleID == 0)
                {
                    itr.rect.setLeft(m_rectView.left() + m_nCX);
                    itr.rect.setTop(m_rectView.top() + m_nCY);
                    itr.rect.setRight(m_rectView.left() + m_nCX + 10 + nDesSize);
                    itr.rect.setBottom(m_rectView.top() + m_nCY + 30 + nDesSize);
                }

                else
                {
                    QPoint pt = convertWorldToView3D((float)m_sForm.m_vFormPos[SelectVehicleID - 1].x,
                                                     (float)m_sForm.m_vFormPos[SelectVehicleID - 1].y,
                                                     (float)m_sForm.m_vFormPos[SelectVehicleID - 1].z);
                    itr.rect.setLeft(pt.rx());
                    itr.rect.setTop(pt.ry());
                    itr.rect.setRight(pt.rx() + 10 + nDesSize);
                    itr.rect.setBottom(pt.ry() + 30 + nDesSize);
                }
            }
        }
        else
        {
            itr.eOperType = OPER_MOVE_NONE;
        }
    }
    m_LastMousePt = mousePt;

}

void DrawVehicleIcon::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
    {
        return;
    }

    if (m_EBtnStates != BTN_EDIT && m_EBtnStates != BTN_ADD)
    {
        return;
    }

    if (m_UAVnumber < 2 || m_line_kind == 1)
    {
        return;
    }

    int nDesSize = getIconDescSize();
    QPoint point = event->pos();

    int count1(0), count2(0);
    for (auto &itr : m_VehicleShowInfoVec)
    {
        SelectVehicleID = getMouseHoverVehicleID(point);
        if ((itr.eOperType == OPER_MOVE_VEHICLE) || (itr.eOperType == OPER_MOVE_VEHICLE_SELECTED))
        {
            if (m_Btn_Flag == 1)
            {
                m_Btn_Flag = 0;
                itr.eOperType = OPER_MOVE_NONE;
                break;
            }
            if (SelectVehicleID != -1)
            {
                itr.Flag = true;
                itr.SelectVehicleID = SelectVehicleID;
                for (auto &itr2 : m_VehicleShowInfoVec)
                {
                    if ((count1 != count2) && (itr2.SelectVehicleID == SelectVehicleID))
                    {
                        itr2.SelectVehicleID = -1;
                        itr2.Flag = false;
                    }
                    count2++;
                }
                LastSelectVehicleID = SelectVehicleID;
            }
            else
            {
                itr.Flag = false;
                itr.SelectVehicleID = -1;
            }
        }
        else
        {
            if (itr.Flag != false)
            {
                SelectVehicleID = LastSelectVehicleID;
            }
            else
            {
                SelectVehicleID = -1;
                itr.SelectVehicleID = -1;
            }
        }
        itr.eOperType = OPER_MOVE_NONE;
        count1++;
    }

    initVehicleInfoPosition();
    repaint();
}

void DrawVehicleIcon::mouseMoveEvent(QMouseEvent *event)
{
    QPoint mousePt = event->pos();

    if (m_EBtnStates != BTN_EDIT && m_EBtnStates != BTN_ADD)
    {
        return;
    }

    if (m_UAVnumber < 2 || m_line_kind == 1)
    {
        return;
    }

    int nHalfSize = getIconDescSize() / 2;

    QPoint VehicleInfo;
    float fOffsetH = 1;
    float fOffsetV = 1;
    QPoint ptOffset = mousePt - m_LastMousePt;

    VehicleInfo.setX(s_VehicleShowInfo.rect.left());
    VehicleInfo.setY(s_VehicleShowInfo.rect.top());

    for (auto &itr : m_VehicleShowInfoVec)
    {
        switch (itr.eOperType)
        {
        case OPER_MOVE_VEHICLE:

            itr.rect.setLeft(itr.rect.left() + ptOffset.rx());
            itr.rect.setTop(itr.rect.top() + ptOffset.ry());
            if (m_LastMousePt == ptOffset)
            {
                m_Btn_Flag = 1;
            }
            else
            {
                m_Btn_Flag = 0;
            }
            m_LastMousePt = mousePt;
            repaint();
            break;
        case OPER_MOVE_VEHICLE_SELECTED:
            itr.rect.setLeft(itr.rect.left() + ptOffset.rx());
            itr.rect.setTop(itr.rect.top() + ptOffset.ry());
            itr.Flag = false;
            m_LastMousePt = mousePt;
            repaint();
            break;
        default:
        {

        }
        break;
        }
    }
}

void DrawVehicleIcon::InitData()
{
    m_fScale = 0.5f;
    m_line_kind = 0;
    m_UAVnumber = 0;
    m_eViewMode = VIEW_TOP;
    SelectVehicleID = -1;
    m_nW = 0;
    m_nH = 0;
    m_nCX = 0;
    m_nCY = 0;;
    m_nCXOri = 0;
    m_nCYOri = 0;
    Last_UAVnumber = -1;
    m_Btn_Flag = 0;
    LastSelectVehicleID = -1;
    m_VehicleIcon[VIEW_TOP].load(":/images/UAV2_Cyan.png");
    m_VehicleIcon[VIEW_BACK].load(":/images/UAV2_Cyan_back_view.png");
    m_VehicleIcon[VIEW_TOP_SELECT].load(":/images/UAV2.png");
    memset(&s_VehicleShowInfo, 0x00, sizeof(VehicleShowInfo));

}

void DrawVehicleIcon::drawVehicleInfoIcon(QPainter &mPainter, int nX, int nY, int nW, int nH, int nID, int Color)
{

    QPen penLine(QColor(255, 255, 255), 1.0);
    mPainter.setPen(penLine);

    int nSrcW = m_VehicleIcon[m_eViewMode].width();
    int nSrcH = m_VehicleIcon[m_eViewMode].height();

    QRect destRect(nX - nW / 2, nY - nH / 2, nW, nH);
    if (Color == -1)
    {

        mPainter.drawPixmap(destRect.left(), destRect.top(), getIconDescSize(), getIconDescSize(), m_VehicleIcon[m_eViewMode]);

    }
    else
    {

        mPainter.drawPixmap(destRect.left(), destRect.top(), getIconDescSize(), getIconDescSize(), m_VehicleIcon[VIEW_TOP_SELECT]);

    }

    int nOrisY = destRect.top();
    destRect.setTop(nOrisY + 8);
    destRect.setHeight(32);
    QString strTmp = QString::number(nID);
    QFont font;
    font.setFamily("Arial");
    font.setPointSize(8);
    mPainter.setFont(font);
    QPen pen(COLOR_TXT_FLYID1);
    mPainter.setPen(pen);
    mPainter.drawText(destRect, Qt::AlignHCenter, strTmp);
}

void DrawVehicleIcon::drawSelectedVehicles(QPainter &mPainter)
{
    int nDesSize = getIconDescSize();

    for (auto itr : m_VehicleShowInfoVec)
    {

        if (itr.SelectVehicleID == 0 && (itr.Flag == true))
        {
            QPoint pFirstRect;
            pFirstRect.setX(m_rectView.left() + m_nCX);
            pFirstRect.setY(m_rectView.top() + m_nCY);
            drawVehicleInfoIcon(mPainter, (int)pFirstRect.rx(), (int)pFirstRect.ry(), nDesSize, nDesSize, itr.CountNo, -1);
        }

        else if ((itr.SelectVehicleID != -1) && (itr.Flag == true))
        {
            if ((itr.SelectVehicleID - 1 <= m_sForm.GetVehicleNum())
                    && (itr.SelectVehicleID <= m_sForm.m_vFormPos.size()))
            {
                QPoint pt = convertWorldToView3D((float)m_sForm.m_vFormPos[itr.SelectVehicleID - 1].x,
                                                 (float)m_sForm.m_vFormPos[itr.SelectVehicleID - 1].y,
                                                 (float)m_sForm.m_vFormPos[itr.SelectVehicleID - 1].z);
                drawVehicleInfoIcon(mPainter, (int)pt.rx(), (int)pt.ry(), nDesSize, nDesSize, itr.CountNo, -1);
            }
        }
    }
}

QPoint DrawVehicleIcon::convertWorldToView2D(float fX, float fY)
{
    QPoint pt;

    QPoint ptOri = convertViewToWorld((float)m_nCX, (float)m_nCY);

    fX -= ptOri.rx();
    fY -= ptOri.ry();

    float fRangeX, fRangeY;
    getViewRange(1, 1, fRangeX, fRangeY);

    fX /= fRangeX;
    fY /= fRangeY;

    pt.setX(m_rectView.left() + m_nCX + fX);
    pt.setY(m_rectView.top() + m_nCY - fY);
    return pt;
}

QPoint DrawVehicleIcon::convertWorldToView3D(float fX, float fY, float fZ)
{
    QPoint pt(0.0f, 0.0f);
    switch (m_eViewMode)
    {
    case VIEW_TOP:
        pt = convertWorldToView2D(fX, fY);
        break;
    case VIEW_BACK:
        pt = convertWorldToView2D(fX, fZ);
        break;
    default:
        break;
    }
    return pt;
}

void DrawVehicleIcon::getViewRange(int nW, int nH, float &fRangeX, float &fRangeY)
{
    fRangeX = (float)nW / PIXELS_PER_UNIT;
    fRangeY = (float)nH / PIXELS_PER_UNIT;
    fRangeX *= m_fScale;
    fRangeY *= m_fScale;
}

QPoint DrawVehicleIcon::convertViewToWorld(float fX, float fY)
{
    QPoint pt;

    float fOffsetX2Ori = fX - m_nCX;
    float fOffsetY2Ori = fY - m_nCY;

    float fRangeX2Ori, fRangeY2Ori;
    getViewRange((int)fOffsetX2Ori, (int)fOffsetY2Ori, fRangeX2Ori, fRangeY2Ori);
    pt.setX(fRangeX2Ori);
    pt.setY(-fRangeY2Ori);
    return pt;
}

void DrawVehicleIcon::drawVehicleBorder(QPainter &mPainter, bool bSel)
{
    QFont m_JWStrFont;
    m_JWStrFont.setFamily("Arial");
    m_JWStrFont.setPointSize(8);
    mPainter.setFont(m_JWStrFont);
    QPen TextPen(qRgb(255, 255, 255));
    mPainter.setPen(TextPen);
    mPainter.setBackgroundMode(Qt::TransparentMode);
    QPen penSel(QColor(200, 255, 187, 255), 2);
    QPen pen(QColor(255, 200, 0, 200), 2);
    int nHalfSize = getIconDescSize() / 2;

    QPoint ptFirstRect[4];
    ptFirstRect[0].setX(m_rectView.left() + m_nCX - nHalfSize);
    ptFirstRect[0].setY(m_rectView.top() + m_nCY - nHalfSize);
    ptFirstRect[1].setX(m_rectView.left() + m_nCX + nHalfSize);
    ptFirstRect[1].setY(m_rectView.top() + m_nCY - nHalfSize);
    ptFirstRect[2].setX(m_rectView.left() + m_nCX + nHalfSize);
    ptFirstRect[2].setY(m_rectView.top() + m_nCY + nHalfSize);
    ptFirstRect[3].setX(m_rectView.left() + m_nCX - nHalfSize);
    ptFirstRect[3].setY(m_rectView.top() + m_nCY + nHalfSize);

    bool bSeled = false;
    bSeled = JudgeSelected(0);
    if (bSeled == true)
    {
        mPainter.setPen(pen);

    }
    else
    {
        mPainter.setPen(penSel);

    }
    mPainter.drawLine(ptFirstRect[0], ptFirstRect[1]);
    mPainter.drawLine(ptFirstRect[1], ptFirstRect[2]);
    mPainter.drawLine(ptFirstRect[2], ptFirstRect[3]);
    mPainter.drawLine(ptFirstRect[3], ptFirstRect[0]);

    QString strFirst = tr2("1");
    mPainter.setPen(TextPen);
    QRect ptText = m_rectView;
    ptText.setLeft(ptFirstRect[0].rx());
    ptText.setTop(ptFirstRect[0].ry() - 14);
    ptText.setWidth(getIconDescSize());
    ptText.setHeight(getIconDescSize());
    mPainter.drawText(ptText, Qt::AlignHCenter, strFirst);

    for (int i = 0; i < m_sForm.GetVehicleNum() - 1; i++)
    {
        QPoint pt = convertWorldToView3D((float)m_sForm.m_vFormPos[i].x,
                                         (float)m_sForm.m_vFormPos[i].y,
                                         (float)m_sForm.m_vFormPos[i].z);

        QPoint ptRect[4];
        ptRect[0].setX(pt.rx() - nHalfSize);
        ptRect[0].setY(pt.ry() - nHalfSize);
        ptRect[1].setX(pt.rx() + nHalfSize);
        ptRect[1].setY(pt.ry() - nHalfSize);
        ptRect[2].setX(pt.rx() + nHalfSize);
        ptRect[2].setY(pt.ry() + nHalfSize);
        ptRect[3].setX(pt.rx() - nHalfSize);
        ptRect[3].setY(pt.ry() + nHalfSize);

        bSeled = JudgeSelected(i + 1);
        if (bSeled == true)
        {
            mPainter.setPen(pen);

        }
        else
        {
            mPainter.setPen(penSel);

        }
        mPainter.drawLine(ptRect[0], ptRect[1]);
        mPainter.drawLine(ptRect[1], ptRect[2]);
        mPainter.drawLine(ptRect[2], ptRect[3]);
        mPainter.drawLine(ptRect[3], ptRect[0]);

        QString str;
        if (i == m_sForm.GetVehicleNum() - 1)
        {
            str = "1";
        }
        else
        {
            str = QString::number(i + 2);
        }
        mPainter.setPen(TextPen);
        ptText.setLeft(ptRect[0].rx());
        ptText.setTop(ptRect[0].ry() - 14);
        ptText.setWidth(getIconDescSize());
        ptText.setHeight(getIconDescSize());
        mPainter.drawText(ptText, Qt::AlignHCenter, str);
    }
}

void DrawVehicleIcon::drawVehicleInfoShow(QPainter &mPainter)
{
    int nDesSize = getIconDescSize();
    QFont m_JWStrFont;
    m_JWStrFont.setFamily("Î¢ÈíÑÅºÚ");
    m_JWStrFont.setPointSize(10);
    mPainter.setFont(m_JWStrFont);
    QPen TextPen(qRgb(202, 212, 228));
    mPainter.setPen(TextPen);

    QRect ptText;
    ptText.setLeft(m_rectInfoView.left());
    ptText.setTop(m_rectInfoView.top() + 5);
    ptText.setWidth(100);
    ptText.setHeight(20);
    mPainter.drawText(ptText, Qt::AlignLeft, tr("UAV Information"));

    if (m_UAVnumber > 0)
    {
        for (auto itr : m_VehicleShowInfoVec)
        {
            if (itr.Flag_In_Group == true)
            {
                if (itr.eOperType == OPER_MOVE_VEHICLE_SELECTED)
                {
                    drawVehicleInfoIcon(mPainter, itr.rect.left(), itr.rect.top(), nDesSize, nDesSize, itr.CountNo, 1);

                    QPen WhitePen(qRgb(255, 255, 255));
                    mPainter.setPen(WhitePen);
                    ptText.setLeft(m_rectInfoView.left() + itr.rect.left() + nDesSize);
                    ptText.setTop(m_rectInfoView.top() + itr.rect.top() + 60 - nDesSize / 2);
                    ptText.setWidth(nDesSize);
                    ptText.setHeight(nDesSize);
                    mPainter.drawText(ptText, Qt::AlignCenter, itr.name);

                }
                else
                {
                    drawVehicleInfoIcon(mPainter, m_rectInfoView.left() + itr.rect.left() + nDesSize / 2, m_rectInfoView.top() + itr.rect.top() + 60, nDesSize, nDesSize, itr.CountNo, itr.SelectVehicleID);

                    QPen WhitePen(qRgb(255, 255, 255));
                    mPainter.setPen(WhitePen);
                    ptText.setLeft(m_rectInfoView.left() + itr.rect.left() + nDesSize);
                    ptText.setTop(m_rectInfoView.top() + itr.rect.top() + 60 - nDesSize / 2);
                    ptText.setWidth(nDesSize);
                    ptText.setHeight(nDesSize);
                    mPainter.drawText(ptText, Qt::AlignCenter, itr.name);

                }
            }
        }

    }
}

void DrawVehicleIcon::initVehicleInfoPosition()
{
    int count = 0;
    int nDesSize = getIconDescSize();
    if (m_UAVnumber > 0)
    {
        for (auto &itr : m_VehicleShowInfoVec)
        {
            if (itr.Flag_In_Group == true)
            {
                itr.rect.setLeft(s_VehicleShowInfo.rect.left());
                itr.rect.setTop(s_VehicleShowInfo.rect.top() + nDesSize * count - 10);
                itr.rect.setRight(itr.rect.left() + nDesSize);
                itr.rect.setBottom(itr.rect.top() + nDesSize);

                count++;
                itr.CountNo = count;
            }
        }
    }
}

void DrawVehicleIcon::SetVehicleShowInfo(VehicleShowInfoVec &DataVec)
{
    m_VehicleShowInfoVec = DataVec;
}

VehicleShowInfoVec DrawVehicleIcon::GetVehicleShowInfo()
{
    return m_VehicleShowInfoVec;
}

void DrawVehicleIcon::SetUAVNumber(int nNumber)
{
    m_UAVnumber = nNumber;
}

void DrawVehicleIcon::SetScale(float fScale)
{
    m_fScale = fScale;
}

bool DrawVehicleIcon::JudgeSelected(int nID)
{
    bool bSeled = false;
    for (auto itr : m_VehicleShowInfoVec)
    {
        if (itr, SelectVehicleID == nID)
        {
            bSeled = true;
            break;
        }
    }
    return bSeled;
}

void DrawVehicleIcon::SetUAVForm(SForm &sform)
{
    m_sForm = sform;
}

void DrawVehicleIcon::SetLineKind(int nKind)
{
    m_line_kind = nKind;
}

void DrawVehicleIcon::SetWndStates(E_BTN_STATES btnStates)
{
    m_EBtnStates = btnStates;
}

int DrawVehicleIcon::getMouseHoverVehicleID(QPoint point)
{
    int nHalfSize = getIconDescSize() / 2;

    QPoint pFirstRect;
    pFirstRect.setX(m_rectView.left() + m_nCX);
    pFirstRect.setY(m_rectView.top() + m_nCY);
    if (abs(point.rx() - pFirstRect.rx()) < nHalfSize && abs(point.ry() - pFirstRect.ry()) < nHalfSize)
    {
        return 0;
    }

    for (int i = 0; i <= m_sForm.GetVehicleNum() - 1; i++)
    {

        QPoint pt = convertWorldToView3D((float)m_sForm.m_vFormPos[i].x, (float)m_sForm.m_vFormPos[i].y, (float)m_sForm.m_vFormPos[i].z);
        if (abs(point.rx() - pt.rx()) < nHalfSize && abs(point.ry() - pt.ry()) < nHalfSize)
        {
            return i + 1;
        }
    }
    return -1;
}

bool DrawVehicleIcon::getMouseHoverVehicleIDLegend(QPoint point)
{
    bool bresult = false;
    int nHalfSize = getIconDescSize() / 2;
    for (auto &itr : m_VehicleShowInfoVec)
    {
        if (abs(point.rx() - itr.rect.left() - m_rectInfoView.left() - nHalfSize) < nHalfSize &&
                abs(point.ry() - itr.rect.top() - m_rectInfoView.top() - 60) < nHalfSize)
        {
            itr.Flag_Selected = true;
            bresult = true;
        }
        else
        {
            itr.Flag_Selected = false;
        }
    }
    return bresult;
}
