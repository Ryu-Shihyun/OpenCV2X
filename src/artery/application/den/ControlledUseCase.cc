/*
 * Artery V2X Simulation Framework
 * Copyright 2016-2018 Raphael Riebl, Christina Obermaier
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/application/DenService.h"
#include "artery/application/LocalDynamicMap.h"
#include "artery/application/den/ControlledUseCase.h"
#include "artery/application/SampleBufferAlgorithm.h"
#include <boost/units/base_units/metric/hour.hpp>
#include <boost/units/systems/si/length.hpp>
#include <boost/units/systems/si/time.hpp>
#include <boost/units/systems/si/prefixes.hpp>
#include <omnetpp/csimulation.h>
#include <vanetza/btp/data_request.hpp>
#include <vanetza/facilities/cam_functions.hpp>
#include <vanetza/units/acceleration.hpp>
#include <vanetza/units/time.hpp>
#include <vanetza/units/velocity.hpp>
#include <algorithm>
#include <numeric>

static const auto hour = 3600.0 * boost::units::si::seconds;
static const auto km_per_hour = boost::units::si::kilo * boost::units::si::meter / hour;

using omnetpp::SIMTIME_S;
using omnetpp::SIMTIME_MS;

using namespace omnetpp;

Define_Module(artery::den::Controlled);

namespace artery
{
namespace den
{

void Controlled::initialize(int stage)
{
    UseCase::initialize(stage);
    if (stage == 0) {
        mDenmMemory = mService->getMemory();
        mLocalDynamicMap = &mService->getFacilities().get_const<LocalDynamicMap>();
        triggerTime = simTime() + par("triggerTime");
    }
}

void Controlled::check()
{
    if (!isDetectionBlocked() && checkConditions())
    {
        blockDetection();
        auto message = createMessage();
        auto request = createRequest();
        mService->sendDenm(std::move(message), request);
    }
}

bool Controlled::checkConditions()
{
    bool denmTrigger = false;

    if (simTime() > triggerTime){
        denmTrigger = true;
        double delay = 0.001 * intuniform(100, 1000, 0);
        triggerTime = simTime() + delay;
    }

    return denmTrigger;
}

vanetza::asn1::Denm Controlled::createMessage()
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

vanetza::btp::DataRequestB Controlled::createRequest()
{
    namespace geonet = vanetza::geonet;
    using vanetza::units::si::seconds;
    using vanetza::units::si::meter;

    vanetza::btp::DataRequestB request;
    request.gn.traffic_class.tc_id(1);

//    geonet::DataRequest::Repetition repetition;
//    repetition.interval = 1.0 * seconds;
//    repetition.maximum = 0 * seconds;
//
//    request.gn.repetition = repetition;

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
