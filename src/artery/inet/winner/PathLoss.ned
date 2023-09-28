package artery.inet.winner;

import inet.physicallayer.contract.packetlevel.IPathLoss;

module winner like IPathLoss
{
    parameters:
        @class(winner::PathLoss);
        bool withShadowing = default(true);
        bool withSensing = default(true);
        bool withFading = default(true);

    submodules:
        B1: <default("B1")> like IPathLoss {
            @display("p=20,20");
        }

        LOS: <default("TwoRayInterference")> like IPathLoss {
                    @display("p=20,20");
                }

        shadowing: <default("LogNormalShadowing")> like IPathLoss{
            @display("p=20,40");
        }

        fading: <default("NakagamiFading")> like IPathLoss{
            @display("p=20,60");
        }
}
