#ifndef RegularGridGANModel_H
#define RegularGridGANModel_H

#include "DDML/ModelInterface.h"
#include "DDML/FastMLShower.h"


namespace ddml {

/** Class for running a regular grid based GAN (or GAN like) ML model for fast shower simulation.
 *  Assumes a regular cartesian grid output with (x,y) defining the calorimeter planes and z the depth
 *  of the calorimeter.
 * 
 *  @author F.Gaede, DESY
 *  @date Mar 2023
 */
  
  class RegularGridGANModel : public ModelInterface {
    
  public:
    RegularGridGANModel() {} ;
    
    virtual ~RegularGridGANModel(){};
    
    /// declare the proerties needed for the plugin
    void declareProperties( dd4hep::sim::Geant4Action* plugin ) {

      plugin->declareProperty("NCellsX" , this->_nCellsX ) ;
      plugin->declareProperty("NCellsY" , this->_nCellsY ) ;
      plugin->declareProperty("NCellsZ" , this->_nCellsZ ) ;

      plugin->declareProperty("LatentVectorSize" , this->_latentSize ) ;

      plugin->declareProperty("CellSizeX" , this->_cellSizeX ) ;
      plugin->declareProperty("CellSizeY" , this->_cellSizeY ) ;
    }
  

  
    /** prepare the input vector and resize the output vector for this model
     *  based on the current FastTrack (e.g. extract kinetic energy and incident
     *  angles.)
     */
    
    virtual void prepareInput(G4FastTrack const& aFastTrack,
			      std::vector<float>& input,
			      std::vector<float>& output ) ;


    /** create a vector of spacepoints per layer interpreting the model output  
     */
    virtual void convertOutput(G4FastTrack const& aFastTrack,
			       const std::vector<float>& output,
			       std::vector<SpacePointVec>& spacepoints ) ;


  
  private:

    /// model properties for plugin

    /*
    // These grid sizes were used for the GAN
    int _nCellsX = 30 ; 
    int _nCellsY = 30 ; 
    int _nCellsZ = 30 ; 
    */

    // These grid sizes were used for the angular BIBAE
    int _nCellsX = 30;
    int _nCellsY = 60;
    int _nCellsZ = 30;

    int _latentSize = 100 ;
    float _cellSizeX = 5. ;
    float _cellSizeY = 5. ;

  };

} // namespace
#endif
