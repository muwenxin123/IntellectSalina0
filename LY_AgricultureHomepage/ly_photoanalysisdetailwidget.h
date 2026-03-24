#ifndef LY_PHOTOANALYSISDETAILWIDGET_H
#define LY_PHOTOANALYSISDETAILWIDGET_H

#include <QWidget>
#include "LyDialog.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPixmap>
#include "ly_mointorviewmodel.h"
namespace Ui {
class LY_PhotoAnalysisDetailWidget;
}

class LY_PhotoAnalysisDetailWidget : public LyDialog
{
    Q_OBJECT

public:
    explicit LY_PhotoAnalysisDetailWidget(QWidget *parent = nullptr);
    ~LY_PhotoAnalysisDetailWidget();

	void setData(WormRecord model);

private:
    Ui::LY_PhotoAnalysisDetailWidget *ui;

	QNetworkAccessManager *networkManager;
	WormRecord m_model;
	void loadImageFromUrl(const QString &url);
};

#endif // LY_PHOTOANALYSISDETAILWIDGET_H
