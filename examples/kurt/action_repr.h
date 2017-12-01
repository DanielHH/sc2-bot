#pragma once

#include <map>
#include <vector>
#include "sc2api/sc2_api.h"
#include "action_enum.h"

class ActionRepr {
public:
    

    std::map<sc2::UNIT_TYPEID, int> required;
    std::map<sc2::UNIT_TYPEID, int> consumed;
    std::map<sc2::UNIT_TYPEID, int> borrowed;
    std::map<sc2::UNIT_TYPEID, int> produced;

    double time_required;

    static std::map<ACTION, ActionRepr> values;

private:
    ActionRepr(std::map<sc2::UNIT_TYPEID, int> const &,
        std::map<sc2::UNIT_TYPEID, int> const &,
        std::map<sc2::UNIT_TYPEID, int> const &,
        std::map<sc2::UNIT_TYPEID, int> const &,
        float time);
};
