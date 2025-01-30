#ifndef L2LFlowsx9Model_H
#define L2LFlowsx9Model_H

#include "DDML/ModelInterface.h"
#include "DDML/FastMLShower.h"


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
  class L2LFlowsx9Model : public ModelInterface {

  public:
    L2LFlowsx9Model() {} ;

    virtual ~L2LFlowsx9Model(){};

    /// declare the proerties needed for the plugin
    void declareProperties( dd4hep::sim::Geant4Action* plugin ) {
      plugin->declareProperty("NCellsX" , this->_nCellsX ) ;
      plugin->declareProperty("NCellsY" , this->_nCellsY ) ;
      plugin->declareProperty("NCellsZ" , this->_nCellsZ ) ;

      plugin->declareProperty("CellSizeX" , this->_cellSizeX ) ;
      plugin->declareProperty("CellSizeY" , this->_cellSizeY ) ;
      plugin->declareProperty("CellThickness" , this->_cell_thickness ) ;
    }



    /** prepare the input vector and resize the output vector for this model
     *  based on the current FastTrack (e.g. extract kinetic energy and incident
     *  angles.)
     */
    virtual void prepareInput(G4FastTrack const& aFastTrack,
			      G4ThreeVector const& localDir,
			      InputVecs& inputs, TensorDimVecs& tensDims,
			      std::vector<float>& output ) ;


    /** create a vector of spacepoints per layer interpreting the model output
     */
    virtual void convertOutput(G4FastTrack const& aFastTrack,
			       G4ThreeVector const& localDir,
			       const std::vector<float>& output,
			       std::vector<SpacePointVec>& spacepoints ) ;


  private:

    /// model properties for plugin
    int _nCellsX = 90;
    int _nCellsY = 90;
    int _nCellsZ = 30;
    float _cellSizeX = 5.088333;
    float _cellSizeY = 5.088333;
    float _cell_thickness = 0.5250244140625;
    float _factor = 3.0;
    const float layer_bottom_pos[30] = {
        1811.34020996, 1814.46508789, 1823.81005859, 1826.93505859,
        1836.2800293 , 1839.4050293 , 1848.75      , 1851.875     ,
        1861.2199707 , 1864.3449707 , 1873.68994141, 1876.81494141,
        1886.16003418, 1889.28503418, 1898.63000488, 1901.75500488,
        1911.09997559, 1914.22497559, 1923.56994629, 1926.69494629,
        1938.14001465, 1943.36499023, 1954.81005859, 1960.03503418,
        1971.47998047, 1976.70495605, 1988.15002441, 1993.375     ,
        2004.81994629, 2010.04504395
    };

    TensorDimVecs _tensDims = { {1, 4} };

    void _shift(const G4ThreeVector& p_normed, std::vector<float>& x_shift,
                std::vector<float>& y_shift) ;
  };

} // namespace
#endif
