#include <sc2api/sc2_api.h>

#include "BPState.h"

using namespace sc2;

BPState::BPState() {
}

BPState::BPState(const ObservationInterface* observation) {
    // TODO
}

BPState::~BPState() {
    // TODO
}

BPState::BPState(BPState const initial, BPAction const step) {
    // TODO
}

std::vector<BPAction> BPState::AvailableActions() const {
    std::vector<BPAction> tmp; // TODO
    return tmp;
}

int BPState::GetUnitAmount(UNIT_TYPEID type) const {
    // Need to test if element exist to prevent allocating more values
    if (unit_amount.count(type) == 0) {
        return 0;
    } else {
        return unit_amount[type];
    }
}

void SetUnitAmount(UNIT_TYPEID type, int amount) {
    unit_amount[type] = amount;
}
