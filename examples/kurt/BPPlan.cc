#include "BPPlan.h"

#include "kurt.h"
#include "build_manager.h"
#include "BPState.h"
#include "BPAction.h"

#include "sc2api/sc2_api.h"

#include <iostream>
#include <queue>
#include <stack>

using namespace sc2;

void BPPlan::AddBasicPlan(BPState * const start,
        BPState * const goal) {
    BPState built(start);
    std::stack<BPAction> add_to_plan;
    std::queue<UNIT_TYPEID> need_to_build;
    int mineral_cost = 0;
    int vespene_cost = 0;
    /*
     * Add buildings directly from the goal.
     */
    for (auto it = goal->UnitsBegin(); it != goal->UnitsEnd(); ++it) {
        UNIT_TYPEID type = it->first;
        int amount = it->second;
        if (built.GetUnitAmount(type) >= amount) {
            continue;
        }
        need_to_build.push(type);
        for (int i = start->GetUnitAmount(type); i < amount; ++i) {
            add_to_plan.push(BPAction::CreatesUnit(type));
        }
        built.SetUnitAmount(type, amount);
        mineral_cost += Kurt::GetUnitType(type)->mineral_cost;
        vespene_cost += Kurt::GetUnitType(type)->vespene_cost;
    }
    /*
     * Add buildings required for to reach the goal.
     */
    // TODO Chech if alias exist, should be barracks tech lab > barracks and NOT !=
    while (! need_to_build.empty()) {
        UNIT_TYPEID curr = need_to_build.front();
        need_to_build.pop();
        for (UNIT_TYPEID req : BuildManager::GetRequirements(curr)) {
            if (built.GetUnitAmount(req) > 0) {
                continue;
            }
            need_to_build.push(req);
            add_to_plan.push(BPAction::CreatesUnit(req));
            built.SetUnitAmount(req, 1);
            mineral_cost += Kurt::GetUnitType(req)->mineral_cost;
            vespene_cost += Kurt::GetUnitType(req)->vespene_cost;
        }
    }
    /*
     * Add refinery if any building requires vespene gas.
     *
     * TODO Is this bad for MCTS?
     */
    if (vespene_cost > 0) {
        UNIT_TYPEID refinery = UNIT_TYPEID::TERRAN_REFINERY;
        built.SetUnitAmount(refinery, 1);
        add_to_plan.push(BPAction(0, BPAction::GATHER_VESPENE));
        add_to_plan.push(BPAction(0, BPAction::GATHER_VESPENE));
        add_to_plan.push(BPAction::CreatesUnit(refinery));
        mineral_cost += Kurt::GetUnitType(refinery)->mineral_cost;
        vespene_cost += Kurt::GetUnitType(refinery)->vespene_cost;
    }
    /*
     * Add all actions in reverse.
     */
    while (! add_to_plan.empty()) {
        push_back(add_to_plan.top());
        add_to_plan.pop();
    }
}

float BPPlan::TimeRequired() const {
    return INFINITY; // TODO
}

void BPPlan::ExecuteStep() {
    // TODO
}
