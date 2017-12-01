#include "BPAction.h"
#include "build_manager.h"
#include "kurt.h"

#include <vector>
#include <stdexcept>

//#define DEBUG // Comment out to disable debug prints in this file.
#ifdef DEBUG
#include <iostream>
#define PRINT(s) std::cout << s << std::endl;
#define TEST(s) s
#else
#define PRINT(s)
#define TEST(s)
#endif // DEBUG

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

bool IsSCV(Unit const &unit) {
    return unit.unit_type == UNIT_TYPEID::TERRAN_SCV;
}

std::set<ABILITY_ID> BPAction::acceptable_to_interrupt = {
      ABILITY_ID::HARVEST_GATHER, ABILITY_ID::HARVEST_GATHER_DRONE
    , ABILITY_ID::HARVEST_GATHER_PROBE
    , ABILITY_ID::HARVEST_GATHER_SCV
    , ABILITY_ID::HARVEST_RETURN, ABILITY_ID::HARVEST_RETURN_DRONE
    , ABILITY_ID::HARVEST_RETURN_MULE
    , ABILITY_ID::HARVEST_RETURN_PROBE
    , ABILITY_ID::HARVEST_RETURN_SCV
};

Unit const *FindNearestUnitOfType(UNIT_TYPEID type, Point2D const &location, ObservationInterface const *obs, Unit::Alliance *alliance = nullptr) {
    Units candidates;
    if (alliance == nullptr) candidates = obs->GetUnits([type](Unit const &unit) { return unit.unit_type == type; });
    else candidates = obs->GetUnits(*alliance, [type, alliance](Unit const &unit) { return unit.unit_type == type; });
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

bool BPAction::Execute(
        Kurt * const kurt,
        ActionInterface *action,
        QueryInterface *query,
        ObservationInterface const *obs) {
    Units us;
    auto is_idle_or_scv = [](Unit const &unit) {
        return unit.orders.empty()
            || unit.unit_type.ToType() == UNIT_TYPEID::TERRAN_SCV;
    };
    switch (action_type) {
    case BPAction::USE_ABILITY:
        for (const Unit *u : obs->GetUnits(Unit::Alliance::Self, is_idle_or_scv)) {
            for (AvailableAbility order : query->GetAbilitiesForUnit(u).abilities) {
                if (order.ability_id != ability) {
                    continue;
                }
                if (u->unit_type.ToType() == UNIT_TYPEID::TERRAN_SCV &&
                        ! kurt->UnitInScvMinerals(u)) {
                    continue;
                }
                Point2D target_point(u->pos.x + GetRandomScalar() * 15
                                    , u->pos.y + GetRandomScalar() * 15);
                Unit const *target_unit;
                switch (Kurt::GetAbility(ability)->target) {
                case sc2::AbilityData::Target::None:
                    action->UnitCommand(u, ability);
                    break;
                case sc2::AbilityData::Target::Point:
                    // Assume we have to place a unit.
                    while (!query->Placement(ability, target_point, u)) {
                        target_point = Point2D(u->pos.x + GetRandomScalar() * 15
                            , u->pos.y + GetRandomScalar() * 15);
                    }
                    action->UnitCommand(u, ability, target_point);
                    break;
                case sc2::AbilityData::Target::Unit:
                    if (ability == ABILITY_ID::BUILD_REFINERY) {
                        Unit::Alliance neutral = Unit::Alliance::Neutral;
                        target_unit = FindNearestUnitOfType(
                              UNIT_TYPEID::NEUTRAL_VESPENEGEYSER
                            , u->pos
                            , obs
                            , &neutral);
                    }
                    action->UnitCommand(u, ability, target_unit);
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
                /*Point2D pt = Point2D(u->pos.x, u->pos.y);
                action->UnitCommand(u, ability, pt);*/
                if (u->unit_type.ToType() == UNIT_TYPEID::TERRAN_SCV) {
                    kurt->scv_minerals.remove(u);
                    kurt->workers.push_back(u);
                }
                return true;
            }
        }

        break;
    case BPAction::GATHER_MINERALS_SCV:
        // Make an SCV stop gather vespene and start gather minerals
        us = obs->GetUnits(Unit::Alliance::Self, IsSCV);
        for (Unit const * scv : us) {
            if (! kurt->UnitInScvVespene(scv)) {
                continue;
            }
            Unit const *target = FindNearestUnitOfType(
                    UNIT_TYPEID::NEUTRAL_MINERALFIELD, scv->pos, obs);
            if (target == nullptr) {
                continue;
            }
            action->UnitCommand(scv, ABILITY_ID::SMART, target);
            kurt->scv_vespene.remove(scv);
            kurt->scv_minerals.push_back(scv);
            return true;
        }
        return false;
    case BPAction::GATHER_VESPENE_SCV:
        // Make an SCV stop gather minerals and start gather vespene
        us = obs->GetUnits(Unit::Alliance::Self, IsSCV);
        for (Unit const * scv : us) {
            if (! kurt->UnitInScvMinerals(scv)) {
                continue;
            }
            Unit const *target = FindNearestUnitOfType(
                    UNIT_TYPEID::TERRAN_REFINERY, scv->pos, obs);
            if (target == nullptr) {
                continue;
            }
            action->UnitCommand(scv, ABILITY_ID::SMART, target);
            kurt->scv_vespene.push_back(scv);
            kurt->scv_minerals.remove(scv);
            return true;
        }
        return false;
    default:
        throw std::runtime_error("Build planner - invalid action executed");
    }
    return false;
}

bool BPAction::CanExecute() const {
    return false; // TODO
}

BPAction BPAction::CreatesUnit(sc2::UNIT_TYPEID unit_type) {
    return BPAction(Kurt::GetUnitType(unit_type)->ability_id);
}

std::string BPAction::ToString() const {
    if (action_type == BPAction::USE_ABILITY) {
        return AbilityTypeToName(AbilityID(ability));
    } else if (action_type == BPAction::GATHER_MINERALS_SCV) {
        return "GATHER_MINERALS";
    } else if (action_type == BPAction::GATHER_VESPENE_SCV) {
        return "GATHER_VESPENE";
    } else {
        return "INVALID";
    }
}

std::ostream& operator<<(std::ostream& os, const BPAction & action) {
    return os << action.ToString();
}


#undef DEBUG
#undef PRINT
#undef TEST
