#ifndef AGRIFARMMANAGERTREE_H_
#define AGRIFARMMANAGERTREE_H_
 
#include <QWidget>
#include <QSqlQuery>
#include <QTreeWidget>
#include "IProj.h"
#include "DataManage/AgriFarm/AgriFarmManagerDecl.h"
#include "IAgriFarmManagerActivator.h"
#include <mutex>
namespace Ui
{
	class AgriFarmManagerTree;
}

template <typename T>
class ThreadSafeMap
{
public:
	void insert(const std::string& id, const T& data)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		data_[id] = data;
	}

	bool query(const std::string& id, T& result)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		auto it = data_.find(id);
		if (it != data_.end()) {
			result = it->second;
			return true;
		}
		return false;
	}

	bool remove(const std::string& id)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		auto it = data_.find(id);
		if (it != data_.end()) {
			data_.erase(it);
			return true;
		}
		return false;
	}

	void iterate_update(std::function<void(const std::string&, T&)> callback)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		for (auto& pair : data_) {
			callback(pair.first, pair.second);
		}
	}

	void iterate_query(std::function<void(const std::string&, const T&)> callback)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		for (const auto& pair : data_) {
			callback(pair.first, pair.second);
		}
	}

	void clear()
	{
		std::lock_guard<std::mutex> lock(mutex_);
		data_.clear();
	}

private:
	std::map<std::string, T> data_;
	std::mutex mutex_;
};
class AgriAddFarmDialog;
class AgriManageFarmDialog;
class AgriAddFieldDialog;
class AgriAssociateSensorDialog;
class AgriFarmManagerTree : public QWidget
{
	Q_OBJECT

public:
	explicit AgriFarmManagerTree(QWidget *parent = nullptr);
	~AgriFarmManagerTree();

public:
	void Init();
	void InitTree();
	void DefaultTreeSelect();
	void CreateTree();
	AgriAddFarmDialog* getAddFarmDialog();
	bool GetActiveFarm(QString &FarmId);
	AgriFarmInfoPtr GetActiveFarmInfo();
	AgriFieldInfoPtrVec GetActiveFieldList();

	void ZoneHighLight(QPainter &painter, IProj *pProj);
private:
	void BuildTree(QTreeWidget *treeWidge);
	void InsertLeafNodeData(QTreeWidget *treeWidget);
	void FilterTreeNodes(QTreeWidget *treeWidget, const QString &filter);
	bool FilterItemNodes(QTreeWidgetItem *item, const QString &filter);
	void DeleteTreeNode(QTreeWidgetItem *item);
	void SetScreenCenter(double &dLon, double &dLat);

private:
	void AddFarm();
	void ManageFarm();
	void DeleteFarm(QTreeWidgetItem *item, AgriFarmInfo farmInfo);

	void AddField(QTreeWidgetItem *parentItem, AgriFarmInfo farmInfo);
	void DeleteField(QTreeWidgetItem *item, AgriFieldInfoPtr pFieldInfo);
	void UpdateField(QTreeWidgetItem *item, AgriFieldInfoPtr pFieldInfo);

	void AssociateSensor(QTreeWidgetItem *item, AgriFieldInfoPtr pFieldInfo);

signals:
	void LeafNodeClicked(QTreeWidgetItem *item, int column);
	void SwitchFarm(AgriFarmInfoPtr farmInfo);

private slots:
	void onItemClicked(QTreeWidgetItem *item, int column);
	void onRightClicked(const QPoint& point);
	void on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);

private:
	QString													m_strTreeTableName = "";
	QString													m_sCurActiveFarm = "";
	std::map<std::string, std::string>                      SystemSysIDAndName;
	AgriFarmInfoPtr											m_ActiveFarmInfo = nullptr;
	AgriFieldInfoPtrVec										m_ActiveFieldList;

private:
	Ui::AgriFarmManagerTree									*ui;
	AgriAddFarmDialog										*m_AddFarmDialog = nullptr;
	AgriManageFarmDialog									*m_ManageFarmDialog = nullptr;
	AgriAddFieldDialog										*m_AddFieldDialog = nullptr;	// Ìí¼Ó
	AgriAddFieldDialog										*m_UpdateFieldDialog = nullptr; // ¸üÐÂ
	AgriAssociateSensorDialog								*m_AssociateSensorDialog = nullptr;

	ThreadSafeMap<qnzkna::ZoneManage::CZoneBase>			m_zoneDrawMap;
	AgriFieldInfo											m_fieldInfo;
};


#endif  // AgriFarmManagerTree_H