#ifndef ARTERY_SPEEDADVISORYMESSAGE_H_AEPKF5GQ
#define ARTERY_SPEEDADVISORYMESSAGE_H_AEPKF5GQ

#include <omnetpp/cpacket.h>

namespace artery
{

class SpeedAdvisoryMessage : public omnetpp::cPacket
{
public:
    SpeedAdvisoryMessage();
    omnetpp::cPacket* dup() const override;


    void setSourceStation(int id) { mSourceStation = id; }
    int getSourceStation() const { return mSourceStation; }

    void setSequenceNumber(int n) { mSequenceNumber = n; }
    int getSequenceNumber() const { return mSequenceNumber; }

    void setSpeedAdvice(double n) { mSpeedAdvice = n; }
    double getSpeedAdvice() const { return mSpeedAdvice; }

    void setNorthSouth(bool NS) {northSouth = NS;}
    void setLongitudeE(double longi) {longitudeE = longi;}
    void setLongitudeW(double longi) {longitudeW = longi;}
    void setLatitudeN(double lat) {latitudeN = lat;}
    void setLatitudeS(double lat) {latitudeS = lat;}

    bool getNorthSouth() {return northSouth;}
    double getLongitudeE() {return longitudeE;}
    double getLongitudeW() {return longitudeW;}
    double getLatitudeN() {return latitudeN;}
    double getLatitudeS() {return latitudeS;}

    void setDirection(double dir) { direction=dir;}
    double getDirection() const { return direction; }

    omnetpp::SimTime getGenerationTimestamp() const { return mGenerated; }

private:
    int mSourceStation = 0;
    int mSequenceNumber = 0;
    bool northSouth = true;
    double mSpeedAdvice = 0;
    double longitudeE = 0.0;
    double longitudeW = 0.0;
    double latitudeN = 0.0;
    double latitudeS = 0.0;
    double direction = 0.0;

    omnetpp::SimTime mGenerated = omnetpp::SimTime::ZERO;
};

} // namespace artery

#endif /* ARTERY_INFRASTRUCTUREMOCKMESSAGE_H_AEPKF5GQ */

