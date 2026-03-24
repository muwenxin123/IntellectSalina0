#ifndef IUAVNAVIGATIONARMSHOTALONE54_ALL_DIALOG_H
#define IUAVNAVIGATIONARMSHOTALONE54_ALL_DIALOG_H

#include <QWidget>
#include <QCheckBox>
#include <QTimer>
#include <QMap>

namespace Ui {
class IUAVNavigationArmShotAlone54_ALL_Dialog;
}


class IUAVNavigationArmShotAlone54Dialog;

class IUAVNavigationArmShotAlone54_ALL_Dialog : public QWidget
{
    Q_OBJECT

public:
    explicit IUAVNavigationArmShotAlone54_ALL_Dialog(QWidget *parent = nullptr);
    ~IUAVNavigationArmShotAlone54_ALL_Dialog();

    bool initSensorAndBombList(std::list<IUAVNavigationArmShotAlone54Dialog *>   pIUAVNavigationArmShotAlone54DialogList);

private slots:
    void on_pushButton_Select_Cancle_clicked();

    void on_pushButton_Select_ALL_clicked();

    void on_pushButton_Control_lock_clicked();

    void on_pushButton_Control_unlock_clicked();

    void on_pushButton_Control_Percussion_clicked();

    void on_pushButton_95_1_38MM_Control_Continue_Percussion_Stop_clicked();

    void on_pushButton_95_1_38MM_Control_Continue_Percussion_Start_clicked();

private:
    Ui::IUAVNavigationArmShotAlone54_ALL_Dialog *ui;

    QMap<IUAVNavigationArmShotAlone54Dialog *, QCheckBox *> m_pIUAVNavigationArmShotAlone54Dialog_95_1__38_MM_List;

    unsigned int                               m_Times_95_1_38MM_Control_Continue_Percussion;
    QTimer                                     m_Timer_95_1_38MM_Control_Continue_Percussion;
};

#endif // IUAVNAVIGATIONARMSHOTALONE54_ALL_DIALOG_H
