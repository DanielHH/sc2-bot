#pragma once

#include <map>
#include <vector>
#include "sc2api/sc2_api.h"
#include "action_enum.h"

class ActionRepr {
public:
    

    std::map<sc2::UNIT_TYPEID, int> const required;
    std::map<sc2::UNIT_TYPEID, int> const consumed;
    std::map<sc2::UNIT_TYPEID, int> const borrowed;
    std::map<sc2::UNIT_TYPEID, int> const produced;

    double time_required;

    static std::map<ACTION, ActionRepr> values;

    static std::map<ACTION, sc2::ABILITY_ID> convert_our_api;
    static std::map<sc2::ABILITY_ID, ACTION> convert_api_our;
    static void InitConvertMap();

    static ACTION CreatesUnit(sc2::UNIT_TYPEID unit);

private:
    ActionRepr(std::map<sc2::UNIT_TYPEID, int> const &,
        std::map<sc2::UNIT_TYPEID, int> const &,
        std::map<sc2::UNIT_TYPEID, int> const &,
        std::map<sc2::UNIT_TYPEID, int> const &,
        float time);
};
