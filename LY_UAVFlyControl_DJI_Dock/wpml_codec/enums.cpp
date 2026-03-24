#include <wpml_codec/enums.h>

namespace wpml_codec
{
	// ===================== 映射表特化（完整实现） =====================

	// ActionActuatorFunc
	template <>
	const std::unordered_map<std::string, wpml_codec::enums::ActionActuatorFunc>&
		EnumConverter::getStringToEnumMap<wpml_codec::enums::ActionActuatorFunc>() {
		static const std::unordered_map<std::string, wpml_codec::enums::ActionActuatorFunc> map = {
#define MAP_ENTRY(name, str) {str, wpml_codec::enums::ActionActuatorFunc::name},
			ACTION_ACTUATOR_FUNC_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}

	template <>
	const std::unordered_map<wpml_codec::enums::ActionActuatorFunc, std::string>&
		EnumConverter::getEnumToStringMap<wpml_codec::enums::ActionActuatorFunc>() {
		static const std::unordered_map<wpml_codec::enums::ActionActuatorFunc, std::string> map = {
#define MAP_ENTRY(name, str) {wpml_codec::enums::ActionActuatorFunc::name, str},
			ACTION_ACTUATOR_FUNC_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}

	// ActionGroupMode
	template <>
	const std::unordered_map<std::string, wpml_codec::enums::ActionGroupMode>&
		EnumConverter::getStringToEnumMap<wpml_codec::enums::ActionGroupMode>() {
		static const std::unordered_map<std::string, wpml_codec::enums::ActionGroupMode> map = {
#define MAP_ENTRY(name, str) {str, wpml_codec::enums::ActionGroupMode::name},
			ACTION_GROUP_MODE_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}

	template <>
	const std::unordered_map<wpml_codec::enums::ActionGroupMode, std::string>&
		EnumConverter::getEnumToStringMap<wpml_codec::enums::ActionGroupMode>() {
		static const std::unordered_map<wpml_codec::enums::ActionGroupMode, std::string> map = {
#define MAP_ENTRY(name, str) {wpml_codec::enums::ActionGroupMode::name, str},
			ACTION_GROUP_MODE_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}

	// ActionTriggerType
	template <>
	const std::unordered_map<std::string, wpml_codec::enums::ActionTriggerType>&
		EnumConverter::getStringToEnumMap<wpml_codec::enums::ActionTriggerType>() {
		static const std::unordered_map<std::string, wpml_codec::enums::ActionTriggerType> map = {
#define MAP_ENTRY(name, str) {str, wpml_codec::enums::ActionTriggerType::name},
			ACTION_TRIGGER_TYPE_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}

	template <>
	const std::unordered_map<wpml_codec::enums::ActionTriggerType, std::string>&
		EnumConverter::getEnumToStringMap<wpml_codec::enums::ActionTriggerType>() {
		static const std::unordered_map<wpml_codec::enums::ActionTriggerType, std::string> map = {
#define MAP_ENTRY(name, str) {wpml_codec::enums::ActionTriggerType::name, str},
			ACTION_TRIGGER_TYPE_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}

	// AircraftPathMode
	template <>
	const std::unordered_map<std::string, wpml_codec::enums::AircraftPathMode>&
		EnumConverter::getStringToEnumMap<wpml_codec::enums::AircraftPathMode>() {
		static const std::unordered_map<std::string, wpml_codec::enums::AircraftPathMode> map = {
#define MAP_ENTRY(name, str) {str, wpml_codec::enums::AircraftPathMode::name},
			AIRCRAFT_PATH_MODE_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}

	template <>
	const std::unordered_map<wpml_codec::enums::AircraftPathMode, std::string>&
		EnumConverter::getEnumToStringMap<wpml_codec::enums::AircraftPathMode>() {
		static const std::unordered_map<wpml_codec::enums::AircraftPathMode, std::string> map = {
#define MAP_ENTRY(name, str) {wpml_codec::enums::AircraftPathMode::name, str},
			AIRCRAFT_PATH_MODE_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}

	// CoordinateMode
	template <>
	const std::unordered_map<std::string, wpml_codec::enums::CoordinateMode>&
		EnumConverter::getStringToEnumMap<wpml_codec::enums::CoordinateMode>() {
		static const std::unordered_map<std::string, wpml_codec::enums::CoordinateMode> map = {
#define MAP_ENTRY(name, str) {str, wpml_codec::enums::CoordinateMode::name},
			COORDINATE_MODE_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}

	template <>
	const std::unordered_map<wpml_codec::enums::CoordinateMode, std::string>&
		EnumConverter::getEnumToStringMap<wpml_codec::enums::CoordinateMode>() {
		static const std::unordered_map<wpml_codec::enums::CoordinateMode, std::string> map = {
#define MAP_ENTRY(name, str) {wpml_codec::enums::CoordinateMode::name, str},
			COORDINATE_MODE_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}

	// ExecuteHeightMode
	template <>
	const std::unordered_map<std::string, wpml_codec::enums::ExecuteHeightMode>&
		EnumConverter::getStringToEnumMap<wpml_codec::enums::ExecuteHeightMode>() {
		static const std::unordered_map<std::string, wpml_codec::enums::ExecuteHeightMode> map = {
#define MAP_ENTRY(name, str) {str, wpml_codec::enums::ExecuteHeightMode::name},
			EXECUTE_HEIGHT_MODE_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}

	template <>
	const std::unordered_map<wpml_codec::enums::ExecuteHeightMode, std::string>&
		EnumConverter::getEnumToStringMap<wpml_codec::enums::ExecuteHeightMode>() {
		static const std::unordered_map<wpml_codec::enums::ExecuteHeightMode, std::string> map = {
#define MAP_ENTRY(name, str) {wpml_codec::enums::ExecuteHeightMode::name, str},
			EXECUTE_HEIGHT_MODE_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}

	// ExecuteRCLostAction
	template <>
	const std::unordered_map<std::string, wpml_codec::enums::ExecuteRCLostAction>&
		EnumConverter::getStringToEnumMap<wpml_codec::enums::ExecuteRCLostAction>() {
		static const std::unordered_map<std::string, wpml_codec::enums::ExecuteRCLostAction> map = {
#define MAP_ENTRY(name, str) {str, wpml_codec::enums::ExecuteRCLostAction::name},
			EXECUTE_RC_LOST_ACTION_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}

	template <>
	const std::unordered_map<wpml_codec::enums::ExecuteRCLostAction, std::string>&
		EnumConverter::getEnumToStringMap<wpml_codec::enums::ExecuteRCLostAction>() {
		static const std::unordered_map<wpml_codec::enums::ExecuteRCLostAction, std::string> map = {
#define MAP_ENTRY(name, str) {wpml_codec::enums::ExecuteRCLostAction::name, str},
			EXECUTE_RC_LOST_ACTION_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}

	// ExitOnRCLost
	template <>
	const std::unordered_map<std::string, wpml_codec::enums::ExitOnRCLost>&
		EnumConverter::getStringToEnumMap<wpml_codec::enums::ExitOnRCLost>() {
		static const std::unordered_map<std::string, wpml_codec::enums::ExitOnRCLost> map = {
#define MAP_ENTRY(name, str) {str, wpml_codec::enums::ExitOnRCLost::name},
			EXIT_ON_RC_LOST_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}

	template <>
	const std::unordered_map<wpml_codec::enums::ExitOnRCLost, std::string>&
		EnumConverter::getEnumToStringMap<wpml_codec::enums::ExitOnRCLost>() {
		static const std::unordered_map<wpml_codec::enums::ExitOnRCLost, std::string> map = {
#define MAP_ENTRY(name, str) {wpml_codec::enums::ExitOnRCLost::name, str},
			EXIT_ON_RC_LOST_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}

	// FinishAction
	template <>
	const std::unordered_map<std::string, wpml_codec::enums::FinishAction>&
		EnumConverter::getStringToEnumMap<wpml_codec::enums::FinishAction>() {
		static const std::unordered_map<std::string, wpml_codec::enums::FinishAction> map = {
#define MAP_ENTRY(name, str) {str, wpml_codec::enums::FinishAction::name},
			FINISH_ACTION_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}

	template <>
	const std::unordered_map<wpml_codec::enums::FinishAction, std::string>&
		EnumConverter::getEnumToStringMap<wpml_codec::enums::FinishAction>() {
		static const std::unordered_map<wpml_codec::enums::FinishAction, std::string> map = {
#define MAP_ENTRY(name, str) {wpml_codec::enums::FinishAction::name, str},
			FINISH_ACTION_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}

	// FlyToWaylineMode
	template <>
	const std::unordered_map<std::string, wpml_codec::enums::FlyToWaylineMode>&
		EnumConverter::getStringToEnumMap<wpml_codec::enums::FlyToWaylineMode>() {
		static const std::unordered_map<std::string, wpml_codec::enums::FlyToWaylineMode> map = {
#define MAP_ENTRY(name, str) {str, wpml_codec::enums::FlyToWaylineMode::name},
			FLY_TO_WAYLINE_MODE_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}

	template <>
	const std::unordered_map<wpml_codec::enums::FlyToWaylineMode, std::string>&
		EnumConverter::getEnumToStringMap<wpml_codec::enums::FlyToWaylineMode>() {
		static const std::unordered_map<wpml_codec::enums::FlyToWaylineMode, std::string> map = {
#define MAP_ENTRY(name, str) {wpml_codec::enums::FlyToWaylineMode::name, str},
			FLY_TO_WAYLINE_MODE_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}

	// FocusMode
	template <>
	const std::unordered_map<std::string, wpml_codec::enums::FocusMode>&
		EnumConverter::getStringToEnumMap<wpml_codec::enums::FocusMode>() {
		static const std::unordered_map<std::string, wpml_codec::enums::FocusMode> map = {
#define MAP_ENTRY(name, str) {str, wpml_codec::enums::FocusMode::name},
			FOCUS_MODE_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}

	template <>
	const std::unordered_map<wpml_codec::enums::FocusMode, std::string>&
		EnumConverter::getEnumToStringMap<wpml_codec::enums::FocusMode>() {
		static const std::unordered_map<wpml_codec::enums::FocusMode, std::string> map = {
#define MAP_ENTRY(name, str) {wpml_codec::enums::FocusMode::name, str},
			FOCUS_MODE_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}

	// GimbalHeadingYawBase
	template <>
	const std::unordered_map<std::string, wpml_codec::enums::GimbalHeadingYawBase>&
		EnumConverter::getStringToEnumMap<wpml_codec::enums::GimbalHeadingYawBase>() {
		static const std::unordered_map<std::string, wpml_codec::enums::GimbalHeadingYawBase> map = {
#define MAP_ENTRY(name, str) {str, wpml_codec::enums::GimbalHeadingYawBase::name},
			GIMBAL_HEADING_YAW_BASE_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}

	template <>
	const std::unordered_map<wpml_codec::enums::GimbalHeadingYawBase, std::string>&
		EnumConverter::getEnumToStringMap<wpml_codec::enums::GimbalHeadingYawBase>() {
		static const std::unordered_map<wpml_codec::enums::GimbalHeadingYawBase, std::string> map = {
#define MAP_ENTRY(name, str) {wpml_codec::enums::GimbalHeadingYawBase::name, str},
			GIMBAL_HEADING_YAW_BASE_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}

	// GimbalPitchMode
	template <>
	const std::unordered_map<std::string, wpml_codec::enums::GimbalPitchMode>&
		EnumConverter::getStringToEnumMap<wpml_codec::enums::GimbalPitchMode>() {
		static const std::unordered_map<std::string, wpml_codec::enums::GimbalPitchMode> map = {
#define MAP_ENTRY(name, str) {str, wpml_codec::enums::GimbalPitchMode::name},
			GIMBAL_PITCH_MODE_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}

	template <>
	const std::unordered_map<wpml_codec::enums::GimbalPitchMode, std::string>&
		EnumConverter::getEnumToStringMap<wpml_codec::enums::GimbalPitchMode>() {
		static const std::unordered_map<wpml_codec::enums::GimbalPitchMode, std::string> map = {
#define MAP_ENTRY(name, str) {wpml_codec::enums::GimbalPitchMode::name, str},
			GIMBAL_PITCH_MODE_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}

	// GimbalRotateMode
	template <>
	const std::unordered_map<std::string, wpml_codec::enums::GimbalRotateMode>&
		EnumConverter::getStringToEnumMap<wpml_codec::enums::GimbalRotateMode>() {
		static const std::unordered_map<std::string, wpml_codec::enums::GimbalRotateMode> map = {
#define MAP_ENTRY(name, str) {str, wpml_codec::enums::GimbalRotateMode::name},
			GIMBAL_ROTATE_MODE_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}

	template <>
	const std::unordered_map<wpml_codec::enums::GimbalRotateMode, std::string>&
		EnumConverter::getEnumToStringMap<wpml_codec::enums::GimbalRotateMode>() {
		static const std::unordered_map<wpml_codec::enums::GimbalRotateMode, std::string> map = {
#define MAP_ENTRY(name, str) {wpml_codec::enums::GimbalRotateMode::name, str},
			GIMBAL_ROTATE_MODE_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}

	// HeightMode
	template <>
	const std::unordered_map<std::string, wpml_codec::enums::HeightMode>&
		EnumConverter::getStringToEnumMap<wpml_codec::enums::HeightMode>() {
		static const std::unordered_map<std::string, wpml_codec::enums::HeightMode> map = {
#define MAP_ENTRY(name, str) {str, wpml_codec::enums::HeightMode::name},
			HEIGHT_MODE_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}

	template <>
	const std::unordered_map<wpml_codec::enums::HeightMode, std::string>&
		EnumConverter::getEnumToStringMap<wpml_codec::enums::HeightMode>() {
		static const std::unordered_map<wpml_codec::enums::HeightMode, std::string> map = {
#define MAP_ENTRY(name, str) {wpml_codec::enums::HeightMode::name, str},
			HEIGHT_MODE_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}

	// ImageFormat
	template <>
	const std::unordered_map<std::string, wpml_codec::enums::ImageFormat>&
		EnumConverter::getStringToEnumMap<wpml_codec::enums::ImageFormat>() {
		static const std::unordered_map<std::string, wpml_codec::enums::ImageFormat> map = {
#define MAP_ENTRY(name, str) {str, wpml_codec::enums::ImageFormat::name},
			IMAGE_FORMAT_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}

	template <>
	const std::unordered_map<wpml_codec::enums::ImageFormat, std::string>&
		EnumConverter::getEnumToStringMap<wpml_codec::enums::ImageFormat>() {
		static const std::unordered_map<wpml_codec::enums::ImageFormat, std::string> map = {
#define MAP_ENTRY(name, str) {wpml_codec::enums::ImageFormat::name, str},
			IMAGE_FORMAT_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}

	// MappingHeadingMode
	template <>
	const std::unordered_map<std::string, wpml_codec::enums::MappingHeadingMode>&
		EnumConverter::getStringToEnumMap<wpml_codec::enums::MappingHeadingMode>() {
		static const std::unordered_map<std::string, wpml_codec::enums::MappingHeadingMode> map = {
#define MAP_ENTRY(name, str) {str, wpml_codec::enums::MappingHeadingMode::name},
			MAPPING_HEADING_MODE_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}

	template <>
	const std::unordered_map<wpml_codec::enums::MappingHeadingMode, std::string>&
		EnumConverter::getEnumToStringMap<wpml_codec::enums::MappingHeadingMode>() {
		static const std::unordered_map<wpml_codec::enums::MappingHeadingMode, std::string> map = {
#define MAP_ENTRY(name, str) {wpml_codec::enums::MappingHeadingMode::name, str},
			MAPPING_HEADING_MODE_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}

	// MeteringMode
	template <>
	const std::unordered_map<std::string, wpml_codec::enums::MeteringMode>&
		EnumConverter::getStringToEnumMap<wpml_codec::enums::MeteringMode>() {
		static const std::unordered_map<std::string, wpml_codec::enums::MeteringMode> map = {
#define MAP_ENTRY(name, str) {str, wpml_codec::enums::MeteringMode::name},
			METERING_MODE_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}

	template <>
	const std::unordered_map<wpml_codec::enums::MeteringMode, std::string>&
		EnumConverter::getEnumToStringMap<wpml_codec::enums::MeteringMode>() {
		static const std::unordered_map<wpml_codec::enums::MeteringMode, std::string> map = {
#define MAP_ENTRY(name, str) {wpml_codec::enums::MeteringMode::name, str},
			METERING_MODE_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}

	// OrientedPhotoMode
	template <>
	const std::unordered_map<std::string, wpml_codec::enums::OrientedPhotoMode>&
		EnumConverter::getStringToEnumMap<wpml_codec::enums::OrientedPhotoMode>() {
		static const std::unordered_map<std::string, wpml_codec::enums::OrientedPhotoMode> map = {
#define MAP_ENTRY(name, str) {str, wpml_codec::enums::OrientedPhotoMode::name},
			ORIENTED_PHOTO_MODE_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}

	template <>
	const std::unordered_map<wpml_codec::enums::OrientedPhotoMode, std::string>&
		EnumConverter::getEnumToStringMap<wpml_codec::enums::OrientedPhotoMode>() {
		static const std::unordered_map<wpml_codec::enums::OrientedPhotoMode, std::string> map = {
#define MAP_ENTRY(name, str) {wpml_codec::enums::OrientedPhotoMode::name, str},
			ORIENTED_PHOTO_MODE_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}

	// PanoShotSubMode
	template <>
	const std::unordered_map<std::string, wpml_codec::enums::PanoShotSubMode>&
		EnumConverter::getStringToEnumMap<wpml_codec::enums::PanoShotSubMode>() {
		static const std::unordered_map<std::string, wpml_codec::enums::PanoShotSubMode> map = {
#define MAP_ENTRY(name, str) {str, wpml_codec::enums::PanoShotSubMode::name},
			PANO_SHOT_SUB_MODE_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}

	template <>
	const std::unordered_map<wpml_codec::enums::PanoShotSubMode, std::string>&
		EnumConverter::getEnumToStringMap<wpml_codec::enums::PanoShotSubMode>() {
		static const std::unordered_map<wpml_codec::enums::PanoShotSubMode, std::string> map = {
#define MAP_ENTRY(name, str) {wpml_codec::enums::PanoShotSubMode::name, str},
			PANO_SHOT_SUB_MODE_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}

	// PositioningType
	template <>
	const std::unordered_map<std::string, wpml_codec::enums::PositioningType>&
		EnumConverter::getStringToEnumMap<wpml_codec::enums::PositioningType>() {
		static const std::unordered_map<std::string, wpml_codec::enums::PositioningType> map = {
#define MAP_ENTRY(name, str) {str, wpml_codec::enums::PositioningType::name},
			POSITIONING_TYPE_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}

	template <>
	const std::unordered_map<wpml_codec::enums::PositioningType, std::string>&
		EnumConverter::getEnumToStringMap<wpml_codec::enums::PositioningType>() {
		static const std::unordered_map<wpml_codec::enums::PositioningType, std::string> map = {
#define MAP_ENTRY(name, str) {wpml_codec::enums::PositioningType::name, str},
			POSITIONING_TYPE_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}

	// RecordPointCloudOperate
	template <>
	const std::unordered_map<std::string, wpml_codec::enums::RecordPointCloudOperate>&
		EnumConverter::getStringToEnumMap<wpml_codec::enums::RecordPointCloudOperate>() {
		static const std::unordered_map<std::string, wpml_codec::enums::RecordPointCloudOperate> map = {
#define MAP_ENTRY(name, str) {str, wpml_codec::enums::RecordPointCloudOperate::name},
			RECORD_POINT_CLOUD_OPERATE_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}

	template <>
	const std::unordered_map<wpml_codec::enums::RecordPointCloudOperate, std::string>&
		EnumConverter::getEnumToStringMap<wpml_codec::enums::RecordPointCloudOperate>() {
		static const std::unordered_map<wpml_codec::enums::RecordPointCloudOperate, std::string> map = {
#define MAP_ENTRY(name, str) {wpml_codec::enums::RecordPointCloudOperate::name, str},
			RECORD_POINT_CLOUD_OPERATE_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}

	// ReturnMode
	template <>
	const std::unordered_map<std::string, wpml_codec::enums::ReturnMode>&
		EnumConverter::getStringToEnumMap<wpml_codec::enums::ReturnMode>() {
		static const std::unordered_map<std::string, wpml_codec::enums::ReturnMode> map = {
#define MAP_ENTRY(name, str) {str, wpml_codec::enums::ReturnMode::name},
			RETURN_MODE_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}

	template <>
	const std::unordered_map<wpml_codec::enums::ReturnMode, std::string>&
		EnumConverter::getEnumToStringMap<wpml_codec::enums::ReturnMode>() {
		static const std::unordered_map<wpml_codec::enums::ReturnMode, std::string> map = {
#define MAP_ENTRY(name, str) {wpml_codec::enums::ReturnMode::name, str},
			RETURN_MODE_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}

	// ScanningMode
	template <>
	const std::unordered_map<std::string, wpml_codec::enums::ScanningMode>&
		EnumConverter::getStringToEnumMap<wpml_codec::enums::ScanningMode>() {
		static const std::unordered_map<std::string, wpml_codec::enums::ScanningMode> map = {
#define MAP_ENTRY(name, str) {str, wpml_codec::enums::ScanningMode::name},
			SCANNING_MODE_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}

	template <>
	const std::unordered_map<wpml_codec::enums::ScanningMode, std::string>&
		EnumConverter::getEnumToStringMap<wpml_codec::enums::ScanningMode>() {
		static const std::unordered_map<wpml_codec::enums::ScanningMode, std::string> map = {
#define MAP_ENTRY(name, str) {wpml_codec::enums::ScanningMode::name, str},
			SCANNING_MODE_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}

	// ShootType
	template <>
	const std::unordered_map<std::string, wpml_codec::enums::ShootType>&
		EnumConverter::getStringToEnumMap<wpml_codec::enums::ShootType>() {
		static const std::unordered_map<std::string, wpml_codec::enums::ShootType> map = {
#define MAP_ENTRY(name, str) {str, wpml_codec::enums::ShootType::name},
			SHOOT_TYPE_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}

	template <>
	const std::unordered_map<wpml_codec::enums::ShootType, std::string>&
		EnumConverter::getEnumToStringMap<wpml_codec::enums::ShootType>() {
		static const std::unordered_map<wpml_codec::enums::ShootType, std::string> map = {
#define MAP_ENTRY(name, str) {wpml_codec::enums::ShootType::name, str},
			SHOOT_TYPE_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}

	// TemplateType
	template <>
	const std::unordered_map<std::string, wpml_codec::enums::TemplateType>&
		EnumConverter::getStringToEnumMap<wpml_codec::enums::TemplateType>() {
		static const std::unordered_map<std::string, wpml_codec::enums::TemplateType> map = {
#define MAP_ENTRY(name, str) {str, wpml_codec::enums::TemplateType::name},
			TEMPLATE_TYPE_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}

	template <>
	const std::unordered_map<wpml_codec::enums::TemplateType, std::string>&
		EnumConverter::getEnumToStringMap<wpml_codec::enums::TemplateType>() {
		static const std::unordered_map<wpml_codec::enums::TemplateType, std::string> map = {
#define MAP_ENTRY(name, str) {wpml_codec::enums::TemplateType::name, str},
			TEMPLATE_TYPE_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}

	// WaypointHeadingMode
	template <>
	const std::unordered_map<std::string, wpml_codec::enums::WaypointHeadingMode>&
		EnumConverter::getStringToEnumMap<wpml_codec::enums::WaypointHeadingMode>() {
		static const std::unordered_map<std::string, wpml_codec::enums::WaypointHeadingMode> map = {
#define MAP_ENTRY(name, str) {str, wpml_codec::enums::WaypointHeadingMode::name},
			WAYPOINT_HEADING_MODE_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}

	template <>
	const std::unordered_map<wpml_codec::enums::WaypointHeadingMode, std::string>&
		EnumConverter::getEnumToStringMap<wpml_codec::enums::WaypointHeadingMode>() {
		static const std::unordered_map<wpml_codec::enums::WaypointHeadingMode, std::string> map = {
#define MAP_ENTRY(name, str) {wpml_codec::enums::WaypointHeadingMode::name, str},
			WAYPOINT_HEADING_MODE_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}

	// WaypointHeadingPathMode
	template <>
	const std::unordered_map<std::string, wpml_codec::enums::WaypointHeadingPathMode>&
		EnumConverter::getStringToEnumMap<wpml_codec::enums::WaypointHeadingPathMode>() {
		static const std::unordered_map<std::string, wpml_codec::enums::WaypointHeadingPathMode> map = {
#define MAP_ENTRY(name, str) {str, wpml_codec::enums::WaypointHeadingPathMode::name},
			WAYPOINT_HEADING_PATH_MODE_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}

	template <>
	const std::unordered_map<wpml_codec::enums::WaypointHeadingPathMode, std::string>&
		EnumConverter::getEnumToStringMap<wpml_codec::enums::WaypointHeadingPathMode>() {
		static const std::unordered_map<wpml_codec::enums::WaypointHeadingPathMode, std::string> map = {
#define MAP_ENTRY(name, str) {wpml_codec::enums::WaypointHeadingPathMode::name, str},
			WAYPOINT_HEADING_PATH_MODE_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}

	// WaypointTurnMode
	template <>
	const std::unordered_map<std::string, wpml_codec::enums::WaypointTurnMode>&
		EnumConverter::getStringToEnumMap<wpml_codec::enums::WaypointTurnMode>() {
		static const std::unordered_map<std::string, wpml_codec::enums::WaypointTurnMode> map = {
#define MAP_ENTRY(name, str) {str, wpml_codec::enums::WaypointTurnMode::name},
			WAYPOINT_TURN_MODE_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}

	template <>
	const std::unordered_map<wpml_codec::enums::WaypointTurnMode, std::string>&
		EnumConverter::getEnumToStringMap<wpml_codec::enums::WaypointTurnMode>() {
		static const std::unordered_map<wpml_codec::enums::WaypointTurnMode, std::string> map = {
#define MAP_ENTRY(name, str) {wpml_codec::enums::WaypointTurnMode::name, str},
			WAYPOINT_TURN_MODE_ENUM(MAP_ENTRY)
#undef MAP_ENTRY
		};
		return map;
	}
}