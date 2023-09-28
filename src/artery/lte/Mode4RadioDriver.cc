#include "artery/networking/GeoNetIndication.h"
#include "artery/networking/GeoNetRequest.h"
#include "artery/nic/RadioDriverProperties.h"
#include "artery/lte/Mode4RadioDriver.h"
#include "veins/base/utils/FindModule.h"
#include <vanetza/btp/data_interface.hpp>

#include "common/LteControlInfo.h"
#include "stack/phy/packet/cbr_m.h"

using namespace omnetpp;

namespace artery
{

Register_Class(Mode4RadioDriver)

namespace {

long convert(const vanetza::MacAddress& mac)
{
    long addr = 0;
    for (unsigned i = 0; i < mac.octets.size(); ++i) {
        addr <<= 8;
        addr |= mac.octets[i];
    }
    return addr;
}

vanetza::MacAddress convert(long addr)
{
    vanetza::MacAddress mac;
    for (unsigned i = mac.octets.size(); i > 0; --i) {
        mac.octets[i - 1] = addr & 0xff;
        addr >>= 8;
    }
    return mac;
}

int user_priority(vanetza::access::AccessCategory ac)
{
    using AC = vanetza::access::AccessCategory;
    int up = 0;
    switch (ac) {
        case AC::BK:
            up = 1;
            break;
        case AC::BE:
            up = 0;
            break;
        case AC::VI:
            up = 5;
            break;
        case AC::VO:
            up = 7;
            break;
    }
    return up;
}

const simsignal_t channelBusySignal = cComponent::registerSignal("sigChannelBusy");

} // namespace

void Mode4RadioDriver::initialize()
{
    addStartUpDelay_ = par("addStartUpDelay");
    packetLatencyLimit_ = par("packetLatencyLimit");
    startUpComplete_ = false;
    cbrStart_ = false;

    RadioDriverBase::initialize();
    mHost = veins::FindModule<>::findHost(this);
    mHost->subscribe(channelBusySignal, this);

    mLowerLayerOut = gate("lowerLayerOut");
    mLowerLayerIn = gate("lowerLayerIn");

    if (addStartUpDelay_) {
        cMessage *startUpMessage = new cMessage("StartUpMsg");
        double delay = 0.001 * intuniform(0, 1000, 0);
        scheduleAt((simTime() + delay).trunc(SIMTIME_MS), startUpMessage);

        cMessage *startUpCBRMessage = new cMessage("StartUpCBRMsg");
        double cbrDelay = 0.001 * intuniform(0, 1000, 0);
        scheduleAt((simTime() + cbrDelay).trunc(SIMTIME_MS), startUpCBRMessage);
    } else {
        startUpComplete_ = true;
        cbrStart_ = true;
    }

    auto properties = new RadioDriverProperties();
    properties->LinkLayerAddress = vanetza::create_mac_address(mHost->getIndex());
    // CCH used to ensure DCC configures correctly.
    properties->ServingChannel = channel::CCH;
    indicateProperties(properties);

    camGen = registerSignal("camGen");

    binder_ = getBinder();

    cModule *ue = getParentModule();
    nodeId_ = binder_->registerNode(ue, UE, 0);
    binder_->setMacNodeId(convert(properties->LinkLayerAddress), nodeId_);
}

void Mode4RadioDriver::finish()
{
    binder_->unregisterNode(nodeId_);
}

void Mode4RadioDriver::handleMessage(cMessage* msg){
    if (strcmp(msg->getName(),"CBR") == 0) {
        Cbr* cbrPkt = check_and_cast<Cbr*>(msg);
        double channel_load = cbrPkt->getCbr();
        double channel_occupancy = cbrPkt->getCr();
        delete cbrPkt;
        if (cbrStart_) {
            emit(RadioDriverBase::ChannelLoadSignal, channel_load);
            emit(RadioDriverBase::ChannelOccupancySignal, channel_occupancy);
        }
    } else if (RadioDriverBase::isDataRequest(msg)) {
        handleDataRequest(msg);
    } else if (msg->getArrivalGate() == mLowerLayerIn) {
        handleDataIndication(msg);
    } else if (strcmp(msg->getName(), "StartUpMsg") == 0) {
        startUpComplete_ = true;
    } else if (strcmp(msg->getName(), "StartUpCBRMsg") == 0) {
        cbrStart_ = true;
    } else {
        throw cRuntimeError("unexpected message");
    }
}

void Mode4RadioDriver::handleDataIndication(cMessage* packet)
{
    auto* lteControlInfo = check_and_cast<FlowControlInfoNonIp*>(packet->removeControlInfo());
    auto* indication = new GeoNetIndication();
    indication->source = convert(lteControlInfo->getSrcAddr());
    indication->destination = convert(lteControlInfo->getDstAddr());
    packet->setControlInfo(indication);
    delete lteControlInfo;

    indicateData(packet);
}

void Mode4RadioDriver::handleDataRequest(cMessage* packet)
{
    if (startUpComplete_) {
        auto request = check_and_cast<GeoNetRequest *>(packet->removeControlInfo());

        cPacket* pkt = check_and_cast<cPacket*>(packet);

        auto lteControlInfo = new FlowControlInfoNonIp();

        lteControlInfo->setSrcAddr(convert(request->source_addr));
        lteControlInfo->setDstAddr(convert(vanetza::cBroadcastMacAddress));
        lteControlInfo->setPriority(user_priority(request->access_category));

        // Want to be able to fill these automatically
        if (request->message_rate > 0) {
            lteControlInfo->setRRI(request->message_rate);
        } else {
            lteControlInfo->setRRI(-1);
        }

        if (request->message_category > 0) {
            lteControlInfo->setMessageCategory(request->message_category);
        } else{
            delete packet;
            delete request;
            return;
        }

        lteControlInfo->setDuration(packetLatencyLimit_);
        lteControlInfo->setCreationTime(packet->getCreationTime());

        lteControlInfo->setDirection(D2D_MULTI);

        pkt->setControlInfo(lteControlInfo);

        if (request->fixed_length > 0) {
            pkt->setByteLength(request->fixed_length);
        } else {
            delete packet;
            delete request;
            return;
        }

        emit(camGen, 1);

        delete request;

        send(pkt, mLowerLayerOut);
    } else {
        delete packet;
    }
}

int Mode4RadioDriver::getNodeID() {
    return nodeId_;
}

void Mode4RadioDriver::receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t signal, bool busy, omnetpp::cObject*)
{
}

} // namespace artery
