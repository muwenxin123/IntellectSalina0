#ifndef _CDLGSETPLAT__
#define _CDLGSETPLAT__

#include "LyDialog.h"
#include "QCString.h"
#include "ui_CDlgSetPlat.h"
#include "MissionPlatType.h"
#include <vector>
#include <string>
#include "lY_TaskPlatMatch.h"

namespace Ui
{
class CDlgSetPlat;
}

class CDlgSetPlat : public LyDialog
{
    Q_OBJECT

public:
    CDlgSetPlat(QWidget *parent = Q_NULLPTR);
    ~CDlgSetPlat();

    void initComBoBox(QStringList strList);
    void setComBoBoxIndex(int Index);
    int  getComCruIndex();
    void initMissionName(QString strName);
    QComboBox *getMissionComBox();

    void setPlatType(missionPlatType _platType);
    missionPlatType getPlatType();
    void updateDialog();

    void setConBoxCurIndexNewName(int curIndex, string newName);

    std::vector<std::string> getPlatNames();
    std::vector<std::string> getCtrlers()
    {
        return platType.getControlers();
    }
    std::string getMissionName()
    {
        return platType.getMissionName();
    }
    std::string getMissionID()
    {
        return platType.getMissionID();
    }

    void setIsSetReTrue(bool _isSetRe)
    {
        isSetRe = _isSetRe;
    }

    void setTime(QString _StartTime, QString _endTime);

    void updateItemsColor();
private slots :

    void on_pbEnsure_clicked();
    void on_pbCancel_clicked();

    void checkIsRepetition(const QString &text);

    void ctrlChanged(const QString &text);

signals:

    void on_pbEnsure_isClicked();

protected:
    virtual void closeEvent(QCloseEvent *event);
private:
    Ui::CDlgSetPlat *ui;
    missionPlatType platType;
    bool isSetRe = false;
    QString   startTime;
    QString	  endTime;
};

#endif