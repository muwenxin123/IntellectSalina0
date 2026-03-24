#ifndef CDLGSYSTEMMESSAGEDISTRUBUTE__
#define CDLGSYSTEMMESSAGEDISTRUBUTE__

#include <QWidget>
#include "LyWidget.h"
#include "ui_CDlgSystemMessageDistribute.h"
#include <string.h>
#include "SelectMarkDialog.h"
#include "SystemPlane_Timer.h"
#include <QDateTime>
#include <QDb.h>
#include <Common/CommonUser.h>

class SelectMarkDialog;

namespace Ui
{
class CDlgSystemMessageDistribute;
}

class CDlgSystemMessageDistribute : public QWidget
{
    Q_OBJECT

public:
    CDlgSystemMessageDistribute(QWidget *parent = Q_NULLPTR);
    ~CDlgSystemMessageDistribute();

    void Init();

    void getDatabaseUser();

    void TabWidgetConnet();
    void CreateTableMenu();

    void getSystemOnlineInfo();

    void localWeaponNameBindToId();

    void initTableTitle();

    void addTopItem_Widget(std::string SystemName);
    void delTopItem_Widget(std::string SystemName);

    void addChildItem(QTreeWidgetItem &topItem, std::pair<int, int> childInfo);
    void addChildItem(QTreeWidgetItem &topItem, unsigned long long childID);
    void addChildItems(QTreeWidgetItem &topItem, std::vector<std::pair<int, int>> childInfos);
    void addChildItemsID(QTreeWidgetItem &topItem, std::vector<unsigned long long> childIDS);
    void removeChildItem(QTreeWidgetItem &topItem, std::pair<int, int> childInfo);
    void removeChildItem(QTreeWidgetItem &topItem, unsigned long long childID);
    void removeAllChildItems(QTreeWidgetItem &topItem);

    void sendMessage_distrubute(std::string systemName, std::vector<std::pair<int, int>> targets);
    void sendMessageID_distrubute(std::string systemName, std::vector<unsigned long long> targets);

    void getNetMessage_distrubute(int nLength, char *lpData);

    void applySystemSysDistribute();

    std::map<std::string, std::string> getSystemSysNameAndID();
    std::map<std::string, std::string> getSystemSysIDAndName();

private slots:

    void showSelectMarkDia();

    void customContextMenuRequested(const QPoint &pos);

    void on_Menu_Add();

    void on_Menu_Remove();

    void updateSystemMarks();

    void deleteTopItem(std::string systemName);

private slots:

    void recvMarkID(int targetID);

private:
    Ui::CDlgSystemMessageDistribute *ui;
    QDb				g_db;
    QTimer         *timer;
    int             updateIndex = 1;

    QMenu						*m_TableMenu;
    QAction						*m_MenuAdd;
    QAction						*m_MenuRemove;

    SelectMarkDialog   *selectMarkDia;
    std::map<std::string, SystemPlane_Timer *> systemOnlines;
    std::map<std::string, std::vector<std::pair<int, int>>> systemNeedMarks;
    std::map<std::string, std::vector<unsigned long long>> systemNeedMarkIDs;
    std::vector<std::pair<int, int>> targets;
    std::vector<unsigned long long> targetIDs;
    std::vector<std::string> targetNames;
    std::vector<_User> g_vCurrUsers;

    std::map<std::string, std::string> SystemSysNameAndID;
    std::map<std::string, std::string> SystemSysIDAndName;
};

#endif
