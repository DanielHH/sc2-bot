#pragma once

#include <map>
#include <vector>
#include "sc2api/sc2_api.h"
#include "action_enum.h"

struct UnitAmount {
    sc2::UNIT_TYPEID type;
    int amount;
};

class ActionRepr {
public:
    ActionRepr( std::vector<struct UnitAmount> const &,
                std::vector<struct UnitAmount> const &,
                std::vector<struct UnitAmount> const &,
                std::vector<struct UnitAmount> const &);

    std::vector<struct UnitAmount> required;
    std::vector<struct UnitAmount> consumed;
    std::vector<struct UnitAmount> borrowed;
    std::vector<struct UnitAmount> produced;

    double time_required;

    static std::map<ACTION, ActionRepr> values;
};
