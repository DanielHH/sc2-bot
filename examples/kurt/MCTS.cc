#include "MCTS.h"
#include "BPPlan.h"
#include "sc2api/sc2_interfaces.h"
#include "s2clientprotocol/common.pb.h"
#include "BPState.h"
#include "BPAction.h"
#include <fstream>
#include <map>
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

void MCTS::Search(int num_iterations) {
    BPState root;
    BPAction action;

    std::map < BPState, std::vector<BPState>> tree;
    BPState *current = &root;

    while (current) { // TODO: exit loop sometimes
        for (auto action : current->AvailableActions()) {
            tree[current].push_back(BPState(current, action));
        }
        // TODO: More stuff.
    }

    return; // TODO: Anything
}

BPPlan * MCTS::BestPlan() {
    return nullptr; // TODO: FIX THIS
}

#undef DEBUG
#undef PRINT
#undef TEST
