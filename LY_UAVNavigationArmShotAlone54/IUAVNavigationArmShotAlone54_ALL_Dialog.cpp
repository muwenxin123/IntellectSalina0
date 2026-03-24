#include "IUAVNavigationArmShotAlone54_ALL_Dialog.h"
#include "ui_IUAVNavigationArmShotAlone54_ALL_Dialog.h"


#include "IUAVNavigationArmShotAlone54Dialog.h"


#include "lyUI/LyMessageBox.h"
#include "QCString.h"


IUAVNavigationArmShotAlone54_ALL_Dialog::IUAVNavigationArmShotAlone54_ALL_Dialog(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::IUAVNavigationArmShotAlone54_ALL_Dialog)
    , m_Times_95_1_38MM_Control_Continue_Percussion(0)
    , m_Timer_95_1_38MM_Control_Continue_Percussion(this)
{
    ui->setupUi(this);

    //ui->groupBox_WeaponInfoList_51_1_38MM->setEnabled(false);


    //ui->pushButton_Select_ALL->setEnabled(false);
    //ui->pushButton_Select_Cancle->setEnabled(false);


    // ui->pushButton_Control_unlock->setEnabled(false);
    ui->pushButton_Control_Percussion->setEnabled(false);


    m_pIUAVNavigationArmShotAlone54Dialog_95_1__38_MM_List.clear();


    connect(&m_Timer_95_1_38MM_Control_Continue_Percussion, &QTimer::timeout, this, [this](){
        if (ui == nullptr) {
            return;
        }

        if (ui->groupBox_95_1_38MM_Control_Continue_Percussion == nullptr) {
            m_Timer_95_1_38MM_Control_Continue_Percussion.stop();
            ui->label_95_1_38MM_Control_Continue_Percussion_State->setText(tr2("状态: 连续击发停止"));
            return;
        }

        if (ui->groupBox_95_1_38MM_Control_Continue_Percussion->isChecked() == false) {
            m_Timer_95_1_38MM_Control_Continue_Percussion.stop();
            ui->label_95_1_38MM_Control_Continue_Percussion_State->setText(tr2("状态: 连续击发停止"));
            return;
        }

        if(m_pIUAVNavigationArmShotAlone54Dialog_95_1__38_MM_List.size() <= 0){
            m_Timer_95_1_38MM_Control_Continue_Percussion.stop();
            ui->label_95_1_38MM_Control_Continue_Percussion_State->setText(tr2("状态: 连续击发停止"));
            return ;
        }

        const int control_Continue_Percussion_Times = ui->spinBox_95_1_38MM_Control_Continue_Percussion_Times->value();
        if (control_Continue_Percussion_Times <= 0 || control_Continue_Percussion_Times > 15) {
            m_Timer_95_1_38MM_Control_Continue_Percussion.stop();
            ui->label_95_1_38MM_Control_Continue_Percussion_State->setText(tr2("状态: 连续击发停止,连续击发95-1弹药次数范围：1~15"));
            return ;
        }

        m_Times_95_1_38MM_Control_Continue_Percussion++;

        if (m_Times_95_1_38MM_Control_Continue_Percussion > control_Continue_Percussion_Times) {
            m_Timer_95_1_38MM_Control_Continue_Percussion.stop();
            ui->label_95_1_38MM_Control_Continue_Percussion_State->setText(QString(tr2("状态: 连续击发停止,击发完成, 击发 %1 次")).arg(m_Times_95_1_38MM_Control_Continue_Percussion - 1));
            return ;
        }


        QString strTimes_95_1_38MM_Control_Continue_Percussion = QString(tr2(" 击发第 %1 次 ")).arg(m_Times_95_1_38MM_Control_Continue_Percussion);

        ui->label_95_1_38MM_Control_Continue_Percussion_State->setText(tr2("状态: 连续击发次数,") + strTimes_95_1_38MM_Control_Continue_Percussion);

        QString strTitle("");
        {
            QMap<IUAVNavigationArmShotAlone54Dialog *, QCheckBox *>::const_iterator iter = m_pIUAVNavigationArmShotAlone54Dialog_95_1__38_MM_List.begin();
            for (; iter != m_pIUAVNavigationArmShotAlone54Dialog_95_1__38_MM_List.end(); ++iter)
            {
                if (iter.value()->isChecked()) {
                    strTitle += iter.key()->windowTitle() + " ";
                }
            }

            if (strTitle.isEmpty()) {
                m_Timer_95_1_38MM_Control_Continue_Percussion.stop();
                ui->label_95_1_38MM_Control_Continue_Percussion_State->setText(tr2("状态: 连续击发停止,未选择击发95-1弹药装备"));
                return ;
            }
        }

        ui->label_95_1_38MM_Control_Continue_Percussion_State->setText(tr2("状态: 连续击发次数,") + strTitle + strTimes_95_1_38MM_Control_Continue_Percussion);

        if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr2("95-1弹药 警告"), (strTitle + strTimes_95_1_38MM_Control_Continue_Percussion + tr2(" : 是否击发95-1弹药?")), QMessageBox::Yes | QMessageBox::No))
        {
            QMap<IUAVNavigationArmShotAlone54Dialog *, QCheckBox *>::const_iterator iter = m_pIUAVNavigationArmShotAlone54Dialog_95_1__38_MM_List.begin();
            for (; iter != m_pIUAVNavigationArmShotAlone54Dialog_95_1__38_MM_List.end(); ++iter)
            {
                if(iter.value()->isChecked()){
                    iter.key()->Control__95_1__38MM__Percussion();
                }
            }
        }

    });
    m_Timer_95_1_38MM_Control_Continue_Percussion.stop();
}

IUAVNavigationArmShotAlone54_ALL_Dialog::~IUAVNavigationArmShotAlone54_ALL_Dialog()
{
    delete ui;
}

bool IUAVNavigationArmShotAlone54_ALL_Dialog::initSensorAndBombList(std::list<IUAVNavigationArmShotAlone54Dialog *> pIUAVNavigationArmShotAlone54DialogList)
{
    if(pIUAVNavigationArmShotAlone54DialogList.size() <= 0){
        return false;
    }

    //
    if (ui == nullptr) {
        return false;
    }

    {
        std::list<IUAVNavigationArmShotAlone54Dialog *>::const_iterator iter = pIUAVNavigationArmShotAlone54DialogList.begin();
        for (; iter != pIUAVNavigationArmShotAlone54DialogList.end(); ++iter)
        {
            if ( ( (*iter)->getSensorType() == "FireControl95-1" && (*iter)->getBombType() == IUAVNavigationArmShotAlone54Dialog::DY54_BombType_Enum::DY54_BombType_Enum_95_1)
                 || ( (*iter)->getSensorType() == "FireControl95-1-Wolf" && (*iter)->getBombType() == IUAVNavigationArmShotAlone54Dialog::DY54_BombType_Enum::DY54_BombType_Enum_95_1_Wolf)
                 )
            {
                const std::string& currentComponentInfo = (*iter)->getCurrentComponentInfo();
                const QString&     titleInfo            = (*iter)->getTitleInfo();
                const QString&     windowTitle          = (*iter)->windowTitle();
                QCheckBox* checkBox = new QCheckBox( windowTitle + QString::fromStdString(currentComponentInfo), nullptr);
                checkBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
                checkBox->setProperty("CurrentComponentInfo", QString::fromStdString(currentComponentInfo));
                ui->verticalLayout_WeaponInfoList_51_1_38MM->addWidget(checkBox);

                m_pIUAVNavigationArmShotAlone54Dialog_95_1__38_MM_List.insert((*iter), checkBox);
            }
        }

    }
    return true;
}

void IUAVNavigationArmShotAlone54_ALL_Dialog::on_pushButton_Select_Cancle_clicked()
{
    QMap<IUAVNavigationArmShotAlone54Dialog *, QCheckBox *>::const_iterator iter = m_pIUAVNavigationArmShotAlone54Dialog_95_1__38_MM_List.begin();
    for (; iter != m_pIUAVNavigationArmShotAlone54Dialog_95_1__38_MM_List.end(); ++iter)
    {
        iter.value()->setChecked(false);
    }
}

void IUAVNavigationArmShotAlone54_ALL_Dialog::on_pushButton_Select_ALL_clicked()
{
    QMap<IUAVNavigationArmShotAlone54Dialog *, QCheckBox *>::const_iterator iter = m_pIUAVNavigationArmShotAlone54Dialog_95_1__38_MM_List.begin();
    for (; iter != m_pIUAVNavigationArmShotAlone54Dialog_95_1__38_MM_List.end(); ++iter)
    {
        iter.value()->setChecked(true);
    }
}

void IUAVNavigationArmShotAlone54_ALL_Dialog::on_pushButton_Control_lock_clicked()
{
    //
    if (ui == nullptr) {
        return ;
    }


    QString strTitle("");
    {
        QMap<IUAVNavigationArmShotAlone54Dialog *, QCheckBox *>::const_iterator iter = m_pIUAVNavigationArmShotAlone54Dialog_95_1__38_MM_List.begin();
        for (; iter != m_pIUAVNavigationArmShotAlone54Dialog_95_1__38_MM_List.end(); ++iter)
        {
            if (iter.value()->isChecked()) {
                strTitle += iter.key()->windowTitle() + " ";
            }
        }

        if (strTitle.isEmpty()) {
            LyMessageBox::warning(nullptr, tr2("95-1弹药 警告"), (strTitle + tr2("请选择控制加锁95-1弹药?")), QMessageBox::Yes | QMessageBox::No);
            return;
        }
    }


    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr2("95-1弹药 警告"), (strTitle + tr2(" : 是否加锁95-1弹药?")), QMessageBox::Yes | QMessageBox::No))
    {

        //ui->groupBox_WeaponInfoList_51_1_38MM->setEnabled(false);


        //ui->pushButton_Select_ALL->setEnabled(false);
        //ui->pushButton_Select_Cancle->setEnabled(false);


        // ui->pushButton_Control_unlock->setEnabled(false);
        ui->pushButton_Control_Percussion->setEnabled(false);


        QMap<IUAVNavigationArmShotAlone54Dialog *, QCheckBox *>::const_iterator iter = m_pIUAVNavigationArmShotAlone54Dialog_95_1__38_MM_List.begin();
        for (; iter != m_pIUAVNavigationArmShotAlone54Dialog_95_1__38_MM_List.end(); ++iter)
        {
            //iter.value()->setEnabled(false);
            if (iter.value()->isChecked()) {
                iter.key()->Control__95_1__38MM__lock();
                iter.value()->setStyleSheet("background-color:none;");
            }
        }
    }
}

void IUAVNavigationArmShotAlone54_ALL_Dialog::on_pushButton_Control_unlock_clicked()
{
    //
    if (ui == nullptr) {
        return ;
    }


    QString strTitle("");
    {
        QMap<IUAVNavigationArmShotAlone54Dialog *, QCheckBox *>::const_iterator iter = m_pIUAVNavigationArmShotAlone54Dialog_95_1__38_MM_List.begin();
        for (; iter != m_pIUAVNavigationArmShotAlone54Dialog_95_1__38_MM_List.end(); ++iter)
        {
            if (iter.value()->isChecked()) {
                strTitle += iter.key()->windowTitle() + " ";
            }
        }

        if (strTitle.isEmpty()) {
            LyMessageBox::warning(nullptr, tr2("95-1弹药 警告"), (strTitle + tr2("请选择控制解锁95-1弹药?")), QMessageBox::Yes | QMessageBox::No);
            return;
        }
    }


    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr2("95-1弹药 警告"), (strTitle + tr2(" : 是否解锁95-1弹药?")), QMessageBox::Yes | QMessageBox::No))
    {
        //ui->groupBox_WeaponInfoList_51_1_38MM->setEnabled(true);


        //ui->pushButton_Select_ALL->setEnabled(true);
        //ui->pushButton_Select_Cancle->setEnabled(true);


        // ui->pushButton_Control_unlock->setEnabled(true);
        ui->pushButton_Control_Percussion->setEnabled(true);

        QMap<IUAVNavigationArmShotAlone54Dialog *, QCheckBox *>::const_iterator iter = m_pIUAVNavigationArmShotAlone54Dialog_95_1__38_MM_List.begin();
        for (; iter != m_pIUAVNavigationArmShotAlone54Dialog_95_1__38_MM_List.end(); ++iter)
        {
            //iter.value()->setEnabled(true);

            if(iter.value()->isChecked()){
                iter.key()->Control__95_1__38MM__unlock();
                iter.value()->setStyleSheet("background-color: red;");
            }
        }
    }
}

void IUAVNavigationArmShotAlone54_ALL_Dialog::on_pushButton_Control_Percussion_clicked()
{
    //
    if (ui == nullptr) {
        return ;
    }


    QString strTitle("");
    {
        QMap<IUAVNavigationArmShotAlone54Dialog *, QCheckBox *>::const_iterator iter = m_pIUAVNavigationArmShotAlone54Dialog_95_1__38_MM_List.begin();
        for (; iter != m_pIUAVNavigationArmShotAlone54Dialog_95_1__38_MM_List.end(); ++iter)
        {
            if (iter.value()->isChecked()) {
                strTitle += iter.key()->windowTitle() + " ";
            }
        }

        if (strTitle.isEmpty()) {
            LyMessageBox::warning(nullptr, tr2("95-1弹药 警告"), (strTitle + tr2("请选择控制击发95-1弹药?")), QMessageBox::Yes | QMessageBox::No);
            return;
        }
    }


    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr2("95-1弹药 警告"), (strTitle + tr2(" : 是否击发95-1弹药?")), QMessageBox::Yes | QMessageBox::No))
    {
        QMap<IUAVNavigationArmShotAlone54Dialog *, QCheckBox *>::const_iterator iter = m_pIUAVNavigationArmShotAlone54Dialog_95_1__38_MM_List.begin();
        for (; iter != m_pIUAVNavigationArmShotAlone54Dialog_95_1__38_MM_List.end(); ++iter)
        {
            if(iter.value()->isChecked()){
                iter.key()->Control__95_1__38MM__Percussion();
            }
        }
    }
}

void IUAVNavigationArmShotAlone54_ALL_Dialog::on_pushButton_95_1_38MM_Control_Continue_Percussion_Stop_clicked()
{
    //
    if (ui == nullptr) {
        return ;
    }

    m_Timer_95_1_38MM_Control_Continue_Percussion.stop();

    if(m_Timer_95_1_38MM_Control_Continue_Percussion.isActive()){
        m_Timer_95_1_38MM_Control_Continue_Percussion.stop();
    }

    if(!m_Timer_95_1_38MM_Control_Continue_Percussion.isActive()){
        ui->label_95_1_38MM_Control_Continue_Percussion_State->setText(tr2("状态: 连续击发停止"));
    }
    else {
        ui->label_95_1_38MM_Control_Continue_Percussion_State->setText(tr2("状态: 连续击发停止失败，注意安全！"));
    }
}

void IUAVNavigationArmShotAlone54_ALL_Dialog::on_pushButton_95_1_38MM_Control_Continue_Percussion_Start_clicked()
{
    //
    if (ui == nullptr) {
        return ;
    }

    if (ui->groupBox_95_1_38MM_Control_Continue_Percussion == nullptr) {
        return;
    }

    if (ui->groupBox_95_1_38MM_Control_Continue_Percussion->isChecked() == false) {
        return;
    }

    if (m_Timer_95_1_38MM_Control_Continue_Percussion.isActive()) {
        LyMessageBox::warning(nullptr, tr2("连续击发 警告"), (tr2("连续击发95-1弹药已经开始，请先停止后再开始?")), QMessageBox::Yes | QMessageBox::No);
        return ;
    }

    const int control_Continue_Percussion_Times = ui->spinBox_95_1_38MM_Control_Continue_Percussion_Times->value();
    if (control_Continue_Percussion_Times <= 0 || control_Continue_Percussion_Times > 15) {
        LyMessageBox::warning(nullptr, tr2("连续击发 警告"), (tr2("请连续击发95-1弹药次数范围：1~15?")), QMessageBox::Yes | QMessageBox::No);
        return;
    }

    QString strTitle("");
    {
        QMap<IUAVNavigationArmShotAlone54Dialog *, QCheckBox *>::const_iterator iter = m_pIUAVNavigationArmShotAlone54Dialog_95_1__38_MM_List.begin();
        for (; iter != m_pIUAVNavigationArmShotAlone54Dialog_95_1__38_MM_List.end(); ++iter)
        {
            if (iter.value()->isChecked()) {
                strTitle += iter.key()->windowTitle() + " ";
            }
        }
    }


    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr2("连续击发 警告"), (strTitle + tr2(": 是否连续击发95-1弹药?")), QMessageBox::Yes | QMessageBox::No))
    {
        if (m_Timer_95_1_38MM_Control_Continue_Percussion.isActive()) {
            m_Timer_95_1_38MM_Control_Continue_Percussion.stop();
        }
        m_Times_95_1_38MM_Control_Continue_Percussion = 0;
        m_Timer_95_1_38MM_Control_Continue_Percussion.setInterval(1000);
        m_Timer_95_1_38MM_Control_Continue_Percussion.start();
    }
}
