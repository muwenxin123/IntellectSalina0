#include "ly_customcombobox.h"
#include "qcombobox.h"
#include "qcoreevent.h"
#include "qlineedit.h"
#include "qlistwidget.h"
#include <QtCore/QDebug>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QListWidgetItem>
#include "QCString.h"
#include <set>

LY_CustomComboBox::LY_CustomComboBox(QWidget *parent): QComboBox{parent}
{

    list_widget_ = new QListWidget();
    line_edit_   = new QLineEdit();
    checkAll     = new QCheckBox();


	list_widget_->setStyleSheet(
		"QListWidget {"
		"   background: white;"
		"   border-radius: 8px;"
		"   border: 1px solid #e0e0e0;"
		"   padding: 4px;"
		"   font-family: 'Segoe UI';"
		"}"
		"QListWidget::item {"
		"   height: 35px;"
		"   border-radius: 4px;"
		"   margin: 2px 0;"
		"   padding: 0 16px;"
		"}"

		"QScrollBar:vertical {"
		"   width: 10px;"
		"   background: #f8f9fa;"
		"}"
		"QScrollBar::handle:vertical {"
		"   background: #dee2e6;"
		"   border-radius: 4px;"
		"}"
	);

    /*设置文本框*/
    //设为只读，因为该输入框只用来显示选中的选项，称为文本框更合适些
    line_edit_->setReadOnly(true);
    //把当前对象安装(或注册)为事件过滤器，当前也称为过滤器对象。事件过滤器通常在构造函数中进行注册。
    line_edit_->installEventFilter(this);
    //设置禁用样式，因为不受样式表控制，临时这样解决
  //  line_edit_->setStyleSheet("QLineEdit:disabled{background:rgb(233,233,233);}");
    //设置全选框
    QListWidgetItem *currentItem = new QListWidgetItem(list_widget_);
    checkAll->setText(tr2("全选"));
    list_widget_->addItem(currentItem);
    list_widget_->setItemWidget(currentItem, checkAll);

	
    this->setModel(list_widget_->model());
    this->setView(list_widget_);
    this->setLineEdit(line_edit_);
	senListVec.clear();
    connect(this, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, &LY_CustomComboBox::itemClicked);
    //全选
    connect(checkAll, &QCheckBox::released, [=]() mutable {
        bool ischecked = checkAll->isChecked();
        for (int i = 0; i < list_widget_->count(); i++) {
            QCheckBox *check_box = static_cast<QCheckBox *>(list_widget_->itemWidget(list_widget_->item(i)));
			check_box->blockSignals(true);
			check_box->setChecked(ischecked); // 防止重复触发
			check_box->blockSignals(false);
        }
		senListVec.clear();
		stateChange(0);
    });
}
void LY_CustomComboBox::clearnItem()
{
	// 获取总项数
	int count = list_widget_->count();

	// 从最后一项开始删除，直到只剩第一项
	for (int i = count - 1; i >= 1; --i) {
		QListWidgetItem* item = list_widget_->takeItem(i);
		delete item; // 记得删除取出的项，避免内存泄漏
	}
	senListVec.clear();
}
void LY_CustomComboBox::checkAllState(int state)
{
    if (Qt::Unchecked == state) {
        checkAll->setChecked(false);
    } else if (Qt::Checked == state) {
        bool isallcheck = true;
        for (int i = 0; i < list_widget_->count(); i++) {
            QCheckBox *check_box = static_cast<QCheckBox *>(list_widget_->itemWidget(list_widget_->item(i)));
            if (check_box->isChecked() == false) {
                isallcheck = false;
            }
        }
        checkAll->setChecked(isallcheck);
    }
}
void LY_CustomComboBox::hidePopup()
{
    show_flag_ = false;
    int width  = this->width();
    int height = this->height();
    int x      = QCursor::pos().x() - mapToGlobal(geometry().topLeft()).x() + geometry().x();
    int y      = QCursor::pos().y() - mapToGlobal(geometry().topLeft()).y() + geometry().y();
    if (x >= 0 && x <= width && y >= this->height() && y <= height + this->height()) {
    } else {
        QComboBox::hidePopup();
    }
}

void LY_CustomComboBox::addItem(const SensorListModel &_model, const QVariant& _variant /*= QVariant()*/)
{
    Q_UNUSED(_variant);
    QListWidgetItem *item     = new QListWidgetItem(list_widget_);
    QCheckBox       *checkbox = new QCheckBox(this);
    checkbox->setText(_model.mointName);
    list_widget_->addItem(item);
	item->setData(Qt::UserRole, QVariant::fromValue(_model));
    list_widget_->setItemWidget(item, checkbox);
    connect(checkbox, &QCheckBox::stateChanged, this, &LY_CustomComboBox::stateChange);
   connect(checkbox, &QCheckBox::stateChanged, this, &LY_CustomComboBox::checkAllState);
	 //connect(checkbox, &QCheckBox::stateChanged, this, &LY_CustomComboBox::stateChange);
}

void LY_CustomComboBox::addItems(const QStringList &_text_list)
{
   /* for (const auto &text_one : _text_list) {
        addItem(text_one);
    }*/
}

QStringList LY_CustomComboBox::currentText()
{
    QStringList text_list;
    if (!line_edit_->text().isEmpty()) {
        //以;为分隔符分割字符串
        text_list = line_edit_->text().split(';');
    }
    return text_list;
}

int LY_CustomComboBox::count() const
{
    int count = list_widget_->count() - 1;
    if (count < 0) {
        count = 0;
    }
    return count;
}

void LY_CustomComboBox::SetPlaceHolderText(const QString &_text)
{
    line_edit_->setPlaceholderText(_text);
}

void LY_CustomComboBox::ResetSelection()
{
    int count = list_widget_->count();
    for (int i = 1; i < count; i++) {
        //获取对应位置的QWidget对象
        QWidget   *widget    = list_widget_->itemWidget(list_widget_->item(i));
        //将QWidget对象转换成对应的类型
        QCheckBox *check_box = static_cast<QCheckBox *>(widget);
        check_box->setChecked(false);
    }
}

void LY_CustomComboBox::setCurrentText(const QString &_text)
{
    int count = list_widget_->count();
    for (int i = 1; i < count; i++) {
        //获取对应位置的QWidget对象
        QWidget   *widget    = list_widget_->itemWidget(list_widget_->item(i));
        //将QWidget对象转换成对应的类型
        QCheckBox *check_box = static_cast<QCheckBox *>(widget);
        if (_text.compare(check_box->text()))
            check_box->setChecked(true);
    }
}

void LY_CustomComboBox::setCurrentText(const QStringList &_text_list)
{
    int count = list_widget_->count();
    for (int i = 1; i < count; i++) {
        //获取对应位置的QWidget对象
        QWidget   *widget    = list_widget_->itemWidget(list_widget_->item(i));
        //将QWidget对象转换成对应的类型
        QCheckBox *check_box = static_cast<QCheckBox *>(widget);
        if (_text_list.contains(check_box->text()))
            check_box->setChecked(true);
    }
}

bool LY_CustomComboBox::eventFilter(QObject *watched, QEvent *event)
{
    //设置点击输入框也可以弹出下拉框
    if (watched == line_edit_ && event->type() == QEvent::MouseButtonRelease && this->isEnabled()) {
        showPopup();
        return true;
    }
    return false;
}

void LY_CustomComboBox::wheelEvent(QWheelEvent *event)
{
    //禁用QComboBox默认的滚轮事件
    Q_UNUSED(event);
}

void LY_CustomComboBox::keyPressEvent(QKeyEvent *event)
{
    QComboBox::keyPressEvent(event);
}

void LY_CustomComboBox::stateChange(int _row)
{
    Q_UNUSED(_row);

    QString selected_data("");
    int     count = list_widget_->count();
	
    for (int i = 1; i < count; i++) {
		QListWidgetItem *item = list_widget_->item(i);
		if (item == nullptr)
		{
			continue;
		}
        QWidget   *widget    = list_widget_->itemWidget(item);
        QCheckBox *check_box = static_cast<QCheckBox *>(widget);
        if (check_box->isChecked()) {
			SensorListModel model = item->data(Qt::UserRole).value<SensorListModel>();
			selected_data.append(model.mointName).append(";");
			senListVec.push_back(model);
        }
    }


    selected_data.chop(1);
    if (!selected_data.isEmpty()) {
        line_edit_->setText(selected_data);
    } else {
        line_edit_->clear();
    }
	
    line_edit_->setToolTip(selected_data);
    emit selectionChange(senListVec);
}

void LY_CustomComboBox::itemClicked(int _index)
{
    if (_index != 0) {
        QCheckBox *check_box = static_cast<QCheckBox *>(list_widget_->itemWidget(list_widget_->item(_index)));
        check_box->setChecked(!check_box->isChecked());
    }
}
