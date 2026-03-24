#ifndef AGRIFIELDRECORDS_H_
#define AGRIFIELDRECORDS_H_
 
#include <QWidget>

namespace Ui
{
	class AgriFieldRecords;
}

class AgriFieldRecords : public QWidget
{
	Q_OBJECT

public:
	explicit AgriFieldRecords(QWidget *parent = nullptr);
	~AgriFieldRecords();

	void Init();
private:
	void onTabBarClicked(int index);

private:
	Ui::AgriFieldRecords			*ui;
};

#endif  // AgriFieldRecords_H