#ifndef AGRIMACHINEMANAGERDIALOG_H_
#define AGRIMACHINEMANAGERDIALOG_H_
 
#include <QWidget>
#include <Protocol/XygsJL/sharedFile.h>

#include "DataManage/AgriFarm/AgriFarmManagerDecl.h"

#include "IProj.h"
namespace Ui
{
	class AgriFarmManagerDialog;
}

class AgriAddFarmDialog;
class AgriFarmManagerDialog : public QWidget
{
	Q_OBJECT

public:
	explicit AgriFarmManagerDialog(QWidget *parent = nullptr);
	~AgriFarmManagerDialog();

	AgriAddFarmDialog* getAddFarmDialog();

	void ZoneHighLight_Tree(QPainter &painter, IProj *pProj);
protected:
	virtual void changeEvent(QEvent *event);
	virtual void showEvent(QShowEvent *event);
	virtual void hideEvent(QHideEvent *event);

private:
	void updateFarmBasicInfo();

private:
	Ui::AgriFarmManagerDialog							*ui;
	bool												m_show;
	bool												m_isUpdatingComboBox;
	std::map<int, AgriFarmInfoPtrVec>					m_vAgriFarmInfoMap;	// <pid, vector>
};

#endif  // AgriFarmManagerDialog_H