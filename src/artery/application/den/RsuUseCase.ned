package artery.application.den;

moduleinterface RsuUseCase
{
}

moduleinterface RsuSuspendableUseCase extends RsuUseCase
{
    double detectionBlockingInterval @unit(s);
}
