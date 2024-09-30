#include "DDML/EndcapTriggerTwoAngleBIBAE.h"

#include <G4FastTrack.hh> //for G4FastTrack

namespace ddml {

bool EndcapTriggerTwoAngleBIBAE::check_trigger(const G4FastTrack& aFastTrack) {
  G4ThreeVector direction = aFastTrack.GetPrimaryTrack()->GetMomentumDirection();

  G4double global_theta = direction.theta();

  if (
      // Cuts on global theta
      ((global_theta > 33. * (M_PI / 180.)) && (global_theta < M_PI / 2.)) ||
      ((global_theta < 147. * (M_PI / 180.)) && (global_theta > M_PI / 2.))

      //(global_theta <= (33./ M_PI) * 180. ) ||
      //(global_theta >= (147./ M_PI) * 180. )
  ) {
    return false;
  } //{return true;}
  else {
    return true;
  }
};

} // namespace ddml
