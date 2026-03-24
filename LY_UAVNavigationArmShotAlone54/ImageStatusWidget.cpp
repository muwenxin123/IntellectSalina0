#include "ImageStatusWidget.h"
#include "ui_ImageStatusWidget.h"

ImageStatusWidget::ImageStatusWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ImageStatusWidget)
    , m_IUAVNavigationArmShotAlone54Dialog(nullptr)
    , m_scaledContents(false)
    , m_ImageStatusPixmap(ImageStatusPixmap::ImageStatusPixmap_Dark)
    , m_pixmap{}
    , m_pixmapMiddle{}
    , m_scaledPixmap{}
{
    ui->setupUi(this);

    this->setStyleSheet("margin: 0px; border-width: 0px; padding: 0px; padding: 0px;");
    this->setContentsMargins(0, 0, 0, 0);
}

ImageStatusWidget::~ImageStatusWidget()
{
    delete ui;
}

bool ImageStatusWidget::setImageStatusPixmap(const QPixmap &darkPixmap, const QPixmap &lightPixmap, const QPixmap &redPixmap)
{
    if (darkPixmap.isNull() || lightPixmap.isNull())
    {
        return false;
    }

    if (!darkPixmap.isNull())
    {
        m_pixmap[ImageStatusPixmap::ImageStatusPixmap_Dark] = darkPixmap;
    }

    if (!lightPixmap.isNull())
    {
        m_pixmap[ImageStatusPixmap::ImageStatusPixmap_Light] = lightPixmap;
    }

    if (!redPixmap.isNull())
    {
        m_pixmap[ImageStatusPixmap::ImageStatusPixmap_Red] = redPixmap;
    }

    return true;
}

bool ImageStatusWidget::setBombStatusMiddlePixmap(const QPixmap &darkMiddlePixmap, const QPixmap &lightMiddlePixmap, const QPixmap &redMiddlePixmap)
{
    if (darkMiddlePixmap.isNull() || lightMiddlePixmap.isNull())
    {
        return false;
    }

    if (!darkMiddlePixmap.isNull())
    {
        m_pixmapMiddle[ImageStatusPixmap::ImageStatusPixmap_Dark] = darkMiddlePixmap;
    }

    if (!lightMiddlePixmap.isNull())
    {
        m_pixmapMiddle[ImageStatusPixmap::ImageStatusPixmap_Light] = lightMiddlePixmap;
    }

    if (!redMiddlePixmap.isNull())
    {
        m_pixmapMiddle[ImageStatusPixmap::ImageStatusPixmap_Red] = redMiddlePixmap;
    }

    return true;
}

void ImageStatusWidget::setStatusDark()
{
    if (ImageStatusPixmap::ImageStatusPixmap_Dark != m_ImageStatusPixmap)
    {
        m_ImageStatusPixmap = ImageStatusPixmap::ImageStatusPixmap_Dark;
        update();
    }
}

void ImageStatusWidget::setStatusLight()
{
    if (ImageStatusPixmap::ImageStatusPixmap_Light != m_ImageStatusPixmap)
    {
        m_ImageStatusPixmap = ImageStatusPixmap::ImageStatusPixmap_Light;
        update();
    }
}

void ImageStatusWidget::setStatusRed()
{
    if (ImageStatusPixmap::ImageStatusPixmap_Light != m_ImageStatusPixmap)
    {
        m_ImageStatusPixmap = ImageStatusPixmap::ImageStatusPixmap_Red;
        update();
    }
}

void ImageStatusWidget::setText(const QString &text)
{
    Q_UNUSED(text)
}

void ImageStatusWidget::setIUAVNavigationArmShotAlone54Dialog(QWidget *IUAVNavigationArmShotAlone54Dialog)
{
    m_IUAVNavigationArmShotAlone54Dialog = IUAVNavigationArmShotAlone54Dialog;
}

void ImageStatusWidget::paintEvent(QPaintEvent *)
{
    QStyle *style = QWidget::style();
    QPainter painter(this);
    QRect cr = rect();

    int align = QStyle::visualAlignment(layoutDirection(), Qt::AlignmentFlag::AlignHCenter | Qt::AlignmentFlag::AlignVCenter);

    QPixmap *pixmap = &m_pixmap[m_ImageStatusPixmap];
    if (m_IUAVNavigationArmShotAlone54Dialog && (m_IUAVNavigationArmShotAlone54Dialog->frameGeometry().width() < m_MiddleWidth || m_IUAVNavigationArmShotAlone54Dialog->frameGeometry().height() < m_MiddleHeight))
    {
        pixmap = &m_pixmapMiddle[m_ImageStatusPixmap];
        if (!pixmap || pixmap->isNull())
        {
            pixmap = &m_pixmap[m_ImageStatusPixmap];
        }
    }
    if (pixmap && !pixmap->isNull())
    {
        QPixmap pix;
        if (m_scaledContents)
        {
            QPixmap *scaledpixmap = &m_scaledPixmap[m_ImageStatusPixmap];
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

