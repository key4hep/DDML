#include "DDML/ParallelWorldALLEGROCaloDiTModel.h"

#include <G4FastTrack.hh> // for G4FastTrack

#include "CLHEP/Random/RandFlat.h"  // for RandGauss
#include "CLHEP/Random/RandGauss.h" // for RandGauss

#define DEBUGPRINT 0

namespace ddml {

void ParallelWorldALLEGROCaloDiTModel::prepareInput(G4FastTrack const& aFastTrack, G4ThreeVector const& localDir,
                                             InputVecs& inputs, TensorDimVecs& tensDims, std::vector<float>& output) {
  tensDims = _tensDims;
  G4double energy = aFastTrack.GetPrimaryTrack()->GetKineticEnergy() / 1000; // MeV -> GeV
  G4ThreeVector position = aFastTrack.GetPrimaryTrack()->GetPosition();
  G4ThreeVector direction = aFastTrack.GetPrimaryTrack()->GetMomentumDirection();

  // the input for this model is [energy, phi, theta]

  if (inputs.size() != 1) {
    inputs.resize(1);
  }

  inputs[0].resize(_latentSize);

  inputs[0][0] = energy;
  inputs[0][1] = atan2(direction.y(), direction.x()); // phi- global direction in radians not degree
  inputs[0][2] = acos(direction.z());                 // theta- same
  // Geometry condition inputs for model
  // Geo conditions for CLD are: [0, 0, 0, 1, 0]
  inputs[0][3] = 0.;
  inputs[0][4] = 0.;
  inputs[0][5] = 0.;
  inputs[0][6] = 1.;
  inputs[0][7] = 0.;

  // inputs.resize(_latentSize);
  // inputs[0].resize(1);   // Energy
  // inputs[1].resize(1);   // Phi
  // inputs[2].resize(1);   // Theta

  // if( DEBUGPRINT )
  std::cout << "  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ " << std::endl;
  std::cout << " inputs   [energy, phi, theta] = " << inputs[0][0] << "," << inputs[0][1] << "," << inputs[0][2]
            << std::endl;
  // std::cout << "local theta angle" << localDir.z() << std::endl ;
  // std::cout << "local energy" << energy << std::endl ;
  // std::cout << "localDir" << localDir.x() <<"," << localDir.y() <<","<< localDir.z() << std::endl ;
  // std::cout << "direction" << direction.x() <<"," << direction.y() <<","<< direction.z() << std::endl ;
  // std::cout << "position" << position.x() <<"," << position.y() <<","<< position.z() << std::endl ;
  std::cout << "  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ " << std::endl;

  std::cout << "phi = " << atan2(localDir.y(), localDir.x()) / M_PI * 180.
            << "   theta : " << acos(localDir.z()) / M_PI * 180. << std::endl;

  // // inputs[0][0] =  energy/ CLHEP::GeV;
  // inputs[0][0] =  50;
  // inputs[1][0] =  0;
  // inputs[2][0] =  1.57;

  // ----  resize output vector
  int outputSize = _nCellsRho * _nCellsPhi * _nCellsZ;

  output.assign(outputSize, 0);
}

void ParallelWorldALLEGROCaloDiTModel::convertOutput(G4FastTrack const&, G4ThreeVector const& localDir,
                                              const std::vector<float>& output,
                                              std::vector<SpacePointVec>& spacepoints) {
  int nLayer = _nCellsZ; // number of layers is z dimension

  spacepoints.resize(nLayer);

  for (int l = 0; l < nLayer; ++l) {
    spacepoints[l].reserve(_nCellsRho * _nCellsPhi);
  }

  // Calculate rotation matrix along the particle momentum direction
  // It will rotate the shower axes to match the incoming particle direction
  // This currently does the positioning of showers in the global coordinate position.
  // Should factorise out the global positioning to a local to global in the geo implementation.

  // compute local incident angle
  double theta = acos(localDir.z());
  double phi = atan2(localDir.y(), localDir.x());

  G4RotationMatrix rotMatrix = G4RotationMatrix();
  double particleTheta = theta;
  double particlePhi = phi;

  rotMatrix.rotateZ(-particlePhi);
  rotMatrix.rotateY(-particleTheta);

  G4RotationMatrix rotMatrixInv = CLHEP::inverseOf(rotMatrix);

  G4ThreeVector check_rot = rotMatrix * G4ThreeVector(0, 0, 1);

  // check rotation by applying it to a known unit vector
  std::cout << "ParallelWorldALLEGROCaloDiTModel::convertOutput - check_rot = " << check_rot << std::endl;

  int iHit = 0;

  for (int i = 0; i < _nCellsRho; ++i) {
    double rho = (i + 0.5) * _cellSizeRho;

    for (int j = 0; j < _nCellsPhi; ++j) {
      double phiCell = (j + 0.5) * 2. * CLHEP::pi / _nCellsPhi;
      double x = rho * cos(phiCell);
      double y = rho * sin(phiCell);

      for (int l = 0; l < nLayer; ++l) {
        double z =  (l+1.5) * _cellSizeZ; // This is actually the separation between layers? First position is position of first layer//(l + 0.5) * _cellSizeZ; Note- 1.5 to place at the center of cell...

        if (output[iHit] > 0.) {
          G4ThreeVector local_cylindrical_spacepoint = rotMatrixInv * G4ThreeVector(x, y, z);

          ddml::SpacePoint sp(local_cylindrical_spacepoint.x(), local_cylindrical_spacepoint.y(),
                              local_cylindrical_spacepoint.z(),
                              output[iHit]/CLHEP::GeV, // model output is in fraction of initial E
                              0.);

          spacepoints[l].emplace_back(sp);
        }

        ++iHit;
      }
    }
  }
}

} // namespace ddml
