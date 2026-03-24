#include "CoordinateEdit.h"
#include <QCString.h>
#include <QLineEdit>
#include <QLabel>
#include <QHBoxLayout>
#include <QRegExp>
#include <QRegExpValidator>
#include <QKeyEvent>

CoordinateEdit::CoordinateEdit(QWidget *parent, Type type)
    : QWidget(parent)
    , m_type(type)
{
    if (parent)
    {
        parent->setLayout(new QHBoxLayout);
        parent->layout()->addWidget(this);
    }
    m_degree = new QLineEdit;
    m_degree->setObjectName("Degree");
    m_degree->setAlignment(Qt::AlignCenter);
    m_degree->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_minute = new QLineEdit;
    m_minute->setObjectName("Minute");
    m_minute->setAlignment(Qt::AlignCenter);
    m_minute->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_second = new QLineEdit;
    m_second->setObjectName("Second");
    m_second->setAlignment(Qt::AlignCenter);
    m_second->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_msec = new QLineEdit;
    m_msec->setObjectName("Msec");
    m_msec->setAlignment(Qt::AlignCenter);
    m_msec->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(m_degree, &QLineEdit::textEdited, this, &CoordinateEdit::OnLineEdit);
    connect(m_minute, &QLineEdit::textEdited, this, &CoordinateEdit::OnLineEdit);
    connect(m_second, &QLineEdit::textEdited, this, &CoordinateEdit::OnLineEdit);
    connect(m_msec, &QLineEdit::textEdited, this, &CoordinateEdit::OnLineEdit);
    m_dLab = new QLabel(tr2("¡ã"));
    m_mLab = new QLabel(tr2("¡ä"));
    m_sLab = new QLabel(tr2("¡å"));
    QHBoxLayout *blay = new QHBoxLayout(this);
    blay->addWidget(m_degree);
    blay->addWidget(m_dLab);
    blay->addWidget(m_minute);
    blay->addWidget(m_mLab);
    blay->addWidget(m_second);
    blay->addWidget(m_sLab);
    blay->addWidget(m_msec);
    blay->setMargin(0);
    blay->setSpacing(0);
    this->setStyleSheet("QWidget{background:rgba(0,0,0,32);color:white;border:1px solid white;}\
						QLabel{border:0px;}\
						QLineEdit#Degree{boder-left:0px;border-right:0px;}\
						QLineEdit#Minute{boder-left:0px;border-right:0px;}\
						QLineEdit#Second{boder-left:0px;border-right:0px;}\
						QLineEdit#Msec{boder-left:0px;border-right:0px;}");
    if (m_type == Type_Longitude)
    {
        QRegExp dreg("(([-]?[0-9])|([-]?[2-9][0-9])|([-]?[1]([0-7])[0-9]))");
        QRegExpValidator *dvalidator = new QRegExpValidator(dreg, this);
        m_degree->setValidator(dvalidator);
    }
    else if (m_type == Type_Lagitude)
    {
        QRegExp dreg("(([-]?[0-9])|([-]?[1-8][0-9]))");
        QRegExpValidator *dvalidator = new QRegExpValidator(dreg, this);
        m_degree->setValidator(dvalidator);
    }
    QRegExp mreg("(([0-9])|([1-5][0-9]))");
    QRegExpValidator *mvalidator = new QRegExpValidator(mreg, this);
    m_minute->setValidator(mvalidator);
    m_second->setValidator(mvalidator);
    QRegExp msreg("(([0-9])|([0-9][0-9])|([0-9][0-9][0-9]))");
    QRegExpValidator *msvalidator = new QRegExpValidator(msreg, this);
    m_msec->setValidator(msvalidator);
    m_degree->installEventFilter(this);
    m_minute->installEventFilter(this);
    m_second->installEventFilter(this);
    m_msec->installEventFilter(this);

}

CoordinateEdit::~CoordinateEdit()
{
}

double CoordinateEdit::value()
{
    return m_d.toInt() + m_m.toInt() / 60.0 + m_s.toInt() / 3600.0 + m_ms.toInt() / 3600000.0;
}

void CoordinateEdit::setValue(double v)
{
    m_d = QString::asprintf("%d", (int)v);
    int nV = v * 3600000;
    m_ms = QString::asprintf("%d", nV % 1000);
    nV /= 1000;
    m_s = QString::asprintf("%d", nV % 60);
    m_m = QString::asprintf("%d", nV / 60 % 60);
    m_degree->setText(m_d);
    m_minute->setText(m_m);
    m_second->setText(m_s);
    m_msec->setText(m_ms);
}

bool CoordinateEdit::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QLineEdit *edit = (QLineEdit *)watched;
        if (edit == m_degree || edit == m_minute ||
                edit == m_second || edit == m_msec)
        {
            QKeyEvent *key = (QKeyEvent *)event;
            if (key->key() == Qt::Key_Backspace)
            {
                if (edit->text().length() <= 1)
                {
                    this->focusNextPrevChild(false);
                }
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}

void CoordinateEdit::OnLineEdit(const QString &text)
{
    QLineEdit *edit = (QLineEdit *)sender();
    int len = text.length();
    int value = text.toInt();
    if (m_type == Type_Longitude)
    {
        if (edit == m_degree)
        {
            if (value > 0 && len == 3)
            {
                this->focusNextChild();
            }
            else if (value < 0 && len == 4)
            {
                this->focusNextChild();
            }
        }
        else if (edit == m_msec)
        {
            if (len == 3)
            {
                this->focusNextChild();
            }
        }
        else
        {
            if (len == 2)
            {
                this->focusNextChild();
            }
        }
    }
    else
    {
        if (edit == m_degree)
        {
            if (value > 0 && len == 2)
            {
                this->focusNextChild();
            }
            else if (value < 0 && len == 3)
            {
                this->focusNextChild();
            }
        }
        else if (edit == m_msec)
        {
            if (len == 3)
            {
                this->focusNextChild();
            }
        }
        else
        {
            if (len == 2)
            {
                this->focusNextChild();
            }
        }
    }
    m_coordinate = QString(tr2("%1¡ã%2¡ä%3¡å%4"))
                   .arg(m_degree->text()).arg(m_minute->text())
                   .arg(m_second->text()).arg(m_msec->text());
    m_d = m_degree->text();
    m_m = m_minute->text();
    m_s = m_second->text();
    m_ms = m_msec->text();
    emit coordChange(m_coordinate);
}
