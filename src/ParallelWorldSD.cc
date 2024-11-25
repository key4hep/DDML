#include "DDG4/Geant4SensDetAction.inl"
#include "DDG4/Geant4FastSimHandler.h"
#include "DDG4/Geant4EventAction.h"
#include "DD4hep/DetType.h"
#include "G4Event.hh"


/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim   {

    /// Class to implement the standard sensitive detector for scoring mesh calorimeters
    struct Geant4FullFastCalorimeter : public Geant4Calorimeter{
      /// Slice ID of the sensitive layer
      int fSensitiveEMSlice = 0;
      int fSensitiveHadSlice = 0;
      Geant4FullFastCalorimeter() : Geant4Calorimeter() {}
    };
    /// Define collections created by this sensitivie action object
    template <> void Geant4SensitiveAction<Geant4FullFastCalorimeter>::defineCollections() {
      m_collectionID = defineCollection<Geant4Calorimeter::Hit>(m_sensitive.readout().name());
    }
    /// Initialization overload for specialization
    template <> void Geant4SensitiveAction<Geant4FullFastCalorimeter>::initialize() {}
    /// template specialization for c'tor
    template <> 
    Geant4SensitiveAction<Geant4FullFastCalorimeter>::Geant4SensitiveAction(Geant4Context* ctxt,
										const std::string& nam,
										DetElement det,
										Detector& lcdd_ref)
      : Geant4Sensitive(ctxt,nam,det,lcdd_ref), m_collectionID(0)
    {
      initialize();
      defineCollections();
      InstanceCount::increment(this);
      declareProperty("sensitiveEMSlice", m_userData.fSensitiveEMSlice );
      declareProperty("sensitiveHadSlice", m_userData.fSensitiveHadSlice );
      // TODO figure out from the DetType which detector is it: EM or Had.
      // for the moment use only EM 
    }
    /// Method for generating hit(s) using the information of G4Step object.
    /// based on Geant4SensitiveAction<Geant4Calorimeter>::process from DDG4/plugins/Geant4SDActions.cpp
    template <> bool
    Geant4SensitiveAction<Geant4FullFastCalorimeter>::process(const G4Step* step,G4TouchableHistory*) {
      typedef Geant4Calorimeter::Hit Hit;
      Geant4StepHandler    h(step);
      HitContribution      contrib = Hit::extractContribution(step);
      Geant4HitCollection* coll    = collection(m_collectionID);
      VolumeID cell = 0;

      try {
        cell = cellID(step);
      } catch(std::runtime_error &e) {
        std::stringstream out;
        out << std::setprecision(20) << std::scientific;
        out << "ERROR: " << e.what()  << std::endl;
        out << "Position: "
            << "Pre (" << std::setw(24) << step->GetPreStepPoint()->GetPosition() << ") "
            << "Post (" << std::setw(24) << step->GetPostStepPoint()->GetPosition() << ") "
            << std::endl;
        out << "Momentum: "
            << " Pre (" <<std::setw(24) << step->GetPreStepPoint() ->GetMomentum()  << ") "
            << " Post (" <<std::setw(24) << step->GetPostStepPoint()->GetMomentum() << ") "
            << std::endl;
        std::cout << out.str();
        return true;
      }

      // For full simulation: proceed only if we entered a sensitive slice
      if( ! (m_readout.idSpec().decoder()->get(cell, "slice") == m_userData.fSensitiveEMSlice) ) return true;

      Hit* hit = coll->findByKey<Hit>(cell);
      if ( !hit ) {
        Geant4TouchableHandler handler(step);
        DDSegmentation::Vector3D pos = m_segmentation.position(cell);
        Position global = h.localToGlobal(pos);
        hit = new Hit(global);
        hit->cellID = cell;
        coll->add(cell, hit);
        printM2("%s> CREATE hit with deposit:%e MeV  Pos:%8.2f %8.2f %8.2f  %s  [%s]",
                c_name(),contrib.deposit,pos.X,pos.Y,pos.Z,handler.path().c_str(),
                coll->GetName().c_str());
        if ( 0 == hit->cellID )  { // for debugging only!
          hit->cellID = cellID(step);
          except("+++ Invalid CELL ID for hit!");
        }
      }
      hit->truth.emplace_back(contrib);
      hit->energyDeposit += contrib.deposit;
      mark(h.track);
      return true;
    }
    /// Method for generating hit(s) using the information of Geant4FastSimSpot object.
    /// based on Geant4SensitiveAction<Geant4Calorimeter>::process from DDG4/plugins/Geant4SDActions.cpp
    template <> bool
    Geant4SensitiveAction<Geant4FullFastCalorimeter>::processFastSim(const Geant4FastSimSpot* spot,
							     G4TouchableHistory* /* hist */)
    {
      typedef Geant4Calorimeter::Hit Hit;
      Geant4FastSimHandler h(spot);
      HitContribution      contrib = Hit::extractContribution(spot);
      Geant4HitCollection* coll    = collection(m_collectionID);
      VolumeID cell = 0;
      
      try {
        cell = cellID(h.touchable(), h.avgPositionG4());
      } catch(std::runtime_error &e) {
        std::stringstream out;
        out << std::setprecision(20) << std::scientific;
        out << "ERROR: " << e.what()  << std::endl;
        out << "Position: (" << std::setw(24) << h.avgPositionG4() << ") " << std::endl;
        out << "Momentum: (" << std::setw(24) << h.momentumG4() << ") " << std::endl;
        std::cout << out.str();
        return true;
      }

      Hit* hit = coll->findByKey<Hit>(cell);
      if ( !hit ) {
	      Geant4TouchableHandler   handler(h.touchable());
        DDSegmentation::Vector3D pos = m_segmentation.position(cell);
        Position global = h.localToGlobal(pos);
        hit = new Hit(global);
        hit->cellID = cell;
        coll->add(cell, hit);
        printM2("%s> CREATE hit with deposit:%e MeV  Pos:%8.2f %8.2f %8.2f  %s  [%s]",
                c_name(),contrib.deposit,pos.X,pos.Y,pos.Z,handler.path().c_str(),
                coll->GetName().c_str());
        if ( 0 == hit->cellID )  { // for debugging only!
          hit->cellID = cellID(h.touchable(), h.avgPositionG4());
          except("+++ Invalid CELL ID for hit!");
        }
      }
      hit->truth.emplace_back(contrib);
      hit->energyDeposit += contrib.deposit;
      mark(h.track);
      return true;
    }

    typedef Geant4SensitiveAction<Geant4FullFastCalorimeter> Geant4FullFastCalorimeterAction;

  }}
#include "DDG4/Factories.h"
DECLARE_GEANT4SENSITIVE(Geant4FullFastCalorimeterAction)

