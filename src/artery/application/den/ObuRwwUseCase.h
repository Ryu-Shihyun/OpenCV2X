/*
 * Artery V2X Simulation Framework
 * Copyright 2016-2018 Raphael Riebl, Christina Obermaier
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ARTERY_OBURWWUSECASE_H_TN78N6QA
#define ARTERY_OBURWWUSECASE_H_TN78N6QA

#include "artery/application/den/Memory.h"
#include "artery/application/den/SuspendableUseCase.h"
#include "artery/application/Sampling.h"
#include "artery/application/VehicleDataProvider.h"
#include <vanetza/units/velocity.hpp>
#include <vanetza/btp/data_request.hpp>

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

class ObuRwwMobileUnit : public SuspendableUseCase
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
    void indicate(const artery::DenmObject&) override {};
    void handleStoryboardTrigger(const StoryboardSignal&) override {};

protected:
    void initialize(int) override;

private:
    std::shared_ptr<const den::Memory> mDenmMemory;
    bool mMobileUnit;
};

class ObuRwwLaneClosure : public UseCase
{
public:
    /**
     * Switch if on-board map or camera sensor shall report a "non-urban environment".
     * A non-urban environment is the precondition for this use case, i.e. DENMs are only generated when set to true.
     * \param flag true if vehicle is assumed to be in an non-urban environment
     */

    vanetza::btp::DataRequestB createRequest();
    vanetza::asn1::Denm createMessage();

    void check() override{};
    void indicate(const artery::DenmObject&) override;
    void handleStoryboardTrigger(const StoryboardSignal&) override {};

protected:
    void initialize(int) override;

private:
    std::shared_ptr<const den::Memory> mDenmMemory;
    int mClosedLane;
};

} // namespace den
} // namespace artery

#endif /* ARTERY_RWWUSECASE_H_TN78N6QA */

