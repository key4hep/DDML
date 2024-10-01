#ifndef Par04ExampleVAE_H
#define Par04ExampleVAE_H

#include "DDML/FastMLShower.h"
#include "DDML/ModelInterface.h"

namespace ddml {

/** Class for running the ML model from Geant4 - Par04 example for fast shower
 * simulation. Assumes a cylindrical geometry with (rho, phi) defining the
 * calorimeter planes and z the depth of the calorimeter.
 *
 *  @author F.Gaede, DESY
 *  @date May 2023
 */

class Par04ExampleVAE : public ModelInterface {
public:
  Par04ExampleVAE(){};

  virtual ~Par04ExampleVAE(){};

  /// declare the proerties needed for the plugin
  void declareProperties(dd4hep::sim::Geant4Action* plugin) {
    plugin->declareProperty("NCellsRho", this->m_nCellsRho);
    plugin->declareProperty("NCellsPhi", this->m_nCellsPhi);
    plugin->declareProperty("NCellsZ", this->m_nCellsZ);

    plugin->declareProperty("LatentVectorSize", this->m_latentSize);

    plugin->declareProperty("CellSizeRho", this->m_cellSizeRho);
    plugin->declareProperty("CellSizeZ", this->m_cellSizeZ);
  }

  /** prepare the input vector and resize the output vector for this model
   *  based on the current FastTrack (e.g. extract kinetic energy and incident
   *  angles.)
   */

  virtual void prepareInput(G4FastTrack const& aFastTrack, G4ThreeVector const& localDir, InputVecs& inputs,
                            TensorDimVecs& tensDims, std::vector<float>& output);

  /** create a vector of spacepoints per layer interpreting the model output
   */
  virtual void convertOutput(G4FastTrack const& aFastTrack, G4ThreeVector const& localDir,
                             const std::vector<float>& output, std::vector<SpacePointVec>& spacepoints);

private:
  float m_initialEnergy = 0.;
  /// model properties for plugin
  //    - using numbers from examplePar04_onnx.in as default parameters
  int m_nCellsRho = 18;
  int m_nCellsPhi = 50;
  int m_nCellsZ = 45;
  int m_latentSize = 10.;
  float m_cellSizeRho = 2.325; // mm
  float m_cellSizeZ = 3.4;     // mm - not used really
  TensorDimVecs m_tensDims = {{1, m_latentSize + 4}};
};

} // namespace ddml
#endif
