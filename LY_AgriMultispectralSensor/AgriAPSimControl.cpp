#include <QScrollBar>
#include <QProcess>
#include <QTimer>
#include <QDebug>
#include <QFileDialog>

#include "AgriAPSimControl.h"
#include "ui_AgriAPSimControl.h"
#include "LyMessageBox.h"
#include "QCString.h"


// 仿真结束
#define SIMULATION_FINISHIED "Simulation is terminating due to end criteria being met."

AgriAPSimControl::AgriAPSimControl(QWidget *parent)
	: QWidget(parent)
    , ui(new Ui::AgriAPSimControl)
{
	ui->setupUi(this);

	ui->progressBar->setValue(0);
}

AgriAPSimControl::~AgriAPSimControl()
{
    delete ui;
}

void AgriAPSimControl::AppendLogMessage(const QString& msg, const QString& color, LineWrappingType optType)
{
	// 格式化当前时间
	//QDateTime dt = QDateTime::currentDateTime();
	//QString strTime = dt.toString("yyyyMMdd HHmmss.zzz");
	//QString ts = QString("<font color='gray'>[%1] </font>").arg(strTime);
	//QString coloredMessage = QString("<font color='%1'>%2</font>").arg(color).arg(msg);

	switch (optType)
	{
	case ADD_TYPE_NOLINE:
	{
		ui->textBrowser_2->moveCursor(QTextCursor::End);
		ui->textBrowser_2->insertHtml(msg);
	}
	break;
	case ADD_TYPE_LINE:
	{
		ui->textBrowser_2->append(msg);
	}
	break;
	case ADD_TYPE_REPLACE_LINE:
	{
		// 获取当前的QTextCursor  
		QTextCursor cursor = ui->textBrowser_2->textCursor();

		// 检查是否有选中的文本或光标是否在有效位置  
		if (!cursor.hasSelection() && cursor.position() != -1) {
			// 定位到当前行的开始  
			cursor.movePosition(QTextCursor::StartOfLine);
			// 保存当前行的开始位置  
			int start = cursor.position();

			// 定位到当前行的结束（下一行的开始或文档末尾）  
			cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
			// 删除当前行  
			cursor.removeSelectedText();

			// 将修改后的cursor设置回QTextBrowser  
			ui->textBrowser_2->setTextCursor(cursor);

			// 设置新的行文本
			ui->textBrowser_2->insertHtml(msg);
		}
	}
	break;
	default:
		break;
	}

	// Scroll to the end.
	QScrollBar* verticalScrollBar = ui->textBrowser_2->verticalScrollBar();
	verticalScrollBar->setValue(verticalScrollBar->maximum());
}

void AgriAPSimControl::SetProcess(double dVal)
{
	if (dVal > 100) return;
	ui->progressBar->setValue(dVal);
	ui->progressBar->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	ui->progressBar->setFormat(QStringLiteral("%1%").arg(QString::number(dVal, 'f', 2)));
}

void AgriAPSimControl::on_BT_LoadData_clicked()
{
	filePath = QFileDialog::getOpenFileName(this, tr("Open Files"), "../data/model/apsim/files", tr("Files (*.apsim)"), nullptr, QFileDialog::Option::ReadOnly);
	ui->FL_Name->setText(filePath);
}

void AgriAPSimControl::on_BT_Play_clicked()
{
	if (!filePath.isEmpty()) {
		QFile file(filePath);
		if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
			file.close();
		}
		else {
			LyMessageBox::warning(this, tr2("错误"), tr2("无法打开文件！"));
		}
	}
	else
	{
		LyMessageBox::warning(this, tr2("错误"), tr2("未选择文件！"));
	}

	ui->textBrowser_2->clear();

	QProcess process;
	QString program = "../data/model/apsim/bin/Model/ApsimModel.exe";
	QStringList arguments;
	arguments << filePath;

	process.start(program, arguments);

	QObject::connect(&process, &QProcess::readyReadStandardOutput, [&]() {
		while (process.canReadLine()) {

			// 更新显示
			QString lineText = process.readLine().replace("\r\n", "");
			AppendLogMessage(lineText, "white", LineWrappingType::ADD_TYPE_LINE);

			// 运行结束
			if (lineText.contains(SIMULATION_FINISHIED))
			{
				QTimer::singleShot(200, this, [&]() {
					SetProcess(100);

					// 推送结束信息
					emit APSimRunFinishied(filePath);
				});
			}
		}
	});

	QObject::connect(&process, &QProcess::readyReadStandardError, [&]() {
		
		QCoreApplication::processEvents();

		auto extractNumeric = [&](const QString &str)->QString {
			QRegExp re("\\d+"); // 正则表达式，匹配一个或多个数字
			int pos = 0;
			QString numericStr;
			while ((pos = re.indexIn(str, pos)) != -1) {
				numericStr += re.cap(0); // 将找到的数字添加到结果字符串中
				pos += re.matchedLength(); // 移动到下一个匹配位置
			}
			return numericStr;
		};

		//qDebug() << extractNumeric(process.readAllStandardError());

		double dPor = (extractNumeric(process.readAllStandardError())).toDouble();
		SetProcess(dPor);

	});

	if (!process.waitForFinished()) {
		qDebug() << "Failed to run the external program.";
	}
	else {
		QString output = process.readAllStandardOutput();
		QString errorOutput = process.readAllStandardError();
		if (!output.isEmpty()) {
			qDebug() << "Program output:" << output;
		}
		if (!errorOutput.isEmpty()) {
			qDebug() << "Program error output:" << errorOutput;
		}
	}
}

void AgriAPSimControl::on_BT_Stop_clicked()
{

}
