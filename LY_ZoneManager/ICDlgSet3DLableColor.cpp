#include "ICDlgSet3DLableColor.h"
#include "ui_ICDlgSet3DLableColor.h"
#include "QCString.h"
#include "LY_ZoneManager.h"
#include "IZoneManagerActivator.h"
#include "QColor.h"

ICDlgSet3DLableColor::ICDlgSet3DLableColor(QWidget *parent) :
	LyDialog(tr2("±êÅÆÑÕÉ«ÉèÖÃ"), parent),
    ui(new Ui::ICDlgSet3DLableColor)
{
	ui->setupUi(centralWidget());
	this->setResizable(true);
	m_pIZoneManageService = IZoneManagerActivator::Get().getPIZoneManageService();
	connect(ui->btnOK, &QPushButton::clicked, this, &ICDlgSet3DLableColor::on_btnOK_clicked);
	connect(ui->btnCencel, &QPushButton::clicked, this, &ICDlgSet3DLableColor::on_btnCencel_clicked);
	connect(ui->btnDefault, &QPushButton::clicked, this, &ICDlgSet3DLableColor::on_btnDefault_clicked);
	m_vDefaultTRGB = LY_ZoneManager::Get().GetPZoneManageWidget()->m_vDefaultTRGB;
	m_SetColorMap = LY_ZoneManager::Get().GetPZoneManageWidget()->m_SetColorMap;
}

ICDlgSet3DLableColor::~ICDlgSet3DLableColor()
{
    delete ui;
}

void ICDlgSet3DLableColor::Init()
{
	std::vector<std::vector<int>> m_vInitTRGB;
	auto itrcolor = m_SetColorMap.begin();
	for (;itrcolor!=m_SetColorMap.end();itrcolor++)
	{
		if (itrcolor->second)
		{
			m_vInitTRGB = itrcolor->first;
			break;
		}
	}
	QColor DefaultbrackgroundColor(m_vInitTRGB[0][1], m_vInitTRGB[0][2], m_vInitTRGB[0][3]);
	QColor DefaultscriptColor(m_vInitTRGB[1][1], m_vInitTRGB[1][2], m_vInitTRGB[1][3]);
	QColor DefaulframeColor(m_vInitTRGB[2][1], m_vInitTRGB[2][2], m_vInitTRGB[2][3]);
	ui->pb_xsBackGroundColor->setColor(DefaultbrackgroundColor);
	ui->pb_xsScriptColor->setColor(DefaultscriptColor);
	ui->pb_xsFrameColor->setColor(DefaulframeColor);
	ui->ledBackGroundT->setText(QString::number(m_vInitTRGB[0][0]));
	ui->ledScriptT->setText(QString::number(m_vInitTRGB[1][0]));
	ui->ledFrameT->setText(QString::number(m_vInitTRGB[2][0]));
	ui->ledScriptSize->setText(QString::number(m_vInitTRGB[3][0]));
}

void ICDlgSet3DLableColor::on_btnOK_clicked()
{
	m_SetColorMap.clear();
	m_vUpdateTRGB.clear();

	QColor ScriptColor = ui->pb_xsScriptColor->color();
	int uiVScriptColor = (ScriptColor.blue() & 0xFF) << 16 | (ScriptColor.green() & 0xFF) << 8 | (ScriptColor.red() & 0xFF);
	QColor BackGroundColor = ui->pb_xsBackGroundColor->color();
	int uiVBackGroundColor = (BackGroundColor.blue() & 0xFF) << 16 | (BackGroundColor.green() & 0xFF) << 8 | (BackGroundColor.red() & 0xFF);
	QColor FrameColor = ui->pb_xsFrameColor->color();
	int uiVFrameColor = (FrameColor.blue() & 0xFF) << 16 | (FrameColor.green() & 0xFF) << 8 | (FrameColor.red() & 0xFF);

	std::vector<int> trgbbrackground{ ui->ledBackGroundT->text().toInt(), BackGroundColor.red(), BackGroundColor.green(), BackGroundColor.blue() };
	std::vector<int> trgbscript{ ui->ledScriptT->text().toInt(), ScriptColor.red(), ScriptColor.green(), ScriptColor.blue() };
	std::vector<int> trgbframe{ ui->ledFrameT->text().toInt(), FrameColor.red(), FrameColor.green(), FrameColor.blue() };
	std::vector<int> scriptSize{ ui->ledScriptSize->text().toInt() };
	m_vUpdateTRGB.push_back(trgbbrackground);
	m_vUpdateTRGB.push_back(trgbscript);
	m_vUpdateTRGB.push_back(trgbframe);
	m_vUpdateTRGB.push_back(scriptSize);

	m_SetColorMap[m_vDefaultTRGB] = false;
	m_SetColorMap[m_vUpdateTRGB] = true;
	LY_ZoneManager::Get().GetPZoneManageWidget()->m_SetColorMap = m_SetColorMap;
	LY_ZoneManager::Get().GetPZoneManageWidget()->UpdateLableColorShow();

	std::vector<int> addColorDB;
	addColorDB.push_back(uiVBackGroundColor);
	addColorDB.push_back(uiVScriptColor);
	addColorDB.push_back(uiVFrameColor);
	addColorDB.push_back(ui->ledBackGroundT->text().toInt());
	addColorDB.push_back(ui->ledScriptT->text().toInt());
	addColorDB.push_back(ui->ledFrameT->text().toInt());
	addColorDB.push_back(ui->ledScriptSize->text().toInt());
	m_pIZoneManageService->addLableColor(addColorDB);

}

void ICDlgSet3DLableColor::on_btnCencel_clicked()
{
	this->close();
}

void ICDlgSet3DLableColor::on_btnDefault_clicked()
{
	auto itrcolor = m_SetColorMap.begin();
	for (; itrcolor != m_SetColorMap.end(); itrcolor++)
	{
		if (itrcolor->first== m_vDefaultTRGB)
		{
			itrcolor->second = true;
		}
		else
		{
			itrcolor->second = false;
		}
	}
	LY_ZoneManager::Get().GetPZoneManageWidget()->m_SetColorMap = m_SetColorMap;
	LY_ZoneManager::Get().GetPZoneManageWidget()->UpdateLableColorShow();
	Init();
}
