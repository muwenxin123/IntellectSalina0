#ifndef CDLGZONEPARAMETEREDIT_H
#define CDLGZONEPARAMETEREDIT_H

#include <QWidget>
#include "Zone/ZoneManagerInfo.h"
#include "IZoneManageService.h"
#include <QMenu>
#include <QAction>
namespace Ui {
class CDlgZoneParameterEdit;
}

class CDlgZoneParameterEdit : public QWidget
{
    Q_OBJECT

public:
    explicit CDlgZoneParameterEdit(QWidget *parent = nullptr);
    ~CDlgZoneParameterEdit();

public:

	void SetZonebase(const qnzkna::ZoneManage::CZoneBase &stZoneBase,const bool bEdited);
	void SetEdited(bool bEdited);

	void UpdateCtrlData();

	bool isQYMCEmpty();
	qnzkna::ZoneManage::CZoneBase GetZonneBase();

	void CreateTimeListHeader();
	void UpdateTimeListData();

	void CreatePointListHeader();
	void UpdatePointListData();

	void InitTypeCombobox();
	void setGroupHiden();

	void EditMouseMoveJWD(double ll, double bb);
	void EditMouseMoveRadius(int raduis);
	void EditMouseMoveSEAngle(int qsj, int zzj);
	void EditMouseMoveCDRadius(int cbz, int dbz);
	void EditMouseMoveWNRadius(int wbj, int nbj);
	void EditMouseMoveChangKuan(int chang, int kuan);

signals:
	void sign_lyqymc_changed();

public slots:
void on_tw_dj_customContextMenuRequested(const QPoint &pos);

void on_tw_djMenu_delete();

private:

    Ui::CDlgZoneParameterEdit*							ui;

	qnzkna::ZoneManage::CZoneBase						m_stZoneBase;

	qnzkna::ZoneManage::IZoneManageService*				m_pIZoneManageService = nullptr;

	QMenu*												m_treeMenu;

	QAction*											m_deleteAction;
	

};

#endif
