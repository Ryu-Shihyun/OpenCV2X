/*
* Artery V2X Simulation Framework
* Copyright 2019 Raphael Riebl et al.
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#ifndef ARTERY_SPEEDADVISORYSERVICE_H_9QTWT1BY
#define ARTERY_SPEEDADVISORYSERVICE_H_9QTWT1BY

#include "artery/application/ItsG5Service.h"
#include "artery/networking/PositionProvider.h"
#include <vanetza/units/length.hpp>
#include <string>

namespace artery
{

class SpeedAdvisoryService : public artery::ItsG5Service
{
    public:
        virtual ~SpeedAdvisoryService();

    protected:
        void initialize() override;
        void handleMessage(omnetpp::cMessage*) override;
        void generatePacket();

    private:
        int mHostId = 0;
        int mSequenceNumber = 0;
        double mSpeedAdvice = 0;
        const PositionProvider* mPositionProvider = nullptr;
        omnetpp::cMessage* mTrigger = nullptr;
        omnetpp::SimTime mInterval = omnetpp::SimTime::ZERO;
        vanetza::units::Length mDisseminationRadius;
        std::string mPacketName;
        unsigned mPacketPriority = 0;
        unsigned mMessageLength = 0;
        bool mNS;
        bool mActive;
        double mLatitudeN;
        double mLatitudeS;
        double mLongitudeE;
        double mLongitudeW;
        double mDirection;
};

} // namespace artery

#endif /* ARTERY_SPEEDADVISORYSERVICE_H_9QTWT1BY */

