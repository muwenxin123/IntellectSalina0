#ifndef LY_PHOTOANALYSISWIDGET_H
#define LY_PHOTOANALYSISWIDGET_H

#include <QWidget>
#include "ly_mointorviewmodel.h"

namespace Ui {
class LY_PhotoAnalysisWidget;
}

class LY_PhotoAnalysisWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LY_PhotoAnalysisWidget(QWidget *parent = nullptr);
    ~LY_PhotoAnalysisWidget();

private slots:

	void getWormDataList(WormRecordData data);
	void showContextMenu(const QPoint &pos);

private:
    Ui::LY_PhotoAnalysisWidget *ui;
};

#endif // LY_PHOTOANALYSISWIDGET_H
