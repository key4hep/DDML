#ifndef EndcapGeometryParallelWorld_H
#define EndcapGeometryParallelWorld_H

#include "DDML/GeometryInterface.h"
#include "DDML/FastMLShower.h"


namespace ddml {

/** Class for geometry conversions for an endcap geometry symmetric around the origin.
 *  Faking the angle conditioning by positioning the layers centered at intersections
 *  of particle direction and calo layers. 
 * 
 * Adapted class for placement of virtual readout mesh in Parrallel world geometry once it
 * is available in DD4HEP
 * 
 *  @author P. McKeown, CERN
 *  @date April 2024
 */
  
  class EndcapGeometryParallelWorld : public GeometryInterface {
    
  public:
    EndcapGeometryParallelWorld(){
      initialize() ;
    } ;

    /// initialize the plugin - after properties have been set
    void initialize() ;
    
    virtual ~EndcapGeometryParallelWorld(){};
    
    /// declare the proerties needed for the plugin
    void declareProperties( dd4hep::sim::Geant4Action* plugin ) {

      plugin->declareProperty("Detector" , this->_detector ) ;
      plugin->declareProperty("CorrectForAngles" , this->_correctForAngles ) ;
    }
  
    /** compute local direction in coordinate system that has the z-axis pointing into the calorimeter,
     *  normal to the layers
     */
    G4ThreeVector localDirection(G4FastTrack const& aFastTrack)  const ;

    /** convert the local spacepoints to global spacepoints
     */
    virtual void localToGlobal(G4FastTrack const& aFastTrack,
			       std::vector<SpacePointVec>& spacepoints ) const  ;
    
    
  private:
    std::vector<float> _caloLayerDistances ={} ;

    /// model properties for plugin
    std::string _detector = { "ECalEndcap" } ;
    bool _correctForAngles = false  ;

    int _nCellsZ = 45; // number of cells in mesh in z' direction (along particle flight path)
  };

} // namespace
#endif
