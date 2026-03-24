#include "VMarksWidget.h"
#include <QPainter>
#include <QPixmap>

VMarksWidget::VMarksWidget(QWidget *parent)
	: QWidget(parent)
	, m_pMarkGroup(NULL)
	, m_vLayout(new QVBoxLayout(this))
	, m_iOffset(0)
{
}

VMarksWidget::~VMarksWidget()
{
}

void VMarksWidget::setMarks(const QStringList & marks)
{
	if (m_pMarkGroup == NULL)
	{
		m_pMarkGroup = new QButtonGroup(this);
		m_pMarkGroup->setExclusive(false);
		connect(m_pMarkGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(onMarkClicked(QAbstractButton*)));
	}
	else
	{
		for (QHash<QString, QCheckBox*>::iterator iter = m_markBtns.begin(); iter != m_markBtns.end(); ++iter)
		{
			m_vLayout->removeWidget(iter.value());
			m_pMarkGroup->removeButton(iter.value());
			delete iter.value();
		}

		m_markBtns.clear();
	}

	for (int i = 0; i < marks.size(); ++i)
	{
		if (!m_markBtns.contains(marks[i]))
		{
			QCheckBox * chb = new QCheckBox(marks[i], this);
			m_pMarkGroup->addButton(chb);
			m_markBtns[marks[i]] = chb;
			m_marks.append(marks[i]);
			m_vLayout->addWidget(chb);
		}
	}
}

bool VMarksWidget::isMarkSelected(const QString & mark)
{
	if (m_markBtns.contains(mark)) return m_markBtns[mark]->isChecked();

	return false;
}

void VMarksWidget::setMarkSelected(const QString & mark, bool selected)
{
	if (m_markBtns.contains(mark)) m_markBtns[mark]->setChecked(selected);
}

void VMarksWidget::resizeEvent(QResizeEvent * event)
{
	update();
	QWidget::resizeEvent(event);
}

void VMarksWidget::paintEvent(QPaintEvent * event)
{
	if (m_markBtns.size() >= 2)
	{
		QPoint beginPos = m_markBtns[m_marks.first()]->pos();
		QPoint endPos = m_markBtns[m_marks.last()]->pos();
		QPixmap lineBgPix(m_strLineBg);
		QPixmap lineFgPix(m_strLineFg);

		QPainter painter(this);
		painter.drawPixmap(beginPos.x() + m_iOffset - lineBgPix.width() / 2, beginPos.y() + m_iOffset, lineBgPix.width(), endPos.y() - beginPos.y(), lineBgPix);

		for (int i = 0; i < m_marks.size() - 1; ++i)
		{
			if (m_markBtns[m_marks[i]]->isChecked() && m_markBtns[m_marks[i + 1]]->isChecked())
			{
				QPoint pos1 = m_markBtns[m_marks[i]]->pos();
				QPoint pos2 = m_markBtns[m_marks[i + 1]]->pos();
				painter.drawPixmap(pos1.x() + m_iOffset - lineFgPix.width() / 2, pos1.y() + m_iOffset, lineFgPix.width(), pos2.y() - pos1.y(), lineFgPix);
			}
		}
	}

	QWidget::paintEvent(event);
}

void VMarksWidget::onMarkClicked(QAbstractButton * button)
{
	update();
	emit markSelectChanged(button->text(), button->isChecked());
}
