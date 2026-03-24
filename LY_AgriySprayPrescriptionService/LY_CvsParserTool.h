#include <QFile>
#include <QTextStream>
#include <QList>
#include <QString>
#include <QMap>
#include <QDebug>

// CSV 行数据结构
struct CsvRow {
	QMap<QString, QString> fields; // 列名->值的映射

								   // 获取字段值
	QString get(const QString& columnName, const QString& defaultValue = "") const {
		return fields.value(columnName, defaultValue);
	}

	// 设置字段值
	void set(const QString& columnName, const QString& value) {
		fields[columnName] = value;
	}
};

// CSV 表数据结构
struct CsvTable {
	QStringList headers;       // 表头列名
	QList<CsvRow> rows;        // 数据行

							   // 获取行数
	int rowCount() const {
		return rows.size();
	}

	// 获取列数
	int columnCount() const {
		return headers.size();
	}

	// 按行索引获取行
	const CsvRow& rowAt(int index) const {
		return rows.at(index);
	}
};

class CsvParser {
public:
	// 解析整个 CSV 文件
	static CsvTable parseFile(const QString& filePath, QChar delimiter = ',',
		bool firstRowIsHeader = true) {
		CsvTable result;
		QList<QStringList> rawData = parseRaw(filePath, delimiter);

		if (rawData.isEmpty()) return result;

		// 处理表头
		if (firstRowIsHeader && !rawData.isEmpty()) {
			result.headers = rawData.first();
			rawData.removeFirst(); // 移除表头行
		}

		// 创建数据行
		for (const QStringList& row : rawData) {
			if (row.isEmpty()) continue;

			CsvRow csvRow;
			// 如果定义了表头，使用字段名映射
			if (!result.headers.isEmpty()) {
				for (int i = 0; i < qMin(row.size(), result.headers.size()); ++i) {
					csvRow.set(result.headers[i], row[i]);
				}
			}
			// 没有表头则使用列索引作为键名
			else {
				for (int i = 0; i < row.size(); ++i) {
					csvRow.set("Column" + QString::number(i), row[i]);
				}
			}
			result.rows.append(csvRow);
		}

		return result;
	}

private:
	// 内部解析方法，返回原始字符串列表
	static QList<QStringList> parseRaw(const QString& filePath, QChar delimiter = ',') {
		QList<QStringList> result;
		QFile file(filePath);

		if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
			qWarning() << "Cannot open file:" << filePath;
			return result;
		}

		QTextStream in(&file);
		in.setCodec("UTF-8");

		QStringList currentRow;
		QString currentField;
		bool inQuotes = false;
		bool wasQuote = false;

		while (!in.atEnd()) {
			QString chStr = in.read(1);
			if (chStr.isEmpty()) break;
			QChar ch = chStr.at(0);

			// 状态机处理
			if (inQuotes) {
				if (wasQuote) {
					if (ch == '"') {
						currentField += '"';
						wasQuote = false;
					}
					else {
						inQuotes = false;
						wasQuote = false;
						// 引号后只能跟分隔符/换行符/文件结尾
						if (ch != delimiter && ch != '\n' && ch != '\r') {
							currentField += ch;
						}
					}
				}
				else {
					if (ch == '"') {
						wasQuote = true;
					}
					else {
						currentField += ch;
					}
				}
			}
			else {
				if (ch == '"' && currentField.isEmpty()) {
					inQuotes = true;
				}
				else if (ch == delimiter) {
					currentRow.append(currentField);
					currentField.clear();
				}
				else if (ch == '\n') {
					// 处理行结束
					currentRow.append(currentField);
					result.append(currentRow);
					currentRow.clear();
					currentField.clear();
				}
				else if (ch != '\r') {
					currentField += ch;
				}
			}
		}

		// 处理最后一行
		if (!currentField.isEmpty() || !currentRow.isEmpty()) {
			currentRow.append(currentField);
			result.append(currentRow);
		}

		file.close();
		return result;
	}
};