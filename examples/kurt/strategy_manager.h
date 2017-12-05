#pragma once

#include <sc2api/sc2_api.h>
#include "kurt.h"
#include "army_manager.h"
#include <map>
#include <string>

class GamePlan;

class StrategyManager{

public:
    StrategyManager(Kurt* kurt);

    void OnStep(const sc2::ObservationInterface* observation);

    // New unit is saved in vector Units. (Call this function whenever a new unit is created)
    void SaveOurUnits(const sc2::Unit* unit);

    /* Removes a dead unit from the correct list of saved units */
    void RemoveDeadUnit(const sc2::Unit* unit);

    /* Executes the next step in the current plan */
    void ExecuteSubplan();

    // Decides whether we should be in attack-mode, Defence-mode or Harrass-mode based on a comparison of our_cp and enemy_cp.
    void CalculateCombatMode();

    // Decides what units should be built.
    void SetBuildGoal();

private:
    Kurt* kurt;
    GamePlan* current_plan;

    /*
    Splits up observed enemy units into enemy_structures and enemy_units vectors. This saves the minimal
    amount of units that we know the enemy has, but the enemy might have more units in the fog of war.
    */
    void SaveSpottedEnemyUnits(const sc2::ObservationInterface* observation);

    /* Saves the minimal amount of units we know the enemy have */
    void SaveSpottedEnemyUnitsHelper(sc2::Units* new_units, sc2::Units* saved_units);

    //
    void SetGamePlan();

    //UNUSED FUNCTIONS
    // Returns true if Enemy structure is observed.
    bool FindEnemyStructure(const sc2::ObservationInterface* observation, const sc2::Unit*& enemy_unit);

    void CheckCombatStyle(const sc2::Unit* unit, std::map<std::string, sc2::Units> map);

};