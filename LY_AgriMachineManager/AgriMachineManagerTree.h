#ifndef AGRIMACHINEMANAGERTREE_H_
#define AGRIMACHINEMANAGERTREE_H_
 
#include <QWidget>
#include <QSqlQuery>
#include <QTreeWidget>

#include "DataManage/AgriMachineManager/AgriMachineManagerDecl.h"

namespace Ui
{
	class AgriMachineManagerTree;
}

class AgriMachineManagerTree : public QWidget
{
	Q_OBJECT

public:
	explicit AgriMachineManagerTree(QWidget *parent = nullptr);
	~AgriMachineManagerTree();
public:
	void CreateTree();

private:
	int BuildTree(QTreeWidget *treeWidge, int parentCode, QTreeWidgetItem *parentItem = nullptr);
	void InsertLeafNodeData(QTreeWidget *treeWidget);
	void InsertLeafDataRecursively(QTreeWidgetItem *item, QSqlQuery &query);
	void FilterTreeNodes(QTreeWidget *treeWidget, const QString &filter);
	bool FilterItemNodes(QTreeWidgetItem *item, const QString &filter);
	bool FilterExistItemNodes(QTreeWidgetItem *item, bool filter);

private:
	int countLeafNodes(QTreeWidgetItem *item);
	void updateParentTextWithLeafCount(QTreeWidgetItem *item);
	void updateTreeLeafCount(QTreeWidget *tree);

signals:
	void LeafNodeClicked(AgriMachineBasicInfo& machineInfo);

private slots:
	void onItemClicked(QTreeWidgetItem *item, int column);
	void onCheckBoxToggled(bool checked);

public:
	QString	 strTreeTableName = "";
	std::map<std::string, std::string>                      SystemSysIDAndName;
private:
	Ui::AgriMachineManagerTree *ui;
};

#endif  // AgriMachineManagerTree_H