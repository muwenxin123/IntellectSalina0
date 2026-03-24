#ifndef INTELLECTIMAGESHANGBODIALOG_H
#define INTELLECTIMAGESHANGBODIALOG_H

#include <QWidget>
#include "Bag_ShangBoHeader.h"
#include <QTcpSocket>
#include <QTimer>
#include <QUdpSocket>

namespace Ui {
class IntellectImageShangBoDialog;
}

class IntellectImageShangBoDialog : public QWidget
{
    Q_OBJECT

public:
    explicit IntellectImageShangBoDialog(QWidget *parent = nullptr);
    ~IntellectImageShangBoDialog();

    void InitWeaponInfoList();

protected:
    void changeEvent(QEvent *e);

private slots:
    void on_pushButton_Control_Common_Up_clicked();

    void on_pushButton_Control_Common_Down_clicked();

    void on_pushButton_Control_Common_Left_clicked();

    void on_pushButton_Control_Common_Right_clicked();

    void on_pushButton_Control_Common_Stop_clicked();

    void on_pushButton_Control_Common_Center_clicked();

    void on_pushButton_Control_Common_BearFollow_clicked();

    void on_pushButton_Control_Common_OffMotor_clicked();

    void on_pushButton_Control_Common_StartDistance_clicked();

    void on_pushButton_Control_Common_StopDistance_clicked();

    void on_pushButton_Trace_Common_Recognition1_clicked();

    void on_pushButton_Trace_Common_Recognition2_clicked();

    void on_pushButton_Image_Common_Photograph_clicked();

    void on_pushButton_Image_Common_StartVideo_clicked();

    void on_pushButton_Image_Common_ChangeMultiple_Add_clicked();

    void on_pushButton_Image_Common_Zoom_Add_clicked();

    void on_pushButton_Image_Common_StopVideo_clicked();

    void on_pushButton_Image_Common_ChangeMulitiple_Sub_clicked();

    void on_checkBox_Trace_Common_OpenRecognition_stateChanged(int arg1);

    void on_pushButton_Control_Connect_clicked();

    void on_pushButton_Control_DisConnect_clicked();

    void on_checkBox_Control_AutoConnect_stateChanged(int arg1);

    void on_lineEdit_Control_ConnectAddress_editingFinished();

    void on_lineEdit_Control_ConnectPort_editingFinished();

    void on_pushButton_Control_Common_HDown_clicked();

    void on_pushButton_Control_Common_SetAngle_clicked();

private:
    void ReadShangBoPodConfig();
    void SaveShangBoPodConfig();
    void InitSocketInfoConnect();
    void InitSocketInfoDisConnect();
    void sendShangBoControlMsg(const ShangBoIntellectMsg::ShangBoControlMsg &shangBoControlMsg);

private:
    Ui::IntellectImageShangBoDialog *ui;
    std::string                     m_StrSensorId;
    std::string                     m_StrComponentID;
    unsigned long long              m_ComponentID;

    QTimer                          m_Timer;
    bool          m_ShangBoPodAutoConnect;

    QTcpSocket    m_socket;
    QHostAddress  m_ServerAddress;
    int           m_ServerPort;
    int           m_LocalPort;
    bool          m_bindPort;

	bool		  b_BearFollow = false;
	bool		  b_Motor = true;
};

#endif
