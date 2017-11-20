#pragma once

#include <sc2api/sc2_api.h>
#include <map>
#include <string>


class StrategyManager{
public:

    struct CombatPower {
        float g2g;
        float g2a;
        float a2g;
        float a2a;
    };

    CombatPower our_cp;
    CombatPower enemy_cp;

    StrategyManager();

    void OnStep(const sc2::ObservationInterface* observation);

    void SortOurUnits(const sc2::Unit* unit);

private:
    //Save enemy units
    void SaveSpottedEnemyUnits(const sc2::ObservationInterface* observation);

    void CheckCombatStyle(const sc2::Unit* unit, std::map<std::string, sc2::Units> map);

    // calculates combatpower for given combatstyle
    void  CalculateCombatPower(CombatPower *cp);

    void ChooseCombatMode();

    // Returns true if Enemy structure is observed. Med ObservationInterface ser man väl allt?
    bool FindEnemyStructure(const sc2::ObservationInterface* observation, const sc2::Unit*& enemy_unit);

};