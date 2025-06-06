#ifndef CaloCloudsTwoAngleModel_H
#define CaloCloudsTwoAngleModel_H

#include "DDML/FastMLShower.h"
#include "DDML/ModelInterface.h"

namespace ddml {

/** Class for running a point cloud based ML model for fast shower simulation.
 *  Assumes a cartesian (x,y) coordinates defining the calorimeter planes (layers) and z the depth
 *  of the calorimeter.
 *
 * Based on BiBAETwoAngleModel.
 *
 * For CaloClouds with two angle and energy conditioning.
 *
 *  @author A.Korol, DESY
 *  @date May. 2024
 */

class CaloCloudsTwoAngleModel : public ModelInterface {
public:
  CaloCloudsTwoAngleModel(){};

  virtual ~CaloCloudsTwoAngleModel(){};

  /// declare the proerties needed for the plugin
  void declareProperties(dd4hep::sim::Geant4Action* plugin) {
    plugin->declareProperty("LatentVectorSize", this->m_latentSize);
  }

  /** prepare the input vector and resize the output vector for this model
   *  based on the current FastTrack (e.g. extract kinetic energy and incident
   *  angles.)
   */
  virtual void prepareInput(G4FastTrack const& aFastTrack, G4ThreeVector const& localDir, InputVecs& inputs,
                            TensorDimVecs& tensDims, std::vector<float>& output);

  /** create a vector of spacepoints per layer interpreting the model output
   */
  virtual void convertOutput(G4FastTrack const&, G4ThreeVector const&, const std::vector<float>& output,
                             std::vector<SpacePointVec>& spacepoints);

private:
  /// model properties for plugin
  // These grid sizes were used for the two angle BIBAE
  int m_nLayer = 30;
  int m_maxNumElements = 35000; // number of space points in the output multiplied by 4 (x,y,z,energy)
  int m_latentSize = 3;         // number of input features (energy, theta, phi)

  struct Vector3d {
    double x;
    double y;
    double z;
  };

  Vector3d crossProduct(const Vector3d& v1, const Vector3d& v2);

  Vector3d normalize(const Vector3d& v);

  TensorDimVecs m_tensDims = {{1, 1}, {1, 1}, {1, 1}, {1, 3}};
};

} // namespace ddml
#endif
