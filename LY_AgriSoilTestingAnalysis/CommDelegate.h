#ifndef COMMDELEGATE_H
#define COMMDELEGATE_H

#include <QStyledItemDelegate>

class SpinBoxDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	SpinBoxDelegate(int minVal, int maxVal, QObject *parent = 0);

	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
		const QModelIndex &index) const override;

	void setEditorData(QWidget *editor, const QModelIndex &index) const override;
	void setModelData(QWidget *editor, QAbstractItemModel *model,
		const QModelIndex &index) const override;

	void updateEditorGeometry(QWidget *editor,
		const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
	int m_minVal = 0;
	int m_maxVal = 100;
};

class DoubleSpinBoxDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	DoubleSpinBoxDelegate(double minVal, double maxVal, QObject *parent = 0);

	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
		const QModelIndex &index) const override;

	void setEditorData(QWidget *editor, const QModelIndex &index) const override;
	void setModelData(QWidget *editor, QAbstractItemModel *model,
		const QModelIndex &index) const override;

	void updateEditorGeometry(QWidget *editor,
		const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
	double m_minVal = 0;
	double m_maxVal = 100;
};

class ComboboxDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	ComboboxDelegate(QObject *parent = 0);

	void setCbbInfo(QList<QPair<QString, QVariant>>& list);

	void clearCbb();

	void GetDefault(QString& val, QVariant& data);

	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
		const QModelIndex &index) const override;

	void setEditorData(QWidget *editor, const QModelIndex &index) const override;
	void setModelData(QWidget *editor, QAbstractItemModel *model,
		const QModelIndex &index) const override;

	void updateEditorGeometry(QWidget *editor,
		const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
	QList<QPair<QString, QVariant>> m_list;
};

class CommPixMapDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	enum EOption
	{
		ResizeType,
		PixMapSize,
		MarginSize,
		ScaleType
	};
	CommPixMapDelegate(QObject *parent = 0);
	
	// 设置选项
	void setOption(const EOption& type, const QVariant&  data);

	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

	virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
	int m_resizeItemType = 2;														// 0 代表根据Item的大小  1代表根据图片大小 2代表指定大小
	QSize m_pixSize = QSize(100,100);											// 图片大小
	int m_marginSize = 0;																// 图片边框大小
	Qt::AspectRatioMode m_ratioType = Qt::IgnoreAspectRatio;	// 图片缩放方式
};

#define DELEGATE_DATA_ROLE Qt::UserRole+1
#endif // COMMSEARCHEDITVIEW_H
