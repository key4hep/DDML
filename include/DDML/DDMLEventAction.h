#include "G4UserEventAction.hh" // for G4UserEventAction
#include <G4Types.hh>           // for G4int, G4double
#include <vector>               // for vector
// #include "G4Timer.hh"            // for G4Timer
class G4Event;
#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "DDG4/Geant4EventAction.h"
#include "DDG4/Geant4Handle.h"
#include "DDG4/Geant4Kernel.h"

namespace ddml {

/** Event action for ddml.
 *
 *  @author P. McKeown, CERN
 *  @date Feb 2025
 *
 */

class FastSimTriggerInfoRecordingEventAction : public dd4hep::sim::Geant4EventAction {
public:
  /// Standard constructor with initializing arguments
  FastSimTriggerInfoRecordingEventAction(dd4hep::sim::Geant4Context* c, const std::string& n);
  /// Default destructor
  virtual ~FastSimTriggerInfoRecordingEventAction();
  /// End-of-event callback
  void end(const G4Event*) override;

  //// Get and Set methods for Calo Face info
  inline std::vector<G4int>& GetCaloMC_PDG() { return m_caloMCPDG; }
  inline std::vector<G4double>& GetCaloMC_E() { return m_caloMCE; }
  inline std::vector<G4double>& GetCaloMC_PosX() { return m_caloMCPosX; }
  inline std::vector<G4double>& GetCaloMC_PosY() { return m_caloMCPosY; }
  inline std::vector<G4double>& GetCaloMC_PosZ() { return m_caloMCPosZ; }
  inline std::vector<G4double>& GetCaloMC_DirX() { return m_caloMCDirX; }
  inline std::vector<G4double>& GetCaloMC_DirY() { return m_caloMCDirY; }
  inline std::vector<G4double>& GetCaloMC_DirZ() { return m_caloMCDirZ; }

  // Set methods to push back vector
  inline void SetElCaloMC_PDG(G4int aValue) { m_caloMCPDG.push_back(aValue); }
  inline void SetElCaloMC_E(G4double aValue) { m_caloMCE.push_back(aValue); }
  inline void SetElCaloMC_PosX(G4double aValue) { m_caloMCPosX.push_back(aValue); }
  inline void SetElCaloMC_PosY(G4double aValue) { m_caloMCPosY.push_back(aValue); }
  inline void SetElCaloMC_PosZ(G4double aValue) { m_caloMCPosZ.push_back(aValue); }
  inline void SetElCaloMC_DirX(G4double aValue) { m_caloMCDirX.push_back(aValue); }
  inline void SetElCaloMC_DirY(G4double aValue) { m_caloMCDirY.push_back(aValue); }
  inline void SetElCaloMC_DirZ(G4double aValue) { m_caloMCDirZ.push_back(aValue); }

private:
  // Fast Sim Calo entrace particle properties to store in ntuple
  std::vector<G4int> m_caloMCPDG;
  std::vector<G4double> m_caloMCE;
  std::vector<G4double> m_caloMCPosX;
  std::vector<G4double> m_caloMCPosY;
  std::vector<G4double> m_caloMCPosZ;
  std::vector<G4double> m_caloMCDirX;
  std::vector<G4double> m_caloMCDirY;
  std::vector<G4double> m_caloMCDirZ;
};

} // namespace ddml
