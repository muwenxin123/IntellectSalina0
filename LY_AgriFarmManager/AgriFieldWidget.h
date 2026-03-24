#ifndef AGRIFIELDWIDGET_H_
#define AGRIFIELDWIDGET_H_
 
#include <QWidget>
#include <Protocol/XygsJL/sharedFile.h>

#include "DataManage/AgriFarm/AgriFarmManagerDecl.h"
#include "IAgriFarmManagerActivator.h"
#include "AgriFarmManagerUtil.h"



namespace Ui
{
	class AgriFieldWidget;
}

class AgriFieldWidget : public QWidget
{
	friend class AgriFarmManagerDialog;

	Q_OBJECT

public:
	explicit AgriFieldWidget(QWidget *parent = nullptr);
	~AgriFieldWidget();

public:
	void setCurrentField(const AgriFieldInfo& fieldInfo);
	void getActiveZoneData(qnzkna::ZoneManage::CZoneBase& zoneBase);

protected:
	virtual void changeEvent(QEvent *event);
	virtual void showEvent(QShowEvent *event);
	virtual void hideEvent(QHideEvent *event);

private:
	void InitData();
	void CreatePointListHeader();
	void UpdatePointListData(const qnzkna::ZoneManage::CZoneBase& zoneBase);
	void cleanUp();

private:
	Ui::AgriFieldWidget									*ui;
	bool												m_show;
	AgriFieldInfo										m_fieldInfo;
	qnzkna::ZoneManage::CZoneBase 						m_zoneBase;
};

#endif  // AgriFieldWidget_H