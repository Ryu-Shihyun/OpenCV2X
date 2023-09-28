/*
 * Artery V2X Simulation Framework
 * Copyright 2017 Thiago Vieira, Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/inet/winner/PathLoss.h"
#include "artery/utility/Geometry.h"
#include "inet/physicallayer/contract/packetlevel/IRadioMedium.h"
#include "inet/physicallayer/analogmodel/packetlevel/ScalarReception.h"
#include "inet/physicallayer/base/packetlevel/ScalarAnalogModelBase.h"
#include <omnetpp/checkandcast.h>
#include <omnetpp/cexception.h>
#include <inet/physicallayer/contract/packetlevel/IRadioSignal.h>

namespace artery
{
namespace winner
{

Define_Module(PathLoss)

using namespace inet;
namespace phy = inet::physicallayer;

PathLoss::PathLoss() :
    m_b1(nullptr),m_log_normal(nullptr),m_nakagami(nullptr)
{
}

void PathLoss::initialize()
{
    m_b1 = check_and_cast<IPathLoss*>(getSubmodule("B1"));

    m_log_normal = check_and_cast<IPathLoss*>(getSubmodule("shadowing"));
    m_nakagami = check_and_cast<IPathLoss*>(getSubmodule("fading"));

    mWithShadowing = par("withShadowing");
    mWithFading = par("withFading");
    mWithSensing = par("withSensing");
}

double PathLoss::computePathLoss(const phy::ITransmission* transmission, const phy::IArrival* arrival) const
{
    // We have the option of adding the additional WINNER models at some point so if needed this can be extended
    // To use the model of choice but B1 is the default and as such will be used.

    double loss = m_b1->computePathLoss(transmission, arrival);

    bool sensed = true;

    if (mWithSensing){
        const inet::physicallayer::IScalarSignal *scalarSignalAnalogModel = check_and_cast<const inet::physicallayer::IScalarSignal *>(transmission->getAnalogModel());
        W power = scalarSignalAnalogModel->getPower();

        double powerdbm = 10 * log10(power.get()) + 30;

        double erfParam = (powerdbm - loss - -90.5) / (3 * sqrt(2));
        double erfValue = erf(erfParam);
        double packetSensingRatio = 0.5 * (1 + erfValue);
        double er = dblrand(0);

        if (er > packetSensingRatio){
            // Basically we can't sense it so loss is now maximum
            loss = 100000;
            sensed = false;
        }
    }

    if(mWithShadowing & sensed){
        // LogNormalShadowing::computePathLoss(mps propagationSpeed, Hz frequency, m distance)
        loss += m_log_normal->computePathLoss(transmission, arrival);
    }

    if(mWithFading & sensed){
        // NakagamiFading::computePathLoss(mps propagationSpeed, Hz frequency, m distance)
        loss -= m_nakagami->computePathLoss(transmission, arrival);
    }

    return loss;
}

double PathLoss::computePathLoss(mps, Hz, m) const
{
    throw omnetpp::cRuntimeError("Incompatible usage of Winner path loss model");
    return 1.0;
}

m PathLoss::computeRange(mps, Hz, double loss) const
{
    return m (1000.0);
}

} // namespace gemv2
} // namespace artery
