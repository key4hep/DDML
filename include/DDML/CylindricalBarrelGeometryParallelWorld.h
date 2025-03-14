#ifndef CylindricalBarrelGeometryParallelWorld_H
#define CylindricalBarrelGeometryParallelWorld_H

#include "DDML/FastMLShower.h"
#include "DDML/GeometryInterface.h"

namespace ddml{

/** Class for geometry conversions for a cylindrical barrel geometry
 * Designed for use with a virtual readout mesh and a Parallel world geometry
 * 
 * @author P. McKeown, CERN
 * @date Feb. 2025
*/

class CylindricalBarrelGeometryParallelWorld : public GeometryInterface{
public:
CylindricalBarrelGeometryParallelWorld() {
    initialize();
};

virtual ~CylindricalBarrelGeometryParallelWorld(){};

// initialize the plugin - after properties have been set
void initialize();

// declare the proerties needed for the plugin
void declareProperties(dd4hep::sim::Geant4Action* plugin) {
    plugin->declareProperty("Detector", this->m_detector);
}

/** compute local direction in coordinate system that has the z-axis pointing into the calorimeter,
 *  normal to the layers
*/
G4ThreeVector localDirection(G4FastTrack const& aFastTrack) const;

// convert local spacepoints to global spacepoints
virtual void localToGlobal(G4FastTrack const& aFastTrack, std::vector<SpacePointVec>& spacepoints) const;

private:
   
    std::string m_detector = {"ECalBarrel"}; // needs to be moved to steering file (across all geometry files!)

    int m_nCellsZ = 45; // number of cells in mesh in z' direction (along particle flight path)
                        // This is also a parameter used in the model implementation, so should be sourced from a common definitiion

};

} // namespace ddml
#endif