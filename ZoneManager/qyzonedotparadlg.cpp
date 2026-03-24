#include "qyzonedotparadlg.h"
#include "LyMessageBox.h"
#include "QCString.h"
#include "ui_qyzonedotparadialog.h"

QYZoneDotParaDlg::QYZoneDotParaDlg(int index, QUYU_Zone &zone, QWidget *parent, bool isModal): LyDialog(tr2("区域点参数"), parent), ui(new Ui::QYZoneDotParaDialog())
{
    ui->setupUi((QDialog *)centralWidget());

    m_Zone = zone;
    m_iParaIndex = index;

    m_iInnerRadius = 0;
    m_iOuterRadius = 0;
    m_iStartAngle = 0;
    m_iEndAngle = 0;

    connect(ui->pushButton, &QPushButton::clicked, this, &QYZoneDotParaDlg::OnAdd);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &QYZoneDotParaDlg::OnDelete);
    connect(ui->pushButton_3, &QPushButton::clicked, this, &QYZoneDotParaDlg::applyParaToItem);
    connect(ui->pushButton_4, &QPushButton::clicked, this, &QYZoneDotParaDlg::reject);

    initUI();
}

QYZoneDotParaDlg::~QYZoneDotParaDlg()
{
    delete ui;
}

void QYZoneDotParaDlg::initUI()
{
    if (m_iParaIndex >= MAX_ky_point || m_iParaIndex < 0)
    {
        return ;
    }

    resetParaCtrls();
    if (m_iParaIndex < m_Zone.data.n)
    {
        QString str;
        int jwd;
        jwd = (int)(m_Zone.data.h[m_iParaIndex].x * 3600 + 0.5);
        str = QStringLiteral("%1.%2'%3\"").arg(jwd / 3600, 3, 10, QChar('0')).arg(jwd % 3600 / 60, 2, 10, QChar('0')).arg(jwd % 3600 % 60, 2, 10, QChar('0'));
        ui->lineEdit->setText(str);

        jwd = (int)(m_Zone.data.h[m_iParaIndex].y * 3600 + 0.5);
        str = QStringLiteral("%1.%2'%3\"").arg(jwd / 3600, 2, 10, QChar('0')).arg(jwd % 3600 / 60, 2, 10, QChar('0')).arg(jwd % 3600 % 60, 2, 10, QChar('0'));
        ui->lineEdit_2->setText(str);

        switch (m_Zone.type)
        {
        case CIRCLE:
        {
            ui->lineEdit_6->setEnabled(true);
            ui->label_6->setText(tr2("半径"));
            str = QStringLiteral("%1").arg(m_Zone.data.h[2].x);
            ui->lineEdit_6->setText(str);
            break;
        }
        case PIE:
        {
            ui->lineEdit_6->setEnabled(true);
            ui->label_6->setText(tr2("半径"));
            str = QStringLiteral("%1").arg(m_Zone.data.h[2].x);
            ui->lineEdit_6->setText(str);

            ui->lineEdit_4->setEnabled(true);
            str = QStringLiteral("%1").arg(m_Zone.data.h[1].x);
            ui->lineEdit_4->setText(str);

            ui->lineEdit_5->setEnabled(true);
            str = QStringLiteral("%1").arg(m_Zone.data.h[1].y);
            ui->lineEdit_5->setText(str);
            break;
        }
        case ELLIPSE:
        {
            ui->lineEdit_6->setEnabled(true);
            ui->label_6->setText(tr2("长轴半径"));
            str = QStringLiteral("%1").arg(m_Zone.data.h[2].x);
            ui->lineEdit_6->setText(str);

            ui->lineEdit_3->setEnabled(true);
            ui->label_3->setText(tr2("短轴半径"));
            str = QStringLiteral("%1").arg(m_Zone.data.h[2].y);
            ui->lineEdit_3->setText(str);

            ui->lineEdit_4->setEnabled(true);
            ui->label_4->setText(tr2("长轴与正北夹角"));
            str = QStringLiteral("%1").arg(m_Zone.data.h[1].x);
            ui->lineEdit_4->setText(str);
            break;
        }
        case POLY:
        case LINE:
        {
            str = tr2("区域端点 %1 维护").arg(m_iParaIndex + 1);
            setWindowTitle(str);

            ui->pushButton->setEnabled(true);
            if (m_Zone.data.n > 3)
            {
                ui->pushButton_2->setEnabled(true);
            }
            break;
        }
        case CIRCLERING:
        {
            ui->lineEdit_6->setEnabled(true);
            str = QStringLiteral("%1").arg(m_Zone.data.h[2].x);
            ui->lineEdit_6->setText(str);

            ui->lineEdit_3->setEnabled(true);
            str = QStringLiteral("%1").arg(m_Zone.data.h[2].y);
            ui->lineEdit_3->setText(str);
            break;
        }
        case PIERING:
        {
            ui->lineEdit_6->setEnabled(true);
            str = QStringLiteral("%1").arg(m_Zone.data.h[2].x);
            ui->lineEdit_6->setText(str);

            ui->lineEdit_3->setEnabled(true);
            str = QStringLiteral("%1").arg(m_Zone.data.h[2].y);
            ui->lineEdit_3->setText(str);

            ui->lineEdit_4->setEnabled(true);
            str = QStringLiteral("%1").arg(m_Zone.data.h[1].x);
            ui->lineEdit_4->setText(str);

            ui->lineEdit_5->setEnabled(true);
            str = QStringLiteral("%1").arg(m_Zone.data.h[1].y);
            ui->lineEdit_5->setText(str);
            break;
        }
        }
    }
}

double QYZoneDotParaDlg::getValue(const QString &str)
{
    double ret = 0;
    int du = 0, fen = 0, miao = 0;
    sscanf(str.toLocal8Bit().data(), "%d.%d'%d\"", &du, &fen, &miao);
    ret = (3600.0 * du + 60 * fen + miao) / 3600;
    return ret;
}

void QYZoneDotParaDlg::applyParaToItem()
{
    if (m_iParaIndex < m_Zone.data.n)
    {
        m_Zone.data.h[m_iParaIndex].x = getValue(ui->lineEdit->displayText().trimmed());
        m_Zone.data.h[m_iParaIndex].y = getValue(ui->lineEdit_2->displayText().trimmed());
        switch (m_Zone.type)
        {
        case CIRCLE:
        {
            m_iOuterRadius = ui->lineEdit_6->text().trimmed().toInt();
            if (m_iOuterRadius < 0 || m_iOuterRadius > 5000)
            {
                LyMessageBox::warning(this, tr2("警告"), tr2("半径请输入0-5000之间的数!"));
                return;
            }
            m_Zone.data.h[2].x = m_iOuterRadius;
            break;
        }
        case PIE:
        {
            m_iOuterRadius = ui->lineEdit_6->text().trimmed().toInt();
            m_iStartAngle = ui->lineEdit_4->text().trimmed().toInt();
            m_iEndAngle = ui->lineEdit_5->text().trimmed().toInt();

            if (m_iOuterRadius < 0 || m_iOuterRadius > 5000)
            {
                LyMessageBox::warning(this, tr2("警告"), tr2("半径请输入0-5000之间的数!"));
                return;
            }
            if (m_iStartAngle < 0 || m_iStartAngle > 360 || m_iEndAngle < 0 || m_iEndAngle > 360)
            {
                LyMessageBox::warning(this, tr2("警告"), tr2("角度请输入0-360之间的数!"));
                return;
            }

            m_Zone.data.h[2].x = m_iOuterRadius;
            m_Zone.data.h[1].x = m_iStartAngle;
            m_Zone.data.h[1].y = m_iEndAngle;
            break;
        }
        case ELLIPSE:
        {
            m_iOuterRadius = ui->lineEdit_6->text().trimmed().toInt();
            m_iInnerRadius = ui->lineEdit_3->text().trimmed().toInt();
            m_iStartAngle = ui->lineEdit_4->text().trimmed().toInt();

            if (m_iOuterRadius < 0 || m_iOuterRadius > 5000 || m_iInnerRadius < 0 || m_iInnerRadius > 5000)
            {
                LyMessageBox::warning(this, tr2("警告"), tr2("半径请输入0-5000之间的数!"));
                return;
            }

            if (m_iStartAngle < 0 || m_iStartAngle > 360)
            {
                LyMessageBox::warning(this, tr2("警告"), tr2("角度请输入0-360之间的数!"));
                return;
            }

            m_Zone.data.h[2].x = m_iOuterRadius;
            m_Zone.data.h[2].y = m_iInnerRadius;
            m_Zone.data.h[1].x = m_iStartAngle;
            break;
        }
        case CIRCLERING:
        {
            m_iOuterRadius = ui->lineEdit_6->text().trimmed().toInt();
            m_iInnerRadius = ui->lineEdit_3->text().trimmed().toInt();

            if (m_iOuterRadius < 0 || m_iOuterRadius > 5000 || m_iInnerRadius < 0 || m_iInnerRadius > 5000)
            {
                LyMessageBox::warning(this, tr2("警告"), tr2("半径请输入0-5000之间的数!"));
                return;
            }

            m_Zone.data.h[2].x = m_iOuterRadius;
            m_Zone.data.h[2].y = m_iInnerRadius;
            break;
        }
        case PIERING:
        {
            m_iOuterRadius = ui->lineEdit_6->text().trimmed().toInt();
            m_iInnerRadius = ui->lineEdit_3->text().trimmed().toInt();
            m_iStartAngle = ui->lineEdit_4->text().trimmed().toInt();
            m_iEndAngle = ui->lineEdit_5->text().trimmed().toInt();

            if (m_iOuterRadius < 0 || m_iOuterRadius > 5000 || m_iInnerRadius < 0 || m_iInnerRadius > 5000)
            {
                LyMessageBox::warning(this, tr2("警告"), tr2("半径请输入0-5000之间的数!"));
                return;
            }
            if (m_iStartAngle < 0 || m_iStartAngle > 360 || m_iEndAngle < 0 || m_iEndAngle > 360)
            {
                LyMessageBox::warning(this, tr2("警告"), tr2("角度请输入0-360之间的数!"));
                return;
            }

            m_Zone.data.h[2].x = m_iOuterRadius;
            m_Zone.data.h[2].y = m_iInnerRadius;
            m_Zone.data.h[1].x = m_iStartAngle;
            m_Zone.data.h[1].y = m_iEndAngle;
            break;
        }
        }
    }
    accept();
}

void QYZoneDotParaDlg::OnAdd()
{
    if (m_Zone.data.n >= MAX_ky_point)
    {
        LyMessageBox::warning(this, tr2("警告"), tr2("区域端点数不能超过30个！"));
        return;
    }
    if (m_iParaIndex < m_Zone.data.n)
    {
        for (int i = m_Zone.data.n; i >= m_iParaIndex; i--)
        {
            m_Zone.data.h[i + 1] = m_Zone.data.h[i];
        }

        m_Zone.data.h[m_iParaIndex].x = getValue(ui->lineEdit->displayText().trimmed());
        m_Zone.data.h[m_iParaIndex].y = getValue(ui->lineEdit_2->displayText().trimmed());
        m_Zone.data.n++;
    }
    accept();
}

void QYZoneDotParaDlg::OnDelete()
{
    if (m_iParaIndex < m_Zone.data.n)
    {
        for (int i = m_iParaIndex; i < m_Zone.data.n; i++)
        {
            m_Zone.data.h[i] = m_Zone.data.h[i + 1];
        }
        m_Zone.data.n--;
    }
    accept();
}

void QYZoneDotParaDlg::resetParaCtrls()
{
    setWindowTitle(tr2("区域参数修改"));
    ui->lineEdit_3->setEnabled(false);
    ui->lineEdit_4->setEnabled(false);
    ui->lineEdit_5->setEnabled(false);
    ui->lineEdit_6->setEnabled(false);

    ui->pushButton->setEnabled(false);
    ui->pushButton_2->setEnabled(false);

    ui->label_3->setText(tr2("内半径"));
    ui->label_4->setText(tr2("起始角度"));
    ui->label_5->setText(tr2("结束角度"));
    ui->label_6->setText(tr2("外半径"));
}
