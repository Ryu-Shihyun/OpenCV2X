#include "artery/networking/LimericDccEntity.h"
#include "artery/utility/PointerCheck.h"

using namespace omnetpp;

namespace artery
{

Define_Module(LimericDccEntity)

static const simsignal_t scSignalDCCDelta = cComponent::registerSignal("dccLimericDelta");

void LimericDccEntity::finish()
{
    // free those objects before runtime vanishes
    mTransmitRateControl.reset();
    mAlgorithm.reset();
    DccEntityBase::finish();
}

vanetza::dcc::TransmitRateThrottle* LimericDccEntity::getTransmitRateThrottle()
{
    return notNullPtr(mTransmitRateControl);
}

vanetza::dcc::TransmitRateControl* LimericDccEntity::getTransmitRateControl()
{
    return notNullPtr(mTransmitRateControl);
}

void LimericDccEntity::initializeTransmitRateControl()
{
    ASSERT(mRuntime);
    using namespace vanetza::dcc;

    Limeric::Parameters params;
    params.cbr_target = mTargetCbr;

    mAlgorithm.reset(new Limeric(*mRuntime, params));
    if (par("enableDualAlpha")) {
        Limeric::DualAlphaParameters dual_params;
        mAlgorithm->configure_dual_alpha(dual_params);
    }
    mTransmitRateControl.reset(new LimericTransmitRateControl(*mRuntime, *mAlgorithm));

    mAlgorithm->on_duty_cycle_change = [this](const Limeric*, vanetza::Clock::time_point) {
        mTransmitRateControl->update();
    };

    double delta = mAlgorithm->permitted_duty_cycle().value();

    emit(scSignalDCCDelta, delta);
}

void LimericDccEntity::onGlobalCbr(vanetza::dcc::ChannelLoad cbr)
{
    ASSERT(mAlgorithm);
    mAlgorithm->update_cbr(cbr);
}

} // namespace arterd
