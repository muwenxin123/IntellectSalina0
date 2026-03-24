#include "MouseInfoWidget.h"
#include "QCString.h"

MouseInfoWidget::MouseInfoWidget(QWidget *parent)
	: QLabel(parent)
{
}

MouseInfoWidget::~MouseInfoWidget()
{
}

void MouseInfoWidget::updateLocationInfo(const QPoint & pos)
{
	setText(tr2("%1 %2").arg(pos.x()).arg(pos.y()));
}
