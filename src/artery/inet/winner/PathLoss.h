/*
 * Artery V2X Simulation Framework
 * Copyright 2017 Thiago Vieira, Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef PATHLOSS_H_ZABKB47G
#define PATHLOSS_H_ZABKB47G

#include <inet/common/Units.h>
#include <inet/physicallayer/contract/packetlevel/IPathLoss.h>
#include <omnetpp/csimplemodule.h>

namespace artery
{
namespace winner
{

class PathLoss : public omnetpp::cSimpleModule, public inet::physicallayer::IPathLoss
{
public:
    PathLoss();

    // OMNeT++ simple module
    void initialize() override;

    // INET IPathLoss interface
    double computePathLoss(const inet::physicallayer::ITransmission*, const inet::physicallayer::IArrival*) const override;
    double computePathLoss(inet::mps, inet::Hz, inet::m) const override;
    inet::m computeRange(inet::mps, inet::Hz, double loss) const override;

private:
    using meter = inet::m;

    inet::physicallayer::IPathLoss* m_b1;
    inet::physicallayer::IPathLoss* m_log_normal;
    inet::physicallayer::IPathLoss* m_nakagami;

    bool mWithShadowing;
    bool mWithSensing;
    bool mWithFading;
};

} // namespace winner
} // namespace artery

#endif /* PATHLOSS_H_ZABKB47G */

