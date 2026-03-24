#pragma once

#ifndef WPML_CODEC_MACROS_H
#define WPML_CODEC_MACROS_H

#include <wpml_codec/common.h>

#define SET_OPT_WPML_ARG_B(doc, xml, name)                                                                             \
    do                                                                                                                 \
    {                                                                                                                  \
        auto pEle = xml->FirstChildElement("wpml:" #name);                                                             \
        if (pEle != nullptr)                                                                                           \
        {                                                                                                              \
            auto boolVal = wcu::toBool(pEle->GetText());                                                               \
            if (boolVal.is_initialized())                                                                                   \
            {                                                                                                          \
                (doc).name = std::move(boolVal.value());                                                               \
            }                                                                                                          \
        }                                                                                                              \
    } while (0)

#define SET_OPT_WPML_ARG_D(doc, xml, name)                                                                             \
    do                                                                                                                 \
    {                                                                                                                  \
        auto pEle = xml->FirstChildElement("wpml:" #name);                                                             \
        if (pEle != nullptr)                                                                                           \
        {                                                                                                              \
            auto doubleVal = wcu::toDouble(pEle->GetText());                                                           \
            if (doubleVal.is_initialized())                                                                                 \
            {                                                                                                          \
                (doc).name = std::move(doubleVal.value());                                                             \
            }                                                                                                          \
        }                                                                                                              \
    } while (0)

#define SET_OPT_WPML_ARG_E(doc, xml, name, enumType)                                                                   \
    do                                                                                                                 \
    {                                                                                                                  \
        auto pEle = xml->FirstChildElement("wpml:" #name);                                                             \
        if (pEle != nullptr)                                                                                           \
        {                                                                                                              \
            auto enumVal = my_enum::enum_cast<wce::enumType>(pEle->GetText());                                      \
           if (enumVal)                                                                                 \
            {                                                                                                          \
                (doc).name = std::move(enumVal.value());                                                               \
            }                                                                                                          \
        }                                                                                                              \
    } while (0)

#define SET_OPT_WPML_ARG_ES(doc, xml, name, enumType)                                                                  \
    do                                                                                                                 \
    {                                                                                                                  \
        auto pEle = xml->FirstChildElement("wpml:" #name);                                                             \
        if (pEle != nullptr)                                                                                           \
        {                                                                                                              \
            auto enumsStr = pEle->GetText();                                                                           \
            if (enumsStr != nullptr)                                                                                   \
            {                                                                                                          \
                auto enumStrVec = wcu::split(enumsStr, ",");                                                           \
                std::vector<wce::enumType> enumVec(enumStrVec.size());                                                 \
                size_t i = 0;                                                                                          \
                for (const auto& enumStr : enumStrVec)                                                                 \
                {                                                                                                      \
                    auto enumVal = my_enum::enum_cast<wce::enumType>(enumStr);                                      \
                    if (enumVal)                                                                           \
                    {                                                                                                  \
                        enumVec[i++] = std::move(enumVal.value());                                                     \
                    }                                                                                                  \
                }                                                                                                      \
                (doc).name = std::move(enumVec);                                                                       \
            }                                                                                                          \
        }                                                                                                              \
    } while (0)

#define SET_OPT_WPML_ARG_I(doc, xml, name)                                                                             \
    do                                                                                                                 \
    {                                                                                                                  \
        auto pEle = xml->FirstChildElement("wpml:" #name);                                                             \
        if (pEle != nullptr)                                                                                           \
        {                                                                                                              \
            auto intVal = wcu::toInt(pEle->GetText());                                                                 \
            if (intVal.is_initialized())                                                                                    \
            {                                                                                                          \
                (doc).name = std::move(intVal.value());                                                                \
            }                                                                                                          \
        }                                                                                                              \
    } while (0)

#define SET_OPT_WPML_ARG_L(doc, xml, name)                                                                             \
    do                                                                                                                 \
    {                                                                                                                  \
        auto pEle = xml->FirstChildElement("wpml:" #name);                                                             \
        if (pEle != nullptr)                                                                                           \
        {                                                                                                              \
            auto longVal = wcu::toLongLong(pEle->GetText());                                                           \
            if (longVal.is_initialized())                                                                                   \
            {                                                                                                          \
                (doc).name = std::move(longVal.value());                                                               \
            }                                                                                                          \
        }                                                                                                              \
    } while (0)

#define SET_OPT_WPML_ARG_KLS(doc, xml, name)                                                                           \
    do                                                                                                                 \
    {                                                                                                                  \
        auto pEle = xml->FirstChildElement("LineString");                                                              \
        if (pEle != nullptr)                                                                                           \
        {                                                                                                              \
            auto lineStr = pEle->FirstChildElement("coordinates")->GetText();                                          \
            auto poisStr = wcu::split(wcu::trim(lineStr), NEW_LINE);                                                   \
            std::vector<wcs::KPoint> points(poisStr.size());                                                           \
            for (const auto& poiStr : poisStr)                                                                         \
            {                                                                                                          \
                auto point = wcu::toDoubles(poiStr);                                                                   \
                wcs::KPoint p{point[0], point[1], boost::none};                                                       \
                if (point.size() > 2)                                                                                  \
                {                                                                                                      \
                    p.alt = std::move(point[2]);                                                                       \
                }                                                                                                      \
                points.emplace_back(p);                                                                                \
            }                                                                                                          \
            (doc).name.points = std::move(points);                                                                     \
        }                                                                                                              \
    } while (0)

#define SET_OPT_WPML_ARG_KPN(doc, xml, name)                                                                           \
    do                                                                                                                 \
    {                                                                                                                  \
        auto pEle = xml->FirstChildElement("Polygon");                                                                 \
        if (pEle != nullptr)                                                                                           \
        {                                                                                                              \
            auto pOuter = pEle->FirstChildElement("outerBoundaryIs");                                                  \
            if (pOuter != nullptr)                                                                                     \
            {                                                                                                          \
                auto coorStr = pOuter->FirstChildElement("LinearRing")->FirstChildElement("coordinates");              \
                auto poisStr = wcu::split(wcu::trim(coorStr->GetText()), NEW_LINE);                                    \
                wcs::KLineString outer;                                                                                \
                for (const auto& poiStr : poisStr)                                                                     \
                {                                                                                                      \
                    auto point = wcu::toDoubles(poiStr);                                                               \
                    wcs::KPoint p{point[0], point[1], boost::none};                                                   \
                    if (point.size() > 2)                                                                              \
                    {                                                                                                  \
                        p.alt = std::move(point[2]);                                                                   \
                    }                                                                                                  \
                    outer.points.emplace_back(p);                                                                      \
                }                                                                                                      \
                (doc).name.outerLine = std::move(outer);                                                               \
            }                                                                                                          \
            auto pInner = pEle->FirstChildElement("innerBoundaryIs");                                                  \
            if (pInner != nullptr)                                                                                     \
            {                                                                                                          \
                auto coorStr = pInner->FirstChildElement("LinearRing")->FirstChildElement("coordinates");              \
                auto poisStr = wcu::split(wcu::trim(coorStr->GetText()), NEW_LINE);                                    \
                wcs::KLineString inner;                                                                                \
                for (const auto& poiStr : poisStr)                                                                     \
                {                                                                                                      \
                    auto point = wcu::toDoubles(poiStr);                                                               \
                    wcs::KPoint p{point[0], point[1], boost::none};                                                   \
                    if (point.size() > 2)                                                                              \
                    {                                                                                                  \
                        p.alt = std::move(point[2]);                                                                   \
                    }                                                                                                  \
                    inner.points.emplace_back(p);                                                                      \
                }                                                                                                      \
                (doc).name.innerLine = std::move(inner);                                                               \
            }                                                                                                          \
        }                                                                                                              \
    } while (0)

#define SET_OPT_WPML_ARG_KPT(doc, xml, name)                                                                           \
    do                                                                                                                 \
    {                                                                                                                  \
        auto pEle = xml->FirstChildElement("Point");                                                                   \
        if (pEle != nullptr)                                                                                           \
        {                                                                                                              \
            auto pointStr = wcu::trim(pEle->FirstChildElement("coordinates")->GetText());                              \
            auto point = wcu::toDoubles(pointStr);                                                                     \
            wcs::KPoint p{point[0], point[1], boost::none};                                                           \
            if (point.size() > 2)                                                                                      \
            {                                                                                                          \
                p.alt = std::move(point[2]);                                                                           \
            }                                                                                                          \
            (doc).name = std::move(p);                                                                                 \
        }                                                                                                              \
    } while (0)

#define SET_OPT_WPML_ARG_CPT(doc, xml, name)                                                                           \
    do                                                                                                                 \
    {                                                                                                                  \
        auto pEle = xml->FirstChildElement("Point");                                                                   \
        if (pEle != nullptr)                                                                                           \
        {                                                                                                              \
            auto pointStr = wcu::trim(pEle->FirstChildElement("coordinates")->GetText());                              \
            auto point = wcu::toDoubles(pointStr);                                                                     \
            wcs::CPoint p{point[0], point[1]};																			\
            (doc).name = std::move(p);                                                                                 \
        }                                                                                                              \
    } while (0)

#define SET_OPT_WPML_ARG_P(doc, xml, name)                                                                             \
    do                                                                                                                 \
    {                                                                                                                  \
        auto pEle = xml->FirstChildElement("wpml:" #name);                                                             \
        if (pEle != nullptr)                                                                                           \
        {                                                                                                              \
            auto reals = wcu::toDoubles(pEle->GetText());                                                              \
            wcs::Point p{reals[0], reals[1]};                                                                          \
            if (reals.size() == 3)                                                                                     \
            {                                                                                                          \
                p.alt = std::move(reals[2]);                                                                           \
            }                                                                                                          \
            (doc).name = std::move(p);                                                                                 \
        }                                                                                                              \
    } while (0)

#define SET_OPT_WPML_ARG_S(doc, xml, name)                                                                             \
    do                                                                                                                 \
    {                                                                                                                  \
        auto pEle = xml->FirstChildElement("wpml:" #name);                                                             \
        if (pEle != nullptr)                                                                                           \
        {                                                                                                              \
            auto strVal = pEle->GetText();                                                                             \
            if (strVal != nullptr)                                                                                     \
            {                                                                                                          \
                (doc).name = std::move(strVal);                                                                        \
            }                                                                                                          \
        }                                                                                                              \
    } while (0)

#define GET_OPT_WPML_ARG_E(doc, ele, iptData, name)                                                                    \
    do                                                                                                                 \
    {                                                                                                                  \
        if ((iptData).name.is_initialized())                                                                                \
        {                                                                                                              \
            xml::XMLElement *tmpElement = doc.NewElement(("wpml:" + wcu::split(#name, ".").back()).c_str());           \
            tmpElement->SetText(my_enum::enum_name((iptData).name.value()).data());                                 \
            ele->InsertEndChild(tmpElement);                                                                           \
        }                                                                                                              \
    } while (0)

#define GET_NEC_WPML_ARG_E(doc, ele, iptData, name)                                                                    \
    do                                                                                                                 \
    {                                                                                                                  \
        xml::XMLElement *tmpElement = doc.NewElement(("wpml:" + wcu::split(#name, ".").back()).c_str());               \
        tmpElement->SetText(my_enum::enum_name((iptData).name).data());                                             \
        ele->InsertEndChild(tmpElement);                                                                               \
    } while (0)

#define GET_OPT_WPML_ARG_ES(doc, ele, iptData, name)                                                                   \
    do                                                                                                                 \
    {                                                                                                                  \
        if ((iptData).name.is_initialized())                                                                                \
        {                                                                                                              \
            xml::XMLElement *tmpElement = doc.NewElement(("wpml:" + wcu::split(#name, ".").back()).c_str());           \
            auto enumVec = (iptData).name.value();                                                                     \
            std::string res = "";                                                                                      \
            for (const auto& enumVal : enumVec)                                                                        \
            {                                                                                                          \
                res += (std::string(my_enum::enum_name(enumVal)) + ",");                                            \
            }                                                                                                          \
            if (res != "")                                                                                             \
            {                                                                                                          \
                res.pop_back();                                                                                        \
            }                                                                                                          \
            tmpElement->SetText(res.c_str());                                                                          \
            ele->InsertEndChild(tmpElement);                                                                           \
        }                                                                                                              \
    } while (0)

#define GET_NEC_WPML_ARG_ES(doc, ele, iptData, name)                                                                   \
    do                                                                                                                 \
    {                                                                                                                  \
        xml::XMLElement *tmpElement = doc.NewElement(("wpml:" + wcu::split(#name, ".").back()).c_str());               \
        auto enumVec = (iptData).name;                                                                                 \
        std::string res = "";                                                                                          \
        for (const auto& enumVal : enumVec)                                                                            \
        {                                                                                                              \
            res += (std::string(my_enum::enum_name(enumVal)) + ",");                                                \
        }                                                                                                              \
        if (res != "")                                                                                                 \
        {                                                                                                              \
            res.pop_back();                                                                                            \
        }                                                                                                              \
        tmpElement->SetText(res.c_str());                                                                              \
        ele->InsertEndChild(tmpElement);                                                                               \
    } while (0)

#define GET_NEC_WPML_ARG_KLS(doc, ele, iptData, name)                                                                  \
    do                                                                                                                 \
    {                                                                                                                  \
        xml::XMLElement *tmpElement = doc.NewElement("LineString");                                                    \
        xml::XMLElement *tmpLineCorsElement = doc.NewElement("coordinates");                                           \
        auto line = (iptData).name;                                                                                    \
        std::string res = wcs::kLineStringToStr(line);                                                                 \
        tmpLineCorsElement->SetText(res.c_str());                                                                      \
        tmpElement->InsertEndChild(tmpLineCorsElement);                                                                \
        ele->InsertEndChild(tmpElement);                                                                               \
    } while (0)

#define GET_NEC_WPML_ARG_KPN(doc, ele, iptData, name)                                                                  \
    do                                                                                                                 \
    {                                                                                                                  \
        xml::XMLElement *tmpElement = doc.NewElement("Polygon");                                                       \
        xml::XMLElement *tmpOuterElement = doc.NewElement("outerBoundaryIs");                                          \
        xml::XMLElement *tmpOuterLineElement = doc.NewElement("LinearRing");                                           \
        xml::XMLElement *tmpOuterCorsElement = doc.NewElement("coordinates");                                          \
        auto poly = (iptData).name;                                                                                    \
        std::string res = wcs::kLineStringToStr(poly.outerLine);                                                       \
        tmpOuterCorsElement->SetText(res.c_str());                                                                     \
        tmpOuterLineElement->InsertEndChild(tmpOuterCorsElement);                                                      \
        tmpOuterElement->InsertEndChild(tmpOuterLineElement);                                                          \
        tmpElement->InsertEndChild(tmpOuterElement);                                                                   \
        if (poly.innerLine.is_initialized())                                                                                \
        {                                                                                                              \
            xml::XMLElement *tmpInnerElement = doc.NewElement("innerBoundaryIs");                                      \
            xml::XMLElement *tmpInnerLineElement = doc.NewElement("LinearRing");                                       \
            xml::XMLElement *tmpInnerCorsElement = doc.NewElement("coordinates");                                      \
            std::string res2 = wcs::kLineStringToStr(poly.innerLine.value());                                          \
            tmpInnerCorsElement->SetText(res2.c_str());                                                                \
            tmpInnerLineElement->InsertEndChild(tmpInnerCorsElement);                                                  \
            tmpInnerElement->InsertEndChild(tmpInnerLineElement);                                                      \
            tmpElement->InsertEndChild(tmpInnerElement);                                                               \
        }                                                                                                              \
        ele->InsertEndChild(tmpElement);                                                                               \
    } while (0)

#define GET_NEC_WPML_ARG_KPT(doc, ele, iptData, name)                                                                  \
    do                                                                                                                 \
    {                                                                                                                  \
        xml::XMLElement *tmpElement = doc.NewElement("Point");                                                         \
        xml::XMLElement *tmpPointCorsElement = doc.NewElement("coordinates");                                          \
        auto point = (iptData).name;                                                                                   \
        std::string res = wcs::kPointToStr(point);                                                                     \
        tmpPointCorsElement->SetText(res.c_str());                                                                     \
        tmpElement->InsertEndChild(tmpPointCorsElement);                                                               \
        ele->InsertEndChild(tmpElement);                                                                               \
    } while (0)

#define GET_NEC_WPML_ARG_CPT(doc, ele, iptData, name)                                                                  \
    do                                                                                                                 \
    {                                                                                                                  \
        xml::XMLElement *tmpElement = doc.NewElement("Point");                                                         \
        xml::XMLElement *tmpPointCorsElement = doc.NewElement("coordinates");                                          \
        auto point = (iptData).name;                                                                                   \
        std::string res = wcs::cPointToStr(point);                                                                     \
        tmpPointCorsElement->SetText(res.c_str());                                                                     \
        tmpElement->InsertEndChild(tmpPointCorsElement);                                                               \
        ele->InsertEndChild(tmpElement);                                                                               \
    } while (0)

#define GET_OPT_WPML_ARG_N(doc, ele, iptData, name)                                                                    \
    do                                                                                                                 \
    {                                                                                                                  \
        if ((iptData).name.is_initialized())                                                                                \
        {                                                                                                              \
            xml::XMLElement *tmpElement = doc.NewElement(("wpml:" + wcu::split(#name, ".").back()).c_str());           \
            tmpElement->SetText(wcu::toString((iptData).name.value()).c_str());                                        \
            ele->InsertEndChild(tmpElement);                                                                           \
        }                                                                                                              \
    } while (0)

#define GET_NEC_WPML_ARG_N(doc, ele, iptData, name)                                                                    \
    do                                                                                                                 \
    {                                                                                                                  \
        xml::XMLElement *tmpElement = doc.NewElement(("wpml:" + wcu::split(#name, ".").back()).c_str());               \
        tmpElement->SetText(wcu::toString((iptData).name).c_str());                                                    \
        ele->InsertEndChild(tmpElement);                                                                               \
    } while (0)

#define GET_OPT_WPML_ARG_P(doc, ele, iptData, name)                                                                    \
    do                                                                                                                 \
    {                                                                                                                  \
        if ((iptData).name.is_initialized())                                                                                \
        {                                                                                                              \
            xml::XMLElement *tmpElement = doc.NewElement(("wpml:" + wcu::split(#name, ".").back()).c_str());           \
            auto point = (iptData).name.value();                                                                       \
            std::vector<double> loc{point.lat, point.lon};                                                             \
            if (point.alt.is_initialized())                                                                                 \
            {                                                                                                          \
                loc.push_back(point.alt.value());                                                                      \
            }                                                                                                          \
            tmpElement->SetText(wcu::toString(loc).c_str());                                                           \
            ele->InsertEndChild(tmpElement);                                                                           \
        }                                                                                                              \
    } while (0)

#define GET_NEC_WPML_ARG_P(doc, ele, iptData, name)                                                                    \
    do                                                                                                                 \
    {                                                                                                                  \
        xml::XMLElement *tmpElement = doc.NewElement(("wpml:" + wcu::split(#name, ".").back()).c_str());               \
        auto point = (iptData).name;                                                                                   \
        std::vector<double> loc{point.lat, point.lon};                                                                 \
        if (point.alt.is_initialized())                                                                                     \
        {                                                                                                              \
            loc.push_back(point.alt.value());                                                                          \
        }                                                                                                              \
        tmpElement->SetText(wcu::toString(loc).c_str());                                                               \
        ele->InsertEndChild(tmpElement);                                                                               \
    } while (0)

#define GET_OPT_WPML_ARG_S(doc, ele, iptData, name)                                                                    \
    do                                                                                                                 \
    {                                                                                                                  \
        if ((iptData).name.is_initialized())                                                                                \
        {                                                                                                              \
            xml::XMLElement *tmpElement = doc.NewElement(("wpml:" + wcu::split(#name, ".").back()).c_str());           \
            tmpElement->SetText((iptData).name.value().c_str());                                                       \
            ele->InsertEndChild(tmpElement);                                                                           \
        }                                                                                                              \
    } while (0)

#define GET_NEC_WPML_ARG_S(doc, ele, iptData, name)                                                                    \
    do                                                                                                                 \
    {                                                                                                                  \
        xml::XMLElement *tmpElement = doc.NewElement(("wpml:" + wcu::split(#name, ".").back()).c_str());               \
        tmpElement->SetText((iptData).name.c_str());                                                                   \
        ele->InsertEndChild(tmpElement);                                                                               \
    } while (0)

#endif // WPML_CODEC_MACROS_H
