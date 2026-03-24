#ifndef LY_PHOTOLAYOUTWIDGET_H
#define LY_PHOTOLAYOUTWIDGET_H
#include "ly_cameralayoutwidget.h"
#include <QWidget>
#include <QEvent>
#include <QMouseEvent>
#include <QMenu>
#include <QLabel>
#include <QLayout>
#include "ly_mointorviewmodel.h"
#include "camerawidget.h"
#include <QPainter>
#include <opencv2/opencv.hpp>
#include "DataManage/AgriFarm/AgriHomePageDecl.h"
#include "LY_AgricultureHomepageActivator.h"

namespace Ui {
class LY_PhotoLayoutWidget;
}

class LY_PhotoLayoutWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LY_PhotoLayoutWidget(QWidget *parent = nullptr);
    ~LY_PhotoLayoutWidget();
	void initUi();

	vector<FarmPestPhotoInfication> m_pestPhotoVec;
	void setPhoto(QVector<pair<QString, QImage>> pVec);

protected:
	bool eventFilter(QObject *watched, QEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);

	void paintEvent(QPaintEvent *event) override;
	public slots:

	//切换1画面
	void cutOneWidget(int indexWidget);
	//切换1画面
	void on_pushButton_clicked();
	//切换4
	void on_pushButton_2_clicked();
	//切换8
	void on_pushButton_3_clicked();
	//切换16
	void on_pushButton_4_clicked();
	//切换1大7小
	void on_pushButton_5_clicked();



private:
	//创建通道布局
	//QWidget* creationLayout();
	//删除所有通道
	void delAllWidget();
	//字符串截取数字
	int strCutNumber(QString str);
	int strCutNumber(QString str, QString sep);


private slots:
	void triggeredOne(QAction *action);
	void triggeredFour(QAction *action);
	void triggeredEnght(QAction *action);

	//全屏显示通道
	void fullScreenShowWidget();


	//切换4画面
	void cutFourWidget(int indexWidget);
	//切换8画面
	void cutEnghtWidget(int indexWidget);
	//切换8画面1大7小
	void cutOneBigSevenLitteWidget(int indexWidget);
	//切换16画面
	void cutSixTeenWidget();

private:
	Ui::LY_PhotoLayoutWidget *ui;

	bool m_IsRightClick;
	int m_CurrentSelectWidget;
	int m_CurrentOneWidget; // 保存窗口第一个通道
	QWidget* m_FullScreenWidget;
	bool m_IsFullScreenShow;

	QMenu* m_Menu;
	QList<WidgetLayout> m_AllWidgetList;

	QVector<pair<QString, QImage>> m_photoVec;
	LY_MointorViewModel *viewModel;
	

	QVector<QLabel * > photoWidgetVec;

	cv::Mat QImageToMat(const QImage &image);

	std::map<int, std::string> name_map2{
		{ 0, "maize rough dwarf disease" },
		{ 1, "puccinia polysra" },
		{ 2, "leaf blight of corn" },
		{ 3, "leaf spot of corn" },
		{ 4, "Maize sheath blight" },
		{ 5, "Maize Head Smut" },
		{ 6, "common smut" },
		{ 7, "Maize stalk rot" },
		{ 8, "Bacterial stem rot of Maize" },
		{ 9,"gray leaf spot" },
		{ 10,"corn brown spot" },
		{ 11,"Alternaria alternata leaf blight of Maize" },
		{ 12,"Curvularia leaf spot of Maize" },
		{ 13,"Corn anthracnose" },
		{ 14,"Corn eye spot" },
		{ 15,"Corn take all" },
		{ 16,"Maize Downy mildew" },
		{ 17,"Corn dry rot" },
		{ 18,"Maize scab" },
		{ 19,"Maize dwarf mosaic virus disease" },
		{ 20,"Maize stripe dwarf disease" },
		{ 21,"Empty corn stalk" },
		{ 22,"Corn top rot" },
		{ 23,"aphid" },
		{ 24,"bollworm" },
		{ 25,"Beet armyworm" },
		{ 26,"Spodoptera litura" },
		{ 27,"corn borer" },
		{ 28,"armyworm" },
		{ 29,"Spodoptera frugiperda" },
		{ 30,"Athetis lepigone" },
		{ 31,"thrips" },
		{ 32,"cutworm" },
		{ 33,"Corn sunflower mealybug" },
		{ 34,"Scarab Beetle" },
		{ 35,"Oriental Migratory Locust" },
		{ 36,"Atractomorpha sinensis" },
		{ 37,"Miliary stink bug" }
	};


};


#endif // LY_PHOTOLAYOUTWIDGET_H
