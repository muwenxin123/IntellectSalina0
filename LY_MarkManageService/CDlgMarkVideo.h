#ifndef CDLGMARKVIDEO_H
#define CDLGMARKVIDEO_H

#include <QWidget>
#include "lyUI/LyDialog.h"
#include <QApplication>
#include <QMediaPlayer>
#include <QBuffer>
#include <QVideoWidget>

typedef std::vector<std::string> PictureNameVec;

namespace Ui
{
class CDlgMarkVideo;
}

class CDlgMarkVideo : public LyDialog
{
    Q_OBJECT

public:
    explicit CDlgMarkVideo(QWidget *parent = nullptr);
    ~CDlgMarkVideo();

public:
    void SetPicNameVec(const PictureNameVec &NameVec);

private:

    void showEvent(QShowEvent *event);

private:
    Ui::CDlgMarkVideo				*ui;
    PictureNameVec						m_PictureNameVec;
    int									m_nIndex = 0;
};

#endif
