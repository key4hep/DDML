#include <G4Types.hh>            // for G4int, G4double
#include <vector>                // for vector
#include "G4UserEventAction.hh"  // for G4UserEventAction
//#include "G4Timer.hh"            // for G4Timer
class G4Event;
#include "DDG4/Geant4Handle.h"
#include "DDG4/Geant4Kernel.h"
#include "DDG4/Geant4EventAction.h"   
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"


namespace ddml {

/** Event action for ddml. 
*
*  @author P. McKeown, CERN
*  @date Feb 2025
*
*/

class DDMLEventAction: public dd4hep::sim::Geant4EventAction{
public:
    /// Standard constructor with initializing arguments
    DDMLEventAction(dd4hep::sim::Geant4Context* c, const std::string& n);
    /// Default destructor
    virtual ~DDMLEventAction();
    /// begin-of-event callback
    inline virtual void begin(const G4Event*) override;
    /// End-of-event callback
    virtual void end(const G4Event*) override;
    /// begin-of-run callback
    void beginRun(const G4Run*);
    /// End-of-run callback
    void endRun(const G4Run*);

    //// Get and Set methods for Calo Face info
    inline std::vector<G4int>& GetCaloMC_PDG() {return m_CaloMCPDG;}
    inline std::vector<G4double>& GetCaloMC_E() {return m_CaloMCE;}
    inline std::vector<G4double>& GetCaloMC_PosX() {return m_CaloMCPosX;}
    inline std::vector<G4double>& GetCaloMC_PosY() {return m_CaloMCPosY;}
    inline std::vector<G4double>& GetCaloMC_PosZ() {return m_CaloMCPosZ;}
    inline std::vector<G4double>& GetCaloMC_DirX() {return m_CaloMCDirX;}
    inline std::vector<G4double>& GetCaloMC_DirY() {return m_CaloMCDirY;}
    inline std::vector<G4double>& GetCaloMC_DirZ() {return m_CaloMCDirZ;}

    // Set methods to push back vector
    inline void SetElCaloMC_PDG(G4int aValue) {m_CaloMCPDG.push_back(aValue);}
    inline void SetElCaloMC_E(G4double aValue) {m_CaloMCE.push_back(aValue);}
    inline void SetElCaloMC_PosX(G4double aValue) {m_CaloMCPosX.push_back(aValue);}
    inline void SetElCaloMC_PosY(G4double aValue) {m_CaloMCPosY.push_back(aValue);}
    inline void SetElCaloMC_PosZ(G4double aValue) {m_CaloMCPosZ.push_back(aValue);}
    inline void SetElCaloMC_DirX(G4double aValue) {m_CaloMCDirX.push_back(aValue);}
    inline void SetElCaloMC_DirY(G4double aValue) {m_CaloMCDirY.push_back(aValue);}
    inline void SetElCaloMC_DirZ(G4double aValue) {m_CaloMCDirZ.push_back(aValue);}

private:
    // Fast Sim Calo entrace particle properties to store in ntuple
    std::vector<G4int> m_CaloMCPDG;
    std::vector<G4double> m_CaloMCE;
    std::vector<G4double> m_CaloMCPosX;
    std::vector<G4double> m_CaloMCPosY;
    std::vector<G4double> m_CaloMCPosZ;
    std::vector<G4double> m_CaloMCDirX;
    std::vector<G4double> m_CaloMCDirY;
    std::vector<G4double> m_CaloMCDirZ;
};

} //namespace

