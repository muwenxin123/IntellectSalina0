#ifndef LY_CUSTOMCOMBOBOX_H
#define LY_CUSTOMCOMBOBOX_H

#include "qlineedit.h"
#include "qlistwidget.h"
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include "DataManage/AgriFarm/AgriHomePageDecl.h"

class LY_CustomComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit LY_CustomComboBox(QWidget *parent = nullptr);
	//隐藏下拉框
	virtual void hidePopup();

	void clearnItem();
	//添加一条选项
	void addItem(const SensorListModel &_model, const QVariant& _variant = QVariant());

	//添加多条选项
	void addItems(const QStringList& _text_list);

	//返回当前选中选项
	QStringList currentText();

	//返回当前选项条数
	int count() const;

	//设置文本框默认文字
	void SetPlaceHolderText(const QString& _text);

	//下拉框状态恢复默认
	void ResetSelection();
	//设置选中文本--单
	void setCurrentText(const QString& _text);

	//设置选中文本--多
	void setCurrentText(const QStringList& _text_list);


protected:

	//事件过滤器
	virtual bool eventFilter(QObject* watched, QEvent* event);

	//滚轮事件
	virtual void wheelEvent(QWheelEvent* event);

	//按键事件
	virtual void keyPressEvent(QKeyEvent* event);

	private slots:

	//槽函数：文本框文本变化
	void stateChange(int _row);
	//槽函数：点击下拉框选项
	void itemClicked(int _index);
	//槽函数：单项与全选状态
	void checkAllState(int state);

signals:

	//信号：发送当前选中选项
	void selectionChange(SensorListModelVec vec);

private:
	SensorListModelVec senListVec;
	//下拉框
	QListWidget* list_widget_;
	//文本框，搜索框
	QLineEdit*   line_edit_;
	//全选
	QCheckBox*   checkAll;
	//下拉框显示标志
	bool         show_flag_;
};

#endif // LY_CUSTOMCOMBOBOX_H
