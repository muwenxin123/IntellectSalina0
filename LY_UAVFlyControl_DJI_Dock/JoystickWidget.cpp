#include "JoystickWidget.h"
#include "ui_JoystickWidget.h"

#include <QSettings>

#ifdef Q_OS_WIN
#define  DIRECTINPUT_VERSION 0x700
#include <dinput.h>

#include <QWindow>

static JoystickWidget*         g_lpJoystickWidget   = nullptr;
static LPDIRECTINPUT           g_lpDInput           = nullptr ;
static LPDIRECTINPUTDEVICE     g_lpDIDeviceKeyboard = nullptr;
static LPDIRECTINPUTDEVICE2    g_lpDIDeviceJoystick = nullptr ;

BOOL CALLBACK EnumJoy(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
{
    BOOL                 bEnumForce ;
    DIPROPRANGE          dipRange ;
    DIPROPDWORD          dipDWORD ;
    LPDIRECTINPUTDEVICE  lpDIDeviceJoystickTemp ;
    HRESULT              hr ;

    bEnumForce = (BOOL) (pvRef != nullptr);

    if( FAILED(g_lpDInput->CreateDevice(lpddi->guidInstance,
                                        &lpDIDeviceJoystickTemp, NULL)))
    {
        g_lpDIDeviceJoystick = nullptr ;
        return DIENUM_CONTINUE ;
    }

    hr = lpDIDeviceJoystickTemp->QueryInterface(IID_IDirectInputDevice2, (void **) &g_lpDIDeviceJoystick) ;
    lpDIDeviceJoystickTemp->Release() ;
    if( FAILED( hr ) )
    {
        g_lpDIDeviceJoystick = nullptr ;
        return DIENUM_CONTINUE ;
    }

    g_lpDIDeviceJoystick->SetDataFormat(&c_dfDIJoystick) ;

    g_lpDIDeviceJoystick->SetCooperativeLevel((HWND)(g_lpJoystickWidget->winId()), DISCL_EXCLUSIVE | DISCL_BACKGROUND) ;

    dipRange.diph.dwSize       = sizeof(dipRange) ;
    dipRange.diph.dwHeaderSize = sizeof(dipRange.diph) ;
    dipRange.diph.dwObj        = DIJOFS_X ;
    dipRange.diph.dwHow        = DIPH_BYOFFSET ;
    dipRange.lMin              = -100 ;
    dipRange.lMax              = +100 ;
    g_lpDIDeviceJoystick->SetProperty( DIPROP_RANGE, &dipRange.diph) ;

    dipRange.diph.dwSize       = sizeof(dipRange) ;
    dipRange.diph.dwHeaderSize = sizeof(dipRange.diph) ;
    dipRange.diph.dwObj        = DIJOFS_Y ;
    dipRange.diph.dwHow        = DIPH_BYOFFSET ;
    dipRange.lMin              = -100 ;
    dipRange.lMax              = +100 ;
    g_lpDIDeviceJoystick->SetProperty( DIPROP_RANGE, &dipRange.diph) ;

    dipDWORD.diph.dwSize       = sizeof(dipDWORD) ;
    dipDWORD.diph.dwHeaderSize = sizeof(dipDWORD.diph) ;
    dipDWORD.diph.dwObj        = DIJOFS_X ;
    dipDWORD.diph.dwHow        = DIPH_BYOFFSET ;
    dipDWORD.dwData            = 2000 ;
    g_lpDIDeviceJoystick->SetProperty( DIPROP_DEADZONE, &dipDWORD.diph) ;

    dipDWORD.diph.dwSize       = sizeof(dipDWORD) ;
    dipDWORD.diph.dwHeaderSize = sizeof(dipDWORD.diph) ;
    dipDWORD.diph.dwObj        = DIJOFS_Y ;
    dipDWORD.diph.dwHow        = DIPH_BYOFFSET ;
    dipDWORD.dwData            = 2000 ;
    g_lpDIDeviceJoystick->SetProperty( DIPROP_DEADZONE, &dipDWORD.diph) ;

    return DIENUM_STOP ;
}

BOOL InitDirectInput( void )
{

    if (g_lpDInput == nullptr) {
        if (FAILED(CoCreateInstance(CLSID_DirectInput, NULL, CLSCTX_INPROC_SERVER,
                                    IID_IDirectInput, (void **)&g_lpDInput)))
            return FALSE;

        if (FAILED(g_lpDInput->Initialize((HINSTANCE)(g_lpJoystickWidget->winId()), DIRECTINPUT_VERSION)))
            return FALSE;
    }

    if (g_lpDIDeviceKeyboard == nullptr) {
        if (!FAILED(g_lpDInput->CreateDevice(GUID_SysKeyboard,
                                             &g_lpDIDeviceKeyboard, NULL)))
        {
            g_lpDIDeviceKeyboard->SetDataFormat(&c_dfDIKeyboard);

            g_lpDIDeviceKeyboard->SetCooperativeLevel((HWND)(g_lpJoystickWidget->winId()), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
        }
        else
            g_lpDIDeviceKeyboard = nullptr;
    }

    if (g_lpDIDeviceJoystick == nullptr) {
        g_lpDInput->EnumDevices(DIDEVTYPE_JOYSTICK, EnumJoy, (LPVOID)TRUE,
                                DIEDFL_FORCEFEEDBACK | DIEDFL_ATTACHEDONLY);
        if (!g_lpDIDeviceJoystick)
            g_lpDInput->EnumDevices(DIDEVTYPE_JOYSTICK, EnumJoy, (LPVOID)FALSE,
                                    DIEDFL_ATTACHEDONLY);
    }

    return true;
}

BOOL InitDirectX( void )
{

    if (FAILED(CoInitialize(NULL)))
        return FALSE ;

    if (!InitDirectInput()) {
        CoUninitialize();
        return FALSE;
    }

    return TRUE ;
}

BOOL GetKeyboardInput( int *nX, int *nY, BOOL *bButton )
{
    char     cBuffer[256] ;
    HRESULT  hr ;

    if(!g_lpDIDeviceKeyboard)
        return FALSE ;

    *bButton = FALSE ;

    hr = g_lpDIDeviceKeyboard->GetDeviceState(sizeof(cBuffer),(LPVOID)cBuffer) ;
    if(hr== DIERR_INPUTLOST || hr== DIERR_NOTACQUIRED )
    {
        g_lpDIDeviceKeyboard->Acquire() ;
        hr = g_lpDIDeviceKeyboard->GetDeviceState( sizeof(cBuffer), (LPVOID)cBuffer) ;
    }

    if( FAILED(hr) )
        return FALSE ;

    if(cBuffer[DIK_RIGHT]&0x80)
        (*nX)++ ;

    if(cBuffer[DIK_LEFT]&0x80)
        (*nX)-- ;

    if(cBuffer[DIK_UP]&0x80)
        (*nY)-- ;

    if(cBuffer[DIK_DOWN]&0x80)
        (*nY)++ ;

    if(cBuffer[DIK_LCONTROL]&0x80)
        *bButton = TRUE ;

    return TRUE ;

}

BOOL GetJoystickInput( long int *nX, long int *nY, long int *nFangwei, long int *nYoumen )
{
    HRESULT     hr ;
    DIJOYSTATE  diJoyState ;

    if(!g_lpDIDeviceJoystick)
        return FALSE ;

    hr = g_lpDIDeviceJoystick->Poll() ;
    if(hr== DIERR_INPUTLOST || hr== DIERR_NOTACQUIRED )
    {
        g_lpDIDeviceJoystick->Acquire() ;

        hr = g_lpDIDeviceJoystick->Poll() ;
    }
    if( FAILED(hr) )
        return FALSE ;

    if( FAILED(g_lpDIDeviceJoystick->GetDeviceState( sizeof(DIJOYSTATE), & diJoyState)) )
        return FALSE ;

    *nX =  diJoyState.lX ;
    *nY = -diJoyState.lY ;
    *nYoumen  = diJoyState.rglSlider[0];
    *nFangwei = diJoyState.lRz;
    return TRUE ;

}

void UnInitDirectInput( void )
{
    if(g_lpDIDeviceKeyboard)
    {
        g_lpDIDeviceKeyboard->Unacquire() ;
        g_lpDIDeviceKeyboard->Release() ;
    }

    if(g_lpDIDeviceJoystick)
    {
        g_lpDIDeviceJoystick->Unacquire() ;
        g_lpDIDeviceJoystick->Release() ;
    }

    if(g_lpDInput)
        g_lpDInput->Release() ;

    return ;
}

void UnInitDirectX( void )
{
    UnInitDirectInput() ;

    CoUninitialize() ;
}
#endif

JoystickWidget::JoystickWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::JoystickWidget),
    m_JoystickManageSelfSettings(false),
    m_InitDirectX(false),
    m_timer(this),
    m_minX(-100),
    m_maxX(100),
    m_minY(-100),
    m_maxY(100),
    m_axisVisible(false),
    m_axisMouse(false),
    m_mousePressed(false),
    m_axisX(0),
    m_axisY(0),
    m_axisXValue(0),
    m_axisYValue(0),
    m_Throttle(0),
    m_Yaw(0)
{
    ui->setupUi(this);
    g_lpJoystickWidget = this;

    QSettings settings("../data/conf/JoystickManageSettings.ini", QSettings::IniFormat);
    settings.beginGroup("JoystickManageSelfSettings");
    if(settings.contains("JoystickManageSelf")){
        const bool JoystickManageSelfSettings = settings.value("JoystickManageSelf", false).toBool();
        m_JoystickManageSelfSettings = JoystickManageSelfSettings;
    }else{
        m_JoystickManageSelfSettings = false;
    }
    settings.endGroup();

    if(m_JoystickManageSelfSettings){
        m_InitDirectX = InitDirectX();
    }

    m_timer.setInterval(300);

    connect(&m_timer, &QTimer::timeout, [this]()
    {
        if(m_axisVisible && m_JoystickManageSelfSettings && m_InitDirectX)
        {
            long int nX = 0, nY = 0, nFangWei = 0, nYouMen = 0;
            bool result = GetJoystickInput(&nX, &nY, &nFangWei, &nYouMen);
            if(result)
            {
                m_axisXValue = nX;
                m_axisYValue = nY;

                const QRectF& rect = AxisRect();
                const qreal   midX = rect.width()  / 2.0 + rect.x();
                const qreal   midY = rect.height() / 2.0 + rect.y();

                if(rect.width() != 0.0 && m_maxX != 0.0 && rect.height() != 0.0 && m_maxY != 0.0){
                    m_axisX = m_axisXValue / ( m_maxX * 2 / rect.width()) + midX;
                    m_axisY = midY - m_axisYValue / ( m_maxY * 2 / rect.height());
                    update();
                }

                long int lX = m_axisXValue;
                long int lY = m_axisYValue;
                emit joystickAxis(lX, lY, nYouMen, nFangWei);
            }
        } else if(m_axisMouse){
            if(m_axisMouse && m_mousePressed){
                long int lX = m_axisXValue;
                long int lY = m_axisYValue;
                if(lX != 0 && lY != 0){
                    emit joystickAxis(lX, lY);
                }
            }
        }else{
            m_timer.stop();
        }
    });
}

JoystickWidget::~JoystickWidget()
{
    m_timer.stop();
    if(m_JoystickManageSelfSettings && m_InitDirectX){
        UnInitDirectX();
    }
    delete ui;
}

bool JoystickWidget::getAxisVisible() const
{
    return m_axisVisible;
}

void JoystickWidget::setAxisVisible(bool value)
{
    m_axisMouse   = false;
    m_axisVisible = value;
    if(m_axisVisible){
        if (m_JoystickManageSelfSettings && m_InitDirectX == false) {
            m_InitDirectX = InitDirectX();
        }
        if(!m_timer.isActive()){
            m_timer.start();
        }
    }else{
        m_timer.stop();
    }
}

bool JoystickWidget::getAxisMouse() const
{
    return m_axisMouse;
}

void JoystickWidget::setAxisMouse(bool value)
{
    m_axisVisible = false;
    m_axisMouse   = value;
    if(m_axisMouse){
        if(!m_timer.isActive()){
            m_timer.start();
        }
    }else{
        m_timer.stop();
    }
}

void JoystickWidget::setText(const QString &)
{

}

void JoystickWidget::mousePressEvent(QMouseEvent *event)
{
    if(m_axisMouse){
        if(AxisRect().contains(event->pos())){
            m_mousePressed = true;
            const QRectF& rect = AxisRect();
            if(rect.contains(event->pos()))
            {
                const qreal midX = rect.width()  / 2.0 + rect.x();
                const qreal midY = rect.height() / 2.0 + rect.y();

                m_axisX = event->x();
                m_axisY = event->y();

                m_axisXValue = (m_axisX - midX)  * ( m_maxX * 2 / rect.width());
                m_axisYValue = (midY  - m_axisY) * ( m_maxY * 2 / rect.height());

                long int lX = m_axisXValue;
                long int lY = m_axisYValue;
                emit mouseReleaseEventSig(lX, lY);
                update();
            }
        }
    }
    QWidget::mousePressEvent(event);
}

void JoystickWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if(m_axisMouse){
        if(m_mousePressed){
            const QRectF& rect = AxisRect();
            if(rect.contains(event->pos()))
            {
                const qreal midX = rect.width()  / 2.0 + rect.x();
                const qreal midY = rect.height() / 2.0 + rect.y();

                m_axisX = event->x();
                m_axisY = event->y();

                m_axisXValue = (m_axisX - midX)  * ( m_maxX * 2 / rect.width());
                m_axisYValue = (midY  - m_axisY) * ( m_maxY * 2 / rect.height());

                long int lX = m_axisXValue;
                long int lY = m_axisYValue;
                emit mouseReleaseEventSig(lX, lY);
                update();
            }
        }

        resumeAxis();
    }

    QWidget::mouseReleaseEvent(event);
}

void JoystickWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    QWidget::mouseDoubleClickEvent(event);
}

void JoystickWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(m_axisMouse && m_mousePressed)
    {
        const QRectF& rect = AxisRect();
        if(rect.contains(event->pos()))
        {
            const qreal midX = rect.width()  / 2.0 + rect.x();
            const qreal midY = rect.height() / 2.0 + rect.y();

            m_axisX = event->x();
            m_axisY = event->y();

            m_axisXValue = (m_axisX - midX)  * ( m_maxX * 2 / rect.width());
            m_axisYValue = (midY  - m_axisY) * ( m_maxY * 2 / rect.height());

            long int lX = m_axisXValue;
            long int lY = m_axisYValue;
            emit joystickAxis(lX, lY);
            update();
        }
    }

    QWidget::mouseMoveEvent(event);
}

void JoystickWidget::focusInEvent(QFocusEvent *event)
{
    QWidget::focusInEvent(event);
}

void JoystickWidget::focusOutEvent(QFocusEvent *event)
{
    if(m_axisMouse){
        resumeAxis();
    }

    QWidget::focusOutEvent(event);
}
void JoystickWidget::enterEvent(QEvent *event)
{
    QWidget::enterEvent(event);
}

void JoystickWidget::leaveEvent(QEvent *event)
{
    if(m_axisMouse){
        resumeAxis();
    }

    QWidget::leaveEvent(event);
}

void JoystickWidget::paintEvent(QPaintEvent *event)
{
    QFont           font = this->font();
    QBrush         brush = this->palette().text();
    const QRectF    rect = AxisRect();
    const qreal        x = rect.x();
    const qreal        y = rect.y();

    const qreal     midX = rect.width()  / 2.0 + x;
    const qreal     midY = rect.height() / 2.0 + y;
    const qreal penWidth = 1.0;

    QPainter painter(this);
    QPen     pen(brush, penWidth);
    painter.setBrush(brush);
    painter.setPen(pen);
    if(font.pointSize() != -1){
        font.setPointSize(lround(font.pointSize() * 7.0 / 8));
    }else if(font.pixelSize() != -1){
        font.setPixelSize(lround(font.pixelSize() * 7.0 / 8));
    }
    painter.setFont(font);

    int fontWidth = painter.font().pointSize();
    if(fontWidth == -1){
        fontWidth = font.pixelSize();
    }

    QLineF lineX(x, midY, x + rect.width() + m_margins, midY);
    painter.drawLine(lineX);
    const QPointF pointsX[4] = {
        QPointF(x + rect.width() + m_margins,     midY),
        QPointF(x + rect.width() + m_margins / 2, midY - m_margins / 4),
        QPointF(x + rect.width() + m_margins / 2, midY + m_margins / 4),
        QPointF(x + rect.width() + m_margins,     midY)
    };
    painter.drawConvexPolygon(pointsX, 4);
    painter.drawText(QPointF(x + rect.width() + m_margins / 2, midY + m_margins / 4 + fontWidth), QString("X"));

    QLineF lineY(midX, y - m_margins, midX, y + rect.height());
    painter.drawLine(lineY);
    const QPointF pointsY[4] = {
        QPointF(midX,                 y - m_margins),
        QPointF(midX - m_margins / 4, y - m_margins / 2),
        QPointF(midX + m_margins / 4, y - m_margins / 2),
        QPointF(midX,                 y - m_margins)
    };
    painter.drawConvexPolygon(pointsY, 4);
    painter.drawText(QPointF(midX - m_margins / 4 - fontWidth, y - m_margins / 2), QString("Y"));

    const qreal xDist  = rect.width() / m_axisNum;
    for(int i = 0; i <= m_axisNum / 2; i++){
        QLineF lineXRight;
        if(i % 2 == 0){
            lineXRight.setLine(midX + xDist * i, midY, midX + xDist * i, midY - m_axisXYMax);
            if(i > 0 && i % 5 == 0){
                painter.drawText(QPointF(midX + xDist * i - fontWidth, midY + fontWidth), QString::number(i * 5));
            }
        }else{
            lineXRight.setLine(midX + xDist * i, midY, midX + xDist * i, midY - m_axisXYMin);
        }

        QLineF lineXLeft;
        if(i % 2 == 0){
            lineXLeft.setLine(midX - xDist * i, midY, midX - xDist * i, midY - m_axisXYMax);
            if(i > 0 && i % 5 == 0){
                painter.drawText(QPointF(midX - xDist * i - fontWidth, midY + fontWidth), QString::number(-i * 5));
            }
        }else{
            lineXLeft.setLine(midX - xDist * i, midY, midX - xDist * i, midY - m_axisXYMin);
        }

        painter.drawLine(lineXRight);
        painter.drawLine(lineXLeft);
    }

    const qreal yDist  = rect.height() / m_axisNum;
    for(int i = 0; i <= m_axisNum / 2; i++){
        QLineF lineYtop;
        if(i % 2 == 0){
            lineYtop.setLine(midX, midY - yDist * i, midX + m_axisXYMax, midY - yDist * i);
            if(i > 0 && i % 5 == 0){
                painter.drawText(QPointF(midX - fontWidth * 2, midY - yDist * i + m_axisXYMin), QString::number(i * 5));
            }
        }else{
            lineYtop.setLine(midX, midY - yDist * i, midX + m_axisXYMin,  midY - yDist * i);
        }

        QLineF lineYBottom;
        if(i % 2 == 0){
            lineYBottom.setLine(midX, midY + yDist * i, midX + m_axisXYMax, midY + yDist * i);
            if(i > 0 && i % 5 == 0){
                painter.drawText(QPointF(midX - fontWidth * 2.3, midY + yDist * i + m_axisXYMin), QString::number(-i * 5));
            }
        }else{
            lineYBottom.setLine(midX, midY + yDist * i, midX + m_axisXYMin,  midY + yDist * i);
        }
        painter.drawLine(lineYtop);
        painter.drawLine(lineYBottom);
    }

    QPen penVH(Qt::GlobalColor::green);
    painter.setPen(penVH);

    QPointF point(m_axisX, m_axisY);
    QLineF  hLine(point.x() - m_margins / 2, point.y(),                 point.x() + m_margins / 2, point.y());
    QLineF  vLine(point.x(),                 point.y() - m_margins / 2, point.x(),                 point.y() + m_margins / 2);
    painter.drawLine(hLine);
    painter.drawLine(vLine);

    painter.drawText(QPointF(x - m_margins, y - m_margins), QString::asprintf("X:%.0f, Y:%.0f", m_axisXValue, m_axisYValue));

    QWidget::paintEvent(event);
}

void JoystickWidget::showEvent(QShowEvent *event)
{
    calAxisXYValue();
    QWidget::showEvent(event);
}

void JoystickWidget::resizeEvent(QResizeEvent *event)
{

    calAxisXYValue();
    QWidget::resizeEvent(event);
}

const QRectF JoystickWidget::AxisRect() const
{
    return QRectF( QPointF(this->geometry().left()  + m_margins / 2, this->geometry().top()    + m_margins)
                   ,QPointF(this->geometry().right() - m_margins * 2, this->geometry().bottom() - m_margins));
}

void JoystickWidget::resumeAxis()
{
    if(m_mousePressed){
        m_mousePressed = false;
        const QRectF rect = AxisRect();
        m_axisX = rect.width()  / 2.0 + rect.x();
        m_axisY = rect.height() / 2.0 + rect.y();

        calAxisXYValue();

        update();

        emit joystickAxis(0, 0);
    }
}

void JoystickWidget::calAxisXYValue()
{
    const QRectF rect = AxisRect();
    m_axisX = rect.width()  / 2.0 + rect.x();
    m_axisY = rect.height() / 2.0 + rect.y();

    const qreal midX = rect.width()  / 2.0 + rect.x();
    const qreal midY = rect.height() / 2.0 + rect.y();
    m_axisXValue = (m_axisX - midX)  * ( m_maxX * 2 / rect.width());
    m_axisYValue = (midY  - m_axisY) * ( m_maxY * 2 / rect.height());
}

qreal JoystickWidget::getYaw() const
{
    return m_Yaw;
}

void JoystickWidget::setYaw(const qreal &yaw)
{
    m_Yaw = yaw;
}

qreal JoystickWidget::getThrottle() const
{
    return m_Throttle;
}

void JoystickWidget::setThrottle(const qreal &throttle)
{
    m_Throttle = throttle;
}

void JoystickWidget::setAxisYValue(const qreal &axisYValue)
{
    m_axisYValue = axisYValue;
}

void JoystickWidget::setAxisXValue(const qreal &axisXValue)
{
    m_axisXValue = axisXValue;
}

void JoystickWidget::updateAxisValue()
{
    if(!m_JoystickManageSelfSettings && m_axisVisible){
        const QRectF& rect = AxisRect();
        const qreal   midX = rect.width()  / 2.0 + rect.x();
        const qreal   midY = rect.height() / 2.0 + rect.y();
        if(rect.width() != 0.0 && m_maxX != 0.0 && rect.height() != 0.0 && m_maxY != 0.0){
            m_axisX = m_axisXValue / ( m_maxX * 2 / rect.width()) + midX;
            m_axisY = midY - m_axisYValue / ( m_maxY * 2 / rect.height());
            update();
        }
    }

    if(!m_JoystickManageSelfSettings && m_axisVisible)
    {
        long int lX       = m_axisXValue;
        long int lY       = m_axisYValue;
        long int nFangWei = m_Yaw;
        long int nYouMen  = m_Throttle;
        emit joystickAxis(lX, lY, nYouMen, nFangWei);
    }
}

