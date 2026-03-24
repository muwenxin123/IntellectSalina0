#include "UAVFlyManage_MavlinkMissionManagerOpenFileDialog.h"
#include "ui_UAVFlyManage_MavlinkMissionManagerOpenFileDialog.h"

#include <QDir>
#include <QDebug>

UAVFlyManage_MavlinkMissionManagerOpenFileDialog::UAVFlyManage_MavlinkMissionManagerOpenFileDialog(QWidget *parent) :
    LyDialog("\xe6\x89\x93\xe5\xbc\x80\xe4\xbb\xbb\xe5\x8a\xa1", parent),
    ui(new Ui::UAVFlyManage_MavlinkMissionManagerOpenFileDialog)
  , m_TaskFileDir("../data/TaskRecord/TaskMav/")
{
    ui->setupUi(centralWidget());
    setResizable(true);

    resize(centralWidget()->size());

    connect(ui->pushButton_MavlinkMissionManagerOpenFileDialog_Open, &QPushButton::clicked, [this](){
        this->accept();
    });

    connect(ui->pushButton_MavlinkMissionManagerOpenFileDialog_Close, &QPushButton::clicked, [this](){
        this->reject();
    });
}

UAVFlyManage_MavlinkMissionManagerOpenFileDialog::~UAVFlyManage_MavlinkMissionManagerOpenFileDialog()
{
    delete ui;
}

void UAVFlyManage_MavlinkMissionManagerOpenFileDialog::changeEvent(QEvent *e)
{
    LyDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void UAVFlyManage_MavlinkMissionManagerOpenFileDialog::showEvent(QShowEvent *event)
{
    if(!ui){
        return ;
    }

    if(!ui->listWidget_MavlinkMissionManagerOpenFileDialog_List){
        return ;
    }
    ui->listWidget_MavlinkMissionManagerOpenFileDialog_List->clear();

    QStringList infoList;
    infoList.append("TaskMav_*");
    QDir dir(m_TaskFileDir);
    QFileInfoList list = dir.entryInfoList(infoList, QDir::Dirs);
    qDebug() << "     Bytes Filename";

    infoList.clear();
    for (int i = 0; i < list.size(); ++i) {
        QFileInfo fileInfo = list.at(i);
        qDebug() << qPrintable(QString("%1 %2").arg(fileInfo.size(), 10)
                               .arg(fileInfo.fileName()));
        infoList.append(fileInfo.baseName());
    }
    ui->listWidget_MavlinkMissionManagerOpenFileDialog_List->addItems(infoList);
}

void UAVFlyManage_MavlinkMissionManagerOpenFileDialog::hideEvent(QHideEvent *event)
{

}

QString UAVFlyManage_MavlinkMissionManagerOpenFileDialog::TaskFileDir() const
{
    auto* currentItem = ui->listWidget_MavlinkMissionManagerOpenFileDialog_List->currentItem();
    if(!currentItem || currentItem->text().isEmpty()){
        return QString("");
    }
    return m_TaskFileDir + currentItem->text() + "/";
}

void UAVFlyManage_MavlinkMissionManagerOpenFileDialog::on_pushButton_MavlinkMissionManagerOpenFileDialog_Open_clicked()
{

}
