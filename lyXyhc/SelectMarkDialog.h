#ifndef _SELECTMARKDIALOG__
#define _SELECTMARKDIALOG__

#include <QWidget>
#include "LyDialog.h"
#include "ui_SelectMarkDialog.h"
#include "lyXyhc.h"

namespace Ui
{
class SelectMarkDialog;
}

class SelectMarkDialog : public LyDialog
{
    Q_OBJECT

public:
    SelectMarkDialog(QWidget *parent = Q_NULLPTR);
    ~SelectMarkDialog();

    void initTableWidget(std::vector<std::pair<int, int>> selectedMarks, std::vector<unsigned long long> selectedMarkIDs,
                         std::vector<std::pair<int, int>> allMarks, std::vector<unsigned long long> MarkIDs);

    void initTableZP(std::vector<std::pair<int, int>> selectedMarks, std::vector<std::pair<int, int>> allMarks);
    void initTableID(std::vector<unsigned long long> selectedMarkIDs, std::vector<unsigned long long> MarkIDs);

    std::vector<std::pair<int, int>> getMarks();
    std::vector<unsigned long long> getMarkIDs();

    void addMark(std::pair<int, int> target);
    void removeMark(std::pair<int, int> target);

    void addMarkID(unsigned long long targetID);
    void removeMarkID(unsigned long long targetID);

    QPushButton *getEnsure();
    QPushButton *getCancel();

public slots:
    void selectBox(int r, int c);
    void selectBoxID(int r, int c);

    void comboBoxChanged();

private:
    Ui::SelectMarkDialog *ui;

    std::vector<std::pair<int, int>> targets;
    std::vector<unsigned long long> targetIDs;
    std::vector<std::pair<int, int>> alltargets;
    std::vector<unsigned long long> alltargetIDs;

    std::map<std::string, std::string> SystemSysNameAndID;
    std::map<std::string, std::string> SystemSysIDAndName;
};

#endif
