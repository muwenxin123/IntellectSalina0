#ifndef AGRIMANAGEFARMDIALOG_H_
#define AGRIMANAGEFARMDIALOG_H_
 
#include <QWidget>
#include <QTableWidget>
#include <QButtonGroup>
#include <QContextMenuEvent>

#include "LyDialog.h"
#include "DataManage/AgriFarm/AgriFarmManagerDecl.h"

namespace Ui {
	class AgriManageFarmDialog;
}

class AgriFarmInfo;
class AgriManageFarmDialog : public LyDialog
{
	Q_OBJECT

public:
	explicit AgriManageFarmDialog(const AgriFarmInfoVec& FarmList, QWidget *parent = nullptr);
	~AgriManageFarmDialog();
	void Init(const AgriFarmInfoVec& FarmList);

protected:
	void closeEvent(QCloseEvent* event) override;
	void contextMenuEvent(QContextMenuEvent *event) override;

private:
	void initSelectedRadioButton();

signals:
	void addFarm();
	void switchFarm(QString farmId);

public slots:
	void on_btnOK_clicked();
	void on_btnCancel_clicked();
	void viewDetails();
	void deleteRow();
	void onRadioButtonClicked(int id);

private:
	int						m_nChoiceRow = -1;
	QTableWidget			*m_tableWidget = nullptr;
	QButtonGroup			*m_buttonGroup = nullptr;
private:
	Ui::AgriManageFarmDialog *ui;
};

#endif  // AgriManageFarmDialog_H