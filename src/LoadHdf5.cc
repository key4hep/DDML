#include "DDML/LoadHdf5.h"
#include <DD4hep/Printout.h>
#include <H5Cpp.h>
#include <H5Lpublic.h>
#include <cassert>
#include <numeric>
#include <functional>
#include <iostream>

#define DEBUGPRINT 0

namespace ddml {

void LoadHdf5::declareProperties(dd4hep::sim::Geant4Action* plugin) {
  // allow file path and dataset names to be configured
  plugin->declareProperty("FilePath", this->m_filePath);
  plugin->declareProperty("MomentumDatasetName", this->m_momentumDatasetName);
}

void LoadHdf5::initialize() {
  // Open file read-only
  dd4hep::printout(dd4hep::INFO, "LoadHdf5::initialize",
                   "About to read HDF5 from path %s", m_filePath.c_str());
  m_file = H5::H5File(m_filePath.c_str(), H5F_ACC_RDONLY);
  dd4hep::printout(dd4hep::DEBUG, "LoadHdf5::initialize", "Opened HDF5 file");

  // --- Read "layers" dataset ---
  const std::string datasetName = "layers";
  H5::DataSet dataset = m_file.openDataSet(datasetName);
  dd4hep::printout(dd4hep::DEBUG, "LoadHdf5::initialize",
                   "Accessed HDF5 dataset: %s", datasetName.c_str());
  H5::DataSpace dataspace = dataset.getSpace();
  if (!dataspace.isSimple()) {
    dd4hep::printout(dd4hep::WARNING, "LoadHdf5::initialize",
                     "Dataset '%s' is not simple. Results may be unpredictable.",
                     datasetName.c_str());
  }
  int rank = dataspace.getSimpleExtentNdims();
  m_dimsOut.resize(rank);
  dataspace.getSimpleExtentDims(m_dimsOut.data(), nullptr);
  dd4hep::printout(dd4hep::DEBUG, "LoadHdf5::initialize",
                   "Dataset '%s' rank: %i", datasetName.c_str(), rank);
  for (int i = 0; i < rank; ++i) {
    dd4hep::printout(dd4hep::DEBUG, "LoadHdf5::initialize",
                     "  Dimension %i: %lu", i, static_cast<unsigned long>(m_dimsOut[i]));
  }
  m_numSamples = m_dimsOut[0];
  m_sampleSize = 1;
  for (size_t i = 1; i < m_dimsOut.size(); ++i) {
    m_sampleSize *= m_dimsOut[i];
  }
  m_totalSize = m_numSamples * m_sampleSize;
  dd4hep::printout(dd4hep::INFO, "LoadHdf5::initialize",
                   "Found %lu samples, each with %lu elements (total %lu)",
                   static_cast<unsigned long>(m_numSamples),
                   static_cast<unsigned long>(m_sampleSize),
                   static_cast<unsigned long>(m_totalSize));
  m_library.resize(m_totalSize);
  dataset.read(m_library.data(), H5::PredType::NATIVE_FLOAT);
  dataspace.close();
  dataset.close();

  // --- Read "mc_momentum" dataset ---
  dd4hep::printout(dd4hep::INFO, "LoadHdf5::initialize",
                   "About to read momentum dataset '%s'", m_momentumDatasetName.c_str());
  try {
    H5::DataSet momDataset = m_file.openDataSet(m_momentumDatasetName);
    H5::DataSpace momSpace = momDataset.getSpace();
    if (!momSpace.isSimple()) {
      dd4hep::printout(dd4hep::WARNING, "LoadHdf5::initialize",
                       "Momentum dataset '%s' is not simple.",
                       m_momentumDatasetName.c_str());
    }
    int momRank = momSpace.getSimpleExtentNdims();
    m_momDims.resize(momRank);
    momSpace.getSimpleExtentDims(m_momDims.data(), nullptr);
    dd4hep::printout(dd4hep::DEBUG, "LoadHdf5::initialize",
                     "Momentum dataset '%s' rank: %i", m_momentumDatasetName.c_str(), momRank);
    for (int i = 0; i < momRank; ++i) {
      dd4hep::printout(dd4hep::DEBUG, "LoadHdf5::initialize",
                       "  Momentum dim %i: %lu", i, static_cast<unsigned long>(m_momDims[i]));
    }
    if (m_momDims[0] != m_numSamples) {
      dd4hep::printout(dd4hep::WARNING, "LoadHdf5::initialize",
                       "Number of momentum samples (%lu) does not match layers samples (%lu)",
                       static_cast<unsigned long>(m_momDims[0]),
                       static_cast<unsigned long>(m_numSamples));
    }
    m_momSampleSize = 1;
    for (size_t i = 1; i < m_momDims.size(); ++i) {
      m_momSampleSize *= m_momDims[i];
    }
    size_t totalMomSize = m_numSamples * m_momSampleSize;
    m_momentumLibrary.resize(totalMomSize);
    momDataset.read(m_momentumLibrary.data(), H5::PredType::NATIVE_FLOAT);
    momSpace.close();
    momDataset.close();
  } catch (H5::Exception& e) {
    dd4hep::printout(dd4hep::ERROR, "LoadHdf5::initialize",
                     "Failed to open momentum dataset '%s': %s",
                     m_momentumDatasetName.c_str(), e.getDetailMsg().c_str());
    dd4hep::printout(dd4hep::INFO, "LoadHdf5::initialize",
                     "Available top-level objects (via H5Literate):");
    // ensure idx is initialized for iteration
    hsize_t idx = 0;
    H5Literate(m_file.getId(), H5_INDEX_NAME, H5_ITER_INC, &idx,
      [](hid_t /*group*/, const char* name, const H5L_info_t*, void*) -> herr_t {
        dd4hep::printout(dd4hep::INFO, "LoadHdf5::initialize",
                         "  - %s", name);
        return 0;
      }, nullptr);
  }

  // Close file
  m_file.close();
}

void LoadHdf5::runInference(const InputVecs& m_InputVecs,
                            const TensorDimVecs& /*unused*/,
                            std::vector<float>& output) {
  if (!m_isInitialized) {
    initialize();
    m_isInitialized = true;
  }

  // Wrap around if necessary
  if (m_count >= m_numSamples) {
    m_count = 0;
    std::cerr << "WARNING: Reached end of samples, resetting counter.\n";
  }

  // --- Compare mc_momentum ---
  float total_diff = 100.0;
  while (total_diff > 0.01) {
    size_t momOffset = m_count * m_momSampleSize;
    std::vector<float> mcMom(m_momentumLibrary.begin() + momOffset,
                             m_momentumLibrary.begin() + momOffset + m_momSampleSize);
    
    // Calculate momentum difference
    total_diff = 0.0;
    for (size_t i = 0; i < mcMom.size(); ++i) {
      total_diff += std::abs(mcMom[i] - m_InputVecs[i][0]);
    }
    
    if (total_diff > 0.01) {
      std::cout << "WARNING: Sample difference exceeds threshold!\n";
      std::cout << "skipping sample " << m_count << "\n";
      ++m_count;
    }
    else {
      std::cout << "Sample " << m_count << " is valid.\n";
      std::cout << "Total momentum difference: " << total_diff << "\n";
      std::cout << "Accepted.\n";
    }

    if (m_count >= m_numSamples) {
      std::cerr << "WARNING: Reached end of samples, resetting counter. No corecposding photon found.\n";
      m_count = 0;
      break;
    }
    
  }

  // Extract one layers sample
  size_t offset = m_count * m_sampleSize;
  std::vector<float> sample(m_library.begin() + offset,
                            m_library.begin() + offset + m_sampleSize);
  output = std::move(sample);

  
  

  ++m_count;
  std::cout << "CURRENT m_count IS: " << m_count << std::endl;
}

} // namespace ddml
