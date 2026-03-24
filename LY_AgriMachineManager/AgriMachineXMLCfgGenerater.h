#ifndef AGRIMACHINEXMLCFGGENERATER_H_
#define AGRIMACHINEXMLCFGGENERATER_H_

#include <QObject>

#include "DataManage/AgriMachineManager/AgriMachineManagerDecl.h"

class AgriMachineXMLCfgGenerater : public QObject
{
	Q_OBJECT

public:
	explicit AgriMachineXMLCfgGenerater(QObject *parent = nullptr);
	~AgriMachineXMLCfgGenerater();

	bool generateManchineXmlCfg(const QString& filePath);

private:
};

#endif  // AgriMachineXMLCfgGenerater_H