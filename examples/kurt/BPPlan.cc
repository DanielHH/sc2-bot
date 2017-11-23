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
    // TODO Remove duplicated code?
    BPState built(start);
    std::stack<BPAction> add_to_plan;
    std::queue<UNIT_TYPEID> need_to_build;
    int mineral_cost = 0;
    int vespene_cost = 0;
    int food_required = 0;
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
        UnitTypeData * t_data = Kurt::GetUnitType(type);
        mineral_cost += t_data->mineral_cost * amount;
        vespene_cost += t_data->vespene_cost * amount;
        food_required += t_data->food_required * amount;
        food_required -= t_data->food_provided * amount;
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
            UnitTypeData * t_data = Kurt::GetUnitType(req);
            mineral_cost += t_data->mineral_cost;
            vespene_cost += t_data->vespene_cost;
            food_required += t_data->food_required;
            food_required -= t_data->food_provided;
        }
    }
    /*
     * Add supplydepots if there is not enough food.
     */
    int food_in_store = start->GetFoodCap() - start->GetFoodUsed();
    while (food_required > food_in_store) {
        UNIT_TYPEID supplydepot = UNIT_TYPEID::TERRAN_SUPPLYDEPOT;
        built.SetUnitAmount(supplydepot, built.GetUnitAmount(supplydepot) + 1);
        add_to_plan.push(BPAction::CreatesUnit(supplydepot));
        UnitTypeData * t_data = Kurt::GetUnitType(supplydepot);
        mineral_cost += t_data->mineral_cost;
        vespene_cost += t_data->vespene_cost;
        food_required += t_data->food_required;
        food_required -= t_data->food_provided;
    }
    /*
     * Add refinery if plan requires more vespene than we got
     * and no vespene is beeing produced.
     *
     * TODO Is this bad for MCTS?
     */
    if (vespene_cost > built.GetVespene() &&
            built.GetUnitAmount(UNIT_TYPEID::TERRAN_REFINERY) == 0) {
        UNIT_TYPEID refinery = UNIT_TYPEID::TERRAN_REFINERY;
        built.SetUnitAmount(refinery, 1);
        add_to_plan.push(BPAction(0, BPAction::GATHER_VESPENE));
        add_to_plan.push(BPAction(0, BPAction::GATHER_VESPENE));
        add_to_plan.push(BPAction::CreatesUnit(refinery));
        UnitTypeData * t_data = Kurt::GetUnitType(refinery);
        mineral_cost += t_data->mineral_cost;
        vespene_cost += t_data->vespene_cost;
        food_required += t_data->food_required;
        food_required -= t_data->food_provided;
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

std::string BPPlan::ToString() const {
    std::string val = "BPPlan(";
    for (int i = 0; i < vector::size(); ++i) {
        BPAction action = vector::operator[](i);
        val += action.ToString();
        if (i + 1 < vector::size()) {
            val += ", ";
        }
    }
    val += ")";
    return val;
}

std::ostream& operator<<(std::ostream& os, const BPPlan & plan) {
    return os << plan.ToString();
}
