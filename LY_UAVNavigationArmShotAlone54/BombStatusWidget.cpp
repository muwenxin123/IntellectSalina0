#include "BombStatusWidget.h"
#include "ui_BombStatusWidget.h"

#include "IUAVNavigationArmShotAlone54Dialog.h"

BombStatusWidget::BombStatusWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BombStatusWidget)
    , m_IUAVNavigationArmShotAlone54Dialog(nullptr)
    , m_bomBCallBackFunction(nullptr)
    , m_bombData()
    , m_bombStatus(BombStatus::BombStatus_UnReign)
    , m_bombStatusRect(0, 0, 0, 0)
    , m_bombStatusRectMiddle(0, 0, 0, 0)
    , m_selected(false)
    , m_scaledContents(false)
    , m_pixmap{}
    , m_pixmapMiddle{}
    , m_scaledPixmap{}
{
    ui->setupUi(this);

    this->setStyleSheet("margin: 0px; border-width: 0px; padding: 0px; padding: 0px;");
    this->setContentsMargins(0, 0, 0, 0);
}

BombStatusWidget::~BombStatusWidget()
{
    delete ui;
}

void BombStatusWidget::setUnReign()
{
    if (m_bombStatus != BombStatus::BombStatus_UnReign)
    {
        m_bombStatus  = BombStatus::BombStatus_UnReign;
        update();
    }
}

void BombStatusWidget::setReign()
{
    if (m_bombStatus != BombStatus::BombStatus_Reign)
    {
        m_bombStatus  = BombStatus::BombStatus_Reign;
        update();
    }
}

void BombStatusWidget::setLock()
{
    if (m_bombStatus != BombStatus::BombStatus_Lock)
    {
        m_bombStatus  = BombStatus::BombStatus_Lock;
        update();
    }
}

void BombStatusWidget::setUnlock()
{
    if (m_bombStatus != BombStatus::BombStatus_UnLock)
    {
        m_bombStatus  = BombStatus::BombStatus_UnLock;
        update();
    }
}

bool BombStatusWidget::getSelected() const
{
    return m_selected;
}

void BombStatusWidget::setSelected(bool selected)
{
    if (m_selected != selected)
    {
        m_selected = selected;
        update();
    }
}

const BombStatusWidget::BombData &BombStatusWidget::getBombData() const
{
    return m_bombData;
}

void BombStatusWidget::initBombData(const BombData &bombData, IUAVNavigationArmShotAlone54Dialog *pIUAVNavigationArmShotAlone54Dialog, BomBCallBackFunction bomBCallBackFunction)
{
    m_IUAVNavigationArmShotAlone54Dialog = pIUAVNavigationArmShotAlone54Dialog;
    m_bomBCallBackFunction = bomBCallBackFunction;
    m_bombData = bombData;
}

bool BombStatusWidget::setBombStatusPixmap(const QPixmap &unReignPixmap, const QPixmap &unReignSelPixmap, const QPixmap &reignPixmap, const QPixmap &reignSelPixmap, const QPixmap &unLockPixmap, const QPixmap &unLockSelPixmap)
{
    if (unReignPixmap.isNull() || unReignSelPixmap.isNull() || reignPixmap.isNull() || reignSelPixmap.isNull() || unLockPixmap.isNull() || unLockSelPixmap.isNull())
    {
        return false;
    }

    if (!unReignPixmap.isNull())
    {
        m_pixmap[BombStatusPixmap::BombStatusPixmap_UnReignPixmap] = unReignPixmap;
    }

    if (!unReignSelPixmap.isNull())
    {
        m_pixmap[BombStatusPixmap::BombStatusPixmap_UnReignSelPixmap] = unReignSelPixmap;
    }

    if (!reignPixmap.isNull())
    {
        m_pixmap[BombStatusPixmap::BombStatusPixmap_ReignPixmap] = reignPixmap;
    }

    if (!reignSelPixmap.isNull())
    {
        m_pixmap[BombStatusPixmap::BombStatusPixmap_ReignSelPixmap] = reignSelPixmap;
    }

    if (!unLockPixmap.isNull())
    {
        m_pixmap[BombStatusPixmap::BombStatusPixmap_UnLockPixmap] = unLockPixmap;
    }

    if (!unLockSelPixmap.isNull())
    {
        m_pixmap[BombStatusPixmap::BombStatusPixmap_UnLockSelPixmap] = unLockSelPixmap;
    }

    m_bombStatusRect = unLockSelPixmap.rect();

    return true;
}

bool BombStatusWidget::setBombStatusMiddlePixmap(const QPixmap &unReignMiddlePixmap, const QPixmap &unReignSelMiddlePixmap, const QPixmap &reignMiddlePixmap, const QPixmap &reignSelMiddlePixmap, const QPixmap &unLockMiddlePixmap, const QPixmap &unLockSelMiddlePixmap)
{
    if (unReignMiddlePixmap.isNull() || unReignSelMiddlePixmap.isNull() || reignMiddlePixmap.isNull() || reignSelMiddlePixmap.isNull() || unLockMiddlePixmap.isNull() || unLockSelMiddlePixmap.isNull())
    {
        return false;
    }

    if (!unReignMiddlePixmap.isNull())
    {
        m_pixmapMiddle[BombStatusPixmap::BombStatusPixmap_UnReignPixmap] = unReignMiddlePixmap;
    }

    if (!unReignSelMiddlePixmap.isNull())
    {
        m_pixmapMiddle[BombStatusPixmap::BombStatusPixmap_UnReignSelPixmap] = unReignSelMiddlePixmap;
    }

    if (!reignMiddlePixmap.isNull())
    {
        m_pixmapMiddle[BombStatusPixmap::BombStatusPixmap_ReignPixmap] = reignMiddlePixmap;
    }

    if (!reignSelMiddlePixmap.isNull())
    {
        m_pixmapMiddle[BombStatusPixmap::BombStatusPixmap_ReignSelPixmap] = reignSelMiddlePixmap;
    }

    if (!unLockMiddlePixmap.isNull())
    {
        m_pixmapMiddle[BombStatusPixmap::BombStatusPixmap_UnLockPixmap] = unLockMiddlePixmap;
    }

    if (!unLockSelMiddlePixmap.isNull())
    {
        m_pixmapMiddle[BombStatusPixmap::BombStatusPixmap_UnLockSelPixmap] = unLockSelMiddlePixmap;
    }

    m_bombStatusRectMiddle = unLockSelMiddlePixmap.rect();

    return true;
}

void BombStatusWidget::mousePressEvent(QMouseEvent *ev)
{
    Q_UNUSED(ev)
    if (ev == nullptr)
    {
        return ;
    }

    QRect bombStatusRect(m_bombStatusRect);
    if (m_IUAVNavigationArmShotAlone54Dialog && (m_IUAVNavigationArmShotAlone54Dialog->frameGeometry().width() < m_MiddleWidth || m_IUAVNavigationArmShotAlone54Dialog->frameGeometry().height() < m_MiddleHeight))
    {
        bombStatusRect = m_bombStatusRectMiddle;
    }
    bombStatusRect.moveCenter(rect().center());
    if (!bombStatusRect.contains(ev->pos()))
    {
        return ;
    }

    m_selected = (!m_selected);

    if (m_bomBCallBackFunction != nullptr)
    {
        m_bomBCallBackFunction(this, m_IUAVNavigationArmShotAlone54Dialog);
    }

    update();
}

void BombStatusWidget::paintEvent(QPaintEvent *)
{
    QStyle *style = QWidget::style();
    QPainter painter(this);
    QRect cr = rect();

    int align = QStyle::visualAlignment(layoutDirection(), Qt::AlignmentFlag::AlignHCenter | Qt::AlignmentFlag::AlignVCenter);

    BombStatusPixmap bombStatusPixmap = BombStatusPixmap::BombStatusPixmap_UnReignPixmap;
    if (m_bombStatus == BombStatus::BombStatus_UnReign)
    {
        if (m_selected)
        {
            bombStatusPixmap = BombStatusPixmap::BombStatusPixmap_UnReignSelPixmap;
        }
        else
        {
            bombStatusPixmap = BombStatusPixmap::BombStatusPixmap_UnReignPixmap;
        }
    }
    else if ((m_bombStatus == BombStatus::BombStatus_Reign) || (m_bombStatus == BombStatus::BombStatus_Lock))
    {
        if (m_selected)
        {
            bombStatusPixmap = BombStatusPixmap::BombStatusPixmap_ReignSelPixmap;
        }
        else
        {
            bombStatusPixmap = BombStatusPixmap::BombStatusPixmap_ReignPixmap;
        }
    }
    else if (m_bombStatus == BombStatus::BombStatus_UnLock)
    {
        if (m_selected)
        {
            bombStatusPixmap = BombStatusPixmap::BombStatusPixmap_UnLockSelPixmap;
        }
        else
        {
            bombStatusPixmap = BombStatusPixmap::BombStatusPixmap_UnLockPixmap;
        }
    }

    QPixmap *pixmap       = &m_pixmap[bombStatusPixmap];
    if (m_IUAVNavigationArmShotAlone54Dialog && (m_IUAVNavigationArmShotAlone54Dialog->frameGeometry().width() < m_MiddleWidth || m_IUAVNavigationArmShotAlone54Dialog->frameGeometry().height() < m_MiddleHeight))
    {
        pixmap = &m_pixmapMiddle[bombStatusPixmap];
        if (!pixmap || pixmap->isNull())
        {
            pixmap = &m_pixmap[bombStatusPixmap];
        }
    }
    if (pixmap && !pixmap->isNull())
    {
        QPixmap pix;
        if (m_scaledContents)
        {
            QPixmap *scaledpixmap = &m_scaledPixmap[bombStatusPixmap];
            QSize scaledSize = cr.size() * devicePixelRatioF();
            if (!scaledpixmap || scaledpixmap->size() != scaledSize)
            {
                *scaledpixmap = pixmap->scaled(scaledSize, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
                scaledpixmap->setDevicePixelRatio(devicePixelRatioF());
            }
            pix = *scaledpixmap;
        }
        else
        {
            pix = *pixmap;
        }
        QStyleOption opt;
        opt.initFrom(this);
        if (!isEnabled())
        {
            pix = style->generatedIconPixmap(QIcon::Disabled, pix, &opt);
        }

        style->drawItemPixmap(&painter, cr, align, pix);
    }
}

void BombStatusWidget::drawFrame(QPainter *)
{
    QStyleOptionFrame opt;
    initStyleOption(&opt);
}

void BombStatusWidget::initStyleOption(QStyleOptionFrame *option) const
{
    Q_UNUSED(option)
}

