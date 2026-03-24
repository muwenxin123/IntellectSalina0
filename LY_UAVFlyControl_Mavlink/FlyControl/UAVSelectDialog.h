#ifndef UAVSELECTDIALOG_H
#define UAVSELECTDIALOG_H

#include <QWidget>
#include <QVBoxLayout>
#include <QButtonGroup>

#include "UAVPushButton.h"
#include "FlowLayout.h"

#include "DataManage/XYWeaponManage/XYWeaponManage.h"

namespace Ui {
class UAVSelectDialog;
}

class UAVSelectDialog : public QWidget
{
    Q_OBJECT

public:
    explicit UAVSelectDialog(QWidget *parent = nullptr);
    ~UAVSelectDialog();

    void InitWeaponInfoList();

    const UAVPushButton* getSelectCurrentWeaponComponent() const;

    const UAVPushButton *getSelectAllWeaponComponent() const;

    const QMap<UAVPushButton *, unsigned long long> getButtonListWeaponComponent() const;

private slots:

    void SelectAllWeaponComponent();
    void SelectSingleWeaponComponent();

protected:

    virtual void resizeEvent(QResizeEvent *event);
    virtual void showEvent(QShowEvent *event);
    virtual void hideEvent(QHideEvent *event);

private:
    Ui::UAVSelectDialog                             *ui;
    bool                                            m_show;
    bool                                            m_InitWeaponInfoList;
    QVBoxLayout*                                    m_VerticalLayout;
    const int                                       m_FlowLayoutleftMargins;

    QHBoxLayout*                                    m_FlowLayout;
    UAVPushButton*                                  m_SelectCurrentWeaponComponent;
    UAVPushButton*                                  m_SelectAllWeaponComponent;
    QMap<UAVPushButton *, unsigned long long int>    m_ButtonListWeaponComponent;
    const CXYWeaponManage                           m_CXYWeaponManage;
};

#endif
