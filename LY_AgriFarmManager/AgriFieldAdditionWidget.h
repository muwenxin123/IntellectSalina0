#ifndef AGRIFIELDADDITIONWIDGET_H_
#define AGRIFIELDADDITIONWIDGET_H_
 
#include <QWidget>
#include <QTableWidget>
#include <Protocol/XygsJL/sharedFile.h>

#include "DataManage/AgriFarm/AgriFarmManagerDecl.h"
#include "IAgriFarmManagerActivator.h"


namespace Ui
{
	class AgriFieldAdditionWidget;
}


class AgriFieldAdditionWidget : public QWidget
{
	friend class AgriFarmManagerDialog;

	Q_OBJECT

public:
	explicit AgriFieldAdditionWidget(QWidget *parent = nullptr);
	~AgriFieldAdditionWidget();

public:
	void setCurrentField(const AgriFieldInfoPtr& fieldInfo);
	void createSensorTableHeader();

private:
	void onTabBarClicked(int index);
	void handleSensorTabClick(QWidget* tabWidget);
	void handleObstacleTabClick(QWidget* tabWidget);
	void cleanUp();

protected:
	virtual void changeEvent(QEvent *event);
	virtual void showEvent(QShowEvent *event);
	virtual void hideEvent(QHideEvent *event);

private:
	Ui::AgriFieldAdditionWidget		*ui;
	AgriFieldInfoPtr				m_pFieldInfo;
	QTableWidget					*m_tableWidget = nullptr;
};

#endif  // AgriFieldAdditionWidget_H