#include "BPPlan.h"
#include "BPState.h"

#include <queue>
#include <set>

using namespace sc2;

void BPPlan::AddBasicPlan(BPState const * const start, BPState const * const goal) {
    std::queue<BPAction *> toAdd;
    std::set<UNIT_TYPEID> toBuild;
    // TODO
}

float BPPlan::TimeRequired() const {
    return INFINITY; // TODO
}

void BPPlan::ExecuteStep() {
    // TODO
}
