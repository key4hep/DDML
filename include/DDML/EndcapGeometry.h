#ifndef EndcapGeometry_H
#define EndcapGeometry_H

#include "DDML/FastMLShower.h"
#include "DDML/GeometryInterface.h"

namespace ddml {

/** Class for geometry conversions for an endcap geometry symmetric around the
 * origin. Faking the angle conditioning by positioning the layers centered at
 * intersections of particle direction and calo layers.
 *
 *  @author F.Gaede, DESY
 *  @date Mar 2023
 */

class EndcapGeometry : public GeometryInterface {
public:
  EndcapGeometry() {
    initialize();
  };

  /// initialize the plugin - after properties have been set
  void initialize();

  virtual ~EndcapGeometry(){};

  /// declare the proerties needed for the plugin
  void declareProperties(dd4hep::sim::Geant4Action* plugin) {
    plugin->declareProperty("Detector", this->m_detector);
    plugin->declareProperty("CorrectForAngles", this->m_correctForAngles);
  }

  /** compute local direction in coordinate system that has the z-axis pointing
   * into the calorimeter, normal to the layers
   */
  G4ThreeVector localDirection(G4FastTrack const& aFastTrack) const;

  /** convert the local spacepoints to global spacepoints
   */
  virtual void localToGlobal(G4FastTrack const& aFastTrack, std::vector<SpacePointVec>& spacepoints) const;

private:
  std::vector<float> m_caloLayerDistances = {};

  /// model properties for plugin
  std::string m_detector = {"EcalEndcap"};
  bool m_correctForAngles = false;
};

} // namespace ddml
#endif
