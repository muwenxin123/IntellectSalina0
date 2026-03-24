#ifndef CDlgCropGrowStageWidget_H
#define CDlgCropGrowStageWidget_H

#include <QWidget>
#include "LyDialog.h"
#include "IAgroTaskService.h"
#include "Zone\AgroTaskInfo.h"
#include "QPushButton"
#include <vector>
#include "QTreeWidget"
#include "QLineEdit"
#include "QTextEdit"

namespace Ui {
class CDlgCropGrowStageWidget;
}

class CreateGrowStageWidget : public LyDialog
{
	Q_OBJECT

public:
	explicit CreateGrowStageWidget(QWidget *parent = nullptr);
	~CreateGrowStageWidget();

signals:
	void sig_create_stage(const QString &name, const QString &details);

private:
	void init_widget();

private:
	QLineEdit *name_lineedit_ = nullptr;
	QTextEdit *details_textedit_ = nullptr;
};

class CDlgCropGrowStageWidget : public LyDialog
{
    Q_OBJECT

public:
    explicit CDlgCropGrowStageWidget(QWidget *parent = nullptr);
    ~CDlgCropGrowStageWidget();

public slots:

private:
	void init_widget();
	void init_connect();
	void add_top_level_item(int type, const QString &name);
	void add_child_item(QTreeWidgetItem *parent, int type, const QString &name, int index1 = -1, int index2 = -1);
	void create_details_widget(int index, const QString &name, int zt, int zzt, int zzzt);

private:
    Ui::CDlgCropGrowStageWidget *ui;

	qnzkna::AgroTask::IAgroTaskService *m_pIAgroTaskService;

	std::vector<std::vector<QString>> seedSubStateVec;
	std::vector<std::vector<std::vector<QString>>> seedSubSubStateVec;

	qnzkna::AgroTask::CAgroRefDataInfoVec m_AgroRefDataInfoVec;

	int item_index = 0; //右侧详情窗口下表

};

#endif // CDlgCropGrowStageWidget_H
