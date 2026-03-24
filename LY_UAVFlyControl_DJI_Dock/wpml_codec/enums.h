#pragma once

#ifndef WPML_CODEC_ENUMS_H
#define WPML_CODEC_ENUMS_H

#include <string>
#include <unordered_map>
#include <stdexcept>
#include <vector>

//#include <magic_enum/magic_enum.hpp>

namespace wpml_codec
{	/*
	namespace enums
	{
		enum class ActionActuatorFunc
		{
			takePhoto,          ///< 单拍
			startRecord,        ///< 开始录像
			stopRecord,         ///< 结束录像
			focus,              ///< 对焦
			zoom,               ///< 变焦
			customDirName,      ///< 创建新文件夹
			gimbalRotate,       ///< 旋转云台
			rotateYaw,          ///< 飞行器偏航
			hover,              ///< 悬停等待
			gimbalEvenlyRotate, ///< 航段间均匀转动云台pitch角
			accurateShoot,      ///< 精准复拍动作（已暂停维护，建议使用orientedShoot）
			orientedShoot,      ///< 定向拍照动作
			panoShot,           ///< 全景拍照动作
			recordPointCloud    ///< 点云录制操作
		};

		enum class ActionGroupMode
		{
			sequence, ///< 串行执行
		};

		enum class ActionTriggerType
		{
			reachPoint,            ///< 到达航点时执行
			betweenAdjacentPoints, ///< 航段触发，均匀转云台
			multipleTiming,        ///< 等时触发
			multipleDistance,      ///< 等距触发
		};

		enum class AircraftPathMode
		{
			clockwise,        ///< 顺时针旋转
			counterClockwise, ///< 逆时针旋转
		};

		enum class CoordinateMode
		{
			WGS84, ///< WGS84坐标系，当前固定使用
		};

		enum class ExecuteHeightMode
		{
			WGS84,                 ///< 椭球高模式
			relativeToStartPoint,  ///< 相对起飞点高度模式
			realTimeFollowSurface, ///< 使用实时仿地模式
		};

		enum class ExecuteRCLostAction
		{
			goBack,  ///< 返航
			landing, ///< 降落
			hover,   ///< 悬停
		};

		enum class ExitOnRCLost
		{
			goContinue,        ///< 继续执行航线
			executeLostAction, ///< 退出航线，执行失控动作
		};

		enum class FinishAction
		{
			goHome,            ///< 退出航线模式并返航
			noAction,          ///< 退出航线模式
			autoLand,          ///< 退出航线模式并原地降落
			gotoFirstWaypoint, ///< 立即飞向航线起始点，到达后退出航线模式
		};

		enum class FlyToWaylineMode
		{
			safely,       ///< 安全模式
			pointToPoint, ///< 倾斜飞行模式
		};

		enum class FocusMode
		{
			firstPoint, ///< 首个航点自动对焦
			custom,     ///< 标定对焦值对焦
		};

		enum class GimbalHeadingYawBase
		{
			north, ///< 相对地理北
		};

		enum class GimbalPitchMode
		{
			manual,          ///< 手动控制
			usePointSetting, ///< 依照每个航点设置（仅航点飞行）
			fixed,           ///< 固定为用户设置的俯仰角（仅建图航拍）
		};

		enum class GimbalRotateMode
		{
			absoluteAngle, ///< 绝对角度，相对于正北方的角度
		};

		enum class HeightMode
		{
			EGM96,                 ///< 使用海拔高编辑
			relativeToStartPoint,  ///< 使用相对点的高度进行编辑
			aboveGroundLevel,      ///< 使用地形数据，AGL下编辑
			realTimeFollowSurface, ///< 使用实时仿地模式
		};

		enum class ImageFormat
		{
			wide,        ///< 存储广角镜头照片
			zoom,        ///< 存储变焦镜头照片
			ir,          ///< 存储红外镜头照片
			narrow_band, ///< 存储窄带镜头拍摄照片
			visable,     ///< 可见光照片
		};

		enum class MappingHeadingMode
		{
			fixed,         ///< 固定为用户设置的偏航角
			followWayline, ///< 偏航角跟随航线
		};

		enum class MeteringMode
		{
			average, ///< 全局测光
			spot,    ///< 点测光
		};

		enum class OrientedPhotoMode
		{
			normalPhoto,           ///< 普通拍照
			lowLightSmartShooting, ///< 智能低光拍照
		};

		enum class PanoShotSubMode
		{
			panoShot_360, ///< 全景模式
		};

		enum class PositioningType
		{
			GPS,            ///< 位置数据采集来源为GPS/BDS/GLONASS/GALILEO等
			RTKBaseStation, ///< 采集位置数据时，使用RTK基站进行差分定位
			QianXun,        ///< 采集位置数据时，使用千寻网络RTK进行差分定位
			Custom,         ///< 采集位置数据时，使用自定义网络RTK进行差分定位
		};

		enum class RecordPointCloudOperate
		{
			startRecord,  ///< 开始点云录制
			pauseRecord,  ///< 暂停点云录制
			resumeRecord, ///< 继续点云录制
			stopRecord,   ///< 结束点云录制
		};

		enum class ReturnMode
		{
			singleReturnStrongest, ///< 单回波
			dualReturn,            ///< 双回波
			tripleReturn,          ///< 三回波
		};

		enum class ScanningMode
		{
			repetitive,    ///< 重复扫描
			nonRepetitive, ///< 非重复扫描
		};

		enum class ShootType
		{
			time,     ///< 等时间拍照
			distance, ///< 等间隔拍照
		};

		enum class TemplateType
		{
			waypoint,     ///< 航点飞行
			mapping2d,    ///< 建图航拍
			mapping3d,    ///< 倾斜摄影
			mappingStrip, ///< 航带飞行
		};

		enum class WaypointHeadingMode
		{
			followWayline,    ///< 沿航线方向
			manually,         ///< 手动控制
			fixed,            ///< 锁定当前偏航角
			smoothTransition, ///< 自定义
			towardPOI,        ///< 朝向兴趣点
		};

		enum class WaypointHeadingPathMode
		{
			clockwise,        ///< 顺时针旋转飞行器偏航角
			counterClockwise, ///< 逆时针旋转飞行器偏航角
			followBadArc,     ///< 沿最短路径旋转飞行器偏航角
		};

		enum class WaypointTurnMode
		{
			coordinateTurn,                           ///< 协调转弯，不过点，提前转弯
			toPointAndStopWithDiscontinuityCurvature, ///< 直线飞行，飞行器到点停
			toPointAndStopWithContinuityCurvature,    ///< 曲线飞行，飞行器到点停
			toPointAndPassWithContinuityCurvature,    ///< 曲线飞行，飞行器过点不停
		};
	}
	*/

	namespace enums
	{
#define ACTION_ACTUATOR_FUNC_ENUM(XX) \
    XX(takePhoto, "takePhoto") \
    XX(startRecord, "startRecord") \
    XX(stopRecord, "stopRecord") \
    XX(focus, "focus") \
    XX(zoom, "zoom") \
    XX(customDirName, "customDirName") \
    XX(gimbalRotate, "gimbalRotate") \
    XX(rotateYaw, "rotateYaw") \
    XX(hover, "hover") \
    XX(gimbalEvenlyRotate, "gimbalEvenlyRotate") \
    XX(accurateShoot, "accurateShoot") \
    XX(orientedShoot, "orientedShoot") \
    XX(panoShot, "panoShot") \
    XX(recordPointCloud, "recordPointCloud")

#define ACTION_GROUP_MODE_ENUM(XX) \
    XX(sequence, "sequence")

#define ACTION_TRIGGER_TYPE_ENUM(XX) \
    XX(reachPoint, "reachPoint") \
    XX(betweenAdjacentPoints, "betweenAdjacentPoints") \
    XX(multipleTiming, "multipleTiming") \
    XX(multipleDistance, "multipleDistance")

#define AIRCRAFT_PATH_MODE_ENUM(XX) \
    XX(clockwise, "clockwise") \
    XX(counterClockwise, "counterClockwise")

#define COORDINATE_MODE_ENUM(XX) \
    XX(WGS84, "WGS84")

#define EXECUTE_HEIGHT_MODE_ENUM(XX) \
    XX(WGS84, "WGS84") \
    XX(relativeToStartPoint, "relativeToStartPoint") \
    XX(realTimeFollowSurface, "realTimeFollowSurface")

#define EXECUTE_RC_LOST_ACTION_ENUM(XX) \
    XX(goBack, "goBack") \
    XX(landing, "landing") \
    XX(hover, "hover")

#define EXIT_ON_RC_LOST_ENUM(XX) \
    XX(goContinue, "goContinue") \
    XX(executeLostAction, "executeLostAction")

#define FINISH_ACTION_ENUM(XX) \
    XX(goHome, "goHome") \
    XX(noAction, "noAction") \
    XX(autoLand, "autoLand") \
    XX(gotoFirstWaypoint, "gotoFirstWaypoint")

#define FLY_TO_WAYLINE_MODE_ENUM(XX) \
    XX(safely, "safely") \
    XX(pointToPoint, "pointToPoint")

#define FOCUS_MODE_ENUM(XX) \
    XX(firstPoint, "firstPoint") \
    XX(custom, "custom")

#define GIMBAL_HEADING_YAW_BASE_ENUM(XX) \
    XX(north, "north")

#define GIMBAL_PITCH_MODE_ENUM(XX) \
    XX(manual, "manual") \
    XX(usePointSetting, "usePointSetting") \
    XX(fixed, "fixed")

#define GIMBAL_ROTATE_MODE_ENUM(XX) \
    XX(absoluteAngle, "absoluteAngle")

#define HEIGHT_MODE_ENUM(XX) \
    XX(EGM96, "EGM96") \
    XX(relativeToStartPoint, "relativeToStartPoint") \
    XX(aboveGroundLevel, "aboveGroundLevel") \
    XX(realTimeFollowSurface, "realTimeFollowSurface")

#define IMAGE_FORMAT_ENUM(XX) \
    XX(wide, "wide") \
    XX(zoom, "zoom") \
    XX(ir, "ir") \
    XX(narrow_band, "narrow_band") \
    XX(visable, "visable")

#define MAPPING_HEADING_MODE_ENUM(XX) \
    XX(fixed, "fixed") \
    XX(followWayline, "followWayline")

#define METERING_MODE_ENUM(XX) \
    XX(average, "average") \
    XX(spot, "spot")

#define ORIENTED_PHOTO_MODE_ENUM(XX) \
    XX(normalPhoto, "normalPhoto") \
    XX(lowLightSmartShooting, "lowLightSmartShooting")

#define PANO_SHOT_SUB_MODE_ENUM(XX) \
    XX(panoShot_360, "panoShot_360")

#define POSITIONING_TYPE_ENUM(XX) \
    XX(GPS, "GPS") \
    XX(RTKBaseStation, "RTKBaseStation") \
    XX(QianXun, "QianXun") \
    XX(Custom, "Custom")

#define RECORD_POINT_CLOUD_OPERATE_ENUM(XX) \
    XX(startRecord, "startRecord") \
    XX(pauseRecord, "pauseRecord") \
    XX(resumeRecord, "resumeRecord") \
    XX(stopRecord, "stopRecord")

#define RETURN_MODE_ENUM(XX) \
    XX(singleReturnStrongest, "singleReturnStrongest") \
    XX(dualReturn, "dualReturn") \
    XX(tripleReturn, "tripleReturn")

#define SCANNING_MODE_ENUM(XX) \
    XX(repetitive, "repetitive") \
    XX(nonRepetitive, "nonRepetitive")

#define SHOOT_TYPE_ENUM(XX) \
    XX(time, "time") \
    XX(distance, "distance")

#define TEMPLATE_TYPE_ENUM(XX) \
    XX(waypoint, "waypoint") \
    XX(mapping2d, "mapping2d") \
    XX(mapping3d, "mapping3d") \
    XX(mappingStrip, "mappingStrip")

#define WAYPOINT_HEADING_MODE_ENUM(XX) \
    XX(followWayline, "followWayline") \
    XX(manually, "manually") \
    XX(fixed, "fixed") \
    XX(smoothTransition, "smoothTransition") \
    XX(towardPOI, "towardPOI")

#define WAYPOINT_HEADING_PATH_MODE_ENUM(XX) \
    XX(clockwise, "clockwise") \
    XX(counterClockwise, "counterClockwise") \
    XX(followBadArc, "followBadArc")

#define WAYPOINT_TURN_MODE_ENUM(XX) \
    XX(coordinateTurn, "coordinateTurn") \
    XX(toPointAndStopWithDiscontinuityCurvature, "toPointAndStopWithDiscontinuityCurvature") \
    XX(toPointAndStopWithContinuityCurvature, "toPointAndStopWithContinuityCurvature") \
    XX(toPointAndPassWithContinuityCurvature, "toPointAndPassWithContinuityCurvature")

		// ===================== 枚举声明 =====================

		enum class ActionActuatorFunc {
#define DECL_ENUM_VALUE(name, str) name,
			ACTION_ACTUATOR_FUNC_ENUM(DECL_ENUM_VALUE)
#undef DECL_ENUM_VALUE
		};

		const std::unordered_map<std::string, ActionActuatorFunc> kStringToActionActuatorFunc = {
#define DECL_ENUM_VALUE(name, str) {str, ActionActuatorFunc::name},
			ACTION_ACTUATOR_FUNC_ENUM(DECL_ENUM_VALUE)
#undef DECL_ENUM_VALUE
		};

		enum class ActionGroupMode {
#define DECL_ENUM_VALUE(name, str) name,
			ACTION_GROUP_MODE_ENUM(DECL_ENUM_VALUE)
#undef DECL_ENUM_VALUE
		};

		enum class ActionTriggerType {
#define DECL_ENUM_VALUE(name, str) name,
			ACTION_TRIGGER_TYPE_ENUM(DECL_ENUM_VALUE)
#undef DECL_ENUM_VALUE
		};

		const std::unordered_map<std::string, ActionTriggerType> kStringToAActionTriggerType = {
#define DECL_ENUM_VALUE(name, str) {str, ActionTriggerType::name},
			ACTION_TRIGGER_TYPE_ENUM(DECL_ENUM_VALUE)
#undef DECL_ENUM_VALUE
		};

		enum class AircraftPathMode {
#define DECL_ENUM_VALUE(name, str) name,
			AIRCRAFT_PATH_MODE_ENUM(DECL_ENUM_VALUE)
#undef DECL_ENUM_VALUE
		};

		enum class CoordinateMode {
#define DECL_ENUM_VALUE(name, str) name,
			COORDINATE_MODE_ENUM(DECL_ENUM_VALUE)
#undef DECL_ENUM_VALUE
		};

		enum class ExecuteHeightMode {
#define DECL_ENUM_VALUE(name, str) name,
			EXECUTE_HEIGHT_MODE_ENUM(DECL_ENUM_VALUE)
#undef DECL_ENUM_VALUE
		};

		enum class ExecuteRCLostAction {
#define DECL_ENUM_VALUE(name, str) name,
			EXECUTE_RC_LOST_ACTION_ENUM(DECL_ENUM_VALUE)
#undef DECL_ENUM_VALUE
		};

		enum class ExitOnRCLost {
#define DECL_ENUM_VALUE(name, str) name,
			EXIT_ON_RC_LOST_ENUM(DECL_ENUM_VALUE)
#undef DECL_ENUM_VALUE
		};

		enum class FinishAction {
#define DECL_ENUM_VALUE(name, str) name,
			FINISH_ACTION_ENUM(DECL_ENUM_VALUE)
#undef DECL_ENUM_VALUE
		};

		enum class FlyToWaylineMode {
#define DECL_ENUM_VALUE(name, str) name,
			FLY_TO_WAYLINE_MODE_ENUM(DECL_ENUM_VALUE)
#undef DECL_ENUM_VALUE
		};

		enum class FocusMode {
#define DECL_ENUM_VALUE(name, str) name,
			FOCUS_MODE_ENUM(DECL_ENUM_VALUE)
#undef DECL_ENUM_VALUE
		};

		enum class GimbalHeadingYawBase {
#define DECL_ENUM_VALUE(name, str) name,
			GIMBAL_HEADING_YAW_BASE_ENUM(DECL_ENUM_VALUE)
#undef DECL_ENUM_VALUE
		};

		enum class GimbalPitchMode {
#define DECL_ENUM_VALUE(name, str) name,
			GIMBAL_PITCH_MODE_ENUM(DECL_ENUM_VALUE)
#undef DECL_ENUM_VALUE
		};

		enum class GimbalRotateMode {
#define DECL_ENUM_VALUE(name, str) name,
			GIMBAL_ROTATE_MODE_ENUM(DECL_ENUM_VALUE)
#undef DECL_ENUM_VALUE
		};

		enum class HeightMode {
#define DECL_ENUM_VALUE(name, str) name,
			HEIGHT_MODE_ENUM(DECL_ENUM_VALUE)
#undef DECL_ENUM_VALUE
		};

		enum class ImageFormat {
#define DECL_ENUM_VALUE(name, str) name,
			IMAGE_FORMAT_ENUM(DECL_ENUM_VALUE)
#undef DECL_ENUM_VALUE
		};

		enum class MappingHeadingMode {
#define DECL_ENUM_VALUE(name, str) name,
			MAPPING_HEADING_MODE_ENUM(DECL_ENUM_VALUE)
#undef DECL_ENUM_VALUE
		};

		enum class MeteringMode {
#define DECL_ENUM_VALUE(name, str) name,
			METERING_MODE_ENUM(DECL_ENUM_VALUE)
#undef DECL_ENUM_VALUE
		};

		enum class OrientedPhotoMode {
#define DECL_ENUM_VALUE(name, str) name,
			ORIENTED_PHOTO_MODE_ENUM(DECL_ENUM_VALUE)
#undef DECL_ENUM_VALUE
		};

		enum class PanoShotSubMode {
#define DECL_ENUM_VALUE(name, str) name,
			PANO_SHOT_SUB_MODE_ENUM(DECL_ENUM_VALUE)
#undef DECL_ENUM_VALUE
		};

		enum class PositioningType {
#define DECL_ENUM_VALUE(name, str) name,
			POSITIONING_TYPE_ENUM(DECL_ENUM_VALUE)
#undef DECL_ENUM_VALUE
		};

		enum class RecordPointCloudOperate {
#define DECL_ENUM_VALUE(name, str) name,
			RECORD_POINT_CLOUD_OPERATE_ENUM(DECL_ENUM_VALUE)
#undef DECL_ENUM_VALUE
		};

		enum class ReturnMode {
#define DECL_ENUM_VALUE(name, str) name,
			RETURN_MODE_ENUM(DECL_ENUM_VALUE)
#undef DECL_ENUM_VALUE
		};

		enum class ScanningMode {
#define DECL_ENUM_VALUE(name, str) name,
			SCANNING_MODE_ENUM(DECL_ENUM_VALUE)
#undef DECL_ENUM_VALUE
		};

		enum class ShootType {
#define DECL_ENUM_VALUE(name, str) name,
			SHOOT_TYPE_ENUM(DECL_ENUM_VALUE)
#undef DECL_ENUM_VALUE
		};

		enum class TemplateType {
#define DECL_ENUM_VALUE(name, str) name,
			TEMPLATE_TYPE_ENUM(DECL_ENUM_VALUE)
#undef DECL_ENUM_VALUE
		};

		enum class WaypointHeadingMode {
#define DECL_ENUM_VALUE(name, str) name,
			WAYPOINT_HEADING_MODE_ENUM(DECL_ENUM_VALUE)
#undef DECL_ENUM_VALUE
		};

		enum class WaypointHeadingPathMode {
#define DECL_ENUM_VALUE(name, str) name,
			WAYPOINT_HEADING_PATH_MODE_ENUM(DECL_ENUM_VALUE)
#undef DECL_ENUM_VALUE
		};

		enum class WaypointTurnMode {
#define DECL_ENUM_VALUE(name, str) name,
			WAYPOINT_TURN_MODE_ENUM(DECL_ENUM_VALUE)
#undef DECL_ENUM_VALUE
		};
	}


	// ===================== 转换工具类 =====================
	class EnumConverter {
	public:
		// 字符串 → 枚举
		template <typename EnumType>
		static bool fromString(const std::string& str, EnumType& out) {
			const auto& map = getStringToEnumMap<EnumType>();
			auto it = map.find(str);
			if (it != map.end()) {
				out = it->second;
				return true;
			}
			return false;
		}

		// 枚举 → 字符串
		template <typename EnumType>
		static std::string toString(EnumType value) {
			const auto& map = getEnumToStringMap<EnumType>();
			auto it = map.find(value);
			if (it != map.end()) {
				return it->second;
			}
			throw std::invalid_argument("Invalid enum value");
		}

	private:
		// 映射表获取函数（需特化）
		template <typename EnumType>
		static const std::unordered_map<std::string, EnumType>& getStringToEnumMap();

		template <typename EnumType>
		static const std::unordered_map<EnumType, std::string>& getEnumToStringMap();
	};



		// ===================== 映射表特化（完整实现） =====================

		// ActionActuatorFunc
		template <>
		const std::unordered_map<std::string, wpml_codec::enums::ActionActuatorFunc>&
			EnumConverter::getStringToEnumMap<wpml_codec::enums::ActionActuatorFunc>();

		template <>
		const std::unordered_map<wpml_codec::enums::ActionActuatorFunc, std::string>&
			EnumConverter::getEnumToStringMap<wpml_codec::enums::ActionActuatorFunc>();

		// ActionGroupMode
		template <>
		const std::unordered_map<std::string, wpml_codec::enums::ActionGroupMode>&
			EnumConverter::getStringToEnumMap<wpml_codec::enums::ActionGroupMode>();

		template <>
		const std::unordered_map<wpml_codec::enums::ActionGroupMode, std::string>&
			EnumConverter::getEnumToStringMap<wpml_codec::enums::ActionGroupMode>();

		// ActionTriggerType
		template <>
		const std::unordered_map<std::string, wpml_codec::enums::ActionTriggerType>&
			EnumConverter::getStringToEnumMap<wpml_codec::enums::ActionTriggerType>();

		template <>
		const std::unordered_map<wpml_codec::enums::ActionTriggerType, std::string>&
			EnumConverter::getEnumToStringMap<wpml_codec::enums::ActionTriggerType>();

		// AircraftPathMode
		template <>
		const std::unordered_map<std::string, wpml_codec::enums::AircraftPathMode>&
			EnumConverter::getStringToEnumMap<wpml_codec::enums::AircraftPathMode>();

		template <>
		const std::unordered_map<wpml_codec::enums::AircraftPathMode, std::string>&
			EnumConverter::getEnumToStringMap<wpml_codec::enums::AircraftPathMode>();

		// CoordinateMode
		template <>
		const std::unordered_map<std::string, wpml_codec::enums::CoordinateMode>&
			EnumConverter::getStringToEnumMap<wpml_codec::enums::CoordinateMode>();

		template <>
		const std::unordered_map<wpml_codec::enums::CoordinateMode, std::string>&
			EnumConverter::getEnumToStringMap<wpml_codec::enums::CoordinateMode>();

		// ExecuteHeightMode
		template <>
		const std::unordered_map<std::string, wpml_codec::enums::ExecuteHeightMode>&
			EnumConverter::getStringToEnumMap<wpml_codec::enums::ExecuteHeightMode>();

		template <>
		const std::unordered_map<wpml_codec::enums::ExecuteHeightMode, std::string>&
			EnumConverter::getEnumToStringMap<wpml_codec::enums::ExecuteHeightMode>();

		// ExecuteRCLostAction
		template <>
		const std::unordered_map<std::string, wpml_codec::enums::ExecuteRCLostAction>&
			EnumConverter::getStringToEnumMap<wpml_codec::enums::ExecuteRCLostAction>();

		template <>
		const std::unordered_map<wpml_codec::enums::ExecuteRCLostAction, std::string>&
			EnumConverter::getEnumToStringMap<wpml_codec::enums::ExecuteRCLostAction>();

		// ExitOnRCLost
		template <>
		const std::unordered_map<std::string, wpml_codec::enums::ExitOnRCLost>&
			EnumConverter::getStringToEnumMap<wpml_codec::enums::ExitOnRCLost>();

		template <>
		const std::unordered_map<wpml_codec::enums::ExitOnRCLost, std::string>&
			EnumConverter::getEnumToStringMap<wpml_codec::enums::ExitOnRCLost>();

		// FinishAction
		template <>
		const std::unordered_map<std::string, wpml_codec::enums::FinishAction>&
			EnumConverter::getStringToEnumMap<wpml_codec::enums::FinishAction>();

		template <>
		const std::unordered_map<wpml_codec::enums::FinishAction, std::string>&
			EnumConverter::getEnumToStringMap<wpml_codec::enums::FinishAction>();

		// FlyToWaylineMode
		template <>
		const std::unordered_map<std::string, wpml_codec::enums::FlyToWaylineMode>&
			EnumConverter::getStringToEnumMap<wpml_codec::enums::FlyToWaylineMode>();

		template <>
		const std::unordered_map<wpml_codec::enums::FlyToWaylineMode, std::string>&
			EnumConverter::getEnumToStringMap<wpml_codec::enums::FlyToWaylineMode>();

		// FocusMode
		template <>
		const std::unordered_map<std::string, wpml_codec::enums::FocusMode>&
			EnumConverter::getStringToEnumMap<wpml_codec::enums::FocusMode>();

		template <>
		const std::unordered_map<wpml_codec::enums::FocusMode, std::string>&
			EnumConverter::getEnumToStringMap<wpml_codec::enums::FocusMode>();

		// GimbalHeadingYawBase
		template <>
		const std::unordered_map<std::string, wpml_codec::enums::GimbalHeadingYawBase>&
			EnumConverter::getStringToEnumMap<wpml_codec::enums::GimbalHeadingYawBase>();

		template <>
		const std::unordered_map<wpml_codec::enums::GimbalHeadingYawBase, std::string>&
			EnumConverter::getEnumToStringMap<wpml_codec::enums::GimbalHeadingYawBase>();

		// GimbalPitchMode
		template <>
		const std::unordered_map<std::string, wpml_codec::enums::GimbalPitchMode>&
			EnumConverter::getStringToEnumMap<wpml_codec::enums::GimbalPitchMode>();

		template <>
		const std::unordered_map<wpml_codec::enums::GimbalPitchMode, std::string>&
			EnumConverter::getEnumToStringMap<wpml_codec::enums::GimbalPitchMode>();

		// GimbalRotateMode
		template <>
		const std::unordered_map<std::string, wpml_codec::enums::GimbalRotateMode>&
			EnumConverter::getStringToEnumMap<wpml_codec::enums::GimbalRotateMode>();

		template <>
		const std::unordered_map<wpml_codec::enums::GimbalRotateMode, std::string>&
			EnumConverter::getEnumToStringMap<wpml_codec::enums::GimbalRotateMode>();

		// HeightMode
		template <>
		const std::unordered_map<std::string, wpml_codec::enums::HeightMode>&
			EnumConverter::getStringToEnumMap<wpml_codec::enums::HeightMode>();

		template <>
		const std::unordered_map<wpml_codec::enums::HeightMode, std::string>&
			EnumConverter::getEnumToStringMap<wpml_codec::enums::HeightMode>();

		// ImageFormat
		template <>
		const std::unordered_map<std::string, wpml_codec::enums::ImageFormat>&
			EnumConverter::getStringToEnumMap<wpml_codec::enums::ImageFormat>();

		template <>
		const std::unordered_map<wpml_codec::enums::ImageFormat, std::string>&
			EnumConverter::getEnumToStringMap<wpml_codec::enums::ImageFormat>();

		// MappingHeadingMode
		template <>
		const std::unordered_map<std::string, wpml_codec::enums::MappingHeadingMode>&
			EnumConverter::getStringToEnumMap<wpml_codec::enums::MappingHeadingMode>();

		template <>
		const std::unordered_map<wpml_codec::enums::MappingHeadingMode, std::string>&
			EnumConverter::getEnumToStringMap<wpml_codec::enums::MappingHeadingMode>();

		// MeteringMode
		template <>
		const std::unordered_map<std::string, wpml_codec::enums::MeteringMode>&
			EnumConverter::getStringToEnumMap<wpml_codec::enums::MeteringMode>();

		template <>
		const std::unordered_map<wpml_codec::enums::MeteringMode, std::string>&
			EnumConverter::getEnumToStringMap<wpml_codec::enums::MeteringMode>();

		// OrientedPhotoMode
		template <>
		const std::unordered_map<std::string, wpml_codec::enums::OrientedPhotoMode>&
			EnumConverter::getStringToEnumMap<wpml_codec::enums::OrientedPhotoMode>();

		template <>
		const std::unordered_map<wpml_codec::enums::OrientedPhotoMode, std::string>&
			EnumConverter::getEnumToStringMap<wpml_codec::enums::OrientedPhotoMode>();

		// PanoShotSubMode
		template <>
		const std::unordered_map<std::string, wpml_codec::enums::PanoShotSubMode>&
			EnumConverter::getStringToEnumMap<wpml_codec::enums::PanoShotSubMode>();

		template <>
		const std::unordered_map<wpml_codec::enums::PanoShotSubMode, std::string>&
			EnumConverter::getEnumToStringMap<wpml_codec::enums::PanoShotSubMode>();

		// PositioningType
		template <>
		const std::unordered_map<std::string, wpml_codec::enums::PositioningType>&
			EnumConverter::getStringToEnumMap<wpml_codec::enums::PositioningType>();

		template <>
		const std::unordered_map<wpml_codec::enums::PositioningType, std::string>&
			EnumConverter::getEnumToStringMap<wpml_codec::enums::PositioningType>();

		// RecordPointCloudOperate
		template <>
		const std::unordered_map<std::string, wpml_codec::enums::RecordPointCloudOperate>&
			EnumConverter::getStringToEnumMap<wpml_codec::enums::RecordPointCloudOperate>();

		template <>
		const std::unordered_map<wpml_codec::enums::RecordPointCloudOperate, std::string>&
			EnumConverter::getEnumToStringMap<wpml_codec::enums::RecordPointCloudOperate>();

		// ReturnMode
		template <>
		const std::unordered_map<std::string, wpml_codec::enums::ReturnMode>&
			EnumConverter::getStringToEnumMap<wpml_codec::enums::ReturnMode>();

		template <>
		const std::unordered_map<wpml_codec::enums::ReturnMode, std::string>&
			EnumConverter::getEnumToStringMap<wpml_codec::enums::ReturnMode>();

		// ScanningMode
		template <>
		const std::unordered_map<std::string, wpml_codec::enums::ScanningMode>&
			EnumConverter::getStringToEnumMap<wpml_codec::enums::ScanningMode>();

		template <>
		const std::unordered_map<wpml_codec::enums::ScanningMode, std::string>&
			EnumConverter::getEnumToStringMap<wpml_codec::enums::ScanningMode>();

		// ShootType
		template <>
		const std::unordered_map<std::string, wpml_codec::enums::ShootType>&
			EnumConverter::getStringToEnumMap<wpml_codec::enums::ShootType>();

		template <>
		const std::unordered_map<wpml_codec::enums::ShootType, std::string>&
			EnumConverter::getEnumToStringMap<wpml_codec::enums::ShootType>();

		// TemplateType
		template <>
		const std::unordered_map<std::string, wpml_codec::enums::TemplateType>&
			EnumConverter::getStringToEnumMap<wpml_codec::enums::TemplateType>();

		template <>
		const std::unordered_map<wpml_codec::enums::TemplateType, std::string>&
			EnumConverter::getEnumToStringMap<wpml_codec::enums::TemplateType>();

		// WaypointHeadingMode
		template <>
		const std::unordered_map<std::string, wpml_codec::enums::WaypointHeadingMode>&
			EnumConverter::getStringToEnumMap<wpml_codec::enums::WaypointHeadingMode>();

		template <>
		const std::unordered_map<wpml_codec::enums::WaypointHeadingMode, std::string>&
			EnumConverter::getEnumToStringMap<wpml_codec::enums::WaypointHeadingMode>();

		// WaypointHeadingPathMode
		template <>
		const std::unordered_map<std::string, wpml_codec::enums::WaypointHeadingPathMode>&
			EnumConverter::getStringToEnumMap<wpml_codec::enums::WaypointHeadingPathMode>();

		template <>
		const std::unordered_map<wpml_codec::enums::WaypointHeadingPathMode, std::string>&
			EnumConverter::getEnumToStringMap<wpml_codec::enums::WaypointHeadingPathMode>();

		// WaypointTurnMode
		template <>
		const std::unordered_map<std::string, wpml_codec::enums::WaypointTurnMode>&
			EnumConverter::getStringToEnumMap<wpml_codec::enums::WaypointTurnMode>();

		template <>
		const std::unordered_map<wpml_codec::enums::WaypointTurnMode, std::string>&
			EnumConverter::getEnumToStringMap<wpml_codec::enums::WaypointTurnMode>();



}; // namespace wpml_codec

namespace wce = wpml_codec::enums;



namespace my_enum {
	// 仿照magic_enum::enum_cast的返回类型
	template <typename EnumType>
	class enum_result {
		EnumType value_;
		bool has_value_;
	public:
		enum_result(EnumType value) : value_(value), has_value_(true) {}
		enum_result() : has_value_(false) {}

		explicit operator bool() const { return has_value_; }
		EnumType value() const { return value_; }
	};

	// 字符串 → 枚举 (仿照magic_enum::enum_cast)
	template <typename EnumType>
	enum_result<EnumType> enum_cast(const std::string& str) {
		EnumType out;
		if (wpml_codec::EnumConverter::fromString(str, out)) {
			return enum_result<EnumType>(out);
		}
		return enum_result<EnumType>();
	}

	// 枚举 → 字符串 (新增接口)
	template <typename EnumType>
	std::string enum_name(EnumType value) {
		try {
			return wpml_codec::EnumConverter::toString(value);
		}
		catch (const std::invalid_argument&) {
			return ""; // 或者根据需求返回其他默认值
		}
	}
}


#endif // WPML_CODEC_ENUMS_H
