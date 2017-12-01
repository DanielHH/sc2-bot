#include "action_enum.h"

#include "action_repr.h"

ActiveAction::ActiveAction(ACTION action_) : action(action_) {
    time_left = ActionRepr::values.at(action_).time_required;
}

bool operator<(ActiveAction const & lhs, ActiveAction const & rhs) {
    return lhs.time_left < rhs.time_left;
}
