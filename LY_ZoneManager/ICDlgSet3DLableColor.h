#ifndef ICDLGSET3DLABLECOLOR_H
#define ICDLGSET3DLABLECOLOR_H

#include <QWidget>
#include "LyDialog.h"
#include "IZoneManageService.h"

namespace Ui {
class ICDlgSet3DLableColor;
}

class ICDlgSet3DLableColor : public LyDialog
{
    Q_OBJECT

public:
    explicit ICDlgSet3DLableColor(QWidget *parent = nullptr);
    ~ICDlgSet3DLableColor();
	void Init();

public:
	std::vector<std::vector<int>> m_vDefaultTRGB;
	std::vector<std::vector<int>> m_vUpdateTRGB;
	std::map<std::vector<std::vector<int>>, bool>        m_SetColorMap;
	qnzkna::ZoneManage::IZoneManageService*					m_pIZoneManageService = nullptr;
public slots:
void on_btnOK_clicked();
void on_btnCencel_clicked();
void on_btnDefault_clicked();

private:
    Ui::ICDlgSet3DLableColor *ui;
};

#endif
