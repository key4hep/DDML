#ifndef FastMLShower_H
#define FastMLShower_H

#include <DDG4/Geant4FastSimShowerModel.h>

#include <G4FastStep.hh>
#include <G4FastTrack.hh>
#include <G4Track.hh>
#include <G4FastHit.hh>

#include "DDML/DDML.h"

#ifndef DDML_INSTRUMENT_MODEL_SHOWER
#define DDML_INSTRUMENT_MODEL_SHOWER 0
#endif

#if DDML_INSTRUMENT_MODEL_SHOWER
#include "podio/ROOTFrameWriter.h"
#include "podio/UserDataCollection.h"
#include "podio/Frame.h"

#include <chrono>
#include <functional>
#include <numeric>

using ClockT = std::chrono::high_resolution_clock;

template <class Obj, typename MemberFunc, typename... Args>
inline auto run_void_member_timed(Obj& obj, MemberFunc func, Args&&... args) {
  const auto start = ClockT::now();
  std::invoke(func, obj, std::forward<Args>(args)...);
  const auto end = ClockT::now();

  return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
}
#endif


namespace ddml {
  /** The templated base class for running fast shower simulation with ML.
   *  The actual implementation is provided by the templated class 
   *  ML_MODEL.  
   * 
   * @author F.Gaede, DESY
   * @date Mar 2023
   */
  template <typename ML_MODEL>
  class FastMLShower : public dd4hep::sim::Geant4FastSimShowerModel    {

  protected:
    ML_MODEL fastsimML ;

    InputVecs _input ;
    TensorDimVecs _dimVecs ;
    std::vector<float> _output ;
    std::vector<ddml::SpacePointVec> _spacepoints ;

#if DDML_INSTRUMENT_MODEL_SHOWER
    podio::ROOTFrameWriter m_timeWriter;
#endif

  public:
    /** C'tor that calls initialize of the concrete model implementation in order to
     *  allow for dedicated properties to be declared.
     */
    FastMLShower(dd4hep::sim::Geant4Context* context, const std::string& nam)
      : Geant4FastSimShowerModel(context, nam)
#if DDML_INSTRUMENT_MODEL_SHOWER
      , m_timeWriter("times_" + nam + ".root")
#endif
      {
	
      fastsimML.declareProperties( this );
    }

    /// Default destructor
    virtual ~FastMLShower(){
#if DDML_INSTRUMENT_MODEL_SHOWER
      m_timeWriter.finish();
#endif
    }

    /// Geometry construction callback. Called at "Construct()"
    virtual void constructGeo(dd4hep::sim::Geant4DetectorConstructionContext* ctxt)  override {
      // if( fastsimML.has_constructGeo )      fastsimML.constructGeo( ctxt ) ;
      // else
      this->Geant4FastSimShowerModel::constructGeo(ctxt);
				    
    }

    /// Electromagnetic field construction callback. Called at "ConstructSDandField()"
    virtual void constructField(dd4hep::sim::Geant4DetectorConstructionContext* ctxt)  override {
      // if( fastsimML.has_constructField )      fastsimML.constructField( ctxt ) ;
      // else
      this->Geant4FastSimShowerModel::constructField(ctxt);
    }

    /// Sensitive detector construction callback. Called at "ConstructSDandField()"
    virtual void constructSensitives(dd4hep::sim::Geant4DetectorConstructionContext* ctxt)  override {
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
	
#if DDML_INSTRUMENT_MODEL_SHOWER
      podio::UserDataCollection<double> prepareInputTime;
      podio::UserDataCollection<double> runInferenceTime;
      podio::UserDataCollection<double> convertOutputTime;
      podio::UserDataCollection<double> localToGlobalTime;
      podio::UserDataCollection<double> hitMakerTime;
      podio::UserDataCollection<uint64_t> nHits;
#endif

      for( auto& invec : _input ) invec.clear() ;

      _output.clear() ;
      for( auto& layerSPs : _spacepoints )
	layerSPs.clear() ;

#if DDML_INSTRUMENT_MODEL_SHOWER
      prepareInputTime.push_back(
          run_void_member_timed(fastsimML.model,
                                &ML_MODEL::MLModelT::prepareInput, track,
                                _input, _output)
              .count());
      runInferenceTime.push_back(
          run_void_member_timed(fastsimML.inference,
                                &ML_MODEL::InferenceT::runInference, _input,
                                _output)
              .count());
      convertOutputTime.push_back(
          run_void_member_timed(fastsimML.model,
                                &ML_MODEL::MLModelT::convertOutput, track,
                                _output, _spacepoints)
              .count());
      localToGlobalTime.push_back(
          run_void_member_timed(fastsimML.geometry,
                                &ML_MODEL::GeometryT::localToGlobal, track,
                                _spacepoints)
              .count());

      nHits.push_back(std::accumulate(
          _output.begin(), _output.end(), 0u,
          [](const auto sum, const auto v) { return sum + (v != 0); }));
#else
      fastsimML.model.prepareInput( track, _input , _output ) ;
      fastsimML.inference.runInference(_input, _output ) ;
      fastsimML.model.convertOutput( track, _output , _spacepoints) ;
      fastsimML.geometry.localToGlobal( track, _spacepoints ) ;
#endif
      // now deposit energies in the detector using calculated global positions
#if DDML_INSTRUMENT_MODEL_SHOWER
      const auto start = ClockT::now();
#endif
      for( auto& layerSPs : _spacepoints )
	for( auto& sp : layerSPs ) {
	  fastsimML.hitMaker->make( G4FastHit( G4ThreeVector(sp.X,sp.Y,sp.Z) , sp.E ), track);
	}

#if DDML_INSTRUMENT_MODEL_SHOWER
      const auto end = ClockT::now();
      hitMakerTime.push_back(
          std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)
              .count());

      auto frame = podio::Frame{};
      frame.put(std::move(prepareInputTime), "prepareInput");
      frame.put(std::move(runInferenceTime), "runInference");
      frame.put(std::move(convertOutputTime), "convertOutput");
      frame.put(std::move(localToGlobalTime), "localToGlobal");
      frame.put(std::move(hitMakerTime), "hitMaker");
      frame.put(std::move(nHits), "nHits");

      m_timeWriter.writeFrame(frame, "events");
#endif
    }
  };


  /** Template class to put together a complete ML model by specifying implementations for the Inference, the Model, the Geometry and a HitMaker.
   * 
   * @author F.Gaede, DESY
   * @date Mar 2023
   */
  template< class Inference, class MLModel, class Geometry, class HitMaker>
  struct FastMLModel {

    using InferenceT = Inference;
    using MLModelT = MLModel;
    using GeometryT = Geometry;
    using HitMakerT = HitMaker;
    
    Inference inference={} ;
    MLModel   model={} ;
    Geometry  geometry={} ;
    HitMaker*  hitMaker={} ;
    
    FastMLModel() : hitMaker( new HitMaker ) {} 
    
    const bool has_constructGeo = false ;
    const bool has_constructField = false ;
    const bool has_constructSensitives = false ;
    const bool has_check_applicability = false ;
    const bool has_check_trigger = false ;

  
    void declareProperties( dd4hep::sim::Geant4Action* plugin ) {

      model.declareProperties( plugin ) ;
      inference.declareProperties( plugin ) ;
      geometry.declareProperties( plugin ) ;
    }
  };


}  // namespace

#endif // DDG4_GEANT4FASTSIMSHOWERMODEL_H
