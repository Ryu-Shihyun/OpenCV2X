package artery.application;

//
// InfrastructureMockService mimicks a service typically operated by a RSU.
// Its messages are disseminated as GeoBroadcast packets centred around the RSU's location.
// If the actual content of an RSU message is not relevant in your simulation, e.g. you are
// only interested in the impact on the channel congestion, you can employ this service to
// produce load similar to SPATEM and MAPEM messages.
//
simple SpeedAdvisoryService like ItsG5Service
{
    parameters:
        @signal[SamSent](type=SpeedAdvisoryMessage);
        @statistic[transmission](source=SamSent; record=count,vector(samSource)?,vector(samSequenceNumber)?);

        // generation interval
        double generationInterval @unit(s) = default(1s);
        double generationOffset @unit(s) = default(uniform(0.0s, 0.2s));
        bool active = default(true);
        int packetPriority = default(3);
        double disseminationRadius @unit(m) = default(800m);

        double speedAdvice = default(22.22);
        bool northSouth = default(true);
        double latitudeN = default(0.0);
        double latitudeS = default(0.0);
        double longitudeE = default(0.0);
        double longitudeW = default(0.0);
        double length = default(500.0);
        double direction = default(-1); // Represents that it applies to all directions
}
