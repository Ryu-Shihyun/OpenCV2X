package artery.application.den;

simple RsuHlnWcw like UseCase
{
    @class(den::RsuHlnWcw);
    double interval @unit(s) = default(1s);

    // Cause code indications the category of weather; 19 = precipitation, 17 = extreme weather condition (for wind)
    int CauseCode = default(19);

    // Sub Cause code indicates the type of weather event; 19 in the following 1 = rain, 3 = softHail, 2 = heavy snow,
    // 17 in the following 1 = strong winds
    int SubCauseCode = default(1);

    int packetSize = default(420);
}

