#pragma once

#include "constants.h"
#include "BPPlan.h"

#include <sc2api/sc2_api.h>
#include <map>
#include <vector>

class BPState;
class MCTS;
class Kurt;

class BuildManager {
    friend class Kurt;
public:
    BuildManager(Kurt *const);
    static std::vector<sc2::UnitTypeData*> GetRequirements(sc2::UnitTypeData* unit);
    static std::vector<sc2::UNIT_TYPEID> GetRequirements(sc2::UNIT_TYPEID);
    void OnStep(const sc2::ObservationInterface* observation);
    void OnGameStart(const sc2::ObservationInterface* observation);
    /* Removes all potential other goals and set given goal as the goal */
    void SetGoal(BPState * const);

    /* Called when a major change
     * (like a new goal or loosing a building) happens
     */
    void InitNewPlan();

private:
    static std::map<sc2::UNIT_TYPEID, std::vector<sc2::UNIT_TYPEID> > tech_tree_2;
    static std::map<sc2::UNIT_TYPEID, sc2::UnitTypeData> unit_types;
    static void SetUpTechTree(const sc2::ObservationInterface* observation);
    static bool setup_finished;

    BPState * goal = nullptr;
    BPPlan current_plan;
    MCTS * mcts = nullptr;
    int steps_until_replan = -1;
    int old_steps_until_replan = -1;

    Kurt *const agent;

    const int STEPS_BETWEEN_REPLAN_MAX = STEPS_PER_SEC * 4;
    const int STEPS_BETWEEN_REPLAN_MIN = 2;
    const int SEARCH_ITER_PER_STEP = 8;
};
