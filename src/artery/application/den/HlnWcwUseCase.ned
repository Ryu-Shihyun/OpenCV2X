package artery.application.den;

simple HlnWcw like SuspendableUseCase
{
    @class(den::HlnWcw);
    bool active = default(true);
    double interval @unit(s) = default(1s);

    // Cause code indications the category of weather; 19 = precipitation, 17 = extreme weather condition (for wind)
    int CauseCode = default(19);

    // Sub Cause code indicates the type of weather event; 19 in the following 1 = rain, 3 = softHail, 2 = heavy snow,
    // 17 in the following 1 = strong winds
    int SubCauseCode = default(1);

    // Message size in bytes
    int packetSize = default(420);

    bool nonUrbanEnvironment = default(true);
    double detectionBlockingInterval @unit(s) = default(60s);
}

