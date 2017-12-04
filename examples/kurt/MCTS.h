#pragma once

#include "BPPlan.h"

class BPState;

class MCTS {
public:
    /* Creates an MCTS object with the given initial and goal state */
    MCTS(BPState * const, BPState * const);

    /* Destructors */
    ~MCTS();

    /* Performs given amount of iterations. */
    void Search(int);

    /* Returns the current best plan. */
    BPPlan BestPlan();

private:

    /* Performs one iteration. */
    void SearchOnce();

    /* The root node, also the start state of the search/planning. */
    BPState * root;

    /* The goal node/state. */
    BPState * goal;

    double basic_time;
    int basic_minerals;
    int basic_vespene;

    /* The scale of exploration versus exploitation in the selection phase. */
    const double EXPLORATION_SCALE = 0.2;

    /* The minimum reward for a state that gives a plan
     * that is faster or equaly fast as the basic plan.
     * Should be in range [0, 1].
     */
    const double REWARD_START = 0.2;
};
