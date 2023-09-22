#ifndef RegularGridTwoAngleBIBAEModel_H
#define RegularGridTwoAngleBIBAEModel_H

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
  
  class RegularGridTwoAngleBIBAEModel : public ModelInterface {
    
  public:
    RegularGridTwoAngleBIBAEModel() {} ;
    
    virtual ~RegularGridTwoAngleBIBAEModel(){};
    
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
    // These grid sizes were used for the two angle BIBAE
    int _nCellsX = 30;
    int _nCellsY = 49;
    int _nCellsZ = 30;
    int _latentSize = 4;
    float _cellSizeX = 5.088333;  //5. ;
    float _cellSizeY = 5.088333;  //5. ;


    // Define incident cell in the regular grid for centering
    // One-angle BIBAE: center: 15,12
    // These are no longer used in the two angle implementation
    int _centerCellX = _nCellsX/2 ; // center of grid in X
    int _centerCellY = 12. ;

    // Function to compute incident cell in regular grid from input phi and theta angles
    std::vector<double> getIncidentCell(const double& theta, const double& phi) ;

    std::vector<double> isect_line_plane_3Vec(const double& p0_x, const double& p0_y, const double& p0_z, const double& p1_x, const double& p1_y, const double& p1_z, 
                                                const double& p_co_x, const double& p_co_y, const double& p_co_z, const double& p_no_x, const double& p_no_y, const double& p_no_z);

    struct Vector3d{double x; double y; double z;};

    Vector3d crossProduct(const Vector3d& v1, const Vector3d& v2);

    Vector3d normalize(const Vector3d& v);

    TensorDimVecs _tensDims = { {1, 1}, {1, 1}, {1,1}, {1,3} };
  };

} // namespace
#endif

