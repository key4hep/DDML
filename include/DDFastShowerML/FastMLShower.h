#ifndef FastMLShower_H
#define FastMLShower_H

#include <DDG4/Geant4FastSimShowerModel.h>


/// Namespace for the AIDA detector description toolkit
namespace dd4hep  {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim  {

    /** The templated base class for running fast shower simulation with ML.
     *  The actual implementation is or can be provided by the templated class 
     *  ML_MODEL.  
     * 
     * @author F.Gaede, DESY
     * @date Mar 2023
     */
    template <typename ML_MODEL>
    class FastMLShower : public Geant4FastSimShowerModel    {

    protected:
      ML_MODEL model;

    public:
      /** C'tor that calls initialize of the concrete model implementation in order to
       *  allow for dedicated properties to be declared.
       */
      FastMLShower(Geant4Context* context, const std::string& nam)
	: Geant4FastSimShowerModel(context, nam) {
	
	model.declareProperties( this );
      }

      /// Default destructor
      virtual ~FastMLShower(){}

      /// Geometry construction callback. Called at "Construct()"
      virtual void constructGeo(Geant4DetectorConstructionContext* ctxt)  override {
	// if( model.has_constructGeo )      model.constructGeo( ctxt ) ;
	// else
	  this->Geant4FastSimShowerModel::constructGeo(ctxt);
				    
      }

      /// Electromagnetic field construction callback. Called at "ConstructSDandField()"
      virtual void constructField(Geant4DetectorConstructionContext* ctxt)  override {
	// if( model.has_constructField )      model.constructField( ctxt ) ;
	// else
	this->Geant4FastSimShowerModel::constructField(ctxt);
      }

      /// Sensitive detector construction callback. Called at "ConstructSDandField()"
      virtual void constructSensitives(Geant4DetectorConstructionContext* ctxt)  override {
	// if( model.has_constructSensitives )      model.constructSensitives( ctxt ) ;
	// else
	this->Geant4FastSimShowerModel::constructSensitives(ctxt);
    }

      /// User callback to determine if the model is applicable for the particle type
      /** Default implementation checks if the particle is registered in 'ApplicableParticles'
       */
      virtual bool check_applicability(const G4ParticleDefinition& particle)  override {
	// if( model.has_check_applicability )   return  model.check_applicability(particle) ;
	// else
	return this->Geant4FastSimShowerModel::check_applicability(particle);
      }
      
      /// User callback to determine if the shower creation should be triggered
      /** Default implementation checks if for all particles registered in 'Etrigger'
       *  the kinetic energy is bigger than the value.
       */
      virtual bool check_trigger(const G4FastTrack& track)  override {
	// if( model.has_check_trigger ) return model.check_trigger(track ) ;
	// else
	return this->Geant4FastSimShowerModel::check_trigger(track);
      }

      /// User callback to model the particle/energy shower
      virtual void modelShower(const G4FastTrack& track, G4FastStep& step)  override ;
    };
  }     /* End namespace sim   */
}       /* End namespace dd4hep */
#endif // DDG4_GEANT4FASTSIMSHOWERMODEL_H
