#include "BPAction.h"
#include "build_manager.h"
#include "kurt.h"

#include <vector>
#include <stdexcept>

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

bool IsIdleSCV(Unit const &unit) {
    return unit.unit_type == UNIT_TYPEID::TERRAN_SCV && unit.orders.empty();
}

std::set<ABILITY_ID> acceptable_to_interrupt = {
      ABILITY_ID::HARVEST_GATHER, ABILITY_ID::HARVEST_GATHER_DRONE
    , ABILITY_ID::HARVEST_GATHER_PROBE
    , ABILITY_ID::HARVEST_GATHER_SCV
    , ABILITY_ID::HARVEST_RETURN, ABILITY_ID::HARVEST_RETURN_DRONE
    , ABILITY_ID::HARVEST_RETURN_MULE
    , ABILITY_ID::HARVEST_RETURN_PROBE
    , ABILITY_ID::HARVEST_RETURN_SCV
};

Unit const *FindNearestUnitOfType(UNIT_TYPEID type, Point2D const &location, ObservationInterface const *obs, Unit::Alliance *alliance = nullptr) {
    Units candidates = obs->GetUnits(*alliance, [type, alliance](Unit const &unit) { return unit.unit_type == type; });
    Unit const *best = nullptr;
    float distance_squared = INFINITY;
    for (Unit const *candidate : candidates) {
        float dx2 = (candidate->pos.x - location.x) * (candidate->pos.x - location.x);
        float dy2 = (candidate->pos.y - location.y) * (candidate->pos.y - location.y);
        if (dx2 + dy2 < distance_squared) {
            distance_squared = dx2 + dy2;
            best = candidate;
        }
    }
    return best;
}

bool BPAction::Execute(ActionInterface *action, QueryInterface *query, ObservationInterface const *obs) {
    Unit::Alliance self = Unit::Alliance::Self;
    Units us;
    auto isnt_busy = [](Unit const &unit) {
        return unit.orders.empty()
            || acceptable_to_interrupt.count(unit.orders[0].ability_id);
    };
    switch (action_type) {
    case BPAction::USE_ABILITY:
        for (const Unit *u : obs->GetUnits(Unit::Alliance::Self, isnt_busy)) {
            for (AvailableAbility order : query->GetAbilitiesForUnit(u).abilities) {
                if (order.ability_id == ability) {
                    // TODO: Initialize sensibly
                    Point2D target_point(u->pos.x + GetRandomScalar() * 15
                                        , u->pos.y + GetRandomScalar() * 15);
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
                        std::cerr << "Invalid target type!!" << std::endl;
                        throw std::runtime_error("Build planner - ability had invalid targeting method");
                    }
                    Point2D pt = Point2D(u->pos.x, u->pos.y);
                    action->UnitCommand(u, ability, pt);
                    return true;
                }
            }
        }

        break;
    case BPAction::GATHER_MINERALS:
        // Find an idle SCV
        us = obs->GetUnits(IsIdleSCV);
        if (!us.empty()) {
            Unit const *target = FindNearestUnitOfType(UNIT_TYPEID::NEUTRAL_MINERALFIELD, us[0]->pos, obs);
            if (target == nullptr) {
                throw std::runtime_error("There are no minerals!?");
            }
            action->UnitCommand(us[0], ABILITY_ID::SMART, target);
        }
        else {
            // TODO: Find a suitable SCV to interrupt, instead of throwing up.
            throw std::runtime_error("No idle SCVs found for GATHER_MINERALS action (TODO: Find a suitable one to interrupt)");
        }
        break;
    case BPAction::GATHER_VESPENE:
        us = obs->GetUnits([](Unit const &unit) { return unit.unit_type == UNIT_TYPEID::TERRAN_SCV && unit.orders.empty(); });
        if (!us.empty()) {
            Unit::Alliance a = Unit::Alliance::Self;
            Unit const *target = FindNearestUnitOfType(UNIT_TYPEID::TERRAN_REFINERY, us[0]->pos, obs, &self);
            if (target == nullptr) {
                throw std::runtime_error("You must construct additional refineries");
            }
        }
        else {
            // TODO: Find a suitable SCV to interrupt, instead of throwing up.
            throw std::runtime_error("No idle SCVs found for GATHER_VESPENE action (TODO: Find a suitable one to interrupt)");
        }
        break;
    default:
        throw std::runtime_error("Build planner - invalid action executed");
    }
    return false;
}

bool BPAction::CanExecute() const {
    return false; // TODO
}

bool BPAction::CanExecuteInStateNow(BPState const * const state) const {
    return false; // TODO
}

bool BPAction::CanExecuteInStateNowOrSoon(BPState const * const state) const {
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

