#include <sc2api/sc2_api.h>
#include "BPAction.h"
#include "build_manager.h"
#include "kurt.h"

using namespace sc2;


BPAction::BPAction() {
    // TODO
}

BPAction::BPAction(sc2::ABILITY_ID ability) : ability(ability), action_type(USE_ABILITY) {

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
    BPAction tmp(Kurt::GetUnitType(unit_type)->ability_id);
    return tmp;
}
