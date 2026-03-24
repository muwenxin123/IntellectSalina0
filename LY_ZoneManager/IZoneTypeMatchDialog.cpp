#include "IZoneTypeMatchDialog.h"
#include "ui_IZoneTypeMatchDialog.h"
#include "QCString.h"
#include "LY_ZoneManager.h"
IZoneTypeMatchDialog::IZoneTypeMatchDialog(QWidget *parent) :
	LyDialog(tr2("区域规则描述"), parent),
    ui(new Ui::IZoneTypeMatchDialog)
{
	ui->setupUi(centralWidget());
	this->setResizable(true);

	QStringList list;
	list << tr2("管理区") << tr2("医院") << tr2("学校") << tr2("大厦");
	m_AllChks[tr2("禁飞区")] = list;
	m_AllChks[tr2("限飞区")] = list;
	list.clear();
	list << tr2("气流区") << tr2("高压线");
	m_AllChks[tr2("危险区")] = list;
	list.clear();
	list << tr2("充电点") << tr2("起降区");
	m_AllChks[tr2("起降区")] = list;
	connect(ui->btn_ok, &QPushButton::clicked, this, &IZoneTypeMatchDialog::on_PbtnOK_Clicked);
	connect(ui->btn_cencel, &QPushButton::clicked, this, &IZoneTypeMatchDialog::on_PbtnCancel_Clicked);
}

IZoneTypeMatchDialog::~IZoneTypeMatchDialog()
{
    delete ui;
}

void IZoneTypeMatchDialog::Init(int row,QString rule,QStringList chkList)
{
	m_nrow = row;
	QLayout *lay = ui->groupBox->layout();
	if (lay != nullptr)
	{
		QLayoutItem* item;
		while ((item = lay->takeAt(0)) != nullptr)
		{
			if (item->widget() != nullptr)
			{
				item->widget()->setParent(nullptr);
			}
			delete item;
		}
		delete lay;
		ui->groupBox->setLayout(nullptr);
	}
	QVBoxLayout* layout = new QVBoxLayout;
	auto itr = m_AllChks.find(rule);
	if (itr!=m_AllChks.end())
	{
		for (auto check:itr->second)
		{
			bool isselect = false;
			for (auto check1:chkList)
			{
				if (check==check1)
				{
					QCheckBox *box = new QCheckBox();
					box->setText(check);
					box->setCheckState(Qt::Checked);
					layout->addWidget(box);
					isselect = true;
					break;
				}
			}
			if (!isselect)
			{
				QCheckBox *box = new QCheckBox();
				box->setText(check);
				box->setCheckState(Qt::Unchecked);

				layout->addWidget(box);
			}
		}
	}
 		ui->groupBox->setLayout(layout);

}

void IZoneTypeMatchDialog::on_PbtnOK_Clicked()
{
	m_vSelectChksVec.clear();
	QList<QCheckBox*>checkboxs = ui->groupBox->findChildren<QCheckBox*>();
	for (QCheckBox* checkbox:checkboxs)
	{
		if (checkbox->isChecked())
		{
			m_vSelectChksVec.push_back(checkbox->text());
		}
	}
	LY_ZoneManager::Get().GetPDlgZoneRuleManage()->UpdateTablewidget(m_nrow, m_vSelectChksVec);
	this->close();
}

void IZoneTypeMatchDialog::on_PbtnCancel_Clicked()
{
    close();
}
