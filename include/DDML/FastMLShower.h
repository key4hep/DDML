#ifndef FastMLShower_H
#define FastMLShower_H

#include <DDG4/Geant4FastSimShowerModel.h>

#include <G4FastStep.hh>
#include <G4FastTrack.hh>
#include <G4Track.hh>
#include <G4FastHit.hh>

#include "DDML/DDML.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep  {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim  {

    /** The templated base class for running fast shower simulation with ML.
     *  The actual implementation is provided by the templated class 
     *  ML_MODEL.  
     * 
     * @author F.Gaede, DESY
     * @date Mar 2023
     */
    template <typename ML_MODEL>
    class FastMLShower : public Geant4FastSimShowerModel    {

    protected:
      ML_MODEL fastsimML ;

    public:
      /** C'tor that calls initialize of the concrete model implementation in order to
       *  allow for dedicated properties to be declared.
       */
      FastMLShower(Geant4Context* context, const std::string& nam)
	: Geant4FastSimShowerModel(context, nam) {
	
	fastsimML.declareProperties( this );
      }

      /// Default destructor
      virtual ~FastMLShower(){}

      /// Geometry construction callback. Called at "Construct()"
      virtual void constructGeo(Geant4DetectorConstructionContext* ctxt)  override {
	// if( fastsimML.has_constructGeo )      fastsimML.constructGeo( ctxt ) ;
	// else
	  this->Geant4FastSimShowerModel::constructGeo(ctxt);
				    
      }

      /// Electromagnetic field construction callback. Called at "ConstructSDandField()"
      virtual void constructField(Geant4DetectorConstructionContext* ctxt)  override {
	// if( fastsimML.has_constructField )      fastsimML.constructField( ctxt ) ;
	// else
	this->Geant4FastSimShowerModel::constructField(ctxt);
      }

      /// Sensitive detector construction callback. Called at "ConstructSDandField()"
      virtual void constructSensitives(Geant4DetectorConstructionContext* ctxt)  override {
	// if( fastsimML.has_constructSensitives )      fastsimML.constructSensitives( ctxt ) ;
	// else
	this->Geant4FastSimShowerModel::constructSensitives(ctxt);
    }

      /// User callback to determine if the model is applicable for the particle type
      /** Default implementation checks if the particle is registered in 'ApplicableParticles'
       */
      virtual bool check_applicability(const G4ParticleDefinition& particle)  override {
	// if( fastsimML.has_check_applicability )   return  fastsimML.check_applicability(particle) ;
	// else
	return this->Geant4FastSimShowerModel::check_applicability(particle);
      }
      
      /// User callback to determine if the shower creation should be triggered
      /** Default implementation checks if for all particles registered in 'Etrigger'
       *  the kinetic energy is bigger than the value.
       */
      virtual bool check_trigger(const G4FastTrack& track)  override {
	// if( fastsimML.has_check_trigger ) return fastsimML.check_trigger(track ) ;
	// else
	return this->Geant4FastSimShowerModel::check_trigger(track);
      }

      /// User callback to model the particle/energy shower - details defined in ML_MODEL
      virtual void modelShower(const G4FastTrack& track, G4FastStep& step) override {
	
	// remove particle from further processing by G4
	step.KillPrimaryTrack();
	step.SetPrimaryTrackPathLength(0.0);
	G4double energy = track.GetPrimaryTrack()->GetKineticEnergy();
	step.SetTotalEnergyDeposited(energy);
	
	
	std::vector<float> input, output ;
	std::vector<ddml::SpacePointVec> spacepoints ;
	
	fastsimML.model.prepareInput( track, input , output ) ;
	
	fastsimML.inference.runInference(input, output ) ;
	
	fastsimML.model.convertOutput( track, output , spacepoints) ;
	
	fastsimML.geometry.localToGlobal( track, spacepoints ) ;
	
	// now deposit energies in the detector using calculated global positions
	
	for( auto& layerSPs : spacepoints )
	  for( auto& sp : layerSPs ) {
	    fastsimML.hitMaker->make( G4FastHit( G4ThreeVector(sp.X,sp.Y,sp.Z) , sp.E ), track);
	  }
      }
      
    };
  }     /* End namespace sim   */
}       /* End namespace dd4hep */
#endif // DDG4_GEANT4FASTSIMSHOWERMODEL_H
