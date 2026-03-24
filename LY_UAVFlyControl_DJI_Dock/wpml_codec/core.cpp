#include <wpml_codec/core.h>
#include <wpml_codec/utils.h>
#include <wpml_codec/macros.h>
#include <tinyxml/tinyxml2.h>
#include <future>
#include <iostream>
#include <thread>

namespace xml = tinyxml2;

namespace wpml_codec
{
	namespace core
{
boost::optional<wcs::KMLDocument> parseKML(const std::string& kmlPath)
{
    wcs::KMLDocument res;
    xml::XMLDocument doc;
    xml::XMLError eResult = doc.LoadFile(kmlPath.c_str());
    if (eResult != xml::XML_SUCCESS)
    {
        std::cerr << "Failed to load kml file: " << kmlPath << std::endl;
        return boost::none;
    }
    // Parse KML
    // Step 1: Implement File Creation Information
    xml::XMLElement *pDocument = doc.FirstChildElement("kml")->FirstChildElement("Document");
    if (pDocument == nullptr)
    {
        std::cerr << "No Document element found" << std::endl;
        return boost::none;
    }
    SET_OPT_WPML_ARG_S(res, pDocument, author);
    SET_OPT_WPML_ARG_L(res, pDocument, createTime);
    SET_OPT_WPML_ARG_L(res, pDocument, updateTime);
    // Step 2: Setup Mission Configuration
    xml::XMLElement *pMissionConfig = pDocument->FirstChildElement("wpml:missionConfig");
    if (pMissionConfig == nullptr)
    {
        std::cerr << "No wpml:missionConfig element found" << std::endl;
        return boost::none;
    }
    SET_OPT_WPML_ARG_E(res.missionConfig, pMissionConfig, flyToWaylineMode, FlyToWaylineMode);
    SET_OPT_WPML_ARG_E(res.missionConfig, pMissionConfig, finishAction, FinishAction);
    SET_OPT_WPML_ARG_E(res.missionConfig, pMissionConfig, exitOnRCLost, ExitOnRCLost);
    SET_OPT_WPML_ARG_E(res.missionConfig, pMissionConfig, executeRCLostAction, ExecuteRCLostAction);
    SET_OPT_WPML_ARG_D(res.missionConfig, pMissionConfig, takeOffSecurityHeight);
    SET_OPT_WPML_ARG_D(res.missionConfig, pMissionConfig, globalTransitionalSpeed);
    SET_OPT_WPML_ARG_D(res.missionConfig, pMissionConfig, globalRTHHeight);
    SET_OPT_WPML_ARG_P(res.missionConfig, pMissionConfig, takeOffRefPoint);
    SET_OPT_WPML_ARG_D(res.missionConfig, pMissionConfig, takeOffRefPointAGLHeight);
    xml::XMLElement *pDroneInfo = pMissionConfig->FirstChildElement("wpml:droneInfo");
    if (pDroneInfo != nullptr)
    {
        SET_OPT_WPML_ARG_I(res.missionConfig.droneInfo, pDroneInfo, droneEnumValue);
        SET_OPT_WPML_ARG_I(res.missionConfig.droneInfo, pDroneInfo, droneSubEnumValue);
    }
    xml::XMLElement *pPayloadInfo = pMissionConfig->FirstChildElement("wpml:payloadInfo");
    if (pPayloadInfo != nullptr)
    {
        SET_OPT_WPML_ARG_I(res.missionConfig.payloadInfo, pPayloadInfo, payloadEnumValue);
        SET_OPT_WPML_ARG_I(res.missionConfig.payloadInfo, pPayloadInfo, payloadPositionIndex);
    }
    xml::XMLElement *pAutoRerouteInfo = pMissionConfig->FirstChildElement("wpml:autoRerouteInfo");
    if (pAutoRerouteInfo != nullptr)
    {
        SET_OPT_WPML_ARG_I(res.missionConfig.autoRerouteInfo, pAutoRerouteInfo, missionAutoRerouteMode);
        SET_OPT_WPML_ARG_I(res.missionConfig.autoRerouteInfo, pAutoRerouteInfo, transitionalAutoRerouteMode);
    }
    // Step 3: Setup A Folder for Waypoint Template
    xml::XMLElement *pFolder = pDocument->FirstChildElement("Folder");
    if (pFolder == nullptr)
    {
        std::cerr << "No Folder element found" << std::endl;
        return boost::none;
    }
    std::string templateType = pFolder->FirstChildElement("wpml:templateType")->GetText();
    if (wcu::equal(templateType, "waypoint"))
    {
        res.folder = std::make_shared<wcs::WaypointFlightKMLFolder>();
        auto curFolder = std::dynamic_pointer_cast<wcs::WaypointFlightKMLFolder>(res.folder);
        SET_OPT_WPML_ARG_E(*curFolder, pFolder, templateType, TemplateType);
        SET_OPT_WPML_ARG_I(*curFolder, pFolder, templateId);
        SET_OPT_WPML_ARG_D(*curFolder, pFolder, autoFlightSpeed);
        SET_OPT_WPML_ARG_E(*curFolder, pFolder, globalWaypointTurnMode, WaypointTurnMode);
        SET_OPT_WPML_ARG_I(*curFolder, pFolder, globalUseStraightLine);
        SET_OPT_WPML_ARG_E(*curFolder, pFolder, gimbalPitchMode, GimbalPitchMode);
        SET_OPT_WPML_ARG_D(*curFolder, pFolder, globalHeight);
        xml::XMLElement *pWaylineCoordinateSysParam = pFolder->FirstChildElement("wpml:waylineCoordinateSysParam");
        if (pWaylineCoordinateSysParam != nullptr)
        {
            SET_OPT_WPML_ARG_E(
                curFolder->waylineCoordinateSysParam, pWaylineCoordinateSysParam, coordinateMode, CoordinateMode);
            SET_OPT_WPML_ARG_E(
                curFolder->waylineCoordinateSysParam, pWaylineCoordinateSysParam, heightMode, HeightMode);
            SET_OPT_WPML_ARG_E(
                curFolder->waylineCoordinateSysParam, pWaylineCoordinateSysParam, positioningType, PositioningType);
            SET_OPT_WPML_ARG_D(curFolder->waylineCoordinateSysParam, pWaylineCoordinateSysParam, globalShootHeight);
            SET_OPT_WPML_ARG_I(
                curFolder->waylineCoordinateSysParam, pWaylineCoordinateSysParam, surfaceFollowModeEnable);
            SET_OPT_WPML_ARG_D(curFolder->waylineCoordinateSysParam, pWaylineCoordinateSysParam, surfaceRelativeHeight);
        }
        xml::XMLElement *pPayloadParam = pFolder->FirstChildElement("wpml:payloadParam");
        if (pPayloadParam != nullptr)
        {
            SET_OPT_WPML_ARG_I(curFolder->payloadParam, pPayloadParam, payloadPositionIndex);
            SET_OPT_WPML_ARG_E(curFolder->payloadParam, pPayloadParam, focusMode, FocusMode);
            SET_OPT_WPML_ARG_E(curFolder->payloadParam, pPayloadParam, meteringMode, MeteringMode);
            SET_OPT_WPML_ARG_I(curFolder->payloadParam, pPayloadParam, dewarpingEnable);
            SET_OPT_WPML_ARG_E(curFolder->payloadParam, pPayloadParam, returnMode, ReturnMode);
            SET_OPT_WPML_ARG_I(curFolder->payloadParam, pPayloadParam, samplingRate);
            SET_OPT_WPML_ARG_E(curFolder->payloadParam, pPayloadParam, scanningMode, ScanningMode);
            SET_OPT_WPML_ARG_I(curFolder->payloadParam, pPayloadParam, modelColoringEnable);
            SET_OPT_WPML_ARG_ES(curFolder->payloadParam, pPayloadParam, imageFormat, ImageFormat);
        }
        xml::XMLElement *pGlobalWaypointHeadingParam = pFolder->FirstChildElement("wpml:globalWaypointHeadingParam");
        if (pGlobalWaypointHeadingParam != nullptr)
        {
            SET_OPT_WPML_ARG_E(curFolder->globalWaypointHeadingParam,
                               pGlobalWaypointHeadingParam,
                               waypointHeadingMode,
                               WaypointHeadingMode);
            SET_OPT_WPML_ARG_D(
                curFolder->globalWaypointHeadingParam, pGlobalWaypointHeadingParam, waypointHeadingAngle);
            SET_OPT_WPML_ARG_P(curFolder->globalWaypointHeadingParam, pGlobalWaypointHeadingParam, waypointPoiPoint);
            SET_OPT_WPML_ARG_E(curFolder->globalWaypointHeadingParam,
                               pGlobalWaypointHeadingParam,
                               waypointHeadingPathMode,
                               WaypointHeadingPathMode);
        }
        // Step 4: Setup Waypoint Placemark
        std::vector<wcs::WaypointInfoKMLPlacemark> waypointPlacemarks{};
        xml::XMLElement *pPlacemark = pFolder->FirstChildElement("Placemark");
        while (pPlacemark)
        {
            wcs::WaypointInfoKMLPlacemark tmpPlacemark;
            SET_OPT_WPML_ARG_I(tmpPlacemark, pPlacemark, isRisky);
			SET_OPT_WPML_ARG_CPT(tmpPlacemark, pPlacemark, point);
            SET_OPT_WPML_ARG_I(tmpPlacemark, pPlacemark, index);
            SET_OPT_WPML_ARG_I(tmpPlacemark, pPlacemark, useGlobalHeight);
            SET_OPT_WPML_ARG_D(tmpPlacemark, pPlacemark, ellipsoidHeight);
            SET_OPT_WPML_ARG_D(tmpPlacemark, pPlacemark, height);
            SET_OPT_WPML_ARG_I(tmpPlacemark, pPlacemark, useGlobalSpeed);
            SET_OPT_WPML_ARG_D(tmpPlacemark, pPlacemark, waypointSpeed);
            SET_OPT_WPML_ARG_I(tmpPlacemark, pPlacemark, useGlobalTurnParam);
            SET_OPT_WPML_ARG_I(tmpPlacemark, pPlacemark, useStraightLine);
            SET_OPT_WPML_ARG_D(tmpPlacemark, pPlacemark, gimbalPitchAngle);
            xml::XMLElement *pWaypointHeadingParam = pPlacemark->FirstChildElement("wpml:waypointHeadingParam");
            if (pWaypointHeadingParam != nullptr)
            {
                SET_OPT_WPML_ARG_E(
                    tmpPlacemark.waypointHeadingParam, pWaypointHeadingParam, waypointHeadingMode, WaypointHeadingMode);
                SET_OPT_WPML_ARG_D(tmpPlacemark.waypointHeadingParam, pWaypointHeadingParam, waypointHeadingAngle);
                SET_OPT_WPML_ARG_P(tmpPlacemark.waypointHeadingParam, pWaypointHeadingParam, waypointPoiPoint);
                SET_OPT_WPML_ARG_E(tmpPlacemark.waypointHeadingParam,
                                   pWaypointHeadingParam,
                                   waypointHeadingPathMode,
                                   WaypointHeadingPathMode);
            }
            xml::XMLElement *pWaypointTurnParam = pPlacemark->FirstChildElement("wpml:waypointTurnParam");
            if (pWaypointTurnParam != nullptr)
            {
                SET_OPT_WPML_ARG_E(
                    tmpPlacemark.waypointTurnParam, pWaypointTurnParam, waypointTurnMode, WaypointTurnMode);
                SET_OPT_WPML_ARG_D(tmpPlacemark.waypointTurnParam, pWaypointTurnParam, waypointTurnDampingDist);
            }
            waypointPlacemarks.emplace_back(tmpPlacemark);
            pPlacemark = pPlacemark->NextSiblingElement("Placemark");
        }
        curFolder->placemark = std::move(waypointPlacemarks);
    }
    else
    {
        res.folder = std::make_shared<wcs::KMLFolder>();
        auto curFolder = std::dynamic_pointer_cast<wcs::KMLFolder>(res.folder);
        SET_OPT_WPML_ARG_E(*curFolder, pFolder, templateType, TemplateType);
        SET_OPT_WPML_ARG_I(*curFolder, pFolder, templateId);
        SET_OPT_WPML_ARG_D(*curFolder, pFolder, autoFlightSpeed);
        xml::XMLElement *pWaylineCoordinateSysParam = pFolder->FirstChildElement("wpml:waylineCoordinateSysParam");
        if (pWaylineCoordinateSysParam != nullptr)
        {
            SET_OPT_WPML_ARG_E(
                curFolder->waylineCoordinateSysParam, pWaylineCoordinateSysParam, coordinateMode, CoordinateMode);
            SET_OPT_WPML_ARG_E(
                curFolder->waylineCoordinateSysParam, pWaylineCoordinateSysParam, heightMode, HeightMode);
            SET_OPT_WPML_ARG_E(
                curFolder->waylineCoordinateSysParam, pWaylineCoordinateSysParam, positioningType, PositioningType);
            SET_OPT_WPML_ARG_D(curFolder->waylineCoordinateSysParam, pWaylineCoordinateSysParam, globalShootHeight);
            SET_OPT_WPML_ARG_I(
                curFolder->waylineCoordinateSysParam, pWaylineCoordinateSysParam, surfaceFollowModeEnable);
            SET_OPT_WPML_ARG_D(curFolder->waylineCoordinateSysParam, pWaylineCoordinateSysParam, surfaceRelativeHeight);
        }
        xml::XMLElement *pPayloadParam = pFolder->FirstChildElement("wpml:payloadParam");
        if (pPayloadParam != nullptr)
        {
            SET_OPT_WPML_ARG_I(curFolder->payloadParam, pPayloadParam, payloadPositionIndex);
            SET_OPT_WPML_ARG_E(curFolder->payloadParam, pPayloadParam, focusMode, FocusMode);
            SET_OPT_WPML_ARG_E(curFolder->payloadParam, pPayloadParam, meteringMode, MeteringMode);
            SET_OPT_WPML_ARG_I(curFolder->payloadParam, pPayloadParam, dewarpingEnable);
            SET_OPT_WPML_ARG_E(curFolder->payloadParam, pPayloadParam, returnMode, ReturnMode);
            SET_OPT_WPML_ARG_I(curFolder->payloadParam, pPayloadParam, samplingRate);
            SET_OPT_WPML_ARG_E(curFolder->payloadParam, pPayloadParam, scanningMode, ScanningMode);
            SET_OPT_WPML_ARG_I(curFolder->payloadParam, pPayloadParam, modelColoringEnable);
            SET_OPT_WPML_ARG_ES(curFolder->payloadParam, pPayloadParam, imageFormat, ImageFormat);
        }
        // Step 4: Setup Placemark
        xml::XMLElement *pPlacemark = pFolder->FirstChildElement("Placemark");
        if (pPlacemark == nullptr)
        {
            std::cerr << "No Placemark element found" << std::endl;
            return boost::none;
        }
        switch (curFolder->templateType)
        {
            case wce::TemplateType::mapping2d :
            {
                curFolder->placemark = std::make_shared<wcs::MappingAerialKMLPlacemark>();
                auto curPlacemark = std::dynamic_pointer_cast<wcs::MappingAerialKMLPlacemark>(curFolder->placemark);
                SET_OPT_WPML_ARG_I(*curPlacemark, pPlacemark, caliFlightEnable);
                SET_OPT_WPML_ARG_I(*curPlacemark, pPlacemark, elevationOptimizeEnable);
                SET_OPT_WPML_ARG_I(*curPlacemark, pPlacemark, smartObliqueEnable);
                SET_OPT_WPML_ARG_I(*curPlacemark, pPlacemark, smartObliqueGimbalPitch);
                SET_OPT_WPML_ARG_E(*curPlacemark, pPlacemark, shootType, ShootType);
                SET_OPT_WPML_ARG_I(*curPlacemark, pPlacemark, direction);
                SET_OPT_WPML_ARG_I(*curPlacemark, pPlacemark, margin);
                xml::XMLElement *pOverlap = pPlacemark->FirstChildElement("wpml:overlap");
                if (pOverlap != nullptr)
                {
                    SET_OPT_WPML_ARG_I(curPlacemark->overlap, pOverlap, orthoLidarOverlapH);
                    SET_OPT_WPML_ARG_I(curPlacemark->overlap, pOverlap, orthoLidarOverlapW);
                    SET_OPT_WPML_ARG_I(curPlacemark->overlap, pOverlap, orthoCameraOverlapH);
                    SET_OPT_WPML_ARG_I(curPlacemark->overlap, pOverlap, orthoCameraOverlapW);
                    SET_OPT_WPML_ARG_I(curPlacemark->overlap, pOverlap, inclinedLidarOverlapH);
                    SET_OPT_WPML_ARG_I(curPlacemark->overlap, pOverlap, inclinedLidarOverlapW);
                    SET_OPT_WPML_ARG_I(curPlacemark->overlap, pOverlap, inclinedCameraOverlapH);
                    SET_OPT_WPML_ARG_I(curPlacemark->overlap, pOverlap, inclinedCameraOverlapW);
                }
                SET_OPT_WPML_ARG_D(*curPlacemark, pPlacemark, ellipsoidHeight);
                SET_OPT_WPML_ARG_D(*curPlacemark, pPlacemark, height);
                SET_OPT_WPML_ARG_I(*curPlacemark, pPlacemark, facadeWaylineEnable);
                SET_OPT_WPML_ARG_KPN(*curPlacemark, pPlacemark, polygon);
                xml::XMLElement *pMappingHeadingParam = pPlacemark->FirstChildElement("wpml:mappingHeadingParam");
                if (pMappingHeadingParam != nullptr)
                {
                    SET_OPT_WPML_ARG_E(curPlacemark->mappingHeadingParam,
                                       pMappingHeadingParam,
                                       mappingHeadingMode,
                                       MappingHeadingMode);
                    SET_OPT_WPML_ARG_I(curPlacemark->mappingHeadingParam, pMappingHeadingParam, mappingHeadingAngle);
                }
                SET_OPT_WPML_ARG_E(*curPlacemark, pPlacemark, gimbalPitchMode, GimbalPitchMode);
                SET_OPT_WPML_ARG_I(*curPlacemark, pPlacemark, gimbalPitchAngle);
                SET_OPT_WPML_ARG_I(*curPlacemark, pPlacemark, quickOrthoMappingEnable);
                SET_OPT_WPML_ARG_I(*curPlacemark, pPlacemark, quickOrthoMappingPitch);
                break;
            }
            case wce::TemplateType::mapping3d :
            {
                curFolder->placemark = std::make_shared<wcs::ObliquePhotographyKMLPlacemark>();
                auto curPlacemark =
                    std::dynamic_pointer_cast<wcs::ObliquePhotographyKMLPlacemark>(curFolder->placemark);
                SET_OPT_WPML_ARG_I(*curPlacemark, pPlacemark, caliFlightEnable);
                SET_OPT_WPML_ARG_I(*curPlacemark, pPlacemark, inclinedGimbalPitch);
                SET_OPT_WPML_ARG_D(*curPlacemark, pPlacemark, inclinedFlightSpeed);
                SET_OPT_WPML_ARG_E(*curPlacemark, pPlacemark, shootType, ShootType);
                SET_OPT_WPML_ARG_I(*curPlacemark, pPlacemark, direction);
                SET_OPT_WPML_ARG_I(*curPlacemark, pPlacemark, margin);
                xml::XMLElement *pOverlap = pPlacemark->FirstChildElement("wpml:overlap");
                if (pOverlap != nullptr)
                {
                    SET_OPT_WPML_ARG_I(curPlacemark->overlap, pOverlap, orthoLidarOverlapH);
                    SET_OPT_WPML_ARG_I(curPlacemark->overlap, pOverlap, orthoLidarOverlapW);
                    SET_OPT_WPML_ARG_I(curPlacemark->overlap, pOverlap, orthoCameraOverlapH);
                    SET_OPT_WPML_ARG_I(curPlacemark->overlap, pOverlap, orthoCameraOverlapW);
                    SET_OPT_WPML_ARG_I(curPlacemark->overlap, pOverlap, inclinedLidarOverlapH);
                    SET_OPT_WPML_ARG_I(curPlacemark->overlap, pOverlap, inclinedLidarOverlapW);
                    SET_OPT_WPML_ARG_I(curPlacemark->overlap, pOverlap, inclinedCameraOverlapH);
                    SET_OPT_WPML_ARG_I(curPlacemark->overlap, pOverlap, inclinedCameraOverlapW);
                }
                SET_OPT_WPML_ARG_D(*curPlacemark, pPlacemark, ellipsoidHeight);
                SET_OPT_WPML_ARG_D(*curPlacemark, pPlacemark, height);
                SET_OPT_WPML_ARG_KPN(*curPlacemark, pPlacemark, polygon);
                break;
            }
            case wce::TemplateType::mappingStrip :
            {
                curFolder->placemark = std::make_shared<wcs::WaypointSegmentFlightKMLPlacemark>();
                auto curPlacemark =
                    std::dynamic_pointer_cast<wcs::WaypointSegmentFlightKMLPlacemark>(curFolder->placemark);
                SET_OPT_WPML_ARG_I(*curPlacemark, pPlacemark, caliFlightEnable);
                SET_OPT_WPML_ARG_E(*curPlacemark, pPlacemark, shootType, ShootType);
                SET_OPT_WPML_ARG_I(*curPlacemark, pPlacemark, direction);
                SET_OPT_WPML_ARG_I(*curPlacemark, pPlacemark, margin);
                SET_OPT_WPML_ARG_I(*curPlacemark, pPlacemark, singleLineEnable);
                SET_OPT_WPML_ARG_D(*curPlacemark, pPlacemark, cuttingDistance);
                SET_OPT_WPML_ARG_I(*curPlacemark, pPlacemark, boundaryOptimEnable);
                SET_OPT_WPML_ARG_I(*curPlacemark, pPlacemark, leftExtend);
                SET_OPT_WPML_ARG_I(*curPlacemark, pPlacemark, rightExtend);
                SET_OPT_WPML_ARG_I(*curPlacemark, pPlacemark, includeCenterEnable);
                xml::XMLElement *pOverlap = pPlacemark->FirstChildElement("wpml:overlap");
                if (pOverlap != nullptr)
                {
                    SET_OPT_WPML_ARG_I(curPlacemark->overlap, pOverlap, orthoLidarOverlapH);
                    SET_OPT_WPML_ARG_I(curPlacemark->overlap, pOverlap, orthoLidarOverlapW);
                    SET_OPT_WPML_ARG_I(curPlacemark->overlap, pOverlap, orthoCameraOverlapH);
                    SET_OPT_WPML_ARG_I(curPlacemark->overlap, pOverlap, orthoCameraOverlapW);
                    SET_OPT_WPML_ARG_I(curPlacemark->overlap, pOverlap, inclinedLidarOverlapH);
                    SET_OPT_WPML_ARG_I(curPlacemark->overlap, pOverlap, inclinedLidarOverlapW);
                    SET_OPT_WPML_ARG_I(curPlacemark->overlap, pOverlap, inclinedCameraOverlapH);
                    SET_OPT_WPML_ARG_I(curPlacemark->overlap, pOverlap, inclinedCameraOverlapW);
                }
                SET_OPT_WPML_ARG_D(*curPlacemark, pPlacemark, ellipsoidHeight);
                SET_OPT_WPML_ARG_D(*curPlacemark, pPlacemark, height);
                SET_OPT_WPML_ARG_I(*curPlacemark, pPlacemark, stripUseTemplateAltitude);
                SET_OPT_WPML_ARG_KLS(*curPlacemark, pPlacemark, lineString);
                break;
            }
            default :
                curFolder->placemark = nullptr;
                break;
        }
    }
    return res;
}

bool createKML(const wcs::KMLDocument& data, const std::string& kmlPath)
{
    try
    {
        xml::XMLDocument doc;
        // Step 1: Create xml
        xml::XMLDeclaration *decl = doc.NewDeclaration("xml version=\"1.0\" encoding=\"UTF-8\"");
        doc.InsertFirstChild(decl);
        // Step 2: Create kml Element
        xml::XMLElement *kmlElement = doc.NewElement("kml");
        kmlElement->SetAttribute("xmlns", "http://www.opengis.net/kml/2.2");
        kmlElement->SetAttribute("xmlns:wpml", "http://www.dji.com/wpmz/1.0.6");
        doc.InsertEndChild(kmlElement);
        // Step 3: Create Document Element
        xml::XMLElement *documentElement = doc.NewElement("Document");
        kmlElement->InsertEndChild(documentElement);
        GET_OPT_WPML_ARG_S(doc, documentElement, data, author);
        GET_OPT_WPML_ARG_N(doc, documentElement, data, createTime);
        GET_OPT_WPML_ARG_N(doc, documentElement, data, updateTime);
        // Step 4: Create missionConfig Element
        xml::XMLElement *missionElement = doc.NewElement("wpml:missionConfig");
        documentElement->InsertEndChild(missionElement);
        GET_NEC_WPML_ARG_E(doc, missionElement, data, missionConfig.flyToWaylineMode);
        GET_NEC_WPML_ARG_E(doc, missionElement, data, missionConfig.finishAction);
        GET_NEC_WPML_ARG_E(doc, missionElement, data, missionConfig.exitOnRCLost);
        GET_OPT_WPML_ARG_E(doc, missionElement, data, missionConfig.executeRCLostAction);
        GET_NEC_WPML_ARG_N(doc, missionElement, data, missionConfig.takeOffSecurityHeight);
        GET_NEC_WPML_ARG_N(doc, missionElement, data, missionConfig.globalTransitionalSpeed);
        GET_NEC_WPML_ARG_N(doc, missionElement, data, missionConfig.globalRTHHeight);
        GET_OPT_WPML_ARG_P(doc, missionElement, data, missionConfig.takeOffRefPoint);
        GET_OPT_WPML_ARG_N(doc, missionElement, data, missionConfig.takeOffRefPointAGLHeight);
        xml::XMLElement *droneInfoElement = doc.NewElement("wpml:droneInfo");
        missionElement->InsertEndChild(droneInfoElement);
        GET_NEC_WPML_ARG_N(doc, droneInfoElement, data, missionConfig.droneInfo.droneEnumValue);
        GET_OPT_WPML_ARG_N(doc, droneInfoElement, data, missionConfig.droneInfo.droneSubEnumValue);
        xml::XMLElement *payloadInfoElement = doc.NewElement("wpml:payloadInfo");
        missionElement->InsertEndChild(payloadInfoElement);
        GET_NEC_WPML_ARG_N(doc, payloadInfoElement, data, missionConfig.payloadInfo.payloadEnumValue);
        GET_NEC_WPML_ARG_N(doc, payloadInfoElement, data, missionConfig.payloadInfo.payloadPositionIndex);
        xml::XMLElement *autoRerouteInfoElement = doc.NewElement("wpml:autoRerouteInfo");
        missionElement->InsertEndChild(autoRerouteInfoElement);
        GET_NEC_WPML_ARG_N(doc, autoRerouteInfoElement, data, missionConfig.autoRerouteInfo.missionAutoRerouteMode);
        GET_NEC_WPML_ARG_N(
            doc, autoRerouteInfoElement, data, missionConfig.autoRerouteInfo.transitionalAutoRerouteMode);
        // Step 5: Create Folder Element
        xml::XMLElement *folderElement = doc.NewElement("Folder");
        documentElement->InsertEndChild(folderElement);
        if (data.folder->templateType == wce::TemplateType::waypoint)
        {
            auto curFolder = std::dynamic_pointer_cast<wcs::WaypointFlightKMLFolder>(data.folder);
            GET_NEC_WPML_ARG_E(doc, folderElement, *curFolder, templateType);
            GET_NEC_WPML_ARG_N(doc, folderElement, *curFolder, templateId);
            GET_NEC_WPML_ARG_N(doc, folderElement, *curFolder, autoFlightSpeed);
            GET_NEC_WPML_ARG_E(doc, folderElement, *curFolder, globalWaypointTurnMode);
            GET_OPT_WPML_ARG_N(doc, folderElement, *curFolder, globalUseStraightLine);
            GET_NEC_WPML_ARG_E(doc, folderElement, *curFolder, gimbalPitchMode);
            GET_NEC_WPML_ARG_N(doc, folderElement, *curFolder, globalHeight);
            xml::XMLElement *waylineCoordinateSysParamElement = doc.NewElement("wpml:waylineCoordinateSysParam");
            folderElement->InsertEndChild(waylineCoordinateSysParamElement);
            GET_NEC_WPML_ARG_E(
                doc, waylineCoordinateSysParamElement, curFolder->waylineCoordinateSysParam, coordinateMode);
            GET_NEC_WPML_ARG_E(doc, waylineCoordinateSysParamElement, curFolder->waylineCoordinateSysParam, heightMode);
            GET_OPT_WPML_ARG_E(
                doc, waylineCoordinateSysParamElement, curFolder->waylineCoordinateSysParam, positioningType);
            GET_NEC_WPML_ARG_N(
                doc, waylineCoordinateSysParamElement, curFolder->waylineCoordinateSysParam, globalShootHeight);
            GET_NEC_WPML_ARG_N(
                doc, waylineCoordinateSysParamElement, curFolder->waylineCoordinateSysParam, surfaceFollowModeEnable);
            GET_OPT_WPML_ARG_N(
                doc, waylineCoordinateSysParamElement, curFolder->waylineCoordinateSysParam, surfaceRelativeHeight);
            xml::XMLElement *payloadParamElement = doc.NewElement("wpml:payloadParam");
            folderElement->InsertEndChild(payloadParamElement);
            GET_NEC_WPML_ARG_N(doc, payloadParamElement, curFolder->payloadParam, payloadPositionIndex);
            GET_OPT_WPML_ARG_E(doc, payloadParamElement, curFolder->payloadParam, focusMode);
            GET_OPT_WPML_ARG_E(doc, payloadParamElement, curFolder->payloadParam, meteringMode);
            GET_OPT_WPML_ARG_N(doc, payloadParamElement, curFolder->payloadParam, dewarpingEnable);
            GET_OPT_WPML_ARG_E(doc, payloadParamElement, curFolder->payloadParam, returnMode);
            GET_OPT_WPML_ARG_N(doc, payloadParamElement, curFolder->payloadParam, samplingRate);
            GET_OPT_WPML_ARG_E(doc, payloadParamElement, curFolder->payloadParam, scanningMode);
            GET_OPT_WPML_ARG_N(doc, payloadParamElement, curFolder->payloadParam, modelColoringEnable);
            GET_NEC_WPML_ARG_ES(doc, payloadParamElement, curFolder->payloadParam, imageFormat);
            xml::XMLElement *globalWaypointHeadingParamElement = doc.NewElement("wpml:globalWaypointHeadingParam");
            folderElement->InsertEndChild(globalWaypointHeadingParamElement);
            GET_NEC_WPML_ARG_E(
                doc, globalWaypointHeadingParamElement, curFolder->globalWaypointHeadingParam, waypointHeadingMode);
            GET_OPT_WPML_ARG_N(
                doc, globalWaypointHeadingParamElement, curFolder->globalWaypointHeadingParam, waypointHeadingAngle);
            GET_OPT_WPML_ARG_P(
                doc, globalWaypointHeadingParamElement, curFolder->globalWaypointHeadingParam, waypointPoiPoint);
            GET_NEC_WPML_ARG_E(
                doc, globalWaypointHeadingParamElement, curFolder->globalWaypointHeadingParam, waypointHeadingPathMode);
            // Step 6: Create Waypoint Placemark Element
            for (const auto& pointPlace : curFolder->placemark)
            {
                xml::XMLElement *placemarkElement = doc.NewElement("Placemark");
                folderElement->InsertEndChild(placemarkElement);
                GET_OPT_WPML_ARG_N(doc, placemarkElement, pointPlace, isRisky);
                GET_NEC_WPML_ARG_CPT(doc, placemarkElement, pointPlace, point);
                GET_NEC_WPML_ARG_N(doc, placemarkElement, pointPlace, index);
                GET_NEC_WPML_ARG_N(doc, placemarkElement, pointPlace, useGlobalHeight);
                GET_NEC_WPML_ARG_N(doc, placemarkElement, pointPlace, ellipsoidHeight);
                GET_NEC_WPML_ARG_N(doc, placemarkElement, pointPlace, height);
                GET_NEC_WPML_ARG_N(doc, placemarkElement, pointPlace, useGlobalSpeed);
                GET_NEC_WPML_ARG_N(doc, placemarkElement, pointPlace, waypointSpeed);
                GET_NEC_WPML_ARG_N(doc, placemarkElement, pointPlace, useGlobalHeadingParam);
                GET_NEC_WPML_ARG_N(doc, placemarkElement, pointPlace, useGlobalTurnParam);
                GET_NEC_WPML_ARG_N(doc, placemarkElement, pointPlace, useStraightLine);
                GET_NEC_WPML_ARG_N(doc, placemarkElement, pointPlace, gimbalPitchAngle);
                xml::XMLElement *waypointHeadingParamElement = doc.NewElement("wpml:waypointHeadingParam");
                placemarkElement->InsertEndChild(waypointHeadingParamElement);
                GET_NEC_WPML_ARG_E(
                    doc, waypointHeadingParamElement, pointPlace.waypointHeadingParam, waypointHeadingMode);
                GET_OPT_WPML_ARG_N(
                    doc, waypointHeadingParamElement, pointPlace.waypointHeadingParam, waypointHeadingAngle);
                GET_OPT_WPML_ARG_P(doc, waypointHeadingParamElement, pointPlace.waypointHeadingParam, waypointPoiPoint);
                GET_NEC_WPML_ARG_E(
                    doc, waypointHeadingParamElement, pointPlace.waypointHeadingParam, waypointHeadingPathMode);
                xml::XMLElement *waypointTurnParamElement = doc.NewElement("wpml:waypointTurnParam");
                placemarkElement->InsertEndChild(waypointTurnParamElement);
                GET_NEC_WPML_ARG_E(doc, waypointTurnParamElement, pointPlace.waypointTurnParam, waypointTurnMode);
                GET_OPT_WPML_ARG_N(
                    doc, waypointTurnParamElement, pointPlace.waypointTurnParam, waypointTurnDampingDist);
            }
        }
        else
        {
            auto curFolder = std::dynamic_pointer_cast<wcs::KMLFolder>(data.folder);
            GET_NEC_WPML_ARG_E(doc, folderElement, *curFolder, templateType);
            GET_NEC_WPML_ARG_N(doc, folderElement, *curFolder, templateId);
            GET_NEC_WPML_ARG_N(doc, folderElement, *curFolder, autoFlightSpeed);
            xml::XMLElement *waylineCoordinateSysParamElement = doc.NewElement("wpml:waylineCoordinateSysParam");
            folderElement->InsertEndChild(waylineCoordinateSysParamElement);
            GET_NEC_WPML_ARG_E(
                doc, waylineCoordinateSysParamElement, curFolder->waylineCoordinateSysParam, coordinateMode);
            GET_NEC_WPML_ARG_E(doc, waylineCoordinateSysParamElement, curFolder->waylineCoordinateSysParam, heightMode);
            GET_OPT_WPML_ARG_E(
                doc, waylineCoordinateSysParamElement, curFolder->waylineCoordinateSysParam, positioningType);
            GET_NEC_WPML_ARG_N(
                doc, waylineCoordinateSysParamElement, curFolder->waylineCoordinateSysParam, globalShootHeight);
            GET_NEC_WPML_ARG_N(
                doc, waylineCoordinateSysParamElement, curFolder->waylineCoordinateSysParam, surfaceFollowModeEnable);
            GET_OPT_WPML_ARG_N(
                doc, waylineCoordinateSysParamElement, curFolder->waylineCoordinateSysParam, surfaceRelativeHeight);
            xml::XMLElement *payloadParamElement = doc.NewElement("wpml:payloadParam");
            folderElement->InsertEndChild(payloadParamElement);
            GET_NEC_WPML_ARG_N(doc, payloadParamElement, curFolder->payloadParam, payloadPositionIndex);
            GET_OPT_WPML_ARG_E(doc, payloadParamElement, curFolder->payloadParam, focusMode);
            GET_OPT_WPML_ARG_E(doc, payloadParamElement, curFolder->payloadParam, meteringMode);
            GET_OPT_WPML_ARG_N(doc, payloadParamElement, curFolder->payloadParam, dewarpingEnable);
            GET_OPT_WPML_ARG_E(doc, payloadParamElement, curFolder->payloadParam, returnMode);
            GET_OPT_WPML_ARG_N(doc, payloadParamElement, curFolder->payloadParam, samplingRate);
            GET_OPT_WPML_ARG_E(doc, payloadParamElement, curFolder->payloadParam, scanningMode);
            GET_OPT_WPML_ARG_N(doc, payloadParamElement, curFolder->payloadParam, modelColoringEnable);
            GET_NEC_WPML_ARG_ES(doc, payloadParamElement, curFolder->payloadParam, imageFormat);
            // Step 6: Create Placemark Element
            xml::XMLElement *placemarkElement = doc.NewElement("Placemark");
            folderElement->InsertEndChild(placemarkElement);
            switch (data.folder->templateType)
            {
                case wce::TemplateType::mapping2d :
                {
                    auto curPlacemark = std::dynamic_pointer_cast<wcs::MappingAerialKMLPlacemark>(curFolder->placemark);
                    GET_OPT_WPML_ARG_N(doc, placemarkElement, *curPlacemark, caliFlightEnable);
                    GET_NEC_WPML_ARG_N(doc, placemarkElement, *curPlacemark, elevationOptimizeEnable);
                    GET_OPT_WPML_ARG_N(doc, placemarkElement, *curPlacemark, smartObliqueEnable);
                    GET_OPT_WPML_ARG_N(doc, placemarkElement, *curPlacemark, smartObliqueGimbalPitch);
                    GET_NEC_WPML_ARG_E(doc, placemarkElement, *curPlacemark, shootType);
                    GET_NEC_WPML_ARG_N(doc, placemarkElement, *curPlacemark, direction);
                    GET_NEC_WPML_ARG_N(doc, placemarkElement, *curPlacemark, margin);
                    GET_NEC_WPML_ARG_N(doc, placemarkElement, *curPlacemark, ellipsoidHeight);
                    GET_NEC_WPML_ARG_N(doc, placemarkElement, *curPlacemark, height);
                    GET_OPT_WPML_ARG_N(doc, placemarkElement, *curPlacemark, facadeWaylineEnable);
                    GET_NEC_WPML_ARG_KPN(doc, placemarkElement, *curPlacemark, polygon);
                    GET_OPT_WPML_ARG_E(doc, placemarkElement, *curPlacemark, gimbalPitchMode);
                    GET_OPT_WPML_ARG_N(doc, placemarkElement, *curPlacemark, gimbalPitchAngle);
                    GET_OPT_WPML_ARG_N(doc, placemarkElement, *curPlacemark, quickOrthoMappingEnable);
                    GET_OPT_WPML_ARG_N(doc, placemarkElement, *curPlacemark, quickOrthoMappingPitch);
                    xml::XMLElement *overlapElement = doc.NewElement("wpml:overlap");
                    placemarkElement->InsertEndChild(overlapElement);
                    GET_OPT_WPML_ARG_N(doc, overlapElement, curPlacemark->overlap, orthoLidarOverlapH);
                    GET_OPT_WPML_ARG_N(doc, overlapElement, curPlacemark->overlap, orthoLidarOverlapW);
                    GET_OPT_WPML_ARG_N(doc, overlapElement, curPlacemark->overlap, orthoCameraOverlapH);
                    GET_OPT_WPML_ARG_N(doc, overlapElement, curPlacemark->overlap, orthoCameraOverlapW);
                    GET_OPT_WPML_ARG_N(doc, overlapElement, curPlacemark->overlap, inclinedLidarOverlapH);
                    GET_OPT_WPML_ARG_N(doc, overlapElement, curPlacemark->overlap, inclinedLidarOverlapW);
                    GET_OPT_WPML_ARG_N(doc, overlapElement, curPlacemark->overlap, inclinedCameraOverlapH);
                    GET_OPT_WPML_ARG_N(doc, overlapElement, curPlacemark->overlap, inclinedCameraOverlapW);
                    xml::XMLElement *mappingHeadingParamElement = doc.NewElement("wpml:mappingHeadingParam");
                    placemarkElement->InsertEndChild(mappingHeadingParamElement);
                    GET_OPT_WPML_ARG_E(
                        doc, mappingHeadingParamElement, curPlacemark->mappingHeadingParam, mappingHeadingMode);
                    GET_OPT_WPML_ARG_N(
                        doc, mappingHeadingParamElement, curPlacemark->mappingHeadingParam, mappingHeadingAngle);
                    break;
                }
                case wce::TemplateType::mapping3d :
                {
                    auto curPlacemark =
                        std::dynamic_pointer_cast<wcs::ObliquePhotographyKMLPlacemark>(curFolder->placemark);
                    GET_OPT_WPML_ARG_N(doc, placemarkElement, *curPlacemark, caliFlightEnable);
                    GET_NEC_WPML_ARG_N(doc, placemarkElement, *curPlacemark, inclinedGimbalPitch);
                    GET_NEC_WPML_ARG_N(doc, placemarkElement, *curPlacemark, inclinedFlightSpeed);
                    GET_NEC_WPML_ARG_E(doc, placemarkElement, *curPlacemark, shootType);
                    GET_NEC_WPML_ARG_N(doc, placemarkElement, *curPlacemark, direction);
                    GET_NEC_WPML_ARG_N(doc, placemarkElement, *curPlacemark, margin);
                    GET_NEC_WPML_ARG_N(doc, placemarkElement, *curPlacemark, ellipsoidHeight);
                    GET_NEC_WPML_ARG_N(doc, placemarkElement, *curPlacemark, height);
                    GET_NEC_WPML_ARG_KPN(doc, placemarkElement, *curPlacemark, polygon);
                    xml::XMLElement *overlapElement = doc.NewElement("wpml:overlap");
                    placemarkElement->InsertEndChild(overlapElement);
                    GET_OPT_WPML_ARG_N(doc, overlapElement, curPlacemark->overlap, orthoLidarOverlapH);
                    GET_OPT_WPML_ARG_N(doc, overlapElement, curPlacemark->overlap, orthoLidarOverlapW);
                    GET_OPT_WPML_ARG_N(doc, overlapElement, curPlacemark->overlap, orthoCameraOverlapH);
                    GET_OPT_WPML_ARG_N(doc, overlapElement, curPlacemark->overlap, orthoCameraOverlapW);
                    GET_OPT_WPML_ARG_N(doc, overlapElement, curPlacemark->overlap, inclinedLidarOverlapH);
                    GET_OPT_WPML_ARG_N(doc, overlapElement, curPlacemark->overlap, inclinedLidarOverlapW);
                    GET_OPT_WPML_ARG_N(doc, overlapElement, curPlacemark->overlap, inclinedCameraOverlapH);
                    GET_OPT_WPML_ARG_N(doc, overlapElement, curPlacemark->overlap, inclinedCameraOverlapW);
                    break;
                }
                case wce::TemplateType::mappingStrip :
                {
                    auto curPlacemark =
                        std::dynamic_pointer_cast<wcs::WaypointSegmentFlightKMLPlacemark>(curFolder->placemark);
                    GET_NEC_WPML_ARG_N(doc, placemarkElement, *curPlacemark, caliFlightEnable);
                    GET_NEC_WPML_ARG_E(doc, placemarkElement, *curPlacemark, shootType);
                    GET_NEC_WPML_ARG_N(doc, placemarkElement, *curPlacemark, direction);
                    GET_NEC_WPML_ARG_N(doc, placemarkElement, *curPlacemark, margin);
                    GET_NEC_WPML_ARG_N(doc, placemarkElement, *curPlacemark, singleLineEnable);
                    GET_NEC_WPML_ARG_N(doc, placemarkElement, *curPlacemark, cuttingDistance);
                    GET_NEC_WPML_ARG_N(doc, placemarkElement, *curPlacemark, boundaryOptimEnable);
                    GET_NEC_WPML_ARG_N(doc, placemarkElement, *curPlacemark, leftExtend);
                    GET_NEC_WPML_ARG_N(doc, placemarkElement, *curPlacemark, rightExtend);
                    GET_NEC_WPML_ARG_N(doc, placemarkElement, *curPlacemark, includeCenterEnable);
                    GET_NEC_WPML_ARG_N(doc, placemarkElement, *curPlacemark, ellipsoidHeight);
                    GET_NEC_WPML_ARG_N(doc, placemarkElement, *curPlacemark, height);
                    GET_NEC_WPML_ARG_N(doc, placemarkElement, *curPlacemark, stripUseTemplateAltitude);
                    GET_NEC_WPML_ARG_KLS(doc, placemarkElement, *curPlacemark, lineString);
                    xml::XMLElement *overlapElement = doc.NewElement("wpml:overlap");
                    placemarkElement->InsertEndChild(overlapElement);
                    GET_OPT_WPML_ARG_N(doc, overlapElement, curPlacemark->overlap, orthoLidarOverlapH);
                    GET_OPT_WPML_ARG_N(doc, overlapElement, curPlacemark->overlap, orthoLidarOverlapW);
                    GET_OPT_WPML_ARG_N(doc, overlapElement, curPlacemark->overlap, orthoCameraOverlapH);
                    GET_OPT_WPML_ARG_N(doc, overlapElement, curPlacemark->overlap, orthoCameraOverlapW);
                    GET_OPT_WPML_ARG_N(doc, overlapElement, curPlacemark->overlap, inclinedLidarOverlapH);
                    GET_OPT_WPML_ARG_N(doc, overlapElement, curPlacemark->overlap, inclinedLidarOverlapW);
                    GET_OPT_WPML_ARG_N(doc, overlapElement, curPlacemark->overlap, inclinedCameraOverlapH);
                    GET_OPT_WPML_ARG_N(doc, overlapElement, curPlacemark->overlap, inclinedCameraOverlapW);
                    break;
                }
                default :
                    curFolder->placemark = nullptr;
                    break;
            }
        }
        // Save file
        doc.SaveFile(kmlPath.c_str());
        return true;
    }
    catch (...)
    {
        return false;
    }
}

boost::optional<wcs::WPMLDocument> parseWPML(const std::string& wpmlPath)
{
    wcs::WPMLDocument res;
    xml::XMLDocument doc;
    xml::XMLError eResult = doc.LoadFile(wpmlPath.c_str());
    if (eResult != xml::XML_SUCCESS)
    {
        std::cerr << "Failed to load wpml file: " << wpmlPath << std::endl;
        return boost::none;
    }
    // Parse KML
    // Step 1: Implement File Creation Information
    xml::XMLElement *pDocument = doc.FirstChildElement("kml")->FirstChildElement("Document");
    if (pDocument == nullptr)
    {
        std::cerr << "No Document element found" << std::endl;
        return boost::none;
    }
    // Step 2: Setup Mission Configuration
    xml::XMLElement *pMissionConfig = pDocument->FirstChildElement("wpml:missionConfig");
    if (pMissionConfig == nullptr)
    {
        std::cerr << "No wpml:missionConfig element found" << std::endl;
        return boost::none;
    }
    SET_OPT_WPML_ARG_E(res.missionConfig, pMissionConfig, flyToWaylineMode, FlyToWaylineMode);
    SET_OPT_WPML_ARG_E(res.missionConfig, pMissionConfig, finishAction, FinishAction);
    SET_OPT_WPML_ARG_E(res.missionConfig, pMissionConfig, exitOnRCLost, ExitOnRCLost);
    SET_OPT_WPML_ARG_E(res.missionConfig, pMissionConfig, executeRCLostAction, ExecuteRCLostAction);
    SET_OPT_WPML_ARG_D(res.missionConfig, pMissionConfig, takeOffSecurityHeight);
    SET_OPT_WPML_ARG_D(res.missionConfig, pMissionConfig, globalTransitionalSpeed);
    SET_OPT_WPML_ARG_D(res.missionConfig, pMissionConfig, globalRTHHeight);
    xml::XMLElement *pDroneInfo = pMissionConfig->FirstChildElement("wpml:droneInfo");
    if (pDroneInfo != nullptr)
    {
        SET_OPT_WPML_ARG_I(res.missionConfig.droneInfo, pDroneInfo, droneEnumValue);
        SET_OPT_WPML_ARG_I(res.missionConfig.droneInfo, pDroneInfo, droneSubEnumValue);
    }
    xml::XMLElement *pPayloadInfo = pMissionConfig->FirstChildElement("wpml:payloadInfo");
    if (pPayloadInfo != nullptr)
    {
        SET_OPT_WPML_ARG_I(res.missionConfig.payloadInfo, pPayloadInfo, payloadEnumValue);
        SET_OPT_WPML_ARG_I(res.missionConfig.payloadInfo, pPayloadInfo, payloadPositionIndex);
    }
    xml::XMLElement *pAutoRerouteInfo = pMissionConfig->FirstChildElement("wpml:autoRerouteInfo");
    if (pAutoRerouteInfo != nullptr)
    {
        SET_OPT_WPML_ARG_I(res.missionConfig.autoRerouteInfo, pAutoRerouteInfo, missionAutoRerouteMode);
        SET_OPT_WPML_ARG_I(res.missionConfig.autoRerouteInfo, pAutoRerouteInfo, transitionalAutoRerouteMode);
    }
    // Step 3: Setup A Folder for Waypoint Template
    std::vector<wcs::WPMLFolder> folders{};
    xml::XMLElement *pFolder = pDocument->FirstChildElement("Folder");
    while (pFolder)
    {
        wcs::WPMLFolder tmpFolder;
        SET_OPT_WPML_ARG_I(tmpFolder, pFolder, templateId);
        SET_OPT_WPML_ARG_I(tmpFolder, pFolder, waylineId);
        SET_OPT_WPML_ARG_D(tmpFolder, pFolder, autoFlightSpeed);
        SET_OPT_WPML_ARG_E(tmpFolder, pFolder, executeHeightMode, ExecuteHeightMode);
        // Step 4: Setup Waypoint Placemark
        std::vector<wcs::WPMLPlacemark> waypointPlacemarks{};
        xml::XMLElement *pPlacemark = pFolder->FirstChildElement("Placemark");
        while (pPlacemark)
        {
            wcs::WPMLPlacemark tmpPlacemark;
            SET_OPT_WPML_ARG_I(tmpPlacemark, pPlacemark, isRisky);
			SET_OPT_WPML_ARG_CPT(tmpPlacemark, pPlacemark, point);
            SET_OPT_WPML_ARG_I(tmpPlacemark, pPlacemark, index);
            SET_OPT_WPML_ARG_D(tmpPlacemark, pPlacemark, executeHeight);
            SET_OPT_WPML_ARG_D(tmpPlacemark, pPlacemark, waypointSpeed);
            xml::XMLElement *pWaypointHeadingParam = pPlacemark->FirstChildElement("wpml:waypointHeadingParam");
            if (pWaypointHeadingParam != nullptr)
            {
                SET_OPT_WPML_ARG_E(
                    tmpPlacemark.waypointHeadingParam, pWaypointHeadingParam, waypointHeadingMode, WaypointHeadingMode);
                SET_OPT_WPML_ARG_D(tmpPlacemark.waypointHeadingParam, pWaypointHeadingParam, waypointHeadingAngle);
                SET_OPT_WPML_ARG_P(tmpPlacemark.waypointHeadingParam, pWaypointHeadingParam, waypointPoiPoint);
                SET_OPT_WPML_ARG_E(tmpPlacemark.waypointHeadingParam,
                                   pWaypointHeadingParam,
                                   waypointHeadingPathMode,
                                   WaypointHeadingPathMode);
            }
            xml::XMLElement *pWaypointTurnParam = pPlacemark->FirstChildElement("wpml:waypointTurnParam");
            if (pWaypointTurnParam != nullptr)
            {
                SET_OPT_WPML_ARG_E(
                    tmpPlacemark.waypointTurnParam, pWaypointTurnParam, waypointTurnMode, WaypointTurnMode);
                SET_OPT_WPML_ARG_D(tmpPlacemark.waypointTurnParam, pWaypointTurnParam, waypointTurnDampingDist);
            }
            SET_OPT_WPML_ARG_I(tmpPlacemark, pPlacemark, useStraightLine);
            xml::XMLElement *pActionGroup = pPlacemark->FirstChildElement("wpml:actionGroup");
            if (pActionGroup != nullptr)
            {
                wcs::ActionGroup tmpActionGroup;
                SET_OPT_WPML_ARG_I(tmpActionGroup, pActionGroup, actionGroupId);
                SET_OPT_WPML_ARG_I(tmpActionGroup, pActionGroup, actionGroupStartIndex);
                SET_OPT_WPML_ARG_I(tmpActionGroup, pActionGroup, actionGroupEndIndex);
                SET_OPT_WPML_ARG_E(tmpActionGroup, pActionGroup, actionGroupMode, ActionGroupMode);
                xml::XMLElement *pActionTrigger = pActionGroup->FirstChildElement("wpml:actionTrigger");
                if (pActionTrigger != nullptr)
                {
                    SET_OPT_WPML_ARG_E(
                        tmpActionGroup.actionTrigger, pActionTrigger, actionTriggerType, ActionTriggerType);
                    SET_OPT_WPML_ARG_D(tmpActionGroup.actionTrigger, pActionTrigger, actionTriggerParam);
                }
                std::vector<wcs::Action> actions{};
                xml::XMLElement *pAction = pActionGroup->FirstChildElement("wpml:action");
                while (pAction)
                {
                    wcs::Action tmpAction;
                    SET_OPT_WPML_ARG_I(tmpAction, pAction, actionId);
                    SET_OPT_WPML_ARG_E(tmpAction, pAction, actionActuatorFunc, ActionActuatorFunc);
                    xml::XMLElement *pAFP = pAction->FirstChildElement("wpml:actionActuatorFuncParam");
                    if (pAFP != nullptr)
                    {
                        switch (tmpAction.actionActuatorFunc)
                        {
                            case wce::ActionActuatorFunc::takePhoto :
                            {
                                tmpAction.actionActuatorFuncParam = std::make_shared<wcs::TakePhotoParam>();
                                auto curAFP =
                                    std::dynamic_pointer_cast<wcs::TakePhotoParam>(tmpAction.actionActuatorFuncParam);
                                SET_OPT_WPML_ARG_I(*curAFP, pAFP, payloadPositionIndex);
                                SET_OPT_WPML_ARG_S(*curAFP, pAFP, fileSuffix);
                                SET_OPT_WPML_ARG_ES(*curAFP, pAFP, payloadLensIndex, ImageFormat);
                                SET_OPT_WPML_ARG_I(*curAFP, pAFP, useGlobalPayloadLensIndex);
                                break;
                            }
                            case wce::ActionActuatorFunc::startRecord :
                            {
                                tmpAction.actionActuatorFuncParam = std::make_shared<wcs::StartRecordParam>();
                                auto curAFP =
                                    std::dynamic_pointer_cast<wcs::StartRecordParam>(tmpAction.actionActuatorFuncParam);
                                SET_OPT_WPML_ARG_I(*curAFP, pAFP, payloadPositionIndex);
                                SET_OPT_WPML_ARG_S(*curAFP, pAFP, fileSuffix);
                                SET_OPT_WPML_ARG_ES(*curAFP, pAFP, payloadLensIndex, ImageFormat);
                                SET_OPT_WPML_ARG_I(*curAFP, pAFP, useGlobalPayloadLensIndex);
                                break;
                            }
                            case wce::ActionActuatorFunc::stopRecord :
                            {
                                tmpAction.actionActuatorFuncParam = std::make_shared<wcs::StopRecordParam>();
                                auto curAFP =
                                    std::dynamic_pointer_cast<wcs::StopRecordParam>(tmpAction.actionActuatorFuncParam);
                                SET_OPT_WPML_ARG_I(*curAFP, pAFP, payloadPositionIndex);
                                SET_OPT_WPML_ARG_ES(*curAFP, pAFP, payloadLensIndex, ImageFormat);
                                break;
                            }
                            case wce::ActionActuatorFunc::focus :
                            {
                                tmpAction.actionActuatorFuncParam = std::make_shared<wcs::FocusParam>();
                                auto curAFP =
                                    std::dynamic_pointer_cast<wcs::FocusParam>(tmpAction.actionActuatorFuncParam);
                                SET_OPT_WPML_ARG_I(*curAFP, pAFP, payloadPositionIndex);
                                SET_OPT_WPML_ARG_I(*curAFP, pAFP, isPointFocus);
                                SET_OPT_WPML_ARG_D(*curAFP, pAFP, focusX);
                                SET_OPT_WPML_ARG_D(*curAFP, pAFP, focusY);
                                SET_OPT_WPML_ARG_D(*curAFP, pAFP, focusRegionWidth);
                                SET_OPT_WPML_ARG_D(*curAFP, pAFP, focusRegionHeight);
                                SET_OPT_WPML_ARG_I(*curAFP, pAFP, isInfiniteFocus);
                                break;
                            }
                            case wce::ActionActuatorFunc::zoom :
                            {
                                tmpAction.actionActuatorFuncParam = std::make_shared<wcs::ZoomParam>();
                                auto curAFP =
                                    std::dynamic_pointer_cast<wcs::ZoomParam>(tmpAction.actionActuatorFuncParam);
                                SET_OPT_WPML_ARG_I(*curAFP, pAFP, payloadPositionIndex);
                                SET_OPT_WPML_ARG_D(*curAFP, pAFP, focalLength);
                                break;
                            }
                            case wce::ActionActuatorFunc::customDirName :
                            {
                                tmpAction.actionActuatorFuncParam = std::make_shared<wcs::CustomDirNameParam>();
                                auto curAFP = std::dynamic_pointer_cast<wcs::CustomDirNameParam>(
                                    tmpAction.actionActuatorFuncParam);
                                SET_OPT_WPML_ARG_I(*curAFP, pAFP, payloadPositionIndex);
                                SET_OPT_WPML_ARG_S(*curAFP, pAFP, directoryName);
                                break;
                            }
                            case wce::ActionActuatorFunc::gimbalRotate :
                            {
                                tmpAction.actionActuatorFuncParam = std::make_shared<wcs::GimbalRotateParam>();
                                auto curAFP = std::dynamic_pointer_cast<wcs::GimbalRotateParam>(
                                    tmpAction.actionActuatorFuncParam);
                                SET_OPT_WPML_ARG_I(*curAFP, pAFP, payloadPositionIndex);
                                SET_OPT_WPML_ARG_E(*curAFP, pAFP, gimbalHeadingYawBase, GimbalHeadingYawBase);
                                SET_OPT_WPML_ARG_E(*curAFP, pAFP, gimbalRotateMode, GimbalRotateMode);
                                SET_OPT_WPML_ARG_I(*curAFP, pAFP, gimbalPitchRotateEnable);
                                SET_OPT_WPML_ARG_D(*curAFP, pAFP, gimbalPitchRotateAngle);
                                SET_OPT_WPML_ARG_I(*curAFP, pAFP, gimbalRollRotateEnable);
                                SET_OPT_WPML_ARG_D(*curAFP, pAFP, gimbalRollRotateAngle);
                                SET_OPT_WPML_ARG_I(*curAFP, pAFP, gimbalYawRotateEnable);
                                SET_OPT_WPML_ARG_D(*curAFP, pAFP, gimbalYawRotateAngle);
                                SET_OPT_WPML_ARG_I(*curAFP, pAFP, gimbalRotateTimeEnable);
                                SET_OPT_WPML_ARG_D(*curAFP, pAFP, gimbalRotateTime);
                                break;
                            }
                            case wce::ActionActuatorFunc::rotateYaw :
                            {
                                tmpAction.actionActuatorFuncParam = std::make_shared<wcs::RotateYawParam>();
                                auto curAFP =
                                    std::dynamic_pointer_cast<wcs::RotateYawParam>(tmpAction.actionActuatorFuncParam);
                                SET_OPT_WPML_ARG_D(*curAFP, pAFP, aircraftHeading);
                                SET_OPT_WPML_ARG_E(*curAFP, pAFP, aircraftPathMode, AircraftPathMode);
                                break;
                            }
                            case wce::ActionActuatorFunc::hover :
                            {
                                tmpAction.actionActuatorFuncParam = std::make_shared<wcs::HoverParam>();
                                auto curAFP =
                                    std::dynamic_pointer_cast<wcs::HoverParam>(tmpAction.actionActuatorFuncParam);
                                SET_OPT_WPML_ARG_D(*curAFP, pAFP, hoverTime);
                                break;
                            }
                            case wce::ActionActuatorFunc::gimbalEvenlyRotate :
                            {
                                tmpAction.actionActuatorFuncParam = std::make_shared<wcs::GimbalEvenlyRotateParam>();
                                auto curAFP = std::dynamic_pointer_cast<wcs::GimbalEvenlyRotateParam>(
                                    tmpAction.actionActuatorFuncParam);
                                SET_OPT_WPML_ARG_I(*curAFP, pAFP, payloadPositionIndex);
                                SET_OPT_WPML_ARG_D(*curAFP, pAFP, gimbalPitchRotateAngle);
                                break;
                            }
                            case wce::ActionActuatorFunc::accurateShoot :
                            {
                                tmpAction.actionActuatorFuncParam = std::make_shared<wcs::AccurateShootParam>();
                                auto curAFP = std::dynamic_pointer_cast<wcs::AccurateShootParam>(
                                    tmpAction.actionActuatorFuncParam);
                                SET_OPT_WPML_ARG_D(*curAFP, pAFP, gimbalPitchRotateAngle);
                                SET_OPT_WPML_ARG_D(*curAFP, pAFP, gimbalYawRotateAngle);
                                SET_OPT_WPML_ARG_I(*curAFP, pAFP, focusX);
                                SET_OPT_WPML_ARG_I(*curAFP, pAFP, focusY);
                                SET_OPT_WPML_ARG_I(*curAFP, pAFP, focusRegionWidth);
                                SET_OPT_WPML_ARG_I(*curAFP, pAFP, focusRegionHeight);
                                SET_OPT_WPML_ARG_D(*curAFP, pAFP, focalLength);
                                SET_OPT_WPML_ARG_D(*curAFP, pAFP, aircraftHeading);
                                SET_OPT_WPML_ARG_I(*curAFP, pAFP, accurateFrameValid);
                                SET_OPT_WPML_ARG_I(*curAFP, pAFP, payloadPositionIndex);
                                SET_OPT_WPML_ARG_ES(*curAFP, pAFP, payloadLensIndex, ImageFormat);
                                SET_OPT_WPML_ARG_I(*curAFP, pAFP, useGlobalPayloadLensIndex);
                                SET_OPT_WPML_ARG_D(*curAFP, pAFP, targetAngle);
                                SET_OPT_WPML_ARG_I(*curAFP, pAFP, imageWidth);
                                SET_OPT_WPML_ARG_I(*curAFP, pAFP, imageHeight);
                                SET_OPT_WPML_ARG_I(*curAFP, pAFP, AFPos);
                                SET_OPT_WPML_ARG_I(*curAFP, pAFP, gimbalPort);
                                SET_OPT_WPML_ARG_I(*curAFP, pAFP, accurateCameraType);
                                SET_OPT_WPML_ARG_S(*curAFP, pAFP, accurateFilePath);
                                SET_OPT_WPML_ARG_S(*curAFP, pAFP, accurateFileMD5);
                                SET_OPT_WPML_ARG_I(*curAFP, pAFP, accurateFileSize);
                                SET_OPT_WPML_ARG_S(*curAFP, pAFP, accurateFileSuffix);
                                SET_OPT_WPML_ARG_I(*curAFP, pAFP, accurateCameraApertue);
                                SET_OPT_WPML_ARG_I(*curAFP, pAFP, accurateCameraLuminance);
                                SET_OPT_WPML_ARG_D(*curAFP, pAFP, accurateCameraShutterTime);
                                SET_OPT_WPML_ARG_I(*curAFP, pAFP, accurateCameraISO);
                                break;
                            }
                            case wce::ActionActuatorFunc::orientedShoot :
                            {
                                tmpAction.actionActuatorFuncParam = std::make_shared<wcs::OrientedShootParams>();
                                auto curAFP = std::dynamic_pointer_cast<wcs::OrientedShootParams>(
                                    tmpAction.actionActuatorFuncParam);
                                SET_OPT_WPML_ARG_D(*curAFP, pAFP, gimbalPitchRotateAngle);
                                SET_OPT_WPML_ARG_D(*curAFP, pAFP, gimbalYawRotateAngle);
                                SET_OPT_WPML_ARG_I(*curAFP, pAFP, focusX);
                                SET_OPT_WPML_ARG_I(*curAFP, pAFP, focusY);
                                SET_OPT_WPML_ARG_I(*curAFP, pAFP, focusRegionWidth);
                                SET_OPT_WPML_ARG_I(*curAFP, pAFP, focusRegionHeight);
                                SET_OPT_WPML_ARG_D(*curAFP, pAFP, focalLength);
                                SET_OPT_WPML_ARG_D(*curAFP, pAFP, aircraftHeading);
                                SET_OPT_WPML_ARG_I(*curAFP, pAFP, accurateFrameValid);
                                SET_OPT_WPML_ARG_I(*curAFP, pAFP, payloadPositionIndex);
                                SET_OPT_WPML_ARG_ES(*curAFP, pAFP, payloadLensIndex, ImageFormat);
                                SET_OPT_WPML_ARG_I(*curAFP, pAFP, useGlobalPayloadLensIndex);
                                SET_OPT_WPML_ARG_D(*curAFP, pAFP, targetAngle);
                                SET_OPT_WPML_ARG_S(*curAFP, pAFP, actionUUID);
                                SET_OPT_WPML_ARG_I(*curAFP, pAFP, imageWidth);
                                SET_OPT_WPML_ARG_I(*curAFP, pAFP, imageHeight);
                                SET_OPT_WPML_ARG_I(*curAFP, pAFP, AFPos);
                                SET_OPT_WPML_ARG_I(*curAFP, pAFP, gimbalPort);
                                SET_OPT_WPML_ARG_I(*curAFP, pAFP, orientedCameraType);
                                SET_OPT_WPML_ARG_S(*curAFP, pAFP, orientedFilePath);
                                SET_OPT_WPML_ARG_S(*curAFP, pAFP, orientedFileMD5);
                                SET_OPT_WPML_ARG_I(*curAFP, pAFP, orientedFileSize);
                                SET_OPT_WPML_ARG_S(*curAFP, pAFP, orientedFileSuffix);
                                SET_OPT_WPML_ARG_I(*curAFP, pAFP, orientedCameraApertue);
                                SET_OPT_WPML_ARG_I(*curAFP, pAFP, orientedCameraLuminance);
                                SET_OPT_WPML_ARG_D(*curAFP, pAFP, orientedCameraShutterTime);
                                SET_OPT_WPML_ARG_I(*curAFP, pAFP, orientedCameraISO);
                                SET_OPT_WPML_ARG_E(*curAFP, pAFP, orientedPhotoMode, OrientedPhotoMode);
                                break;
                            }
                            case wce::ActionActuatorFunc::panoShot :
                            {
                                tmpAction.actionActuatorFuncParam = std::make_shared<wcs::PanoShotParams>();
                                auto curAFP =
                                    std::dynamic_pointer_cast<wcs::PanoShotParams>(tmpAction.actionActuatorFuncParam);
                                SET_OPT_WPML_ARG_I(*curAFP, pAFP, payloadPositionIndex);
                                SET_OPT_WPML_ARG_ES(*curAFP, pAFP, payloadLensIndex, ImageFormat);
                                SET_OPT_WPML_ARG_I(*curAFP, pAFP, useGlobalPayloadLensIndex);
                                SET_OPT_WPML_ARG_E(*curAFP, pAFP, panoShotSubMode, PanoShotSubMode);
                                break;
                            }
                            case wce::ActionActuatorFunc::recordPointCloud :
                            {
                                tmpAction.actionActuatorFuncParam = std::make_shared<wcs::RecordPointCloudParams>();
                                auto curAFP = std::dynamic_pointer_cast<wcs::RecordPointCloudParams>(
                                    tmpAction.actionActuatorFuncParam);
                                SET_OPT_WPML_ARG_I(*curAFP, pAFP, payloadPositionIndex);
                                SET_OPT_WPML_ARG_E(*curAFP, pAFP, recordPointCloudOperate, RecordPointCloudOperate);
                                break;
                            }
                            default :
                                break;
                        }
                        actions.emplace_back(tmpAction);
                    }
                    pAction = pAction->NextSiblingElement("wpml:action");
                }
                tmpActionGroup.action = std::move(actions);
                tmpPlacemark.startActionGroup = std::move(tmpActionGroup);
            }
            waypointPlacemarks.emplace_back(tmpPlacemark);
            pPlacemark = pPlacemark->NextSiblingElement("Placemark");
        }
        tmpFolder.placemark = std::move(waypointPlacemarks);
        folders.emplace_back(tmpFolder);
        pFolder = pFolder->NextSiblingElement("Folder");
    }
    res.folder = std::move(folders);
    return res;
}

bool createWPML(const wcs::WPMLDocument& data, const std::string& wpmlPath)
{
    try
    {
        xml::XMLDocument doc;
        // Step 1: Create xml
        xml::XMLDeclaration *decl = doc.NewDeclaration("xml version=\"1.0\" encoding=\"UTF-8\"");
        doc.InsertFirstChild(decl);
        // Step 2: Create kml Element
        xml::XMLElement *kmlElement = doc.NewElement("kml");
        kmlElement->SetAttribute("xmlns", "http://www.opengis.net/kml/2.2");
        kmlElement->SetAttribute("xmlns:wpml", "http://www.dji.com/wpmz/1.0.6");
        doc.InsertEndChild(kmlElement);
        // Step 3: Create Document Element
        xml::XMLElement *documentElement = doc.NewElement("Document");
        kmlElement->InsertEndChild(documentElement);
        // Step 4: Create missionConfig Element
        xml::XMLElement *missionElement = doc.NewElement("wpml:missionConfig");
        documentElement->InsertEndChild(missionElement);
        GET_NEC_WPML_ARG_E(doc, missionElement, data, missionConfig.flyToWaylineMode);
        GET_NEC_WPML_ARG_E(doc, missionElement, data, missionConfig.finishAction);
        GET_NEC_WPML_ARG_E(doc, missionElement, data, missionConfig.exitOnRCLost);
        GET_OPT_WPML_ARG_E(doc, missionElement, data, missionConfig.executeRCLostAction);
        GET_NEC_WPML_ARG_N(doc, missionElement, data, missionConfig.takeOffSecurityHeight);
        GET_NEC_WPML_ARG_N(doc, missionElement, data, missionConfig.globalTransitionalSpeed);
        GET_NEC_WPML_ARG_N(doc, missionElement, data, missionConfig.globalRTHHeight);
        xml::XMLElement *droneInfoElement = doc.NewElement("wpml:droneInfo");
        missionElement->InsertEndChild(droneInfoElement);
        GET_NEC_WPML_ARG_N(doc, droneInfoElement, data, missionConfig.droneInfo.droneEnumValue);
        GET_OPT_WPML_ARG_N(doc, droneInfoElement, data, missionConfig.droneInfo.droneSubEnumValue);
        xml::XMLElement *payloadInfoElement = doc.NewElement("wpml:payloadInfo");
        missionElement->InsertEndChild(payloadInfoElement);
        GET_NEC_WPML_ARG_N(doc, payloadInfoElement, data, missionConfig.payloadInfo.payloadEnumValue);
        GET_NEC_WPML_ARG_N(doc, payloadInfoElement, data, missionConfig.payloadInfo.payloadPositionIndex);
        xml::XMLElement *autoRerouteInfoElement = doc.NewElement("wpml:autoRerouteInfo");
        missionElement->InsertEndChild(autoRerouteInfoElement);
        GET_NEC_WPML_ARG_N(doc, autoRerouteInfoElement, data, missionConfig.autoRerouteInfo.missionAutoRerouteMode);
        GET_NEC_WPML_ARG_N(
            doc, autoRerouteInfoElement, data, missionConfig.autoRerouteInfo.transitionalAutoRerouteMode);
        // Step 5: Create Folder Element
        for (const auto& fd : data.folder)
        {
            xml::XMLElement *folderElement = doc.NewElement("Folder");
            documentElement->InsertEndChild(folderElement);
            GET_NEC_WPML_ARG_N(doc, folderElement, fd, templateId);
            GET_NEC_WPML_ARG_N(doc, folderElement, fd, waylineId);
            GET_NEC_WPML_ARG_N(doc, folderElement, fd, autoFlightSpeed);
            GET_NEC_WPML_ARG_E(doc, folderElement, fd, executeHeightMode);
            // Step 5: Create Placemark Element
            for (const auto& wpm : fd.placemark)
            {
                xml::XMLElement *placemarkElement = doc.NewElement("Placemark");
                folderElement->InsertEndChild(placemarkElement);
                GET_OPT_WPML_ARG_N(doc, placemarkElement, wpm, isRisky);
				GET_NEC_WPML_ARG_CPT(doc, placemarkElement, wpm, point);
                GET_NEC_WPML_ARG_N(doc, placemarkElement, wpm, index);
                GET_NEC_WPML_ARG_N(doc, placemarkElement, wpm, executeHeight);
                GET_NEC_WPML_ARG_N(doc, placemarkElement, wpm, waypointSpeed);
                GET_NEC_WPML_ARG_N(doc, placemarkElement, wpm, useStraightLine);
                xml::XMLElement *waypointHeadingParamElement = doc.NewElement("wpml:waypointHeadingParam");
                placemarkElement->InsertEndChild(waypointHeadingParamElement);
                GET_NEC_WPML_ARG_E(doc, waypointHeadingParamElement, wpm, waypointHeadingParam.waypointHeadingMode);
                GET_OPT_WPML_ARG_N(doc, waypointHeadingParamElement, wpm, waypointHeadingParam.waypointHeadingAngle);
                GET_OPT_WPML_ARG_P(doc, waypointHeadingParamElement, wpm, waypointHeadingParam.waypointPoiPoint);
                GET_NEC_WPML_ARG_E(doc, waypointHeadingParamElement, wpm, waypointHeadingParam.waypointHeadingPathMode);
                xml::XMLElement *waypointTurnParamElement = doc.NewElement("wpml:waypointTurnParam");
                placemarkElement->InsertEndChild(waypointTurnParamElement);
                GET_NEC_WPML_ARG_E(doc, waypointTurnParamElement, wpm, waypointTurnParam.waypointTurnMode);
                GET_OPT_WPML_ARG_N(doc, waypointTurnParamElement, wpm, waypointTurnParam.waypointTurnDampingDist);
                if (wpm.startActionGroup.is_initialized())
                {
                    auto tmpAG = wpm.startActionGroup.value();
                    xml::XMLElement *actionGroupElement = doc.NewElement("wpml:actionGroup");
                    placemarkElement->InsertEndChild(actionGroupElement);
                    GET_NEC_WPML_ARG_N(doc, actionGroupElement, tmpAG, actionGroupId);
                    GET_NEC_WPML_ARG_N(doc, actionGroupElement, tmpAG, actionGroupStartIndex);
                    GET_NEC_WPML_ARG_N(doc, actionGroupElement, tmpAG, actionGroupEndIndex);
                    GET_NEC_WPML_ARG_E(doc, actionGroupElement, tmpAG, actionGroupMode);
                    xml::XMLElement *actionTriggerElement = doc.NewElement("wpml:actionTrigger");
                    actionGroupElement->InsertEndChild(actionTriggerElement);
                    GET_NEC_WPML_ARG_E(doc, actionTriggerElement, tmpAG, actionTrigger.actionTriggerType);
                    GET_OPT_WPML_ARG_N(doc, actionTriggerElement, tmpAG, actionTrigger.actionTriggerParam);
                    for (auto& act : tmpAG.action)
                    {
                        xml::XMLElement *actionElement = doc.NewElement("wpml:action");
                        actionGroupElement->InsertEndChild(actionElement);
                        GET_NEC_WPML_ARG_N(doc, actionElement, act, actionId);
                        GET_NEC_WPML_ARG_E(doc, actionElement, act, actionActuatorFunc);
                        xml::XMLElement *actionActuatorFuncParamElement =
                            doc.NewElement("wpml:actionActuatorFuncParam");
                        actionElement->InsertEndChild(actionActuatorFuncParamElement);
                        switch (act.actionActuatorFunc)
                        {
                            case wce::ActionActuatorFunc::takePhoto :
                            {
                                auto curAFP =
                                    std::dynamic_pointer_cast<wcs::TakePhotoParam>(act.actionActuatorFuncParam);
                                GET_NEC_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, payloadPositionIndex);
                                GET_NEC_WPML_ARG_S(doc, actionActuatorFuncParamElement, *curAFP, fileSuffix);
                                GET_NEC_WPML_ARG_ES(doc, actionActuatorFuncParamElement, *curAFP, payloadLensIndex);
                                GET_NEC_WPML_ARG_N(
                                    doc, actionActuatorFuncParamElement, *curAFP, useGlobalPayloadLensIndex);
                                break;
                            }
                            case wce::ActionActuatorFunc::startRecord :
                            {
                                auto curAFP =
                                    std::dynamic_pointer_cast<wcs::StartRecordParam>(act.actionActuatorFuncParam);
                                GET_NEC_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, payloadPositionIndex);
                                GET_NEC_WPML_ARG_S(doc, actionActuatorFuncParamElement, *curAFP, fileSuffix);
                                GET_NEC_WPML_ARG_ES(doc, actionActuatorFuncParamElement, *curAFP, payloadLensIndex);
                                GET_NEC_WPML_ARG_N(
                                    doc, actionActuatorFuncParamElement, *curAFP, useGlobalPayloadLensIndex);
                                break;
                            }
                            case wce::ActionActuatorFunc::stopRecord :
                            {
                                auto curAFP =
                                    std::dynamic_pointer_cast<wcs::StopRecordParam>(act.actionActuatorFuncParam);
                                GET_NEC_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, payloadPositionIndex);
                                GET_NEC_WPML_ARG_ES(doc, actionActuatorFuncParamElement, *curAFP, payloadLensIndex);
                                break;
                            }
                            case wce::ActionActuatorFunc::focus :
                            {
                                auto curAFP = std::dynamic_pointer_cast<wcs::FocusParam>(act.actionActuatorFuncParam);
                                GET_NEC_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, payloadPositionIndex);
                                GET_NEC_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, isPointFocus);
                                GET_NEC_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, focusX);
                                GET_NEC_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, focusY);
                                GET_OPT_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, focusRegionWidth);
                                GET_OPT_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, focusRegionHeight);
                                GET_NEC_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, isInfiniteFocus);
                                break;
                            }
                            case wce::ActionActuatorFunc::zoom :
                            {
                                auto curAFP = std::dynamic_pointer_cast<wcs::ZoomParam>(act.actionActuatorFuncParam);
                                GET_NEC_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, payloadPositionIndex);
                                GET_NEC_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, focalLength);
                                break;
                            }
                            case wce::ActionActuatorFunc::customDirName :
                            {
                                auto curAFP =
                                    std::dynamic_pointer_cast<wcs::CustomDirNameParam>(act.actionActuatorFuncParam);
                                GET_NEC_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, payloadPositionIndex);
                                GET_NEC_WPML_ARG_S(doc, actionActuatorFuncParamElement, *curAFP, directoryName);
                                break;
                            }
                            case wce::ActionActuatorFunc::gimbalRotate :
                            {
                                auto curAFP =
                                    std::dynamic_pointer_cast<wcs::GimbalRotateParam>(act.actionActuatorFuncParam);
                                GET_NEC_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, payloadPositionIndex);
                                GET_NEC_WPML_ARG_E(doc, actionActuatorFuncParamElement, *curAFP, gimbalHeadingYawBase);
                                GET_NEC_WPML_ARG_E(doc, actionActuatorFuncParamElement, *curAFP, gimbalRotateMode);
                                GET_NEC_WPML_ARG_N(
                                    doc, actionActuatorFuncParamElement, *curAFP, gimbalPitchRotateEnable);
                                GET_NEC_WPML_ARG_N(
                                    doc, actionActuatorFuncParamElement, *curAFP, gimbalPitchRotateAngle);
                                GET_NEC_WPML_ARG_N(
                                    doc, actionActuatorFuncParamElement, *curAFP, gimbalRollRotateEnable);
                                GET_NEC_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, gimbalRollRotateAngle);
                                GET_NEC_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, gimbalYawRotateEnable);
                                GET_NEC_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, gimbalYawRotateAngle);
                                GET_NEC_WPML_ARG_N(
                                    doc, actionActuatorFuncParamElement, *curAFP, gimbalRotateTimeEnable);
                                GET_NEC_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, gimbalRotateTime);
                                break;
                            }
                            case wce::ActionActuatorFunc::rotateYaw :
                            {
                                auto curAFP =
                                    std::dynamic_pointer_cast<wcs::RotateYawParam>(act.actionActuatorFuncParam);
                                GET_NEC_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, aircraftHeading);
                                GET_NEC_WPML_ARG_E(doc, actionActuatorFuncParamElement, *curAFP, aircraftPathMode);
                                break;
                            }
                            case wce::ActionActuatorFunc::hover :
                            {
                                auto curAFP = std::dynamic_pointer_cast<wcs::HoverParam>(act.actionActuatorFuncParam);
                                GET_NEC_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, hoverTime);
                                break;
                            }
                            case wce::ActionActuatorFunc::gimbalEvenlyRotate :
                            {
                                auto curAFP = std::dynamic_pointer_cast<wcs::GimbalEvenlyRotateParam>(
                                    act.actionActuatorFuncParam);
                                GET_NEC_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, payloadPositionIndex);
                                GET_NEC_WPML_ARG_N(
                                    doc, actionActuatorFuncParamElement, *curAFP, gimbalPitchRotateAngle);
                                break;
                            }
                            case wce::ActionActuatorFunc::accurateShoot :
                            {
                                auto curAFP =
                                    std::dynamic_pointer_cast<wcs::AccurateShootParam>(act.actionActuatorFuncParam);
                                GET_NEC_WPML_ARG_N(
                                    doc, actionActuatorFuncParamElement, *curAFP, gimbalPitchRotateAngle);
                                GET_NEC_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, gimbalYawRotateAngle);
                                GET_NEC_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, focusX);
                                GET_NEC_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, focusY);
                                GET_NEC_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, focusRegionWidth);
                                GET_NEC_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, focusRegionHeight);
                                GET_NEC_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, focalLength);
                                GET_NEC_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, aircraftHeading);
                                GET_NEC_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, accurateFrameValid);
                                GET_NEC_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, payloadPositionIndex);
                                GET_NEC_WPML_ARG_ES(doc, actionActuatorFuncParamElement, *curAFP, payloadLensIndex);
                                GET_NEC_WPML_ARG_N(
                                    doc, actionActuatorFuncParamElement, *curAFP, useGlobalPayloadLensIndex);
                                GET_NEC_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, targetAngle);
                                GET_NEC_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, imageWidth);
                                GET_NEC_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, imageHeight);
                                GET_NEC_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, AFPos);
                                GET_NEC_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, gimbalPort);
                                GET_NEC_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, accurateCameraType);
                                GET_NEC_WPML_ARG_S(doc, actionActuatorFuncParamElement, *curAFP, accurateFilePath);
                                GET_NEC_WPML_ARG_S(doc, actionActuatorFuncParamElement, *curAFP, accurateFileMD5);
                                GET_NEC_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, accurateFileSize);
                                GET_NEC_WPML_ARG_S(doc, actionActuatorFuncParamElement, *curAFP, accurateFileSuffix);
                                GET_NEC_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, accurateCameraApertue);
                                GET_NEC_WPML_ARG_N(
                                    doc, actionActuatorFuncParamElement, *curAFP, accurateCameraLuminance);
                                GET_NEC_WPML_ARG_N(
                                    doc, actionActuatorFuncParamElement, *curAFP, accurateCameraShutterTime);
                                GET_NEC_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, accurateCameraISO);
                                break;
                            }
                            case wce::ActionActuatorFunc::orientedShoot :
                            {
                                auto curAFP =
                                    std::dynamic_pointer_cast<wcs::OrientedShootParams>(act.actionActuatorFuncParam);
                                GET_NEC_WPML_ARG_N(
                                    doc, actionActuatorFuncParamElement, *curAFP, gimbalPitchRotateAngle);
                                GET_NEC_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, gimbalYawRotateAngle);
                                GET_NEC_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, focusX);
                                GET_NEC_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, focusY);
                                GET_NEC_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, focusRegionWidth);
                                GET_NEC_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, focusRegionHeight);
                                GET_NEC_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, focalLength);
                                GET_NEC_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, aircraftHeading);
                                GET_NEC_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, accurateFrameValid);
                                GET_NEC_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, payloadPositionIndex);
                                GET_NEC_WPML_ARG_ES(doc, actionActuatorFuncParamElement, *curAFP, payloadLensIndex);
                                GET_NEC_WPML_ARG_N(
                                    doc, actionActuatorFuncParamElement, *curAFP, useGlobalPayloadLensIndex);
                                GET_NEC_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, targetAngle);
                                GET_NEC_WPML_ARG_S(doc, actionActuatorFuncParamElement, *curAFP, actionUUID);
                                GET_NEC_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, imageWidth);
                                GET_NEC_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, imageHeight);
                                GET_NEC_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, AFPos);
                                GET_NEC_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, gimbalPort);
                                GET_NEC_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, orientedCameraType);
                                GET_NEC_WPML_ARG_S(doc, actionActuatorFuncParamElement, *curAFP, orientedFilePath);
                                GET_NEC_WPML_ARG_S(doc, actionActuatorFuncParamElement, *curAFP, orientedFileMD5);
                                GET_NEC_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, orientedFileSize);
                                GET_NEC_WPML_ARG_S(doc, actionActuatorFuncParamElement, *curAFP, orientedFileSuffix);
                                GET_NEC_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, orientedCameraApertue);
                                GET_NEC_WPML_ARG_N(
                                    doc, actionActuatorFuncParamElement, *curAFP, orientedCameraLuminance);
                                GET_NEC_WPML_ARG_N(
                                    doc, actionActuatorFuncParamElement, *curAFP, orientedCameraShutterTime);
                                GET_NEC_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, orientedCameraISO);
                                GET_NEC_WPML_ARG_E(doc, actionActuatorFuncParamElement, *curAFP, orientedPhotoMode);
                                break;
                            }
                            case wce::ActionActuatorFunc::panoShot :
                            {
                                auto curAFP =
                                    std::dynamic_pointer_cast<wcs::PanoShotParams>(act.actionActuatorFuncParam);
                                GET_NEC_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, payloadPositionIndex);
                                GET_NEC_WPML_ARG_ES(doc, actionActuatorFuncParamElement, *curAFP, payloadLensIndex);
                                GET_NEC_WPML_ARG_N(
                                    doc, actionActuatorFuncParamElement, *curAFP, useGlobalPayloadLensIndex);
                                GET_NEC_WPML_ARG_E(doc, actionActuatorFuncParamElement, *curAFP, panoShotSubMode);
                                break;
                            }
                            case wce::ActionActuatorFunc::recordPointCloud :
                            {
                                auto curAFP =
                                    std::dynamic_pointer_cast<wcs::RecordPointCloudParams>(act.actionActuatorFuncParam);
                                GET_NEC_WPML_ARG_N(doc, actionActuatorFuncParamElement, *curAFP, payloadPositionIndex);
                                GET_NEC_WPML_ARG_E(
                                    doc, actionActuatorFuncParamElement, *curAFP, recordPointCloudOperate);
                                break;
                            }
                            default :
                                break;
                        }
                    }
                }
            }
        }
        // Save file
        doc.SaveFile(wpmlPath.c_str());
        return true;
    }
    catch (...)
    {
        return false;
    }
}

boost::optional<wcs::WPMZData> parseWPMZ(const std::string& kmzPath)
{
    std::string outputDir = wcu::getTempDir() + PATH_SEPARATOR + wcu::getFileName(kmzPath) + wcu::getNowTimestamp();
    wcu::removeFileOrDir(outputDir);
    if (!wcu::makeDir(outputDir))
    {
        return boost::none;
    }
    bool isExtract = wcu::extractKMZ(kmzPath, outputDir);
    if (!isExtract)
    {
        wcu::removeFileOrDir(outputDir);
        return boost::none;
    }
    wcs::WPMZData res;
    std::future<boost::optional<wcs::KMLDocument>> fKml;
    std::future<boost::optional<wcs::WPMLDocument>> fWpml;
    std::vector<std::string> files = wcu::findFiles(outputDir);
    for (const auto& f : files)
    {
        if (wcu::endWith(f, ".kml"))
        {
            fKml = std::async(std::launch::async, wcc::parseKML, f);
        }
        else if (wcu::endWith(f, ".wpml"))
        {
            fWpml = std::async(std::launch::async, wcc::parseWPML, f);
        }
        else if (wcu::endWith(f, "res"))
        {
            res.resDir = f;
        }
    }
    auto resKml = fKml.get();
    auto resWpml = fWpml.get();
    if (!resKml.is_initialized() || !resWpml.is_initialized())
    {
        wcu::removeFileOrDir(outputDir);
        return boost::none;
    }
    res.templateKML = resKml.value();
    res.waylinesWPML = resWpml.value();
    wcu::removeFileOrDir(outputDir);
    return res;
}

bool createWPMZ(const wcs::WPMZData& data, const std::string& kmzPath)
{
    std::string packageDir = /*wcu::getTempDir() + PATH_SEPARATOR +*/ "../data/wpmz/temp/"+  wcu::getFileName(kmzPath);
    std::string outputDir = packageDir + PATH_SEPARATOR + "wpmz";
    wcu::removeFileOrDir(packageDir);
    if (!wcu::makeDir(outputDir))
    {
        return false;
    }
    std::future<bool> succKml = std::async(std::launch::async,
                                           wcc::createKML,
                                           data.templateKML,
                                           outputDir + PATH_SEPARATOR + "template.kml"),
                      succWpml = std::async(std::launch::async,
                                            wcc::createWPML,
                                            data.waylinesWPML,
                                            outputDir + PATH_SEPARATOR + "waylines.wpml"),
                      succRes = std::async(
                          std::launch::async,
                          [&data](const std::string& outputDir)
                          {
                              if (data.resDir.is_initialized())
                              {
                                  return wcu::copyFileOrDir(data.resDir.value(), outputDir);
                              }
                              return true;
                          },
                          outputDir);
    if (!(succKml.get() && succWpml.get() && succRes.get()))
    {
        return false;
    }
    return wcu::packageKMZ(packageDir, kmzPath) && wcu::removeFileOrDir(packageDir);
}
}
} // namespace wpml_codec::core
