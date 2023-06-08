#ifndef DDML_H
#define DDML_H


#include <vector>
#include <cstdint>

/** Some basic helper classes and utilities for DDML.
 *
 *  @author F.Gaede, DESY
 *  @date Mar 2023
 */

namespace ddml {

  /// vector of input vectors
  typedef std::vector< std::vector< float > > InputVecs ;

  /// vector of vectors with input tensor dimensions
  typedef std::vector< std::vector<int64_t> > TensorDimVecs ;


/** The basic space point structure for fast ML showers
 *  with cartesian position, energy and time;
 */
  struct SpacePoint {
    SpacePoint(float x, float y, float z, float e, float t) :
      X(x),Y(y),Z(z),E(e),T(t) {}
    float X=0 ;
    float Y=0 ;
    float Z=0 ;
    float E=0 ;
    float T=0 ;
  };


  typedef std::vector<SpacePoint> SpacePointVec;

} // namespace

#endif
