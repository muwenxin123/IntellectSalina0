#include "CDlgSetPlat.h"
#include "lY_TaskPlatMatch.h"
#include <LyMessageBox.h>

CDlgSetPlat::CDlgSetPlat(QWidget *parent)
    : LyDialog(QObject::tr("SetPlat"), parent),
      ui(new Ui::CDlgSetPlat)
{
    ui->setupUi(centralWidget());
    this->setResizable(true);
    this->resize(600, 400);
    this->setMaximumSize(QSize(600, 400));

    QStringList headers;
    headers.clear();
    headers << QObject::tr("number") << QObject::tr("plat model") << QObject::tr("match equipment") << QObject::tr("control seat") << QObject::tr("start time") << QObject::tr("end time");
    ui->tableWidget->setColumnCount(headers.size());
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->tableWidget->setEnabled(true);
    ui->tableWidget->verticalHeader()->hide();

    connect(ui->pBEnsure, &QPushButton::clicked, this, &CDlgSetPlat::on_pbEnsure_clicked);
    connect(ui->pBCancel, &QPushButton::clicked, this, &CDlgSetPlat::on_pbCancel_clicked);
    this->hide();
}

CDlgSetPlat::~CDlgSetPlat()
{
}

void CDlgSetPlat::initComBoBox(QStringList strList)
{
    ui->comboBoxMissionName->clear();
    ui->comboBoxMissionName->addItems(strList);
}

void CDlgSetPlat::setComBoBoxIndex(int Index)
{
    if (ui->comboBoxMissionName->count() > Index)
    {
        ui->comboBoxMissionName->setCurrentIndex(Index);
    }
}

int CDlgSetPlat::getComCruIndex()
{
    return ui->comboBoxMissionName->currentIndex();
}

void CDlgSetPlat::initMissionName(QString strName)
{
    ui->lineEditMissionName->setText(strName);
}

QComboBox *CDlgSetPlat::getMissionComBox()
{
    return ui->comboBoxMissionName;
}

void CDlgSetPlat::setPlatType(missionPlatType _platType)
{
    platType = _platType;
    updateDialog();
    updateItemsColor();
}

missionPlatType CDlgSetPlat::getPlatType()
{
    return platType;
}

void CDlgSetPlat::updateDialog()
{
    ui->tableWidget->clearContents();

    ui->tableWidget->setRowCount(platType.getPlatNum());
    std::vector<std::string> names = platType.getPlatNames();
    std::vector<std::string> names_selected = platType.getPlatNames_Selected();
    std::vector<std::string> ctrlersName = platType.getControlers();
    std::vector<std::string> allCtrlers = platType.getAllControlers();

    if (names_selected.empty())
    {
        for (int i = 0; i < platType.getPlatNum(); i++)
        {
            QTableWidgetItem *item0 = new QTableWidgetItem(QString::number(i + 1));
            QTableWidgetItem *item1 = new QTableWidgetItem(QString::fromStdString(platType.getPlatTypeName()));

            QComboBox *tmpBox = new QComboBox;
            tmpBox->addItem(" ");
            tmpBox->setCurrentText(" ");

            QComboBox *tmpBoxCtrl = new QComboBox;
            tmpBoxCtrl->addItem(" ");
            tmpBoxCtrl->setCurrentText(" ");

            QTableWidgetItem *item4 = new QTableWidgetItem(startTime);
            QTableWidgetItem *item5 = new QTableWidgetItem(endTime);

            ui->tableWidget->setCellWidget(i, 3, tmpBox);

            connect(tmpBox, SIGNAL(currentTextChanged(const QString &)), this, SLOT(checkIsRepetition(const QString &)));
            connect(tmpBoxCtrl, SIGNAL(currentTextChanged(const QString &)), this, SLOT(ctrlChanged(const QString &)));

            ui->tableWidget->setItem(i, 0, item0);
            ui->tableWidget->setItem(i, 1, item1);
            ui->tableWidget->setCellWidget(i, 2, tmpBox);
            ui->tableWidget->setCellWidget(i, 3, tmpBoxCtrl);
            ui->tableWidget->setItem(i, 4, item4);
            ui->tableWidget->setItem(i, 5, item5);
            ui->tableWidget->item(i, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tableWidget->item(i, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tableWidget->item(i, 4)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tableWidget->item(i, 5)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

        }
    }
    QStringList namesList;
    QStringList allCtrlList;
    namesList.clear();
    allCtrlList.clear();
    namesList << " ";
    for (int i = 0; i < names.size(); i++)
    {
        namesList << QString::fromStdString(names[i]);
    }
    for (int i = 0; i < allCtrlers.size(); i++)
    {
        allCtrlList << QString::fromStdString(allCtrlers[i]);
    }

    for (int i = 0; i < names_selected.size(); i++)
    {
        QTableWidgetItem *item0 = new QTableWidgetItem(QString::number(i + 1));
        QTableWidgetItem *item1 = new QTableWidgetItem(QString::fromStdString(platType.getPlatTypeName()));
        QTableWidgetItem *item4 = new QTableWidgetItem(startTime);
        QTableWidgetItem *item5 = new QTableWidgetItem(endTime);

        QComboBox *tmpBox = new QComboBox;
        QComboBox *tmpBoxCtrl = new QComboBox;

        tmpBox->addItems(namesList);
        tmpBoxCtrl->addItems(allCtrlList);
        if (tmpBox->count() > 0)
        {
            tmpBox->setCurrentText(QString::fromStdString(names_selected[i]));
        }
        if (tmpBoxCtrl->count() > 0)
        {
            tmpBoxCtrl->setCurrentText(QString::fromStdString(ctrlersName[i]));
        }

        connect(tmpBox, SIGNAL(currentTextChanged(const QString &)), this, SLOT(checkIsRepetition(const QString &)));
        connect(tmpBoxCtrl, SIGNAL(currentTextChanged(const QString &)), this, SLOT(ctrlChanged(const QString &)));

        ui->tableWidget->setItem(i, 0, item0);
        ui->tableWidget->setItem(i, 1, item1);
        ui->tableWidget->setCellWidget(i, 2, tmpBox);
        ui->tableWidget->setCellWidget(i, 3, tmpBoxCtrl);
        ui->tableWidget->setItem(i, 4, item4);
        ui->tableWidget->setItem(i, 5, item5);
        ui->tableWidget->item(i, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget->item(i, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget->item(i, 4)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget->item(i, 5)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    }

    if (ui->tableWidget->rowCount() <= 0)
    {
        this->resize(QSize(600, 400));

    }
    else
    {
        int Hei = ui->tableWidget->rowHeight(0);
        int wid = ui->tableWidget->columnWidth(0);
        this->resize(QSize(wid * 5, Hei * (ui->tableWidget->rowCount() + 6)));

    }

    this->show();
}

void CDlgSetPlat::setConBoxCurIndexNewName(int curIndex, string newName)
{

    if (curIndex < ui->tableWidget->rowCount())
    {
        ((QComboBox *)ui->tableWidget->cellWidget(curIndex, 2))->setCurrentText(QString::fromStdString(newName));
        auto tmpVec = platType.getPlatNames_Selected();
        tmpVec[curIndex] = newName;
        platType.setPlatNames_Selected(tmpVec);
        isSetRe = false;
    }

}

std::vector<std::string> CDlgSetPlat::getPlatNames()
{
    return platType.getPlatNames_Selected();
}

void CDlgSetPlat::on_pbEnsure_clicked()
{

    this->hide();

}
void CDlgSetPlat::setTime(QString _StartTime, QString _endTime)
{
    startTime = _StartTime;
    endTime = _endTime;
}
void CDlgSetPlat::updateItemsColor()
{
    auto missionWeaponTypeAndNames = LY_TaskPlatMatch::Get().gettaskPlatMatch()->getMissionWeaponTypeAndNames();
    if (missionWeaponTypeAndNames.empty())
    {
        return;
    }
    auto missionItor = missionWeaponTypeAndNames.find(platType.getMissionID());
    if (missionItor == missionWeaponTypeAndNames.end())
    {
        return;
    }
    auto weaponTypeItor = missionItor->second.find(platType.getWeaponType());
    if (weaponTypeItor == missionItor->second.end())
    {
        return;
    }
    auto names = weaponTypeItor->second;
    if (true)
    {
        auto platNames = platType.getPlatNames();
        for (int nameIndex = 0; nameIndex < platNames.size(); nameIndex++)
        {
            QColor *tmpColor = NULL;
            if (std::find(names.begin(), names.end(), platNames[nameIndex]) == names.end())
            {
                tmpColor = new QColor(Qt::red);
            }
            else
            {
                tmpColor = new QColor(Qt::green);
            }
            for (int i = 0; i < ui->tableWidget->rowCount(); i++)
            {
                QComboBox *tmpCom = (QComboBox *)ui->tableWidget->cellWidget(i, 2);
                if (tmpCom != NULL)
                {
                    tmpCom->setItemData(nameIndex + 1, QBrush(*tmpColor), Qt::TextColorRole);
                }
            }
            delete tmpColor;
        }
    }
}
void CDlgSetPlat::on_pbCancel_clicked()
{
    on_pbEnsure_clicked();
}

void CDlgSetPlat::checkIsRepetition(const QString &text)
{
    if (ui->tableWidget->currentRow() >= platType.getPlatNames_Selected().size() || ui->tableWidget->currentRow() < 0)
    {
        return;
    }
    if (text.toStdString() == platType.getPlatNames_Selected()[ui->tableWidget->currentRow()])
    {
        return;
    }
    auto missionWeaponTypeAndNames = LY_TaskPlatMatch::Get().gettaskPlatMatch()->getMissionWeaponTypeAndNames();
    if (missionWeaponTypeAndNames.empty())
    {
        return;
    }
    auto missionItor = missionWeaponTypeAndNames.find(platType.getMissionID());
    if (missionItor == missionWeaponTypeAndNames.end())
    {
        return;
    }
    auto weaponTypeItor = missionItor->second.find(platType.getWeaponType());
    if (weaponTypeItor == missionItor->second.end())
    {
        return;
    }

    auto textWeaponItor = std::find(weaponTypeItor->second.begin(), weaponTypeItor->second.end(), text.toStdString());

    if (textWeaponItor != weaponTypeItor->second.end() || text == " ")
    {
        vector<string> selNames = platType.getPlatNames_Selected();
        if (ui->tableWidget->currentRow() >= selNames.size())
        {
            return;
        }
        string oldName = selNames[ui->tableWidget->currentRow()];
        selNames[ui->tableWidget->currentRow()] = text.toStdString();
        platType.setPlatNames_Selected(selNames);
        LY_TaskPlatMatch::Get().gettaskPlatMatch()->updateMissionTypeOldToNewName(platType.getMissionID(), platType.getWeaponType(),
                                                                                  platType.getPlatTypeName(), ui->tableWidget->currentRow(),
                                                                                  oldName, text.toStdString());

    }
    else
    {

        vector<string> selNames = platType.getPlatNames_Selected();
        auto newNameItor = std::find(selNames.begin(), selNames.end(), text.toStdString());

        if (newNameItor != selNames.end())
        {
            string oldName = selNames[ui->tableWidget->currentRow()];
            int oldIndex = -1;
            for (int i = 0; i < selNames.size(); i++)
            {
                if (selNames[i] == text.toStdString())
                {
                    oldIndex = i;
                    break;
                }
            }
            if (oldIndex != -1)
            {
                selNames[oldIndex] = oldName;
                selNames[ui->tableWidget->currentRow()] = text.toStdString();
                QComboBox *tmpCell = (QComboBox *)(ui->tableWidget->cellWidget(oldIndex, 2));
                tmpCell->setCurrentText(QString::fromStdString(oldName));
                platType.setPlatNames_Selected(selNames);

                LY_TaskPlatMatch::Get().gettaskPlatMatch()->swapMissionTypeOldToNewName(platType.getMissionID(), platType.getPlatTypeName(),
                                                                                        oldIndex, oldName, ui->tableWidget->currentRow(), text.toStdString());
            }
        }
        else
        {
            string oldName = selNames[ui->tableWidget->currentRow()];
            LY_TaskPlatMatch::Get().gettaskPlatMatch()->swapDiffMissionTypeOldToNewName(platType.getMissionID(), platType.getWeaponType(),
                                                                                        platType.getPlatTypeName(), ui->tableWidget->currentRow(), oldName, text.toStdString());

        }
    }
    updateItemsColor();

}

void CDlgSetPlat::ctrlChanged(const QString &text)
{
    if (ui->tableWidget->currentRow() >= 0 && ui->tableWidget->currentRow() < ui->tableWidget->rowCount())
    {
        vector<string> ctrlers = platType.getControlers();
        ctrlers[ui->tableWidget->currentRow()] = text.toStdString();
        platType.setControlers(ctrlers);
        LY_TaskPlatMatch::Get().gettaskPlatMatch()->updateCtrler(platType.getMissionID(), platType.getPlatTypeName(),
                                                                 ui->tableWidget->currentRow(), text.toStdString());
    }
}

void CDlgSetPlat::closeEvent(QCloseEvent *event)
{
    on_pbEnsure_clicked();
}
