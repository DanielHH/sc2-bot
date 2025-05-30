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
    static bool dynamic_flag;

    void OnStep(const sc2::ObservationInterface* observation);

    void OnUnitEnterVision(const sc2::Unit* unit);

    static sc2::UNIT_TYPEID current_best_counter_type;

    // New unit is saved in vector Units. (Call this function whenever a new unit is created)
    void SaveOurUnits(const sc2::Unit* unit);

    /* Removes a dead unit from the correct list of saved units */
    void RemoveDeadUnit(const sc2::Unit* unit);

    /* Executes the next step in the current plan */
    void ExecuteSubplan();

    /* Calculates a new plan to execute */
    void CalculateNewPlan();

    void UpdateCurrentBestCounterType();

    // Decides whether we should be in attack-mode, Defence-mode or Harrass-mode based on a comparison of our_cp and enemy_cp.
    void CalculateCombatMode();

    // Decides what units should be built.
    void SetBuildGoal();

    void AddToBuildGoal();

    void StuffWeLikeToHave();

    void SetProgressionMode(bool new_progression_mode);

    bool GetProgressionMode();

private:
    Kurt* kurt;
    GamePlan* current_plan;
    bool progression_mode;

    /*
    Splits up observed enemy units into enemy_structures and enemy_units vectors. This saves the minimal
    amount of units that we know the enemy has, but the enemy might have more units in the fog of war.
    */
    void SaveSpottedEnemyUnits(const sc2::ObservationInterface* observation);
};