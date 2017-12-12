#include "exec_action.h"

#include "sc2api/sc2_api.h"
#include "sc2lib/sc2_lib.h"

#include <iostream>
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

using namespace sc2;
using std::vector;

std::map<Unit const*, int> ExecAction::sent_order_time;
std::map<Unit const*, int> ExecAction::built_refinery_time;
std::vector<Point3D> ExecAction::commandcenter_locations;
int ExecAction::scv_gather_vespene_delay = 0;
int ExecAction::scv_gather_minerals_delay = 0;

vector<Tag> techlab_builders;
vector<Tag> reactor_builders;

double ExecAction::TimeSinceOrderSent(Unit const * unit, Kurt * kurt) {
std::cout << "exec_action: 36" << std::endl;
    if (sent_order_time.count(unit) == 0) {
std::cout << "exec_action: 37" << std::endl;
        return 0;
std::cout << "exec_action: 38" << std::endl;
    } else {
std::cout << "exec_action: 39" << std::endl;
        return (kurt->Observation()->GetGameLoop() - sent_order_time[unit])
            / (double) STEPS_PER_SEC;
std::cout << "exec_action: 41" << std::endl;
    }
}

void ExecAction::OnStep(Kurt * kurt) {
std::cout << "exec_action: 45" << std::endl;
    if (scv_gather_vespene_delay > 0) { --scv_gather_vespene_delay; }
std::cout << "exec_action: 46" << std::endl;
    if (scv_gather_minerals_delay > 0) { --scv_gather_minerals_delay; }

std::cout << "exec_action: 48" << std::endl;
    if (! kurt->scv_idle.empty()) {
std::cout << "exec_action: 49" << std::endl;
        Unit const * scv = kurt->scv_idle.front();
std::cout << "exec_action: 50" << std::endl;
        Unit const * target = FindNextMineralField(kurt->Observation());
std::cout << "exec_action: 51" << std::endl;
        if (target != nullptr) {
std::cout << "exec_action: 52" << std::endl;
            if (! kurt->UnitInScvMinerals(scv)) {
std::cout << "exec_action: 53" << std::endl;
                kurt->scv_minerals.push_back(scv);
std::cout << "exec_action: 54" << std::endl;
            }
std::cout << "exec_action: 55" << std::endl;
        } else {
std::cout << "exec_action: 56" << std::endl;
            target = FindNextRefinery(kurt->Observation());
std::cout << "exec_action: 57" << std::endl;
            if (target != nullptr) {
std::cout << "exec_action: 58" << std::endl;
                if (! kurt->UnitInScvVespene(scv)) {
std::cout << "exec_action: 59" << std::endl;
                    kurt->scv_vespene.push_back(scv);
std::cout << "exec_action: 60" << std::endl;
                }
std::cout << "exec_action: 61" << std::endl;
            }
std::cout << "exec_action: 62" << std::endl;
        }
std::cout << "exec_action: 63" << std::endl;
        if (target == nullptr) {
std::cout << "exec_action: 64" << std::endl;
            return;
std::cout << "exec_action: 65" << std::endl;
        }
std::cout << "exec_action: 66" << std::endl;
        kurt->scv_idle.pop_front();
std::cout << "exec_action: 67" << std::endl;
        kurt->scv_minerals.push_back(scv);
std::cout << "exec_action: 68" << std::endl;
        kurt->Actions()->UnitCommand(scv, ABILITY_ID::SMART, target);
std::cout << "exec_action: 69" << std::endl;
    }
}

bool IsVespeneGeyser(Unit const & unit) {
std::cout << "exec_action: 73" << std::endl;
    return unit.unit_type == UNIT_TYPEID::NEUTRAL_VESPENEGEYSER;
};

bool IsMineralField(Unit const & unit) {
std::cout << "exec_action: 77" << std::endl;
    return
        unit.unit_type == UNIT_TYPEID::NEUTRAL_MINERALFIELD ||
        unit.unit_type == UNIT_TYPEID::NEUTRAL_MINERALFIELD750 ||
        unit.unit_type == UNIT_TYPEID::NEUTRAL_RICHMINERALFIELD ||
        unit.unit_type == UNIT_TYPEID::NEUTRAL_RICHMINERALFIELD750;
};

bool IsSCV(Unit const & unit) {
std::cout << "exec_action: 85" << std::endl;
    return unit.unit_type == UNIT_TYPEID::TERRAN_SCV;
};

bool IsRefinery(Unit const & unit) {
std::cout << "exec_action: 89" << std::endl;
    return unit.unit_type == UNIT_TYPEID::TERRAN_REFINERY;
};

bool IsCommandcenter(Unit const & unit) {
std::cout << "exec_action: 93" << std::endl;
    return
        unit.unit_type == UNIT_TYPEID::TERRAN_COMMANDCENTER ||
        unit.unit_type == UNIT_TYPEID::TERRAN_ORBITALCOMMAND ||
        unit.unit_type == UNIT_TYPEID::TERRAN_PLANETARYFORTRESS;
};

void ExecAction::OnUnitIdle(
        Unit const * unit, Kurt * kurt) {
std::cout << "exec_action: 101" << std::endl;
    Unit const * target;
std::cout << "exec_action: 102" << std::endl;
    switch (unit->unit_type.ToType()) {
std::cout << "exec_action: 103" << std::endl;
    case UNIT_TYPEID::TERRAN_SCV:
std::cout << "exec_action: 104" << std::endl;
        kurt->scv_building.remove(unit);
std::cout << "exec_action: 105" << std::endl;
        kurt->scv_idle.remove(unit);
std::cout << "exec_action: 106" << std::endl;
        kurt->scv_minerals.remove(unit);
std::cout << "exec_action: 107" << std::endl;
        kurt->scv_vespene.remove(unit);
std::cout << "exec_action: 108" << std::endl;
        target = FindNextMineralField(kurt->Observation());
std::cout << "exec_action: 109" << std::endl;
        if (target == nullptr) {
std::cout << "exec_action: 110" << std::endl;
            if (! kurt->UnitInList(kurt->scv_idle, unit)) {
std::cout << "exec_action: 111" << std::endl;
                kurt->scv_idle.push_back(unit);
std::cout << "exec_action: 112" << std::endl;
            }
std::cout << "exec_action: 113" << std::endl;
        } else {
std::cout << "exec_action: 114" << std::endl;
            kurt->Actions()->UnitCommand(unit, ABILITY_ID::SMART, target);
std::cout << "exec_action: 115" << std::endl;
            if (! kurt->UnitInScvMinerals(unit)) {
std::cout << "exec_action: 116" << std::endl;
                kurt->scv_minerals.push_back(unit);
std::cout << "exec_action: 117" << std::endl;
            }
std::cout << "exec_action: 118" << std::endl;
        }
std::cout << "exec_action: 119" << std::endl;
        break;
std::cout << "exec_action: 120" << std::endl;
    default:
std::cout << "exec_action: 121" << std::endl;
        break;
std::cout << "exec_action: 122" << std::endl;
    }
}

bool ExecAction::Exec(Kurt * const kurt, ACTION action) {
std::cout << "exec_action: 126" << std::endl;
    Units us;
std::cout << "exec_action: 127" << std::endl;
    Unit const * target;
std::cout << "exec_action: 128" << std::endl;
    ActionInterface * action_interface = kurt->Actions();
std::cout << "exec_action: 129" << std::endl;
    QueryInterface *query = kurt->Query();
std::cout << "exec_action: 130" << std::endl;
    ObservationInterface const *obs = kurt->Observation();
std::cout << "exec_action: 131" << std::endl;
    switch (action) {
std::cout << "exec_action: 132" << std::endl;
    default:
std::cout << "exec_action: 133" << std::endl;
        //
std::cout << "exec_action: 134" << std::endl;
        // Test if action can be represented by some ability from the api.
std::cout << "exec_action: 135" << std::endl;
        //
std::cout << "exec_action: 136" << std::endl;
        if (ActionRepr::convert_our_api.count(action) != 0) {
std::cout << "exec_action: 137" << std::endl;
            ABILITY_ID ability = ActionRepr::convert_our_api.at(action);
std::cout << "exec_action: 138" << std::endl;
            return ExecAbility(kurt, ability);
std::cout << "exec_action: 139" << std::endl;
        }
        else {
std::cout << "exec_action: 141" << std::endl;
            std::cout << "Error: exec_action: No case for action: " << ActionToName(action) << std::endl;
std::cout << "exec_action: 142" << std::endl;
            return false;
std::cout << "exec_action: 143" << std::endl;
        }
std::cout << "exec_action: 144" << std::endl;
    case ACTION::BUILD_BARRACKS_REACTOR:
std::cout << "exec_action: 145" << std::endl;
        for (auto it = reactor_builders.begin(); it != reactor_builders.end(); ++it) {
std::cout << "exec_action: 146" << std::endl;
            Tag t = *it;
std::cout << "exec_action: 147" << std::endl;
            Unit const *u;
std::cout << "exec_action: 148" << std::endl;
            if ((u = obs->GetUnit(t))->unit_type.ToType() == UNIT_TYPEID::TERRAN_BARRACKSFLYING) {
std::cout << "exec_action: 149" << std::endl;
                Point2D target_point;
std::cout << "exec_action: 150" << std::endl;
                do {
std::cout << "exec_action: 151" << std::endl;
                    target_point = { u->pos.x + GetRandomScalar() * 15, u->pos.y + GetRandomScalar() * 15 };
std::cout << "exec_action: 152" << std::endl;
                } while (!query->Placement(ABILITY_ID::BUILD_REACTOR_BARRACKS, u->pos, u));
std::cout << "exec_action: 153" << std::endl;
                reactor_builders.erase(it);
std::cout << "exec_action: 154" << std::endl;
                return true;
std::cout << "exec_action: 155" << std::endl;
            }
std::cout << "exec_action: 156" << std::endl;
        }
std::cout << "exec_action: 157" << std::endl;
        for (Unit const *u : obs->GetUnits(Unit::Alliance::Self, [](Unit const &u) {return u.unit_type.ToType() == UNIT_TYPEID::TERRAN_BARRACKS; })) {
std::cout << "exec_action: 158" << std::endl;
            vector<AvailableAbility> aas = query->GetAbilitiesForUnit(u, false).abilities;
std::cout << "exec_action: 159" << std::endl;
            if (std::any_of(aas.begin(), aas.end(), [](AvailableAbility a) {return a.ability_id.ToType() == ABILITY_ID::BUILD_REACTOR; })) {
std::cout << "exec_action: 160" << std::endl;
                if (query->Placement(ABILITY_ID::BUILD_REACTOR_BARRACKS, u->pos, u)) {
std::cout << "exec_action: 161" << std::endl;
                    action_interface->UnitCommand(u, ABILITY_ID::BUILD_REACTOR_BARRACKS, true);
std::cout << "exec_action: 162" << std::endl;
                    return true;
std::cout << "exec_action: 163" << std::endl;
                }
                else {
std::cout << "exec_action: 165" << std::endl;
                    action_interface->UnitCommand(u, ABILITY_ID::LIFT);
std::cout << "exec_action: 166" << std::endl;
                    reactor_builders.push_back(u->tag);
std::cout << "exec_action: 167" << std::endl;
                    return false;
std::cout << "exec_action: 168" << std::endl;
                }
std::cout << "exec_action: 169" << std::endl;
            }
std::cout << "exec_action: 170" << std::endl;
        }
std::cout << "exec_action: 171" << std::endl;
        return false;
std::cout << "exec_action: 172" << std::endl;
    case ACTION::BUILD_BARRACKS_TECH_LAB:
std::cout << "exec_action: 173" << std::endl;
        for (auto it = techlab_builders.begin(); it != techlab_builders.end(); ++it) {
std::cout << "exec_action: 174" << std::endl;
            Tag t = *it;
std::cout << "exec_action: 175" << std::endl;
            Unit const *u;
std::cout << "exec_action: 176" << std::endl;
            if ((u = obs->GetUnit(t))->unit_type.ToType() == UNIT_TYPEID::TERRAN_BARRACKSFLYING) {
std::cout << "exec_action: 177" << std::endl;
                Point2D target_point;
std::cout << "exec_action: 178" << std::endl;
                do {
std::cout << "exec_action: 179" << std::endl;
                    target_point = { u->pos.x + GetRandomScalar() * 15, u->pos.y + GetRandomScalar() * 15 };
std::cout << "exec_action: 180" << std::endl;
                } while (!query->Placement(ABILITY_ID::BUILD_TECHLAB_BARRACKS, u->pos, u));
std::cout << "exec_action: 181" << std::endl;
                techlab_builders.erase(it);
std::cout << "exec_action: 182" << std::endl;
                return true;
std::cout << "exec_action: 183" << std::endl;
            }
std::cout << "exec_action: 184" << std::endl;
        }
std::cout << "exec_action: 185" << std::endl;
        for (Unit const *u : obs->GetUnits(Unit::Alliance::Self, [](Unit const &u) {return u.unit_type.ToType() == UNIT_TYPEID::TERRAN_BARRACKS; })) {
std::cout << "exec_action: 186" << std::endl;
            vector<AvailableAbility> aas = query->GetAbilitiesForUnit(u, false).abilities;
std::cout << "exec_action: 187" << std::endl;
            if (std::any_of(aas.begin(), aas.end(), [](AvailableAbility a) {return a.ability_id.ToType() == ABILITY_ID::BUILD_TECHLAB; })) {
std::cout << "exec_action: 188" << std::endl;
                if (query->Placement(ABILITY_ID::BUILD_TECHLAB_BARRACKS, u->pos, u)) {
std::cout << "exec_action: 189" << std::endl;
                    action_interface->UnitCommand(u, ABILITY_ID::BUILD_TECHLAB_BARRACKS, true);
std::cout << "exec_action: 190" << std::endl;
                    return true;
std::cout << "exec_action: 191" << std::endl;
                }
                else {
std::cout << "exec_action: 193" << std::endl;
                    action_interface->UnitCommand(u, ABILITY_ID::LIFT);
std::cout << "exec_action: 194" << std::endl;
                    techlab_builders.push_back(u->tag);
std::cout << "exec_action: 195" << std::endl;
                    return false;
std::cout << "exec_action: 196" << std::endl;
                }
std::cout << "exec_action: 197" << std::endl;
            }
std::cout << "exec_action: 198" << std::endl;
        }
std::cout << "exec_action: 199" << std::endl;
        return false;
std::cout << "exec_action: 200" << std::endl;
    case ACTION::BUILD_FACTORY_REACTOR:
std::cout << "exec_action: 201" << std::endl;
            for (auto it = reactor_builders.begin(); it != reactor_builders.end(); ++it) {
std::cout << "exec_action: 202" << std::endl;
                Tag t = *it;
std::cout << "exec_action: 203" << std::endl;
                Unit const *u;
std::cout << "exec_action: 204" << std::endl;
                if ((u = obs->GetUnit(t))->unit_type.ToType() == UNIT_TYPEID::TERRAN_FACTORYFLYING) {
std::cout << "exec_action: 205" << std::endl;
                    Point2D target_point;
std::cout << "exec_action: 206" << std::endl;
                    do {
std::cout << "exec_action: 207" << std::endl;
                        target_point = { u->pos.x + GetRandomScalar() * 15, u->pos.y + GetRandomScalar() * 15 };
std::cout << "exec_action: 208" << std::endl;
                    } while (!query->Placement(ABILITY_ID::BUILD_REACTOR_FACTORY, u->pos, u));
std::cout << "exec_action: 209" << std::endl;
                    reactor_builders.erase(it);
std::cout << "exec_action: 210" << std::endl;
                    return true;
std::cout << "exec_action: 211" << std::endl;
                }
std::cout << "exec_action: 212" << std::endl;
            }
std::cout << "exec_action: 213" << std::endl;
            for (Unit const *u : obs->GetUnits(Unit::Alliance::Self, [](Unit const &u) {return u.unit_type.ToType() == UNIT_TYPEID::TERRAN_FACTORY; })) {
std::cout << "exec_action: 214" << std::endl;
                vector<AvailableAbility> aas = query->GetAbilitiesForUnit(u, false).abilities;
std::cout << "exec_action: 215" << std::endl;
                if (std::any_of(aas.begin(), aas.end(), [](AvailableAbility a) {return a.ability_id.ToType() == ABILITY_ID::BUILD_REACTOR; })) {
std::cout << "exec_action: 216" << std::endl;
                    if (query->Placement(ABILITY_ID::BUILD_REACTOR_FACTORY, u->pos, u)) {
std::cout << "exec_action: 217" << std::endl;
                        action_interface->UnitCommand(u, ABILITY_ID::BUILD_REACTOR_FACTORY, true);
std::cout << "exec_action: 218" << std::endl;
                        return true;
std::cout << "exec_action: 219" << std::endl;
                    }
                    else {
std::cout << "exec_action: 221" << std::endl;
                        action_interface->UnitCommand(u, ABILITY_ID::LIFT);
std::cout << "exec_action: 222" << std::endl;
                        reactor_builders.push_back(u->tag);
std::cout << "exec_action: 223" << std::endl;
                        return false;
std::cout << "exec_action: 224" << std::endl;
                    }
std::cout << "exec_action: 225" << std::endl;
                }
std::cout << "exec_action: 226" << std::endl;
            }
std::cout << "exec_action: 227" << std::endl;
            return false;
std::cout << "exec_action: 228" << std::endl;
        case ACTION::BUILD_FACTORY_TECH_LAB:
std::cout << "exec_action: 229" << std::endl;
            for (auto it = techlab_builders.begin(); it != techlab_builders.end(); ++it) {
std::cout << "exec_action: 230" << std::endl;
                Tag t = *it;
std::cout << "exec_action: 231" << std::endl;
                Unit const *u;
std::cout << "exec_action: 232" << std::endl;
                if ((u = obs->GetUnit(t))->unit_type.ToType() == UNIT_TYPEID::TERRAN_FACTORYFLYING) {
std::cout << "exec_action: 233" << std::endl;
                    Point2D target_point;
std::cout << "exec_action: 234" << std::endl;
                    do {
std::cout << "exec_action: 235" << std::endl;
                        target_point = { u->pos.x + GetRandomScalar() * 15, u->pos.y + GetRandomScalar() * 15 };
std::cout << "exec_action: 236" << std::endl;
                    } while (!query->Placement(ABILITY_ID::BUILD_TECHLAB_FACTORY, u->pos, u));
std::cout << "exec_action: 237" << std::endl;
                    techlab_builders.erase(it);
std::cout << "exec_action: 238" << std::endl;
                    return true;
std::cout << "exec_action: 239" << std::endl;
                }
std::cout << "exec_action: 240" << std::endl;
            }
std::cout << "exec_action: 241" << std::endl;
            for (Unit const *u : obs->GetUnits(Unit::Alliance::Self, [](Unit const &u) {return u.unit_type.ToType() == UNIT_TYPEID::TERRAN_FACTORY; })) {
std::cout << "exec_action: 242" << std::endl;
                vector<AvailableAbility> aas = query->GetAbilitiesForUnit(u, false).abilities;
std::cout << "exec_action: 243" << std::endl;
                if (std::any_of(aas.begin(), aas.end(), [](AvailableAbility a) {return a.ability_id.ToType() == ABILITY_ID::BUILD_TECHLAB; })) {
std::cout << "exec_action: 244" << std::endl;
                    if (query->Placement(ABILITY_ID::BUILD_TECHLAB_FACTORY, u->pos, u)) {
std::cout << "exec_action: 245" << std::endl;
                        action_interface->UnitCommand(u, ABILITY_ID::BUILD_TECHLAB_FACTORY, true);
std::cout << "exec_action: 246" << std::endl;
                        return true;
std::cout << "exec_action: 247" << std::endl;
                    }
                    else {
std::cout << "exec_action: 249" << std::endl;
                        action_interface->UnitCommand(u, ABILITY_ID::LIFT);
std::cout << "exec_action: 250" << std::endl;
                        techlab_builders.push_back(u->tag);
std::cout << "exec_action: 251" << std::endl;
                        return false;
std::cout << "exec_action: 252" << std::endl;
                    }
std::cout << "exec_action: 253" << std::endl;
                }
std::cout << "exec_action: 254" << std::endl;
            }
std::cout << "exec_action: 255" << std::endl;
            return false;
std::cout << "exec_action: 256" << std::endl;
    case ACTION::BUILD_STARPORT_REACTOR:
std::cout << "exec_action: 257" << std::endl;
        for (auto it = reactor_builders.begin(); it != reactor_builders.end(); ++it) {
std::cout << "exec_action: 258" << std::endl;
            Tag t = *it;
std::cout << "exec_action: 259" << std::endl;
            Unit const *u;
std::cout << "exec_action: 260" << std::endl;
            if ((u = obs->GetUnit(t))->unit_type.ToType() == UNIT_TYPEID::TERRAN_STARPORTFLYING) {
std::cout << "exec_action: 261" << std::endl;
                Point2D target_point;
std::cout << "exec_action: 262" << std::endl;
                do {
std::cout << "exec_action: 263" << std::endl;
                    target_point = { u->pos.x + GetRandomScalar() * 15, u->pos.y + GetRandomScalar() * 15 };
std::cout << "exec_action: 264" << std::endl;
                } while (!query->Placement(ABILITY_ID::BUILD_REACTOR_STARPORT, u->pos, u));
std::cout << "exec_action: 265" << std::endl;
                reactor_builders.erase(it);
std::cout << "exec_action: 266" << std::endl;
                return true;
std::cout << "exec_action: 267" << std::endl;
            }
std::cout << "exec_action: 268" << std::endl;
        }
std::cout << "exec_action: 269" << std::endl;
        for (Unit const *u : obs->GetUnits(Unit::Alliance::Self, [](Unit const &u) {return u.unit_type.ToType() == UNIT_TYPEID::TERRAN_STARPORT; })) {
std::cout << "exec_action: 270" << std::endl;
            vector<AvailableAbility> aas = query->GetAbilitiesForUnit(u, false).abilities;
std::cout << "exec_action: 271" << std::endl;
            if (std::any_of(aas.begin(), aas.end(), [](AvailableAbility a) {return a.ability_id.ToType() == ABILITY_ID::BUILD_REACTOR; })) {
std::cout << "exec_action: 272" << std::endl;
                if (query->Placement(ABILITY_ID::BUILD_REACTOR_STARPORT, u->pos, u)) {
std::cout << "exec_action: 273" << std::endl;
                    action_interface->UnitCommand(u, ABILITY_ID::BUILD_REACTOR_STARPORT, true);
std::cout << "exec_action: 274" << std::endl;
                    return true;
std::cout << "exec_action: 275" << std::endl;
                }
                else {
std::cout << "exec_action: 277" << std::endl;
                    action_interface->UnitCommand(u, ABILITY_ID::LIFT);
std::cout << "exec_action: 278" << std::endl;
                    reactor_builders.push_back(u->tag);
std::cout << "exec_action: 279" << std::endl;
                    return false;
std::cout << "exec_action: 280" << std::endl;
                }
std::cout << "exec_action: 281" << std::endl;
            }
std::cout << "exec_action: 282" << std::endl;
        }
std::cout << "exec_action: 283" << std::endl;
        return false;
std::cout << "exec_action: 284" << std::endl;
    case ACTION::BUILD_STARPORT_TECH_LAB:
std::cout << "exec_action: 285" << std::endl;
        for (auto it = techlab_builders.begin(); it != techlab_builders.end(); ++it) {
std::cout << "exec_action: 286" << std::endl;
            Tag t = *it;
std::cout << "exec_action: 287" << std::endl;
            Unit const *u;
std::cout << "exec_action: 288" << std::endl;
            if ((u = obs->GetUnit(t))->unit_type.ToType() == UNIT_TYPEID::TERRAN_STARPORTFLYING) {
std::cout << "exec_action: 289" << std::endl;
                Point2D target_point;
std::cout << "exec_action: 290" << std::endl;
                do {
std::cout << "exec_action: 291" << std::endl;
                    target_point = { u->pos.x + GetRandomScalar() * 15, u->pos.y + GetRandomScalar() * 15};
std::cout << "exec_action: 292" << std::endl;
                } while (!query->Placement(ABILITY_ID::BUILD_TECHLAB_STARPORT, u->pos, u));
std::cout << "exec_action: 293" << std::endl;
                techlab_builders.erase(it);
std::cout << "exec_action: 294" << std::endl;
                return true;
std::cout << "exec_action: 295" << std::endl;
            }
std::cout << "exec_action: 296" << std::endl;
        }
std::cout << "exec_action: 297" << std::endl;
        for (Unit const *u : obs->GetUnits(Unit::Alliance::Self, [](Unit const &u) {return u.unit_type.ToType() == UNIT_TYPEID::TERRAN_STARPORT; })) {
std::cout << "exec_action: 298" << std::endl;
            vector<AvailableAbility> aas = query->GetAbilitiesForUnit(u, false).abilities;
std::cout << "exec_action: 299" << std::endl;
            if (std::any_of(aas.begin(), aas.end(), [](AvailableAbility a) {return a.ability_id.ToType() == ABILITY_ID::BUILD_TECHLAB; })) {
std::cout << "exec_action: 300" << std::endl;
                if (query->Placement(ABILITY_ID::BUILD_TECHLAB_STARPORT, u->pos, u)) {
std::cout << "exec_action: 301" << std::endl;
                    action_interface->UnitCommand(u, ABILITY_ID::BUILD_TECHLAB_STARPORT, true);
std::cout << "exec_action: 302" << std::endl;
                    return true;
std::cout << "exec_action: 303" << std::endl;
                } else {
std::cout << "exec_action: 304" << std::endl;
                    action_interface->UnitCommand(u, ABILITY_ID::LIFT);
std::cout << "exec_action: 305" << std::endl;
                    techlab_builders.push_back(u->tag);
std::cout << "exec_action: 306" << std::endl;
                    return false;
std::cout << "exec_action: 307" << std::endl;
                }
std::cout << "exec_action: 308" << std::endl;
            }
std::cout << "exec_action: 309" << std::endl;
        }
std::cout << "exec_action: 310" << std::endl;
        return false;
std::cout << "exec_action: 311" << std::endl;
    case ACTION::SCV_GATHER_MINERALS:
std::cout << "exec_action: 312" << std::endl;
        // Make an SCV stop gather vespene and start gather minerals
std::cout << "exec_action: 313" << std::endl;
        if (scv_gather_minerals_delay > 0) {
std::cout << "exec_action: 314" << std::endl;
            return false;
std::cout << "exec_action: 315" << std::endl;
        }
std::cout << "exec_action: 316" << std::endl;
        target = FindNextMineralField(obs);
std::cout << "exec_action: 317" << std::endl;
        if (target == nullptr) {
std::cout << "exec_action: 318" << std::endl;
            return false;
std::cout << "exec_action: 319" << std::endl;
        }
std::cout << "exec_action: 320" << std::endl;
        us = obs->GetUnits(Unit::Alliance::Self, IsSCV);
std::cout << "exec_action: 321" << std::endl;
        for (Unit const * scv : us) {
std::cout << "exec_action: 322" << std::endl;
            if (!kurt->UnitInScvVespene(scv)) {
std::cout << "exec_action: 323" << std::endl;
                continue;
std::cout << "exec_action: 324" << std::endl;
            }
std::cout << "exec_action: 325" << std::endl;
            if (!scv->orders.empty() &&
                scv->orders[0].ability_id.ToType() == ABILITY_ID::BUILD_REFINERY) {
std::cout << "exec_action: 327" << std::endl;
                continue;
std::cout << "exec_action: 328" << std::endl;
            }
std::cout << "exec_action: 329" << std::endl;
            action_interface->UnitCommand(scv, ABILITY_ID::SMART, target);
std::cout << "exec_action: 330" << std::endl;
            kurt->scv_vespene.remove(scv);
std::cout << "exec_action: 331" << std::endl;
            kurt->scv_minerals.push_back(scv);
std::cout << "exec_action: 332" << std::endl;
            scv_gather_minerals_delay = 1 * STEPS_PER_SEC;
std::cout << "exec_action: 333" << std::endl;
            return true;
std::cout << "exec_action: 334" << std::endl;
        }
std::cout << "exec_action: 335" << std::endl;
        return false;
std::cout << "exec_action: 336" << std::endl;
    case ACTION::SCV_GATHER_VESPENE:
std::cout << "exec_action: 337" << std::endl;
        // Make an SCV stop gather minerals and start gather vespene
std::cout << "exec_action: 338" << std::endl;
        target = FindNextRefinery(obs);
std::cout << "exec_action: 339" << std::endl;
        if (scv_gather_vespene_delay > 0) {
std::cout << "exec_action: 340" << std::endl;
            return false;
std::cout << "exec_action: 341" << std::endl;
        }
std::cout << "exec_action: 342" << std::endl;
        if (target == nullptr) {
std::cout << "exec_action: 343" << std::endl;
            return false;
std::cout << "exec_action: 344" << std::endl;
        }
std::cout << "exec_action: 345" << std::endl;
        us = obs->GetUnits(Unit::Alliance::Self, IsSCV);
std::cout << "exec_action: 346" << std::endl;
        for (Unit const * scv : us) {
std::cout << "exec_action: 347" << std::endl;
            if (!kurt->UnitInScvMinerals(scv)) {
std::cout << "exec_action: 348" << std::endl;
                continue;
std::cout << "exec_action: 349" << std::endl;
            }
std::cout << "exec_action: 350" << std::endl;
            action_interface->UnitCommand(scv, ABILITY_ID::SMART, target);
std::cout << "exec_action: 351" << std::endl;
            kurt->scv_vespene.push_back(scv);
std::cout << "exec_action: 352" << std::endl;
            kurt->scv_minerals.remove(scv);
std::cout << "exec_action: 353" << std::endl;
            scv_gather_vespene_delay = 1 * STEPS_PER_SEC;

std::cout << "exec_action: 355" << std::endl;
            return TEST(built_a_tech_lab = )true;
std::cout << "exec_action: 356" << std::endl;
        }
std::cout << "exec_action: 357" << std::endl;
        return false;
std::cout << "exec_action: 358" << std::endl;
    }
}

bool ExecAction::ExecAbility(Kurt * const kurt, ABILITY_ID ability) {
std::cout << "exec_action: 362" << std::endl;
    ActionInterface * action_interface = kurt->Actions();
std::cout << "exec_action: 363" << std::endl;
    QueryInterface *query = kurt->Query();
std::cout << "exec_action: 364" << std::endl;
    ObservationInterface const *obs = kurt->Observation();

std::cout << "exec_action: 366" << std::endl;
    auto is_idle_or_scv = [obs](Unit const &unit) {
std::cout << "exec_action: 367" << std::endl;
        if (unit.unit_type.ToType() == UNIT_TYPEID::TERRAN_SCV) return true;
std::cout << "exec_action: 368" << std::endl;
        Unit const *addon_unit = obs->GetUnit(unit.add_on_tag);
std::cout << "exec_action: 369" << std::endl;
        if (addon_unit && addon_unit->build_progress >= 1) {
std::cout << "exec_action: 370" << std::endl;
            switch (addon_unit->unit_type.ToType()) {
std::cout << "exec_action: 371" << std::endl;
            case UNIT_TYPEID::TERRAN_REACTOR:
std::cout << "exec_action: 372" << std::endl;
            case UNIT_TYPEID::TERRAN_BARRACKSREACTOR:
std::cout << "exec_action: 373" << std::endl;
            case UNIT_TYPEID::TERRAN_FACTORYREACTOR:
std::cout << "exec_action: 374" << std::endl;
            case UNIT_TYPEID::TERRAN_STARPORTREACTOR:
std::cout << "exec_action: 375" << std::endl;
                return unit.orders.size() <= 1;
std::cout << "exec_action: 376" << std::endl;
            }
std::cout << "exec_action: 377" << std::endl;
        }
std::cout << "exec_action: 378" << std::endl;
        return unit.orders.empty();
std::cout << "exec_action: 379" << std::endl;
    };

std::cout << "exec_action: 381" << std::endl;
    for (const Unit *u : obs->GetUnits(Unit::Alliance::Self, is_idle_or_scv)) {
std::cout << "exec_action: 382" << std::endl;
        if (ExecAbility(kurt, ability, u)) return true;
std::cout << "exec_action: 383" << std::endl;
    }
std::cout << "exec_action: 384" << std::endl;
    return false;
}

bool ExecAction::ExecAbility(Kurt * const kurt, ABILITY_ID ability, Unit const *u) {
std::cout << "exec_action: 388" << std::endl;
    QueryInterface *query = kurt->Query();
std::cout << "exec_action: 389" << std::endl;
    ActionInterface *action_interface = kurt->Actions();
std::cout << "exec_action: 390" << std::endl;
    ObservationInterface const *obs = kurt->Observation();
std::cout << "exec_action: 391" << std::endl;
    if (u->build_progress < 1) {
std::cout << "exec_action: 392" << std::endl;
        return false; // Unit under construction.
std::cout << "exec_action: 393" << std::endl;
    }
std::cout << "exec_action: 394" << std::endl;
    for (AvailableAbility order : query->GetAbilitiesForUnit(u, true).abilities) {
std::cout << "exec_action: 395" << std::endl;
        if (order.ability_id != ability) {
std::cout << "exec_action: 396" << std::endl;
            continue;
std::cout << "exec_action: 397" << std::endl;
        }
std::cout << "exec_action: 398" << std::endl;
        if (u->unit_type.ToType() == UNIT_TYPEID::TERRAN_SCV &&
            !kurt->UnitInScvMinerals(u)) {
std::cout << "exec_action: 400" << std::endl;
            continue;
std::cout << "exec_action: 401" << std::endl;
        }

std::cout << "exec_action: 403" << std::endl;
        bool can_afford_it = false;
std::cout << "exec_action: 404" << std::endl;
        for (AvailableAbility affordable : query->GetAbilitiesForUnit(u, false).abilities) {
std::cout << "exec_action: 405" << std::endl;
            if (affordable.ability_id == ability) {
std::cout << "exec_action: 406" << std::endl;
                can_afford_it = true;
std::cout << "exec_action: 407" << std::endl;
                break;
std::cout << "exec_action: 408" << std::endl;
            }
std::cout << "exec_action: 409" << std::endl;
        }

std::cout << "exec_action: 411" << std::endl;
        if (!can_afford_it) {
std::cout << "exec_action: 412" << std::endl;
            continue;
std::cout << "exec_action: 413" << std::endl;
        }
std::cout << "exec_action: 414" << std::endl;
        Point2D target_point(u->pos.x + GetRandomScalar() * 15
            , u->pos.y + GetRandomScalar() * 15);
std::cout << "exec_action: 416" << std::endl;
        Unit const *target_unit;
std::cout << "exec_action: 417" << std::endl;
        switch (Kurt::GetAbility(ability)->target) {
std::cout << "exec_action: 418" << std::endl;
        case sc2::AbilityData::Target::None:
std::cout << "exec_action: 419" << std::endl;
            action_interface->UnitCommand(u, ability, true);
std::cout << "exec_action: 420" << std::endl;
            break;
std::cout << "exec_action: 421" << std::endl;
        case sc2::AbilityData::Target::Point:
std::cout << "exec_action: 422" << std::endl;
            switch (ability) {
std::cout << "exec_action: 423" << std::endl;
            case ABILITY_ID::BUILD_COMMANDCENTER: 
std::cout << "exec_action: 424" << std::endl;
                if (!FindNextCommandcenterLoc(obs, query, target_point)) {
std::cout << "exec_action: 425" << std::endl;
                    return false;
std::cout << "exec_action: 426" << std::endl;
                }
std::cout << "exec_action: 427" << std::endl;
                break;
std::cout << "exec_action: 428" << std::endl;
            default:
std::cout << "exec_action: 429" << std::endl;
                // Assume we have to place a unit.
std::cout << "exec_action: 430" << std::endl;
                while (!query->Placement(ability, target_point, u)) {
std::cout << "exec_action: 431" << std::endl;
                    target_point = Point2D(u->pos.x + GetRandomScalar() * 15
                        , u->pos.y + GetRandomScalar() * 15);
std::cout << "exec_action: 433" << std::endl;
                }
std::cout << "exec_action: 434" << std::endl;
                break;
std::cout << "exec_action: 435" << std::endl;
                
std::cout << "exec_action: 436" << std::endl;
            }
std::cout << "exec_action: 437" << std::endl;
            action_interface->UnitCommand(u, ability, target_point);
std::cout << "exec_action: 438" << std::endl;
            break;
std::cout << "exec_action: 439" << std::endl;
        case sc2::AbilityData::Target::Unit:
std::cout << "exec_action: 440" << std::endl;
            if (ability == ABILITY_ID::BUILD_REFINERY) {
std::cout << "exec_action: 441" << std::endl;
                target_unit = FindNextVespeneGeyser(obs);
std::cout << "exec_action: 442" << std::endl;
                if (target_unit == nullptr) {
std::cout << "exec_action: 443" << std::endl;
                    return false;
std::cout << "exec_action: 444" << std::endl;
                }
std::cout << "exec_action: 445" << std::endl;
                built_refinery_time[target_unit] = obs->GetGameLoop();
std::cout << "exec_action: 446" << std::endl;
            }

std::cout << "exec_action: 448" << std::endl;
            action_interface->UnitCommand(u, ability, target_unit);
std::cout << "exec_action: 449" << std::endl;
            break;
std::cout << "exec_action: 450" << std::endl;
        case sc2::AbilityData::Target::PointOrNone:

std::cout << "exec_action: 452" << std::endl;
            action_interface->UnitCommand(u, ability);
std::cout << "exec_action: 453" << std::endl;
            // TODO: Maybe target someplace?
std::cout << "exec_action: 454" << std::endl;
            break;
std::cout << "exec_action: 455" << std::endl;
        case sc2::AbilityData::Target::PointOrUnit:
std::cout << "exec_action: 456" << std::endl;
            action_interface->UnitCommand(u, ability, target_point);
std::cout << "exec_action: 457" << std::endl;
            // TODO: Where or who?

std::cout << "exec_action: 459" << std::endl;
            std::cout << "Warning: exec_action: PointOrUnit, ability: "
                << AbilityTypeToName(AbilityID(ability)) << std::endl;
std::cout << "exec_action: 461" << std::endl;
            break;
std::cout << "exec_action: 462" << std::endl;
        default:
std::cout << "exec_action: 463" << std::endl;
            // No
std::cout << "exec_action: 464" << std::endl;
            std::cout << "Error: exec_action: Invalid target type!!" << std::endl;
std::cout << "exec_action: 465" << std::endl;
            throw std::runtime_error("Build planner - ability had invalid targeting method");
std::cout << "exec_action: 466" << std::endl;
        }
std::cout << "exec_action: 467" << std::endl;
        /*Point2D pt = Point2D(u->pos.x, u->pos.y);
std::cout << "exec_action: 468" << std::endl;
        action->UnitCommand(u, ability, pt);*/
std::cout << "exec_action: 469" << std::endl;
        if (u->unit_type.ToType() == UNIT_TYPEID::TERRAN_SCV) {
std::cout << "exec_action: 470" << std::endl;
            kurt->scv_minerals.remove(u);
std::cout << "exec_action: 471" << std::endl;
            if (ability == ABILITY_ID::BUILD_REFINERY) {
std::cout << "exec_action: 472" << std::endl;
                kurt->scv_vespene.push_back(u);
std::cout << "exec_action: 473" << std::endl;
            }
            else {
std::cout << "exec_action: 475" << std::endl;
                kurt->scv_building.push_back(u);
std::cout << "exec_action: 476" << std::endl;
            }
std::cout << "exec_action: 477" << std::endl;
        }
std::cout << "exec_action: 478" << std::endl;
        sent_order_time[u] = obs->GetGameLoop();
std::cout << "exec_action: 479" << std::endl;
        return true;
std::cout << "exec_action: 480" << std::endl;
    }

std::cout << "exec_action: 482" << std::endl;
    return false;
}

Unit const * ExecAction::FindNextVespeneGeyser(
        ObservationInterface const * obs) {
std::cout << "exec_action: 487" << std::endl;
    Units geysers = obs->GetUnits(Unit::Alliance::Neutral, IsVespeneGeyser);
std::cout << "exec_action: 488" << std::endl;
    Units refineries = obs->GetUnits(Unit::Alliance::Self, IsRefinery);
std::cout << "exec_action: 489" << std::endl;
    Units commandcenters = obs->GetUnits(Unit::Alliance::Self, IsCommandcenter);
std::cout << "exec_action: 490" << std::endl;
    for (Unit const * geyser : geysers) {
std::cout << "exec_action: 491" << std::endl;
        // Geyser isn't empty
std::cout << "exec_action: 492" << std::endl;
        if (geyser->vespene_contents <= 0) {
std::cout << "exec_action: 493" << std::endl;
            continue;
std::cout << "exec_action: 494" << std::endl;
        }
std::cout << "exec_action: 495" << std::endl;
        // We aren't already going to build an refinery at given geyser
std::cout << "exec_action: 496" << std::endl;
        if (built_refinery_time.count(geyser) != 0) {
std::cout << "exec_action: 497" << std::endl;
            if (obs->GetGameLoop() - built_refinery_time.at(geyser) <
                    10 * STEPS_PER_SEC) {
std::cout << "exec_action: 499" << std::endl;
                continue;
std::cout << "exec_action: 500" << std::endl;
            }
std::cout << "exec_action: 501" << std::endl;
        }
std::cout << "exec_action: 502" << std::endl;
        // The geyser is close to some of our commandcenters
std::cout << "exec_action: 503" << std::endl;
        bool in_range = false;
std::cout << "exec_action: 504" << std::endl;
        for (Unit const * commandcenter : commandcenters) {
std::cout << "exec_action: 505" << std::endl;
            if (DistanceSquared3D(geyser->pos, commandcenter->pos) <
                    BASE_RESOURCE_TEST_RANGE2) {
std::cout << "exec_action: 507" << std::endl;
                in_range = true;
std::cout << "exec_action: 508" << std::endl;
                break;
std::cout << "exec_action: 509" << std::endl;
            }
std::cout << "exec_action: 510" << std::endl;
        }
std::cout << "exec_action: 511" << std::endl;
        if (! in_range) {
std::cout << "exec_action: 512" << std::endl;
            continue;
std::cout << "exec_action: 513" << std::endl;
        }
std::cout << "exec_action: 514" << std::endl;
        // Refinery is not already built on given geyser
std::cout << "exec_action: 515" << std::endl;
        bool taken = false;
std::cout << "exec_action: 516" << std::endl;
        for (Unit const * refinery : refineries) {
std::cout << "exec_action: 517" << std::endl;
            if (geyser->pos.x == refinery->pos.x &&
                    geyser->pos.y == refinery->pos.y &&
                    geyser->pos.z == refinery->pos.z) {
std::cout << "exec_action: 520" << std::endl;
                taken = true;
std::cout << "exec_action: 521" << std::endl;
                break;
std::cout << "exec_action: 522" << std::endl;
            }
std::cout << "exec_action: 523" << std::endl;
        }
std::cout << "exec_action: 524" << std::endl;
        if (taken) {
std::cout << "exec_action: 525" << std::endl;
            continue;
std::cout << "exec_action: 526" << std::endl;
        }
std::cout << "exec_action: 527" << std::endl;
        return geyser;
std::cout << "exec_action: 528" << std::endl;
    }
std::cout << "exec_action: 529" << std::endl;
    return nullptr;
}

Unit const * ExecAction::FindNextRefinery(
        ObservationInterface const * obs) {
std::cout << "exec_action: 534" << std::endl;
    Units refineries = obs->GetUnits(Unit::Alliance::Self, IsRefinery);
std::cout << "exec_action: 535" << std::endl;
    for (Unit const * refinery : refineries) {
std::cout << "exec_action: 536" << std::endl;
        // Refinery isn't built yet
std::cout << "exec_action: 537" << std::endl;
        if (refinery->build_progress < 1.0) {
std::cout << "exec_action: 538" << std::endl;
            continue;
std::cout << "exec_action: 539" << std::endl;
        }
std::cout << "exec_action: 540" << std::endl;
        // Refinery isn't overfull
std::cout << "exec_action: 541" << std::endl;
        if (refinery->assigned_harvesters >= refinery->ideal_harvesters) {
std::cout << "exec_action: 542" << std::endl;
            continue;
std::cout << "exec_action: 543" << std::endl;
        }
std::cout << "exec_action: 544" << std::endl;
        return refinery;
std::cout << "exec_action: 545" << std::endl;
    }
std::cout << "exec_action: 546" << std::endl;
    return nullptr;
}

Unit const * ExecAction::FindNextMineralField(
        ObservationInterface const * obs) {
std::cout << "exec_action: 551" << std::endl;
    Units commandcenters = obs->GetUnits(Unit::Alliance::Self, IsCommandcenter);
std::cout << "exec_action: 552" << std::endl;
    Units mineral_fields = obs->GetUnits(Unit::Alliance::Neutral, IsMineralField);
std::cout << "exec_action: 553" << std::endl;
    for (Unit const * commandcenter : commandcenters) {
std::cout << "exec_action: 554" << std::endl;
        // Commandcenter isn't built yet
std::cout << "exec_action: 555" << std::endl;
        if (commandcenter->build_progress < 1.0) {
std::cout << "exec_action: 556" << std::endl;
            continue;
std::cout << "exec_action: 557" << std::endl;
        }
std::cout << "exec_action: 558" << std::endl;
        // Commandcenter isn't overfull
std::cout << "exec_action: 559" << std::endl;
        if (commandcenter->assigned_harvesters >= commandcenter->ideal_harvesters) {
std::cout << "exec_action: 560" << std::endl;
            continue;
std::cout << "exec_action: 561" << std::endl;
        }
std::cout << "exec_action: 562" << std::endl;
        // Find a mineral field near given good commandcenter
std::cout << "exec_action: 563" << std::endl;
        for (Unit const * mineral_field : mineral_fields) {
std::cout << "exec_action: 564" << std::endl;
            if (DistanceSquared3D(commandcenter->pos, mineral_field->pos) <
                    BASE_RESOURCE_TEST_RANGE2) {
std::cout << "exec_action: 566" << std::endl;
                return mineral_field;
std::cout << "exec_action: 567" << std::endl;
            }
std::cout << "exec_action: 568" << std::endl;
        }
std::cout << "exec_action: 569" << std::endl;
    }
std::cout << "exec_action: 570" << std::endl;
    return nullptr;
}

bool ExecAction::FindNextCommandcenterLoc(
        ObservationInterface const * obs,
        QueryInterface * query,
        Point2D & location) {
std::cout << "exec_action: 577" << std::endl;
    Units commandcenters = obs->GetUnits(IsCommandcenter);
std::cout << "exec_action: 578" << std::endl;
    std::vector<int> order(commandcenter_locations.size());
std::cout << "exec_action: 579" << std::endl;
    for (int i = 0; i < commandcenter_locations.size(); ++i) {
std::cout << "exec_action: 580" << std::endl;
        order[i] = i;
std::cout << "exec_action: 581" << std::endl;
    }
std::cout << "exec_action: 582" << std::endl;
    for (int i = 0; i < commandcenter_locations.size(); ++i) {
std::cout << "exec_action: 583" << std::endl;
        Point3D point3D = commandcenter_locations[order[i]];
std::cout << "exec_action: 584" << std::endl;
        bool taken = false;
std::cout << "exec_action: 585" << std::endl;
        for (Unit const * commandcenter : commandcenters) {
std::cout << "exec_action: 586" << std::endl;
            if (DistanceSquared3D(commandcenter->pos, point3D) < 0.1) {
std::cout << "exec_action: 587" << std::endl;
                taken = true;
std::cout << "exec_action: 588" << std::endl;
                break;
std::cout << "exec_action: 589" << std::endl;
            }
std::cout << "exec_action: 590" << std::endl;
        }
std::cout << "exec_action: 591" << std::endl;
        if (taken) {
std::cout << "exec_action: 592" << std::endl;
            continue;
std::cout << "exec_action: 593" << std::endl;
        }
std::cout << "exec_action: 594" << std::endl;
        Point2D point2D(point3D.x, point3D.y);
std::cout << "exec_action: 595" << std::endl;
        if (! query->Placement(ABILITY_ID::BUILD_COMMANDCENTER, point2D)) {
std::cout << "exec_action: 596" << std::endl;
            continue;
std::cout << "exec_action: 597" << std::endl;
        }
std::cout << "exec_action: 598" << std::endl;
        location.x = point2D.x;
std::cout << "exec_action: 599" << std::endl;
        location.y = point2D.y;
std::cout << "exec_action: 600" << std::endl;
        return true;
std::cout << "exec_action: 601" << std::endl;
    }
std::cout << "exec_action: 602" << std::endl;
    return false;
}

Unit const * ExecAction::FindNearestUnitOfType(
        UNIT_TYPEID type,
        Point2D const &location,
        ObservationInterface const *obs,
        Unit::Alliance alliance) {
std::cout << "exec_action: 610" << std::endl;
    Units candidates;
std::cout << "exec_action: 611" << std::endl;
    auto cmp = [type] (Unit const &unit) { return unit.unit_type == type; };
std::cout << "exec_action: 612" << std::endl;
    candidates = obs->GetUnits(alliance, cmp);
std::cout << "exec_action: 613" << std::endl;
    Unit const *best = nullptr;
std::cout << "exec_action: 614" << std::endl;
    float distance_squared = INFINITY;
std::cout << "exec_action: 615" << std::endl;
    for (Unit const *candidate : candidates) {
std::cout << "exec_action: 616" << std::endl;
        float dx2 = (candidate->pos.x - location.x) * (candidate->pos.x - location.x);
std::cout << "exec_action: 617" << std::endl;
        float dy2 = (candidate->pos.y - location.y) * (candidate->pos.y - location.y);
std::cout << "exec_action: 618" << std::endl;
        if (dx2 + dy2 < distance_squared) {
std::cout << "exec_action: 619" << std::endl;
            distance_squared = dx2 + dy2;
std::cout << "exec_action: 620" << std::endl;
            best = candidate;
std::cout << "exec_action: 621" << std::endl;
        }
std::cout << "exec_action: 622" << std::endl;
    }
std::cout << "exec_action: 623" << std::endl;
    return best;
}

void ExecAction::Init(Kurt * const kurt) {
std::cout << "exec_action: 627" << std::endl;
    commandcenter_locations = search::CalculateExpansionLocations(
            kurt->Observation(), kurt->Query());
}
