#ifndef RegularGridBIBAEModel_H
#define RegularGridBIBAEModel_H

#include "DDML/FastMLShower.h"
#include "DDML/ModelInterface.h"

namespace ddml {

/** Class for running a regular grid based GAN (or GAN like) ML model for fast
 * shower simulation. Assumes a regular cartesian grid output with (x,y)
 * defining the calorimeter planes and z the depth of the calorimeter.
 *
 * Based on RegularGridGANModel.
 *
 * For BIBAE with single angle and energy conditioning.
 * Allow for non-central incident cell in grid.
 *
 *  @author P.McKeown, DESY
 *  @date Apr. 2023
 */

class RegularGridBIBAEModel : public ModelInterface {
public:
  RegularGridBIBAEModel(){};

  virtual ~RegularGridBIBAEModel(){};

  /// declare the proerties needed for the plugin
  void declareProperties(dd4hep::sim::Geant4Action* plugin) {
    plugin->declareProperty("NCellsX", this->m_nCellsX);
    plugin->declareProperty("NCellsY", this->m_nCellsY);
    plugin->declareProperty("NCellsZ", this->m_nCellsZ);

    plugin->declareProperty("LatentVectorSize", this->m_latentSize);

    plugin->declareProperty("CellSizeX", this->m_cellSizeX);
    plugin->declareProperty("CellSizeY", this->m_cellSizeY);
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
  /// model properties for plugin
  // These grid sizes were used for the angular BIBAE
  int m_nCellsX = 30;
  int m_nCellsY = 60;
  int m_nCellsZ = 30;
  int m_latentSize = 3;
  float m_cellSizeX = 5.;
  float m_cellSizeY = 5.;

  // Define incident cell in the regular grid for centering
  // One-angle BIBAE: center: 15,12
  int m_centerCellX = m_nCellsX / 2; // center of grid in X
  int m_centerCellY = 12.;
  TensorDimVecs m_tensDims = {{1, 1}, {1, 1}, {1, 2}};
};

} // namespace ddml
#endif
