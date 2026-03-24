#ifndef UAVSELECTDIALOG_H
#define UAVSELECTDIALOG_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QButtonGroup>
#include <QMap>


#include "UAVPushButton.h"
#include "FlowLayout.h"


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

    const QMap<UAVPushButton *, QString> getButtonListWeaponComponent() const;

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
    QMap<UAVPushButton *, QString>                  m_ButtonListWeaponComponent;
};

#endif
