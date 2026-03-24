#ifndef AGRIMULTISPECTRALSENSORCONTROLLER_H_
#define AGRIMULTISPECTRALSENSORCONTROLLER_H_
 
#include "AgriMultispectralSensorDecl.h"

class AgriMultispectralSensorController : public QObject
{
	Q_OBJECT
public:
	AgriMultispectralSensorController();
	virtual ~AgriMultispectralSensorController();

	static AgriMultispectralSensorController &instance();

public:
	void asyncGetRequest_status(/*const QByteArray &reqbody, */std::function<void(const AgriMSICameraStatusInfoPtr &)> callback);
};

#define MSIController() AgriMultispectralSensorController::instance()

#endif  // AgriMultispectralSensorController_H