#include "DDML/DDMLEventAction.h"
#include "G4AnalysisManager.hh"
#include "DD4hep/InstanceCount.h"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4ThreeVector.hh"
#include "G4PrimaryVertex.hh"
#include "G4PrimaryParticle.hh"
#include "DDG4/Geant4Data.h"

#define DEBUGPRINT 0

namespace ddml {

DDMLEventAction::DDMLEventAction(dd4hep::sim::Geant4Context* c, const std::string& n):
Geant4EventAction(c,n)
    {
        dd4hep::InstanceCount::increment(this);
    }

/// Default destructor
DDMLEventAction::~DDMLEventAction() {
    dd4hep::InstanceCount::decrement(this);
    }

void DDMLEventAction::begin(const G4Event*){}

void DDMLEventAction::end(const G4Event*) {
    // Get analysis manager
    auto analysisManager = G4AnalysisManager::Instance();

    // Retrieve information from primary vertex and primary particle
    auto primaryVertex =
    G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetPrimaryVertex();
    G4ThreeVector primaryVertexPos = primaryVertex->GetPosition();
    auto primaryParticle   = primaryVertex->GetPrimary(0);
    G4double primaryEnergy = primaryParticle->GetTotalEnergy();
    G4ThreeVector primaryDirection = primaryParticle->GetMomentumDirection();
    G4int primaryPDG = primaryParticle->GetPDGcode();

    // Fill analysis manager
    analysisManager->FillNtupleIColumn(0, 0, primaryPDG);
    analysisManager->FillNtupleDColumn(0, 1, primaryEnergy);
    analysisManager->FillNtupleDColumn(0, 2, primaryVertexPos.x());
    analysisManager->FillNtupleDColumn(0, 3, primaryVertexPos.y());
    analysisManager->FillNtupleDColumn(0, 4, primaryVertexPos.z());
    analysisManager->FillNtupleDColumn(0, 5, primaryDirection.x());
    analysisManager->FillNtupleDColumn(0, 6, primaryDirection.y());
    analysisManager->FillNtupleDColumn(0, 7, primaryDirection.z());

    analysisManager->AddNtupleRow(0);

}      

} //namespace

#include "DDG4/Factories.h"
DECLARE_GEANT4ACTION_NS(ddml, DDMLEventAction)
