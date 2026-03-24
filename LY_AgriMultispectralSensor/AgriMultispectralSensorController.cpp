#include <QApplication>

#include "AgriMultispectralSensorController.h"
#include "AgriMultispectralSensorUtil.h"

#include "http/http.h"
#include "http/HttpReply.h"

#define MICASENSE_DOMAIN_NAME	"http://192.168.1.83"
#define REQUEST_URL(path) (QUrl(QString(MICASENSE_DOMAIN_NAME) + path))

AgriMultispectralSensorController & AgriMultispectralSensorController::instance()
{
	static AgriMultispectralSensorController i;
	return i;
}

AgriMultispectralSensorController::AgriMultispectralSensorController()
{

}

AgriMultispectralSensorController::~AgriMultispectralSensorController()
{

}

void AgriMultispectralSensorController::asyncGetRequest_status(/*const QByteArray &reqbody, */std::function<void(const AgriMSICameraStatusInfoPtr &)> callback)
{
	QString reqPath = "/status";
	auto reply = Http::instance().get(REQUEST_URL("/status"));
	QObject::connect(reply, &HttpReply::finished, this, [=](const HttpReply &reply) {
		if (reply.isSuccessful()) {
			QByteArray repBody = reply.body();
			AgriMSICameraStatusInfo msiCameraStatus = AgriMultispectralSensorUtil::DeserializeFromJson<AgriMSICameraStatusInfo>(json::parse(repBody));

			AgriMSICameraStatusInfoPtr pMsiCameraStatus = std::make_shared<AgriMSICameraStatusInfo>(msiCameraStatus);
			callback(pMsiCameraStatus);
		}else {
			qDebug() << QString("request [%1] error: [%2] %3 ")
				.arg(reqPath)
				.arg(reply.statusCode())
				.arg(reply.reasonPhrase());
		}
	});
}


