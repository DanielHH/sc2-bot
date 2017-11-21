#pragma once

#include <sc2api/sc2_api.h>
#include <map>
#include <string>


class StrategyManager{
public:

    std::string combat_mode;

    struct CombatPower {
        std::string alliance;
        float g2g;
        float g2a;
        float a2g;
        float a2a;
    } our_cp, enemy_cp;

    StrategyManager();

    void OnStep(const sc2::ObservationInterface* observation);

    // New unit is saved in vector Units. (Call this function whenever a new unit is created)
    void SaveOurUnits(const sc2::Unit* unit);

private:
    //Save enemy units
    void SaveSpottedEnemyUnits(const sc2::ObservationInterface* observation);

    void CheckCombatStyle(const sc2::Unit* unit, std::map<std::string, sc2::Units> map);

    // calculates combatpower for given combatstyle
    void  CalculateCombatPower(CombatPower *cp);

    // Decides whether we should be in attack-mode, Defence-mode or Harrass-mode based on a comparison of our_cp and enemy_cp.
    void ChooseCombatMode();

    void SetCombatMode(std::string new_mode);

    std::string GetCombatMode();

    // Returns true if Enemy structure is observed.
    bool FindEnemyStructure(const sc2::ObservationInterface* observation, const sc2::Unit*& enemy_unit);


    // Decides what units should be built.
    void DecideBuildGoal();

};