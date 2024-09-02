#ifndef EndcapTriggerTwoAngleBIBAE_H
#define EndcapTriggerTwoAngleBIBAE_H

#include "DDML/TriggerInterface.h"

#include <G4FastTrack.hh> 

namespace ddml {
/** Class for fast sim trigger in ILD endcap
 * Applies cuts on global theta, down to 33 deg. because of BIBAE 2 angle training range
 * Straight line from IP: max theta is 37 degrees
 * All cuts tested with 126GeV photons (max training range)
 * 
 * @author P.McKeown, DESY
 * @date Sep. 2023
 * 
 */

class EndcapTriggerTwoAngleBIBAE : public TriggerInterface {

public:
    EndcapTriggerTwoAngleBIBAE(){};

    virtual ~EndcapTriggerTwoAngleBIBAE(){};

    // check trigger

    virtual bool check_trigger(const G4FastTrack& aFastTrack);
};


}
#endif