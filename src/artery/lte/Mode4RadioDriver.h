#ifndef Mode4RADIODRIVER_H_ZJ0SI5XC
#define Mode4RADIODRIVER_H_ZJ0SI5XC

#include "artery/nic/RadioDriverBase.h"
#include "artery/nic/ChannelLoadSampler.h"

#include "common/LteCommon.h"
#include "corenetwork/binder/LteBinder.h"

#include <omnetpp/clistener.h>
#include <omnetpp/csimplemodule.h>

namespace artery
{

    class VehicleDataProvider;

    class Mode4RadioDriver : public RadioDriverBase, public omnetpp::cListener
    {
        public:
            void initialize() override;
            void handleMessage(omnetpp::cMessage*) override;
            int getNodeID();
            void finish() override;
        protected:
            void handleDataIndication(omnetpp::cMessage*);
            void handleDataRequest(omnetpp::cMessage*) override;
            void receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t, bool, omnetpp::cObject*) override;

        private:
            omnetpp::cModule* mHost = nullptr;
            omnetpp::cGate* mLowerLayerOut = nullptr;
            omnetpp::cGate* mLowerLayerIn = nullptr;

            simsignal_t camGen;

            LteBinder* binder_;
            MacNodeId nodeId_;

            bool startUpComplete_;
            bool addStartUpDelay_;
            bool cbrStart_;

            int packetLatencyLimit_;
    };

} // namespace artery

#endif /* Mode4RADIODRIVER_H_ZJ0SI5XC */

