#include "CommDelegate.h"

#include <QPainter>

#include <QSpinBox>
#include <QComboBox>
#include <QStandardItemModel>

SpinBoxDelegate::SpinBoxDelegate(int minVal, int maxVal, QObject *parent)
	: QStyledItemDelegate(parent)
	, m_minVal(minVal)
	, m_maxVal(maxVal)
{
}

QWidget *SpinBoxDelegate::createEditor(QWidget *parent,
	const QStyleOptionViewItem &/* option */,
	const QModelIndex &/* index */) const
{
	QSpinBox *editor = new QSpinBox(parent);
	editor->setFrame(false);
	editor->setMinimum(m_minVal);
	editor->setMaximum(m_maxVal);

	return editor;
}

void SpinBoxDelegate::setEditorData(QWidget *editor,
	const QModelIndex &index) const
{
	int value = index.model()->data(index, Qt::EditRole).toInt();

	QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
	spinBox->setValue(value);
}

void SpinBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
	const QModelIndex &index) const
{
	QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
	spinBox->interpretText();
	int value = spinBox->value();

	model->setData(index, value, Qt::EditRole);
}

void SpinBoxDelegate::updateEditorGeometry(QWidget *editor,
	const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
	editor->setGeometry(option.rect);
}

DoubleSpinBoxDelegate::DoubleSpinBoxDelegate(double minVal, double maxVal, QObject *parent)
	: QStyledItemDelegate(parent)
	, m_minVal(minVal)
	, m_maxVal(maxVal)
{
}

QWidget *DoubleSpinBoxDelegate::createEditor(QWidget *parent,
	const QStyleOptionViewItem &/* option */,
	const QModelIndex &/* index */) const
{
	QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
	editor->setFrame(false);
	editor->setMinimum(m_minVal);
	editor->setMaximum(m_maxVal);

	return editor;
}

void DoubleSpinBoxDelegate::setEditorData(QWidget *editor,
	const QModelIndex &index) const
{
	double value = index.model()->data(index, Qt::EditRole).toDouble();

	QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
	spinBox->setValue(value);
}

void DoubleSpinBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
	const QModelIndex &index) const
{
	QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
	spinBox->interpretText();
	double value = spinBox->value();

	model->setData(index, value, Qt::EditRole);
}

void DoubleSpinBoxDelegate::updateEditorGeometry(QWidget *editor,
	const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
	editor->setGeometry(option.rect);
}


ComboboxDelegate::ComboboxDelegate(QObject *parent /*= 0*/)
	: QStyledItemDelegate(parent)
{

}

void ComboboxDelegate::setCbbInfo(QList<QPair<QString, QVariant>>& list)
{
	m_list = list;
}

void ComboboxDelegate::clearCbb()
{
	m_list.clear();
}

void ComboboxDelegate::GetDefault(QString& val, QVariant& data)
{
	val.clear();
	data = QVariant();
	if (!m_list.isEmpty())
	{
		val = m_list.front().first;
		data = m_list.front().second;
	}
}

QWidget * ComboboxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QComboBox *editor = new QComboBox(parent);
	for (auto& data : m_list)
		editor->addItem(data.first, data.second);

	return editor;
}

void ComboboxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	QString value = index.model()->data(index, Qt::EditRole).toString();

	QComboBox *cbb = static_cast<QComboBox*>(editor);
	cbb->setCurrentText(value);
}

void ComboboxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	QComboBox *cbb = static_cast<QComboBox*>(editor);
	QString text = cbb->currentText();
	QVariant data = cbb->currentData();
	model->setData(index, text, Qt::EditRole);
	model->setData(index, data, DELEGATE_DATA_ROLE);
}

void ComboboxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	editor->setGeometry(option.rect);
}

CommPixMapDelegate::CommPixMapDelegate(QObject *parent)
{

}

void CommPixMapDelegate::setOption(const EOption& type, const QVariant& data)
{
	switch (type)
	{
	case ResizeType:
		m_resizeItemType = data.toInt();
		break;
	case PixMapSize:
		m_pixSize = data.toSize();
		break;
	case MarginSize:
		m_marginSize = data.toInt();
		break;
	case ScaleType:
		m_ratioType = (Qt::AspectRatioMode)data.toInt();
		break;
	default:
		break;
	}
}

void CommPixMapDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QStyledItemDelegate::paint(painter, option, index);

	QVariant data = index.data(DELEGATE_DATA_ROLE);
	if (data.canConvert<QPixmap>())
	{
		QPixmap image = data.value<QPixmap>();
		QRect rect = option.rect;

		QSize pixMapSize;
		switch (m_resizeItemType)
		{
		case 2:
			pixMapSize = image.size().scaled(m_pixSize, m_ratioType);
			break;
		case 1:
		default: {
			pixMapSize = image.size().scaled(QSize(rect.width() - 2 * m_marginSize, rect.height() - 2 * m_marginSize) , m_ratioType);
		}
		break;
		}

		QPoint pt(rect.x() + (rect.width() - pixMapSize.width()) / 2,  rect.y() + (rect.height() - pixMapSize.height())/2);
		painter->drawPixmap(QRect(pt, pixMapSize), image);
	}
}

QSize CommPixMapDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{

	switch (m_resizeItemType)
	{
	// 1代表根据图片大小
	case 1:
	{
		QVariant data = index.data(DELEGATE_DATA_ROLE);
		if (data.canConvert<QPixmap>())
		{
			QSize size = data.value<QPixmap>().size();
			return QSize(size.width() + m_marginSize*2, size.height() + m_marginSize * 2);
		}
	}
		break;
	case 2:
	{
		// 2 代表指定大小
		return QSize(m_pixSize.width()+m_marginSize * 2, m_pixSize.height() + m_marginSize*2);
	}
		break;
	default:
	break;
	}
	return QStyledItemDelegate::sizeHint(option, index);
}
