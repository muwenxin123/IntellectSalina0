#include "CDlgMarkMerge.h"
#include "QCString.h"
#include "IMarkManageImpl.h"
#include "lyUI/LyMessageBox.h"
#include "LY_MarkManageService.h"

CDlgMarkMerge::CDlgMarkMerge(QWidget *parent) :
    LyDialog(tr("Merge Mark"), parent),
    ui(new Ui::CDlgMarkMerge)
{
    ui->setupUi(centralWidget());
    this->setResizable(true);
    this->resize(200, 200);

    connect(ui->pb_Ensure, &QPushButton::clicked, this, &CDlgMarkMerge::on_pushBEnsure_clicked);
    connect(ui->pb_Cancel, &QPushButton::clicked, this, &CDlgMarkMerge::on_pushBCancel_clicked);

}

CDlgMarkMerge::~CDlgMarkMerge()
{
}

void CDlgMarkMerge::InitComBox(const std::vector<QString> MarkNames)
{
    ui->cb_Merge->clear();
    ui->cb_Merged->clear();
    for (int i = 0; i < MarkNames.size(); i++)
    {
        ui->cb_Merge->addItem(MarkNames[i]);
        ui->cb_Merged->addItem(MarkNames[i]);
    }
}

void CDlgMarkMerge::setMergeCurIndex(int index)
{
    int count = ui->cb_Merge->count();
    index = index < count ? index : 0;
    ui->cb_Merge->setCurrentIndex(index);

}

void CDlgMarkMerge::setMergedCurIndex(int index)
{
    index = index < ui->cb_Merged->count() ? index : 0;
    ui->cb_Merged->setCurrentIndex(index);
}

void CDlgMarkMerge::ShowTop()
{
    this->show();
}

void CDlgMarkMerge::on_pushBEnsure_clicked()
{
    QString mergeQStr = ui->cb_Merge->currentText();
    QString mergedQStr = ui->cb_Merged->currentText();
    int mergeIndex = ui->cb_Merge->currentIndex();
    int mergedIndex = ui->cb_Merged->currentIndex();

    if (mergeIndex == mergedIndex)
    {
        LyMessageBox::warning(this, tr("Prompt"), tr("The target is the same as the merged target!"), QMessageBox::Ok);
        return;
    }
    emit mergeIndexAndName(mergeIndex, mergeQStr, mergedIndex, mergedQStr);
    this->hide();
}

void CDlgMarkMerge::on_pushBCancel_clicked()
{
    this->hide();
}

