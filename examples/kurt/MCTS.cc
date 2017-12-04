#include "MCTS.h"

#include "BPPlan.h"
#include "BPState.h"
#include "action_enum.h"

#include <math.h>
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

MCTS::MCTS(BPState * const root_, BPState * const goal_) {
    root = new BPState(root_);
    goal = new BPState(goal_);

    root->parent = nullptr;
    root->iter_amount = 1;

    BPPlan basic_plan;
    basic_plan.AddBasicPlan(root, goal);
    BPState tmp(root);
    tmp.SimulatePlan(basic_plan);
    basic_time = tmp.GetTime() - root->GetTime();
}

MCTS::~MCTS() {
    delete root;
    delete goal;
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
    BPState * leaf = root;
    while (true) {
        if (leaf->available_actions.empty()) {
            leaf->available_actions = leaf->AvailableActions();
            leaf->children.resize(leaf->available_actions.size());
            if (leaf->available_actions.empty()) {
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
        if (best_score == -1) {
            std::cout<<"Error: MCTS: State with invalid children."<<std::endl;
            return;
        }
        leaf = leaf->children[best_index];
        plan.push_back(leaf->available_actions[best_index]);
    }
    /*
     * Expansion phase.
     */
    BPState * new_state = nullptr;
    {
        int index = leaf->iter_amount - 1;
        ACTION action = leaf->available_actions[index];
        plan.push_back(action);
        new_state = new BPState(leaf);
        new_state->AddAction(action);
        new_state->iter_amount = 0;
        leaf->children[index] = new_state;
        new_state->parent = leaf;
    }
    /*
     * Simulation phase.
     */
    plan.AddBasicPlan(new_state, goal);
    BPState tmp(root);
    tmp.SimulatePlan(plan);
    double time = tmp.GetTime() - root->GetTime();
    double reward = 0;
    if (time <= basic_time) {
        reward = REWARD_START + (1 - REWARD_START) *
            (basic_time - time) / basic_time;
    }
    /*
     * Backpropagation phase.
     */
    BPState * curr = new_state;
    while (curr != nullptr) {
        curr->reward = std::max(curr->reward, reward);
        curr->iter_amount++;
        curr = curr->parent;
    }
}

BPPlan MCTS::BestPlan() {
    BPPlan plan;
    BPState * curr = root;
    double curr_score = -1;
    while (true) {
        if (curr->iter_amount - 1 < curr->available_actions.size()) {
            break;
        }
        int best_index = -1;
        double best_score = -1;
        for (int i = 0; i < curr->children.size(); ++i) {
            BPState * child = curr->children[i];
            double score = child->reward;
            if (score > best_score) {
                best_score = score;
                best_index = i;
            }
        }
        if (best_score == -1) {
            std::cout<<"Error: MCTS: State with invalid children."<<std::endl;
            break;
        }
        if (curr_score > best_score) {
            break;
        }
        curr = curr->children[best_index];
        plan.push_back(curr->available_actions[best_index]);
    }
    plan.AddBasicPlan(curr, goal);
    return plan;
}

#undef DEBUG
#undef PRINT
#undef TEST
