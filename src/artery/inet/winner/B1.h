/*
 * Artery V2X Simulation Framework
 * Copyright 2018 Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ARTERY_WINNER_B1_H_UHSKL156
#define ARTERY_WINNER_B1_H_UHSKL156

#include <inet/physicallayer/pathloss/FreeSpacePathLoss.h>

namespace artery
{
namespace winner
{

class B1 : public inet::physicallayer::FreeSpacePathLoss
{
public:
    void initialize(int stage) override;
    std::ostream& printToStream(std::ostream& stream, int level) const override;
    double computePathLoss(const inet::physicallayer::ITransmission *transmission, const inet::physicallayer::IArrival *arrival) const override;
    double computePathLoss(inet::mps propagation, inet::Hz frequency, inet::m distance) const override;
};

} // namespace gemv2
} // namespace artery

#endif /* ARTERY_GEMV2_NLOSF_H_WLNRGJIS */
