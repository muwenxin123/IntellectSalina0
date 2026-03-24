#ifndef AGRIDATAANALYSISPREDICTDIALOG_H_
#define AGRIDATAANALYSISPREDICTDIALOG_H_
 
#include <QWidget>
#include <Protocol/XygsJL/sharedFile.h>

#include "LyDialog.h"

namespace Ui
{
	class AgriDataAnalysisPredictDialog;
}

class AgriDataAnalysisPredictDialog : public LyDialog
{
	Q_OBJECT

public:
	explicit AgriDataAnalysisPredictDialog(QWidget *parent = nullptr);
	~AgriDataAnalysisPredictDialog();

protected:
	virtual void changeEvent(QEvent *event);
	virtual void showEvent(QShowEvent *event);
	virtual void hideEvent(QHideEvent *event);

private:
	void InitData();

public slots :
	void on_pushButton_clicked();

private:
	Ui::AgriDataAnalysisPredictDialog						*ui;
	bool													m_show;
};
#endif  // AgriDataAnalysisPredictDialog_H