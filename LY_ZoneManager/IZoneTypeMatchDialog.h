#ifndef IZONETYPEMATCHDIALOG_H
#define IZONETYPEMATCHDIALOG_H

#include <QWidget>
#include <QCheckBox>
#include <QVBoxLayout>
#include "LyDialog.h"

namespace Ui {
class IZoneTypeMatchDialog;
}

class IZoneTypeMatchDialog : public LyDialog
{
    Q_OBJECT

public:
    explicit IZoneTypeMatchDialog(QWidget *parent = nullptr);
    ~IZoneTypeMatchDialog();

    void Init(int row,QString rule,QStringList chkList);

public:
    void on_PbtnOK_Clicked();

    void on_PbtnCancel_Clicked();
public:
    std::vector<QString> m_vSelectChksVec;
private:
	std::map<QString, QStringList> m_AllChks;
	int   m_nrow =-1;
private:
    Ui::IZoneTypeMatchDialog *ui;
};

#endif
