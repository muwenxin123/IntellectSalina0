#ifndef AGRIADDFARMDIALOG_H_
#define AGRIADDFARMDIALOG_H_
 
#include <QWidget>
#include "LyDialog.h"

namespace Ui {
	class AgriAddFarmDialog;
}

class AgriFarmInfo;
class AgriAddFarmDialog : public LyDialog
{
	Q_OBJECT

public:
	explicit AgriAddFarmDialog(QWidget *parent = nullptr);
	~AgriAddFarmDialog();
	void Init();

public:
	inline bool isPositionPick() { return m_bPick; };
	void updateTargetPoint(const QPointF & point);
	AgriFarmInfo* GetDialogInfo();

protected:
	void closeEvent(QCloseEvent* event) override;

signals:
	void addFinish();

public slots:
	void on_btnOK_clicked();
	void on_btnCancel_clicked();
	void on_btnPick_clicked();

private:
	bool		     m_bPick = false;
	QPointF		     m_targetPoint;
	AgriFarmInfo	 *m_farmInfo = nullptr;
private:
	Ui::AgriAddFarmDialog *ui;
};
#endif  // AgriAddFarmDialog_H