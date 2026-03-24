#ifndef VMARKSWIDGET_H
#define VMARKSWIDGET_H
#include <QWidget>
#include <QCheckBox>
#include <QButtonGroup>
#include <QVBoxLayout>

class VMarksWidget : public QWidget
{
	Q_OBJECT
	Q_PROPERTY(QString lineBg READ lineBg WRITE setLineBg)
	Q_PROPERTY(QString lineFg READ lineFg WRITE setLineFg)
	Q_PROPERTY(int offset READ offset WRITE setOffset)

public:
	VMarksWidget(QWidget *parent = 0);
	~VMarksWidget();

	QStringList marks() const { return m_marks; }
	void setMarks(const QStringList &marks);
	bool isMarkSelected(const QString &mark);
	void setMarkSelected(const QString &mark, bool selected);

	QString lineBg() const { return m_strLineBg; }
	void setLineBg(const QString &bg) { m_strLineBg = bg; update(); }
	QString lineFg() const { return m_strLineFg; }
	void setLineFg(const QString &fg) { m_strLineFg = fg; update(); }
	int offset() const { return m_iOffset; }
	void setOffset(int offset) { m_iOffset = offset; update(); }

signals:
	void markSelectChanged(QString mark, bool selected);

protected:
	void resizeEvent(QResizeEvent *event);
	void paintEvent(QPaintEvent *event);

private:
	Q_SLOT void onMarkClicked(QAbstractButton * button);

private:
	QString m_strLineBg;
	QString m_strLineFg;
	int m_iOffset;
	QHash<QString, QCheckBox*> m_markBtns;
	QStringList m_marks;
	QButtonGroup * m_pMarkGroup;
	QVBoxLayout * m_vLayout;
};

#endif