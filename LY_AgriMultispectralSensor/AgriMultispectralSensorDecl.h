#ifndef AGRIMULTISPECTRALSENSORDECL_H_
#define AGRIMULTISPECTRALSENSORDECL_H_

#include <QString>
#include <QVariant>

#include <rttr/registration>

#include "QCString.h"

#include "DataManage/AgriFarm/AgriFarmManagerDecl.h"


#define AGRI_SENTINEL_METADATA_TABLE_NAME			"agri_sentinel_metadata"
#define AGRI_SENTINEL_IMAGE_PATH_PREFIX				"sentinel_image/output/"


/* 植被指数 */
enum class VegetationIndex
{
	VEG_INDEX_RGB,
	VEG_INDEX_NDVI,
	VEG_INDEX_GNDVI,
	VEG_INDEX_NDRE,
	VEG_INDEX_LCI,
	VEG_INDEX_RENDVI,
	VEG_INDEX_TVI,
	VEG_INDEX_DVI,
	VEG_INDEX_NDWI,
	VEG_INDEX_YIELD,
	VEG_INDEX_GROWTH
};

Q_DECLARE_METATYPE(VegetationIndex)


/* 多光谱相机状态信息 */
class AgriMSICameraStatusInfo
{
public:
	AgriMSICameraStatusInfo()
		: sd_gb_free(0.0),
		sd_gb_total(0.0),
		sd_type("Unknown"),
		sd_warn(false),
		sd_status("Full"),
		bus_volts(0.0),
		gps_used_sats(0),
		gps_vis_sats(0),
		gps_warn(false),
		gps_lat(0.0),
		gps_lon(0.0),
		gps_type("No GPS"),
		course(0.0),
		alt_agl(0.0),
		alt_msl(0.0),
		p_acc(0.0),
		utc_time(""),
		vel_2d(0.0),
		auto_cap_active(false),
		dls_status(""),
		gps_time(""),
		utc_time_valid(false),
		time_source("None")
	{};
	~AgriMSICameraStatusInfo() {};

	/*	SD 卡上的可用空间量 （GB）*/
	float					sd_gb_free;		
	/*	SD 卡上的存储总量 （GB）*/
	float					sd_gb_total;
	/*	SD 卡的文件系统类型。“FAT32”、“exFat”或“Unknown”之一*/
	std::string				sd_type;
	/*	如果为 true，则表示满足 SD 空间不足警告阈值或使用不推荐的 SD 文件系统类型*/
	bool					sd_warn;
	/*	SD 卡的状态，“Ok”、“NotPresent”或“Full”之一*/
	std::string				sd_status;
	/*	测得的电源电压*/
	float					bus_volts;
	/*	GPS 定位使用的卫星数*/
	int						gps_used_sats;
	/*	可见 GPS 卫星的数量*/
	int						gps_vis_sats;
	/*	当 GPS 接收器报告检测到干扰时为 True。*/
	bool					gps_warn;
	/*	当前纬度（弧度）*/
	float					gps_lat;
	/*	当前经度（弧度）*/
	float					gps_lon;
	/*	摄像机正在使用的 GPS 源类型。“无 GPS”、“注入”或“Ublox”之一*/
	std::string				gps_type;
	/*	NED 中的行进方向*/
	float					course;
	/*	地面以上高度*/
	float					alt_agl;
	/*	平均海平面以上的海拔高度*/
	float					alt_msl;
	/*	GPS 定位的定位精度*/
	float					p_acc;
	/*	格式为 YYYY-MM-DDTHH:MM:SS.SSS*/
	std::string				utc_time;
	/*	水平速度 （m/s）*/
	float					vel_2d;
	/*	True 表示开启相机的自动拍摄模式*/
	bool					auto_cap_active;
	/*	DLS 状态。'Ok'、'NotPresent'、'Programming'、'Initializing'* 或 'Error'*之一 * 此状态是在 v6.0.0 中添加的*/
	std::string				dls_status;
	/*	格式为 YYYY-MM-DDTHH:MM:SS.SSS*/
	std::string				gps_time;
	/*	当 'true' 时，此状态中提供的utc_time被认为是正确的*/
	bool					utc_time_valid;
	/*	相机使用的时间源类型。“GPS”、“PPS”或“None”之一*/
	std::string				time_source;
};

typedef std::vector<AgriMSICameraStatusInfo>				AgriMSICameraStatusInfoVec;
typedef	std::shared_ptr<AgriMSICameraStatusInfo>			AgriMSICameraStatusInfoPtr;
typedef std::vector<AgriMSICameraStatusInfoPtr>				AgriMSICameraStatusInfoPtrVec;

Q_DECLARE_METATYPE(AgriMSICameraStatusInfo)


/* 哨兵卫星元数据 */
struct AgriSentinelMetadata {
	AgriSentinelMetadata()
		: guid("")
		, file_name("")
		, size(0)
		, identifier("")
		, begin_time("")
		, end_time("")
		, ingestion_date("")
		, orbit_number(0)
		, relative_orbit_number(0)
		, platform_name("")
		, platform_identifier("")
		, sensor_operational_mode("")
		, product_type("")
		, cloud_cover_percentage(0.0)
		, foot_print("")
		, download_url("")
	{};

	/*	数据产品唯一标识符（元数据ID）	"b3a5c..."*/
	QString					guid;
	/*  文件名	"S2A_MSIL2A_20230501T..."*/
	QString					file_name;
	/*	产品大小  "12000000 bytes"*/
	int						size;
	/*	数据产品名称（包含卫星类型、时间等）	"S2A_MSIL2A_20230501T..."*/
	QString					identifier;
	/*	成像开始时间（ISO8601 格式）	"2023-05-01T10:30:12.024Z"*/
	QString					begin_time;
	/*	成像结束时间	"2023-05-01T10:30:54.024Z"*/
	QString					end_time;
	/*	数据入库时间（可用于判定可用性）	"2023-05-01T15:45:00.000Z"*/
	QString					ingestion_date;
	/*	轨道编号（卫星运行轨道）	12345*/
	int						orbit_number;
	/*	相对轨道号（按周期循环）	89*/
	int						relative_orbit_number;
	/*	卫星平台名称	"Sentinel-2"*/
	QString					platform_name;
	/*	卫星编号（如 Sentinel-2A）	"S2A"*/
	QString					platform_identifier;
	/*  传感器工作模式（如 IW/SM）	"IW"*/
	QString					sensor_operational_mode;
	/*	产品类型（如 SLC, GRD, L1C, L2A）	"L2A"*/
	QString					product_type;
	/* 	云量百分比（Sentinel-2 专属）	3.42*/
	float					cloud_cover_percentage;
	/* 	数据覆盖范围的多边形（WKT 格式）	"POLYGON ((...))"*/
	QString					foot_print; // WKT 多边形
	/* 	下载链接（需要认证）	https://scihub...*/
	QString					download_url;
};

typedef std::vector<AgriSentinelMetadata>				AgriSentinelMetadataVec;
typedef	std::shared_ptr<AgriSentinelMetadata>			AgriSentinelMetadataPtr;
typedef std::vector<AgriSentinelMetadataPtr>			AgriSentinelMetadataPtrVec;

Q_DECLARE_METATYPE(AgriSentinelMetadata)

#endif  // AgriMultispectralSensorDecl_H