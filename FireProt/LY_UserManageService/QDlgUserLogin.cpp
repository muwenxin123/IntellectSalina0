#include "QDlgUserLogin.h"
#include "ui_QDlgUserLogin.h"
#include <QCString.h>
#include "LyMessageBox.h"
#include "TinyXML/tinyxml.h"
#include <QDb.h>
#include <QKeyEvent>
#include <QNetworkInterface>
#include "Toast.h"
#include "md5.h"
#include "DBHeartbeatThread.h"
#include <Common/CommonUser.h>
#include "QListView"

using namespace std;

struct _Seat
{
    int seatType;
    QString seatName;
    QString seatPath;
};

struct _Permission
{
    int id;
    int userRole;
    int seatType;
    QString sNote;
    QString seatPath;
};

vector<_User>	g_vUsers;
vector<_User>	g_vCurrPremissionUsers;
std::vector<_Seat> g_vSeats;
std::vector<_Permission> g_vPermissions;
std::vector<_Permission> g_vCurrPermission;
QDb				g_dbLogin;
QString			g_strSeatPath;
DBHeartbeatThread g_DbHeartbeatThread;

QDlgUserLogin::QDlgUserLogin(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QDlgUserLogin)
{
    ui->setupUi(this);

    ui->cbxUserSeat->hide();
    ui->cbxUserSeat->setVisible(false);
	ui->cbxUsername->setView(new QListView());

    m_bisPWDCorrect = false;
    InitUI();
    LoadLoginFile();

	this->setWindowFlag(Qt::FramelessWindowHint);
}

QDlgUserLogin::~QDlgUserLogin()
{
    g_DbHeartbeatThread.quit();
    g_DbHeartbeatThread.wait();

    delete ui;
}

void QDlgUserLogin::InitUI()
{
    ui->ledtPassWord->setEchoMode(QLineEdit::Password);
    QSettings ini("../data/conf/ly.ini", QSettings::IniFormat);
    ini.beginGroup("Database");
    QString strDbName = ini.value("DbName", "LY").toString();
    QString strDbIP = ini.value("SvrIP", "192.168.1.115").toString();
    QString strDbUser = ini.value("User", "root").toString();
    QString strDbPass = ini.value("Password", "123456").toString();
    QString strDbDriver = ini.value("Driver", "QMYSQL").toString();
    ini.endGroup();

    QString strSeatName("");
    ini.beginGroup("Global");
    qDebug() << ini.allKeys() << ini.childKeys();
    if(ini.contains("SeatName")){
        const QString strSeatNameTemp = ini.value("SeatName", "").toString();
        if(QDir("../data/conf/" + strSeatNameTemp).exists()){
            strSeatName = strSeatNameTemp;
        }
    }
    ini.endGroup();

    DBHeartbeatThread::ConnectParam cp;
    cp.ip = strDbIP;
    cp.port = 3306;
    cp.userName = strDbUser;
    cp.password = strDbPass;
    cp.dbName = strDbName;
    g_DbHeartbeatThread.setDBTypeAndConnectParm(DBHeartbeatThread::MySQL, cp);
    connect(&g_DbHeartbeatThread, &DBHeartbeatThread::signal_connectedDB, [strSeatName, cp, this]()
    {
        if (g_dbLogin.m_db.isOpen())
        {
            g_dbLogin.m_db.close();
        }
        g_dbLogin.init(cp.ip.toLatin1().data(), cp.userName.toLatin1().data(), cp.password.toLatin1().data(), cp.dbName.toLatin1().data());
        g_DbHeartbeatThread.m_bConnected = true;
        if (g_vUsers.size())
        {
            return;
        }

        QDbQuery qrySeat = g_dbLogin.exec("SELECT SeatType,SeatName,FilePath FROM SeatInfo where LoginType = 1");
        while (!qrySeat.eof())
        {
            _Seat seat;
            qrySeat >> seat.seatType >> seat.seatName >> seat.seatPath;
            if(!strSeatName.isEmpty()){
                seat.seatPath = strSeatName;
            }
            g_vSeats.push_back(seat);
        }
		if (g_vSeats.size() == 0)
		{
			return;
		}
        for (int i = 0; i < g_vSeats.size(); i++)
        {

            if (g_vSeats[i].seatName == tr("Battle"))
            {
                ui->cbxUserSeat->addItem(tr("Battle Position"), i);
            }
            else if (g_vSeats[i].seatName == tr("Intelligence"))
            {
                ui->cbxUserSeat->addItem(tr("Information Position"), i);
            }
            else if (g_vSeats[i].seatName == tr("Coordination"))
            {
                ui->cbxUserSeat->addItem(tr("Synergetic Position"), i);
            }
            else if (g_vSeats[i].seatName == tr("Individual"))
            {
                continue;
            }
            else
            {
                ui->cbxUserSeat->addItem(tr("Commanding Position"), i);
            }
        }

        ui->cbxUserSeat->setCurrentIndex(0);
        int index = ui->cbxUserSeat->currentIndex();
        if (g_vSeats.empty())
        {
            LyMessageBox::warning(this, tr("Warning"), tr("No seat data"), QMessageBox::Ok);
            return;
        }
        _Seat currSeat = g_vSeats[index];

        QDbQuery qryPermission = g_dbLogin.exec("SELECT id,UserRole,SeatType,Note FROM UserPermission");
        while (!qryPermission.eof())
        {
            _Permission permission;
            qryPermission >> permission.id >> permission.userRole >> permission.seatType >> permission.sNote;
            g_vPermissions.push_back(permission);
        }

        g_vCurrPermission.clear();
        for (auto pm : g_vPermissions)
        {
            if (pm.seatType == currSeat.seatType)
            {
                pm.seatPath = currSeat.seatPath;
                g_vCurrPermission.push_back(pm);
            }
        }

        QDbQuery qry = g_dbLogin.exec("SELECT id,role,lastTime,ip,name,passwd,lastSeat FROM user");
        while (!qry.eof())
        {
            _User user;
            qry >> user.id >> user.role >> user.lastTime >> user.ip >> user.name >> user.passwd >> user.lastSeat;
            g_vUsers.push_back(user);
        }

        g_vCurrPremissionUsers.clear();
        for (auto cpm : g_vCurrPermission)
        {
            for (auto user : g_vUsers)
            {
                if (user.role == cpm.userRole)
                {
                    user.path = cpm.seatPath;
                    g_vCurrPremissionUsers.push_back(user);
                }
            }
        }

        for (int i = 0; i < g_vCurrPremissionUsers.size(); i++)
        {
            ui->cbxUsername->addItem(g_vCurrPremissionUsers[i].name, i);
        }

        {
            LoadLoginFile();

            vector<string> userName;
            for (auto info : g_vUsers)
            {
                std::string name = tr3(info.name);
                userName.emplace_back(name);
            }

            for (int i = 0; i < m_vecLoginRecord.size(); i++)
            {
                if (::find(userName.begin(), userName.end(), m_vecLoginRecord[i].GetLoginName()) != userName.end())
                {
                    if (ui->cbxUserSeat->findText(QString(tr2(m_vecLoginRecord[i].GetLoginProject().c_str()))) > 0)
                    {
                        ui->cbxUserSeat->setCurrentText(QString(tr2(m_vecLoginRecord[i].GetLoginProject().c_str())));
                        break;
                    }
                }
            }
        }
    }
           );
    g_DbHeartbeatThread.start();

    connect(ui->cbxUserSeat, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [ = ](int index)
    {
        on_cbxUserSeat_itemChanged(index);
    });
    setFocus(Qt::ActiveWindowFocusReason);

    QThread::sleep(1);
}

bool QDlgUserLogin::LoadLoginFile(const string &strLoginFile)
{

    m_vecLoginRecord.clear();
    qDebug() << "m_vecLoginRecord" << m_vecLoginRecord.size();

    TiXmlDocument docElement;
    if (!docElement.LoadFile(strLoginFile.c_str()))

    {
        return false;
    }
    TiXmlElement *pRootEle = docElement.RootElement();
    if (!pRootEle)
    {
        return false;
    }
    TiXmlElement *pRecordListEle = pRootEle->FirstChildElement("µÇÂ¼¼ÇÂ¼ÁÐ±í");
    if (!pRecordListEle)
    {
        return false;
    }

    TiXmlElement *pRecordEle = pRecordListEle->FirstChildElement("µÇÂ¼¼ÇÂ¼");
    while (pRecordEle)
    {
        const char *pStrUserName = pRecordEle->Attribute("µÇÂ¼Ãû³Æ");
        const char *pStrUserPwd = pRecordEle->Attribute("µÇÂ¼ÃÜÂë");
        const char *pStLoginProject = pRecordEle->Attribute("µÇÂ½·½°¸");
        if (pStrUserName && pStrUserPwd)
        {
            CLoginRecord record(pStrUserName, pStrUserPwd);
            if (pStLoginProject)
            {
                record.SetLoginProject(pStLoginProject);
            }
            m_vecLoginRecord.push_back(record);
        }
        pRecordEle = pRecordEle->NextSiblingElement("µÇÂ¼¼ÇÂ¼");
    }
    vector<string> userName;
    for (auto info : g_vUsers)
    {
        std::string name = tr3(info.name);
        userName.emplace_back(name);
    }

    for (int i = 0; i < m_vecLoginRecord.size(); i++)
    {
        if (::find(userName.begin(), userName.end(), m_vecLoginRecord[i].GetLoginName()) != userName.end())
        {
            ui->cbxUsername->setCurrentText(tr2(m_vecLoginRecord[i].GetLoginName().c_str()));
            ui->ledtPassWord->setText(m_vecLoginRecord[i].GetLoginPassword().c_str());
        }
    }
    return true;
}

void QDlgUserLogin::on_pbtnUser_2_clicked()
{
    if (ui->ledtPassWord->echoMode() == QLineEdit::Password)
    {
        ui->ledtPassWord->setEchoMode(QLineEdit::Normal);
    }
    else
    {
        ui->ledtPassWord->setEchoMode(QLineEdit::Password);
    }
}

void QDlgUserLogin::on_cbxUserSeat_itemChanged(int index)
{
    ui->cbxUsername->clear();
    int cindex = ui->cbxUserSeat->currentIndex();
    if (g_vSeats.size() < index)
    {
        LyMessageBox::warning(this, tr("Warning"), tr("No seat data"), QMessageBox::Ok);
        return;
    }
    _Seat currSeat = g_vSeats[cindex];
    g_vCurrPermission.clear();
    for (auto pm : g_vPermissions)
    {
        if (pm.seatType == currSeat.seatType)
        {
            pm.seatPath = currSeat.seatPath;
            g_vCurrPermission.push_back(pm);
        }
    }

    g_vCurrPremissionUsers.clear();
    for (auto cpm : g_vCurrPermission)
    {
        for (auto user : g_vUsers)
        {
            if (user.role == cpm.userRole)
            {
                user.path = cpm.seatPath;
                g_vCurrPremissionUsers.push_back(user);
            }
        }
    }

    for (int i = 0; i < g_vCurrPremissionUsers.size(); i++)
    {
        ui->cbxUsername->addItem(g_vCurrPremissionUsers[i].name, i);
    }

    {
        vector<string> userName;
        for (auto info : g_vUsers)
        {
            std::string name = tr3(info.name);
            userName.emplace_back(name);
        }

        for (int i = 0; i < m_vecLoginRecord.size(); i++)
        {
            if (::find(userName.begin(), userName.end(), m_vecLoginRecord[i].GetLoginName()) != userName.end())
            {
                ui->cbxUsername->setCurrentText(tr2(m_vecLoginRecord[i].GetLoginName().c_str()));
                ui->ledtPassWord->setText(m_vecLoginRecord[i].GetLoginPassword().c_str());
            }
        }
    }
}

bool QDlgUserLogin::SaveLoginFile(const string &strLoginFile)
{
    if ("" == strLoginFile)
    {
        return false;
    }
    TiXmlDocument tiDoc;
    TiXmlDeclaration *pDecl = new TiXmlDeclaration("1.0", "GB2312", "");
    if (!pDecl)
    {
        return false;
    }
    tiDoc.LinkEndChild(pDecl);
    TiXmlElement *pRoot = new TiXmlElement("µÇÂ¼¹ÜÀí");
    if (!pRoot)
    {
        return false;
    }
    tiDoc.LinkEndChild(pRoot);
    TiXmlElement *pRootEle = new TiXmlElement("µÇÂ¼¼ÇÂ¼ÁÐ±í");
    if (!pRootEle)
    {
        return false;
    }
    pRoot->LinkEndChild(pRootEle);
    TiXmlElement *pNewEle = new TiXmlElement("µÇÂ¼¼ÇÂ¼");
    char md5_code[33];
    if (!bEncrypt)
    {
        char userPass[255];
        strcpy(userPass, m_userInfo.GetLoginPassword().c_str());
        md5_data(userPass, strlen(userPass), md5_code);
        pNewEle->SetAttribute("µÇÂ¼Ãû³Æ", m_userInfo.GetLoginName().c_str());
        pNewEle->SetAttribute("µÇÂ¼ÃÜÂë", md5_code);
        pNewEle->SetAttribute("µÇÂ½·½°¸", m_userInfo.GetLoginProject().c_str());
    }
    else
    {
        pNewEle->SetAttribute("µÇÂ¼Ãû³Æ", m_userInfo.GetLoginName().c_str());
        pNewEle->SetAttribute("µÇÂ¼ÃÜÂë", m_userInfo.GetLoginPassword().c_str());
        pNewEle->SetAttribute("µÇÂ½·½°¸", m_userInfo.GetLoginProject().c_str());
    }
    pRootEle->LinkEndChild(pNewEle);

    tiDoc.SaveFile(strLoginFile.c_str());
    return true;
}

void QDlgUserLogin::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Return:
        on_pbtnLogin_clicked();
    default:
        break;
    }
    return;
}

void QDlgUserLogin::on_pbtnCancel_clicked()
{
    close();
}

bool QDlgUserLogin::IsPwdCorrect()
{
    if (m_bisPWDCorrect)
    {
        return true;
    }
    return false;
}

int QDlgUserLogin::getUserInfo() const
{
    return m_LoadUserID;
}

bool ipAddrIsOK(const QString &ip)
{
    if (ip.isEmpty())
    {
        return false;
    }

    QStringList list = ip.split('.');
    if (4 != list.size())
    {
        return false;
    }
    for (const auto &num : list)
    {
        bool bOk = false;
        int temp = num.toInt(&bOk);
        if (!bOk || temp < 0 || temp > 255)
        {
            return false;
        }
    }
    return true;
}

#define UserLoginUpdate_Timeout_MSecond		5000
#define UserLoginUpdate_Timeout_Second		5
void QDlgUserLogin::on_pbtnLogin_clicked()
{
    int nSel = ui->cbxUsername->currentIndex();

    if (g_vCurrPremissionUsers.empty())
    {
        LyMessageBox::warning(this, tr("Warning"), tr("No user"), QMessageBox::Ok);
        return;
    }
    QString str = g_vCurrPremissionUsers[nSel].passwd;
    if (g_vCurrPremissionUsers[nSel].passwd != ui->ledtPassWord->text())
    {
        char md5_code[33];
        char userPass[255];
        std::string strWord = tr3(g_vCurrPremissionUsers[nSel].passwd);
        strcpy(userPass, strWord.c_str());
        md5_data(userPass, strlen(userPass), md5_code);
        if (md5_code != ui->ledtPassWord->text())
        {
            LyMessageBox::warning(this, tr("Warning"), tr("The password is incorrect, please reenter!"), QMessageBox::Ok);
            return;
        }
        bEncrypt = true;
    }

    QDateTime dt;
    QString ip;
    QDbQuery qry = g_dbLogin.exec("SELECT lastTime,ip FROM user WHERE id=" + g_vCurrPremissionUsers[nSel].id);
    if (!qry.eof())
    {
        qry >> dt >> ip;
    }
    QSettings ini("../data/conf/ly.ini", QSettings::IniFormat);
    ini.beginGroup("Multicast");
    QString strMulticastNetworkName = tr2(ini.value("Network").toString().toLatin1().data());
    ini.endGroup();
    int nIndex = 0;
    int i;
    const QList<QNetworkInterface> &lni = QNetworkInterface::allInterfaces();
    for (i = 0; i < lni.size(); i++)
    {
        if (lni[i].humanReadableName().indexOf(strMulticastNetworkName) >= 0)
        {
            nIndex = i;
            break;
        }
    }

    QString strLocalIP = lni[nIndex].addressEntries()[0].ip().toString();
    _Seat currSeat = g_vSeats[ui->cbxUserSeat->currentIndex()];
    QDateTime now = QDateTime::currentDateTime();
    if (now.toSecsSinceEpoch() - dt.toSecsSinceEpoch() <= 6 && ip != strLocalIP)
    {
        QString msg;
        msg = tr("The local IP is: [");
        msg += strLocalIP;
        msg += tr("], continue login IP address[");
        msg += ip;
        msg += tr("]the seat will be unavailable. Do you want to continue?");
        if (LyMessageBox::warning(this, tr("Warning"), msg, QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
        {
            return;
        }
    }

    g_dbLogin.exec(QString::asprintf("UPDATE user SET ip='%s' WHERE id=%d", strLocalIP.toLatin1().data(), g_vCurrPremissionUsers[nSel].id));
    g_dbLogin.exec(QString::asprintf("UPDATE user SET lastSeat='%d' WHERE id=%d", currSeat.seatType, g_vCurrPremissionUsers[nSel].id));

    QSettings settings("LY", "lyQgis");
    settings.beginGroup("User");
    settings.setValue("Role", g_vCurrPremissionUsers[nSel].role);
    settings.setValue("Pilot", g_vCurrPremissionUsers[nSel].id);
    settings.setValue("Name", g_vCurrPremissionUsers[nSel].name);

    QDir dir("../data/conf");
    const QFileInfoList &fil = dir.entryInfoList(QStringList() << "layout*.xml" << "module*.ini" << "Menu*.xml");
    for (i = 0; i < fil.size(); ++i)
    {
        QFile::remove(fil[i].absoluteFilePath());
    }

    g_strSeatPath = QDir("../data/conf/" + g_vCurrPremissionUsers[nSel].path).absolutePath() + "/";
    QDir dir2(g_strSeatPath);
    const QStringList &sl = dir2.entryList(QStringList() << "*");
    for (i = 0; i < sl.size(); ++i)
    {
        if (sl[i].size() > 2)
        {
            QFile::copy(g_strSeatPath + sl[i], "../data/conf/" + sl[i]);
        }
    }

    static QTimer timer;
    int nID = g_vCurrPremissionUsers[nSel].id;
    m_LoadUserID = nID;
    connect(&timer, &QTimer::timeout, [strLocalIP, nID]()
    {
        static QDbQuery *preQry = NULL;
        static QDbQuery *preUpd = NULL;
        if (!g_DbHeartbeatThread.m_bConnected)
        {
            if (preQry != NULL)
            {
                delete preQry;
                delete preUpd;
            }
            preUpd = NULL;
            preQry = NULL;
            return;
        }

        if (preQry == NULL)
        {
            preQry = new QDbQuery(g_dbLogin.prepare("SELECT ip FROM user WHERE id=?", 1));
            preUpd = new QDbQuery(g_dbLogin.prepare("UPDATE user SET lastTime=? WHERE id=?", 2));
        }

        QString ip;
        *preQry << nID;
        preQry->ready();
        if (!preQry->eof1())
        {
            *preQry >> ip;
        }

        if (ipAddrIsOK(ip) && ip != strLocalIP)
        {

            Toast::showTip(tr("You have logged in at IP address '%1', the current login has been logged out!").arg(ip), NULL);

        }

        QDateTime now = QDateTime::currentDateTime();
        *preUpd << now << nID;

        preUpd->exec();
    });

    settings.setValue("Seat", ui->cbxUserSeat->currentIndex());
    string strUserSeat = tr3(ui->cbxUserSeat->currentText());
    string strUserName = tr3(ui->cbxUsername->currentText());
    string strPassWord = tr3(ui->ledtPassWord->text());
    m_userInfo.SetLoginProject(strUserSeat);
    m_userInfo.SetLoginName(strUserName);
    m_userInfo.SetLoginPassword(strPassWord);

    m_bisPWDCorrect = true;
    SaveLoginFile();

    accept();
}
