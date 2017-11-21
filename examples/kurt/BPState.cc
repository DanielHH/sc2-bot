#include <sc2api/sc2_api.h>

#include "BPState.h"

using namespace sc2;

BPState::BPState() {
}

BPState::BPState(BPState * const state) {
    for (auto it = state->UnitsBegin(); it != state->UnitsEnd(); ++it) {
        SetUnitAmount(it->first, it->second);
    }
}

BPState::BPState(const ObservationInterface* observation) {
    for (auto unit : observation->GetUnits(Unit::Alliance::Self)) {
        UNIT_TYPEID type = unit->unit_type.ToType();
        SetUnitAmount(type, GetUnitAmount(type) + 1);
    }
    minerals = observation->GetMinerals();
    vespene = observation->GetVespene();
    food_cap = observation->GetFoodCap();
    food_used = observation->GetFoodUsed();
}

BPState::BPState(BPState const * const initial, BPAction const * const step) {
    // TODO
}

BPState::~BPState() {
    // TODO
}

std::vector<BPAction *> BPState::AvailableActions() const {
    std::vector<BPAction *> tmp; // TODO
    return tmp;
}

int BPState::GetUnitAmount(UNIT_TYPEID type) {
    // Need to test if element exist to prevent allocating more values
    if (unit_amount.count(type) == 0) {
        return 0;
    } else {
        return unit_amount[type];
    }
}

void BPState::SetUnitAmount(UNIT_TYPEID type, int amount) {
    unit_amount[type] = amount;
}

std::map<sc2::UNIT_TYPEID, int>::iterator BPState::UnitsBegin() {
    return unit_amount.begin();
}

std::map<sc2::UNIT_TYPEID, int>::iterator BPState::UnitsEnd() {
    return unit_amount.end();
}

int BPState::GetMinerals() const {
    return minerals;
}

int BPState::GetVespene() const {
    return vespene;
}

int BPState::GetFoodCap() const {
    return food_cap;
}

int BPState::GetFoodUsed() const {
    return food_used;
}

