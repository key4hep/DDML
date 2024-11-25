#ifndef ParallelWorldCaloDiTModel_H
#define ParallelWorldCaloDiTModel_H

#include "DDML/FastMLShower.h"
#include "DDML/ModelInterface.h"

namespace ddml {

/**
 * Class for running the virtual, cylindrical scoring mesh (a la Par04) for fast shower simulation.
 * Remove z dimension orientation along the depth of the calorimeter- it is instead oriented along the
 * shower axis. Rho and phi coordinates are also defined relative to the shower axis.
 *
 *  @author X. Zhu, CERN
 *  @date August 2024
 */

class ParallelWorldCaloDiTModel : public ModelInterface {
public:
  ParallelWorldCaloDiTModel(){};

  virtual ~ParallelWorldCaloDiTModel(){};

  /// declare the proerties needed for the plugin
  void declareProperties(dd4hep::sim::Geant4Action* plugin) {
    plugin->declareProperty("NCellsRho", this->_nCellsRho);
    plugin->declareProperty("NCellsPhi", this->_nCellsPhi);
    plugin->declareProperty("NCellsZ", this->_nCellsZ);

    plugin->declareProperty("LatentVectorSize", this->_latentSize);

    plugin->declareProperty("CellSizeRho", this->_cellSizeRho);
    plugin->declareProperty("CellSizeZ", this->_cellSizeZ);
  }

  /** prepare the input vector and resize the output vector for this model
   *  based on the current FastTrack (e.g. extract kinetic energy and incident
   *  angles.)
   */

  virtual void prepareInput(G4FastTrack const& aFastTrack, G4ThreeVector const& localDir, InputVecs& inputs,
                            TensorDimVecs& tensDims, std::vector<float>& output);

  /** create a vector of spacepoints per layer interpreting the model output
   */
  virtual void convertOutput(G4FastTrack const&, G4ThreeVector const& localDir, const std::vector<float>& output,
                             std::vector<SpacePointVec>& spacepoints);

private:
  // Copied from calo challenge
  G4ThreeVector fMeshSize = G4ThreeVector(4.65 * CLHEP::mm, 1, 3.4 * CLHEP::mm);

  /// model properties for plugin
  //    - using numbers from calochallenge
  
  int _nCellsRho = 9;
  int _nCellsPhi = 16;
  int _nCellsZ = 45;
  int _latentSize = 3;
  float _cellSizeRho = 4.65; // mm
  float _cellSizeZ = 5.05;    // mm
  
  /**** OLD
  int _nCellsRho = 9;
  int _nCellsPhi = 16;
  int _nCellsZ = 45;
  int _latentSize = 3;
  float _cellSizeRho = 4.65; // mm
  float _cellSizeZ = 3.4;    // mm
  */
  TensorDimVecs _tensDims = {{1, _latentSize}};
};

} // namespace ddml
#endif
