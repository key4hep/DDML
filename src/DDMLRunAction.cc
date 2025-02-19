#include "DDML/DDMLRunAction.h"
#include "DDML/DDMLEventAction.h"
#include "G4AnalysisManager.hh"
#include "G4EventManager.hh"
#include "DD4hep/InstanceCount.h"
#include "G4Run.hh"

#define DEBUGPRINT 0

namespace ddml {

/// Standard constructor with initializing arguments
DDMLRunAction::DDMLRunAction(dd4hep::sim::Geant4Context* c, const std::string& n):
        Geant4RunAction(c, n) {
    // Create analysis manager
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    analysisManager->SetDefaultFileType("root");

    // Default filename, can be overriden with /analysis/setFileName
    analysisManager->SetFileName("Output");
    dd4hep::InstanceCount::increment(this);
}

/// Default destructor
DDMLRunAction::~DDMLRunAction() {
    dd4hep::InstanceCount::decrement(this);
}

/// begin-of-run callback
void DDMLRunAction::begin(const G4Run*) {
    // Get analysis manager
G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

// Create directories
analysisManager->SetVerboseLevel(0);

// Create ntuples
analysisManager->CreateNtuple("global", "Event data");
analysisManager->CreateNtupleIColumn("MC_PDG");
analysisManager->CreateNtupleDColumn("MC_Energy");
analysisManager->CreateNtupleDColumn("MC_PosX");
analysisManager->CreateNtupleDColumn("MC_PosY");
analysisManager->CreateNtupleDColumn("MC_PosZ");
analysisManager->CreateNtupleDColumn("MC_DirX");
analysisManager->CreateNtupleDColumn("MC_DirY");
analysisManager->CreateNtupleDColumn("MC_DirZ");
analysisManager->FinishNtuple();

/*
DDMLEventAction* mEventAction = dynamic_cast<DDMLEventAction*>(G4EventManager::GetEventManager()->GetUserEventAction());
analysisManager->CreateNtuple("Fast Sim Info", "MC info at calo face");
analysisManager->CreateNtupleIColumn("Calo_MC_PDG", mEventAction->GetCaloMC_PDG());
analysisManager->CreateNtupleDColumn("Calo_MC_Energy", mEventAction->GetCaloMC_E());
analysisManager->CreateNtupleDColumn("Calo_MC_PosX", mEventAction->GetCaloMC_PosX());
analysisManager->CreateNtupleDColumn("Calo_MC_PosY", mEventAction->GetCaloMC_PosY());
analysisManager->CreateNtupleDColumn("Calo_MC_PosZ", mEventAction->GetCaloMC_PosZ());
analysisManager->CreateNtupleDColumn("Calo_MC_DirX", mEventAction->GetCaloMC_DirX());
analysisManager->CreateNtupleDColumn("Calo_MC_DirY", mEventAction->GetCaloMC_DirY());
analysisManager->CreateNtupleDColumn("Calo_MC_DirZ", mEventAction->GetCaloMC_DirZ());
analysisManager->FinishNtuple();
*/

analysisManager->CreateNtuple("Fast_Sim_Info", "MC info at calo face");
analysisManager->CreateNtupleIColumn("Calo_MC_PDG");
analysisManager->CreateNtupleDColumn("Calo_MC_Energy");
analysisManager->CreateNtupleDColumn("Calo_MC_PosX");
analysisManager->CreateNtupleDColumn("Calo_MC_PosY");
analysisManager->CreateNtupleDColumn("Calo_MC_PosZ");
analysisManager->CreateNtupleDColumn("Calo_MC_DirX");
analysisManager->CreateNtupleDColumn("Calo_MC_DirY");
analysisManager->CreateNtupleDColumn("Calo_MC_DirZ");
analysisManager->FinishNtuple();


analysisManager->CreateNtuple("run", "Run data");
analysisManager->CreateNtupleDColumn("N");
analysisManager->FinishNtuple();

analysisManager->OpenFile();
}

/// End-of-run callback
void DDMLRunAction::end(const G4Run* aRun){
    auto analysisManager = G4AnalysisManager::Instance();
    analysisManager->FillNtupleDColumn(2, 0, aRun->GetNumberOfEvent());
    analysisManager->AddNtupleRow(2);
    analysisManager->Write();
    analysisManager->CloseFile();
}
    
} // namespace

#include "DDG4/Factories.h"
DECLARE_GEANT4ACTION_NS(ddml, DDMLRunAction)