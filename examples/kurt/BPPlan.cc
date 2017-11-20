#include "BPPlan.h"
#include "BPState.h"

#include <queue>
#include <set>

using namespace sc2;

void BPPlan::AddBasicPlan(BPState * const start,
        BPState * const goal) {
    BPState built(start);
    std::queue<BPAction *> add_to_plan;
    std::set<UNIT_TYPEID> need_to_build;
    int mineral_cost = 0;
    int vespene_cost = 0;
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
//        mineral_cost += 
    }
    while (! need_to_build.empty()) {
        UNIT_TYPEID type = *(need_to_build.begin());
        if (built.GetUnitAmount(type) > 0) {
            continue;
        }
        for (UNIT_TYPEID req : GETREQ(type)) {
            if (built.GetUnitAmount(req) > 0) {
                continue;
            }
//            add_to_plan.push();
            built.SetUnitAmount(req, 1);
//            mineral_cost += 
        }
    }
    // TODO build vespene if needed ??
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
