#include "artery/application/SpeedAdvisoryMessage.h"
#include <omnetpp.h>

using namespace omnetpp;

namespace artery
{

Register_Class(SpeedAdvisoryMessage)

SpeedAdvisoryMessage::SpeedAdvisoryMessage() :
    cPacket("Speed Advisory message"),
    mGenerated(omnetpp::simTime())
{
}

cPacket* SpeedAdvisoryMessage::dup() const
{
    return new SpeedAdvisoryMessage(*this);
}


class samSourceResultFilter : public cObjectResultFilter
{
protected:
    void receiveSignal(cResultFilter* prev, simtime_t_cref t, cObject* object, cObject* details) override
    {
        if (auto msg = dynamic_cast<SpeedAdvisoryMessage*>(object)) {
            fire(this, t, static_cast<long>(msg->getSourceStation()), details);
        }
    }
};

Register_ResultFilter("samSource", samSourceResultFilter)


class samSequenceNumberResultFilter : public cObjectResultFilter
{
protected:
    void receiveSignal(cResultFilter* prev, simtime_t_cref t, cObject* object, cObject* details) override
    {
        if (auto msg = dynamic_cast<SpeedAdvisoryMessage*>(object)) {
            fire(this, t, static_cast<long>(msg->getSequenceNumber()), details);
        }
    }
};

Register_ResultFilter("samSequenceNumber", samSequenceNumberResultFilter)

} // namespace artery
