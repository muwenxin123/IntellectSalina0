#ifndef AGRIAPSIMCONTROL_H_
#define AGRIAPSIMCONTROL_H_
 
#include <QWidget>

namespace Ui
{
	class AgriAPSimControl;
}

class AgriAPSimControl : public QWidget
{
	Q_OBJECT

public:
	explicit AgriAPSimControl(QWidget *parent = nullptr);
	~AgriAPSimControl();

private:
	enum LineWrappingType
	{
		ADD_TYPE_NOLINE = 0,        // 不换行
		ADD_TYPE_LINE,              // 换行
		ADD_TYPE_REPLACE_LINE,      // 替换当前行
	};
	void AppendLogMessage(const QString& msg, const QString& color, LineWrappingType optType);
	void SetProcess(double dVal);

signals:
	void APSimRunFinishied(const QString& filePath);

private slots:
	void on_BT_LoadData_clicked();
	void on_BT_Play_clicked();
	void on_BT_Stop_clicked();

private:
	Ui::AgriAPSimControl		*ui;
	QString						filePath;
};

#endif  // AgriAgriAPSimControl_H