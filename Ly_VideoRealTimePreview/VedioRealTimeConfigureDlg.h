#ifndef VIDEOREALTIMECONFIGUREDLG_H
#define VIDEOREALTIMECONFIGUREDLG_H

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>
#include "LyDialog.h"

namespace Ui {
class VedioRealTimeConfigureDlg;
}

class VedioRealTimeConfigureDlg : public LyDialog
{
    Q_OBJECT
public:
    explicit VedioRealTimeConfigureDlg(QWidget *parent = nullptr);
    ~VedioRealTimeConfigureDlg();

	// 初始化
	void Init();

private:
	// 绘制
	void paintEvent(QPaintEvent *event)override;

	void closeEvent(QCloseEvent * event)override;

	void showEvent(QShowEvent * event)override;

	void getUavList(QMap<long long, QString>& idList);

	void ensureUavCbb();

private slots:
	// 确定
	void onSureBtnClicked();
	
	// 退出
	void onQuitBtnClicked();

	// 位置发生变更
	void onViewLocationRadioBtnClicked(bool checked);

	// 显示表示发生变更
	void onShowFlagCheckBoxToggled(bool checked);

	// 是否使用排序发生变更
	void onUseSortCheckBoxToggled(bool checked);
	
	// 方位经纬度发生改变
	void onDspValueChanged(double value);

	// 是否使用友方追踪
	void onUseFriendTrackCheckBoxToggled(bool checked);

	// 主观察者是否发生改变
	void onObeserveComboBoxCurrentTextChanged(const QString &text);

	// 主观察者是否发生改变
	void onOccupyCbbChanged(const QString &text);

private:
    Ui::VedioRealTimeConfigureDlg *ui;
	bool m_isChanged = false;
	int m_location = -1;
	QMap<long long, QString> m_identity2NameMap;
};

#endif // VIDEOREALTIMECONFIGUREDLG_H
