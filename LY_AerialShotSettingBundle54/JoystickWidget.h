#ifndef JOYSTICKWIDGET_H
#define JOYSTICKWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QStyle>
#include <QPen>
#include <QMouseEvent>
#include <QTimer>

namespace Ui
{
class JoystickWidget;
}

class JoystickWidget : public QWidget
{
    Q_OBJECT

public:
    explicit JoystickWidget(QWidget *parent = nullptr);
    ~JoystickWidget();

    bool getAxisVisible() const;
    void setAxisVisible(bool value);

    bool getAxisMouse() const;
    void setAxisMouse(bool value);

    void setAxisXValue(const qreal &axisXValue);

    void setAxisYValue(const qreal &axisYValue);

    void updateAxisValue();

    qreal getThrottle() const;
    void setThrottle(const qreal &throttle);

    qreal getYaw() const;
    void setYaw(const qreal &yaw);

signals:
    void joystickAxis(long int lX, long int lY, long int lYoumen = 0, long int lFangwei = 0);
    void mouseReleaseEventSig(long int lX, long int lY, long int lYoumen = 0, long int lFangwei = 0);

public Q_SLOTS:
    void setText(const QString &);

protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseDoubleClickEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void focusInEvent(QFocusEvent *event);
    virtual void focusOutEvent(QFocusEvent *event);
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    virtual void paintEvent(QPaintEvent *event);
    virtual void showEvent(QShowEvent *event);
    virtual void resizeEvent(QResizeEvent *event);

private:
    const QRectF AxisRect() const;
    void resumeAxis();
    void calAxisXYValue();

private:
    Ui::JoystickWidget *ui;
    bool                m_JoystickManageSelfSettings;
    bool                m_InitDirectX;
    QTimer              m_timer;
    static const int    m_margins   = 20;
    static const int    m_axisNum   = 40;
    static const int    m_axisXYMax = 6;
    static const int    m_axisXYMin = m_axisXYMax / 2;
    const qreal         m_minX;
    const qreal         m_maxX;
    const qreal         m_minY;
    const qreal         m_maxY;

    bool                m_axisVisible;
    bool                m_axisMouse;
    bool                m_mousePressed;
    qreal               m_axisX;
    qreal               m_axisY;
    qreal               m_axisXValue;
    qreal               m_axisYValue;
    qreal               m_Throttle;
    qreal               m_Yaw;
};

#endif
