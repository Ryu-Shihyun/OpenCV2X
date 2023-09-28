package artery.application.den;

simple Controlled like SuspendableUseCase
{
    @class(den::Controlled);
    double triggerTime @unit(s) = default(500ms);
    double detectionBlockingInterval @unit(s) = default(100ms);
}