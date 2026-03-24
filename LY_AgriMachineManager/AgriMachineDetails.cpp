#include <QHeaderView>
#include <QFile>
#include <QDebug>
#include <QXmlStreamReader>

#include "QCString.h"

#include "AgriMachineDetails.h"
#include "LY_AgriMachineManager.h"
#include "ui_AgriMachineDetails.h"

AgriMachineDetails::AgriMachineDetails(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::AgriMachineDetails)
{
	ui->setupUi(this);

	tableEditor = new TableEditor(this);

	loadXmlConfiguration();

	setContentsMargins(0, 10, 0, 0);
}

AgriMachineDetails::~AgriMachineDetails()
{
	delete ui;
}

void AgriMachineDetails::updateDisplayInfo(const AgriMachineBasicInfo& machineInfo)
{
	QString strMachTablename = AgriMachineManagerUtil::getTablenameByGuid(QString::fromStdString(machineInfo.guid));
	
	if (strMachTablename == AGRI_MACHINE_TRACTORS_TABLE_NAME)
	{
		populateTable<AgriMachineTractorsInfo>(machineInfo);
	}
	else if (strMachTablename == AGRI_MACHINE_SUBSOILER_TABLE_NAME)
	{
		populateTable<AgriMachineSubsoilerInfo>(machineInfo);
	}
	else if (strMachTablename == AGRI_MACHINE_GRADER_TABLE_NAME)
	{
		populateTable<AgriMachineGraderInfo>(machineInfo);
	}
	else if (strMachTablename == AGRI_MACHINE_SEEDER_TABLE_NAME)
	{
		populateTable<AgriMachineSeederInfo>(machineInfo);
	}
	else if (strMachTablename == AGRI_MACHINE_HARVESTER_TABLE_NAME)
	{
		populateTable<AgriMachineHarvesterInfo>(machineInfo);
	}
}

void AgriMachineDetails::loadXmlConfiguration()
{
	QFile file(QString::fromStdString(AGRI_PATH_MACHINE_XML_CONFIG));
	if (!file.open(QIODevice::ReadOnly)) {
		qDebug() << tr2("机器xml配置文件打开失败: ") << AGRI_PATH_MACHINE_XML_CONFIG;
		return;
	}

	QXmlStreamReader xml(&file);

	std::string currentCategory;
	bool isCategory = false;

	while (!xml.atEnd())
	{
		xml.readNext();

		if (xml.isStartElement())
		{
			// 检查当前标签是否为 Category 标签（包含子标签 <field>）
			if (xml.name() != "field") 
			{
				currentCategory = xml.name().toString().toStdString();
				isCategory = false;
				QXmlStreamReader::TokenType nextToken = xml.readNext();
				while (nextToken != QXmlStreamReader::EndElement && !xml.atEnd()) 
				{
					if (xml.isStartElement())
					{
						if (xml.name() == "field")
						{
							isCategory = true;
							break;
						} 
						else
						{
							currentCategory = xml.name().toString().toStdString();
						}
					}
					nextToken = xml.readNext();
				}

				if (isCategory)
				{
					//xml.skipCurrentElement();
				}
			}
		}

		// 解析 <field> 元素
		if (xml.name() == "field" && isCategory)
		{
			AgriMachineXmlCfgInfo cfgInfo;
			cfgInfo.name = xml.attributes().value("name").toString().toLocal8Bit();
			cfgInfo.display = xml.attributes().value("display").toString().toLocal8Bit();
			cfgInfo.unit = xml.attributes().value("unit").toString().toLocal8Bit();
			cfgInfo.enums = xml.attributes().value("enums").toString().toLocal8Bit();

			QString visibleStr = xml.attributes().value("visible").toString();
			cfgInfo.visible = (visibleStr == "true");

			if (!currentCategory.empty() && !cfgInfo.name.empty())
			{
				machineCfg[currentCategory][cfgInfo.name] = cfgInfo;
			}
		}

		if (xml.isEndElement() || xml.isWhitespace()) 
		{
			continue;
		}
	}

	if (xml.hasError()) 
	{
		qDebug() << tr2("XML 解析错误 ") << xml.errorString();
		return;
	}
}

/*
void AgriMachineDetails::populateTable(QTableWidget *table, const QVariantMap &data) {
	table->clear();
	table->setRowCount((data.size() + 1)/2);
	table->setColumnCount(4);
	table->verticalHeader()->setVisible(false);
	table->horizontalHeader()->setVisible(false);
	//table->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);
	table->setEditTriggers(QAbstractItemView::NoEditTriggers);

	// 双列展示
	int row = 0, count = 1;
	for (auto it = data.begin(); it != data.end(); ++it, ++count) {
		bool flag = count % 2;
		table->setItem(row, flag ? 0 : 2, new QTableWidgetItem(it.key()));
		QTableWidgetItem *valueItem = new QTableWidgetItem(it.value().toString());
		table->setItem(row, flag ? 1 : 3, valueItem);

		!flag && ++row;
	}
	table->resizeColumnsToContents();
}

QVariantMap AgriMachineDetails::tableToVariantMap(QTableWidget *table) {
	QVariantMap map;
	for (int row = 0; row < table->rowCount(); ++row) {
		QString field = table->item(row, 0)->text();
		QString value = table->item(row, 1)->text();
		map[field] = value;
	}
	return map;
}
*/

TableEditor::TableEditor(QWidget* parent /*= nullptr*/)
	:QTableWidget(parent)
{
	resize(700, 400); 
}

void TableEditor::populateTable(const MapMachineXmlCfg & mapCfg)
{
	clear();

	if (!instanceObj->is_valid() || !objType.is_valid()) {
		//qDebug() << "Invalid instance or type!";
		return;
	}

	auto properties = objType.get_properties();

	// 配置项
	auto typeName = objType.get_name().to_string();
	auto itMap = mapCfg.find(typeName);

	auto countRowNum = [](const int &propCount)->int {
		return (static_cast<int>(propCount) + 1) / 2;
	};

	int propCount = properties.size();
	int tableRowNum = countRowNum(propCount);

	setRowCount(tableRowNum);
	setColumnCount(4); 
	verticalHeader()->setVisible(false);
	horizontalHeader()->setVisible(false);

	int row = 0, count = 1;
	for (auto it = properties.begin(); it != properties.end(); ++it, ++count) 
	{
		bool flag = count % 2;

		QString key(""), value(""), unit("");
		if (itMap != mapCfg.end())
		{
			std::string tmpName = it->get_name().to_string();
			auto &typeConfig = itMap->second;

			auto itTypeConfig = typeConfig.find(tmpName);
			if (itTypeConfig != typeConfig.end())
			{
				const AgriMachineXmlCfgInfo& cfgInfo = itTypeConfig->second;

				// 显隐控制
				if (!cfgInfo.visible) 
				{
					--propCount;
					continue;
				} 

				key = QString::fromLocal8Bit(cfgInfo.display.c_str());
				unit = QString::fromLocal8Bit(cfgInfo.unit.c_str());

				std::string tmpstr = it->get_value(*instanceObj).to_string();

				// 枚举匹配
				if (!cfgInfo.enums.empty())
				{
					value = AgriMachineManagerUtil::FetchValueByEnumStr(QString::fromLocal8Bit(cfgInfo.enums.c_str()), QString::fromLocal8Bit(tmpstr.c_str()));
				}
				else
				{
					value = QString::fromStdString(tmpstr) + (unit.isEmpty() ? "" : "(" + unit + ")");
				}
			}
		}
		else
		{
			// 原始数据展示
			key = QString::fromStdString(it->get_name().to_string());
			std::string tmpstr = it->get_value(*instanceObj).to_string();
			value = QString::fromStdString(tmpstr);
		}

		setItem(row, flag ? 0 : 2, new QTableWidgetItem(key));
		QTableWidgetItem *valueItem = new QTableWidgetItem(value);
		setItem(row, flag ? 1 : 3, valueItem);

		// 使用 HTML 设置工具提示并启用自动换行
		QString toolTipText = QString("<html><body style='width:200px; white-space:normal;'>%1</body></html>").arg(value);
		valueItem->setToolTip(toolTipText);

		// 禁用 key 列的编辑
		item(row, flag ? 0 : 2)->setFlags(item(row, flag ? 0 : 3)->flags() & ~Qt::ItemIsEditable);

		!flag && ++row;
	}

	// 更新表格行数
	tableRowNum = countRowNum(propCount);
	setRowCount(tableRowNum);

	//resizeColumnsToContents();

	horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	setColumnWidth(0, 110);
	setColumnWidth(1, 239);
	setColumnWidth(2, 110);
	setColumnWidth(3, 239);

	// 禁用水平滚动条
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void TableEditor::cellChanged(int row, int column)
{
	if (column != 1 || !instanceObj->is_valid()) return;

	QString key = item(row, 0)->text();
	QString newValue = item(row, 1)->text();

	// 查找属性
	auto prop = objType.get_property(key.toStdString());
	if (!prop.is_valid()) {
		//qDebug() << "Invalid property for key:" << key;
		return;
	}

	// 设置新值
	auto oldValue = prop.get_value(*instanceObj);
	if (oldValue.can_convert<int>()) {
		prop.set_value(*instanceObj, newValue.toInt());
	}
	else if (oldValue.can_convert<double>()) {
		prop.set_value(*instanceObj, newValue.toDouble());
	}
	else if (oldValue.can_convert<std::string>()) {
		prop.set_value(*instanceObj, newValue.toStdString());
	}
	else {
		//qDebug() << "Unsupported type for key:" << key;
	}
}
