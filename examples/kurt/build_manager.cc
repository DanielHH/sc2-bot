#include "build_manager.h"

#include <vector>

#include "kurt.h"
#include "constants.h"
#include "action_repr.h"
#include "action_enum.h"
#include "exec_action.h"
#include "BPState.h"
#include "BPPlan.h"
#include "MCTS.h"

//#define DEBUG // Comment out to disable debug prints in this file.
#ifdef DEBUG
#include <iostream>
#define PRINT(s) std::cout << s << std::endl;
#define TEST(s) s
#else
#define PRINT(s)
#define TEST(s)
#endif // DEBUG

using namespace sc2;

std::map<sc2::UNIT_TYPEID, std::vector<sc2::UNIT_TYPEID> > BuildManager::tech_tree_2;
bool BuildManager::setup_finished = false;

BuildManager::BuildManager(Kurt *const agent_) : agent(agent_) {
    
}

std::vector<UnitTypeData*> BuildManager::GetRequirements(UnitTypeData* unit) {
    assert(setup_finished);
    std::vector<UnitTypeData*> requirements;
    UNIT_TYPEID type = unit->unit_type_id.ToType();
    if (unit->tech_requirement != UNIT_TYPEID::INVALID) {
        requirements.push_back(Kurt::GetUnitType(unit->tech_requirement));
    } else if (tech_tree_2.count(type) > 0) {
        for (UNIT_TYPEID req_elem : tech_tree_2[type]) {
            requirements.push_back(Kurt::GetUnitType(req_elem));
        }
    }
    return requirements;
}

std::vector<UNIT_TYPEID> BuildManager::GetRequirements(UNIT_TYPEID unit) {
    assert(setup_finished);
    std::vector<UNIT_TYPEID> requirements;
    UnitTypeData *data = Kurt::GetUnitType(unit);
    if (tech_tree_2.count(unit) > 0) {
        for (UNIT_TYPEID req_elem : tech_tree_2[unit]) {
            requirements.push_back(req_elem);
        }
    } else if (data->tech_requirement != UNIT_TYPEID::INVALID) {
        requirements.push_back(data->tech_requirement);
    }
    return requirements;
}

void TestMCTS(BPState * const start, BPState * const goal) {
    std::cout << "----------------------------" << std::endl;

    MCTS mcts(start, goal);
    mcts.Search(5000);
    BPPlan best_plan = mcts.BestPlan();
    BPState tmp1(start);
    tmp1.SimulatePlan(best_plan);
    double best_time = tmp1.GetTime() - start->GetTime();
    double best_mineral_rate = tmp1.GetMineralRate();
    double best_vespene_rate = tmp1.GetVespeneRate();

    BPPlan basic_plan;
    basic_plan.AddBasicPlan(start, goal);
    BPState tmp2(start);
    tmp2.SimulatePlan(basic_plan);
    double basic_time = tmp2.GetTime() - start->GetTime();
    double basic_mineral_rate = tmp2.GetMineralRate();
    double basic_vespene_rate = tmp2.GetVespeneRate();

    std::cout << best_plan << std::endl;
    std::cout << "vespene rate basic:  " << basic_vespene_rate << std::endl;
    std::cout << "vespene rate best: " << best_vespene_rate << std::endl;
    std::cout << "mineral rate basic:  " << basic_mineral_rate << std::endl;
    std::cout << "mineral rate best: " << best_mineral_rate << std::endl;
    std::cout << "time basic:  " << basic_time << std::endl;
    std::cout << "time best: " << best_time << std::endl;
    std::cout << "----------------------------" << std::endl;
}

void BuildManager::OnStep(const ObservationInterface* observation) {
    // If there is no goal in life, what is the point of living?
    if (goal == nullptr) {
        return;
    }
    BPState current_state(agent);
    //
    // Do the searching for a better plan.
    //
    if (steps_until_replan <= -1) {
        if (mcts != nullptr) {
            delete mcts;
            mcts = nullptr;
        }
    }
    if (mcts == nullptr) {
        mcts = new MCTS(&current_state, goal);
    }
    mcts->Search(SEARCH_ITER_PER_STEP);
    if (steps_until_replan <= 0) {
        current_plan = mcts->BestPlan();
        delete mcts;
        mcts = nullptr;
        steps_until_replan = STEPS_BETWEEN_REPLAN;

        PRINT("--- Created new plan ---")
        TEST(current_state.Print();)
        PRINT(current_plan)
    }
    --steps_until_replan;
    //
    // Execute the current plan
    //
    current_plan.ExecuteStep(agent);
    //
    // Test if a goal is reached
    //
    if (current_state.ContainsAllUnitsOf(goal)) {
        goal = nullptr;
        PRINT("--- Goal is reached, gametime: " << current_state.GetTime() << " ---")
        agent->ExecuteSubplan();
    }

    //
    // TESTING
    //
    /*
    BPState * curr = new BPState(agent);

    BPState * goal = new BPState();
    goal->SetUnitAmount(UNIT_TYPEID::TERRAN_GHOST, 9);
    TestMCTS(curr, goal);
    goal->SetUnitAmount(UNIT_TYPEID::TERRAN_GHOST, 0);
    goal->SetUnitAmount(UNIT_TYPEID::TERRAN_BATTLECRUISER, 5);
    TestMCTS(curr, goal);
    delete curr;

    throw std::runtime_error("hehehe");
    */
}

void BuildManager::OnGameStart(const ObservationInterface* observation) {
    SetUpTechTree(observation);
    ActionRepr::InitConvertMap();
    ExecAction::Init(agent);
    setup_finished = true;
}

void BuildManager::SetGoal(BPState * const goal_) {
    if (goal != nullptr) {
        delete goal;
    }
    goal = goal_;
    BPState curr(agent);
    for (auto it = goal->UnitsBegin(); it != goal->UnitsEnd(); ++it) {
        goal->IncreaseUnitAmount(it->first, curr.GetUnitAmount(it->first));
    }
    InitNewPlan();
}

void BuildManager::InitNewPlan() {
    steps_until_replan = -1;
}

void BuildManager::SetUpTechTree(const ObservationInterface* observation) {
    // Barrack upgrades
    tech_tree_2[UNIT_TYPEID::TERRAN_BARRACKSTECHLAB].push_back(UNIT_TYPEID::TERRAN_BARRACKS);
    tech_tree_2[UNIT_TYPEID::TERRAN_BARRACKSREACTOR].push_back(UNIT_TYPEID::TERRAN_BARRACKS);

    // Produced at barracks
    tech_tree_2[UNIT_TYPEID::TERRAN_MARINE].push_back(UNIT_TYPEID::TERRAN_BARRACKS);
    tech_tree_2[UNIT_TYPEID::TERRAN_MARAUDER].push_back(UNIT_TYPEID::TERRAN_BARRACKSTECHLAB);
    tech_tree_2[UNIT_TYPEID::TERRAN_REAPER].push_back(UNIT_TYPEID::TERRAN_BARRACKS);
    tech_tree_2[UNIT_TYPEID::TERRAN_GHOST].push_back(UNIT_TYPEID::TERRAN_BARRACKSTECHLAB);
    tech_tree_2[UNIT_TYPEID::TERRAN_GHOST].push_back(UNIT_TYPEID::TERRAN_GHOSTACADEMY);

    // Starport upgrade
    tech_tree_2[UNIT_TYPEID::TERRAN_STARPORTTECHLAB].push_back(UNIT_TYPEID::TERRAN_STARPORT);
    tech_tree_2[UNIT_TYPEID::TERRAN_STARPORTREACTOR].push_back(UNIT_TYPEID::TERRAN_STARPORT);

    // Produced at factory
    tech_tree_2[UNIT_TYPEID::TERRAN_MEDIVAC].push_back(UNIT_TYPEID::TERRAN_STARPORT);
    tech_tree_2[UNIT_TYPEID::TERRAN_BANSHEE].push_back(UNIT_TYPEID::TERRAN_STARPORTTECHLAB);
    tech_tree_2[UNIT_TYPEID::TERRAN_BATTLECRUISER].push_back(UNIT_TYPEID::TERRAN_STARPORTTECHLAB);
    tech_tree_2[UNIT_TYPEID::TERRAN_BATTLECRUISER].push_back(UNIT_TYPEID::TERRAN_FUSIONCORE);

    // TODO Add more data to tech_tree_2
}

#undef DEBUG
#undef PRINT
#undef TEST
