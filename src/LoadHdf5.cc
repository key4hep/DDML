#include "DDML/LoadHdf5.h"
#include <DD4hep/Printout.h>
#include <cassert>

#define DEBUGPRINT 0

namespace ddml {

void LoadHdf5::declareProperties(dd4hep::sim::Geant4Action* plugin) {
  plugin->declareProperty("FilePath", this->m_filePath);
}

void LoadHdf5::initialize() {
  // Open file read-only
  dd4hep::printout(dd4hep::INFO, "LoadHdf5::initialize", "About to read HDF5 from path %s", m_filePath.c_str());
  m_file = H5::H5File(m_filePath.c_str(), H5F_ACC_RDONLY);
  dd4hep::printout(dd4hep::DEBUG, "LoadHdf5::initialize", "Read HDF5");

  // inputs and TensorDimVecs unused

  // Open dataset + dataspace
  std::string datasetName = "spase_points"; //"layers";
  H5::DataSet dataset = m_file.openDataSet(datasetName);
  dd4hep::printout(dd4hep::DEBUG, "LoadHdf5::initialize", "Accessed HDF5 dataset");
  H5::DataSpace dataspace = dataset.getSpace();

  // could check if dataspace is simple- add a test on this in the future?
  // bool H5::DataSpace::isSimple() const;

  // Get dimensions
  int rank = dataspace.getSimpleExtentNdims();
  std::vector<hsize_t> dims_out(rank);
  dataspace.getSimpleExtentDims(dims_out.data(), nullptr);

  // Calculate the total number of elements
  for (const auto& dim : dims_out) {
    m_totalSize *= dim;
  }

  m_dimsOut = dims_out;

//  assert(rank == 4); // assuming 4 dimensional input
  // index 0: shower number
  // index 1, 2, 3: x, y, z cell number

  assert(rank == 3); // assuming 3 dimensional PionCloud input
  // index 0: shower number
  // index 1: number of points
  // index 2: 4 point dimensions (x, y, z, E) - currently in ILD coordinates

  dd4hep::printout(dd4hep::DEBUG, "LoadHdf5::initialize", "Rank %i", rank);

  for (int i = 0, N = rank; i < N; ++i) {
    dd4hep::printout(dd4hep::DEBUG, "LoadHdf5::initialize", "dimension %i: %lu", i, (unsigned long)(m_dimsOut[i]));
  }

  dd4hep::printout(dd4hep::INFO, "LoadHdf5::initialize", "Recorded Total size %lu", m_totalSize);

  // Read the dataset into a buffer
  m_library = std::vector<float>(m_totalSize);
  dd4hep::printout(dd4hep::INFO, "LoadHdf5::initialize", "Recorded N showers %lu", m_dimsOut[0]);

  dataset.read(m_library.data(), H5::PredType::NATIVE_FLOAT);

  // Make sure dataset and file are closed
  dataspace.close();
  dataset.close();
  m_file.close();
}

/// 'run inference' equates to loading showers from file
/// for loading showers, this should really take the file name string as input
void LoadHdf5::runInference(const InputVecs&, const TensorDimVecs&, std::vector<float>& output) {
  if (!m_isInitialized) {
    initialize();
    m_isInitialized = true;
  }

  // keep track of shower index in file
  // LoadHdf5::incrementCounter();

  // If counter exceeds number of showers in file, reset
  if (m_count > m_totalSize) {
    m_count = 0;
  }

  // select shower from library
  //std::vector<float> shower(m_library.begin() + m_count * m_dimsOut[1] * m_dimsOut[2] * m_dimsOut[3],
  //                          m_library.begin() + (m_count + 1) * m_dimsOut[1] * m_dimsOut[2] * m_dimsOut[3]);

  // select shower from library
  std::vector<float> shower(m_library.begin() + m_count * m_dimsOut[1] * m_dimsOut[2],
                            m_library.begin() + (m_count + 1) * m_dimsOut[1] * m_dimsOut[2]);

  // enforce length of shower
  //assert(shower.size() == m_dimsOut[1] * m_dimsOut[2] * m_dimsOut[3]);

  assert(shower.size() == m_dimsOut[1] * m_dimsOut[2]);

  // write output
  output = std::move(shower);

  ++m_count;
}

} // namespace ddml
