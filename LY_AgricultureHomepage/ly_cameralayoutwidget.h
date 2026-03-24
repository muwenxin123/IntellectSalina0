#ifndef LY_CAMERALAYOUTWIDGET_H
#define LY_CAMERALAYOUTWIDGET_H

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

QT_BEGIN_NAMESPACE
namespace Ui {
class LY_CameraLayoutWidget;
}
QT_END_NAMESPACE
struct WidgetLayout
{
    QWidget* widget;
    QVBoxLayout* layout;
    QLabel* label;
    WidgetLayout() {}
};

class PhotoWidget : public QWidget {
	Q_OBJECT

public:
	explicit PhotoWidget(QWidget *parent = nullptr) : QWidget(parent) {}
	QImage m_image;
protected:
	void paintEvent(QPaintEvent *event) override {
		QPainter painter(this);
		// 加载或者设置你的QImage对象
		//QImage image("path/to/your/image.png"); // 确保图像路径正确
		if (!m_image.isNull()) {
		//	painter.drawImage(0, 0, m_image); // 在(0,0)位置绘制图像

			painter.drawImage((width() - m_image.width()) / 2, (height() - m_image.height()) / 2, m_image);
		}
		else {
			painter.drawText(0, 0, "Image not loaded"); // 如果没有图像，显示文本
		}
	}
};
class LY_CameraLayoutWidget : public QWidget
{
    Q_OBJECT

public:
	explicit LY_CameraLayoutWidget(QWidget *parent = nullptr);
    ~LY_CameraLayoutWidget();
    void initUi();

	//static LY_CameraLayoutWidget *instance(QWidget *parent = nullptr);
	void delAllWidget();
	void addPhoto();
	QVector<pair<QString, QImage>> m_photoVec;

	void setUrlVec(SensorListModelVec vec);

	void stopAllPlayer();

protected:
    bool eventFilter(QObject *watched, QEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
	// 覆盖关闭事件，确保正确释放实例
	//void closeEvent(QCloseEvent *event);


	
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

protected:

	void showEvent(QShowEvent *event);

private:
    //创建通道布局
    //QWidget* creationLayout();
    //删除所有通道
    
    //字符串截取数字
    int strCutNumber(QString str);
    int strCutNumber(QString str, QString sep);

	LY_MointorViewModel *m_viewModel;


	//LY_CameraLayoutWidget(const LY_CameraLayoutWidget&) = delete;
	//LY_CameraLayoutWidget& operator=(const LY_CameraLayoutWidget&) = delete;

	//static LY_CameraLayoutWidget* m_instance; // 静态实例指针
	
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
    Ui::LY_CameraLayoutWidget *ui;

    bool m_IsRightClick;
    int m_CurrentSelectWidget;
    int m_CurrentOneWidget; // 保存窗口第一个通道
    QWidget* m_FullScreenWidget;
    bool m_IsFullScreenShow;

    QMenu* m_Menu;
    QList<WidgetLayout> m_AllWidgetList;
	//QVector<QString> m_videoUrlVec;

	QVector<QPair<QString, QString>> m_videoUrlVec;
	QVector<CameraWidget* > cameraWidgetVec;

	//std::map<QString, QString>m_videoUrlmap;

	
};

#endif // LY_CAMERALAYOUTWIDGET_H


