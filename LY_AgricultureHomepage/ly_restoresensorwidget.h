#ifndef LY_RESTORESENSORWIDGET_H
#define LY_RESTORESENSORWIDGET_H

#include <QWidget>
#include "LyDialog.h"
#include <QListWidget>
#include <QListWidgetItem>
#include "LY_AgricultureHomepageActivator.h"
#include "DataManage/AgriFarm/AgriHomePageDecl.h"
#include "ly_restoresensorwidget.h"
#include <QShowEvent>

namespace Ui {
class LY_RestoreSensorWidget;
}

class LY_RestoreSensorWidget : public LyDialog
{
    Q_OBJECT

public:
    explicit LY_RestoreSensorWidget(QWidget *parent = nullptr);
    ~LY_RestoreSensorWidget();

signals:

	void getSensorList(SensorListModelVec leftVec, SensorListModelVec rightVec);
	void deleteSensor(QString mointID, QString mointName);
private slots:
	void on_move_right_action();
	void on_move_left_action();
	void on_sure_action();
	void on_cancle_action();

	void on_left_all();
	void on_left_not_all();
	void on_left_delete();
	void on_right_all();
	void on_right_not_all();

protected:
	void showEvent(QShowEvent *event);

private:
    Ui::LY_RestoreSensorWidget *ui;
	//SensorListModelVec m_originVec;
	
	bool m_processing = false;      //标志避免在更新项状态时重复进入槽函数，防止无限递归。
	void moveItems(QListWidget *src, QListWidget *dst);

	std::vector<SensorListModel> getSelectedLeftItemsData(QListWidget *listWidget) const;
	//std::vector<SensorListModel> m_selectedLeftVec;
	//std::vector<SensorListModel> m_selectedRightVec;
};

#endif // LY_RESTORESENSORWIDGET_H
