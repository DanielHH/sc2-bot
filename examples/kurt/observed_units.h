#pragma once

#include <sc2api/sc2_api.h>
#include <map>
#include <string>
#include <vector>

class ObservedUnits {

public:
    static const std::vector<sc2::UNIT_TYPEID> flying_units; // A vector of the units that can fly

    /* Stores the total DPS all units has together against air and ground units  */
    struct CombatPower {
        float g2g; // Ground to ground DPS
        float g2a; // Ground to air DPS
        float a2g; // Air to ground DPS
        float a2a; // Air to air DPS
    };

    ObservedUnits();

    static std::map <sc2::UNIT_TYPEID, float> unit_max_health;

    /* Takes a Units (vector) and adds units if more than prevoius are observed */
    void AddUnits(const sc2::Units* units);

    /* Adds any amount of a single type of unit, with a default of 1 */
    void AddUnits(const sc2::UNIT_TYPEID unit_type, const int amount = 1);

    /* Adds any amount of a single type of unit, with a default of 1 */
    void AddUnits(const sc2::Unit* unit, const int amount = 1);

    /* Removes a unit from the observations */
    void RemoveUnit(const sc2::Unit* unit);

    /* Calculates the combat power (DPS in each category) for all units in saved_units */
    void calculateCP();

    /* Returns the current combatpower for the saved units */
    const CombatPower* const GetCombatPower();

    /* Returns the sum of all units' max health */
    float GetTotalMaxHealth();

    /* Returns a float which represents the max health a group of units of a certain unittype can have. */
    float CalculateUnitTypeMaxHealth(sc2::UNIT_TYPEID unit_type);

    std::map <sc2::UNIT_TYPEID, int> *const GetSavedUnits();

    ObservedUnits* GetStrongestUnit(ObservedUnits enemies);

    ObservedUnits* GetBestCounterUnit();

    /* Returns a string that prints type and amount of observed units */
    std::string ToString();

private:
    std::map <sc2::UNIT_TYPEID, int> saved_units;
    CombatPower cp;
};

