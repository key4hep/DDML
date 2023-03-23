#ifndef DDML_H
#define DDML_H


#include <vector>

/** Some basic helper classes and utilities for DDML.
 *
 *  @author F.Gaede, DESY
 *  @date Mar 2023
 */

namespace ddml {

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
