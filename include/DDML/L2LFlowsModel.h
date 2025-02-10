#ifndef L2LFlowsModel_H
#define L2LFlowsModel_H

#include "DDML/FastMLShower.h"
#include "DDML/ModelInterface.h"

namespace ddml {

/** Class for running L2LFlows a fixed-grid-based model for fast shower simulation.
 * It uses convolutional normalizing flows.
 *
 * Based on BiBAETwoAngleModel and CaloCloudsTwoAngleModel.
 *
 * For ConvL2LFlows with two angle and energy conditioning.
 *
 *  @author Th. Buss, Uni Hamburg
 *  @date Aug. 2024
 */
class L2LFlowsModel : public ModelInterface {
public:
  L2LFlowsModel(){};

  virtual ~L2LFlowsModel() = default;

  /// declare the proerties needed for the plugin
  void declareProperties(dd4hep::sim::Geant4Action* plugin) {
    plugin->declareProperty("NCellsX", this->m_nCellsX);
    plugin->declareProperty("NCellsY", this->m_nCellsY);
    plugin->declareProperty("NCellsZ", this->m_nCellsZ);

    plugin->declareProperty("CellSizeX", this->m_cellSizeX);
    plugin->declareProperty("CellSizeY", this->m_cellSizeY);

    plugin->declareProperty("Factor", this->m_factor);
    plugin->declareProperty("GridShiftX", this->m_gridShiftX);
    plugin->declareProperty("GridShiftY", this->m_gridShiftY);
    plugin->declareProperty("RandomShift", this->m_randomShift);
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
  int m_nCellsX = 90;
  int m_nCellsY = 90;
  int m_nCellsZ = 30;
  float m_cellSizeX = 5.088333;
  float m_cellSizeY = 5.088333;
  float m_factor = 3.0;
  float m_gridShiftX = 1.4844563802083140;
  float m_gridShiftY = 0.6716766357421875;
  bool m_randomShift = true;
  TensorDimVecs m_tensDims = {{1, 4}};
};

} // namespace ddml
#endif
