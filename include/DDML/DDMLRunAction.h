#include <CLHEP/Units/SystemOfUnits.h>       // for GeV
#include <G4String.hh>                       // for G4String
#include <G4ThreeVector.hh>                  // for G4ThreeVector
#include <G4Types.hh>                        // for G4int
//#include "G4Timer.hh"            // for G4Timer
class G4ParticleDefinition;
#include "DDG4/Geant4Handle.h"
#include "DDG4/Geant4Kernel.h"
#include "DDG4/Geant4RunAction.h"



namespace ddml{
/* Create analysis files in standard G4 manner
*
* @author P. McKeown
* @date Feb 2025
*
*/

class DDMLRunAction: public dd4hep::sim::Geant4RunAction {
    public:
        DDMLRunAction() = delete;
        /// Standard constructor with initializing arguments
        DDMLRunAction(dd4hep::sim::Geant4Context* c, const std::string& n) ;
        /// Default destructor
        virtual ~DDMLRunAction() ;
        /// begin-of-run callback
        void begin(const G4Run*) override;
        /// End-of-run callback
        void end(const G4Run*) override;
        /// begin-of-event callback
        void beginEvent(const G4Event*) ;
        /// End-of-event callback
        void endEvent(const G4Event*) ;

};

} // namespace