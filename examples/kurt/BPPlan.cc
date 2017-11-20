#include "BPPlan.h"

#include "kurt.h"
#include "build_manager.h"
#include "BPState.h"
#include "BPAction.h"

#include "sc2api/sc2_api.h"

#include <iostream>
#include <queue>
#include <set>

using namespace sc2;

void BPPlan::AddBasicPlan(BPState * const start,
        BPState * const goal) {
    BPState built(start);
    std::queue<BPAction> add_to_plan;
    std::set<UNIT_TYPEID> need_to_build;
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
        need_to_build.insert(type);
        for (int i = start->GetUnitAmount(type); i < amount; ++i) {
//            add_to_plan.push(BPAction::ActionForBuilding(it->first));
        }
        built.SetUnitAmount(type, amount);
        mineral_cost += Kurt::GetUnitType(type)->mineral_cost;
        vespene_cost += Kurt::GetUnitType(type)->vespene_cost;
    }
    /*
     * Add buildings required for to reach the goal.
     */
    while (! need_to_build.empty()) {
        UNIT_TYPEID type = *(need_to_build.begin());
        if (built.GetUnitAmount(type) > 0) {
            continue;
        }
        for (UNIT_TYPEID req : BuildManager::GetRequirements(type)) {
            if (built.GetUnitAmount(req) > 0) {
                continue;
            }
//            add_to_plan.push(BPAction::ActionForBuilding(it->first));
            built.SetUnitAmount(req, 1);
            mineral_cost += Kurt::GetUnitType(req)->mineral_cost;
            vespene_cost += Kurt::GetUnitType(req)->vespene_cost;
        }
    }
    // TODO build vespene if needed ??
    /*
     * Add all actions in reverse.
     */
    while (! add_to_plan.empty()) {
        push_back(add_to_plan.front());
        add_to_plan.pop();
    }
}

float BPPlan::TimeRequired() const {
    return INFINITY; // TODO
}

void BPPlan::ExecuteStep() {
    // TODO
}
