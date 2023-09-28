/*
 * Artery V2X Simulation Framework
 * Copyright 2016-2018 Raphael Riebl, Christina Obermaier
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/application/den/RsuHlnWcwUseCase.h"
#include "artery/application/RsuDenService.h"
#include "artery/utility/simtime_cast.h"

#include <boost/units/base_units/metric/hour.hpp>
#include <boost/units/systems/si/length.hpp>
#include <boost/units/systems/si/prefixes.hpp>
#include <boost/units/systems/si/time.hpp>
#include <omnetpp/csimulation.h>
#include <vanetza/btp/data_request.hpp>
#include <vanetza/units/acceleration.hpp>
#include <vanetza/asn1/its/AdverseWeatherCondition-PrecipitationSubCauseCode.h>
#include <vanetza/asn1/its/AdverseWeatherCondition-ExtremeWeatherConditionSubCauseCode.h>

#include "artery/utility/Identity.h"
#include "artery/utility/Geometry.h"

#include <algorithm>
#include <numeric>
#include <cmath>

using omnetpp::SIMTIME_S;
using omnetpp::SIMTIME_MS;

using namespace omnetpp;

Define_Module(artery::den::RsuHlnWcw)

template<typename T, typename U>
long round(const boost::units::quantity<T>& q, const U& u)
{
    boost::units::quantity<U> v { q };
    return std::round(v.value());
}

namespace artery
{
    namespace den
    {

        void RsuHlnWcw::initialize(int stage)
        {
            RsuUseCase::initialize(stage);
            if (stage == 0)
            {
                // Add random startup delay from 0 - 3s
                mLastTransmission = simTime() + uniform(SimTime(401, SIMTIME_MS), SimTime(600, SIMTIME_MS));
                mInterval = par("interval");
                mCauseCode = par("CauseCode");
                mSubCauseCode = par("SubCauseCode");
                mGeoPosition = &mService->getFacilities().get_const<GeoPosition>();
                mPacketSize = par("packetSize");
            }
        }

        void RsuHlnWcw::check()
        {
            // Check if sending at right rate.
            if (simTime() - mLastTransmission >= mInterval){
                auto message = createMessage();
                auto request = createRequest();
                mService->sendDenm(std::move(message), request);
                mLastTransmission = simTime();
            }
        }

        void RsuHlnWcw::indicate(const artery::DenmObject& denm) {
            // record all the stats and such
            if (denm & CauseCode::AdverseWeatherCondition_Precipitation) {
                const vanetza::asn1::Denm& asn1 = denm.asn1();

            } else if (denm & CauseCode::AdverseWeatherCondition_ExtremeWeather) {
                const vanetza::asn1::Denm& asn1 = denm.asn1();

            }
        }

        vanetza::asn1::Denm RsuHlnWcw::createMessage()
        {
            auto msg = createMessageSkeleton();
            msg->denm.management.relevanceDistance = vanetza::asn1::allocate<RelevanceDistance_t>();
            *msg->denm.management.relevanceDistance = RelevanceDistance_lessThan5km;
            msg->denm.management.relevanceTrafficDirection = vanetza::asn1::allocate<RelevanceTrafficDirection_t>();
            *msg->denm.management.relevanceTrafficDirection = RelevanceTrafficDirection_allTrafficDirections;
            msg->denm.management.validityDuration = vanetza::asn1::allocate<ValidityDuration_t>();
            *msg->denm.management.validityDuration = 2;
            msg->denm.management.stationType = StationType_roadSideUnit;

            msg->denm.situation = vanetza::asn1::allocate<SituationContainer_t>();
            msg->denm.situation->informationQuality = 1;
            if (mCauseCode == 19){
                msg->denm.situation->eventType.causeCode = CauseCodeType_adverseWeatherCondition_Precipitation;
                if (mSubCauseCode == 1)
                    msg->denm.situation->eventType.subCauseCode = AdverseWeatherCondition_PrecipitationSubCauseCode_heavyRain;
                if (mSubCauseCode == 2)
                    msg->denm.situation->eventType.subCauseCode = AdverseWeatherCondition_PrecipitationSubCauseCode_heavySnowfall;
                if (mSubCauseCode == 3)
                    msg->denm.situation->eventType.subCauseCode = AdverseWeatherCondition_PrecipitationSubCauseCode_softHail;
            } else if (mCauseCode == 17) {
                msg->denm.situation->eventType.causeCode = CauseCodeType_adverseWeatherCondition_ExtremeWeatherCondition;
                // Has to be strong winds in our case so default to it
                msg->denm.situation->eventType.subCauseCode = AdverseWeatherCondition_ExtremeWeatherConditionSubCauseCode_strongWinds;
            }

            return msg;
        }

        vanetza::btp::DataRequestB RsuHlnWcw::createRequest()
        {
            namespace geonet = vanetza::geonet;
            using vanetza::units::si::seconds;
            using vanetza::units::si::meter;
            vanetza::btp::DataRequestB request;
            request.gn.traffic_class.tc_id(1);

//            geonet::DataRequest::Repetition repetition;
//            repetition.interval = 0.5 * seconds;
//            repetition.maximum = 20.0 * seconds;
//            request.gn.repetition = repetition;

            geonet::Area destination;
            geonet::Circle destination_shape;
            destination_shape.r = 1000.0 * meter;
            destination.shape = destination_shape;

            vanetza::units::GeoAngle longitude = mGeoPosition->longitude;
            vanetza::units::GeoAngle latitude = mGeoPosition->latitude;

            destination.position.latitude = latitude;
            destination.position.longitude = longitude;
            request.gn.destination = destination;
            request.gn.fixed_length = mPacketSize;

            request.gn.message_category = 3;
            int time_interval = int(mInterval.dbl() * 10);
            request.gn.message_rate = time_interval;

            return request;
        }

    } // namespace den
} // namespace artery
