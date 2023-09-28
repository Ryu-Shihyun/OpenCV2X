/*
* Artery V2X Simulation Framework
* Copyright 2020 Raphael Riebl et al.
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#include "artery/application/SpeedAdvisoryReceiver.h"
#include "artery/application/SpeedAdvisoryMessage.h"
#include "artery/traci/VehicleController.h"
#include "artery/application/VehicleDataProvider.h"
#include <omnetpp/checkandcast.h>
#include <omnetpp/cmessage.h>
#include <omnetpp/cwatch.h>
#include <omnetpp/ccomponenttype.h>
#include <omnetpp/cxmlelement.h>
#include "inet/common/ModuleAccess.h"


namespace si = boost::units::si;

namespace traci { class VehicleController; }

namespace artery
{

using namespace omnetpp;

Define_Module(SpeedAdvisoryReceiver)

static const simsignal_t scSignalSamReceived      = cComponent::registerSignal("samReceived");
static const simsignal_t scSignalSamRelevance     = cComponent::registerSignal("samRelevance");
static const simsignal_t scSignalDistanceToSamPOI = cComponent::registerSignal("samDist");
static const simsignal_t scSignalTimeToSamPOI     = cComponent::registerSignal("samTime");
static const simsignal_t scSignalSamLatency       = cComponent::registerSignal("samLatency");


void SpeedAdvisoryReceiver::initialize()
{
    ItsG5BaseService::initialize();

    mVehicleDataProvider = &getFacilities().get_const<VehicleDataProvider>();
    mReceived = false;
}

SpeedAdvisoryReceiver::~SpeedAdvisoryReceiver()
{
}

double SpeedAdvisoryReceiver::getDistanceFromLatLonInm(double lat1, double lon1, double lat2, double lon2) {
    double haversine;
    double temp;
    double radius_earth = 6372797.56085;
    double radians = M_PI / 180;

    lat1  = lat1  * radians;
    lon1 = lon1 * radians;
    lat2  = lat2  * radians;
    lon2 = lon2 * radians;

    haversine = (pow(sin((1.0 / 2) * (lat2 - lat1)), 2)) + ((cos(lat1)) * (cos(lat2)) * (pow(sin((1.0 / 2) * (lon2 - lon1)), 2)));
    temp = 2 * asin(std::min(1.0, sqrt(haversine)));

    return radius_earth * temp;
}

void SpeedAdvisoryReceiver::indicate(const vanetza::btp::DataIndication&, cPacket* packet)
{
    auto msg = check_and_cast<SpeedAdvisoryMessage*>(packet);
    double speedAdvice = msg->getSpeedAdvice();
    bool northSouth = msg->getNorthSouth();
    double direction = msg->getDirection();
    bool relevance = false;

    auto& vehicleController = getFacilities().get_mutable<traci::VehicleController>();
    const std::string id = vehicleController.getVehicleId();
    auto& vehicle_api = vehicleController.getTraCI()->vehicle;

    double heading = mVehicleDataProvider->heading().value();

    if (!mReceived){

        if (direction == -1){
            // Message applies to both directions
            relevance = true;
        } else {
            double headingDiff = 180 - abs(abs(heading - direction) - 180);
            if (headingDiff < 1.57) {
                // if within 90 degrees
                relevance = true;
            }
        }

        if (relevance){
            auto currLong = mVehicleDataProvider->longitude().value();
            auto currLat = mVehicleDataProvider->latitude().value();
            double dist = 0.0;
            bool negative = false;

            if (northSouth){
                double latitude;
                // North is 0 radians as heading
                if ((180 - abs(abs(heading - 0) - 180)) < 3.14){
                    // Heading north.
                    latitude = msg->getLatitudeS();
                    if (currLat > latitude)
                        negative = true;
                } else {
                    latitude = msg->getLatitudeN();
                    if (currLat < latitude)
                        negative = true;
                }
                dist = getDistanceFromLatLonInm(latitude, currLong, currLat, currLong);
            } else {
                double longitude;
                // West is 1.57 radians as heading
                if ((180 - abs(abs(heading - 1.57) - 180)) < 3.14){
                    // Heading west.
                    longitude = msg->getLongitudeE();
                    if (currLong > longitude)
                        negative = true;
                } else {
                    longitude = msg->getLongitudeW();
                    if (currLong < longitude)
                        negative = true;
                }
                dist = getDistanceFromLatLonInm(currLat, longitude, currLat, currLong);
            }

            // calculate time to POI before speed adjustment
            double speed = vehicleController.getSpeed().value();
            double time = dist/speed; // This is slightly simplified given it doesn't account for the nature of the road

            // Take action on message
            vehicle_api.slowDown(id, speedAdvice, 30);

            if (negative) {
                dist = dist * -1;
                time = time * -1;
            }

            // Now record the information
             emit(scSignalDistanceToSamPOI, dist);
             emit(scSignalTimeToSamPOI, time);
        }
    }

    omnetpp::SimTime latency = simTime() - msg->getGenerationTimestamp();
    auto recordMsg = check_and_cast<SpeedAdvisoryMessage*>(packet);
    emit(scSignalSamReceived, recordMsg);
    emit(scSignalSamRelevance, relevance);
    emit(scSignalSamLatency, latency);
    delete packet;
}

} // namespace artery
