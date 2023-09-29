#ifndef TriggerInterface_H
#define TriggerInterface_H

#include "DDML/DDML.h"

class G4FastTrack;

namespace ddml {

/** The basic interface for ML model trigger
 * 
 * @author P.McKeown, DESY
 * @date Sep. 2023
*/

class TriggerInterface {
    public:
    virtual ~TriggerInterface(){};

    virtual bool check_trigger(const G4FastTrack&) = 0;
};


}

#endif