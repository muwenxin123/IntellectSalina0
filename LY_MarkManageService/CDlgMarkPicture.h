#ifndef CDLGMARKPICTURE_H
#define CDLGMARKPICTURE_H

#include <QWidget>
#include "lyUI/LyDialog.h"

namespace Ui
{
class CDlgMarkPicture;
}

typedef std::vector<std::string> PictureNameVec;

class CDlgMarkPicture : public LyDialog
{
    Q_OBJECT

public:
    explicit CDlgMarkPicture(QWidget *parent = nullptr);
    ~CDlgMarkPicture();

public:

    void SetPicNameVec(const PictureNameVec &NameVec);

    void ShowPicture(const int &nIndex);

private slots:

    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

private:

    void showEvent(QShowEvent *event);

private:

    Ui::CDlgMarkPicture				*ui;
    PictureNameVec						m_PictureNameVec;
    int									m_nIndex = 0;
};

#endif
