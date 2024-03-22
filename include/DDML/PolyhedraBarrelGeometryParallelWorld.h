#ifndef PolyhedraBarrelGeometryParallelWorld_H
#define PolyhedraBarrelGeometryParallelWorld_H

#include "DDML/GeometryInterface.h"
#include "DDML/FastMLShower.h"


namespace ddml {

/** Class for geometry conversions for a polyhedra barrel geometry (for now only octagonal).
 *  Faking the angle conditioning by positioning the layers centered at intersections
 *  of particle direction and calo layers. 
 * 
 * Adapted class for placement of virtual readout mesh in Parrallel world geometry once it
 * is available in DD4HEP
 * 
 *  @author P. McKeown, CERN
 *  @date Mar 2024
 */
  
  class PolyhedraBarrelGeometryParallelWorld : public GeometryInterface {
    
  public:
    PolyhedraBarrelGeometryParallelWorld(){
      initialize() ;
    } ;
    
    virtual ~PolyhedraBarrelGeometryParallelWorld(){};
    
    /// initialize the plugin - after properties have been set
    void initialize() ;

    /// declare the proerties needed for the plugin
    void declareProperties( dd4hep::sim::Geant4Action* plugin ) {

      plugin->declareProperty("Detector" , this->_detector ) ;
      plugin->declareProperty("Symmetry" , this->_nSymmetry ) ;
      plugin->declareProperty("CorrectForAngles" , this->_correctForAngles ) ;
    }


    /** compute local direction in coordinate system that has the z-axis pointing into the calorimeter,
     *  normal to the layers
     */
    G4ThreeVector localDirection(G4FastTrack const& aFastTrack) const  ;

    /** convert the local spacepoints to global spacepoints
     */
    virtual void localToGlobal(G4FastTrack const& aFastTrack,
			       std::vector<SpacePointVec>& spacepoints ) const  ;

  protected:
    /// local helper
    int phiSector(G4ThreeVector const& position) const  ;
    
  private:
    std::vector<float> _caloLayerDistances ={} ;

    /// model properties for plugin
    std::string _detector = { "EcalBarrel" } ;
    int _nSymmetry = 8 ;
    bool _correctForAngles = false  ;
  };

} // namespace
#endif
