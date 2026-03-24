#ifndef AGRIMACHINEMANAGERDIALOG_H_
#define AGRIMACHINEMANAGERDIALOG_H_
 
#include <QWidget>
#include <Protocol/XygsJL/sharedFile.h>

#include "DataManage/AgriMachineManager/AgriMachineManagerDecl.h"

namespace Ui
{
	class AgriMachineManagerDialog;
}

class AgriMachineManagerDialog : public QWidget
{
	Q_OBJECT

public:
	explicit AgriMachineManagerDialog(QWidget *parent = nullptr);
	~AgriMachineManagerDialog();

protected:
	virtual void changeEvent(QEvent *event);
	virtual void showEvent(QShowEvent *event);
	virtual void hideEvent(QHideEvent *event);

private:
	void InitData();
	void Insertimage(const std::string& strGuid);
	void updateDisplayInfo(const AgriMachineBasicInfo& machineInfo);
	void updateDisplayCombox(const AgriMachineBasicInfo& machineInfo);

private slots:
	void on_comboBox_second_currentTextChanged(int index);
	void on_comboBox_third_currentTextChanged(int index);
	void on_comboBox_fourth_currentTextChanged(int index);

private:
	Ui::AgriMachineManagerDialog						*ui;
	bool												m_show;
	bool												m_isUpdatingComboBox;
	std::map<int, AgriDirMachineInfoPtrVec>				m_vAgriDirMachineInfoMap;	// <pid, vector>
};

#endif  // AgriMachineManagerDialog_H