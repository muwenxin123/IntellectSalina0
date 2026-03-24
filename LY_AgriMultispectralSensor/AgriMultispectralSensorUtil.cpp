#include <iostream>
#include <string>
#include <qmath.h>

#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include <QDir>
#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDateTime>
#include <QDirIterator>
#include <QtXml/QDomDocument>
#include <QFile>
#include <QDebug>
#include <QPainterPath>


#include "AgriMultispectralSensorUtil.h"

using namespace std;

json AgriMultispectralSensorUtil::SerializeToJson(const rttr::instance& obj)
{
	json j;
	rttr::type type = obj.get_type();

	for (auto& prop : type.get_properties())
	{
		rttr::variant value = prop.get_value(obj);
		if (value.is_valid())
			j[prop.get_name().to_string()] = value.to_string();
	}

	return j;
}

//查找目录下所有的文件夹
static void findDir(std::string dir, std::vector<std::string>& subDirs)
{
	subDirs.clear();
	QDir fromDir(QString::fromLocal8Bit(dir.c_str()));
	QStringList filters;
	//
	QFileInfoList fileInfoList = fromDir.entryInfoList(filters, QDir::AllDirs | QDir::Files);
	foreach(QFileInfo fileInfo, fileInfoList)
	{
		if (fileInfo.fileName() == "." || fileInfo.fileName() == "..")
		{
			continue;
		}
		if (fileInfo.isDir())
		{
			QByteArray dir = fileInfo.filePath().toLocal8Bit();
			subDirs.push_back(dir.data());
		}
	}
}
//得到文件路径的文件名   C:\\b\\a(.txt) -> a
static std::string DirOrPathGetName(std::string filePath)
{
	size_t m = filePath.find_last_of('/');
	if (m == string::npos)
	{
		return filePath;
	}
	size_t p = filePath.find_last_of('.');
	if (p != string::npos && p > m)				//没有点号或者
	{
		filePath.erase(p);
	}
	std::string dirPath = filePath;
	dirPath.erase(0, m + 1);
	return dirPath;
}
void AgriMultispectralSensorUtil::CreateObliqueIndexes(std::string fileDir)
{
	std::string dataDir = fileDir + "/Data";
	osg::ref_ptr<osg::Group> group = new osg::Group();
	std::vector<std::string> subDirs;
	findDir(dataDir, subDirs);
	for (size_t i = 0; i < subDirs.size(); i++)
	{
		std::string name = DirOrPathGetName(subDirs[i]);
		std::string path = subDirs[i] + "/" + name + ".osgb";
		osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(path);
		osg::ref_ptr<osg::PagedLOD> lod = new osg::PagedLOD();
		if (!node || !lod) continue;
		auto bs = node->getBound();
		auto c = bs.center();
		auto r = bs.radius();
		lod->setCenter(c);
		lod->setRadius(r);
		lod->setRangeMode(osg::LOD::RangeMode::PIXEL_SIZE_ON_SCREEN);
		osg::ref_ptr<osg::Geode> geode = new osg::Geode;
		geode->getOrCreateStateSet();
		lod->addChild(geode.get());
		std::string relativeFilePath = "./Data/" + name + "/" + name + ".osgb";  //相对路径
		lod->setFileName(0, "");
		lod->setFileName(1, relativeFilePath);
		lod->setRange(0, 0, 1.0);																							//第一层不可见
		lod->setRange(1, 1.0, FLT_MAX);
		lod->setDatabasePath("");
		group->addChild(lod);
	}
	std::string outputLodFile = fileDir + "/Data.osgb";
	osgDB::writeNodeFile(*group, outputLodFile);
}

QJsonObject AgriMultispectralSensorUtil::StringToJson(const QString jsonString)
{
	QJsonObject jsonObject;
	QTextCodec *tc = QTextCodec::codecForName("UTF-8");//防止中文乱码
	QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonString.toUtf8().data());
	if (!jsonDocument.isNull()) {
		jsonObject = jsonDocument.object();
	}
	return jsonObject;
}

QString AgriMultispectralSensorUtil::JsonToString(const QJsonObject& jsonObject)
{
	return QString(QJsonDocument(jsonObject).toJson());
}

QString AgriMultispectralSensorUtil::FormatToCompactISO(const QString& isoInput)
{
	QDateTime dt = QDateTime::fromString(isoInput, Qt::ISODateWithMs);  // 自动处理 T 和毫秒
	if (!dt.isValid()) {
		dt = QDateTime::fromString(isoInput, Qt::ISODate);  // 尝试无毫秒
	}
	if (dt.isValid()) {
		return dt.toString("yyyyMMdd'T'HHmmss");
	}
	return QString();
}

void AgriMultispectralSensorUtil::AddModelsToEarth(const QString &tilesDirPath, const QString &earthFilePath, const QString &urlPrefix)
{
	// 1. 读取 .earth 文件
	QFile earthFile(earthFilePath);
	if (!earthFile.open(QIODevice::ReadOnly)) {
		qWarning() << "无法打开 earth 文件:" << earthFilePath;
		return;
	}

	QDomDocument doc;
	if (!doc.setContent(&earthFile)) {
		qWarning() << "解析 earth XML 文件失败";
		earthFile.close();
		return;
	}
	earthFile.close();

	// 2. 找到 <map> 节点
	QDomNodeList mapNodes = doc.elementsByTagName("map");
	if (mapNodes.isEmpty()) {
		qWarning() << "未找到 <map> 节点";
		return;
	}
	QDomElement mapElement = mapNodes.at(0).toElement();

	// 3. 遍历 tilesDirPath 下所有子目录
	QDirIterator it(tilesDirPath, QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
	while (it.hasNext()) {
		QString dirPath = it.next();
		QDir dir(dirPath);
		QString relativePath = QDir(tilesDirPath).relativeFilePath(dirPath);

		QString osgbFileName = dir.dirName() + ".osgb";
		QString osgbUrl = urlPrefix + "/" + relativePath + "/" + osgbFileName;
		osgbUrl = QDir::cleanPath(osgbUrl).replace("\\", "/");  // 兼容 Windows 路径

																// 4. 创建 <model> 元素
		QDomElement modelElem = doc.createElement("model");
		modelElem.setAttribute("driver", "simple");
		modelElem.setAttribute("enabled", "true");
		modelElem.setAttribute("visible", "true");
		modelElem.setAttribute("cache_enabled", "true");
		modelElem.setAttribute("loading_priority_offset", "3");
		modelElem.setAttribute("location", "116.19752 40.26351 0");
		modelElem.setAttribute("lod_scale", "1");
		modelElem.setAttribute("max_range", "50000");
		modelElem.setAttribute("min_range", "0");
		modelElem.setAttribute("name", osgbFileName);
		modelElem.setAttribute("orientation", "0 0 0");
		modelElem.setAttribute("paged", "true");
		modelElem.setAttribute("patch", "true");
		modelElem.setAttribute("scale", "1 1 1");
		modelElem.setAttribute("url", osgbUrl);

		// 5. 添加到 <map>
		mapElement.appendChild(modelElem);
	}

	// 6. 写回 .earth 文件
	if (!earthFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		qWarning() << "无法写入 earth 文件";
		return;
	}
	QTextStream out(&earthFile);
	doc.save(out, 4); // 4 表示缩进
	earthFile.close();

	qDebug() << "模型节点添加完成";
}

bool AgriMultispectralSensorUtil::CalcPolygonCenterPoint(const QVector<QPointF>& zonePoints, QPointF& point)
{
	int n = zonePoints.size();
	if (n < 3) {
		//std::cerr << "错误：多边形至少需要 3 个点！" << std::endl;
		return false;
	}

#if 0

	double area = 0.0;
	double cx = 0.0;
	double cy = 0.0;

	for (int i = 0; i < n; ++i) {
		const QPointF& p0 = zonePoints[i];
		const QPointF& p1 = zonePoints[(i + 1) % n]; // 环形连接

		double cross = p0.x() * p1.y() - p1.x() * p0.y();
		area += cross;
		cx += (p0.x() + p1.x()) * cross;
		cy += (p0.y() + p1.y()) * cross;
	}

	area *= 0.5;
	if (std::abs(area) < 1e-10) return false; // 避免除以0

	cx /= (6.0 * area);
	cy /= (6.0 * area);

	point.setX(cx);
	point.setY(cy);

#else

	// WGS84 constants
	constexpr double deg2rad = M_PI / 180.0;
	constexpr double rad2deg = 180.0 / M_PI;

	double x = 0, y = 0, z = 0;
	for (const auto& pt : zonePoints) {
		double lat = pt.y() * deg2rad;
		double lon = pt.x() * deg2rad;

		x += cos(lat) * cos(lon);
		y += cos(lat) * sin(lon);
		z += sin(lat);
	}

	int total = zonePoints.size();
	x /= total;
	y /= total;
	z /= total;

	double hyp = sqrt(x * x + y * y);
	double lat = atan2(z, hyp);
	double lon = atan2(y, x);

	point.setX(lon * rad2deg);
	point.setY(lat * rad2deg);

#endif

	return true;
}

bool AgriMultispectralSensorUtil::IsPolygonCompletelyInside(const QPolygonF& outer, const QPolygonF& inner)
{
	QPainterPath outerPath;
	outerPath.addPolygon(outer);
	outerPath.closeSubpath();

	QPainterPath innerPath;
	innerPath.addPolygon(inner);
	innerPath.closeSubpath();

	return outerPath.contains(innerPath);
}

QPolygonF AgriMultispectralSensorUtil::CalculateIntersection(const QPolygonF& polygon1, const QPolygonF& polygon2)
{
	QPolygonF intersectionPolygon;

	intersectionPolygon = polygon1.intersected(polygon2);
	if (!intersectionPolygon.isEmpty()) {
		// qDebug() << "交集区域点数：" << intersectionPolygon.size();
	}
	else {
		// qDebug() << "无交集";
	}

	return intersectionPolygon;
}

// 近似转换为米的比例（适用于小区域）
const double EARTH_RADIUS = 6378137.0; // 地球半径（单位：米）

bool AgriMultispectralSensorUtil::CalcPolygonArea(const QVector<QPointF>& zonePoints, double& area)
{
	// 至少需要3个点形成多边形
	int n = zonePoints.size();
	if (n < 3) return false;

	QPointF origin = zonePoints[0];
	std::vector<osg::Vec3d> utmPoints;
	for (const auto& point : zonePoints)
	{
		osg::Vec3d tmpPoint;

		double dLon = (point.x() - origin.x()) * M_PI / 180.0;
		double dLat = (point.y() - origin.y()) * M_PI / 180.0;

		tmpPoint.x() = EARTH_RADIUS * dLat; // Y 方向
		tmpPoint.y() = EARTH_RADIUS * dLon * cos(origin.y() * M_PI / 180.0); // X 方向

		utmPoints.emplace_back(tmpPoint);
	}

	// 使用 Shoelace 公式计算面积
	double sum = 0.0;
	for (size_t i = 0; i < utmPoints.size(); ++i)
	{
		size_t next = (i + 1) % utmPoints.size();
		sum += (utmPoints[i].x() * utmPoints[next].y()) - (utmPoints[next].x() * utmPoints[i].y());
	}

	area = std::abs(sum) * 0.5;
	return true;
}
