#ifndef IMAGESTATUSWIDGET_H
#define IMAGESTATUSWIDGET_H

#include <QWidget>
#include <QPixmap>
#include <QBitmap>
#include <QPainter>
#include <QPaintEvent>
#include <QStyleOptionFrame>

namespace Ui
{
class ImageStatusWidget;
}

class ImageStatusWidget : public QWidget
{
    Q_OBJECT

public:
    typedef enum __ImageStatusPixmap
    {
        ImageStatusPixmap_Dark  = 0x00,
        ImageStatusPixmap_Light = 0x01,
        ImageStatusPixmap_Red   = 0x02,
    } ImageStatusPixmap;

public:
    explicit ImageStatusWidget(QWidget *parent = nullptr);
    virtual ~ImageStatusWidget();

    bool setImageStatusPixmap(const QPixmap &darkPixmap, const QPixmap &lightPixmap, const QPixmap &redPixmap);
    bool setBombStatusMiddlePixmap(const QPixmap &darkMiddlePixmap, const QPixmap &lightMiddlePixmap, const QPixmap &redMiddlePixmap);

    void setStatusDark();
    void setStatusLight();
    void setStatusRed();

    void setText(const QString &text);

    void setIUAVNavigationArmShotAlone54Dialog(QWidget *IUAVNavigationArmShotAlone54Dialog);

protected:
    void paintEvent(QPaintEvent *);

private:
    Ui::ImageStatusWidget *ui;
    static constexpr unsigned int     m_PixmapSum    = 3;
    static constexpr int              m_MiddleWidth  = 600;
    static constexpr int              m_MiddleHeight = 600;
    QWidget                          *m_IUAVNavigationArmShotAlone54Dialog;
    bool                              m_scaledContents;
    ImageStatusPixmap                 m_ImageStatusPixmap;
    QPixmap                           m_pixmap[m_PixmapSum];
    QPixmap                           m_pixmapMiddle[m_PixmapSum];
    QPixmap                           m_scaledPixmap[m_PixmapSum];
};

#endif
