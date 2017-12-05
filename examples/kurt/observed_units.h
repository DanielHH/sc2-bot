#pragma once

#include <sc2api\sc2_api.h>
#include <map>
#include <string>

class ObservedUnits {

public:
    ObservedUnits();

    static std::map <sc2::UNIT_TYPEID, float> unit_max_health;

    /* Takes a Units (vector) and adds units if more than prevoius are observed */
    void AddUnits(const sc2::Units* units);

    /* Adds a single unit */
    void AddUnit(const sc2::Unit* unit);

    /* Removes a unit from the observations */
    void RemoveUnit(const sc2::Unit* unit);

    /* Returns a string that prints type and amount of observed units */
    std::string ToString();

    /* Returns a float which represents the max health a group of units of a certain unittype can have.*/
    float CalculateUnitTypeMaxHealth(sc2::UNIT_TYPEID unit_type);

    std::map <sc2::UNIT_TYPEID, int> *const GetSavedUnits();

private:
    std::map <sc2::UNIT_TYPEID, int> saved_units;
};