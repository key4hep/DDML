#ifndef EndcapGeometry_H
#define EndcapGeometry_H

#include "DDML/GeometryInterface.h"
#include "DDML/FastMLShower.h"


namespace ddml {

/** Class for geometry conversions for an endcap geometry symmetric around the origin.
 *  Faking the angle conditioning by positioning the layers centered at intersections
 *  of particle direction and calo layers. 
 * 
 *  @author F.Gaede, DESY
 *  @date Mar 2023
 */
  
  class EndcapGeometry : public GeometryInterface {
    
  public:
    EndcapGeometry(){} ;

    /// initialize the plugin - after properties have been set
    void initialize() ;
    
    virtual ~EndcapGeometry(){};
    
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
    bool _isInitialized = false ;

    /// model properties for plugin
    std::string _detector = { "EcalEndcap" } ;
  };

} // namespace
#endif
