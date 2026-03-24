#pragma once
#ifndef NEWFOLDERNAMEDLG_H
#define NEWFOLDERNAMEDLG_H

#include "ui_newfoldernamedlg.h"
#include "LyDialog.h"

class NewFolderNameDlg: public LyDialog
{
public:
    NewFolderNameDlg(const QString &WndName, const QString &FileName, int maxname = 128, QWidget *pParent = nullptr, bool isModal = false);
    ~NewFolderNameDlg();

private slots:
    void OnOK();
protected:
private:
    Ui::NewFolderNameDlg *ui;

public:
    QString m_WndName;
    QString	m_FileName;
    int m_maxnamelen;
};
#endif
