#include "DDML/OctogonalBarrelTrigger.h"

#include <G4FastTrack.hh>                   //for G4FastTrack

namespace ddml {

    bool OctogonalBarrelTrigger::check_trigger(const G4FastTrack& aFastTrack){
        
        G4ThreeVector direction = aFastTrack.GetPrimaryTrack()->GetMomentumDirection();

        G4double global_phi = direction.phi();
        G4double global_theta = direction.theta();

        if (
                // Cuts on phi in the octagonal barrel: asymmetric layer structure at module intersection
                ( ( M_PI/8. - 0.01*(M_PI/180.) ) < global_phi && global_phi <= (M_PI/8 + 8.*(M_PI/180.) ) ) ||
                ( (M_PI/8. + M_PI/4. - 0.01*(M_PI/180.)) < global_phi && global_phi <= ( (M_PI/8. + M_PI/4.)  + 8.*(M_PI/180.) ) ) ||     
                ( (M_PI/8. + (2*M_PI)/4. - 0.01*(M_PI/180.)) < global_phi && global_phi <= ( (M_PI/8. + (2*M_PI)/4.)  + 8.*(M_PI/180.) ) ) ||     
                ( (M_PI/8. + (3*M_PI)/4. - 0.01*(M_PI/180.) ) < global_phi && global_phi <= ( (M_PI/8. + (3*M_PI)/4.)  + 8.*(M_PI/180.) ) ) ||
                ( ( -M_PI/8. - 0.01*(M_PI/180.) ) < global_phi && global_phi <= (-M_PI/8 + 8.*(M_PI/180.) ) ) ||
                ( (-M_PI/8. - M_PI/4. - 0.01*(M_PI/180.) ) < global_phi && global_phi <= ( (-M_PI/8. - M_PI/4.)  + 8.*(M_PI/180.) ) ) ||     
                ( (-M_PI/8. - (2*M_PI)/4. - 0.01*(M_PI/180.) ) < global_phi && global_phi <= ( (-M_PI/8. - (2*M_PI)/4.)  + 8.*(M_PI/180.) ) ) ||     
                ( (-M_PI/8. - (3*M_PI)/4. - 0.01*(M_PI/180.) ) < global_phi && global_phi <= ( (-M_PI/8. - (3*M_PI)/4.)  + 8.*(M_PI/180.) ) ) ||

                // Cuts on theta right at the edge of the barrel
                ( global_theta < (40. *(M_PI/180.))) ||
                ( global_theta > (140. *(M_PI/180.)))
        ){return false;}
        else {return true;}

    };

}
