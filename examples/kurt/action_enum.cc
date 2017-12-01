#include "action_enum.h"

bool operator<(ActiveAction const & lhs, ActiveAction const & rhs) {
    return lhs.time_left < rhs.time_left;
}
