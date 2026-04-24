#ifndef FastMLShower_H
#define FastMLShower_H

#include <DDG4/Geant4FastSimShowerModel.h>

#include <G4FastHit.hh>
#include <G4FastStep.hh>
#include <G4FastTrack.hh>
#include <G4Track.hh>

#include "DDML/DDML.h"
#include "DDML/Geant4FastHitMakerGlobal.h"
#include "DDML/GeometryInterface.h"
#include "DDML/InferenceInterface.h"
#include "DDML/ModelInterface.h"
#include "DDML/TriggerInterface.h"

#ifndef DDML_INSTRUMENT_MODEL_SHOWER
#define DDML_INSTRUMENT_MODEL_SHOWER 0
#endif

#if DDML_INSTRUMENT_MODEL_SHOWER
#include "podio/Frame.h"
#include "podio/ROOTWriter.h"
#include "podio/UserDataCollection.h"

#include <chrono>
#include <functional>
#include <memory>
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
class FastMLShower : public dd4hep::sim::Geant4FastSimShowerModel {
protected:
  ML_MODEL m_fastsimML;

  InputVecs m_input;
  TensorDimVecs m_dimVecs;
  std::vector<float> m_output;
  std::vector<ddml::SpacePointVec> m_spacepoints;

#if DDML_INSTRUMENT_MODEL_SHOWER
  podio::ROOTWriter m_timeWriter;
#endif

public:
  /** C'tor that calls initialize of the concrete model implementation in order
   * to allow for dedicated properties to be declared.
   */
  FastMLShower(dd4hep::sim::Geant4Context* context, const std::string& nam)
      : Geant4FastSimShowerModel(context, nam)
#if DDML_INSTRUMENT_MODEL_SHOWER
        ,
        m_timeWriter("times_" + nam + ".root")
#endif
  {

    m_fastsimML.declareProperties(this);
  }

  /// Default destructor
  virtual ~FastMLShower() {
#if DDML_INSTRUMENT_MODEL_SHOWER
    m_timeWriter.finish();
#endif
  }

  /// Geometry construction callback. Called at "Construct()"
  virtual void constructGeo(dd4hep::sim::Geant4DetectorConstructionContext* ctxt) override {
    this->Geant4FastSimShowerModel::constructGeo(ctxt);
  }

  /// Electromagnetic field construction callback. Called at
  /// "ConstructSDandField()"
  virtual void constructField(dd4hep::sim::Geant4DetectorConstructionContext* ctxt) override {
    this->Geant4FastSimShowerModel::constructField(ctxt);
  }

  /// Sensitive detector construction callback. Called at
  /// "ConstructSDandField()"
  virtual void constructSensitives(dd4hep::sim::Geant4DetectorConstructionContext* ctxt) override {
    this->Geant4FastSimShowerModel::constructSensitives(ctxt);
    m_fastsimML.initialize();
  }

  /// User callback to determine if the model is applicable for the particle
  /// type
  /** Default implementation checks if the particle is registered in
   * 'ApplicableParticles'
   */
  virtual bool check_applicability(const G4ParticleDefinition& particle) override {
    return this->Geant4FastSimShowerModel::check_applicability(particle);
  }

  /// User callback to determine if the shower creation should be triggered
  /** Default implementation checks if for all particles registered in
   * 'Etrigger' the kinetic energy is bigger than the value.
   */
  virtual bool check_trigger(const G4FastTrack& track) override {
    if (this->Geant4FastSimShowerModel::check_trigger(track)) {
      return m_fastsimML.trigger.check_trigger(track);
    }
    return false;
  }

  /// User callback to model the particle/energy shower - details defined in
  /// ML_MODEL
  virtual void modelShower(const G4FastTrack& track, G4FastStep& step) override {
    G4double energy = track.GetPrimaryTrack()->GetKineticEnergy();
    // remove particle from further processing by G4
    killParticle(step, energy);

#if DDML_INSTRUMENT_MODEL_SHOWER
    podio::UserDataCollection<double> prepareInputTime;
    podio::UserDataCollection<double> runInferenceTime;
    podio::UserDataCollection<double> convertOutputTime;
    podio::UserDataCollection<double> localToGlobalTime;
    podio::UserDataCollection<double> hitMakerTime;
    podio::UserDataCollection<uint64_t> nHits;
#endif

    for (auto& invec : m_input) {
      invec.clear();
    }

    m_output.clear();
    for (auto& layerSPs : m_spacepoints) {
      layerSPs.clear();
    }

    G4ThreeVector localDir = m_fastsimML.geometry.localDirection(track);

#if DDML_INSTRUMENT_MODEL_SHOWER
    prepareInputTime.push_back(run_void_member_timed(m_fastsimML.model, &ML_MODEL::MLModelT::prepareInput, track,
                                                     localDir, m_input, m_dimVecs, m_output)
                                   .count());
    runInferenceTime.push_back(
        run_void_member_timed(m_fastsimML.inference, &ML_MODEL::InferenceT::runInference, m_input, m_dimVecs, m_output)
            .count());
    convertOutputTime.push_back(run_void_member_timed(m_fastsimML.model, &ML_MODEL::MLModelT::convertOutput, track,
                                                      localDir, m_output, m_spacepoints)
                                    .count());
    localToGlobalTime.push_back(
        run_void_member_timed(m_fastsimML.geometry, &ML_MODEL::GeometryT::localToGlobal, track, m_spacepoints).count());

    nHits.push_back(std::accumulate(m_output.begin(), m_output.end(), 0u,
                                    [](const auto sum, const auto v) { return sum + (v != 0); }));
#else
    m_fastsimML.model.prepareInput(track, localDir, m_input, m_dimVecs, m_output);
    m_fastsimML.inference.runInference(m_input, m_dimVecs, m_output);
    m_fastsimML.model.convertOutput(track, localDir, m_output, m_spacepoints);
    m_fastsimML.geometry.localToGlobal(track, m_spacepoints);
#endif
    // now deposit energies in the detector using calculated global positions
#if DDML_INSTRUMENT_MODEL_SHOWER
    const auto start = ClockT::now();
#endif
    for (auto& layerSPs : m_spacepoints) {
      for (auto& sp : layerSPs) {
        m_fastsimML.hitMaker->make(G4FastHit(G4ThreeVector(sp.X, sp.Y, sp.Z), sp.E), track);
      }
    }

#if DDML_INSTRUMENT_MODEL_SHOWER
    const auto end = ClockT::now();
    hitMakerTime.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());

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

struct AlwaysTrueTrigger {
  bool check_trigger(const G4FastTrack&) const { return true; }
};
static_assert(TriggerInterface<AlwaysTrueTrigger>, "AlwaysTrueTrigger should model the TriggerInterface concept");

/** Template class to put together a complete ML model by specifying
 * implementations for the Inference, the Model, the Geometry and a HitMaker.
 *
 * @author F.Gaede, DESY
 * @date Mar 2023
 */
template <InferenceInterface Inference, ModelInterface MLModel, GeometryInterface Geometry,
          TriggerInterface Trigger = AlwaysTrueTrigger>
struct FastMLModel {
  using InferenceT = Inference;
  using MLModelT = MLModel;
  using GeometryT = Geometry;

  Inference inference = {};
  MLModel model = {};
  Geometry geometry = {};
  // NOTE: We leak this on purpose for now because some ref-counting inside the
  // Geant4FastHitMakerGlobal goes wrong and segfaults during cleanup
  Geant4FastHitMakerGlobal* hitMaker = {};
  Trigger trigger{};

  FastMLModel() : hitMaker(new Geant4FastHitMakerGlobal()) {}

  void initialize() { inference.initialize(); }

  void declareProperties(dd4hep::sim::Geant4Action* plugin) {
    model.declareProperties(plugin);
    inference.declareProperties(plugin);
    geometry.declareProperties(plugin);
  }
};

} // namespace ddml

#endif // DDG4_GEANT4FASTSIMSHOWERMODEL_H
