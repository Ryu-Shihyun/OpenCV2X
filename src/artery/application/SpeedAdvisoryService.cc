/*
* Artery V2X Simulation Framework
* Copyright 2019 Raphael Riebl et al.
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#include "artery/application/SpeedAdvisoryService.h"
#include "artery/application/SpeedAdvisoryMessage.h"
#include "artery/application/VehicleDataProvider.h"
#include <boost/format.hpp>
#include <omnetpp/cmessage.h>
#include <omnetpp/cwatch.h>
#include <vanetza/btp/ports.hpp>

namespace artery
{

Define_Module(SpeedAdvisoryService)

namespace {
    using namespace omnetpp;
    const simsignal_t SamSentSignal = cComponent::registerSignal("SamSent");
} // namespace

SpeedAdvisoryService::~SpeedAdvisoryService()
{
    cancelAndDelete(mTrigger);
}

void SpeedAdvisoryService::initialize()
{
    ItsG5Service::initialize();
    mTrigger = new omnetpp::cMessage("trigger speed advisory message");
    mPositionProvider = &getFacilities().get_const<PositionProvider>();
    mHostId = getFacilities().get_const<Identity>().host->getId();

    mPacketName = (boost::format("%1% packet") % this->getName()).str();

    mSpeedAdvice = par("speedAdvice");

    mNS = par("northSouth");

    if (mNS){
        mLatitudeN = par("latitudeN");
        mLatitudeS = par("latitudeS");
    } else {
        mLongitudeE = par("longitudeE");
        mLongitudeW = par("longitudeW");
    }

    mDirection = par("direction");

    mActive = par("active");

    mDisseminationRadius = par("disseminationRadius").doubleValue() * boost::units::si::meter;
    mPacketPriority = par("packetPriority");

    mInterval = par("generationInterval");
    if (mActive)
        scheduleAt(omnetpp::simTime() + par("generationOffset"), mTrigger);
}

void SpeedAdvisoryService::handleMessage(omnetpp::cMessage* msg)
{
    if (msg == mTrigger) {
        generatePacket();
        scheduleAt(omnetpp::simTime() + mInterval, mTrigger);
    } else {
        ItsG5Service::handleMessage(msg);
    }
}

void SpeedAdvisoryService::generatePacket()
{
    using namespace vanetza;
    btp::DataRequestB req;
    req.destination_port = btp::ports::IVIM;
    req.gn.transport_type = geonet::TransportType::GBC;
    req.gn.traffic_class.tc_id(mPacketPriority);
    req.gn.communication_profile = geonet::CommunicationProfile::ITS_G5;

    geonet::Area destination;
    geonet::Circle circle;
    circle.r = mDisseminationRadius;
    destination.shape = circle;
    destination.position.latitude = mPositionProvider->getGeodeticPosition().latitude;
    destination.position.longitude = mPositionProvider->getGeodeticPosition().longitude;
    req.gn.destination = destination;
    req.gn.message_category = 2;
    int time_interval = int(mInterval.dbl() * 10);
    req.gn.message_rate = time_interval;

    auto packet = new SpeedAdvisoryMessage();
    packet->setSourceStation(mHostId);
    packet->setSequenceNumber(mSequenceNumber++);
    packet->setByteLength(mMessageLength);
    packet->setSpeedAdvice(mSpeedAdvice);

    if (mNS){
        packet->setLatitudeN(mLatitudeN);
        packet->setLatitudeS(mLatitudeS);
    } else {
        packet->setLongitudeE(mLongitudeE);
        packet->setLongitudeW(mLongitudeW);
    }

    packet->setNorthSouth(mNS);
    packet->setDirection(mDirection);

    double rand = uniform(0, 1, 0);
    if (rand < 0.5) {
        packet->setByteLength(478);
        packet->setBitLength(478 * 8);
        req.gn.fixed_length = 478;
    } else {
        packet->setByteLength(482);
        packet->setBitLength(482 * 8);
        req.gn.fixed_length = 482;
    }

    emit(SamSentSignal, packet);
    request(req, packet);
}

} // namespace artery
