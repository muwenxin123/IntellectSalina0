#ifndef AGRIADDFIELDDIALOG_H_
#define AGRIADDFIELDDIALOG_H_
 
#include <QWidget>
#include <QTreeWidget>

#include "LyDialog.h"
#include "DataManage/AgriFarm/AgriFarmManagerDecl.h"

namespace Ui {
	class AgriAddFieldDialog;
}

class AgriAddFieldDialog : public LyDialog
{
	Q_OBJECT

public:
	// 添加Dialog
	explicit AgriAddFieldDialog(const QString& farmId, QWidget *parent = nullptr);
	// 修改Dialog
	explicit AgriAddFieldDialog(const AgriFieldInfoPtr pFieldInfo, QWidget *parent = nullptr);
	~AgriAddFieldDialog();
	
public:
	AgriFieldInfoVec* GetDialogFieldInfoVec();
	AgriFieldInfoPtr GetDialogFieldInfo();

protected:
	void closeEvent(QCloseEvent* event) override;

private:
	void Init();
	void InitFieldBasicInfo();
	void InitDisplayCombox();
	void CreateZoneTree();
	void AddZoneTree();

	void UpdateParentItem(QTreeWidgetItem* item);
	void UpdateZoneTree();

signals:
	void addFinish();
	void updateFinish();

public slots:
	void on_btnOK_clicked();
	void on_btnCancel_clicked();
	void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);

	void on_comboBox_crop_currentTextChanged(int index);
	void on_comboBox_crop_variety_currentTextChanged(int index);

private:
	enum CreateDialogType
	{
		CREATE_TYPE_UNKNOW,		// 未知
		CREATE_TYPE_ADD,		// 添加
		CREATE_TYPE_UPDATE,		// 更新
	};

	CreateDialogType				m_eCerateType{ CREATE_TYPE_UNKNOW };
	AgriFieldInfoVec				*m_vFieldInfo = nullptr;
	QStringList						m_TreeHeader;
	QString							m_sFarmId;
	AgriFieldInfoPtr				m_pFieldInfo = nullptr;
	AgriEnumInfoPtrVec				m_vEnumPlantStandard;
	AgriEnumInfoPtrVec				m_vEnumSowModel;
	AgriCropInfoPtrVec				m_vCropInfo;
	AgriCropVarietyInfoPtrVec		m_vCropVarietyInfo;

private:
	Ui::AgriAddFieldDialog *ui;
};
#endif  // AgriAddFieldDialog_H