#include "CDlgCropGrowStageWidget.h"
#include "ui_CDlgCropGrowStageWidget.h"
#include "QCString.h"
#include "IAgroTaskManagerActivator.h"
#include "QDateTime"
#include "QPushButton"
#include "LyMessageBox.h"
#include "QSqlQuery"
#include "QBoxLayout"
#include "QLabel"
#include "QHeaderView"
#include "QStringList"
#include "QStackedWidget"
#include "QTreeWidget"
#include "QLineEdit"
#include "QTableWidget"

using namespace qnzkna::AgroTask;

CDlgCropGrowStageWidget::CDlgCropGrowStageWidget(QWidget *parent) :
	LyDialog(tr2("农作物生长阶段"), parent),
	ui(new Ui::CDlgCropGrowStageWidget)
{
	ui->setupUi(centralWidget());
	this->setResizable(true);
	this->resize(900, 500);

	m_pIAgroTaskService = IAgroTaskManagerActivator::Get().getPIAgroTaskService();

	init_widget();

	init_connect();
}

CDlgCropGrowStageWidget::~CDlgCropGrowStageWidget()
{
    delete ui;
}

void CDlgCropGrowStageWidget::init_widget()
{
	ui->treeWidget->clear();

	QStringList tree_header_list;
	tree_header_list << tr2("阶段") << tr2("添加") << tr2("删除");
	ui->treeWidget->setHeaderLabels(tree_header_list);
	ui->treeWidget->setColumnCount(tree_header_list.size());

	for (int i = 0; i < tree_header_list.size(); i++) {
		ui->treeWidget->header()->setSectionResizeMode(i, QHeaderView::Fixed);
	}

	ui->treeWidget->header()->setDefaultAlignment(Qt::AlignCenter);
	ui->treeWidget->header()->setStretchLastSection(false);
	ui->treeWidget->header()->setSectionResizeMode(0, QHeaderView::ResizeMode::Stretch);

	seedSubStateVec.clear();
	seedSubStateVec.resize(5);

	seedSubSubStateVec.clear();
	seedSubSubStateVec.resize(5);
	seedSubSubStateVec[3].resize(3);

	m_AgroRefDataInfoVec = m_pIAgroTaskService->getAgroRefDataInfoVec();

	QString preText2{ "" };
	int curType = 1;
	for (const auto &info : m_AgroRefDataInfoVec) {
		QString t2 = QString::fromLocal8Bit(info.cPlanSubState);
		QString t3 = QString::fromLocal8Bit(info.cPlanSsubState);
		
		if (t2 != "" && preText2 != t2) {
			seedSubStateVec[info.nPlanState].push_back(t2);
		}
		if (t3 != "") {
			seedSubSubStateVec[info.nPlanState][info.nPlanSubState].push_back(t3);
		}
		preText2 = t2;

	}

	QString preText1{ "" };
	for (const auto &info : m_AgroRefDataInfoVec) {
		QString t1 = QString::fromLocal8Bit(info.cPlanState);
		if (t1 != "" && preText1 != t1) {
			add_top_level_item(curType++, t1);
		}
		preText1 = t1;

	}

	ui->treeWidget->expandAll();
}

void CDlgCropGrowStageWidget::init_connect()
{
	//右侧详情界面切换
	connect(ui->treeWidget, &QTreeWidget::itemClicked, this, [&](QTreeWidgetItem *item, int column) {
		int cur_index = item->data(column, Qt::UserRole + 1).toInt();
		ui->stackedWidget->setCurrentIndex(cur_index);
	});
}

void CDlgCropGrowStageWidget::add_top_level_item(int type, const QString &name)
{
	auto root_item = new QTreeWidgetItem(ui->treeWidget);
	root_item->setTextAlignment(0, Qt::AlignCenter);
	//阶段名称
	root_item->setText(0, name);
	root_item->setData(0, Qt::UserRole, type);//顶层种植状态
	root_item->setData(0, Qt::UserRole + 1, item_index);//状态下标，用来区分右侧窗口显示

	//添加
	auto add_button = new QPushButton("+");
	add_button->setFixedSize(25, 25);
	ui->treeWidget->setItemWidget(root_item, 1, add_button);
	ui->treeWidget->setColumnWidth(1, 35);
	root_item->setData(1, Qt::UserRole, type);
	root_item->setData(1, Qt::UserRole + 1, item_index);

	connect(add_button, &QPushButton::clicked, this, [&, root_item, type]() {
		auto create_widget = new CreateGrowStageWidget(this);
		connect(create_widget, &CreateGrowStageWidget::sig_create_stage, this,
			[&, root_item, type](const QString &name, const QString &details) {
			add_child_item(root_item, type, name);
		});
		create_widget->raise();
		create_widget->show();
	});

	//删除
	auto delete_button = new QPushButton("-");
	delete_button->setFixedSize(25, 25);
	ui->treeWidget->setItemWidget(root_item, 2, delete_button);
	ui->treeWidget->setColumnWidth(2, 35);
	root_item->setData(2, Qt::UserRole, type);
	root_item->setData(2, Qt::UserRole + 1, item_index);

	connect(delete_button, &QPushButton::clicked, this, [&, root_item]() {
		//ui->treeWidget
	});

	//右侧详情界面
	create_details_widget(item_index, name, type, -1, -1);

	item_index++;

	if (type < seedSubStateVec.size()) {
		auto tempVec = seedSubStateVec[type];
		for (int i = 0; i < tempVec.size(); i++) {
			add_child_item(root_item, type, tempVec[i], i);
		}
	}

}

void CDlgCropGrowStageWidget::create_details_widget(int index, const QString &name, int zt, int zzt, int zzzt)
{
	auto details_widget = new QWidget;
	ui->stackedWidget->insertWidget(index, details_widget);
	auto vlayout = new QVBoxLayout;
	vlayout->setContentsMargins(10, 0, 0, 0);
	details_widget->setLayout(vlayout);

	auto title_label = new QLabel(name);
	vlayout->addWidget(title_label, 0, Qt::AlignLeft);

	auto hlayout = new QHBoxLayout;
	hlayout->setContentsMargins(0, 0, 0, 0);
	auto nameLabel = new QLabel(tr2("阶段天数："));
	nameLabel->setFixedWidth(50);
	hlayout->addWidget(nameLabel);

	auto leTime = new QLineEdit;
	leTime->setModified(false);
	hlayout->addWidget(leTime);

	//显示信息
	vlayout->addLayout(hlayout);

	auto paramTablewidget = new QTableWidget;
	vlayout->addWidget(paramTablewidget);

	paramTablewidget->verticalHeader()->setVisible(false);
	paramTablewidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	paramTablewidget->setCornerButtonEnabled(false);
	paramTablewidget->setFocusPolicy(Qt::NoFocus);
	paramTablewidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	paramTablewidget->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);

	QStringList hheader;
	hheader << tr2("参数名称") << tr2("播种日T0 ~ （天）") << tr2("处理办法");
	paramTablewidget->setColumnCount(hheader.size());
	paramTablewidget->setHorizontalHeaderLabels(hheader);
	paramTablewidget->setDisabled(false);

	auto cmp = [](AgroRefDataSubInfo a1, AgroRefDataSubInfo a2) {
		if (a1.nTimeParam < a2.nTimeParam) return true;
		return false;
	};

	int preTime = -30;
	int timeAll = 0;
	for (const auto &info : m_AgroRefDataInfoVec) {
		if (info.nPlanState == zt) {
			if ((info.nPlanSubState == zzt && info.nPlanSsubState == zzzt) ||
				(info.nPlanSubState == zzt && zzzt == -1) ||
				(zzt == -1 && zzzt == -1)) {
				timeAll += info.nDataTime;
				auto infoVec = info.vCAgroRefDataSubInfoVec;
				int curRow = paramTablewidget->rowCount();
				paramTablewidget->setRowCount(curRow + infoVec.size());

				std::sort(infoVec.begin(), infoVec.end(), cmp);

				for (int i = 0; i < infoVec.size(); i++) {
					QTableWidgetItem* item0 = new QTableWidgetItem(QString::fromLocal8Bit(infoVec[i].cParamName));
					item0->setTextAlignment(Qt::AlignCenter);
					paramTablewidget->setItem(curRow + i, 0, item0);

					QTableWidgetItem* item1 = new QTableWidgetItem(QString::number(preTime + infoVec[i].nTimeParam));
					item1->setTextAlignment(Qt::AlignCenter);
					paramTablewidget->setItem(curRow + i, 1, item1);

					QString tempText = QString::fromLocal8Bit(infoVec[i].cHandMethod);
					if (tempText == "") {
						int mmin = infoVec[i].dMinParam;
						int mmax = infoVec[i].dMaxParam;
						if (mmin != 0) {
							tempText += QString::number(mmin) + "%";
						}
						if (mmax != 0) {
							tempText += " ~ ";
							tempText += QString::number(mmax) + "%";
						}
					}
					QTableWidgetItem* item2 = new QTableWidgetItem(tempText);
					item2->setTextAlignment(Qt::AlignCenter);
					item2->setToolTip(QString::fromLocal8Bit(infoVec[i].cHandMethod));
					paramTablewidget->setItem(curRow + i, 2, item2);
				}
			}
		}
		preTime += std::abs(info.nDataTime);
	}

	leTime->setText(QString::number(timeAll));

	auto hlayout2 = new QHBoxLayout;
	hlayout2->addStretch();
	auto edit_button = new QPushButton(tr2("编辑"));
	hlayout2->addWidget(edit_button);
	auto cancel_button = new QPushButton(tr2("取消"));
	hlayout2->addWidget(cancel_button);
	auto save_button = new QPushButton(tr2("保存"));
	hlayout2->addWidget(save_button);
	vlayout->addLayout(hlayout2, 0);
}

void CDlgCropGrowStageWidget::add_child_item(QTreeWidgetItem *parent, int type, const QString &name, int index1 /*= -1*/, int index2 /*= -1*/)
{
	//阶段名称
	auto cur_item = new QTreeWidgetItem(parent);
	cur_item->setTextAlignment(0, Qt::AlignCenter);
	cur_item->setText(0, name);
	cur_item->setData(0, Qt::UserRole, type);
	cur_item->setData(0, Qt::UserRole + 1, item_index);

	//添加
	auto add_button = new QPushButton("+");
	add_button->setFixedSize(25, 25);
	ui->treeWidget->setItemWidget(cur_item, 1, add_button);
	cur_item->setData(1, Qt::UserRole, type);
	cur_item->setData(1, Qt::UserRole + 1, item_index);

	connect(add_button, &QPushButton::clicked, this, [&, cur_item, type]() {
		auto create_widget = new CreateGrowStageWidget(this);
		connect(create_widget, &CreateGrowStageWidget::sig_create_stage, this,
			[&, cur_item, type](const QString &name, const QString &details) {
			add_child_item(cur_item, type, name);
		});
		create_widget->raise();
		create_widget->show();
	});

	//删除
	auto delete_button = new QPushButton("-");
	delete_button->setFixedSize(25, 25);
	ui->treeWidget->setItemWidget(cur_item, 2, delete_button);
	cur_item->setData(2, Qt::UserRole, type);
	cur_item->setData(2, Qt::UserRole + 1, item_index);

	//右侧详情界面
	create_details_widget(item_index, name, type, index1, index2);

	item_index++;

	if (index2 == -1) {
		if (type < seedSubSubStateVec.size()) {
			auto tempVec1 = seedSubSubStateVec[type];
			if (index1 < tempVec1.size()) {
				auto tempVec2 = tempVec1[index1];
				for (int i = 0; i < tempVec2.size(); i++) {
					add_child_item(cur_item, type, tempVec2[i], index1, i);
				}
			}
		}
	}
}

CreateGrowStageWidget::CreateGrowStageWidget(QWidget *parent /*= nullptr*/)
	:LyDialog(tr2("创建农作物生长阶段"), parent)
{
	init_widget();
}

CreateGrowStageWidget::~CreateGrowStageWidget()
{

}

void CreateGrowStageWidget::init_widget()
{
	auto main_vlayout = new QVBoxLayout;
	setLayout(main_vlayout);

	auto hlayout1 = new QHBoxLayout;
	hlayout1->addWidget(new QLabel(tr2("阶段名称：")));
	name_lineedit_ = new QLineEdit;
	hlayout1->addWidget(name_lineedit_, 1);
	main_vlayout->addLayout(hlayout1);

	auto hlayout2 = new QHBoxLayout;
	hlayout2->addWidget(new QLabel(tr2("内容介绍：")));
	details_textedit_ = new QTextEdit;
	hlayout2->addWidget(details_textedit_, 1);
	main_vlayout->addLayout(hlayout2);

	auto hlayout3 = new QHBoxLayout;
	hlayout3->addStretch();
	main_vlayout->addLayout(hlayout3);

	auto ok_button = new QPushButton(tr2("确定"));
	connect(ok_button, &QPushButton::clicked, this, [&]() {
		auto t1 = name_lineedit_->text();
		auto t2 = details_textedit_->toPlainText();
		if (t1 == "") {
			LyMessageBox::warning(this, tr2("提示"), tr2("阶段名称为空！"));
			return;
		}
		emit sig_create_stage(t1, t2);
		this->close();
	});
	hlayout3->addWidget(ok_button);

	auto no_button = new QPushButton(tr2("取消"));
	connect(no_button, &QPushButton::clicked, this, [&]() {
		this->close();
	});
	hlayout3->addWidget(no_button);
}
