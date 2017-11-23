#include "build_manager.h"

#include <iostream>
#include <vector>
#include "kurt.h"

#include "BPState.h"
#include "BPAction.h"
#include "BPPlan.h"
#include "MCTS.h"

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
    if (data->tech_requirement != UNIT_TYPEID::INVALID) {
        requirements.push_back(data->tech_requirement);
    }
    else if (tech_tree_2.count(unit) > 0) {
        for (UNIT_TYPEID req_elem : tech_tree_2[unit]) {
            requirements.push_back(req_elem);
        }
    }
    return requirements;
}

void BuildManager::OnStep(const ObservationInterface* observation) {
    if (current_plan.empty() && goal != nullptr) {
        InitNewPlan(observation);
        if (current_plan.empty()) {
            goal = nullptr;
            std::cout << "goal is reached" << std::endl;
            // Goal is reached, need a better goal checker
            // when multiple goals can be active at the same time.
        }
    }
    current_plan.ExecuteStep(agent);

    // TESTING
    for (const Unit *u : observation->GetUnits(Unit::Alliance::Self, [](Unit const& u) { return u.unit_type == UNIT_TYPEID::TERRAN_SCV; })) {
        std::cout << UnitTypeToName(u->unit_type) << ":";
        for (auto uo : u->orders) {
            std::cout << " " << AbilityTypeToName(uo.ability_id);
        }
        std::cout << std::endl;
    }
}

void BuildManager::OnGameStart(const ObservationInterface* observation) {
    // Set up build tree
    SetUpTechTree(observation);
    setup_finished = true;

    // Set some test goal
    BPState * goal = new BPState();
    goal->SetUnitAmount(UNIT_TYPEID::TERRAN_BATTLECRUISER, 2);
    SetGoal(goal);
}

void BuildManager::SetGoal(BPState * const goal_) {
    goal = goal_;
}

void BuildManager::InitNewPlan(const ObservationInterface* observation) {
    BPState * current_state = new BPState(observation);
    current_plan.AddBasicPlan(current_state, goal);

    std::cout << "--- Creating new plan ---" << std::endl;
    std::cout << "Current state:" << std::endl;
    current_state->Print();
    std::cout << "Goal state:" << std::endl;
    goal->Print();
    std::cout << "Current plan:" << std::endl;
    std::cout << current_plan << std::endl;

    delete current_state;
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
