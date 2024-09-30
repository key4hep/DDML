#ifndef OctogonalBarrelTrigger_H
#define OctogonalBarrelTrigger_H

#include "DDML/TriggerInterface.h"

#include <G4FastTrack.hh>

namespace ddml {
/** Class for fast sim trigger in ILD octogonal barrel
 * Applies cuts on global phi accoriding to intersection of modules in barrel
 * Applies cuts on global theta according to edge of barrel
 * All cuts tested with 126GeV photons (max training range)
 *
 * @author P.McKeown, DESY
 * @date Sep. 2023
 *
 */

class OctogonalBarrelTrigger : public TriggerInterface {
public:
  OctogonalBarrelTrigger(){};

  virtual ~OctogonalBarrelTrigger(){};

  // check trigger

  virtual bool check_trigger(const G4FastTrack& aFastTrack);
};

} // namespace ddml
#endif