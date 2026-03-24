#include "ly_photoanalysiswidget.h"
#include "ui_ly_photoanalysiswidget.h"
#include "LY_AgricultureHomepage.h"
#include "ly_photoanalysisdetailwidget.h"
#include "LyMessageBox.h"
LY_PhotoAnalysisWidget::LY_PhotoAnalysisWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LY_PhotoAnalysisWidget)
{
    ui->setupUi(this);

	QStringList recode_header;
	recode_header << tr2("序号") << tr2("设备名称") << tr2("害虫数量")  << tr2("采样时间") << tr2("操作时间");
	ui->tableWidget->setColumnCount(recode_header.size());
	ui->tableWidget->setHorizontalHeaderLabels(recode_header);
	ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->tableWidget->verticalHeader()->setHidden(true);
	ui->tableWidget->setSelectionBehavior(QTableWidget::SelectRows);
	ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->tableWidget->setAlternatingRowColors(true);
	ui->tableWidget->setRowCount(0);

	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui->tableWidget, &QTableWidget::customContextMenuRequested,
		this, &LY_PhotoAnalysisWidget::showContextMenu);
	//connect(ui->tableWidget, &QTableWidget::customContextMenuRequested, this, &CDlgMissionEdit::on_treeWidget_customContextMenuRequested);
	LY_MointorViewModel *viewModel = LY_AgricultureHomepage::Get().GetViewModel();
	if (viewModel == nullptr)
	{
		return;
	}
	connect(viewModel, &LY_MointorViewModel::getWormDataList, this, &LY_PhotoAnalysisWidget::getWormDataList, Qt::QueuedConnection);

}
void LY_PhotoAnalysisWidget::showContextMenu(const QPoint &pos)
{
	QModelIndex index = ui->tableWidget->indexAt(pos);
	if (!index.isValid()) return;
	QTableWidgetItem *item = ui->tableWidget->item(index.row(), 0); // 获取第一列的item
	if (item == nullptr) return;
	QMenu *menu = new QMenu(this);
	QAction *deleteAction = menu->addAction(tr2("删除"));
	QAction *analysisAction = menu->addAction(tr2("分析"));

	connect(analysisAction, &QAction::triggered, [=]() {
		WormRecord model = item->data(Qt::UserRole).value<WormRecord>();
		LY_PhotoAnalysisDetailWidget *w = new LY_PhotoAnalysisDetailWidget(this);
		w->setData(model);
		w->show();
	});

	connect(deleteAction, &QAction::triggered, [=]() {

		QString strTmp = tr2("请确认是否删除");
		if (QMessageBox::StandardButton::Yes == LyMessageBox::question(this, tr2("提示"), strTmp, QMessageBox::Yes | QMessageBox::Cancel))
		{
			ui->tableWidget->removeRow(index.row());
			LyMessageBox::information(NULL, tr2("提示"), tr2("删除成功"));

		}
		else
		{
			LyMessageBox::information(NULL, tr2("提示"), tr2("删除失败"));
		}
	});
	menu->exec(QCursor::pos());
}
void LY_PhotoAnalysisWidget::getWormDataList(WormRecordData data)
{

	ui->tableWidget->setRowCount(data.rows.size());

	if (data.rows.size() <= 0)
	{
		return;
	}
	for (size_t i = 0; i < data.rows.size(); i++)
	{
		WormRecord worm = data.rows.at(i);
		//ui->tableWidget->setRowHeight(i, 50);
		QTableWidgetItem *item1 = new QTableWidgetItem(QString::number(i + 1));
		item1->setData(Qt::UserRole, QVariant::fromValue(worm));
		QTableWidgetItem *item2 = new QTableWidgetItem(QString::fromStdString(worm.deviceAddr));
		QTableWidgetItem *item3 = new QTableWidgetItem(QString::fromStdString(worm.deviceAddr));
		QTableWidgetItem *item4 = new QTableWidgetItem(QString::fromStdString(worm.createTime));
		QTableWidgetItem *item5 = new QTableWidgetItem(QString::fromStdString(worm.verifyTime));

		ui->tableWidget->setItem(i, 0, item1);
		ui->tableWidget->setItem(i, 1, item2);
		ui->tableWidget->setItem(i, 2, item3);
		ui->tableWidget->setItem(i, 3, item4);
		ui->tableWidget->setItem(i, 4, item5);

		ui->tableWidget->item(i, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		ui->tableWidget->item(i, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		ui->tableWidget->item(i, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		ui->tableWidget->item(i, 3)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		ui->tableWidget->item(i, 4)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	}
}
LY_PhotoAnalysisWidget::~LY_PhotoAnalysisWidget()
{
    delete ui;
}
