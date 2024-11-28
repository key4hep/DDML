#include "DDG4/Geant4SensDetAction.inl"
#include "DDG4/Geant4FastSimHandler.h"
#include "DDG4/Geant4EventAction.h"
#include "DD4hep/DetType.h"
#include "G4Event.hh"
//#include "DDCore/IDDescriptor.h" // For IDDescriptor
//#include "DDCore/VolumeManager.h" // For PlacedVolume
//#include "Geant4Mapping.h" 
//#include "DD4hep/VolumeManager.h"

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
      //if( ! (m_readout.idSpec().decoder()->get(cell, "slice") == m_userData.fSensitiveEMSlice) ) return true;
      if( ! (m_readout.idSpec().decoder()->get(cell, "submodule") == m_userData.fSensitiveEMSlice) ) return true; // String used should be specific in the steering file!
      
      std::cout << "FULL pre SubModule string: " << m_readout.idSpec().decoder()->valueString(cell) << std::endl;
      
      // Which material are we in?
      std::cout << "FULL step material: " << h.volume(step->GetPreStepPoint())->GetLogicalVolume()->GetMaterial()->GetName() << std::endl;

      // Which material are we in?
      std::cout << "FULL Pre Cell: " << cell << std::endl;

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

        std::cout << "Actual FULL Hit creation Pos: x: " << hit->position.x() <<" y: " << hit->position.y() << " z: " << hit->position.z() << std::endl;

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
    /*********************************************************************************************
     *********************************************************************************************
     **************************** What does this do? ********************************************* 
     * Sensitive detector for placement of fast sim hits generated with a generic parallel world
     * scoring (e.g. cylindrical mesh) into the readout cells defined by the segmentation. 
     * This assumes your hit has already landed in the correct calorimeter layer, but may not be
     * correctly placed onto the sensitive slice.
     * Placement is achieved by vertical translation into the senstive slice by means of the cell
     * ID.
     * NOTE!!!! This assummes you have a regular slicing structure within a layer. So a 
     * segmentation in a calorimeter in which slices change size will not work. Here is a diagram:
     *                     |-------/
     *  absorber slice:    |-----x/ <----- 1. Hit in edge of absorber will be translated down
     *                     |-----/
     *  sensitive slice:   |----/x <-----  2. Hit now falls outside of the layer 
     *                     |---/
     * 
     * @author     P. McKeown, CERN
     * @author     A. Zaborowska, CERN
     * @date       Nov. 2024
     * 
     *********************************************************************************************
     *********************************************************************************************
     *********************************************************************************************
     *********************************************************************************************/

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


      //std::cout << "Cell layer: " << m_readout.idSpec().decoder()->get(cell, "layer") << std::endl;
      std::cout << "pre SubModule string: " << m_readout.idSpec().decoder()->valueString(cell) << std::endl;
      
      // Which material are we in?
      std::cout << "Spot material: " << spot->volume()->GetLogicalVolume()->GetMaterial()->GetName() << std::endl;

    // And where are we??
      std::cout << "Spot position: x: " << spot->hitPosition().x() << " y: " << spot->hitPosition().y() << " z: " << spot->hitPosition().z() << std::endl;

    // Which cell?
    std::cout << "Pre Cell: " << cell << std::endl;

      // Cell position?
       // Position of cell where hit is now
      DDSegmentation::Vector3D pre_pos_cell = m_segmentation.position(cell);
      std::cout << "Pre Cell local pos: x: " << pre_pos_cell.x() << " y: " << pre_pos_cell.y() << " z: " << pre_pos_cell.z() << std::endl;
      //std::cout << "Pre Cell global pos: x: " << h.localToGlobal(pre_pos_cell).x() << " y: " << h.localToGlobal(pre_pos_cell).y() << " z: " << h.localToGlobal(pre_pos_cell).z() << std::endl;

      // Set the spot to be in the sensitive material- based on the cell ID
      // Here it is assumed that the "submodule" is the field which corresponds to the slices within a layer

      m_readout.idSpec().decoder()->set(cell, "submodule", m_userData.fSensitiveEMSlice); // String used should be specific in the steering file! 

      //std::cout << "Cell layer: " << m_readout.idSpec().decoder()->get(cell, "layer") << std::endl;
      std::cout << "post SubModule string: " << m_readout.idSpec().decoder()->valueString(cell) << std::endl;
      
      // Encode to a volume id:
      //VolumeID encoded_cell = m_readout.idSpec().encode(m_readout.idSpec().decoder()->fieldDescription())

      // Check Volume ID:
      VolumeID pre_vol_id = m_segmentation->volumeID(cell);

      std::cout << "Pre Volume ID: " << pre_vol_id << std::endl;

      // Get volume manager and look up volumeID to get local to world
      VolumeManager VolMan = VolumeManager().getVolumeManager(m_detDesc);
      std::cout << "Got VOl Manager: " << std::endl;
      DetElement Pre_DetEl = VolMan.lookupDetElement(pre_vol_id);
      std::cout << "Got Local Pos: " << std::endl;
      //Position local_DetEl_Pos = DetEl.placement().position();
      dd4hep::Position pre_cell_loc_pos(pre_pos_cell.x(), pre_pos_cell.y(), pre_pos_cell.z()); // I don't know if there is already functionality to do this conversion in DD4hep?
      Position pre_glob_pos = Pre_DetEl.nominal().localToWorld(pre_cell_loc_pos);

      std::cout << "Pre DetEl Global Position: x: " << pre_glob_pos.x() << " y: " << pre_glob_pos.y() <<  " z: " << pre_glob_pos.z() << std::endl;


      // Get volume placement
      //PlaceVolume local_cell_vol = VolumeManager->lookupVolumePlacement(vol_id);

      /*
      std::cout << "global volume pos: x: " <<  h.localToGlobal(local_cell_vol->position().x()) 
                << " y: " << h.localToGlobal(local_cell_vol->position().y()) 
                << " z: " << h.localToGlobal(local_cell_vol->position().z()) << std::endl;
      */

      // Which cell?
      std::cout << "Post Cell: " << cell << std::endl;

      // Position of cell where hit is now
      DDSegmentation::Vector3D post_pos_cell = m_readout.segmentation().position(cell);
      std::cout << "Post Cell local pos: x: " << post_pos_cell.x() << " y: " << post_pos_cell.y() << " z: " << post_pos_cell.z() << std::endl;
      std::cout << "Post Cell global pos: x: " << h.localToGlobal(post_pos_cell).x() << " y: " << h.localToGlobal(post_pos_cell).y() << " z: " << h.localToGlobal(post_pos_cell).z() << std::endl;

      Hit* hit = coll->findByKey<Hit>(cell);
      if ( !hit ) {
	      Geant4TouchableHandler   handler(h.touchable());  /// This is no longer used to localToGlobal
        DDSegmentation::Vector3D pos = m_segmentation.position(cell);

        // Check Volume ID:
        VolumeID post_vol_id = m_segmentation->volumeID(cell);

        std::cout << "Post Volume ID: " << post_vol_id << std::endl;

        // Already got vol manager for testing- see above.... (Move here when happy!)
        // Get volume manager and look up volumeID to get local to world
        //VolumeManager VolMan = VolumeManager().getVolumeManager(m_detDesc);
        //std::cout << "Got VOl Manager: " << std::endl;
        DetElement Post_DetEl = VolMan.lookupDetElement(post_vol_id);
        std::cout << "Got Post Local Pos: " << std::endl;
        //Position local_DetEl_Pos = DetEl.placement().position();
        dd4hep::Position post_cell_loc_pos(post_pos_cell.x(), post_pos_cell.y(), post_pos_cell.z()); // I don't know if there is already functionality to do this conversion in DD4hep?
        Position global = Post_DetEl.nominal().localToWorld(post_cell_loc_pos)*CLHEP::cm;

        std::cout << "Post DetEl Global Position: x: " << global.x() << " y: " << global.y() <<  " z: " << global.z() << std::endl;
        
        //Position global = h.localToGlobal(pos);
        
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

      std::cout << "Actual Hit creation Pos: x: " << hit->position.x() <<" y: " << hit->position.y() << " z: " << hit->position.z() << std::endl;

      mark(h.track);
      return true;
    }

    template <> void Geant4SensitiveAction<Geant4FullFastCalorimeter>::end(G4HCofThisEvent* hce) 
    {
      //IDDescriptor Descriptor
      //G4VHitsCollection *  HitsColl = hce->GetHC();
      //std::cout << "G4 No hits collection: " << hce->GetNumberOfCollections() << std::endl;
    }

    typedef Geant4SensitiveAction<Geant4FullFastCalorimeter> Geant4FullFastCalorimeterAction;

  }}
#include "DDG4/Factories.h"
DECLARE_GEANT4SENSITIVE(Geant4FullFastCalorimeterAction)

