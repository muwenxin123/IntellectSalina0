#ifndef STYLETESTWIDGET_H
#define STYLETESTWIDGET_H

#include <QWidget>
#include <QTableWidgetItem>

class LyWidget;
namespace Ui
{
	class StyleTestWidget;
}

class LyColorSelector;
class TableItemDelegate;

class StyleTestWidget : public QWidget
{
	Q_OBJECT
	Q_PROPERTY(QString iconPath READ iconPath WRITE setIconPath)

  public:
	StyleTestWidget(QWidget *parent = Q_NULLPTR);
	~StyleTestWidget();

	QString iconPath() const;
	void setIconPath(const QString &iconPath);

  public slots:
	void on_pushButton_clicked();
	void on_selectFileTBtn_clicked();
	void on_selectColorTBtn_clicked();
	void on_selectColorTBtn_2_clicked();
	void on_selectDateTBtn_clicked();
	void on_selectTimeTBtn_clicked();
	void on_selectDateTimeTBtn_clicked();
	void on_selectDateRangeTBtn_clicked();
	void on_selectTimeRangeTBtn_clicked();
	void on_selectDateTimeRangeTBtn_clicked();
	void on_informationTBtn_clicked();
	void on_questionTBtn_clicked();
	void on_warningTBtn_clicked();
	void on_criticalTBtn_clicked();
	void on_comboBox_2_currentTextChanged(const QString &);

	void on_tableWidget_cellClicked(int row, int column);

	void on_pushButton_4_clicked(bool checked);

	void on_toolButton_11_clicked();

	void on_pushButton_5_clicked();
	void on_pushButton_6_clicked();
	void on_pushButton_7_clicked();
	void on_pushButton_8_clicked();
	void on_pushButton_9_clicked();
	void on_pushButton_10_clicked();
	void on_pushButton_11_clicked();
	void on_pushButton_12_clicked();
	void on_pushButton_13_clicked();
	void on_pushButton_14_clicked();
	void on_pushButton_15_clicked();
	void on_pushButton_16_clicked();
	void on_pushButton_17_clicked();
	void on_pushButton_18_clicked();
	void on_pushButton_19_clicked();
	void on_toolButton_12_clicked();

	void on_pushButton_20_clicked();
	void on_pushButton_21_clicked();
	void on_pushButton_22_clicked();
	void on_pushButton_23_clicked();
	void on_pushButton_24_clicked();
	void on_pushButton_25_clicked();
	void on_pushButton_26_clicked();
	void on_pushButton_27_clicked();
	void on_pushButton_28_clicked();
	void on_pushButton_29_clicked();
	void on_pushButton_30_clicked();
	void on_pushButton_31_clicked();
	void on_pushButton_32_clicked();
	void on_pushButton_33_clicked();
	void on_pushButton_34_clicked();
	void on_pushButton_35_clicked();
	void on_pushButton_36_clicked();
	void on_pushButton_37_clicked();
	void on_pushButton_38_clicked();
	void on_pushButton_39_clicked();
	void on_pushButton_40_clicked();
	void on_pushButton_41_clicked();
  private:
	Ui::StyleTestWidget *ui;
	LyWidget *m_pWidget;
	TableItemDelegate *m_pTableItemDelegate;
};

#include <QSpinBox>

class MonthSelector : public QWidget
{
	Q_OBJECT
  public:
	MonthSelector(QWidget *parent = 0);
	void setRange(int begin, int end);
	int begin();
	int end();

  private:
	void init();

  private:
	QSpinBox *m_pBeginSP;
	QSpinBox *m_pEndSP;
};

#include "LyStyledItemDelegate.h"

class TableItemDelegate : public LyStyledItemDelegate
{
	Q_OBJECT

  public:
	TableItemDelegate(QAbstractItemView *view);
	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
	void setEditorData(QWidget *editor, const QModelIndex &index) const override;
	void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
	void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

	QString iconPath() const
	{
		return m_strIconPath;
	}
	void setIconPath(const QString &iconPath);

  private:
	QString m_strIconPath;
};

#endif
