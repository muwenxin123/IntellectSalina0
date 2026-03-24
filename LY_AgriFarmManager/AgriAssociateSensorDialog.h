#ifndef AGRIASSOCIATESENSORDIALOG_H_
#define AGRIASSOCIATESENSORDIALOG_H_
 
#include <QWidget>
#include <QTreeWidget>

#include "LyDialog.h"
#include "DataManage/AgriFarm/AgriFarmManagerDecl.h"

namespace Ui {
	class AgriAssociateSensorDialog;
}

class AgriAssociateSensorDialog : public LyDialog
{
	Q_OBJECT

public:
	explicit AgriAssociateSensorDialog(const AgriFieldInfoPtr pFieldInfo, QWidget *parent = nullptr);
	~AgriAssociateSensorDialog();

private:
	void Init();
	void CreateSensorTree();
	void AddSensorTree();
	void UpdateParentItem(QTreeWidgetItem* item);
	void UpdateTreeCheckState();

protected:
	void closeEvent(QCloseEvent* event) override;

signals:
	void relationFinish();

public slots:
	void on_btnOK_clicked();
	void on_btnCancel_clicked();
	void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);

private:
	AgriFieldInfoPtr				m_pFieldInfo = nullptr;
	QStringList						m_TreeHeader;
	std::vector<int>				m_vecFieldRelateSensorId;
private:
	Ui::AgriAssociateSensorDialog *ui;
};
#endif  // AgriAssociateSensorDialog_H