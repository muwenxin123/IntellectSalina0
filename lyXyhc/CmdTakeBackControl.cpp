#include "CmdTakeBackControl.h"
#include "ui_CmdTakeBackControl.h"
#include "lyXyhc.h"
#include <QDb.h>

struct _PilotUav
{
	int		uav;
	int		pilot;
	QString	name;
};

void SendManualCommand(int nID, int nSubType, int nCmd, bool bNo = true);

QTreeWidgetItem *FindChildTreeItem(QTreeWidgetItem *pItemP, int nID)
{
	for (int i = pItemP->childCount() - 1; i >= 0; i--)
	{
		if (pItemP->child(i)->data(0, Qt::UserRole) == nID)
		{
			return pItemP->child(i);
		}
	}
	return NULL;
}

CmdTakeBackControl::CmdTakeBackControl(QWidget *parent) :
	LyDialog(tr("TakeBackControl"), parent),
    ui(new Ui::CmdTakeBackControl)
{
    ui->setupUi((QDialog*)centralWidget());
	ui->treeWidget->setHeaderLabels(QStringList() << tr("FlightController") << tr("UAV"));
	resize(360, 700);
	QObject::connect(ui->pushButtonCancel, SIGNAL(clicked()), this, SLOT(reject()));
	QMetaObject::connectSlotsByName(this);

	QDb db;
	LyXyhc::Get().GetDb(db);
	QDbQuery qry = db.exec("SELECT * FROM v_pilotUav");
	QTreeWidgetItem *pItemP = ui->treeWidget->invisibleRootItem();
	while (!qry.eof())
	{
		_PilotUav pu;
		qry >> pu.uav >> pu.pilot >> pu.name;
		QTreeWidgetItem *pItem = FindChildTreeItem(pItemP, pu.pilot);
		if (pItem == nullptr)
		{
			pItem = new QTreeWidgetItem(pItemP);
			pItem->setData(0, Qt::UserRole, pu.pilot);
			pItem->setText(0, pu.name);
			pItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
			pItem->setCheckState(0, Qt::Unchecked);
		}
		pItem = new QTreeWidgetItem(pItem);
		pItem->setText(1, QString::asprintf("%d", pu.uav));
	}
	ui->treeWidget->expandAll();
}

CmdTakeBackControl::~CmdTakeBackControl()
{
    delete ui;
}

void CmdTakeBackControl::on_pushButtonOk_clicked()
{
	bool bMsg = false;
	QString msg = tr("Want to retrieve the flight controller:");
	QString sql = "DELETE FROM pilotUav WHERE pilotID IN(";
	QTreeWidgetItem *pItemP = ui->treeWidget->invisibleRootItem();
	for (int i = 0; i < pItemP->childCount(); i++)
	{
		QTreeWidgetItem *pItem = pItemP->child(i);
		if (pItem->checkState(0) == Qt::Checked)
		{
			bMsg = true;
			msg += pItem->text(0);
			msg += tr("¡¢");
			sql += pItem->data(0, Qt::UserRole).toString();
			sql += ",";
		}
	}
	if (bMsg)
	{
		msg = msg.left(msg.length() - 1);
		msg += tr("control power");
		if (QMessageBox::Yes == LyMessageBox::question(this, tr("Ensure"), msg, QMessageBox::Yes | QMessageBox::No))
		{
			sql = sql.left(sql.length() - 1);
			sql += ")";
			LyXyhc::Get().ExecSql(sql);

			for (int i = 0; i < pItemP->childCount(); i++)
			{
				QTreeWidgetItem *pItem = pItemP->child(i);
				if (pItem->checkState(0) == Qt::Checked)
				{
					SendManualCommand(0, 14, pItem->data(0, Qt::UserRole).toInt());
				}
			}
		}
	}
	accept();
}
