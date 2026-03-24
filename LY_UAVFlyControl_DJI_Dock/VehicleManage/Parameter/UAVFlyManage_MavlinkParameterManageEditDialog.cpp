#include "UAVFlyManage_MavlinkParameterManageEditDialog.h"
#include "ui_UAVFlyManage_MavlinkParameterManageEditDialog.h"

#include "lyUI/LyMessageBox.h"

#include "QGCApplication.h"
#include "Vehicle.h"
#include "ParameterManager.h"

UAVFlyManage_MavlinkParameterManageEditDialog::UAVFlyManage_MavlinkParameterManageEditDialog(QWidget *parent, Vehicle *vehicle, int vehicleId)
    : QWidget(parent)
    , ui(new Ui::UAVFlyManage_MavlinkParameterManageEditDialog)
    , m_Show(false)
    , m_vehicle(vehicle)
    , m_vehicleId(vehicleId)
    , m_refreshParameter(false)
    , m_Selected(false)
{
    ui->setupUi(this);

    if (m_vehicle) {
        ParameterManager* parameterManager = m_vehicle->parameterManager();
        if(parameterManager){
            connect(parameterManager, &ParameterManager::parametersReadyChanged, this, &UAVFlyManage_MavlinkParameterManageEditDialog::_parametersReady);
            connect(parameterManager, &ParameterManager::loadProgressChanged,    this, &UAVFlyManage_MavlinkParameterManageEditDialog::_loadProgressChanged);
        }
    }

    if(ui && ui->progressBar_UAVFlyManage_MavlinkParameterManageEdit){
        ui->progressBar_UAVFlyManage_MavlinkParameterManageEdit->hide();
        ui->progressBar_UAVFlyManage_MavlinkParameterManageEdit->setValue(0);
    }

    if(ui && ui->treeWidget_UAVFlyManage_MavlinkParameterManageEdit){
        ui->treeWidget_UAVFlyManage_MavlinkParameterManageEdit->hide();
        ui->treeWidget_UAVFlyManage_MavlinkParameterManageEdit->setVisible(false);
    }

    _parametersReady(false);
}

UAVFlyManage_MavlinkParameterManageEditDialog::~UAVFlyManage_MavlinkParameterManageEditDialog()
{
    delete ui;
}

void UAVFlyManage_MavlinkParameterManageEditDialog::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void UAVFlyManage_MavlinkParameterManageEditDialog::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)
    m_Show = true;

}

void UAVFlyManage_MavlinkParameterManageEditDialog::hideEvent(QHideEvent *event)
{
    Q_UNUSED(event)

    m_Show     = false;
    m_Selected = false;
}

Vehicle *UAVFlyManage_MavlinkParameterManageEditDialog::getVehicle() const
{
    return m_vehicle;
}

QString UAVFlyManage_MavlinkParameterManageEditDialog::getVehicleInfo() const
{
    if(getVehicle()){
        return (" " + getVehicle()->firmwareTypeString() + " : " + getVehicle()->vehicleTypeString());
    }
    return QString("No Vehicle!");
}

bool UAVFlyManage_MavlinkParameterManageEditDialog::getSelected() const
{
    return m_Selected;
}

void UAVFlyManage_MavlinkParameterManageEditDialog::setSelected(bool Selected)
{
    m_Selected = Selected;
}

void UAVFlyManage_MavlinkParameterManageEditDialog::refreshAllParameters()
{
    on_pushButton_UAVFlyManage_MavlinkParameterManageEdit_RefreshAllParameters_clicked();
}

void UAVFlyManage_MavlinkParameterManageEditDialog::MissionManagerTableWidgetDisconnect()
{
    if(!ui){
        return ;
    }

    if(!ui->tableWidget_UAVFlyManage_MavlinkParameterManageEdit){
        return ;
    }

    auto *tableWidget = ui->tableWidget_UAVFlyManage_MavlinkParameterManageEdit;
    if(!tableWidget){
        return ;
    }

    disconnect(ui->tableWidget_UAVFlyManage_MavlinkParameterManageEdit, &QTableWidget::cellChanged, this, &UAVFlyManage_MavlinkParameterManageEditDialog::on_tableWidget_UAVFlyManage_MavlinkParameterManageEdit_cellChanged);
}

void UAVFlyManage_MavlinkParameterManageEditDialog::MissionManagerTableWidgetConnect()
{
    if(!ui){
        return ;
    }

    if(!ui->tableWidget_UAVFlyManage_MavlinkParameterManageEdit){
        return ;
    }

    auto *tableWidget = ui->tableWidget_UAVFlyManage_MavlinkParameterManageEdit;
    if(!tableWidget){
        return ;
    }
    connect(tableWidget, &QTableWidget::cellChanged, this, &UAVFlyManage_MavlinkParameterManageEditDialog::on_tableWidget_UAVFlyManage_MavlinkParameterManageEdit_cellChanged);
}

void UAVFlyManage_MavlinkParameterManageEditDialog::UpdateParametersTableWidget()
{
    if(!ui){
        return ;
    }

    if(!ui->tableWidget_UAVFlyManage_MavlinkParameterManageEdit){
        return ;
    }

    if(!ui->treeWidget_UAVFlyManage_MavlinkParameterManageEdit){
        return ;
    }

    auto *tableWidget = ui->tableWidget_UAVFlyManage_MavlinkParameterManageEdit;
    if(!tableWidget){
        return ;
    }

    auto *treeWidget = ui->treeWidget_UAVFlyManage_MavlinkParameterManageEdit;
    if(!treeWidget){
        return ;
    }

    treeWidget->clear();

    tableWidget->clearContents();
    tableWidget->clear();
    tableWidget->setRowCount(0);
    {
        const QStringList titleList =
        {
            "Component",
            "Category",
            "Group",
            "Name",
            "Value",
            "Units",
            "Min",
            "Max",
            "Short Description",
            "Description",
        };

        tableWidget->setColumnCount(titleList.size());

        tableWidget->setHorizontalHeaderLabels(titleList);
    }

    MissionManagerTableWidgetDisconnect();

    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    if (!m_vehicle) {
        return ;
    }

    ParameterManager* parameterManager = m_vehicle->parameterManager();
    if(!parameterManager){
        return ;
    }

    tableWidget->setVisible(false);

    QMap<QString, QMap<QString, QMap<QString, QString> > > componentId_Category_Group;
    componentId_Category_Group.clear();
    int row = 0;
    for(const auto& componentId : parameterManager->componentIds()){
        for(const auto& parameterName : parameterManager->parameterNames(componentId)){
            const auto* paraFact = parameterManager->getParameter(componentId, parameterName);
            if(paraFact){
                tableWidget->insertRow(tableWidget->rowCount());
                int column = 0;

                const auto& category         = paraFact->category();
                const auto& group            = paraFact->group();
                const auto& name             = paraFact->name();
                const auto& rawValueString   = paraFact->rawValueString();
                const auto& rawUnits         = paraFact->rawUnits();
                const auto& cookedMinString  = paraFact->cookedMinString();
                const auto& cookedMaxString  = paraFact->cookedMaxString();
                const auto& shortDescription = paraFact->shortDescription();
                const auto& longDescription  = paraFact->longDescription();

                componentId_Category_Group[QString("%1").arg(componentId)][category][group] = group;

                QTableWidgetItem* seqItem = new QTableWidgetItem(QString::number(row + 1));
                if(seqItem){
                    seqItem->setData(Qt::UserRole, componentId);
                    tableWidget->setVerticalHeaderItem(row, seqItem);
                    seqItem->setToolTip(longDescription);
                }

                tableWidget->setItem(row, column, new QTableWidgetItem(QString::number(componentId)));
                column++;

                tableWidget->setItem(row, column, new QTableWidgetItem(category));
                column++;

                tableWidget->setItem(row, column, new QTableWidgetItem(group));
                column++;

                tableWidget->setItem(row, column, new QTableWidgetItem(name));
                column++;

                tableWidget->setItem(row, column, new QTableWidgetItem(rawValueString));
                column++;

                tableWidget->setItem(row, column, new QTableWidgetItem(rawUnits));
                column++;

                tableWidget->setItem(row, column, new QTableWidgetItem(cookedMinString));
                column++;

                tableWidget->setItem(row, column, new QTableWidgetItem(cookedMaxString));
                column++;

                tableWidget->setItem(row, column, new QTableWidgetItem(shortDescription));
                column++;

                tableWidget->setItem(row, column, new QTableWidgetItem(longDescription));
                column++;

                row++;
            }
        }
    }

    tableWidget->setColumnWidth(0,  110);
    tableWidget->setColumnWidth(1,  80);

    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
    tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive);
    for(int column = 0; column < tableWidget->columnCount(); column++ ){
        tableWidget->horizontalHeader()->setSectionResizeMode(column, QHeaderView::Interactive);
    }

    tableWidget->setVisible(true);

    MissionManagerTableWidgetConnect();

    if(ui->progressBar_UAVFlyManage_MavlinkParameterManageEdit){
        ui->progressBar_UAVFlyManage_MavlinkParameterManageEdit->hide();
    }
}

void UAVFlyManage_MavlinkParameterManageEditDialog::SearthParametersTableWidget()
{

    if(!ui){
        return ;
    }

    if(!ui->tableWidget_UAVFlyManage_MavlinkParameterManageEdit){
        return ;
    }

    if(!ui->treeWidget_UAVFlyManage_MavlinkParameterManageEdit){
        return ;
    }

    if(!ui->lineEdit_UAVFlyManage_MavlinkParameterManageEdit_SearthParameters){
        return ;
    }

    if(!ui->pushButton_UAVFlyManage_MavlinkParameterManageEdit_SearthParameters){
        return ;
    }

    auto *tableWidget = ui->tableWidget_UAVFlyManage_MavlinkParameterManageEdit;
    if(!tableWidget){
        return ;
    }

    tableWidget->clearContents();
    tableWidget->clear();
    tableWidget->setRowCount(0);
    {
        const QStringList titleList =
        {
            "Component",
            "Category",
            "Group",
            "Name",
            "Value",
            "Units",
            "Min",
            "Max",
            "Short Description",
            "Description",
        };

        tableWidget->setColumnCount(titleList.size());

        tableWidget->setHorizontalHeaderLabels(titleList);
    }

    MissionManagerTableWidgetDisconnect();

    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    if (!m_vehicle) {
        return ;
    }

    ParameterManager* parameterManager = m_vehicle->parameterManager();
    if(!parameterManager){
        return ;
    }

    const QString& searthParameters =  ui->lineEdit_UAVFlyManage_MavlinkParameterManageEdit_SearthParameters->text();
    if(searthParameters.isEmpty()){
        UpdateParametersTableWidget();
        return ;
    }

    int row = 0;
    for(const auto& componentId : parameterManager->componentIds()){
        for(const auto& parameterName : parameterManager->parameterNames(componentId)){
            const auto* paraFact = parameterManager->getParameter(componentId, parameterName);
            if(paraFact){
                const auto& name             = paraFact->name();
                if(!name.contains(searthParameters, Qt::CaseSensitivity::CaseInsensitive)){
                    continue ;
                }

                int column = 0;
                tableWidget->insertRow(tableWidget->rowCount());

                const auto& category         = paraFact->category();
                const auto& group            = paraFact->group();
                const auto& rawValueString   = paraFact->rawValueString();
                const auto& rawUnits         = paraFact->rawUnits();
                const auto& cookedMinString  = paraFact->cookedMinString();
                const auto& cookedMaxString  = paraFact->cookedMaxString();
                const auto& shortDescription = paraFact->shortDescription();
                const auto& longDescription  = paraFact->longDescription();

                QTableWidgetItem* seqItem = new QTableWidgetItem(QString::number(row + 1));
                if(seqItem){
                    seqItem->setData(Qt::UserRole, componentId);
                    tableWidget->setVerticalHeaderItem(row, seqItem);
                    seqItem->setToolTip(longDescription);
                }

                tableWidget->setItem(row, column, new QTableWidgetItem(QString::number(componentId)));
                column++;

                tableWidget->setItem(row, column, new QTableWidgetItem(category));
                column++;

                tableWidget->setItem(row, column, new QTableWidgetItem(group));
                column++;

                tableWidget->setItem(row, column, new QTableWidgetItem(name));
                column++;

                tableWidget->setItem(row, column, new QTableWidgetItem(rawValueString));
                column++;

                tableWidget->setItem(row, column, new QTableWidgetItem(rawUnits));
                column++;

                tableWidget->setItem(row, column, new QTableWidgetItem(cookedMinString));
                column++;

                tableWidget->setItem(row, column, new QTableWidgetItem(cookedMaxString));
                column++;

                tableWidget->setItem(row, column, new QTableWidgetItem(shortDescription));
                column++;

                tableWidget->setItem(row, column, new QTableWidgetItem(longDescription));
                column++;

                row++;
            }
        }
    }

    tableWidget->setColumnWidth(0,  110);
    tableWidget->setColumnWidth(1,  80);

    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
    tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive);
    for(int column = 0; column < tableWidget->columnCount(); column++ ){
        tableWidget->horizontalHeader()->setSectionResizeMode(column, QHeaderView::Interactive);
    }

    MissionManagerTableWidgetConnect();
}

void UAVFlyManage_MavlinkParameterManageEditDialog::_parametersReady(bool parametersReady)
{
    qDebug() << __FILE__ << __LINE__ << parametersReady;
    UpdateParametersTableWidget();
}

void UAVFlyManage_MavlinkParameterManageEditDialog::_loadProgressChanged(float value)
{
    qDebug() << __FILE__ << __LINE__ << value * 100;
    if(!ui){
        return ;
    }

    if(!ui->progressBar_UAVFlyManage_MavlinkParameterManageEdit){
        return ;
    }

    auto *progressBar = ui->progressBar_UAVFlyManage_MavlinkParameterManageEdit;
    if(!progressBar){
        return ;
    }

    if(progressBar->isHidden()){
        progressBar->show();
    }

    progressBar->setValue(static_cast<int>(value * 100));

    if(m_refreshParameter){
        m_refreshParameter = false;
        _parametersReady(true);
    }
}

void UAVFlyManage_MavlinkParameterManageEditDialog::on_tableWidget_UAVFlyManage_MavlinkParameterManageEdit_cellChanged(int row, int column)
{

}

void UAVFlyManage_MavlinkParameterManageEditDialog::on_pushButton_UAVFlyManage_MavlinkParameterManageEdit_RefreshAllParameters_clicked()
{
    if (!m_vehicle) {
        return ;
    }

    ParameterManager* parameterManager = m_vehicle->parameterManager();
    if(!parameterManager){
        return ;
    }

    parameterManager->refreshAllParameters();
}

void UAVFlyManage_MavlinkParameterManageEditDialog::on_pushButton_UAVFlyManage_MavlinkParameterManageEdit_RefreshParameters_clicked()
{

    if(!ui){
        return ;
    }

    if(!ui->tableWidget_UAVFlyManage_MavlinkParameterManageEdit){
        return ;
    }

    auto *tableWidget = ui->tableWidget_UAVFlyManage_MavlinkParameterManageEdit;
    if(!tableWidget){
        return ;
    }

    const auto currentRow = tableWidget->currentRow();
    if(currentRow < 0){
        LyMessageBox::warning(nullptr, tr("Tip"), QString(tr("Please select the parameter to refresh!")), QMessageBox::Yes);
        return ;
    }

    bool ok = false;
    int componentId = 0;
    QTableWidgetItem* seqItem = tableWidget->verticalHeaderItem(currentRow);
    if(seqItem){
        componentId = seqItem->data(Qt::UserRole).toInt(&ok);

        if(!ok){
            return ;
        }
    }

    if(componentId < 0){
        return ;
    }

    const int columnItem = 3;
    QTableWidgetItem* itemParam1 = tableWidget->item(currentRow, columnItem);
    if(!itemParam1 || itemParam1->text().isEmpty()){
        return ;
    }
    const QString& parameterName(itemParam1->text());

    if (!m_vehicle) {
        return ;
    }

    ParameterManager* parameterManager = m_vehicle->parameterManager();
    if(!parameterManager){
        return ;
    }

    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), QString(tr("Are you sure need to refresh parameter: %1 : %2 ?")).arg(componentId).arg(parameterName), QMessageBox::Yes | QMessageBox::No)) {
        m_refreshParameter = true;
        parameterManager->refreshParameter(componentId, parameterName);
    }
}

void UAVFlyManage_MavlinkParameterManageEditDialog::on_pushButton_UAVFlyManage_MavlinkParameterManageEdit_EditParameters_clicked()
{
    if(!ui){
        return ;
    }

    if(!ui->tableWidget_UAVFlyManage_MavlinkParameterManageEdit){
        return ;
    }

    if(!ui->progressBar_UAVFlyManage_MavlinkParameterManageEdit){
        return ;
    }

    auto *tableWidget = ui->tableWidget_UAVFlyManage_MavlinkParameterManageEdit;
    if(!tableWidget){
        return ;
    }

    const auto currentRow = tableWidget->currentRow();
    if(currentRow < 0){
        LyMessageBox::warning(nullptr, tr("Tip"), QString(tr("Please select the parameter to modify!")), QMessageBox::Yes);
        return ;
    }

    bool ok = false;
    int componentId = 0;
    QTableWidgetItem* seqItem = tableWidget->verticalHeaderItem(currentRow);
    if(seqItem){
        componentId = seqItem->data(Qt::UserRole).toInt(&ok);

        if(!ok){
            return ;
        }
    }

    if(componentId < 0){
        return ;
    }

    const int columnItem = 3;
    QTableWidgetItem* itemParamName = tableWidget->item(currentRow, columnItem);
    if(!itemParamName || itemParamName->text().isEmpty()){
        return ;
    }
    const QString& parameterName(itemParamName->text());

    QTableWidgetItem* itemParamValue = tableWidget->item(currentRow, columnItem + 1);
    if(!itemParamValue || itemParamValue->text().isEmpty()){
        return ;
    }

    const QString& parameterValue(itemParamValue->text());
    if(parameterValue.isEmpty()){
        return ;
    }

    if (!m_vehicle) {
        return ;
    }

    ParameterManager* parameterManager = m_vehicle->parameterManager();
    if(!parameterManager){
        return ;
    }

    auto* paraFact = parameterManager->getParameter(componentId, parameterName);
    if(!paraFact){
        return ;
    }

    const auto* metaData = paraFact->metaData();
    if(!metaData){
        return ;
    }

    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), QString(tr("Are you sure need to modify parameter: %1 : %2 : %3 ?")).arg(componentId).arg(parameterName).arg(parameterValue), QMessageBox::Yes | QMessageBox::No)) {

        const FactMetaData::ValueType_t valueType = paraFact->type();
        QVariant rawValue(parameterValue);

        m_refreshParameter = false;
        parameterManager->_factRawValueUpdateWorker(componentId, parameterName, valueType, rawValue);
    }
}

void UAVFlyManage_MavlinkParameterManageEditDialog::on_treeWidget_UAVFlyManage_MavlinkParameterManageEdit_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    if(!ui){
        return ;
    }

    if(!ui->tableWidget_UAVFlyManage_MavlinkParameterManageEdit){
        return ;
    }

    if(!ui->treeWidget_UAVFlyManage_MavlinkParameterManageEdit){
        return ;
    }

    auto *tableWidget = ui->tableWidget_UAVFlyManage_MavlinkParameterManageEdit;
    if(!tableWidget){
        return ;
    }

    auto *treeWidget = ui->treeWidget_UAVFlyManage_MavlinkParameterManageEdit;
    if(!treeWidget){
        return ;
    }

}

void UAVFlyManage_MavlinkParameterManageEditDialog::on_lineEdit_UAVFlyManage_MavlinkParameterManageEdit_SearthParameters_textChanged(const QString &arg1)
{
    SearthParametersTableWidget();
}

void UAVFlyManage_MavlinkParameterManageEditDialog::on_pushButton_UAVFlyManage_MavlinkParameterManageEdit_SearthParameters_clicked()
{
    SearthParametersTableWidget();
}

