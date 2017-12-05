#include "BPPlan.h"

#include "kurt.h"
#include "build_manager.h"
#include "action_enum.h"
#include "action_repr.h"
#include "exec_action.h"
#include "BPState.h"

#include "sc2api/sc2_api.h"

#include <queue>
#include <stack>
#include <vector>

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

void BPPlan::AddBasicPlan(BPState * const start,
        BPState * const goal) {
    // TODO Remove duplicated code?
    BPState built(start);
    built.CompleteAllActions();
    // add_to_plan is almost a queue of stacks
    std::vector<std::stack<ACTION> > add_to_plan;
    std::queue<UNIT_TYPEID> need_to_build;
    int mineral_cost = 0;
    int vespene_cost = 0;
    int food_required = 0;
    /*
     * Add buildings directly from the goal.
     */
    int add_i = 0;
    for (auto it = goal->UnitsBegin(); it != goal->UnitsEnd(); ++it) {
        ++add_i;
        add_to_plan.resize(add_i + 1);
        UNIT_TYPEID type = it->first;
        int amount = it->second;
        if (built.GetUnitAmount(type) >= amount) {
            continue;
        }
        need_to_build.push(type);
        for (int i = start->GetUnitAmount(type); i < amount; ++i) {
            add_to_plan[add_i].push(ActionRepr::CreatesUnit(type));
        }
        built.SetUnitAmount(type, amount);
        UnitTypeData * t_data = Kurt::GetUnitType(type);
        mineral_cost += t_data->mineral_cost * amount;
        vespene_cost += t_data->vespene_cost * amount;
        food_required += (int) t_data->food_required * amount;
        food_required -= (int) t_data->food_provided * amount;
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
                add_to_plan[add_i].push(ActionRepr::CreatesUnit(req));
                built.SetUnitAmount(req, 1);
                UnitTypeData * t_data = Kurt::GetUnitType(req);
                mineral_cost += t_data->mineral_cost;
                vespene_cost += t_data->vespene_cost;
                food_required += (int) t_data->food_required;
                food_required -= (int) t_data->food_provided;
            }
        }
    }
    add_i = 0;
    /*
     * Add supplydepots if there is not enough food.
     */
    int food_in_store = start->GetFoodCap() - start->GetFoodUsed();
    while (food_required > food_in_store) {
        UNIT_TYPEID supplydepot = UNIT_TYPEID::TERRAN_SUPPLYDEPOT;
        built.SetUnitAmount(supplydepot, built.GetUnitAmount(supplydepot) + 1);
        add_to_plan[add_i].push(ActionRepr::CreatesUnit(supplydepot));
        UnitTypeData * t_data = Kurt::GetUnitType(supplydepot);
        mineral_cost += t_data->mineral_cost;
        vespene_cost += t_data->vespene_cost;
        food_required += (int) t_data->food_required;
        food_required -= (int) t_data->food_provided;
    }
    /*
     * Add refinery if plan requires more vespene than we got
     * and no vespene is beeing produced.
     *
     * TODO Is this bad for MCTS?
     */
    if (vespene_cost > built.GetVespene()) {
        if (built.GetVespeneRate() == 0) {
            add_to_plan[add_i].push(ACTION::SCV_GATHER_VESPENE);
            add_to_plan[add_i].push(ACTION::SCV_GATHER_VESPENE);
        }
        if (built.GetUnitAmount(UNIT_TYPEID::TERRAN_REFINERY) == 0) {
            UNIT_TYPEID refinery = UNIT_TYPEID::TERRAN_REFINERY;
            built.SetUnitAmount(refinery, 1);
            add_to_plan[add_i].push(ActionRepr::CreatesUnit(refinery));
        }
    }
    /*
     * Add all actions in reverse.
     */
    for (int i = 0; i < add_to_plan.size(); ++i) {
        while (! add_to_plan[i].empty()) {
            push_back(add_to_plan[i].top());
            add_to_plan[i].pop();
        }
    }
}

float BPPlan::TimeRequired(BPState * const from) {
    BPState tmp(from);
    tmp.SimulatePlan(this);
    return tmp.GetTime() - from->GetTime();
}

void BPPlan::ExecuteStep(Kurt * const kurt) {
    int i;
    for (i = 0; i < std::min(1, (int) vector::size()); ++i) {
        ACTION action = vector::operator[](i);
        PRINT("Try to exec action " << action)
        if (! ExecAction::Exec(kurt, action)) {
            break;
        } else {
            std::cout << "Executed action " << ActionToName(action);
            if (i + 1 < vector::size()) {
                ACTION next_a = vector::operator[](i + 1);
                std::cout << ", next action is " << ActionToName(next_a);
            } else {
                std::cout << ", no more actions in this BPPlan";
            }
            std::cout << std::endl;
        }
    }
    auto beg = vector::begin();
    vector::erase(beg, beg + i);
}

std::string BPPlan::ToString() const {
    std::string val = "BPPlan(";
    for (int i = 0; i < vector::size(); ++i) {
        ACTION action = vector::operator[](i);
        val += ActionToName(action);
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

#undef DEBUG
#undef TEST
#undef PRINT
