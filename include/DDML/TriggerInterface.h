#ifndef TriggerInterface_H
#define TriggerInterface_H

#include "DDML/DDML.h"
#include <concepts>

class G4FastTrack;

namespace ddml {

/** The basic concept for ML model trigger
 *
 * @author P.McKeown, DESY
 * @date Sep. 2023
 */

template <typename T>
concept TriggerInterface = requires(const T t, const G4FastTrack& track) {
  { t.check_trigger(track) } -> std::convertible_to<bool>;
};

} // namespace ddml

#endif
