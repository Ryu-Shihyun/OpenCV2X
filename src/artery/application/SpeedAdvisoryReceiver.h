/*
* Artery V2X Simulation Framework
* Copyright 2020 Raphael Riebl et al.
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#ifndef ARTERY_SPEEDADVISORYRECEIVER_H_3JS8PA4K
#define ARTERY_SPEEDADVISORYRECEIVER_H_3JS8PA4K

#include "artery/application/ItsG5Service.h"
#include "artery/traci/VehicleController.h"
#include <vanetza/units/velocity.hpp>
#include <omnetpp/simtime.h>

namespace artery
{

class VehicleDataProvider;

class SpeedAdvisoryReceiver : public artery::ItsG5Service
{
    public:
        virtual ~SpeedAdvisoryReceiver();
        using Velocity = vanetza::units::Velocity;

    protected:
        void initialize() override;
        void indicate(const vanetza::btp::DataIndication&, omnetpp::cPacket*) override;
        static double getDistanceFromLatLonInm(double lat1, double lon1, double lat2, double lon2);

    private:
        traci::VehicleController* mVehicleController = nullptr;
        const VehicleDataProvider* mVehicleDataProvider = nullptr;
        bool mReceived = false;
};

} // namespace artery

#endif /* ARTERY_INFRASTRUCTUREMOCKRECEIVER_H_3JS8PA4K */

