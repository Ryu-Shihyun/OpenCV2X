package artery.application;

//
// InfrastructureMockReceiver can collect reception statistics for messages emitted by InfrastructureMockService
//
simple SpeedAdvisoryReceiver like ItsG5Service
{
    parameters:
        @signal[samReceived](type=SpeedAdvisoryMessage);
        @statistic[samReceived](source=samReceived;record=count,vector(samSource)?,vector(samSequenceNumber)?);

        @signal[samRelevance](type=bool);
        @statistic[samRelevance](source=samRelevance;record=vector);

        @signal[samDist](type=double);
        @statistic[samDist](source=samDist;record=vector);

        @signal[samTime](type=double);
        @statistic[samTime](source=samTime;record=vector);

        @signal[samLatency](type=simtime_t);
        @statistic[samLatency](source=samLatency;record=vector);
}
