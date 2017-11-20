#include <sc2api/sc2_api.h>
#include "BPAction.h"
#include "build_manager.h"
#include "kurt.h"

using namespace sc2;


BPAction::BPAction() {
    // TODO
}

BPAction::BPAction(sc2::ABILITY_ID ability) :
    ability(ability), action_type(USE_ABILITY) {
}

BPAction::BPAction(bool _, int const type) :
    ability(ABILITY_ID::INVALID), action_type(type) {
}

BPAction::~BPAction() {
    // DOOT
}

void BPAction::Execute() {
    
    // TODO
}

bool BPAction::CanExecute() const {
    return false; // TODO
}

bool BPAction::CanExecuteInState(BPState const * const state) const {
    return false; // TODO
}

BPAction BPAction::CreatesUnit(sc2::UNIT_TYPEID unit_type) {
    return BPAction(Kurt::GetUnitType(unit_type)->ability_id);
}

std::string BPAction::ToString() const {
    if (action_type == BPAction::USE_ABILITY) {
        return AbilityTypeToName(AbilityID(ability));
    } else if (action_type == BPAction::GATHER_MINERALS) {
        return "GATHER_MINERALS";
    } else if (action_type == BPAction::GATHER_VESPENE) {
        return "GATHER_VESPENE";
    }
}

std::ostream& operator<<(std::ostream& os, const BPAction & action) {
    return os << action.ToString();
}

