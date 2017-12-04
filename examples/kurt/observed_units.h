#pragma once

#include <sc2api\sc2_api.h>
#include <map>
#include <string>

class ObservedUnits {

public:
    ObservedUnits();

    void AddUnits(const sc2::Units* units);

    void AddUnit(const sc2::Unit* unit);

    void RemoveUnit(const sc2::Unit* unit);

    std::string ToString();

private:
    std::map <sc2::UNIT_TYPEID, int> saved_units;
};