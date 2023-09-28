package artery.networking;

simple LimericDccEntity like IDccEntity
{
    parameters:
        @class(LimericDccEntity);
        string radioDriverModule;
        string routerModule;
        string runtimeModule;

        string ChannelProbeProcessor = default("Local");
        string NetworkEntity = default("");

        double targetCbr = default(0.68);
        int queueLength = default(2);
        bool enableDualAlpha = default(false);

        @signal[dccLimericDelta](type=double);
        @statistic[dccLimericDelta](source=dccLimericDelta;record=vector);

    gates:
        output radioDriverData;
}
