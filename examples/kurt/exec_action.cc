#include "exec_action.h"

#include "sc2api/sc2_api.h"
#include "sc2lib/sc2_lib.h"

#include "kurt.h"
#include "constants.h"
#include "action_repr.h"
#include "BPState.h"

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

std::map<Unit const*, int> ExecAction::sent_order_time;
std::map<Unit const*, int> ExecAction::built_refinery_time;
std::set<Point3D> ExecAction::commandcenter_locations;
int ExecAction::scv_gather_vespene_delay = 0;
int ExecAction::scv_gather_minerals_delay = 0;

double ExecAction::TimeSinceOrderSent(Unit const * unit, Kurt * kurt) {
    if (sent_order_time.count(unit) == 0) {
        return 0;
    } else {
        return (kurt->Observation()->GetGameLoop() - sent_order_time[unit])
            / (double) STEPS_PER_SEC;
    }
}

void ExecAction::OnStep(Kurt * kurt) {
    if (scv_gather_vespene_delay > 0) { --scv_gather_vespene_delay; }
    if (scv_gather_minerals_delay > 0) { --scv_gather_minerals_delay; }

    if (! kurt->scv_idle.empty()) {
        Unit const * scv = kurt->scv_idle.front();
        Unit const * target = FindNextMineralField(kurt->Observation());
        if (target != nullptr) {
            if (! kurt->UnitInScvMinerals(scv)) {
                kurt->scv_minerals.push_back(scv);
            }
        } else {
            target = FindNextRefinery(kurt->Observation());
            if (target != nullptr) {
                if (! kurt->UnitInScvVespene(scv)) {
                    kurt->scv_vespene.push_back(scv);
                }
            }
        }
        if (target == nullptr) {
            return;
        }
        kurt->scv_idle.pop_front();
        kurt->scv_minerals.push_back(scv);
        kurt->Actions()->UnitCommand(scv, ABILITY_ID::SMART, target);
    }
}

bool IsVespeneGeyser(Unit const & unit) {
    return unit.unit_type == UNIT_TYPEID::NEUTRAL_VESPENEGEYSER;
};

bool IsMineralField(Unit const & unit) {
    return
        unit.unit_type == UNIT_TYPEID::NEUTRAL_MINERALFIELD ||
        unit.unit_type == UNIT_TYPEID::NEUTRAL_MINERALFIELD750 ||
        unit.unit_type == UNIT_TYPEID::NEUTRAL_RICHMINERALFIELD ||
        unit.unit_type == UNIT_TYPEID::NEUTRAL_RICHMINERALFIELD750;
};

bool IsSCV(Unit const & unit) {
    return unit.unit_type == UNIT_TYPEID::TERRAN_SCV;
};

bool IsRefinery(Unit const & unit) {
    return unit.unit_type == UNIT_TYPEID::TERRAN_REFINERY;
};

bool IsCommandcenter(Unit const & unit) {
    return
        unit.unit_type == UNIT_TYPEID::TERRAN_COMMANDCENTER ||
        unit.unit_type == UNIT_TYPEID::TERRAN_ORBITALCOMMAND ||
        unit.unit_type == UNIT_TYPEID::TERRAN_PLANETARYFORTRESS;
};

void ExecAction::OnUnitIdle(
        Unit const * unit, Kurt * kurt) {
    Unit const * target;
    switch (unit->unit_type.ToType()) {
    case UNIT_TYPEID::TERRAN_SCV:
        kurt->scv_building.remove(unit);
        kurt->scv_idle.remove(unit);
        kurt->scv_minerals.remove(unit);
        kurt->scv_vespene.remove(unit);
        target = FindNextMineralField(kurt->Observation());
        if (target == nullptr) {
            if (! kurt->UnitInList(kurt->scv_idle, unit)) {
                kurt->scv_idle.push_back(unit);
            }
        } else {
            kurt->Actions()->UnitCommand(unit, ABILITY_ID::SMART, target);
            if (! kurt->UnitInScvMinerals(unit)) {
                kurt->scv_minerals.push_back(unit);
            }
        }
        break;
    default:
        break;
    }
}

bool ExecAction::Exec(Kurt * const kurt, ACTION action) {
    //
    // Test if action can be represented by some ability from the api.
    //
    if (ActionRepr::convert_our_api.count(action) != 0) {
        ABILITY_ID ability = ActionRepr::convert_our_api.at(action);
        return ExecAbility(kurt, ability);
    }
    //
    // Execute our own custom action.
    //
    ActionInterface * action_interface = kurt->Actions();
    QueryInterface *query = kurt->Query();
    ObservationInterface const *obs = kurt->Observation();

    Units us;
    Unit const * target;
    switch (action) {
    case ACTION::SCV_GATHER_MINERALS:
        // Make an SCV stop gather vespene and start gather minerals
        if (scv_gather_minerals_delay > 0) {
            return false;
        }
        target = FindNextMineralField(obs);
        if (target == nullptr) {
            return false;
        }
        us = obs->GetUnits(Unit::Alliance::Self, IsSCV);
        for (Unit const * scv : us) {
            if (! kurt->UnitInScvVespene(scv)) {
                continue;
            }
            if (! scv->orders.empty() &&
                    scv->orders[0].ability_id.ToType() == ABILITY_ID::BUILD_REFINERY) {
                continue;
            }
            action_interface->UnitCommand(scv, ABILITY_ID::SMART, target);
            kurt->scv_vespene.remove(scv);
            kurt->scv_minerals.push_back(scv);
            scv_gather_minerals_delay = 1 * STEPS_PER_SEC;
            return true;
        }
        return false;
    case ACTION::SCV_GATHER_VESPENE:
        // Make an SCV stop gather minerals and start gather vespene
        target = FindNextRefinery(obs);
        if (scv_gather_vespene_delay > 0) {
            return false;
        }
        if (target == nullptr) {
            return false;
        }
        us = obs->GetUnits(Unit::Alliance::Self, IsSCV);
        for (Unit const * scv : us) {
            if (! kurt->UnitInScvMinerals(scv)) {
                continue;
            }
            action_interface->UnitCommand(scv, ABILITY_ID::SMART, target);
            kurt->scv_vespene.push_back(scv);
            kurt->scv_minerals.remove(scv);
            scv_gather_vespene_delay = 1 * STEPS_PER_SEC;
            return true;
        }
        return false;
    default:
        std::cout << "Error: exec_action: No case for action: " << ActionToName(action) << std::endl;
        return false;
    }
}

bool ExecAction::ExecAbility(Kurt * const kurt, ABILITY_ID ability) {
    ActionInterface * action_interface = kurt->Actions();
    QueryInterface *query = kurt->Query();
    ObservationInterface const *obs = kurt->Observation();

    auto is_idle_or_scv = [](Unit const &unit) {
        return unit.orders.empty()
            || unit.unit_type.ToType() == UNIT_TYPEID::TERRAN_SCV;
    };

    for (const Unit *u : obs->GetUnits(Unit::Alliance::Self, is_idle_or_scv)) {
        for (AvailableAbility order : query->GetAbilitiesForUnit(u).abilities){
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
                if (ability == ABILITY_ID::TRAIN_SCV) {
                    // TODO Fix a better check if there is space for another scv
                    BPState curr(kurt);
                    if (curr.GetUnitAmount(UNIT_TYPEID::TERRAN_COMMANDCENTER) * 16 <=
                            kurt->scv_minerals.size() + kurt->scv_building.size()) {
                        return false;
                    }
                }
                action_interface->UnitCommand(u, ability);
                break;
            case sc2::AbilityData::Target::Point:
                // Assume we have to place a unit.
                while (!query->Placement(ability, target_point, u)) {
                    target_point = Point2D(u->pos.x + GetRandomScalar() * 15
                        , u->pos.y + GetRandomScalar() * 15);
                }
                action_interface->UnitCommand(u, ability, target_point);
                break;
            case sc2::AbilityData::Target::Unit:
                if (ability == ABILITY_ID::BUILD_REFINERY) {
                    target_unit = FindNextVespeneGeyser(obs);
                    if (target_unit == nullptr) {
                        return false;
                    }
                    built_refinery_time[target_unit] = obs->GetGameLoop();
                }
                action_interface->UnitCommand(u, ability, target_unit);
                break;
            case sc2::AbilityData::Target::PointOrNone:
                action_interface->UnitCommand(u, ability);
                // TODO: Maybe target someplace?
                break;
            case sc2::AbilityData::Target::PointOrUnit:
                action_interface->UnitCommand(u, ability, target_point);
                // TODO: Where or who?
                std::cout << "Warning: exec_action: PointOrUnit, ability: "
                    << AbilityTypeToName(AbilityID(ability)) << std::endl;
                break;
            default:
                // No
                std::cout << "Error: exec_action: Invalid target type!!" << std::endl;
                throw std::runtime_error("Build planner - ability had invalid targeting method");
            }
            /*Point2D pt = Point2D(u->pos.x, u->pos.y);
            action->UnitCommand(u, ability, pt);*/
            if (u->unit_type.ToType() == UNIT_TYPEID::TERRAN_SCV) {
                kurt->scv_minerals.remove(u);
                if (ability == ABILITY_ID::BUILD_REFINERY) {
                    kurt->scv_vespene.push_back(u);
                } else {
                    kurt->scv_building.push_back(u);
                }
            }
            sent_order_time[u] = obs->GetGameLoop();
            return true;
        }
    }
    return false;
}

Unit const * ExecAction::FindNextVespeneGeyser(
        ObservationInterface const * obs) {
    Units geysers = obs->GetUnits(Unit::Alliance::Neutral, IsVespeneGeyser);
    Units refineries = obs->GetUnits(Unit::Alliance::Self, IsRefinery);
    Units commandcenters = obs->GetUnits(Unit::Alliance::Self, IsCommandcenter);
    for (Unit const * geyser : geysers) {
        // Geyser isn't empty
        if (geyser->vespene_contents <= 0) {
            continue;
        }
        // We aren't already going to build an refinery at given geyser
        if (built_refinery_time.count(geyser) != 0) {
            if (obs->GetGameLoop() - built_refinery_time.at(geyser) <
                    10 * STEPS_PER_SEC) {
                continue;
            }
        }
        // The geyser is close to some of our commandcenters
        bool in_range = false;
        for (Unit const * commandcenter : commandcenters) {
            if (DistanceSquared3D(geyser->pos, commandcenter->pos) <
                    BASE_RESOURCE_TEST_RANGE2) {
                in_range = true;
                break;
            }
        }
        if (! in_range) {
            continue;
        }
        // Refinery is not already built on given geyser
        bool taken = false;
        for (Unit const * refinery : refineries) {
            if (geyser->pos.x == refinery->pos.x &&
                    geyser->pos.y == refinery->pos.y &&
                    geyser->pos.z == refinery->pos.z) {
                taken = true;
                break;
            }
        }
        if (taken) {
            continue;
        }
        return geyser;
    }
    return nullptr;
}

Unit const * ExecAction::FindNextRefinery(
        ObservationInterface const * obs) {
    Units refineries = obs->GetUnits(Unit::Alliance::Self, IsRefinery);
    for (Unit const * refinery : refineries) {
        // Refinery isn't overfull
        if (refinery->assigned_harvesters >= refinery->ideal_harvesters) {
            continue;
        }
        return refinery;
    }
    return nullptr;
}

Unit const * ExecAction::FindNextMineralField(
        ObservationInterface const * obs) {
    Units commandcenters = obs->GetUnits(Unit::Alliance::Self, IsCommandcenter);
    Units mineral_fields = obs->GetUnits(Unit::Alliance::Neutral, IsMineralField);
    for (Unit const * commandcenter : commandcenters) {
        // Commandcenter isn't overfull
        if (commandcenter->assigned_harvesters >= commandcenter->ideal_harvesters) {
            continue;
        }
        // Find a mineral field near given good commandcenter
        for (Unit const * mineral_field : mineral_fields) {
            if (DistanceSquared3D(commandcenter->pos, mineral_field->pos) <
                    BASE_RESOURCE_TEST_RANGE2) {
                return mineral_field;
            }
        }
    }
    return nullptr;
}

Unit const * ExecAction::FindNearestUnitOfType(
        UNIT_TYPEID type,
        Point2D const &location,
        ObservationInterface const *obs,
        Unit::Alliance alliance) {
    Units candidates;
    auto cmp = [type] (Unit const &unit) { return unit.unit_type == type; };
    candidates = obs->GetUnits(alliance, cmp);
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

void ExecAction::Init(Kurt * const kurt) {
//    std::vector<sc2::Point3D> expansions = search::CalculateExpansionLocations(kurt->Observation(), kurt->Query());
//    for (Point3D point : expansions) {
//    }
}
