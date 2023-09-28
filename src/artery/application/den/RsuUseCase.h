/*
 * Artery V2X Simulation Framework
 * Copyright 2018 Christina Obermaier
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ARTERY_DEN_RSUUSECASE_H_
#define ARTERY_DEN_RSUUSECASE_H_

#include <omnetpp/csimplemodule.h>
#include <vanetza/asn1/denm.hpp>
#include <vanetza/btp/data_request.hpp>
#include <functional>

namespace artery
{

class DenmObject;
class RsuDenService;
class StoryboardSignal;
class GeoPosition;
class Identity;

namespace den
{
class RsuUseCase : public omnetpp::cSimpleModule
{
public:
    /**
     * Evaluate use case triggering conditions
     * Generates DENM if neccessary
     * \note invoked for each middleware time step (trigger)
     */
    virtual void check() = 0;

    /**
     * Forwards the message received by DenService to the UseCase
     * Messages responding to this can be triggered immediately
     */
    virtual void indicate(const artery::DenmObject&) = 0;

    /**
     * Forwards the received Storyboard trigger signal to the UseCase
     * \note Do not invoke message sending in this method!
     * \note Delay sending until check() is called again.
     * \note This avoids station synchronization.
     */
    virtual void handleStoryboardTrigger(const StoryboardSignal&) = 0;

    void initialize(int) override;
    int numInitStages () const override { return 1; }

protected:
    using TriggeringCondition = std::function<bool(void)>;

    virtual vanetza::asn1::Denm createMessageSkeleton();

    const Identity* mIdentity = nullptr;
    const GeoPosition* mGeoPosition = nullptr;

    RsuDenService* mService = nullptr;
};

} // namespace den
} // namespace artery

#endif /* ARTERY_DEN_USECASE_H_ */
