#include "DDML/CylindricalBarrelGeometryParallelWorld.h"

#include <G4FastTrack.hh> // for G4FastTrack

#include "DD4hep/DD4hepUnits.h"
#include "DD4hep/DetType.h"
#include "DD4hep/Detector.h"
#include "DDRec/DetectorData.h"

#define DEBUGPRINT 0

namespace ddml{

void CylindricalBarrelGeometryParallelWorld::initialize() {
    // Could add detector information here later for debugging
}

G4ThreeVector CylindricalBarrelGeometryParallelWorld::localDirection(G4FastTrack const& aFastTrack) const {
    G4ThreeVector position = aFastTrack.GetPrimaryTrack()->GetPosition();
    G4ThreeVector direction = aFastTrack.GetPrimaryTrack()->GetMomentumDirection();

    // First rotate around z axis, such that particle direction in x-y plane aligns with x axis
    G4RotationMatrix rotNeg;
    rotNeg.rotateZ(-direction.phi());

    auto dirR = rotNeg*direction;

    // now transform this direction into a righthanded coordinate system with the z-axis pointing into calo
    G4ThreeVector localDir(-dirR.z(), dirR.y(), dirR.x());

    dd4hep::printout(dd4hep::DEBUG, "CylindricalBarrelGeometryParallelWorld::localDirection",
                                    "position: (%f, %f, %f), direction: (%f, %f, %f), localDir: (%f, %f, %f)", 
                                    position.x(), position.y(), position.z(), 
                                    direction.x(), direction.y(), direction.z(), 
                                    localDir.x(), localDir.y(), localDir.z());

    return localDir;
}

void CylindricalBarrelGeometryParallelWorld::localToGlobal(G4FastTrack const& aFastTrack,
                                                            std::vector<SpacePointVec>& spacepoints) const{
    G4ThreeVector position = aFastTrack.GetPrimaryTrack()->GetPosition();
    G4ThreeVector direction = aFastTrack.GetPrimaryTrack()->GetMomentumDirection();
    
    // rotate position and direction to global x-axis (i.e. allign z axis to global x-axis)
    G4RotationMatrix rotNeg;
    rotNeg.rotateZ(-direction.phi());

    G4RotationMatrix rotPos;
    rotPos.rotateZ(direction.phi());

    auto posR = rotNeg * position;
    auto dirR = rotNeg * direction;

    dd4hep::printout(dd4hep::DEBUG, "CylindricalBarrelGeometryParallelWorld::localToGlobal",
        "position: (%f, %f, %f), direction: (%f, %f, %f), posR: (%f, %f, %f), dirR: (%f, %f, %f)", 
        position.x(), position.y(), position.z(), 
        direction.x(), direction.y(), direction.z(),
        posR.x(), posR.y(), posR.z(),
        dirR.x(), dirR.y(), dirR.z());

    for (int l = 0; l < m_nCellsZ; ++l) {
        for (int i = 0, N = spacepoints[l].size(); i < N; ++i) {
            auto& sp = spacepoints[l][i];
    
            /// PARALLEL WORLD LOCAL TO GLOBAL
            // In this case, don't use any information about the sensitive positions,
            // but rather only about the position of the envelope (i.e. surface of the calorimeter face)
            G4ThreeVector pos(posR.x() + sp.Z, posR.y() + sp.Y, posR.z() - sp.X);
    
            // rotate back to original phi sector
            auto global = rotPos * pos;
    
            sp.X = global.x();
            sp.Y = global.y();
            sp.Z = global.z();
    
            dd4hep::printout(dd4hep::DEBUG, "CylindricalBarrelGeometryParallelWorld::localToGlobal", 
                "sp.X: %f, sp.Y: %f, sp.Z: %f",
                sp.X, sp.Y, sp.Z);
    
        }
        }
}

} // namespace ddml
