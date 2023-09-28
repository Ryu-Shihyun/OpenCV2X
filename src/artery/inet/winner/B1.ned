package artery.inet.winner;

import inet.physicallayer.pathloss.FreeSpacePathLoss;

module B1 extends FreeSpacePathLoss
{
    parameters:
        @display("i=block/control");
        @class(winner::B1);
}
