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
    root->iter_amount = 0;
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
        if (leaf->iter_amount < leaf->available_actions.size()) {
            break;
        }
        BPState * best_state = nullptr;
        double best_score = -1;
        for (BPState * child : leaf->children) {
            double score = child->reward + EXPLORATION_SCALE *
                std::sqrt(std::log(leaf->iter_amount) / child->iter_amount);
            if (score > best_score) {
                best_score = score;
                best_state = child;
            }
        }
        if (best_score == -1) {
            std::cout<<"Error: MCTS: State with invalid children."<<std::endl;
            return;
        }
        leaf = best_state;
    }
    /*
     * Expansion phase.
     */
    BPState * new_state = nullptr;
    {
        int index = leaf->iter_amount;
        ACTION action = leaf->available_actions[index];
        new_state = new BPState(leaf);
        new_state->AddAction(action);
        leaf->children[index] = new_state;
        new_state->parent = leaf;
    }
    /*
     * Simulation phase.
     */
    double reward;
    //TODO Need basic plan to work with new action....
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
    return BPPlan(); // TODO
}

#undef DEBUG
#undef PRINT
#undef TEST
