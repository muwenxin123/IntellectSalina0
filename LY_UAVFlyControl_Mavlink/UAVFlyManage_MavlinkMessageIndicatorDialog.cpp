#include "UAVFlyManage_MavlinkMessageIndicatorDialog.h"
#include "ui_UAVFlyManage_MavlinkMessageIndicatorDialog.h"

#include <QFileDialog>
#include <QRegExp>

#include "lyUI/LyMessageBox.h"

#include "QGCApplication.h"
#include "QGCPalette.h"
#include "AppSettings.h"

UAVFlyManage_MavlinkMessageIndicatorDialog::UAVFlyManage_MavlinkMessageIndicatorDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UAVFlyManage_MavlinkMessageIndicatorDialog)
{
    ui->setupUi(this);
    ui->pushButton_UAVFlyManage_MavlinkMessageIndicator_Set->setVisible(false);
    ui->pushButton_UAVFlyManage_MavlinkMessageIndicator_Set->hide();
}

UAVFlyManage_MavlinkMessageIndicatorDialog::~UAVFlyManage_MavlinkMessageIndicatorDialog()
{
    delete ui;
}

void UAVFlyManage_MavlinkMessageIndicatorDialog::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void UAVFlyManage_MavlinkMessageIndicatorDialog::showAppMessage(const QString& message, const QString& title)
{
    if(!title.isEmpty()){
        appendAppMessage(title + message);
    }else{
        appendAppMessage(message);
    }
}

void UAVFlyManage_MavlinkMessageIndicatorDialog::showAppnewFormattedMessage(const int severity, const QString& message)
{

    if(!ui){
        return ;
    }

    if(!ui->textBrowser_UAVFlyManage_MavlinkMessageIndicator){
        return ;
    }

    QString style;
    QColor  textColor;
    switch (severity)
    {
    case MAV_SEVERITY_EMERGENCY:
    case MAV_SEVERITY_ALERT:
    case MAV_SEVERITY_CRITICAL:
    case MAV_SEVERITY_ERROR:
    {
        style = QString("<#E>");
        textColor = QGCApplication::qgcPalette()->warningText();

        const auto &oldtextColor = ui->textBrowser_UAVFlyManage_MavlinkMessageIndicator->palette().text().color();
        ui->textBrowser_UAVFlyManage_MavlinkMessageIndicator->setTextColor(textColor);
        ui->textBrowser_UAVFlyManage_MavlinkMessageIndicator->append(message);
        ui->textBrowser_UAVFlyManage_MavlinkMessageIndicator->setTextColor(oldtextColor);
    }
        break;
    case MAV_SEVERITY_NOTICE:
    case MAV_SEVERITY_WARNING:
    {
        style = QString("<#I>");
        textColor = QGCApplication::qgcPalette()->warningText();

        const auto &oldtextColor = ui->textBrowser_UAVFlyManage_MavlinkMessageIndicator->palette().text().color();
        ui->textBrowser_UAVFlyManage_MavlinkMessageIndicator->setTextColor(textColor);
        ui->textBrowser_UAVFlyManage_MavlinkMessageIndicator->append(message);
        ui->textBrowser_UAVFlyManage_MavlinkMessageIndicator->setTextColor(oldtextColor);
    }
        break;
    default:
    {
        style = QString("<#N>");
        textColor = QGCApplication::qgcPalette()->text();
        ui->textBrowser_UAVFlyManage_MavlinkMessageIndicator->append(message);
    }
        break;
    }
}

void UAVFlyManage_MavlinkMessageIndicatorDialog::showRebootAppMessage(const QString &message, const QString &title)
{
    if(!title.isEmpty()){
        appendAppMessage(title + message);
    }else{
        appendAppMessage(message);
    }
}

void UAVFlyManage_MavlinkMessageIndicatorDialog::showCriticalVehicleMessage(const QString &message)
{
    QColor textColor = QGCApplication::qgcPalette()->warningText();
    const auto &oldtextColor = ui->textBrowser_UAVFlyManage_MavlinkMessageIndicator->palette().text().color();
    ui->textBrowser_UAVFlyManage_MavlinkMessageIndicator->setTextColor(textColor);
    appendAppMessage(message);
    ui->textBrowser_UAVFlyManage_MavlinkMessageIndicator->setTextColor(oldtextColor);
}

void UAVFlyManage_MavlinkMessageIndicatorDialog::appendAppMessage(const QString &message)
{
    if(!ui){
        return ;
    }

    if(!ui->textBrowser_UAVFlyManage_MavlinkMessageIndicator){
        return ;
    }

    ui->textBrowser_UAVFlyManage_MavlinkMessageIndicator->append(message);
}

void UAVFlyManage_MavlinkMessageIndicatorDialog::on_pushButton_UAVFlyManage_MavlinkMessageIndicator_Clear_clicked()
{
    if(!ui){
        return ;
    }

    if(!ui->textBrowser_UAVFlyManage_MavlinkMessageIndicator){
        return ;
    }

    ui->textBrowser_UAVFlyManage_MavlinkMessageIndicator->clear();
    ui->textBrowser_UAVFlyManage_MavlinkMessageIndicator->setTextColor(ui->textBrowser_UAVFlyManage_MavlinkMessageIndicator->palette().text().color());
}

void UAVFlyManage_MavlinkMessageIndicatorDialog::on_pushButton_UAVFlyManage_MavlinkMessageIndicator_Save_clicked()
{
    if(!ui){
        return ;
    }

    if(!ui->textBrowser_UAVFlyManage_MavlinkMessageIndicator){
        return ;
    }

    const QString& fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                qApp->applicationDirPath() + "../data/Record/" + QDateTime::currentDateTime().toString("yyyy_MM_dd_HHmmss_zzz") + ".txt",
                                tr("Text (*.txt *.csv)"));

    if(fileName.isEmpty()){
        return ;
    }
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        LyMessageBox::warning(this, tr("Mavlink Message Indicator"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName),
                                  file.errorString()) , QMessageBox::Yes);
        return ;
    }

    QTextStream out(&file);
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    out << ui->textBrowser_UAVFlyManage_MavlinkMessageIndicator->toPlainText();
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
    LyMessageBox::warning(this, tr("Mavlink Message Indicator"),
                         tr("File saved") , QMessageBox::Yes);
}
