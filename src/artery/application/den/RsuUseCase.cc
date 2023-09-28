#include "artery/application/den/RsuUseCase.h"
#include "artery/application/RsuDenService.h"
#include "artery/application/StoryboardSignal.h"
#include "artery/utility/Geometry.h"
#include "artery/utility/Identity.h"
#include <boost/units/systems/si/prefixes.hpp>
#include <omnetpp/checkandcast.h>
#include <cmath>

namespace artery
{
namespace den
{

static const auto decidegree = vanetza::units::degree * boost::units::si::deci;
static const auto centimeter_per_second = vanetza::units::si::meter_per_second * boost::units::si::centi;

template<typename T, typename U>
long round(const boost::units::quantity<T>& q, const U& u)
{
    boost::units::quantity<U> v { q };
    return std::round(v.value());
}

void RsuUseCase::initialize(int stage)
{
    if (stage == 0) {
        mService = omnetpp::check_and_cast<RsuDenService*>(getParentModule());
        mIdentity = &mService->getFacilities().get_const<Identity>();
        mGeoPosition = &mService->getFacilities().get_const<GeoPosition>();
    }
}

vanetza::asn1::Denm RsuUseCase::createMessageSkeleton()
{
    vanetza::asn1::Denm message;
    message->header.protocolVersion = 1;
    message->header.messageID = ItsPduHeader__messageID_denm;
    message->header.stationID = mIdentity->application;

    // Do not copy ActionID itself (it also contains a context object)
    auto action_id = mService->requestActionID();
    message->denm.management.actionID.originatingStationID = action_id.originatingStationID;
    message->denm.management.actionID.sequenceNumber = action_id.sequenceNumber;
    message->denm.management.stationType = StationType_roadSideUnit;

    int ret = 0;
    const uint16_t genDeltaTime = countTaiMilliseconds(mService->getTimer()->getCurrentTime());
    ret += asn_long2INTEGER(&message->denm.management.detectionTime, genDeltaTime);
    ret += asn_long2INTEGER(&message->denm.management.referenceTime, genDeltaTime);
    assert(ret == 0);

    message->denm.location = vanetza::asn1::allocate<LocationContainer_t>();
    message->denm.location->eventSpeed = vanetza::asn1::allocate<Speed>();
    message->denm.location->eventSpeed->speedValue = SpeedValue_unavailable;
    message->denm.location->eventSpeed->speedConfidence = SpeedConfidence_equalOrWithinOneCentimeterPerSec * 3;
    message->denm.location->eventPositionHeading = vanetza::asn1::allocate<Heading>();
    message->denm.location->eventPositionHeading->headingValue = HeadingValue_unavailable;
    message->denm.location->eventPositionHeading->headingConfidence = HeadingConfidence_equalOrWithinOneDegree;

    // TODO fill path history
    auto path_history = vanetza::asn1::allocate<PathHistory_t>();
    asn_sequence_add(&message->denm.location->traces, path_history);

    return message;
}

} // namespace den
} // namespace artery
