#ifndef CDLGMARKMERGE_H
#define CDLGMARKMERGE_H

#include <QWidget>
#include "ui_CDlgMarkMerge.h"
#include "lyUI/LyDialog.h"
#include "DataManage/Mark/Mark.h"
#include "Protocol/XygsZC/Bag_XygsZCDefines.h"

namespace Ui
{
class CDlgMarkMerge;
}

class CDlgMarkMerge : public LyDialog
{
    Q_OBJECT

public:
    CDlgMarkMerge(QWidget *parent = Q_NULLPTR);
    ~CDlgMarkMerge();

    void InitComBox(const std::vector<QString> MarkNames);
    void setMergeCurIndex(int index);
    void setMergedCurIndex(int index);
    void ShowTop();

private:
    Ui::CDlgMarkMerge *ui;

private slots:

    void on_pushBEnsure_clicked();
    void on_pushBCancel_clicked();

signals:
    void mergeIndexAndName(int mergeIndex, QString mergeName, int mergedIndex, QString mergedName);
};

#endif