/*
* Artery V2X Simulation Framework
* Copyright 2018 Raphael Riebl
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#include "artery/inet/winner/B1.h"
#include <inet/common/ModuleAccess.h>
#include <inet/common/Units.h>
#include <inet/physicallayer/contract/packetlevel/IRadioMedium.h>
#include <algorithm>
#include <array>
#include <iterator>
#include <list>

using namespace inet;

namespace artery
{
namespace winner
{

Define_Module(B1)

void B1::initialize(int stage)
{
    FreeSpacePathLoss::initialize(stage);
}

std::ostream& B1::printToStream(std::ostream& stream, int level) const
{
    stream << "WINNER B1";
    if (level <= PRINT_LEVEL_TRACE)
        stream << ", alpha = " << alpha
               << ", systemLoss = " << systemLoss;
    return stream;
}

double B1::computePathLoss(const physicallayer::ITransmission *transmission, const physicallayer::IArrival *arrival) const
{
    auto radioMedium = transmission->getTransmitter()->getMedium();
    auto narrowbandSignalAnalogModel = check_and_cast<const physicallayer::INarrowbandSignal *>(transmission->getAnalogModel());
    const mps propagationSpeed = radioMedium->getPropagation()->getPropagationSpeed();
    const Hz carrierFrequency = narrowbandSignalAnalogModel->getCarrierFrequency();
    const m waveLength = propagationSpeed / carrierFrequency;
    m dist { transmission->getStartPosition().distance(arrival->getStartPosition()) };

    double pathLoss = 0;
    double pathLossFree = 0;

    double environmentHeight = 0;

    double distance = transmission->getStartPosition().distance(arrival->getStartPosition());

    double hTx = transmission->getStartPosition().z;
    double hRx = arrival->getStartPosition().z;

    double dBP = 4 * (hTx - environmentHeight) * (hRx - environmentHeight) * carrierFrequency.get() / propagationSpeed.get();

    if (dist.get() < 3){
        dist = m (3);
    }

    if (distance < dBP){
        pathLoss = 22.7 * std::log10(dist.get()) + 27 + 20 * std::log10(carrierFrequency.get()/1e9);
    } else {
        pathLoss = 40*std::log10(dist.get()) + 7.56 - 17.3*std::log10(hRx-environmentHeight) - 17.3*std::log10(hTx-environmentHeight) + 2.7*std::log10(carrierFrequency.get()/1e9);
    }

    pathLossFree = 20 * std::log10(dist.get()) + 46.4 + 20 * std::log10(carrierFrequency.get() * 1e-9 / 5);

    pathLoss = std::max(pathLossFree, pathLoss);

    return pathLoss;
}

double B1::computePathLoss(mps propagationSpeed, Hz carrierFrequency, m distance) const
{
    // insufficient input data, need LOS path for loss computation
    return NaN;
}

} // namespace winner
} // namespace artery
