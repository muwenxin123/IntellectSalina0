#ifndef UAVCURRENTSHOTAIMING_H
#define UAVCURRENTSHOTAIMING_H

#include <QObject>
#include <QLabel>
#include <QWidget>

class UAVCurrentShotAiming : public QWidget
{
    Q_OBJECT
public:
    explicit UAVCurrentShotAiming(QWidget *parent = nullptr);
    ~UAVCurrentShotAiming();

    void updateTitleAndYawAndDist(const std::string title, const double yaw, const double dist, bool stop = false);

signals:

public slots:

protected:
    virtual void changeEvent(QEvent *e);
    virtual void paintEvent(QPaintEvent *event);
    virtual void showEvent(QShowEvent *event);
    virtual void hideEvent(QHideEvent *event);

private:
    bool                  m_show;
    QLabel                m_TitleLabel;
    QLabel                m_YawLabel;
    QLabel                m_DistLabel;
    std::string           m_Title;
    double                m_Yaw;
    double                m_Dist;
    bool                  m_Stop;
};

#endif
