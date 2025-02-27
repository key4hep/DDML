//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//

#include "DDML/Geant4FastHitMakerGlobal.h"

#include "G4TouchableHandle.hh"
#include "G4TransportationManager.hh"
#include "G4VSensitiveDetector.hh"

#include "G4VFastSimSensitiveDetector.hh"

Geant4FastHitMakerGlobal::Geant4FastHitMakerGlobal() {
  m_touchableHandle = new G4TouchableHistory();
  m_navigator = new G4Navigator();
  m_naviSetup = false;
  m_worldWithSdName = "";
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

Geant4FastHitMakerGlobal::~Geant4FastHitMakerGlobal() {
  delete m_navigator;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void Geant4FastHitMakerGlobal::make(const G4FastHit& aHit, const G4FastTrack& aTrack) {
  // do not make empty deposit
  if (aHit.GetEnergy() <= 0) {
    return;
  }
  // Locate the spot
  if (!m_naviSetup) {
    // Choose the world volume that contains the sensitive detector based on its
    // name (empty name for mass geometry)
    G4VPhysicalVolume* worldWithSD = nullptr;
    if (m_worldWithSdName.empty()) {
      worldWithSD = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking()->GetWorldVolume();
    } else {
      worldWithSD = G4TransportationManager::GetTransportationManager()->GetParallelWorld(m_worldWithSdName);
    }
    m_navigator->SetWorldVolume(worldWithSD);
    // use track global position
    m_navigator->LocateGlobalPointAndUpdateTouchable(
        //      aTrack.GetPrimaryTrack()->GetPosition(),
        aHit.GetPosition(), m_touchableHandle(), false);
    m_naviSetup = true;
  } else {
    // for further deposits use hit (local) position and local->global
    // transformation
    m_navigator->LocateGlobalPointAndUpdateTouchable(
        //      aTrack.GetInverseAffineTransformation()->TransformPoint(
        //      aHit.GetPosition()),
        aHit.GetPosition(), m_touchableHandle());
  }
  G4VPhysicalVolume* currentVolume = m_touchableHandle()->GetVolume();

  G4VSensitiveDetector* sensitive;
  if (currentVolume != 0) {
    sensitive = currentVolume->GetLogicalVolume()->GetSensitiveDetector();
    G4VFastSimSensitiveDetector* fastSimSensitive = dynamic_cast<G4VFastSimSensitiveDetector*>(sensitive);
    if (fastSimSensitive) {
      fastSimSensitive->Hit(&aHit, &aTrack, &m_touchableHandle);
    } else if (sensitive && currentVolume->GetLogicalVolume()->GetFastSimulationManager()) {
      G4cerr << "ERROR - Geant4FastHitMakerGlobal::make()" << G4endl << "        It is required to derive from the "
             << G4endl << "        G4VFastSimSensitiveDetector in " << G4endl
             << "        addition to the usual G4VSensitiveDetector class." << G4endl;
      G4Exception("Geant4FastHitMakerGlobal::make()", "InvalidSetup", FatalException,
                  "G4VFastSimSensitiveDetector interface not implemented.");
    }
  }
}
