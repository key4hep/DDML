#include "DDML/EndcapTriggerTwoAngleBIBAE.h"

#include <G4FastTrack.hh>                   //for G4FastTrack

namespace ddml {

    bool EndcapTriggerTwoAngleBIBAE::check_trigger(const G4FastTrack& aFastTrack){

        G4ThreeVector direction = aFastTrack.GetPrimaryTrack()->GetMomentumDirection();

        G4double global_phi = direction.phi();
        G4double global_theta = direction.theta();

        // debugging
        const bool firstCut = global_theta > 33.*(M_PI/180.);
        const bool secondCut = global_theta < M_PI/2.;
        const bool thirdCut = global_theta < 147.*(M_PI/180.);
        const bool fourthCut = global_theta > M_PI/2.;

        std::cout << "GREPME EndcapTrigger " << firstCut << " " << secondCut << " " << thirdCut << " " << fourthCut
                  << " -> " << (firstCut && secondCut) << " | " << (thirdCut && fourthCut) << '\n';

        if ( 
            // Cuts on global theta
            ( (global_theta > 33.*(M_PI/180.)  ) && (global_theta < M_PI/2.)) ||
            ( (global_theta < 147.*(M_PI/180.) ) && (global_theta > M_PI/2.))

            //(global_theta <= (33./ M_PI) * 180. ) ||
            //(global_theta >= (147./ M_PI) * 180. )
        ){return false;}//{return true;}
        else {return true;}

    };

}