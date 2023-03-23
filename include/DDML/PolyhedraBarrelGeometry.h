#ifndef PolyhedraBarrelGeometry_H
#define PolyhedraBarrelGeometry_H

#include "DDML/GeometryInterface.h"
#include "DDML/FastMLShower.h"


namespace ddml {

/** Class for geometry conversions for a 
 * 
 *  @author F.Gaede, DESY
 *  @date Mar 2023
 */
  
  class PolyhedraBarrelGeometry : public GeometryInterface {
    
  public:
    PolyhedraBarrelGeometry() ;
    
    virtual ~PolyhedraBarrelGeometry(){};
    
    /// declare the proerties needed for the plugin
    void declareProperties( dd4hep::sim::Geant4Action* plugin ) {

      plugin->declareProperty("Detector" , this->_detector ) ;

    }
  
    /** convert the local spacepoints to global spacepoints
     */
    virtual void localToGlobal(G4FastTrack const& aFastTrack,
			       std::vector<SpacePointVec>& spacepoints ) ;
    
    
  private:
    std::vector<float> _caloLayerDistances ={} ;

    /// model properties for plugin
    std::string _detector = { "EcalBarrel" } ;
  };

} // namespace
#endif
