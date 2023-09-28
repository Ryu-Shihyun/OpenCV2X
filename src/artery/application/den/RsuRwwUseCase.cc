/*
 * Artery V2X Simulation Framework
 * Copyright 2016-2018 Raphael Riebl, Christina Obermaier
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/application/den/RsuRwwUseCase.h"

#include "artery/application/RsuDenService.h"

#include <boost/units/base_units/metric/hour.hpp>
#include <boost/units/systems/si/length.hpp>
#include <boost/units/systems/si/prefixes.hpp>
#include <boost/units/systems/si/time.hpp>
#include <omnetpp/csimulation.h>
#include <vanetza/btp/data_request.hpp>
#include <vanetza/units/acceleration.hpp>

#include "artery/utility/Geometry.h"
#include "artery/utility/Identity.h"

#include <algorithm>
#include <numeric>
#include <cmath>

#include <iostream>
#include <string>
#include <bitset>

using omnetpp::SIMTIME_S;
using omnetpp::SIMTIME_MS;

using namespace omnetpp;

Define_Module(artery::den::RsuRwwLaneClosure)

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

void RsuRwwLaneClosure::initialize(int stage)
{
    RsuUseCase::initialize(stage);
    if (stage == 0)
    {
        mLastTransmission = simTime() + uniform(SimTime(0, SIMTIME_MS), SimTime(3000, SIMTIME_MS));
        mInterval = par("interval");
        mLongitude = par("longitude");
        mLatitude = par("latitude");
        mRsuCentre = par("rsuCentre");
        mGeoPosition = &mService->getFacilities().get_const<GeoPosition>();
    }
}

void RsuRwwLaneClosure::check()
{
    if (simTime() - mLastTransmission >= mInterval){
        auto message = createMessage();
        auto request = createRequest();
        mService->sendDenm(std::move(message), request);
        mLastTransmission = simTime();
    }
}

void RsuRwwLaneClosure::indicate(const artery::DenmObject& denm) {
    // record all the stats and such
    if (denm & CauseCode::Roadworks) {
        const vanetza::asn1::Denm& asn1 = denm.asn1();
    }
}

vanetza::asn1::Denm RsuRwwLaneClosure::createMessage()
{
    auto msg = createMessageSkeleton();
    msg->denm.management.relevanceDistance = vanetza::asn1::allocate<RelevanceDistance_t>();
    *msg->denm.management.relevanceDistance = RelevanceDistance_lessThan1000m;
    msg->denm.management.relevanceTrafficDirection = vanetza::asn1::allocate<RelevanceTrafficDirection_t>();
    *msg->denm.management.relevanceTrafficDirection = RelevanceTrafficDirection_upstreamTraffic;
    msg->denm.management.validityDuration = vanetza::asn1::allocate<ValidityDuration_t>();
    *msg->denm.management.validityDuration = 20;
    msg->denm.management.stationType = StationType_roadSideUnit;
    msg->denm.management.eventPosition.altitude.altitudeValue = AltitudeValue_unavailable;
    msg->denm.management.eventPosition.altitude.altitudeConfidence = AltitudeConfidence_unavailable;
    if (mRsuCentre) {
        // Road work warning will be placed as though the centre of the roadworks is where the RSU is placed.
        // This is an easy means of avoiding having to manually specify the location of road works.
        const double longitude = mGeoPosition->longitude / vanetza::units::degree;
        msg->denm.management.eventPosition.longitude = std::round(longitude * 1e6 * Longitude_oneMicrodegreeEast);
        const double latitude = mGeoPosition->latitude / vanetza::units::degree;
        msg->denm.management.eventPosition.latitude = std::round(latitude * 1e6 * Latitude_oneMicrodegreeNorth);
    } else{
        msg->denm.management.eventPosition.longitude = std::round(mLongitude * 1e6 * Longitude_oneMicrodegreeEast);
        msg->denm.management.eventPosition.latitude = std::round(mLatitude * 1e6 * Latitude_oneMicrodegreeNorth);
    }
    msg->denm.management.eventPosition.positionConfidenceEllipse.semiMajorOrientation = HeadingValue_unavailable;
    msg->denm.management.eventPosition.positionConfidenceEllipse.semiMajorConfidence = SemiAxisLength_unavailable;
    msg->denm.management.eventPosition.positionConfidenceEllipse.semiMinorConfidence = SemiAxisLength_unavailable;

    msg->denm.situation = vanetza::asn1::allocate<SituationContainer_t>();
    msg->denm.situation->informationQuality = 1;
    msg->denm.situation->eventType.causeCode = CauseCodeType_roadworks;
    msg->denm.situation->eventType.subCauseCode = 4;

    msg->denm.alacarte = vanetza::asn1::allocate<AlacarteContainer_t>();
    msg->denm.alacarte->roadWorks = vanetza::asn1::allocate<RoadWorksContainerExtended_t>();
    msg->denm.alacarte->roadWorks->closedLanes = vanetza::asn1::allocate<ClosedLanes_t>();

//    BIT_STRING_s* laneStatus = new BIT_STRING_s();
//    laneStatus->buf = (0x1E);
//    laneStatus->size = 2;
//    laneStatus->bits_unused = 3;
//    msg->denm.alacarte->roadWorks->closedLanes->drivingLaneStatus = laneStatus;
    
    return msg;
}

vanetza::btp::DataRequestB RsuRwwLaneClosure::createRequest()
{
    namespace geonet = vanetza::geonet;
    using vanetza::units::si::seconds;
    using vanetza::units::si::meter;

    vanetza::btp::DataRequestB request;
    request.gn.traffic_class.tc_id(1);

//    geonet::DataRequest::Repetition repetition;
//    repetition.interval = 1 * seconds;
//    repetition.maximum = 20.0 * seconds;
//    request.gn.repetition = repetition;

    geonet::Area destination;
    geonet::Circle destination_shape;
    destination_shape.r = 1000.0 * meter;
    destination.shape = destination_shape;

    vanetza::units::GeoAngle longitude = mGeoPosition->longitude;
    vanetza::units::GeoAngle latitude = mGeoPosition->latitude;

    destination.position.latitude = latitude;
    destination.position.longitude = longitude;
    request.gn.destination = destination;

    request.gn.message_category = 3;
    int time_interval = int(mInterval.dbl() * 10);
    request.gn.message_rate = time_interval;

    return request;
}

} // namespace den
} // namespace artery
