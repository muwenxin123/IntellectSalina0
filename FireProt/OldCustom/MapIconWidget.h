#ifndef MAPICONWIDGET_H
#define MAPICONWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QToolButton>

typedef struct struMenuData MenuData;

class MapIconWidget : public QWidget
{
    Q_OBJECT
	Q_PROPERTY(QString iconPath READ iconPath WRITE setIconPath)
    Q_PROPERTY(QSize mapTopRightSize READ mapTopRightSize WRITE setMapTopRightSize)

public:
    explicit MapIconWidget(QWidget *parent = nullptr);
    virtual ~MapIconWidget();
    void setData(const QList<MenuData> &data);
    void reset();

    QString iconPath() const
    {
        return m_strIconPath;
    }
    void setIconPath(const QString &iconPath);

    QSize mapTopRightSize() const
    {
        return m_mapTopRightSize;
    }
    void setMapTopRightSize(QSize size)
    {
        m_mapTopRightSize = size;
    }

protected:
    void paintEvent(QPaintEvent *event) override;

signals:
    void triggered(QAction *action);
    void fullScreen(bool bFullScreen);
    void switch23D(bool D3);

private slots :
    void buttonClicked(bool);

public:
    QVBoxLayout	m_vLayout;
    QList<QToolButton *> m_p3DTBtn;

private:
    QVector<QToolButton *> m_btns;
    QVector<QToolButton *> m_btgn;
    QString m_strIconPath;
    QSize m_mapTopRightSize;
};

#endif