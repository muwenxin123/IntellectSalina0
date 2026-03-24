#include "ly_cameralayoutwidget.h"
#include "ui_ly_cameralayoutwidget.h"
#include <QString>
#include <QDebug>
#include "QCString.h"
#include "LY_AgricultureHomepage.h"

//LY_CameraLayoutWidget* LY_CameraLayoutWidget::m_instance = nullptr;

LY_CameraLayoutWidget::LY_CameraLayoutWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::LY_CameraLayoutWidget)
	, m_IsRightClick(false)
	, m_CurrentSelectWidget(1)
	, m_CurrentOneWidget(1)
	, m_FullScreenWidget(nullptr)
	, m_IsFullScreenShow(false)

{
	ui->setupUi(this);


		m_viewModel = LY_AgricultureHomepage::Get().GetViewModel();

		initUi();

		m_videoUrlVec.clear();

}
void LY_CameraLayoutWidget::showEvent(QShowEvent *event)
{

}

LY_CameraLayoutWidget::~LY_CameraLayoutWidget()
{
	//m_instance = nullptr;
	delete ui;
}
void LY_CameraLayoutWidget::stopAllPlayer()
{
	for (size_t i = 0; i < cameraWidgetVec.size(); i++)
	{
		/*if (i < m_videoUrlVec.size())
		{*/
			CameraWidget* widget = cameraWidgetVec.at(i);
			widget->stopPlay();
		//}
	}
}
void LY_CameraLayoutWidget::setUrlVec(SensorListModelVec vec)
{

	/*m_videoUrlVec.push_back(QPair<QString, QString>(model.mointID, model.requsetUrl));

	QVector<QPair<QString, QString>> filteredVec;
	QSet<QString> keySet;

	for (const auto &pair : m_videoUrlVec) {
		const QString &key = pair.first;
		if (!keySet.contains(key)) {
			keySet.insert(key);
			filteredVec.append(pair);
		}
	}*/

	//m_videoUrlVec = filteredVec; 


	for (size_t i = 0; i < cameraWidgetVec.size(); i++)
	{
		CameraWidget* widget = cameraWidgetVec.at(i);
		widget->stopPlay();
	}
	
	for (size_t i = 0; i < cameraWidgetVec.size(); i++)
	{
		CameraWidget* widget = cameraWidgetVec.at(i);
		if (i < vec.size())
		{
			//m_videoUrlVec.push_back(QPair<QString, QString>(vec.at(i).moin, vec.at(i).requsetUrl));
			widget->startHarmManagerPlay(vec.at(i).requsetUrl);
		}
		else
		{
			widget->startHarmManagerPlay("");
		}
	}

	
}
void LY_CameraLayoutWidget::initUi()
{

	ui->pushButton->hide();
	ui->pushButton_2->hide();
	ui->pushButton_3->hide();
	ui->pushButton_4->hide();
	ui->pushButton_5->hide();


	int number = 0;
	for (int i = 1; i <= 4; ++i) {
		for (int j = 1; j <= 4; ++j) {
			WidgetLayout wLayout;
			number++;
			CameraWidget* widget = new CameraWidget();
			widget->setObjectName(QString("Widget_%1").arg(number));
			widget->installEventFilter(this);
			widget->setStyleSheet("border-width: 1px; border-style: solid; border-color: red;");

			QLabel* label = new QLabel(QString(tr2("通道%1")).arg(number));
			label->setStyleSheet("border: none;");

			QVBoxLayout * layout = new QVBoxLayout();
			layout->addWidget(label);

			widget->setLayout(layout);

			ui->gridLayout->addWidget(widget, i, j);

			cameraWidgetVec.append(widget);

			//保存所有控件
			wLayout.widget = widget;
			wLayout.label = label;
			wLayout.layout = layout;
			m_AllWidgetList.append(wLayout);
		}
	}

	//for (size_t i = 0; i < cameraWidgetVec.size(); i++)
	//{
	//	if (i < m_videoUrlVec.size())
	//	{
	//		CameraWidget* widget = cameraWidgetVec.at(i);

	//		widget->startHarmManagerPlay(m_videoUrlVec.at(i).second);
	//	}
	//}

	QMenu* menuOne = new QMenu(tr2("切换1画面"));
	for (int i = 1; i <= 16; ++i) {
		QAction* action = new QAction(QString(tr2("切换%1通道")).arg(i));
		menuOne->addAction(action);
	}
	connect(menuOne, SIGNAL(triggered(QAction*)), this, SLOT(triggeredOne(QAction*)));

	QMenu* menuFour = new QMenu(tr2("切换4画面"));
	for (int i = 0; i < 4; ++i) {
		QAction* action = new QAction(QString(tr2("切换%1-%2通道")).arg(i * 4 + 1).arg((i + 1) * 4));
		menuFour->addAction(action);
	}
	connect(menuFour, SIGNAL(triggered(QAction*)), this, SLOT(triggeredFour(QAction*)));

	QMenu* menuEnght = new QMenu(tr2("切换8画面"));
	for (int i = 0; i < 2; ++i) {
		QAction* action = new QAction(QString(tr2("切换%1-%2通道")).arg(i * 8 + 1).arg((i + 1) * 8));
		menuEnght->addAction(action);
	}
	connect(menuEnght, SIGNAL(triggered(QAction*)), this, SLOT(triggeredEnght(QAction*)));

	QAction* actionSixTeen = new QAction(tr2("切换16画面"));
	connect(actionSixTeen, SIGNAL(triggered(bool)), this, SLOT(cutSixTeenWidget()));

	//QAction* actionFullScreen = new QAction(tr2("全屏显示"));
	//connect(actionFullScreen, SIGNAL(triggered(bool)), this, SLOT(fullScreenShowWidget()));

	m_Menu = new QMenu(this);
	//m_Menu->addAction(actionFullScreen);
	m_Menu->addMenu(menuOne);
	m_Menu->addMenu(menuFour);
	m_Menu->addMenu(menuEnght);
	m_Menu->addAction(actionSixTeen);

	//
	m_FullScreenWidget = new QWidget();
	m_FullScreenWidget->installEventFilter(this);
}
void LY_CameraLayoutWidget::addPhoto()
{
	for (size_t i = 0; i < cameraWidgetVec.size(); i++)
	{
		if (i < m_videoUrlVec.size())
		{
			CameraWidget* widget = cameraWidgetVec.at(i);
			m_photoVec.push_back(make_pair(m_videoUrlVec.at(i).first, widget->m_image));
		}
	}
}


bool LY_CameraLayoutWidget::eventFilter(QObject *watched, QEvent *event)
{
	for (int i = 0; i < m_AllWidgetList.size(); ++i) {
		if (watched == m_AllWidgetList.at(i).widget) {
			if (m_IsRightClick) {
				m_CurrentSelectWidget = i;
				qDebug() << "当前为" << m_CurrentSelectWidget;
				m_Menu->move(cursor().pos());
				m_Menu->show();

				m_IsRightClick = false;
				break;
			}
			if (event->type() == QEvent::MouseButtonDblClick && !m_IsFullScreenShow) {
				m_CurrentSelectWidget = i;
				qDebug() << "当前为" << m_CurrentSelectWidget;
				//fullScreenShowWidget();
			}
		}
	}
	if (watched == m_FullScreenWidget) {
		if (event->type() == QEvent::MouseButtonDblClick && m_IsFullScreenShow) {
			m_AllWidgetList[m_CurrentSelectWidget].widget->setLayout(m_FullScreenWidget->layout());

			m_IsFullScreenShow = false;
			m_FullScreenWidget->close();
		}
	}
	return QWidget::eventFilter(watched, event);
}
//void LY_CameraLayoutWidget::closeEvent(QMouseEvent *event)
//{
//	m_instance = nullptr;
//}
void LY_CameraLayoutWidget::mousePressEvent(QMouseEvent *event)
{

}

void LY_CameraLayoutWidget::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::RightButton) {
		m_IsRightClick = true;
	}
}

void LY_CameraLayoutWidget::on_pushButton_clicked()
{
	if (this == nullptr)
	{
		return;
	}
	cutOneWidget(m_CurrentOneWidget);
}

void LY_CameraLayoutWidget::on_pushButton_2_clicked()
{
	if (this == nullptr)
	{
		return;
	}
	cutFourWidget(m_CurrentOneWidget);
}

void LY_CameraLayoutWidget::on_pushButton_3_clicked()
{
	if (this == nullptr)
	{
		return;
	}
	cutEnghtWidget(m_CurrentOneWidget);
}

void LY_CameraLayoutWidget::delAllWidget()
{
	for (int i = 0; i < m_AllWidgetList.size(); ++i) {
		QWidget* w = m_AllWidgetList[i].widget;
		ui->gridLayout->removeWidget(w);
		m_AllWidgetList[i].widget->setVisible(false);
	}
}

int LY_CameraLayoutWidget::strCutNumber(QString str)
{
	QString number;
	for (int i = 0; i < str.length(); i++) {
		if (str[i] >= '0' && str[i] <= '9') {
			number.append(str[i]);
		}
	}
	return number.toInt();
}

int LY_CameraLayoutWidget::strCutNumber(QString str, QString sep)
{
	QString number;
	for (int i = 0; i < str.length(); i++) {
		if (str[i] >= '0' && str[i] <= '9') {
			number.append(str[i]);
		}
		if (str[i] == sep) {
			return number.toInt();
		}
	}
	return 1;
}

void LY_CameraLayoutWidget::triggeredOne(QAction *action)
{
	int number = strCutNumber(action->text());
	m_CurrentOneWidget = number;

	cutOneWidget(number);
}

void LY_CameraLayoutWidget::triggeredFour(QAction *action)
{
	int number = strCutNumber(action->text(), "-");
	m_CurrentOneWidget = number;

	cutFourWidget(number);
}

void LY_CameraLayoutWidget::triggeredEnght(QAction *action)
{
	int number = strCutNumber(action->text(), "-");
	m_CurrentOneWidget = number;

	cutEnghtWidget(number);
}

void LY_CameraLayoutWidget::fullScreenShowWidget()
{
	m_FullScreenWidget->setLayout(m_AllWidgetList[m_CurrentSelectWidget].widget->layout());
	m_FullScreenWidget->showFullScreen();
	m_IsFullScreenShow = true;
}

void LY_CameraLayoutWidget::cutOneWidget(int indexWidget)
{

	if (m_AllWidgetList.isEmpty() || m_AllWidgetList.size() < indexWidget) {
		return;
	}
	indexWidget--;
	if (indexWidget < 0)
	{
		return;
	}

	delAllWidget();

	ui->gridLayout->addWidget(m_AllWidgetList[indexWidget].widget);
	m_AllWidgetList[indexWidget].widget->setVisible(true);
}

void LY_CameraLayoutWidget::cutFourWidget(int indexWidget)
{
	if (indexWidget <=  0)
	{
		return;
	}
	if (m_AllWidgetList.isEmpty()) {
		return;
	}
	delAllWidget();

	int number = indexWidget - 1;
	//如果切换布局第一个通道+需要切换的画面数大于总数就--
	while (number + 4 > m_AllWidgetList.size()) {
		number--;
	}
	if (number < 0)
	{
		return;
	}

	for (int i = 0; i < 2; ++i) {
		for (int j = 0; j < 2; ++j) {
			ui->gridLayout->addWidget(m_AllWidgetList[number].widget, i, j);
			m_AllWidgetList[number].widget->setVisible(true);

			number++;
		}
	}
}

void LY_CameraLayoutWidget::cutEnghtWidget(int indexWidget)
{
	if (m_AllWidgetList.isEmpty()) {
		return;
	}
	delAllWidget();

	int number = indexWidget - 1;
	//如果切换布局第一个通道+需要切换的画面数大于总数就--
	while (number + 8 > m_AllWidgetList.size()) {
		number--;
	}

	for (int i = 0; i < 2; ++i) {
		for (int j = 0; j < 4; ++j) {
			ui->gridLayout->addWidget(m_AllWidgetList[number].widget, i, j);
			m_AllWidgetList[number].widget->setVisible(true);

			number++;
		}
	}
}

void LY_CameraLayoutWidget::cutOneBigSevenLitteWidget(int indexWidget)
{
	if (m_AllWidgetList.isEmpty()) {
		return;
	}
	delAllWidget();

	int number = indexWidget - 1;
	//如果切换布局第一个通道+需要切换的画面数大于总数就--
	while (number + 8 > m_AllWidgetList.size()) {
		number--;
	}

	m_AllWidgetList[number].widget->setVisible(true);
	m_AllWidgetList[number + 1].widget->setVisible(true);
	m_AllWidgetList[number + 2].widget->setVisible(true);
	m_AllWidgetList[number + 3].widget->setVisible(true);
	m_AllWidgetList[number + 4].widget->setVisible(true);
	m_AllWidgetList[number + 5].widget->setVisible(true);
	m_AllWidgetList[number + 6].widget->setVisible(true);
	m_AllWidgetList[number + 7].widget->setVisible(true);

	ui->gridLayout->addWidget(m_AllWidgetList[number].widget, 0, 0, 3, 3);
	ui->gridLayout->addWidget(m_AllWidgetList[number + 1].widget, 0, 3);
	ui->gridLayout->addWidget(m_AllWidgetList[number + 2].widget, 1, 3);
	ui->gridLayout->addWidget(m_AllWidgetList[number + 3].widget, 2, 3);
	ui->gridLayout->addWidget(m_AllWidgetList[number + 4].widget, 3, 3);
	ui->gridLayout->addWidget(m_AllWidgetList[number + 5].widget, 3, 2);
	ui->gridLayout->addWidget(m_AllWidgetList[number + 6].widget, 3, 1);
	ui->gridLayout->addWidget(m_AllWidgetList[number + 7].widget, 3, 0);

}

void LY_CameraLayoutWidget::cutSixTeenWidget()
{
	if (m_AllWidgetList.isEmpty()) {
		return;
	}
	delAllWidget();

	m_CurrentOneWidget = 1;
	int number = 0;


	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			ui->gridLayout->addWidget(m_AllWidgetList[number].widget, i, j);
			m_AllWidgetList[number].widget->setVisible(true);

			number++;
		}
	}
}

void LY_CameraLayoutWidget::on_pushButton_4_clicked()
{
	cutSixTeenWidget();
}

void LY_CameraLayoutWidget::on_pushButton_5_clicked()
{
	cutOneBigSevenLitteWidget(m_CurrentOneWidget);
}

