#ifndef PolyhedraBarrelGeometry_H
#define PolyhedraBarrelGeometry_H

#include "DDML/FastMLShower.h"
#include "DDML/GeometryInterface.h"

namespace ddml {

/** Class for geometry conversions for a polyhedra barrel geometry (for now only
 * octagonal). Faking the angle conditioning by positioning the layers centered
 * at intersections of particle direction and calo layers.
 *
 *  @author F.Gaede, DESY
 *  @date Mar 2023
 */

class PolyhedraBarrelGeometry : public GeometryInterface {
public:
  PolyhedraBarrelGeometry() {
    initialize();
  };

  virtual ~PolyhedraBarrelGeometry(){};

  /// initialize the plugin - after properties have been set
  void initialize();

  /// declare the proerties needed for the plugin
  void declareProperties(dd4hep::sim::Geant4Action* plugin) {
    plugin->declareProperty("Detector", this->_detector);
    plugin->declareProperty("Symmetry", this->_nSymmetry);
    plugin->declareProperty("CorrectForAngles", this->_correctForAngles);
  }

  /** compute local direction in coordinate system that has the z-axis pointing
   * into the calorimeter, normal to the layers
   */
  G4ThreeVector localDirection(G4FastTrack const& aFastTrack) const;

  /** convert the local spacepoints to global spacepoints
   */
  virtual void localToGlobal(G4FastTrack const& aFastTrack, std::vector<SpacePointVec>& spacepoints) const;

protected:
  /// local helper
  int phiSector(G4ThreeVector const& position) const;

private:
  std::vector<float> _caloLayerDistances = {};

  /// model properties for plugin
  std::string _detector = {"EcalBarrel"};
  int _nSymmetry = 8;
  bool _correctForAngles = false;
};

} // namespace ddml
#endif
