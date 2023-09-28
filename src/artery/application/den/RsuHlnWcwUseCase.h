/*
 * Artery V2X Simulation Framework
 * Copyright 2016-2018 Raphael Riebl, Christina Obermaier
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ARTERY_RSUHLNWCWUSECASE_H
#define ARTERY_RSUHLNWCWUSECASE_H

#include "artery/application/den/Memory.h"
#include "artery/application/den/RsuUseCase.h"
#include "artery/application/Sampling.h"
#include <vanetza/units/velocity.hpp>

namespace artery
{

// forward declaration
class LocalDynamicMap;

namespace den
{

/**
 * Check triggering conditions for "Dangerous End Of Queue" use case.
 * See release 1.1.0 of C2C-CC Triggering Conditions "Traffic Jam" (Version 3.3.0)
 */
class RsuHlnWcw : public RsuUseCase
{
public:
    /**
     * Switch if on-board map or camera sensor shall report a "non-urban environment".
     * A non-urban environment is the precondition for this use case, i.e. DENMs are only generated when set to true.
     * \param flag true if vehicle is assumed to be in an non-urban environment
     */

    vanetza::btp::DataRequestB createRequest();
    vanetza::asn1::Denm createMessage();

    void check() override;
    void indicate(const artery::DenmObject&);
    void handleStoryboardTrigger(const StoryboardSignal&) override {};

protected:
    void initialize(int) override;

private:
    // TODO: Think about packet sizes for these types of packets.
    omnetpp::SimTime mInterval;
    omnetpp::SimTime mLastTransmission;
    int mCauseCode;
    int mPacketSize;
    int mSubCauseCode;
};

} // namespace den
} // namespace artery

#endif /* ARTERY_RWWUSECASE_H_TN78N6QA */

