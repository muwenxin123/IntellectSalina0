#ifndef CDLGOPENFILE_H
#define CDLGOPENFILE_H

#include <QWidget>
#include "LyDialog.h"
#include "DataManage/INebulaTaskManageService.h"

namespace Ui
{
class CDlgOpenFile;
}

class CDlgOpenFile : public LyDialog
{
    Q_OBJECT

public:
    explicit CDlgOpenFile(QWidget *parent = nullptr);
    ~CDlgOpenFile();

signals:
    void openpushButton();

protected:

    virtual void changeEvent(QEvent *event);

private slots:

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::CDlgOpenFile *ui;
    qnzkna::TaskManage::INebulaTaskManageService::NebulaTaskFileNameList	nameList;
    QString																	m_strSelFileName;
private:
    void showEvent(QShowEvent *event);
    void UpdateListData();

public:
    QString	GetOpenFileName();
    bool OpenFile(QString strFileName);
};

#endif
