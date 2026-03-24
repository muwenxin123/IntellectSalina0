#include "NumberLineEditDelegate.h"
#include "VehicleDeflectionDistYawPitchDialog.h"
#include "ui_VehicleDeflectionDistYawPitchDialog.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSortFilterProxyModel>
#include <algorithm>
#include <functional>

#include "LyMessageBox.h"

#include "LY_AerialShotSettingBundle54Service.h"

static const QString &DeflectionDistYawPitchFileName(QStringLiteral("Data/DeflectionDistYawPitch54.json"));

VehicleDeflectionDistYawPitchDialog::VehicleDeflectionDistYawPitchDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VehicleDeflectionDistYawPitchDialog),
    m_ParentWidget(parent),
    m_TableMenu(nullptr),
    m_currentComponentID(),
    m_currentComponentName()
{
    ui->setupUi(this);

    ui->label_Vehicle->setText(QString());

    ui->tableWidget_VehicleDeflectionDistYawPitch->clear();
    ui->tableWidget_VehicleDeflectionDistYawPitch->setColumnCount(4);

    QTableWidgetItem *qtablewidgetitem = new QTableWidgetItem();
    qtablewidgetitem->setText(QApplication::translate("VehicleDeflectionDistYawPitchDialog", "Magnification", nullptr));
    ui->tableWidget_VehicleDeflectionDistYawPitch->setHorizontalHeaderItem(0, qtablewidgetitem);
    QTableWidgetItem *qtablewidgetitem1 = new QTableWidgetItem();
    qtablewidgetitem1->setText(QApplication::translate("VehicleDeflectionDistYawPitchDialog", "Dist", nullptr));
    ui->tableWidget_VehicleDeflectionDistYawPitch->setHorizontalHeaderItem(1, qtablewidgetitem1);
    QTableWidgetItem *qtablewidgetitem2 = new QTableWidgetItem();
    qtablewidgetitem2->setText(QApplication::translate("VehicleDeflectionDistYawPitchDialog", "Yaw", nullptr));
    ui->tableWidget_VehicleDeflectionDistYawPitch->setHorizontalHeaderItem(2, qtablewidgetitem2);
    QTableWidgetItem *qtablewidgetitem3 = new QTableWidgetItem();
    qtablewidgetitem3->setText(QApplication::translate("VehicleDeflectionDistYawPitchDialog", "Pitch", nullptr));
    ui->tableWidget_VehicleDeflectionDistYawPitch->setHorizontalHeaderItem(3, qtablewidgetitem3);

    ui->tableWidget_VehicleDeflectionDistYawPitch->setItemDelegate(new NumberLineEditDelegate());
    ui->tableWidget_VehicleDeflectionDistYawPitch->setEditTriggers(QTableWidget::EditTrigger::NoEditTriggers);
    ui->tableWidget_VehicleDeflectionDistYawPitch->setSelectionBehavior(QTableWidget::SelectRows);
    ui->tableWidget_VehicleDeflectionDistYawPitch->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget_VehicleDeflectionDistYawPitch->setAlternatingRowColors(true);
    ui->tableWidget_VehicleDeflectionDistYawPitch->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tableWidget_VehicleDeflectionDistYawPitch->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Interactive);

    connect(ui->tableWidget_VehicleDeflectionDistYawPitch, &QTableWidget::customContextMenuRequested, this, &VehicleDeflectionDistYawPitchDialog::handleCustomContextMenuRequested);

    m_TableMenu = new QMenu();
    if (m_TableMenu)
    {
        connect(m_TableMenu->addAction(tr("Start Edit", "VehicleDeflectionDistYawPitchDialog")), &QAction::triggered, this,
                [this]()
        {
            ui->tableWidget_VehicleDeflectionDistYawPitch->setEditTriggers(QTableWidget::EditTrigger::DoubleClicked);
        });

        connect(m_TableMenu->addAction(tr("Stop Edit", "VehicleDeflectionDistYawPitchDialog")), &QAction::triggered, this,
                [this]()
        {
            ui->tableWidget_VehicleDeflectionDistYawPitch->setEditTriggers(QTableWidget::EditTrigger::NoEditTriggers);
        });

        connect(m_TableMenu->addAction(tr("Add", "VehicleDeflectionDistYawPitchDialog")), &QAction::triggered, this,
                [this]()
        {
            const int rowCountTable = ui->tableWidget_VehicleDeflectionDistYawPitch->rowCount();
            ui->tableWidget_VehicleDeflectionDistYawPitch->insertRow(rowCountTable);
        });

        connect(m_TableMenu->addAction(tr("Delete", "VehicleDeflectionDistYawPitchDialog")), &QAction::triggered, this,
                [this]()
        {
            const int currentRowTable = ui->tableWidget_VehicleDeflectionDistYawPitch->currentRow();
            const int rowCountTable   = ui->tableWidget_VehicleDeflectionDistYawPitch->rowCount();
            if (currentRowTable >= 0 && currentRowTable < rowCountTable)
            {
                ui->tableWidget_VehicleDeflectionDistYawPitch->removeRow(currentRowTable);
            }
        });
    }

}

VehicleDeflectionDistYawPitchDialog::~VehicleDeflectionDistYawPitchDialog()
{
    delete ui;
}

void VehicleDeflectionDistYawPitchDialog::setVehicleIDAndName(const std::string &strVehicleID, const std::string &strVehicleName)
{
    m_currentComponentID   = strVehicleID;
    m_currentComponentName = strVehicleName;
    ui->label_Vehicle->setText(QString::fromStdString(m_currentComponentName));
}

void VehicleDeflectionDistYawPitchDialog::setVehicleDeflectionDistYawPitchMap(const std::vector<std::pair<unsigned int, std::list<std::pair<unsigned int, std::pair<double, double> > > > > &vehicleDeflectionDistYawPitchMap)
{
    ui->tableWidget_VehicleDeflectionDistYawPitch->clearContents();
    for (const auto &itemvehicleDeflectionDistYawPitch : vehicleDeflectionDistYawPitchMap)
    {
        for (const auto &itemvehicleDeflectionYawPitch : itemvehicleDeflectionDistYawPitch.second)
        {
            ui->tableWidget_VehicleDeflectionDistYawPitch->insertRow(0);
            QTableWidgetItem *item0 = new QTableWidgetItem(QString::number(itemvehicleDeflectionDistYawPitch.first));
            QTableWidgetItem *item1 = new QTableWidgetItem(QString::number(itemvehicleDeflectionYawPitch.first)) ;
            QTableWidgetItem *item2 = new QTableWidgetItem(QString::number(itemvehicleDeflectionYawPitch.second.first));
            QTableWidgetItem *item3 = new QTableWidgetItem(QString::number(itemvehicleDeflectionYawPitch.second.second));

            if (item0 == nullptr || item1 == nullptr || item2 == nullptr || item3 == nullptr)
            {
                continue;
            }

            ui->tableWidget_VehicleDeflectionDistYawPitch->setItem(0, 0, item0);
            ui->tableWidget_VehicleDeflectionDistYawPitch->setItem(0, 1, item1);
            ui->tableWidget_VehicleDeflectionDistYawPitch->setItem(0, 2, item2);
            ui->tableWidget_VehicleDeflectionDistYawPitch->setItem(0, 3, item3);
        }
    }
}

void VehicleDeflectionDistYawPitchDialog::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type())
    {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void VehicleDeflectionDistYawPitchDialog::contextMenuEvent(QContextMenuEvent *event)
{
    QWidget::contextMenuEvent(event);
    m_TableMenu->popup(event->globalPos());
}

void VehicleDeflectionDistYawPitchDialog::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
}

void VehicleDeflectionDistYawPitchDialog::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)
}

void VehicleDeflectionDistYawPitchDialog::handleCustomContextMenuRequested(const QPoint &pos)
{
    m_TableMenu->popup(ui->tableWidget_VehicleDeflectionDistYawPitch->viewport()->mapToGlobal(pos));
}

void VehicleDeflectionDistYawPitchDialog::on_pushButton_Save_clicked()
{
    std::vector< std::pair<unsigned int, std::list< std::pair<unsigned int, std::pair<double, double> > > > > vehicleDeflectionDistYawPitchMap;
    vehicleDeflectionDistYawPitchMap.clear();

    const int rowCountTable = ui->tableWidget_VehicleDeflectionDistYawPitch->rowCount();
    for (int i = 0; i < rowCountTable; i++)
    {
        const QTableWidgetItem *const item0 = ui->tableWidget_VehicleDeflectionDistYawPitch->item(i, 0);
        const QTableWidgetItem *const item1 = ui->tableWidget_VehicleDeflectionDistYawPitch->item(i, 1);
        const QTableWidgetItem *const item2 = ui->tableWidget_VehicleDeflectionDistYawPitch->item(i, 2);
        const QTableWidgetItem *const item3 = ui->tableWidget_VehicleDeflectionDistYawPitch->item(i, 3);
        if (item0 == nullptr || item1 == nullptr || item2 == nullptr || item3 == nullptr)
        {
            continue;
        }

        const QString &item0_Magnification = item0->text();
        const QString &item1_Dist          = item1->text();
        const QString &item2_Yaw           = item2->text();
        const QString &item3_Pitch         = item3->text();

        if (item0_Magnification.isEmpty() || item1_Dist.isEmpty() || item2_Yaw.isEmpty() || item3_Pitch.isEmpty())
        {
            continue;
        }

        bool ok = false;
        const unsigned int &item0_Magnification_value = item0_Magnification.toUInt(&ok);
        if (!ok)
        {
            continue;
        }

        const unsigned int &item1_Dist_value = item1_Dist.toUInt(&ok);
        if (!ok)
        {
            continue;
        }

        const double &item2_Yaw_value = item2_Yaw.toDouble(&ok);
        if (!ok)
        {
            continue;
        }

        const double &item3_Pitch_value = item3_Pitch.toDouble(&ok);
        if (!ok)
        {
            continue;
        }

        const auto iter = std::find_if(vehicleDeflectionDistYawPitchMap.begin(), vehicleDeflectionDistYawPitchMap.end(),
                                       [item0_Magnification_value](const std::pair<unsigned int, std::list< std::pair<unsigned int, std::pair<double, double> > > > &temp)
        {
            if (item0_Magnification_value == temp.first)
            {
                return true;
            }
            return false;
        }
                                      );

        if (iter != vehicleDeflectionDistYawPitchMap.end())
        {
            iter->second.push_back(std::pair<unsigned int, std::pair<double, double> >(item1_Dist_value, std::pair<double, double>(item2_Yaw_value, item3_Pitch_value)));
        }
        else
        {
            vehicleDeflectionDistYawPitchMap.push_back(std::pair<unsigned int, std::list< std::pair<unsigned int, std::pair<double, double> > > >(item0_Magnification_value, {std::pair<unsigned int, std::pair<double, double> >(item1_Dist_value, std::pair<double, double>(item2_Yaw_value, item3_Pitch_value))}));
        }
    }

    std::sort(vehicleDeflectionDistYawPitchMap.begin(), vehicleDeflectionDistYawPitchMap.end(), std::greater<>());
    for (auto &vehicleDeflectionDistYawPitch : vehicleDeflectionDistYawPitchMap)
    {
        vehicleDeflectionDistYawPitch.second.sort(std::greater<>());
    }

    LY_AerialShotSettingBundle54Service::Get().getPIAerialShotSettingBundle54Dialog()->setVehicleDeflectionDistYawPitch54Map(m_currentComponentID, vehicleDeflectionDistYawPitchMap);

    writeVehicleDeflectionDistYawPitchData();
}

void VehicleDeflectionDistYawPitchDialog::on_pushButton_Cancel_clicked()
{
    if (m_ParentWidget)
    {
        m_ParentWidget->close();
    }
}

void VehicleDeflectionDistYawPitchDialog::readVehicleDeflectionDistYawPitchData()
{
    QFile loadFile(DeflectionDistYawPitchFileName);

    if (!loadFile.open(QIODevice::ReadOnly))
    {
        qWarning("Couldn't open save file.");
        return ;
    }

    QByteArray saveData = loadFile.readAll();

    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));

    const QJsonObject &json = loadDoc.object();
    if (json.contains("DeflectionDistYawPitch") && json["DeflectionDistYawPitch"].toString() == "DeflectionDistYawPitch"
            && json.contains("Vehicle") && json["Vehicle"].isArray())
    {
        const QJsonArray &jsonArrayVehicle = json["Vehicle"].toArray();
        const int sizeArrayVehicle = jsonArrayVehicle.size();
        for (int indexArrayVehicle = 0; indexArrayVehicle < sizeArrayVehicle; ++indexArrayVehicle)
        {
            if (jsonArrayVehicle[indexArrayVehicle].isObject())
            {
                const QJsonObject &objectVehicle = jsonArrayVehicle[indexArrayVehicle].toObject();
                if (objectVehicle.contains("ID") && objectVehicle["ID"].isString()
                        && objectVehicle.contains("MagnificationDistYawPitch") && objectVehicle["MagnificationDistYawPitch"].isArray())
                {

                    std::vector< std::pair<unsigned int, std::list< std::pair<unsigned int, std::pair<double, double> > > > > vehicleDeflectionDistYawPitchMap;
                    vehicleDeflectionDistYawPitchMap.clear();

                    const QString &strVehicleID = objectVehicle["ID"].toString();
                    const QJsonArray &jsonArrayMagnificationDistYawPitch = objectVehicle["MagnificationDistYawPitch"].toArray();
                    const int sizeArrayMagnificationDistYawPitch = jsonArrayMagnificationDistYawPitch.size();
                    for (int indexMagnificationDistYawPitch = 0; indexMagnificationDistYawPitch < sizeArrayMagnificationDistYawPitch; ++indexMagnificationDistYawPitch)
                    {
                        if (jsonArrayMagnificationDistYawPitch[indexMagnificationDistYawPitch].isObject())
                        {
                            const QJsonObject &objectMagnificationDistYawPitch = jsonArrayMagnificationDistYawPitch[indexMagnificationDistYawPitch].toObject();
                            if (objectMagnificationDistYawPitch.contains("Magnification") && objectMagnificationDistYawPitch["Magnification"].isDouble()
                                    && objectMagnificationDistYawPitch.contains("Dist") && objectMagnificationDistYawPitch["Dist"].isDouble()
                                    && objectMagnificationDistYawPitch.contains("Yaw") && objectMagnificationDistYawPitch["Yaw"].isDouble()
                                    && objectMagnificationDistYawPitch.contains("Pitch") && objectMagnificationDistYawPitch["Pitch"].isDouble()
                               )
                            {
                                const int value_Magnification    = objectMagnificationDistYawPitch["Magnification"].toInt();
                                const double value_Dist          = objectMagnificationDistYawPitch["Dist"].toDouble();
                                const double value_Yaw           = objectMagnificationDistYawPitch["Yaw"].toDouble();
                                const double value_Pitch         = objectMagnificationDistYawPitch["Pitch"].toDouble();

                                const auto iter = std::find_if(vehicleDeflectionDistYawPitchMap.begin(), vehicleDeflectionDistYawPitchMap.end(),
                                                               [value_Magnification](const std::pair<unsigned int, std::list< std::pair<unsigned int, std::pair<double, double> > > > &temp)
                                {
                                    if (value_Magnification == temp.first)
                                    {
                                        return true;
                                    }
                                    return false;
                                }
                                                              );

                                if (iter != vehicleDeflectionDistYawPitchMap.end())
                                {
                                    iter->second.push_back(std::pair<unsigned int, std::pair<double, double> >(value_Dist, std::pair<double, double>(value_Yaw, value_Pitch)));
                                }
                                else
                                {
                                    vehicleDeflectionDistYawPitchMap.push_back(std::pair<unsigned int, std::list< std::pair<unsigned int, std::pair<double, double> > > >(value_Magnification, {std::pair<unsigned int, std::pair<double, double> >(value_Dist, std::pair<double, double>(value_Yaw, value_Pitch))}));
                                }
                            }
                        }
                    }

                    std::sort(vehicleDeflectionDistYawPitchMap.begin(), vehicleDeflectionDistYawPitchMap.end(), std::greater<>());
                    for (auto &vehicleDeflectionDistYawPitch : vehicleDeflectionDistYawPitchMap)
                    {
                        vehicleDeflectionDistYawPitch.second.sort(std::greater<>());
                    }

                    LY_AerialShotSettingBundle54Service::Get().getPIAerialShotSettingBundle54Dialog()->setVehicleDeflectionDistYawPitch54Map(strVehicleID.toStdString(), vehicleDeflectionDistYawPitchMap);
                }
            }
        }
    }
}

void VehicleDeflectionDistYawPitchDialog::writeVehicleDeflectionDistYawPitchData()
{
    QJsonObject json;
    {
        json["DeflectionDistYawPitch"] = "DeflectionDistYawPitch";

        QJsonArray arrayVehicleDeflectionDistYawPitch;
        const auto &vehicleDeflectionDistYawPitchMap = LY_AerialShotSettingBundle54Service::Get().getPIAerialShotSettingBundle54Dialog()->getVehicleDeflectionDistYawPitch54Map();
        for (const auto &vehicleDeflectionDistYawPitch : vehicleDeflectionDistYawPitchMap)
        {

            QJsonArray arrayDeflectionDistYawPitch;
            for (const auto &deflectionDistYawPitch : vehicleDeflectionDistYawPitch.second)
            {
                for (const auto &distYawPitch : deflectionDistYawPitch.second)
                {
                    QJsonObject objectDeflectionDistYawPitch;
                    objectDeflectionDistYawPitch["Magnification"] = int(deflectionDistYawPitch.first);
                    objectDeflectionDistYawPitch["Dist"]          = int(distYawPitch.first);
                    objectDeflectionDistYawPitch["Yaw"]           = (distYawPitch.second.first);
                    objectDeflectionDistYawPitch["Pitch"]         = (distYawPitch.second.second);
                    arrayDeflectionDistYawPitch.append(objectDeflectionDistYawPitch);
                }
            }

            QJsonObject objectVehicle;
            objectVehicle["ID"] = QString::fromStdString(vehicleDeflectionDistYawPitch.first);
            objectVehicle["MagnificationDistYawPitch"] = arrayDeflectionDistYawPitch;

            arrayVehicleDeflectionDistYawPitch.append(objectVehicle);
        }

        json["Vehicle"] = arrayVehicleDeflectionDistYawPitch;
    }

    QFile saveFile(DeflectionDistYawPitchFileName);

    if (!saveFile.open(QIODevice::WriteOnly))
    {
        qWarning("Couldn't open save file.");
        LyMessageBox::warning(nullptr, QApplication::translate("VehicleDeflectionDistYawPitchDialog", "Couldn't save file", nullptr), QApplication::translate("VehicleDeflectionDistYawPitchDialog", "Couldn't save file.", nullptr), QMessageBox::Ok);
        return ;
    }

    QJsonDocument saveDoc(json);
    saveFile.write(saveDoc.toJson());
    LyMessageBox::information(nullptr, QApplication::translate("VehicleDeflectionDistYawPitchDialog", "Save file", nullptr), QApplication::translate("VehicleDeflectionDistYawPitchDialog", "Save file success.", nullptr), QMessageBox::Ok);
}

