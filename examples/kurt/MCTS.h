#pragma once

#include "BPPlan.h"

class BPState;

class MCTS {
public:
    /* Creates an MCTS object with the given initial and goal state */
    MCTS(BPState * const, BPState * const);

    /* Destructors */
    ~MCTS();

    /* Calculates the reward for a state given time and rate for that state. */
    double CalcReward(double time, double mineral_rate, double vespene_rate);

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

    /* Stored values for executing only the basic plan. */
    double basic_time;
    double basic_mineral_rate;
    double basic_vespene_rate;

    /* How big impact each factor has on the reward for a state.
     * The total should sum up to 1.
     */
    double time_portion = 0.4;
    double minerals_portion = 0.3;
    double vespene_portion = 0.3;

    /* The scale of exploration versus exploitation in the selection phase. */
    const double EXPLORATION_SCALE = 1.2;

    /* The minimum reward for a state that gives a plan
     * that is faster or equaly fast as the basic plan.
     * Should be in range [0, 1].
     */
    const double REWARD_START = 0.2;
};
