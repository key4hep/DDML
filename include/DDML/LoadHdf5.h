#ifndef LoadHdf5_H
#define LoadHdf5_H

#include "DDML/InferenceInterface.h"
#include <DDG4/Geant4Action.h>

#include <H5Cpp.h> // for HDF5

#include <atomic> // for threadsafety

#include <string> // for string
#include <vector> // for vector

namespace ddml {

/**Loading shower from HDF5 file - assuming a simple input and output vector
 * Could be refactored to use a different interface that allows passing of different arguments that weren't needed for
 * inference. Implementation needs to be adapted according to the exact file structure desired!
 * The runInference arguments InputVecs and TensorDimVecs are intended for model inference, and are therefore completely
 * ignored
 *  @author P.McKeown, CERN
 *  @date  Sep. 2024
 */

class LoadHdf5 : public InferenceInterface {
public:
  LoadHdf5() = default;

  ~LoadHdf5() = default;

  /// declare the proerties needed for the plugin
  void declareProperties(dd4hep::sim::Geant4Action* plugin);

  /// initialize the model - based on defined properties
  void initialize();

  /// run the inference model - based on input vector and resized outputvector
  void runInference(const InputVecs&, const TensorDimVecs&, std::vector<float>& output) override;

private:
  H5::H5File m_file = 0;

  // total file size
  hsize_t m_totalSize = 1;

  bool m_isInitialized = false;

  // shower library
  std::vector<float> m_library{};

  // shower library dimensions
  std::vector<unsigned long long> m_dimsOut{}; // This is a hot fix for older HDF5 versions!
  //std::vector<unsigned long> m_dimsOut{};

  // properties for plugin
  std::string m_filePath = {};

  // counter to keep track of location in file
  uint32_t m_count{0};
};

} // namespace ddml
#endif
