#include "SelectMarkDialog.h"
#include "QCString.h"

SelectMarkDialog::SelectMarkDialog(QWidget *parent)
    : LyDialog(tr("SelectMark"), parent),
      ui(new Ui::SelectMarkDialog)
{
    ui->setupUi(centralWidget());
    this->setResizable(true);
    this->resize(400, 600);

    connect(ui->comboBox, &QComboBox::currentTextChanged, this, &SelectMarkDialog::comboBoxChanged);
    connect(ui->pB_cancel, &QPushButton::clicked, this, [this]()
    {
        this->close();

    });
    connect(ui->pB_Ensure, &QPushButton::clicked, this, [this]()
    {
        this->close();
    });
}

SelectMarkDialog::~SelectMarkDialog()
{
}

void SelectMarkDialog::initTableWidget(std::vector<std::pair<int, int>> selectedMarks, std::vector<unsigned long long> selectedMarkIDs,
                                         std::vector<std::pair<int, int>> allMarks, std::vector<unsigned long long> MarkIDs)
{
    SystemSysIDAndName = LyXyhc::Get().getPDlgSystemMessageDistribute()->getSystemSysIDAndName();
    SystemSysNameAndID = LyXyhc::Get().getPDlgSystemMessageDistribute()->getSystemSysNameAndID();

    alltargets = allMarks;
    alltargetIDs = MarkIDs;

    if (allMarks.empty())
    {
        targets.clear();
    }
    else
    {
        targets = selectedMarks;
    }

    if (MarkIDs.empty())
    {
        targetIDs.clear();
    }
    else
    {
        targetIDs = selectedMarkIDs;
    }

    if (ui->comboBox->currentIndex() == 0)
    {
        initTableZP(targets, alltargets);
    }
    else
    {
        initTableID(targetIDs, alltargetIDs);
    }

}

void SelectMarkDialog::initTableZP(std::vector<std::pair<int, int>> selectedMarks, std::vector<std::pair<int, int>> allMarks)
{
    ui->tableWidget->clear();
    disconnect(ui->tableWidget, &QTableWidget::cellClicked, this, &SelectMarkDialog::selectBoxID);

    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->verticalHeader()->setHidden(true);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QStringList head;
    head.clear();
    head << tr("MarkStationNumber") << tr("MarkBatchNumber") << tr("Select");
    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->setRowCount(allMarks.size());

    ui->tableWidget->setHorizontalHeaderLabels(head);

    for (int i = 0; i < allMarks.size(); i++)
    {
        QTableWidgetItem *item1 = new QTableWidgetItem(QString::number(allMarks[i].first));
        item1->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        QTableWidgetItem *item2 = new QTableWidgetItem(QString::number(allMarks[i].second));
        item2->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        QTableWidgetItem *item3 = new QTableWidgetItem;
        item3->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

        if (std::find(selectedMarks.begin(), selectedMarks.end(), allMarks[i]) != selectedMarks.end())
        {
            item3->setCheckState(Qt::Checked);
        }
        else
        {
            item3->setCheckState(Qt::Unchecked);
        }

        ui->tableWidget->setItem(i, 0, item1);
        ui->tableWidget->setItem(i, 1, item2);
        ui->tableWidget->setItem(i, 2, item3);
    }
    connect(ui->tableWidget, &QTableWidget::cellClicked, this, &SelectMarkDialog::selectBox, Qt::UniqueConnection);

}

void SelectMarkDialog::initTableID(std::vector<unsigned long long> selectedMarkIDs, std::vector<unsigned long long> MarkIDs)
{
    ui->tableWidget->clear();
    disconnect(ui->tableWidget, &QTableWidget::cellClicked, this, &SelectMarkDialog::selectBox);

    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->verticalHeader()->setHidden(true);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QStringList head2;
    head2.clear();
    head2 << tr("MarkID") << tr("Select");
    ui->tableWidget->setColumnCount(2);
    ui->tableWidget->setRowCount(MarkIDs.size());

    ui->tableWidget->setHorizontalHeaderLabels(head2);

    for (int i = 0; i < MarkIDs.size(); i++)
    {
        std::string targetName = QString::number(MarkIDs[i]).toStdString();
        auto systemIDAndNameItor = SystemSysIDAndName.find(targetName);
        if (systemIDAndNameItor != SystemSysIDAndName.end())
        {
            targetName = systemIDAndNameItor->second;
        }
        QTableWidgetItem *item1 = new QTableWidgetItem(QString::fromStdString(targetName));
        item1->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        QTableWidgetItem *item3 = new QTableWidgetItem;
        item3->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        if (std::find(selectedMarkIDs.begin(), selectedMarkIDs.end(), MarkIDs[i]) != selectedMarkIDs.end())
        {
            item3->setCheckState(Qt::Checked);
        }
        else
        {
            item3->setCheckState(Qt::Unchecked);
        }

        ui->tableWidget->setItem(i, 0, item1);
        ui->tableWidget->setItem(i, 1, item3);
    }
    connect(ui->tableWidget, &QTableWidget::cellClicked, this, &SelectMarkDialog::selectBoxID, Qt::UniqueConnection);
}

std::vector<std::pair<int, int>> SelectMarkDialog::getMarks()
{
    return targets;
}

std::vector<unsigned long long> SelectMarkDialog::getMarkIDs()
{
    return targetIDs;
}

void SelectMarkDialog::addMark(std::pair<int, int> target)
{
    auto targetItor = std::find(targets.begin(), targets.end(), target);
    if (targetItor == targets.end())
    {
        targets.push_back(target);
    }
}

void SelectMarkDialog::removeMark(std::pair<int, int> target)
{
    auto targetItor = std::find(targets.begin(), targets.end(), target);
    if (targetItor != targets.end())
    {
        targetItor = targets.erase(targetItor);
    }
}

void SelectMarkDialog::addMarkID(unsigned long long targetID)
{
    auto targetItor = std::find(targetIDs.begin(), targetIDs.end(), targetID);
    if (targetItor == targetIDs.end())
    {
        targetIDs.push_back(targetID);
    }
}

void SelectMarkDialog::removeMarkID(unsigned long long targetID)
{
    auto targetItor = std::find(targetIDs.begin(), targetIDs.end(), targetID);
    if (targetItor != targetIDs.end())
    {
        targetItor = targetIDs.erase(targetItor);
    }
}

QPushButton *SelectMarkDialog::getEnsure()
{
    return ui->pB_Ensure;
}

QPushButton *SelectMarkDialog::getCancel()
{
    return ui->pB_cancel;
}

void SelectMarkDialog::comboBoxChanged()
{
    if (ui->comboBox->currentIndex() == 0)
    {
        initTableZP(targets, alltargets);
    }
    else
    {
        initTableID(targetIDs, alltargetIDs);
    }
}

void SelectMarkDialog::selectBox(int r, int c)
{
    if (r < 0 || r >= ui->tableWidget->rowCount())
    {
        return;
    }

    std::pair<int, int> tmpZP(ui->tableWidget->item(r, 0)->text().toUInt(), ui->tableWidget->item(r, 1)->text().toUInt());
    auto targetItor = std::find(targets.begin(), targets.end(), tmpZP);
    if (targetItor != targets.end())
    {
        ui->tableWidget->item(r, 2)->setCheckState(Qt::Unchecked);
        removeMark(tmpZP);
    }
    else
    {
        ui->tableWidget->item(r, 2)->setCheckState(Qt::Checked);
        addMark(tmpZP);
    }

}

void SelectMarkDialog::selectBoxID(int r, int c)
{
    if (r < 0 || r >= ui->tableWidget->rowCount())
    {
        return;
    }

    std::string tmpName = std::to_string(ui->tableWidget->item(r, 0)->text().toULongLong());
    auto systemSysNameAndIDItor = SystemSysNameAndID.find(ui->tableWidget->item(r, 0)->text().toStdString());
    if (systemSysNameAndIDItor != SystemSysNameAndID.end())
    {
        tmpName = systemSysNameAndIDItor->second;
    }
    auto targetItor = std::find(targetIDs.begin(), targetIDs.end(), std::stoull(tmpName));
    if (targetItor != targetIDs.end())
    {
        ui->tableWidget->item(r, 1)->setCheckState(Qt::Unchecked);
        removeMarkID(std::stoull(tmpName));
    }
    else
    {
        ui->tableWidget->item(r, 1)->setCheckState(Qt::Checked);
        addMarkID(std::stoull(tmpName));
    }
}

