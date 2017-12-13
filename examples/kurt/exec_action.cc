#include "exec_action.h"

#include "sc2api/sc2_api.h"
#include "sc2lib/sc2_lib.h"

#include <algorithm>

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

#if __cplusplus == 201703L
#define FALLTHROUGH [[fallthrough]]
#else
#define FALLTHROUGH
#endif
#define UNREACHABLE PRINT(__FILE__ << "." << __LINE__ << ": UNREACHABLE statement reached");assert(false);

using namespace sc2;
using std::vector;
using std::set;

std::map<Unit const*, int> ExecAction::sent_order_time;
std::map<Unit const*, int> ExecAction::built_refinery_time;
std::vector<Point3D> ExecAction::commandcenter_locations;
int ExecAction::scv_gather_vespene_delay = 0;
int ExecAction::scv_gather_minerals_delay = 0;

set<Tag> techlab_builders;
set<Tag> reactor_builders;

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

    //
    // Update scv to not gather minerals at overfull commandcenters
    //
    // TODO Add support for TERRAN_ORBITALCOMMAND and TERRAN_PLANETARYFORTRESS
    //
    int step = kurt->Observation()->GetGameLoop();
    int delay = 1 * STEPS_PER_SEC;
    int index = 0;
    for (auto it = kurt->scv_minerals.begin(); it != kurt->scv_minerals.end(); ++it) {
        ++index;
        if (step % delay != index) {
            continue;
        }
        const Unit * scv = *it;
        Unit const * commandcenter = FindNearestUnitOfType(
                UNIT_TYPEID::TERRAN_COMMANDCENTER,
                Point2D(scv->pos.x, scv->pos.y),
                kurt->Observation(),
                Unit::Alliance::Self);
        if (commandcenter->assigned_harvesters > commandcenter->ideal_harvesters ||
                DistanceSquared3D(commandcenter->pos, scv->pos) >
                BASE_RESOURCE_TEST_RANGE2) {
            Unit const * field = FindNextMineralField(kurt->Observation());
            if (field != nullptr) {
                kurt->Actions()->UnitCommand(scv, ABILITY_ID::SMART, field);
            }
        }
    }
    //
    // Try asign a scv to a mineral field or vespene geyser
    //
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

bool IsAddonAction(ACTION action) {
    switch (action) {
    case ACTION::BUILD_BARRACKS_REACTOR:
    case ACTION::BUILD_FACTORY_REACTOR:
    case ACTION::BUILD_STARPORT_REACTOR:
    case ACTION::BUILD_BARRACKS_TECH_LAB:
    case ACTION::BUILD_FACTORY_TECH_LAB:
    case ACTION::BUILD_STARPORT_TECH_LAB:
        return true;
    }
    return false;
}

bool ExecAction::Exec(Kurt * const kurt, ACTION action) {
    Units us;
    Unit const * target;
    ActionInterface * action_interface = kurt->Actions();
    QueryInterface *query = kurt->Query();
    ObservationInterface const *obs = kurt->Observation();

    if (!IsAddonAction(action)) {
        for (Unit const *fly_guy : obs->GetUnits([](Unit const &u) {return techlab_builders.count(u.tag) != 0; })) {
            if (query->Placement(AbilityID(ABILITY_ID::LAND), fly_guy->pos, fly_guy)) {
                action_interface->UnitCommand(fly_guy, ABILITY_ID::LAND, fly_guy->pos);
                techlab_builders.erase(fly_guy->tag);
            }
        }

        for (Unit const *fly_guy : obs->GetUnits([](Unit const &u) {return reactor_builders.count(u.tag) != 0; })) {
            if (query->Placement(AbilityID(ABILITY_ID::LAND), fly_guy->pos, fly_guy)) {
                action_interface->UnitCommand(fly_guy, ABILITY_ID::LAND, fly_guy->pos);
                reactor_builders.erase(fly_guy->tag);
            }
        }
    }

    switch (action) {
    default:
        //
        // Test if action can be represented by some ability from the api.
        //
        if (ActionRepr::convert_our_api.count(action) != 0) {
            ABILITY_ID ability = ActionRepr::convert_our_api.at(action);
            return ExecAbility(kurt, ability);
        }
        else {
            std::cout << "Error: exec_action: No case for action: " << ActionToName(action) << std::endl;
            return false;
        }
        UNREACHABLE;
    case ACTION::BUILD_BARRACKS_REACTOR:
        for (auto it = reactor_builders.begin(); it != reactor_builders.end(); ++it) {
            Tag t = *it;
            Unit const *u;
            if ((u = obs->GetUnit(t))->unit_type.ToType() == UNIT_TYPEID::TERRAN_BARRACKSFLYING) {
                Point2D target_point;
                for (float dist = 12; dist < 22; dist += 0.5) {
                    target_point = Kurt::RandomPoint(u->pos, dist, dist);
                    if (query->Placement(ABILITY_ID::BUILD_REACTOR_BARRACKS, target_point, u)) {
                        action_interface->UnitCommand(obs->GetUnit(*it), SC2Type<ABILITY_ID>(ABILITY_ID::BUILD_REACTOR_BARRACKS), target_point, true);
                        reactor_builders.erase(it);
                        return true;
                    }
                }
            }
        }
        for (Unit const *u : obs->GetUnits(Unit::Alliance::Self, [](Unit const &u) {return u.unit_type.ToType() == UNIT_TYPEID::TERRAN_BARRACKS; })) {
            vector<AvailableAbility> aas = query->GetAbilitiesForUnit(u, false).abilities;
            if (std::any_of(aas.begin(), aas.end(), [](AvailableAbility a) {return a.ability_id.ToType() == ABILITY_ID::BUILD_REACTOR; })) {
                if (query->Placement(ABILITY_ID::BUILD_REACTOR_BARRACKS, u->pos, u)) {
                    action_interface->UnitCommand(u, ABILITY_ID::BUILD_REACTOR_BARRACKS, true);
                    return true;
                }
                else {
                    action_interface->UnitCommand(u, ABILITY_ID::LIFT);
                    reactor_builders.insert(u->tag);
                    return false;
                }
            }
        }
        return false;
    case ACTION::BUILD_BARRACKS_TECH_LAB:
        for (auto it = techlab_builders.begin(); it != techlab_builders.end(); ++it) {
            Tag t = *it;
            Unit const *u;
            if ((u = obs->GetUnit(t))->unit_type.ToType() == UNIT_TYPEID::TERRAN_BARRACKSFLYING) {
                Point2D target_point;
                for (float dist = 12; dist < 22; dist += 0.5) {
                    target_point = Kurt::RandomPoint(u->pos, dist, dist);
                    if (query->Placement(ABILITY_ID::BUILD_TECHLAB_BARRACKS, target_point, u)) {
                        action_interface->UnitCommand(obs->GetUnit(*it), SC2Type<ABILITY_ID>(ABILITY_ID::BUILD_TECHLAB_BARRACKS), target_point, true);
                        techlab_builders.erase(it);
                        return true;
                    }
                }
            }
        }
        for (Unit const *u : obs->GetUnits(Unit::Alliance::Self, [](Unit const &u) {return u.unit_type.ToType() == UNIT_TYPEID::TERRAN_BARRACKS; })) {
            vector<AvailableAbility> aas = query->GetAbilitiesForUnit(u, false).abilities;
            if (std::any_of(aas.begin(), aas.end(), [](AvailableAbility a) {return a.ability_id.ToType() == ABILITY_ID::BUILD_TECHLAB; })) {
                if (query->Placement(ABILITY_ID::BUILD_TECHLAB_BARRACKS, u->pos, u)) {
                    action_interface->UnitCommand(u, ABILITY_ID::BUILD_TECHLAB_BARRACKS, true);
                    return true;
                }
                else {
                    action_interface->UnitCommand(u, ABILITY_ID::LIFT);
                    techlab_builders.insert(u->tag);
                    return false;
                }
            }
        }
        return false;
    case ACTION::BUILD_FACTORY_REACTOR:
            for (auto it = reactor_builders.begin(); it != reactor_builders.end(); ++it) {
                Tag t = *it;
                Unit const *u;
                if ((u = obs->GetUnit(t))->unit_type.ToType() == UNIT_TYPEID::TERRAN_FACTORYFLYING) {
                    Point2D target_point;
                        for (float dist = 12; dist < 22; dist += 0.5) {
                        target_point = Kurt::RandomPoint(u->pos, dist, dist);
                        if (query->Placement(ABILITY_ID::BUILD_REACTOR_FACTORY, target_point, u)) {
                            action_interface->UnitCommand(obs->GetUnit(*it), SC2Type<ABILITY_ID>(ABILITY_ID::BUILD_REACTOR_FACTORY), target_point, true);
                            reactor_builders.erase(it);
                            return true;
                        }
                    }
                }
            }
            for (Unit const *u : obs->GetUnits(Unit::Alliance::Self, [](Unit const &u) {return u.unit_type.ToType() == UNIT_TYPEID::TERRAN_FACTORY; })) {
                vector<AvailableAbility> aas = query->GetAbilitiesForUnit(u, false).abilities;
                if (std::any_of(aas.begin(), aas.end(), [](AvailableAbility a) {return a.ability_id.ToType() == ABILITY_ID::BUILD_REACTOR; })) {
                    if (query->Placement(ABILITY_ID::BUILD_REACTOR_FACTORY, u->pos, u)) {
                        action_interface->UnitCommand(u, ABILITY_ID::BUILD_REACTOR_FACTORY, true);
                        return true;
                    }
                    else {
                        action_interface->UnitCommand(u, ABILITY_ID::LIFT);
                        reactor_builders.insert(u->tag);
                        return false;
                    }
                }
            }
            return false;
        case ACTION::BUILD_FACTORY_TECH_LAB:
            for (auto it = techlab_builders.begin(); it != techlab_builders.end(); ++it) {
                Tag t = *it;
                Unit const *u;
                if ((u = obs->GetUnit(t))->unit_type.ToType() == UNIT_TYPEID::TERRAN_FACTORYFLYING) {
                    Point2D target_point;
                        for (float dist = 12; dist < 22; dist += 0.5) {
                        target_point = Kurt::RandomPoint(u->pos, dist, dist);
                        if (query->Placement(ABILITY_ID::BUILD_TECHLAB_FACTORY, target_point, u)) {
                            action_interface->UnitCommand(obs->GetUnit(*it), SC2Type<ABILITY_ID>(ABILITY_ID::BUILD_TECHLAB_FACTORY), target_point, true);
                            techlab_builders.erase(it);
                            return true;
                        }
                    }
                }
            }
            for (Unit const *u : obs->GetUnits(Unit::Alliance::Self, [](Unit const &u) {return u.unit_type.ToType() == UNIT_TYPEID::TERRAN_FACTORY; })) {
                vector<AvailableAbility> aas = query->GetAbilitiesForUnit(u, false).abilities;
                if (std::any_of(aas.begin(), aas.end(), [](AvailableAbility a) {return a.ability_id.ToType() == ABILITY_ID::BUILD_TECHLAB; })) {
                    if (query->Placement(ABILITY_ID::BUILD_TECHLAB_FACTORY, u->pos, u)) {
                        action_interface->UnitCommand(u, ABILITY_ID::BUILD_TECHLAB_FACTORY, true);
                        return true;
                    }
                    else {
                        action_interface->UnitCommand(u, ABILITY_ID::LIFT);
                        techlab_builders.insert(u->tag);
                        return false;
                    }
                }
            }
            return false;
    case ACTION::BUILD_STARPORT_REACTOR:
        for (auto it = reactor_builders.begin(); it != reactor_builders.end(); ++it) {
            Tag t = *it;
            Unit const *u;
            if ((u = obs->GetUnit(t))->unit_type.ToType() == UNIT_TYPEID::TERRAN_STARPORTFLYING) {
                Point2D target_point;
                for (float dist = 12; dist < 22; dist += 0.5) {
                    target_point = Kurt::RandomPoint(u->pos, dist, dist);
                    if (query->Placement(ABILITY_ID::BUILD_REACTOR_STARPORT, target_point, u)) {
                        action_interface->UnitCommand(obs->GetUnit(*it), SC2Type<ABILITY_ID>(ABILITY_ID::BUILD_REACTOR_STARPORT), target_point, true);
                        reactor_builders.erase(it);
                        return true;
                    }
                }
            }
        }
        for (Unit const *u : obs->GetUnits(Unit::Alliance::Self, [](Unit const &u) {return u.unit_type.ToType() == UNIT_TYPEID::TERRAN_STARPORT; })) {
            vector<AvailableAbility> aas = query->GetAbilitiesForUnit(u, false).abilities;
            if (std::any_of(aas.begin(), aas.end(), [](AvailableAbility a) {return a.ability_id.ToType() == ABILITY_ID::BUILD_REACTOR; })) {
                if (query->Placement(ABILITY_ID::BUILD_REACTOR_STARPORT, u->pos, u)) {
                    action_interface->UnitCommand(u, ABILITY_ID::BUILD_REACTOR_STARPORT, true);
                    return true;
                }
                else {
                    action_interface->UnitCommand(u, ABILITY_ID::LIFT);
                    reactor_builders.insert(u->tag);
                    return false;
                }
            }
        }
        return false;
    case ACTION::BUILD_STARPORT_TECH_LAB:
        for (auto it = techlab_builders.begin(); it != techlab_builders.end(); ++it) {
            Tag t = *it;
            Unit const *u;
            if ((u = obs->GetUnit(t))->unit_type.ToType() == UNIT_TYPEID::TERRAN_STARPORTFLYING) {
                Point2D target_point;
                for (float dist = 12; dist < 22; dist += 0.5) {
                    target_point = Kurt::RandomPoint(u->pos, dist, dist);
                    if (query->Placement(ABILITY_ID::BUILD_TECHLAB_STARPORT, target_point, u)) {
                        action_interface->UnitCommand(obs->GetUnit(*it), SC2Type<ABILITY_ID>(ABILITY_ID::BUILD_TECHLAB_STARPORT), target_point, true);
                        techlab_builders.erase(it);
                        return true;
                    }
                }
            }
        }
        for (Unit const *u : obs->GetUnits(Unit::Alliance::Self, [](Unit const &u) {return u.unit_type.ToType() == UNIT_TYPEID::TERRAN_STARPORT; })) {
            vector<AvailableAbility> aas = query->GetAbilitiesForUnit(u, false).abilities;
            if (std::any_of(aas.begin(), aas.end(), [](AvailableAbility a) {return a.ability_id.ToType() == ABILITY_ID::BUILD_TECHLAB; })) {
                if (query->Placement(ABILITY_ID::BUILD_TECHLAB_STARPORT, u->pos, u)) {
                    action_interface->UnitCommand(u, ABILITY_ID::BUILD_TECHLAB_STARPORT, true);
                    return true;
                } else {
                    action_interface->UnitCommand(u, ABILITY_ID::LIFT);
                    techlab_builders.insert(u->tag);
                    return false;
                }
            }
        }
        return false;
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
            if (!kurt->UnitInScvVespene(scv)) {
                continue;
            }
            if (!scv->orders.empty() &&
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
            if (!kurt->UnitInScvMinerals(scv)) {
                continue;
            }
            action_interface->UnitCommand(scv, ABILITY_ID::SMART, target);
            kurt->scv_vespene.push_back(scv);
            kurt->scv_minerals.remove(scv);
            scv_gather_vespene_delay = 1 * STEPS_PER_SEC;

            return TEST(built_a_tech_lab = )true;
        }
        return false;
    }
}

bool ExecAction::ExecAbility(Kurt * const kurt, ABILITY_ID ability) {
    ActionInterface * action_interface = kurt->Actions();
    QueryInterface *query = kurt->Query();
    ObservationInterface const *obs = kurt->Observation();

    auto is_idle_or_scv = [obs](Unit const &unit) {
        if (unit.unit_type.ToType() == UNIT_TYPEID::TERRAN_SCV) return true;
        Unit const *addon_unit = obs->GetUnit(unit.add_on_tag);
        if (addon_unit && addon_unit->build_progress >= 1) {
            switch (addon_unit->unit_type.ToType()) {
            case UNIT_TYPEID::TERRAN_REACTOR:
            case UNIT_TYPEID::TERRAN_BARRACKSREACTOR:
            case UNIT_TYPEID::TERRAN_FACTORYREACTOR:
            case UNIT_TYPEID::TERRAN_STARPORTREACTOR:
                return unit.orders.size() <= 1;
            }
        }
        return unit.orders.empty();
    };

    for (const Unit *u : obs->GetUnits(Unit::Alliance::Self, is_idle_or_scv)) {
        if (ExecAbility(kurt, ability, u)) return true;
    }
    return false;
}

bool ExecAction::ExecAbility(Kurt * const kurt, ABILITY_ID ability, Unit const *u) {
    QueryInterface *query = kurt->Query();
    ActionInterface *action_interface = kurt->Actions();
    ObservationInterface const *obs = kurt->Observation();
    if (u->build_progress < 1) {
        return false; // Unit under construction.
    }
    for (AvailableAbility order : query->GetAbilitiesForUnit(u, true).abilities) {
        if (order.ability_id != ability) {
            continue;
        }
        if (u->unit_type.ToType() == UNIT_TYPEID::TERRAN_SCV &&
            !kurt->UnitInScvMinerals(u)) {
            continue;
        }

        bool can_afford_it = false;
        for (AvailableAbility affordable : query->GetAbilitiesForUnit(u, false).abilities) {
            if (affordable.ability_id == ability) {
                can_afford_it = true;
                break;
            }
        }

        if (!can_afford_it) {
            continue;
        }
        Point2D target_point(Kurt::RandomPoint(u->pos, 15, 15));
        Unit const *target_unit;
        switch (Kurt::GetAbility(ability)->target) {
        case sc2::AbilityData::Target::None:
            action_interface->UnitCommand(u, ability, true);
            break;
        case sc2::AbilityData::Target::Point:
            switch (ability) {
            case ABILITY_ID::BUILD_COMMANDCENTER: 
                if (!FindNextCommandcenterLoc(obs, query, u->pos, target_point)) {
                    return false;
                }
                break;
            default:
            {
                bool success = false;
                // Assume we have to place a unit.
                for (float dist = 12; dist < 22; dist += 0.5) {
                    target_point = Kurt::RandomPoint(u->pos, dist, dist);
                    if (query->Placement(ability, target_point, u)) {
                        success = true;
                        break;
                    }
                    dist += 0.5;
                }
                if (!success) return false;
            }
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
            }
            else {
                kurt->scv_building.push_back(u);
            }
        }
        sent_order_time[u] = obs->GetGameLoop();
        return true;
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
        // Refinery isn't built yet
        if (refinery->build_progress < 1.0) {
            continue;
        }
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
        // Commandcenter isn't built yet
        if (commandcenter->build_progress < 1.0) {
            continue;
        }
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

bool ExecAction::FindNextCommandcenterLoc(
        ObservationInterface const * obs,
        QueryInterface * query,
        Point3D const & nearby,
        Point2D & ans) {
    Units commandcenters = obs->GetUnits(IsCommandcenter);
    float closest2 = INFINITY;
    for (int i = 0; i < commandcenter_locations.size(); ++i) {
        Point3D point3D = commandcenter_locations[i];
        float dist2 = DistanceSquared3D(nearby, point3D);
        if (dist2 >= closest2) {
            continue;
        }
        bool taken = false;
        for (Unit const * commandcenter : commandcenters) {
            if (DistanceSquared3D(commandcenter->pos, point3D) < 0.1) {
                taken = true;
                break;
            }
        }
        if (taken) {
            continue;
        }
        Point2D point2D(point3D.x, point3D.y);
        if (! query->Placement(ABILITY_ID::BUILD_COMMANDCENTER, point2D)) {
            continue;
        }
        ans.x = point2D.x;
        ans.y = point2D.y;
        closest2 = dist2;
    }
    return closest2 < INFINITY;
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
    commandcenter_locations = search::CalculateExpansionLocations(
            kurt->Observation(), kurt->Query());
}
