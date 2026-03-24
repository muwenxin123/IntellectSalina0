#ifndef CDLGAGROSAMPLEPARASHOW_H
#define CDLGAGROSAMPLEPARASHOW_H

#include <QWidget>
#include "LyDialog.h"

namespace Ui {
class CDlgAgroSampleParaShow;
}

class CDlgAgroSampleParaShow : public LyDialog
{
    Q_OBJECT

public:
    explicit CDlgAgroSampleParaShow(QWidget *parent = nullptr);
    ~CDlgAgroSampleParaShow();

	void SetStringName(const QString& strTmp);
	void SetStringNYCS(const QString& strTmp);
	void InitCtrl();

protected:

	virtual void hideEvent(QHideEvent *event);

private slots:

    void on_pushButton_clicked();

private:
    Ui::CDlgAgroSampleParaShow *ui;
};

#endif // CDLGAGROSAMPLEPARASHOW_H
