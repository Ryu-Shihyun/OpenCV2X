package artery.application.den;

simple RsuRwwLaneClosure like UseCase
{
    @class(den::RsuRwwLaneClosure);
    bool rsuCentre = default(true);
    int closedLane = default(0);
    double longitude = default(0.0);
    double latitude = default(0.0);
    double radius = default(500);
    double duration = default(3600);
    double interval = default(1);
}

