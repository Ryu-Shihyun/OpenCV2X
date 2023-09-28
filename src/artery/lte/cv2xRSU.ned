package artery.lte;

import artery.application.StationaryMiddleware;
import artery.networking.Vanetza;
import artery.nic.IRadioDriver;
import inet.node.inet.INetworkNode;
import inet.linklayer.contract.IWirelessNic;
import inet.mobility.contract.IMobility;
import inet.networklayer.common.InterfaceTable;

import lte.stack.phy.ILteNic;

module cv2xRSU like INetworkNode
{
    parameters:
        string nicType = default("LteNicVUeMode4");
        string radioType = default("Mode4RadioDriver");
        string dccMechanism = default("FsmDccEntity");
        bool withAntennaMobility = default(false);
        int macNodeId = default(0);
        int masterId = default(0); // Not yet removed from SimuLTE for Mode 4 simulations
        double chance = default(0.0);

        @networkNode;
        @labels(node,ethernet-node,wireless-node);

        int numRadios = default(1);

    gates:
        input lteRadioIn @directIn;
        input radioIn[numRadios] @directIn;
        inout pppg[] @labels(PPPFrame-conn);
        inout ethg[] @labels(EtherFrame-conn);

    submodules:
        lteNic: <nicType> like ILteNic {
            @display("p=150,400");
            nodeType = "UE";
        }

        radioDriver: <radioType> like IRadioDriver {
            parameters:
                @display("p=250,300");
        }

        antennaMobility[numRadios]: AntennaMobility if withAntennaMobility == true {
            parameters:
                @display("p=100,200");
                mobilityModule = "^.mobility";
        }

        vanetza: Vanetza {
            parameters:
                @display("p=250,200");
                *.middlewareModule = absPath("^.middleware");
                *.mobilityModule = absPath("^.mobility");
                *.radioDriverModule = absPath("^.radioDriver");
                *.traciCoreModule = "traci.core";
                position.typename = default("StationaryPositionProvider");
                runtime.datetime = middleware.datetime;
                router.isMobile = false;
        }

        mobility: <default("inet.mobility.static.StationaryMobility")> like IMobility {
            parameters:
                @display("p=200,200");
                initFromDisplayString = false;
        }

        middleware: StationaryMiddleware {
            parameters:
                @display("p=50,100");
        }

    connections allowunconnected:
        lteNic.radioIn <-- lteRadioIn;

        radioDriver.upperLayer <--> vanetza.radioDriverData;
        radioDriver.properties --> vanetza.radioDriverProperties;

        radioDriver.lowerLayerOut --> lteNic.upperLayerIn;
        radioDriver.lowerLayerIn <-- lteNic.upperLayerOut;
}
