#ifndef SYSTEMEVENTRECORDING_H
#define SYSTEMEVENTRECORDING_H

#include <QDialog>
#include <QTableWidget>
#include "DataManage/SystemInfo/SystemInternalInterface.h"
#include "DataManage/INebulaTaskManageService.h"

class SystemEventRecording : public QDialog, public qnzkna::TaskManage::INebulaTaskManageObserver
{
    Q_OBJECT
public:
    explicit SystemEventRecording(QWidget *parent = nullptr);
	~SystemEventRecording();

	virtual void Update(qnzkna::TaskManage::INebulaTaskManageService* sub);

signals:

public slots:
	void sltRecvEvent(EventInfo info);

private:
	void initUI();

private:
    QTableWidget* m_pTableWidget = nullptr;
};

#endif
