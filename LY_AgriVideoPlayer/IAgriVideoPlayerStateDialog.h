#ifndef IAGRIVIDEOPLAYERSTATEDIALOG_H
#define IAGRIVIDEOPLAYERSTATEDIALOG_H

#include <QWidget>

namespace Ui {
class IAgriVideoPlayerStateDialog;
}

class IAgriVideoPlayerStateDialog : public QWidget
{
    Q_OBJECT

public:
    explicit IAgriVideoPlayerStateDialog(QWidget *parent = nullptr);
    ~IAgriVideoPlayerStateDialog();

	void Init(const int& index);
private:
    Ui::IAgriVideoPlayerStateDialog *ui;
};

#endif // IAGRIVIDEOPLAYERSTATEDIALOG_H
