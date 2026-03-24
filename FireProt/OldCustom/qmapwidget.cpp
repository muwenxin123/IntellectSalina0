#include "qmapwidget.h"
#include <math.h>
#include "MapIconWidget.h"
#include <QTabWidget>
#include <QTimer>
#include <QVariant>
#include <QLabel>
#include <QDebug>

const static int disCount = 0; // Æ«ÒÆ¾àÀë

QMapWidget::QMapWidget(QWidget *parent) : QWidget(parent)
{
    m_b3D = false;
    m_pMap = new QWidget(this);
    m_pLayerControl = new QTabWidget(this);
    m_pMapIconWidget = NULL;
    m_pLayerControl->setObjectName("HalfBG");

    m_pTipLabel = new QLabel(this);
    m_pTipLabel->setObjectName("MapWidgetTipLabel");
    m_pTipLabel->setAlignment(Qt::AlignRight);
    m_pTipLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
}

QMapWidget::~QMapWidget()
{
}

void QMapWidget::setTips(const QStringList &tips)
{
    int w = 0, h = 0;
    QString text;
    QFontMetrics fm(m_pTipLabel->font());

    for (int i = 0; i < tips.size(); ++i)
    {
        if (i != tips.size() - 1)
        {
            text.append(tips[i] + "\n");
        }
        else
        {
            text.append(tips[i]);
        }

        h = fm.boundingRect(tips[i]).height();
        int tw = fm.boundingRect(tips[i]).width();
        if (tw > w)
        {
            w = tw;
        }
    }

    m_pTipLabel->setText(text);
    m_pTipLabel->setFixedSize(w + 10, h * tips.size());
    m_pTipLabel->move(QPoint(rect().right() - m_pTipLabel->width(), rect().height() - m_pTipLabel->height()));
}

void QMapWidget::setWidget(QWidget *p3D, QWidget *p2D)
{
    m_pEarth = p3D;
    m_p2D = p2D;
    m_pMapIconWidget = new MapIconWidget(m_p2D);
    m_pMapIconWidget->setObjectName("HalfBG");
    m_pMapIconWidget->show();

}

void QMapWidget::resizeEvent(QResizeEvent *event)
{
    QRect rc = rect();
    m_pMap->resize(size());
    QSize mapTopRightSize = m_pMapIconWidget->mapTopRightSize();
    if (m_pMapIconWidget == NULL)
    {
        return;
    }

    m_pMapIconWidget->move(QPoint(rc.right() - mapTopRightSize.width() - disCount, mapTopRightSize.height()));
    m_pMapIconWidget->raise();
    if (m_b3D)
    {
        if (!m_pMapIconWidget->m_p3DTBtn.isEmpty())
        {
            int count = 0;
            for (auto i : m_pMapIconWidget->m_p3DTBtn)
            {
                i->move(QPoint(rc.right() - mapTopRightSize.width() - disCount, mapTopRightSize.height() + 12 + (i->height() + 6)*count++));
                i->raise();
            }
        }
    }

    m_pTipLabel->move(QPoint(rc.right() - m_pTipLabel->width(), rc.height() - m_pTipLabel->height()));

    emit resizeSig();
    QWidget::resizeEvent(event);
}

void QMapWidget::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
}

void QMapWidget::switch23D(bool D3)
{
    m_b3D = D3;
    if (D3)
    {
        m_pTipLabel->hide();
        if (!m_pMapIconWidget->m_p3DTBtn.isEmpty())
        {
            int count = 0;
            for (auto i : m_pMapIconWidget->m_p3DTBtn)
            {
                i->setParent(m_pEarth);
                i->move(rect().right() - m_pMapIconWidget->mapTopRightSize().width() - disCount, m_pMapIconWidget->mapTopRightSize().height() + 12 + (i->height() + 6)*count++);
                i->raise();
                i->show();
            }
        }
    }
    else
    {
        m_pTipLabel->show();
        if (!m_pMapIconWidget->m_p3DTBtn.isEmpty())
        {
            for (auto i : m_pMapIconWidget->m_p3DTBtn)
            {
                m_pMapIconWidget->m_vLayout.insertWidget(i->property("index").toInt(), i);
            }
        }
    }
}
