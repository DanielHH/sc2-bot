#include "MCTS.h"

#include "BPPlan.h"
#include "BPState.h"
#include "action_enum.h"

#include <math.h>
#include <vector>
#include <iostream>

//#define DEBUG // Comment out to disable debug prints in this file.
#ifdef DEBUG
#include <iostream>
#define PRINT(s) std::cout << s << std::endl;
#define TEST(s) s
#else
#define PRINT(s)
#define TEST(s)
#endif // DEBUG

//#define PRINT_TREE // Comment out to disable printing the search tree.
#ifdef PRINT_TREE
#include <iostream>
#define PT(s); std::cout << s;
#define PTLN(s); std::cout << s << std::endl;
#else
#define PT(s);
#define PTLN(s);
#endif

MCTS::MCTS(BPState * const root_, BPState * const goal_) {
    root = new BPState(root_);
    goal = new BPState(goal_);

    BPPlan basic_plan;
    basic_plan.AddBasicPlan(root, goal);
    BPState tmp(root);
    tmp.SimulatePlan(basic_plan);
    double time = tmp.GetTime() - root->GetTime();
    double mineral_rate = tmp.GetMineralRate();
    double vespene_rate = tmp.GetVespeneRate();
    // The min border is needed since the reward is relative the basic plan.
    // If e.g. basic_vespene_rate is 0, all higher rates gets the same reward.
    basic_time = std::max(40.0, time);
    basic_mineral_rate = std::max(5.0, mineral_rate);
    basic_vespene_rate = std::max(10.0, vespene_rate);

    root->parent = nullptr;
    root->iter_amount = 1;
    root->reward = CalcReward(time, mineral_rate, vespene_rate);
    root->reward_stop = root->reward;
}

MCTS::~MCTS() {
    delete root;
    delete goal;
}

double MCTS::CalcReward(double time, double mineral_rate, double vespene_rate) {
    return
        time_portion * basic_time / (time + basic_time) +
        minerals_portion * mineral_rate / (mineral_rate + basic_mineral_rate) +
        vespene_portion * vespene_rate / (vespene_rate + basic_vespene_rate);
}

void MCTS::Search(int num_iterations) {
    for (int i = 0; i < num_iterations; ++i) {
        SearchOnce();
    }
}

void MCTS::SearchOnce() {
    BPPlan plan;
    /*
     * Select phase.
     */
    PRINT("Select phase.")
    BPState * leaf = root;
    while (true) {
        if (leaf->available_actions.empty()) {
            leaf->available_actions = leaf->AvailableActions();
            leaf->children.resize(leaf->available_actions.size(), nullptr);
            if (leaf->available_actions.empty()) {
                leaf->Print();
                std::cout << "Warning: MCTS: Leaf node found" << std::endl;
                return;
            }
            break;
        }
        if (leaf->iter_amount - 1 < leaf->available_actions.size()) {
            break;
        }
        int best_index = -1;
        double best_score = -1;
        for (int i = 0; i < leaf->children.size(); ++i) {
            BPState * child = leaf->children[i];
            double score = child->reward + EXPLORATION_SCALE *
                std::sqrt(std::log(leaf->iter_amount) / child->iter_amount);
            if (score > best_score) {
                best_score = score;
                best_index = i;
            }
        }
        if (best_index == -1) {
            leaf->Print();
            std::cout << "Error: MCTS: Search: " <<
                "State with invalid children." << std::endl;
            return;
        }
        plan.push_back(leaf->available_actions[best_index]);
        leaf = leaf->children[best_index];
    }
    /*
     * Expansion phase.
     */
    PRINT("Expansion phase.")
    BPState * new_state = nullptr;
    {
        int index = leaf->iter_amount - 1;
        ACTION action = leaf->available_actions[index];
        plan.push_back(action);
        new_state = new BPState(leaf);
        new_state->AddAction(action);
        new_state->reward = 0;
        new_state->iter_amount = 0;
        leaf->children[index] = new_state;
        new_state->parent = leaf;
    }
    /*
     * Simulation phase.
     */
    PRINT("Simulation phase.")
    plan.AddBasicPlan(new_state, goal);
    BPState tmp(root);
    tmp.SimulatePlan(plan);
    double time = tmp.GetTime() - root->GetTime();
    double mineral_rate = tmp.GetMineralRate();
    double vespene_rate = tmp.GetVespeneRate();
    double reward = CalcReward(time, mineral_rate, vespene_rate);
    /*
     * Backpropagation phase.
     */
    PRINT("Backpropagation phase.")
    new_state->reward_stop = reward;
    BPState * curr = new_state;
    while (curr != nullptr) {
        curr->reward = std::max(curr->reward, reward);
        curr->iter_amount++;
        curr = curr->parent;
    }
}

BPPlan MCTS::BestPlan() {
    auto spaces = [] (int len) {
        std::string ans = "";
        while (len-- > 0) {
            ans += " ";
        }
        return ans;
    };
    auto plan_print = [] (BPState * curr, BPState * goal) {
        BPPlan tmp;
        tmp.AddBasicPlan(curr, goal);
        return tmp;
    };
    BPPlan plan;
    BPState * curr = root;
    PTLN("");
    PT("--> ROOT: (" << curr->iter_amount << ") ")
    PTLN(curr->reward_stop << " / " << curr->reward);
    int depth = 0;
    while (true) {
        ++depth;
        if (curr->available_actions.empty()) {
            PTLN(spaces(depth * 2) << "(No children expanded)");
            break;
        }
        int best_index = -1;
        double best_score = -1;
        for (int i = 0; i < curr->children.size(); ++i) {
            BPState * child = curr->children[i];
            if (child == nullptr) {
                PTLN(spaces(depth * 2) << "(Not all children expanded)");
                break;
            }
            double score = child->reward;
            PT(spaces(depth * 2) << curr->available_actions[i] << ": (");
            PT(child->iter_amount << ") ")
            PTLN(child->reward_stop << " / " << child->reward);
            if (score > best_score) {
                best_score = score;
                best_index = i;
            }
        }
        if (best_index == -1) {
            curr->Print();
            std::cout << "Error: MCTS: BestPlan: " <<
                "State with invalid children." << std::endl;
            break;
        }
        if (curr->reward_stop >= best_score) {
            PTLN(spaces(depth * 2) << "--> Choosing to stop here");
            break;
        }
        PT(spaces(depth * 2) << "--> Choosing ");
        PTLN(curr->available_actions[best_index]);
        plan.push_back(curr->available_actions[best_index]);
        curr = curr->children[best_index];
    }
    PTLN("MCTS (head): " << plan);
    PTLN("BasicPlan (tail): " << plan_print(curr, goal));
    plan.AddBasicPlan(curr, goal);
    return plan;
}

#undef DEBUG
#undef PRINT
#undef TEST
