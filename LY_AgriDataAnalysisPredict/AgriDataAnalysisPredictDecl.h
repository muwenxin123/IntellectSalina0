#ifndef AGRIDATAANALYSISPREDICTDECL_H_
#define AGRIDATAANALYSISPREDICTDECL_H_
 
#include <QString>
#include <QVariant>

#include "DataManage/AgriFarm/AgriFarmManagerDecl.h"

#define AGRI_PEST_DATASET_TABLE_NAME				"agri_pest_dataset"
#define AGRI_PEST_COLLECTION_TABLE_NAME				"agri_pest_collection"
#define AGRI_RELATION_FIELD_SENSOR_TABLE_NAME		"agri_relation_field_sensor"
#define AGRI_PEST_PHOTO_INDEFICATION_TABLE_NAME		"agri_pest_photo_indefication"
#define AGRI_PEST_DISTRIBUTE_MAP_TABLE_NAME			"agri_pest_distribute_map"

#define AGRI_MONITOR_PHOTO							3
#define GRID_PIXEL_SPACE_MIN						(1.0 / 110000.0)


/* 作物类型 */
enum class CropType
{
	CROP_TYPE_NONE = 0,				// 0 - 全部
	CROP_TYPE_1,					// 1 - 玉米
	CROP_TYPE_2,					// 2 - 小麦
	CROP_TYPE_3,					// 3 - 水稻
};

Q_DECLARE_METATYPE(CropType)

/* 数据操作方式 */
enum class DataSetHandleMode
{
	DATA_SET_HANDLE_VIEW,			// 查看
	DATA_SET_HANDLE_ADD,			// 添加
	DATA_SET_HANDLE_UPDATE,			// 修改
	DATA_SET_HANDLE_DELETE 			// 删除
};

Q_DECLARE_METATYPE(DataSetHandleMode)

/* 数据采集方式 */
enum class DataCollectType
{
	DATA_COLLECT_UNKNOW,			// 未知
	DATA_COLLECT_ARTIFICIAL,		// 人工现场调查
	DATA_COLLECT_SENSOR,			// 传感器图像识别
	DATA_COLLECT_IMAGE,				// 图片文件识别
	DATA_COLLECT_IMPORT,			// 数据文件导入
};

Q_DECLARE_METATYPE(DataCollectType)

/* 采集点 */
enum class DataCollectHandle
{
	DATA_COLLECT_ADD,				// 添加
	DATA_COLLECT_UPDATE,			// 修改
	DATA_COLLECT_DELETE 			// 删除
};

/* 病虫害 */
enum class PestDisease 
{
	PEST_DISEASE_NONE = 0,			// 0 - 全部
	PEST_DISEASE_1,					// 1 - 虫害
	PEST_DISEASE_2,					// 2 - 病害
	PEST_DISEASE_3,					// 3 - 病虫害
};

Q_DECLARE_METATYPE(PestDisease)

/* 虫害等级 */
enum class PestLevel 
{
	PEST_LEVEL_NONE = 0,			// 0 - 无害
	PEST_LEVEL_1,				// 1 - 轻微
	PEST_LEVEL_2,				// 2 - 轻度
	PEST_LEVEL_3,				// 3 - 中度
	PEST_LEVEL_4,				// 4 - 较严重
	PEST_LEVEL_5,				// 5 - 严重
	PEST_LEVEL_6,				// 6 - 非常严重
	PEST_LEVEL_7,				// 7 - 极严重
	PEST_LEVEL_8,				// 8 - 危险
	PEST_LEVEL_9,				// 9 - 极危险
	PEST_LEVEL_10				// 10 - 灾难
};

Q_DECLARE_METATYPE(PestLevel)

/* 虫害类型 */
enum class PestCategory 
{
	PEST_CATEGORY_NONE = 0,			// 0 - 全部
	PEST_CATEGORY_1,				// 1 - 玉米黏虫
	PEST_CATEGORY_2,				// 2 - 红蜘蛛
	PEST_CATEGORY_3,				// 3 - 穗蚜
	PEST_CATEGORY_4,				// 4 - 吸浆虫
	PEST_CATEGORY_5,				// 5 - 蚜虫
	PEST_CATEGORY_6,				// 6 - 棉铃虫
	PEST_CATEGORY_7,				// 7 - 甜菜夜蛾
	PEST_CATEGORY_8,				// 8 - 斜纹夜蛾
	PEST_CATEGORY_9,				// 9 - 玉米螟
	PEST_CATEGORY_10,				// 10 - 粘虫
	PEST_CATEGORY_11,				// 11 - 草地贪夜蛾
	PEST_CATEGORY_12,				// 12 - 二点委夜蛾
	PEST_CATEGORY_13,				// 13 - 蓟马
	PEST_CATEGORY_14,				// 14 - 地老虎
	PEST_CATEGORY_15,				// 15 - 玉米向日葵粉蚧
	PEST_CATEGORY_16,				// 16 - 金龟子
	PEST_CATEGORY_17,				// 17 - 东亚飞蝗
	PEST_CATEGORY_18,				// 18 - 中华蚱蜢
	PEST_CATEGORY_19,				// 19 - 稻绿蝽
};

Q_DECLARE_METATYPE(PestCategory)

/* 虫害阶段 */
enum class PestStage 
{
	PEST_STAGE_NONE = 0,			// 0 - 未知
	PEST_STAGE_1,					// 1 - 卵
	PEST_STAGE_2,					// 2 - 幼虫
	PEST_STAGE_3,					// 3 - 成虫
};

Q_DECLARE_METATYPE(PestStage)

/* 栅格大小 */
enum class GridSize
{
	GRID_SIZE_1 = 1,				// 1 - 1米
	GRID_SIZE_2,					// 2 - 2米
	GRID_SIZE_3,					// 3 - 3米
	GRID_SIZE_4,					// 4 - 4米
	GRID_SIZE_5,					// 5 - 5米
};

Q_DECLARE_METATYPE(GridSize)

/* 扩散层数 */
enum class DiffusionLayer
{
	DIFFUSION_LAYER_1 = 1,			// 1 - 1层
	DIFFUSION_LAYER_2,				// 2 - 2层
	DIFFUSION_LAYER_3,				// 5 - 5层
	DIFFUSION_LAYER_4,				// 10 - 10层
};

Q_DECLARE_METATYPE(DiffusionLayer)

/* 生成方式 */
enum class GenerateType
{
	GENERATE_EQUAL = 1,				// 1 - 等距扩散
	GENERATE_DYNAMIC,				// 2 - 动态蔓延
};

Q_DECLARE_METATYPE(GenerateType)

/* 风向 */
enum class WindDirection
{
	SOUTH,
	NORTH,
	WEST,
	EAST,
	WEST_SOUTH,
	EAST_SOUTH,
	WEST_NORTH,
	EAST_NORTH
};

Q_DECLARE_METATYPE(WindDirection)

/* 虫害数据集 */
class AgriPestDataset
{
public:
	AgriPestDataset()
		: guid(""),
		name(""),
		field_id(""),
		crop_type(0),
		collect_type(0),
		begin_time(""),
		end_time(""),
		pest_disease(0),
		pest_category(0)
	{};
	~AgriPestDataset() {};

	QString					guid;
	QString					name;
	QString					field_id;
	int						crop_type;
	int						collect_type;
	QString					begin_time;
	QString					end_time;
	int						pest_disease;
	int						pest_category;
};

typedef std::vector<AgriPestDataset>				AgriPestDatasetVec;
typedef	std::shared_ptr<AgriPestDataset>			AgriPestDatasetPtr;
typedef std::vector<AgriPestDatasetPtr>				AgriPestDatasetPtrVec;

Q_DECLARE_METATYPE(AgriPestDataset)


/* 虫害采集点 */
class AgriPestCollection
{
public:
	AgriPestCollection()
		: guid(""),
		name(""),
		dataset_id(""),
		sensor_id(""),
		collect_type(0),
		collect_time(""),
		lon(0.0),
		lat(0.0),
		alt(0.0),
		category(0),
		stage(0),
		count(0),
		range(0.0),
		level(0)
	{};
	~AgriPestCollection() {};

	QString					guid;
	QString					name;
	QString					dataset_id;
	QString					sensor_id;
	int						collect_type;
	QString					collect_time;
	double					lon;
	double					lat;
	double					alt;
	int						category;
	int						stage;
	int						count;
	double					range;
	int						level;
	QByteArray				recognize_image;
};

typedef std::vector<AgriPestCollection>				AgriPestCollectionVec;
typedef	std::shared_ptr<AgriPestCollection>			AgriPestCollectionPtr;
typedef std::vector<AgriPestCollectionPtr>			AgriPestCollectionPtrVec;

Q_DECLARE_METATYPE(AgriPestCollection)


/* 摄像头识别信息 */
class AgriPestPhotoIndefication
{
public:
	AgriPestPhotoIndefication()
		: id(0),
		photo_id(""),
		mointId(),
		recode_time(0),
		localHeight(0.0),
		longitude(0.0),
		latitude(0.0),
		pest_type(""),
		pest_stage(""),
		pest_number(0),
		recode_range(0),
		pest_grade("")
	{};
	~AgriPestPhotoIndefication() {};

	int						id;
	QString					photo_id;
	int						mointId;
	int						recode_time;
	double					localHeight;
	double					longitude;
	double					latitude;
	QString					pest_type;
	QString					pest_stage;
	int						pest_number;
	int						recode_range;
	QString					pest_grade;
	QByteArray				original_image;
	QByteArray				recognize_image;
};

typedef std::vector<AgriPestPhotoIndefication>				AgriPestPhotoIndeficationVec;
typedef	std::shared_ptr<AgriPestPhotoIndefication>			AgriPestPhotoIndeficationPtr;
typedef std::vector<AgriPestPhotoIndeficationPtr>			AgriPestPhotoIndeficationPtrVec;

Q_DECLARE_METATYPE(AgriPestPhotoIndefication)


/* 虫害分布图 */
class AgriPestDistributeMap
{
public:
	AgriPestDistributeMap()
		: guid(""),
		name(""),
		dataset_id(""),
		crop_type(0),
		growth_stage(0),
		field_area(0.0),
		collect_count(0),
		grid_unit(0),
		diffuse_layer(0),
		distribute_statistic("")
	{};
	~AgriPestDistributeMap() {};

	QString					guid;
	QString					name;
	QString					dataset_id;
	int						crop_type;
	int						growth_stage;
	double					field_area;
	int						collect_count;
	int						grid_unit;
	int						diffuse_layer;
	QString					distribute_statistic;
};

typedef std::vector<AgriPestDistributeMap>				AgriPestDistributeMapVec;
typedef	std::shared_ptr<AgriPestDistributeMap>			AgriPestDistributeMapPtr;
typedef std::vector<AgriPestDistributeMapPtr>			AgriPestDistributeMapPtrVec;

Q_DECLARE_METATYPE(AgriPestDistributeMap)


/****************************************************************************************************************************/

/* 气象数据 */
class AgriMetData
{
public:
	AgriMetData()
		: site(""),
		year(0),
		day(0),
		radn(0.0),
		maxt(0.0),
		mint(0.0),
		rain(0.0),
		evap(0.0)
	{};
	~AgriMetData() {};

	std::string				site;
	int						year;
	int						day;
	double					radn;
	double					maxt;
	double					mint;
	double					rain;
	double					evap;
};

typedef std::vector<AgriMetData>            AgriMetDataVec;
typedef AgriMetDataVec::iterator            AgriMetDataVecItr;
typedef std::vector<AgriMetData *>          AgriMetDataPtrVec;
typedef AgriMetDataPtrVec::iterator         AgriMetDataVecPtrVecItr;

Q_DECLARE_METATYPE(AgriMetData)

/* 输出数据 */
class AgriOutputData
{
public:
	AgriOutputData()
		: date(""),
		biomass(0.0),
		yield(0.0),
		grain_protein(0.0),
		grain_size(0.0),
		esw(0.0),
		lai(0.0),
		stage(0.0)
	{};
	~AgriOutputData() {};

	std::string				date;			// 日期，格式为日 / 月 / 年（dd / mm / yyyy）
	double					biomass;		// 生物量（kg / ha）
	double					yield;			// 产量（kg / ha）
	double					grain_protein;	// 谷物蛋白质含量（%）
	double					grain_size;		// 谷物粒径（g）
	double					esw;			// 土壤可提取水分（mm）
	double					lai;			// 叶面积指数（m ^ 2 / m ^ 2）
	double					stage;			// 作物生长阶段，通常为数值编码
};

typedef std::vector<AgriOutputData>            AgriOutputDataVec;
typedef AgriOutputDataVec::iterator            AgriOutputDataVecItr;
typedef std::vector<AgriOutputData *>          AgriOutputDataPtrVec;
typedef AgriOutputDataPtrVec::iterator         AgriOutputDataVecPtrVecItr;

Q_DECLARE_METATYPE(AgriOutputData)
#endif  // AgriDataAnalysisPredictDecl_H