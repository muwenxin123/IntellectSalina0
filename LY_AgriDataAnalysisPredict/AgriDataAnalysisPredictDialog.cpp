#include "AgriDataAnalysisPredictDialog.h"
#include "ui_AgriDataAnalysisPredictDialog.h"
#include "LY_AgriDataAnalysisPredict.h"
#include "AgriAPSimMet.h"

AgriDataAnalysisPredictDialog::AgriDataAnalysisPredictDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AgriDataAnalysisPredictDialog),
    m_show(true)
{
    ui->setupUi(this);
	InitData();

	// 仿真运行结束
	connect(ui->sim_control, &AgriAPSimControl::APSimRunFinishied, [this](const QString& filePath) {
		ui->sim_output->setOutputPath(filePath);
	});
}

AgriDataAnalysisPredictDialog::~AgriDataAnalysisPredictDialog()
{
    delete ui;
}

void AgriDataAnalysisPredictDialog::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);
}

void AgriDataAnalysisPredictDialog::showEvent(QShowEvent *event)
{
    m_show = true;
}

void AgriDataAnalysisPredictDialog::hideEvent(QHideEvent *event)
{
    m_show = false;
}


void AgriDataAnalysisPredictDialog::InitData()
{
}

void AgriDataAnalysisPredictDialog::on_pushButton_clicked()
{

}
