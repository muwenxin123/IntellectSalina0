#include "UAVFlyManage_MavlinkApplicationSetting.h"
#include "ui_UAVFlyManage_MavlinkApplicationSetting.h"

#include "QGCApplication.h"

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QGroupBox>

UAVFlyManage_MavlinkApplicationSetting::UAVFlyManage_MavlinkApplicationSetting(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UAVFlyManage_MavlinkApplicationSetting)
{
    ui->setupUi(this);

    if(MavlinkApp()){
        const QList<QGCApplication::AppSettingsValue<double>>& appSettingsDoubleValueList = MavlinkApp()->AppSettingsValueList(0.0);

        const int maxColumn = MavlinkApp()->maxApplicationSettingGridColumns();

        int row        = 0;
        int column     = 0;
        int rowSpan    = 1;
        int columnSpan = 1;
        for( auto& settingsValue : appSettingsDoubleValueList){
            if(!settingsValue.show){
                continue;
            }
            const QString& valueKey  = settingsValue.valueKey;
            const QString& valueName = settingsValue.valueName;
            double& value            = settingsValue.value;
            QGroupBox* groupBox = new QGroupBox(this);
            groupBox->setObjectName(QString::fromUtf8("groupBox_") + valueKey);
            groupBox->setAlignment(Qt::AlignLeft);
            groupBox->setSizePolicy(QSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Preferred));
            groupBox->setMinimumSize(QSize(2, 0));
            groupBox->setTitle(valueName);
            QHBoxLayout* horizontalLayout = new QHBoxLayout(groupBox);
            horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout_") + valueKey);

            QDoubleSpinBox* doubleSpinBox = new QDoubleSpinBox(groupBox);
            doubleSpinBox->setObjectName(QString::fromUtf8("doubleSpinBox_") + valueKey);
            doubleSpinBox->setDecimals(2);
            doubleSpinBox->setMinimum(0.000000000000000);
            doubleSpinBox->setMaximum(10000.989999999999995);
            if(value > 10000.989999999999995){
                doubleSpinBox->setMaximum(2 * value);
            }
            doubleSpinBox->setValue(value);
            connect(doubleSpinBox, &QDoubleSpinBox::editingFinished, [&value, doubleSpinBox](){
                value = doubleSpinBox->value();
                MavlinkApp()->storeSettings();
            });
            horizontalLayout->addWidget(doubleSpinBox);

            ui->gridLayout->addWidget(groupBox, row, column, rowSpan, columnSpan);
            column++;
            if(column >= maxColumn){
                column = 0;
                row++;
            }
        }

        const QList<QGCApplication::AppSettingsValue<int>>& appSettingsIntValueList = MavlinkApp()->AppSettingsValueList(0);

        if(column != 0){
            row++;
        }
        column     = 0;
        rowSpan    = 1;
        columnSpan = 1;
        for( auto& settingsValue : appSettingsIntValueList){
            if(!settingsValue.show){
                continue;
            }
            const QString& valueKey  = settingsValue.valueKey;
            const QString& valueName = settingsValue.valueName;
            int& value               = settingsValue.value;
            QGroupBox* groupBox = new QGroupBox(this);
            groupBox->setObjectName(QString::fromUtf8("groupBox_") + valueKey);
            groupBox->setAlignment(Qt::AlignLeft);
            groupBox->setSizePolicy(QSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Preferred));
            groupBox->setMinimumSize(QSize(2, 0));
            groupBox->setTitle(valueName);
            QHBoxLayout* horizontalLayout = new QHBoxLayout(groupBox);
            horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout_") + valueKey);
            QSpinBox* spinBox = new QSpinBox(groupBox);
            spinBox->setObjectName(QString::fromUtf8("spinBox_") + valueKey);
            spinBox->setMinimum(1);
            spinBox->setMaximum(10000);
            if(value > 10000){
                spinBox->setMaximum(2 * value);
            }
            spinBox->setValue(value);
            connect(spinBox, &QSpinBox::editingFinished, [&value, spinBox](){
                value = spinBox->value();
                MavlinkApp()->storeSettings();
            });
            horizontalLayout->addWidget(spinBox);

            ui->gridLayout->addWidget(groupBox, row, column, rowSpan, columnSpan);
            column++;
            if(column >= maxColumn){
                column = 0;
                row++;
            }
        }

        const QList<QGCApplication::AppSettingsValue<bool>>& appSettingsBoolValueList = MavlinkApp()->AppSettingsValueList(false);

        if(column != 0){
            row++;
        }
        column     = 0;
        rowSpan    = 1;
        columnSpan = 1;
        for( auto& settingsValue : appSettingsBoolValueList){
            if(!settingsValue.show){
                continue;
            }
            const QString& valueKey  = settingsValue.valueKey;
            const QString& valueName = settingsValue.valueName;
            bool& value              = settingsValue.value;
            QGroupBox* groupBox = new QGroupBox(this);
            groupBox->setObjectName(QString::fromUtf8("groupBox_") + valueKey);
            groupBox->setAlignment(Qt::AlignLeft);
            groupBox->setSizePolicy(QSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Preferred));
            groupBox->setMinimumSize(QSize(2, 0));
            groupBox->setTitle(valueName);
            QHBoxLayout* horizontalLayout = new QHBoxLayout(groupBox);
            horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout_") + valueKey);
            QCheckBox* checkBox = new QCheckBox(groupBox);
            checkBox->setObjectName(QString::fromUtf8("checkBox_") + valueKey);
            checkBox->setChecked(value);
            connect(checkBox, &QCheckBox::stateChanged, [&value, checkBox](){
                value = checkBox->isChecked();
                MavlinkApp()->storeSettings();
            });
            horizontalLayout->addWidget(checkBox);

            ui->gridLayout->addWidget(groupBox, row, column, rowSpan, columnSpan);
            column++;
            if(column >= maxColumn){
                column = 0;
                row++;
            }
        }

        QSpacerItem* horizontalSpacer2 = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
        ui->gridLayout->addItem(horizontalSpacer2, row + 1, maxColumn - 1, 1, 1);
    }
}

UAVFlyManage_MavlinkApplicationSetting::~UAVFlyManage_MavlinkApplicationSetting()
{
    delete ui;
}

void UAVFlyManage_MavlinkApplicationSetting::changeEvent(QEvent *e)
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

void UAVFlyManage_MavlinkApplicationSetting::showEvent(QShowEvent *event)
{

}

void UAVFlyManage_MavlinkApplicationSetting::hideEvent(QHideEvent *event)
{

}
