#include "BPAction.h"
#include "build_manager.h"
#include "kurt.h"
#include <vector>

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

void BPAction::Execute(ActionInterface *action, QueryInterface *query, ObservationInterface *obs) {
    switch (action_type) {
    case BPAction::USE_ABILITY:
        Unit const *unit_of_interest;
        for (const Unit *u : obs->GetUnits(Unit::Alliance::Self)) {
            for (UnitOrder order : u->orders) {
                if (order.ability_id == ability) {
                    // TODO: Initialize sensibly
                    Point2D target_point;
                    Unit target_unit;
                    switch (Kurt::GetAbility(ability)->target) {
                    case sc2::AbilityData::Target::None:
                        action->UnitCommand(u, ability);
                        break;
                    case sc2::AbilityData::Target::Point:
                        action->UnitCommand(u, ability, target_point);
                        break;
                    case sc2::AbilityData::Target::Unit:
                        action->UnitCommand(u, ability, &target_unit);
                        break;
                    case sc2::AbilityData::Target::PointOrNone:
                        action->UnitCommand(u, ability);
                        // TODO: Maybe target someplace?
                        break;
                    case sc2::AbilityData::Target::PointOrUnit:
                        action->UnitCommand(u, ability, target_point);
                        // TODO: Where or who?
                        break;
                    default:
                        // No
                        throw std::exception("Build planner - ability had invalid targeting method");
                    }
                    Point2D pt = Point2D(u->pos.x, u->pos.y);
                    action->UnitCommand(u, ability, pt);
                }
            }
        }
        
        break;
    case BPAction::GATHER_MINERALS:
        // TODO
        break;
    case BPAction::GATHER_GAS:
        // TODO
        break;
    default:
        throw std::exception("Build planner - invalid action executed");
    }
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

