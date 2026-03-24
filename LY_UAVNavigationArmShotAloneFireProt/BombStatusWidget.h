#ifndef BOMBSTATUSWIDGET_H
#define BOMBSTATUSWIDGET_H

#include <QWidget>
#include <QPixmap>
#include <QBitmap>
#include <QPainter>
#include <QPaintEvent>
#include <QStyleOptionFrame>

namespace Ui {
class BombStatusWidget;
}

class BombStatusWidget;
class IUAVNavigationArmShotAloneFireProtDialog;

using BomBCallBackFunction = void(*)(BombStatusWidget *, IUAVNavigationArmShotAloneFireProtDialog*);

class BombStatusWidget : public QWidget
{
    Q_OBJECT

public:
    typedef enum class __BombStatus
    {
        BombStatus_None       = 0x00,
        BombStatus_UnReign    = 0x01,
        BombStatus_Reign      = 0x02,
        BombStatus_Lock       = 0x03,
        BombStatus_UnLock     = 0x04,
    }BombStatus;

    typedef enum __BombStatusPixmap
    {
        BombStatusPixmap_UnReignPixmap    = 0x00,
        BombStatusPixmap_UnReignSelPixmap = 0x01,
        BombStatusPixmap_ReignPixmap      = 0x02,
        BombStatusPixmap_ReignSelPixmap   = 0x03,
        BombStatusPixmap_UnLockPixmap     = 0x04,
        BombStatusPixmap_UnLockSelPixmap  = 0x05,
    }BombStatusPixmap;

    typedef enum __BombHangType
    {
        BombHangType_None = 0,
        BombHangType_A,
        BombHangType_B,
    }BombHangType;

    typedef enum __BombType
    {
        BombType_None = -1,
        BombType_A1 = 0x00,
        BombType_A2 = 0x01,
        BombType_A3 = 0x02,
        BombType_A4 = 0x03,
        BombType_B1 = BombType_A1,
        BombType_B2 = BombType_A2,
        BombType_B3 = BombType_A3,
        BombType_B4 = BombType_A4,
    }BombType;

    typedef struct __BombData {
    public:
        __BombData(const std::string& bombHangTypeName ="", BombHangType bombHangType = BombHangType::BombHangType_None, const std::string& bombTypeName ="", BombType bombType= BombType::BombType_None, int16_t bombIndex = -1)
            : bombHangTypeName(bombHangTypeName)
            , bombHangType(bombHangType)
            , bombTypeName(bombTypeName)
            , bombType(bombType)
            , bombIndex(bombIndex)
        {}

        __BombData(const __BombData& bombData){
            if(this == &bombData){
                return ;
            }

            bombHangTypeName =  (bombData.bombHangTypeName);
            bombHangType     =  (bombData.bombHangType);
            bombTypeName     =  (bombData.bombTypeName);
            bombType         =  (bombData.bombType);
            bombIndex        =  (bombData.bombIndex);
        }

        __BombData operator =(const __BombData& bombData){
            if(this == &bombData){
                return *this;
            }

            bombHangTypeName =  (bombData.bombHangTypeName);
            bombHangType     =  (bombData.bombHangType);
            bombTypeName     =  (bombData.bombTypeName);
            bombType         =  (bombData.bombType);
            bombIndex        =  (bombData.bombIndex);
            return *this;
        }

        ~__BombData() {}
        std::string   bombHangTypeName;
        BombHangType  bombHangType;
        std::string   bombTypeName;
        BombType      bombType;
        int16_t       bombIndex;
    }BombData;

public:
    explicit BombStatusWidget(QWidget *parent = nullptr);
    virtual ~BombStatusWidget();

    void setUnReign();
    void setReign();
    void setLock();
    void setUnlock();

    bool getSelected() const;
    void setSelected(bool selected);

    const BombData &getBombData() const;

    void initBombData(const BombData &bombData, IUAVNavigationArmShotAloneFireProtDialog* pIUAVNavigationArmShotAloneFireProtDialog, BomBCallBackFunction bomBCallBackFunction);

    bool setBombStatusPixmap(const QPixmap& unReignPixmap, const QPixmap& unReignSelPixmap, const QPixmap& reignPixmap, const QPixmap& reignSelPixmap, const QPixmap& unLockPixmap, const QPixmap& unLockSelPixmap);
    bool setBombStatusMiddlePixmap(const QPixmap& unReignMiddlePixmap, const QPixmap& unReignSelMiddlePixmap, const QPixmap& reignMiddlePixmap, const QPixmap& reignSelMiddlePixmap, const QPixmap& unLockMiddlePixmap, const QPixmap& unLockSelMiddlePixmap);

protected:
    void mousePressEvent(QMouseEvent *ev) override;
    void paintEvent(QPaintEvent *) override;
    void drawFrame(QPainter *);
    void initStyleOption(QStyleOptionFrame *option) const;

private:
    Ui::BombStatusWidget *ui;
    static constexpr unsigned int             m_PixmapSum    = 6;
    static constexpr int                      m_MiddleWidth  = 300;
    static constexpr int                      m_MiddleHeight = 300;
    IUAVNavigationArmShotAloneFireProtDialog* m_IUAVNavigationArmShotAloneFireProtDialog;
    BomBCallBackFunction                      m_bomBCallBackFunction;
    BombData                                  m_bombData;
    BombStatus                                m_bombStatus;
    QRect                                     m_bombStatusRect;
    QRect                                     m_bombStatusRectMiddle;
    bool                                      m_selected;
    bool                                      m_scaledContents;
    QPixmap                                   m_pixmap[m_PixmapSum];
    QPixmap                                   m_pixmapMiddle[m_PixmapSum];
    QPixmap                                   m_scaledPixmap[m_PixmapSum];

};

#endif
