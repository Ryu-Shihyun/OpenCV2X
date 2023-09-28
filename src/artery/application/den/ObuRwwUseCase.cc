/*
 * Artery V2X Simulation Framework
 * Copyright 2016-2018 Raphael Riebl, Christina Obermaier
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/application/den/ObuRwwUseCase.h"

#include "artery/application/DenService.h"
#include "artery/application/LocalDynamicMap.h"
#include "artery/traci/VehicleController.h"

#include <boost/units/systems/si/length.hpp>
#include <boost/units/systems/si/time.hpp>
#include <omnetpp/csimulation.h>
#include <vanetza/btp/data_request.hpp>
#include <vanetza/units/acceleration.hpp>


#include <algorithm>
#include <numeric>

static const auto hour = 3600.0 * boost::units::si::seconds;

using omnetpp::SIMTIME_S;
using omnetpp::SIMTIME_MS;

Define_Module(artery::den::ObuRwwLaneClosure)

namespace traci { class VehicleController; }

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

void ObuRwwMobileUnit::initialize(int stage)
{
    UseCase::initialize(stage);
    if (stage == 0) {
        mDenmMemory = mService->getMemory();
        mMobileUnit = false;
    }
}

void ObuRwwMobileUnit::check()
{
    if (mMobileUnit){
        auto message = createMessage();
        auto request = createRequest();
        mService->sendDenm(std::move(message), request);
    }
}

vanetza::asn1::Denm ObuRwwMobileUnit::createMessage()
{
    auto msg = createMessageSkeleton();
    msg->denm.management.relevanceDistance = vanetza::asn1::allocate<RelevanceDistance_t>();
    *msg->denm.management.relevanceDistance = RelevanceDistance_lessThan1000m;
    msg->denm.management.relevanceTrafficDirection = vanetza::asn1::allocate<RelevanceTrafficDirection_t>();
    *msg->denm.management.relevanceTrafficDirection = RelevanceTrafficDirection_upstreamTraffic;
    msg->denm.management.validityDuration = vanetza::asn1::allocate<ValidityDuration_t>();
    *msg->denm.management.validityDuration = 60;
    msg->denm.management.stationType = StationType_unknown; // TODO retrieve type from SUMO

    msg->denm.situation = vanetza::asn1::allocate<SituationContainer_t>();
    msg->denm.situation->informationQuality = 1;
    msg->denm.situation->eventType.causeCode = CauseCodeType_trafficCondition;
    msg->denm.situation->eventType.subCauseCode = 0;

    // TODO set road type in Location container
    // TODO set lane position in Alacarte container

    return msg;
}

vanetza::btp::DataRequestB ObuRwwMobileUnit::createRequest()
{
    namespace geonet = vanetza::geonet;
    using vanetza::units::si::seconds;
    using vanetza::units::si::meter;

    vanetza::btp::DataRequestB request;
    request.gn.traffic_class.tc_id(1);

    geonet::DataRequest::Repetition repetition;
    repetition.interval = 1.0 * seconds;
    repetition.maximum = 60.0 * seconds;

    request.gn.repetition = repetition;

    geonet::Area destination;
    geonet::Circle destination_shape;
    destination_shape.r = 1000.0 * meter;
    destination.shape = destination_shape;
    destination.position.latitude = mVdp->latitude();
    destination.position.longitude = mVdp->longitude();
    request.gn.destination = destination;

    return request;
}


void ObuRwwLaneClosure::initialize(int stage)
{
    UseCase::initialize(stage);
    if (stage == 0) {
        mDenmMemory = mService->getMemory();
        mClosedLane = par("closedLane");
    }
}

void ObuRwwLaneClosure::indicate(const artery::DenmObject& denm){
    // Deal with the packet itself
    if (denm & CauseCode::Roadworks) {
        const vanetza::asn1::Denm& asn1 = denm.asn1();
        if (asn1->denm.situation->eventType.subCauseCode == 4){
            // Dealing with a lane closure
            auto status = asn1->denm.alacarte->roadWorks->closedLanes->drivingLaneStatus;

            auto vehicleController = &mService->getFacilities().get_mutable<traci::VehicleController>();
            const std::string id = vehicleController->getVehicleId();
            auto& vehicle_api = vehicleController->getTraCI()->vehicle;
            // Do something with this.
            vehicle_api.slowDown(id, 22.22, 30);
        }
    }
}

vanetza::asn1::Denm ObuRwwLaneClosure::createMessage()
{
    auto msg = createMessageSkeleton();
    msg->denm.management.relevanceDistance = vanetza::asn1::allocate<RelevanceDistance_t>();
    *msg->denm.management.relevanceDistance = RelevanceDistance_lessThan1000m;
    msg->denm.management.relevanceTrafficDirection = vanetza::asn1::allocate<RelevanceTrafficDirection_t>();
    *msg->denm.management.relevanceTrafficDirection = RelevanceTrafficDirection_upstreamTraffic;
    msg->denm.management.validityDuration = vanetza::asn1::allocate<ValidityDuration_t>();
    *msg->denm.management.validityDuration = 60;
    msg->denm.management.stationType = StationType_unknown; // TODO retrieve type from SUMO

    msg->denm.situation = vanetza::asn1::allocate<SituationContainer_t>();
    msg->denm.situation->informationQuality = 1;
    msg->denm.situation->eventType.causeCode = CauseCodeType_trafficCondition;
    msg->denm.situation->eventType.subCauseCode = 0;

    // TODO set road type in Location container
    // TODO set lane position in Alacarte container

    return msg;
}

vanetza::btp::DataRequestB ObuRwwLaneClosure::createRequest()
{
    namespace geonet = vanetza::geonet;
    using vanetza::units::si::seconds;
    using vanetza::units::si::meter;

    vanetza::btp::DataRequestB request;
    request.gn.traffic_class.tc_id(1);

    geonet::DataRequest::Repetition repetition;
    repetition.interval = 1.0 * seconds;
    repetition.maximum = 60.0 * seconds;

    request.gn.repetition = repetition;

    geonet::Area destination;
    geonet::Circle destination_shape;
    destination_shape.r = 1000.0 * meter;
    destination.shape = destination_shape;
    destination.position.latitude = mVdp->latitude();
    destination.position.longitude = mVdp->longitude();
    request.gn.destination = destination;

    return request;
}

} // namespace den
} // namespace artery
