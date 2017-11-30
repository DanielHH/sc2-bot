#include "build_manager.h"

#include <vector>
#include "kurt.h"

#include "BPState.h"
#include "BPAction.h"
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
    TEST(for (const Unit *u : observation->GetUnits(Unit::Alliance::Self, [](Unit const& u) { return u.unit_type == UNIT_TYPEID::TERRAN_SCV; })) {
        std::cout << UnitTypeToName(u->unit_type) << ":";
        for (auto uo : u->orders) {
            std::cout << " " << AbilityTypeToName(uo.ability_id);
        }
        std::cout << std::endl;
    })
}

void BuildManager::OnGameStart(const ObservationInterface* observation) {
    // Set up build tree
    std::cout << (int)(Kurt::GetAbility(ABILITY_ID::BUILD_TECHLAB_STARPORT)->target) << std::endl;
    SetUpTechTree(observation);
    setup_finished = true;

    // Set some test goal
    BPState * goal = new BPState();
    goal->SetUnitAmount(UNIT_TYPEID::TERRAN_MARINE, 2);
    goal->SetUnitAmount(UNIT_TYPEID::TERRAN_MARAUDER, 2);
    goal->SetUnitAmount(UNIT_TYPEID::TERRAN_GHOST, 2);
    goal->SetUnitAmount(UNIT_TYPEID::TERRAN_BATTLECRUISER, 1);
    SetGoal(goal);
}

void BuildManager::GroupAndSaveUnits(const Unit* unit) {
    if (unit->unit_type.ToType() == UNIT_TYPEID::TERRAN_SCV) {
        if (! agent->UnitInScvMinerals(unit)) {
            agent->scv_minerals.push_back(unit);
        }
    } else {
        // Maybe add to workers list?
    }
}

void BuildManager::SetGoal(BPState * const goal_) {
    goal = goal_;
    
}

void BuildManager::InitNewPlan(const ObservationInterface* observation) {
    BPState * current_state = new BPState(observation, agent);
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

#undef DEBUG
#undef PRINT
#undef TEST
