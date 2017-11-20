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

BuildManager::BuildManager() {
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
    // DO ALL DE ARMY STUFF
}

void BuildManager::OnGameStart(const ObservationInterface* observation) {
    // Set up build tree
    SetUpTechTree(observation);
    setup_finished = true;

    // Testing som functions...
    {
        UNIT_TYPEID bar = UNIT_TYPEID::TERRAN_BARRACKS;
        UnitTypeData* barData = Kurt::GetUnitType(bar);
        std::cout << barData->name << " require: ";
        for (UnitTypeData* r : GetRequirements(barData)) {
            std::cout << r->name << " ";
        }
        std::cout << std::endl;
    }
    {
        UNIT_TYPEID bar = UNIT_TYPEID::TERRAN_GHOST;
        UnitTypeData* barData = Kurt::GetUnitType(bar);
        std::cout << barData->name << " require: ";
        for (UnitTypeData* r : GetRequirements(barData)) {
            std::cout << r->name << " ";
        }
        std::cout << std::endl;
    }
    std::cout << ">>> plan:" << std::endl;
    BPState * curr = new BPState();
    curr->SetUnitAmount(UNIT_TYPEID::TERRAN_SCV, 8);
    curr->SetUnitAmount(UNIT_TYPEID::TERRAN_COMMANDCENTER, 1);
    curr->SetUnitAmount(UNIT_TYPEID::TERRAN_SUPPLYDEPOT, 3);
    BPState * goal = new BPState();
    goal->SetUnitAmount(UNIT_TYPEID::TERRAN_BATTLECRUISER, 2);
    BPPlan plan;
    plan.AddBasicPlan(curr, goal);
    for (BPAction a : plan) {
        std::cout << a << std::endl;
    }
    std::cout << "plan <<<" << std::endl;
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

void BuildManager::SetGoal(BPState const *const goal) {
    // TODO
}
