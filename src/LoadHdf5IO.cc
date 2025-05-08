#include "DDML/LoadHdf5IO.h"

#include <G4FastTrack.hh>
#include <CLHEP/Units/PhysicalConstants.h>  // for GeV

namespace ddml {

void LoadHdf5IO::prepareInput(G4FastTrack const& aFastTrack,
                              G4ThreeVector const& localDir,
                              InputVecs&         inputs,
                              TensorDimVecs&     tensDims,
                              std::vector<float>& output) {

  // keep same tensor dims as CaloCloudsTwoAngleModel
  tensDims = m_tensDims;

  // pull true MC momentum (in GeV)
  auto mom = aFastTrack.GetPrimaryTrack()->GetMomentum();
  float px = mom.x() / CLHEP::GeV;
  float py = mom.y() / CLHEP::GeV;
  float pz = mom.z() / CLHEP::GeV;

  dd4hep::printout(dd4hep::DEBUG,
                   "LoadHdf5IO::prepareInput",
                   "MC momentum = (%f, %f, %f) GeV",
                   px, py, pz);

  // (energy, theta, phi) -> (px, py, pz)
  // (px, py, pz) are used as a primary key for the HDF5 dataset
  // to find the corresponding shower
  inputs.resize(3);
  for (auto& v : inputs) v.resize(1);
  inputs[0][0] = px;
  inputs[1][0] = py;
  inputs[2][0] = pz;

  // reserve space for the output tensor
  output.assign(m_maxNumElements, 0.f);
}

} // namespace ddml
