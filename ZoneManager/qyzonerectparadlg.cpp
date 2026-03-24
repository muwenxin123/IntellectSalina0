#include "qyzonerectparadlg.h"

#include "LyMessageBox.h"
#include "QCString.h"

QYZoneRectParaDlg::QYZoneRectParaDlg(QUYU_Zone &zone, QWidget *parent, bool isModal): LyDialog(tr2("矩形区域参数"), parent), ui(new Ui::QYZoneRectParaDialog())
{
    ui->setupUi((QDialog *)centralWidget());
    m_type = 0;
    m_zone = zone;

    m_buttonGroup = new QButtonGroup(this);
    m_buttonGroup->addButton(ui->radioButton, 0);
    m_buttonGroup->addButton(ui->radioButton_2, 1);

    connect(ui->pushButton, &QPushButton::clicked, this, &QYZoneRectParaDlg::applyParaToItem);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &QYZoneRectParaDlg::reject);

    connect(ui->radioButton, &QRadioButton::clicked, this, &QYZoneRectParaDlg::OnRadioClickedSlot);
    connect(ui->radioButton_2, &QRadioButton::clicked, this, &QYZoneRectParaDlg::OnRadioClickedSlot);

    setCurrentParaToGui();

}

QYZoneRectParaDlg::~QYZoneRectParaDlg()
{
    delete ui;
}

double QYZoneRectParaDlg::getValue(const QString &str)
{
    double ret = 0;
    int du = 0, fen = 0, miao = 0;
    sscanf(str.toLocal8Bit().data(), "%d.%d'%d\"", &du, &fen, &miao);
    ret = (3600.0 * du + 60 * fen + miao) / 3600;
    return ret;
}

void QYZoneRectParaDlg::applyParaToItem()
{
    if (m_type != 0 && m_type != 1)
    {
        m_type = 0;
    }

    if (ui->lineEdit_4->text().trimmed().toDouble() <= 0 || ui->lineEdit_5->text().trimmed().toDouble() <= 0 || ui->lineEdit_6->text().trimmed().toDouble() < 0)
    {
        LyMessageBox::warning(this, tr2("警告"), tr2("长/宽/圆角半径不得为负值!"));
        return ;
    }

    m_zone.data.h[0].x = getValue(ui->lineEdit->displayText().trimmed());
    m_zone.data.h[0].y = getValue(ui->lineEdit_2->displayText().trimmed());
    m_zone.data.h[1].x = ui->lineEdit_4->text().trimmed().toDouble();
    m_zone.data.h[1].y = ui->lineEdit_5->text().trimmed().toDouble();
    m_zone.data.h[2].x = ui->lineEdit_3->text().trimmed().toDouble();

    if (2 * ui->lineEdit_6->text().trimmed().toDouble() > ui->lineEdit_5->text().trimmed().toDouble() || 2 * ui->lineEdit_6->text().trimmed().toDouble() > ui->lineEdit_4->text().trimmed().toDouble())
    {
        LyMessageBox::warning(this, tr2("警告"), tr2("圆角半径大于二分之一长/宽, 请重新设置!"));
        return;
    }

    m_zone.data.h[2].y = ui->lineEdit_6->text().trimmed().toDouble();
    accept();
}

void QYZoneRectParaDlg::setCurrentParaToGui()
{
    QString str;
    int jwd;
    jwd = (int)(m_zone.data.h[0].x * 3600 + 0.5);
    str = QStringLiteral("%1.%2'%3\"").arg(jwd / 3600, 3, 10, QChar('0')).arg(jwd % 3600 / 60, 2, 10, QChar('0')).arg(jwd % 3600 % 60, 2, 10, QChar('0'));
    ui->lineEdit->setText(str);

    jwd = (int)(m_zone.data.h[0].y * 3600 + 0.5);
    str = QStringLiteral("%1.%2'%3\"").arg(jwd / 3600, 2, 10, QChar('0')).arg(jwd % 3600 / 60, 2, 10, QChar('0')).arg(jwd % 3600 % 60, 2, 10, QChar('0'));
    ui->lineEdit_2->setText(str);

    str = QStringLiteral("%1").arg(m_zone.data.h[2].x);
    ui->lineEdit_3->setText(str);

    str = QStringLiteral("%1").arg(m_zone.data.h[1].x);
    ui->lineEdit_4->setText(str);

    str = QStringLiteral("%1").arg(m_zone.data.h[1].y);
    ui->lineEdit_5->setText(str);

    str = QStringLiteral("%1").arg(m_zone.data.h[2].y);
    ui->lineEdit_6->setText(str);

    m_type = 0;
    if (m_type == 0)
    {
        ui->radioButton->setChecked(true);
    }
    else if (m_type == 1)
    {
        ui->radioButton_2->setChecked(true);
    }
}

void QYZoneRectParaDlg::OnRadioClickedSlot()
{
    m_type = m_buttonGroup->checkedId();
}
