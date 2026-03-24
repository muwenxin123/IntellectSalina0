#ifndef QDLGUSERCREATEMODIFY_H
#define QDLGUSERCREATEMODIFY_H

#include <LyDialog.h>
#include <QDialog>
#include "UserManageService.h"

namespace Ui
{
class QDlgUserCreateModify;
}

class QDlgUserCreateModify : public LyDialog
{
    Q_OBJECT

public:
    explicit QDlgUserCreateModify(QWidget *parent = nullptr);
    ~QDlgUserCreateModify();

    void InitUI();

    void UpdateUser(QString strType);

signals:
    void UpdateUserService();

private slots:
    void on_pbtnCancel_clicked();

    void on_pbtnOK_clicked();

public:
    UserManageService *m_puserService;

private:
    Ui::QDlgUserCreateModify *ui;
    QString m_strModifyType;

};

#endif
