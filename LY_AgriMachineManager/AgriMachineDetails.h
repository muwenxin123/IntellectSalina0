#ifndef AGRIMACHINEDETAILS_H_
#define AGRIMACHINEDETAILS_H_

#include <QWidget>
#include <QTableWidget>
#include <rttr/type>

#include "DataManage/AgriMachineManager/AgriMachineManagerDecl.h"

namespace Ui
{
	class AgriMachineDetails;
}

// <typename, <key, info>>
typedef std::unordered_map<std::string, std::unordered_map<std::string, AgriMachineXmlCfgInfo>> MapMachineXmlCfg;

class TableEditor;
class AgriMachineDetails : public QWidget
{
	Q_OBJECT

public:
	explicit AgriMachineDetails(QWidget *parent = nullptr);
	~AgriMachineDetails();

public:
	void updateDisplayInfo(const AgriMachineBasicInfo& machineInfo);

	//std::unordered_map<std::string, FieldConfig>;
	void loadXmlConfiguration();
private:
	template <typename T>
	void populateTable(const AgriMachineBasicInfo& machineInfo) {
		auto detailInfo = LY_AgriMachineManager::Get().GetpDbHandler()->FetchMachineInfo<T>(machineInfo.guid);
		tableEditor->setStructure(detailInfo);
		tableEditor->populateTable(machineCfg);
		ui->verticalLayout->addWidget(tableEditor);
	};

private:
	Ui::AgriMachineDetails *ui;
	TableEditor * tableEditor;
	MapMachineXmlCfg machineCfg;
};

class TableEditor : public QTableWidget {
	Q_OBJECT
public:
	TableEditor(QWidget* parent = nullptr);
public:
	// 设置结构体实例
	template <typename T>
	void setStructure(T& obj) 
	{
		instanceObj = new rttr::instance(obj);
		objType = rttr::type::get<T>();
	}
	void populateTable(const MapMachineXmlCfg & mapCfg);
private:
	void cellChanged(int row, int column);
private:
	rttr::instance *instanceObj = nullptr;
	rttr::type objType = rttr::detail::get_invalid_type();
};
#endif  // AgriMachineDetails_H