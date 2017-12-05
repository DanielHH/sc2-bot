#include "kurt.h"

#include <list>
#include <algorithm>

#include "army_manager.h"
#include "build_manager.h"
#include "strategy_manager.h"
#include "world_representation.h"

#define DEBUG // Comment out to disable debug prints in this file.
#ifdef DEBUG
#include <iostream>
#define PRINT(s) std::cout << s << std::endl;
#define TEST(s) s
#else
#define PRINT(s)
#define TEST(s)
#endif // DEBUG


using namespace sc2;

ArmyManager* army_manager;
BuildManager* build_manager;
StrategyManager* strategy_manager;

void Kurt::OnGameStart() {
    const ObservationInterface *observation = Observation();
    SetUpDataMaps(observation);
    world_rep = new WorldRepresentation(this);
    army_manager = new ArmyManager(this);
    build_manager = new BuildManager(this);
    build_manager->OnGameStart(Observation());
    strategy_manager = new StrategyManager(this);
    world_rep->PrintWorld();
}

void Kurt::OnStep() {
    const ObservationInterface* observation = Observation();
    world_rep->UpdateWorldRep();
    army_manager->OnStep(observation);
    build_manager->OnStep(observation);
    strategy_manager->OnStep(observation);
}

void Kurt::OnUnitCreated(const Unit* unit) {
    if (UnitAlreadyStored(unit)) {
        return;
    }
    const ObservationInterface* observation = Observation();
    army_manager->GroupNewUnit(unit, observation);
    strategy_manager->SaveOurUnits(unit);
}

void Kurt::OnUnitIdle(const Unit* unit) {
    switch (unit->unit_type.ToType()) {
    case UNIT_TYPEID::TERRAN_SCV: {
        const Unit* mineral_target = FindNearestMineralPatch(unit->pos);
        if (!mineral_target) {
            break;
        }
        Actions()->UnitCommand(unit, ABILITY_ID::SMART, mineral_target);
        if (! UnitInScvMinerals(unit)) {
            scv_minerals.push_back(unit);
        }
        break;
    }
    default: {
        break;
    }

    }
}

void Kurt::OnUnitDestroyed(const Unit *destroyed_unit) {
    if (destroyed_unit->alliance != Unit::Alliance::Self) return;
    workers.remove(destroyed_unit);
    scv_minerals.remove(destroyed_unit);
    scv_vespene.remove(destroyed_unit);
    scouts.remove(destroyed_unit);
    army.remove(destroyed_unit);

    
    for (auto it = build_manager->goal->UnitsBegin(); it != build_manager->goal->UnitsEnd(); ++it) {
        if ((*it).first == destroyed_unit->unit_type.ToType() || IsStructureType(it->first)) {
            build_manager->replan = true;
            break;
        }
    }
    
}

bool Kurt::IsArmyUnit(const Unit* unit) {
    return IsArmyUnitType(unit->unit_type.ToType());
}

bool Kurt::IsArmyUnitType(const UNIT_TYPEID &type) {
    if (IsStructureType(type)) {
        return false;
    }
    switch (type) {
    default: return true;
    case UNIT_TYPEID::TERRAN_SCV:
    case UNIT_TYPEID::TERRAN_MULE:
    case UNIT_TYPEID::TERRAN_NUKE:
        return false;
    }
}

bool Kurt::IsStructure(const Unit* unit) {
    return IsStructureType(unit->unit_type.ToType());
}

bool Kurt::IsStructureType(const UNIT_TYPEID &type) {
    bool is_structure = false;
    for (const auto &attr : GetUnitType(type)->attributes) {
        if (attr == Attribute::Structure) return true;
    }
    return false;
}

bool Kurt::UnitInList(std::list<const Unit*>& list, const Unit* unit) {
    return std::find(list.begin(), list.end(), unit) != list.end();
}

bool Kurt::UnitAlreadyStored(const sc2::Unit* unit) {
    return UnitInList(workers, unit) ||
        UnitInList(scv_minerals, unit) ||
        UnitInList(scv_vespene, unit) ||
        UnitInList(scouts, unit) ||
        UnitInList(army, unit);
}

bool Kurt::UnitInScvMinerals(const sc2::Unit* unit) {
    return UnitInList(scv_minerals, unit);
}

bool Kurt::UnitInScvVespene(const sc2::Unit* unit) {
    return UnitInList(scv_vespene, unit);
}

void Kurt::ExecuteSubplan() {
    std::cout << "Kurt exec" << std::endl;
    strategy_manager->ExecuteSubplan();
}

void Kurt::SendBuildOrder(BPState* const build_order) {
    build_manager->SetGoal(build_order);
    std::cout << "Build something!" << std::endl;
}

Kurt::CombatMode Kurt::GetCombatMode() {
    return current_combat_mode;
}

void Kurt::SetCombatMode(CombatMode new_combat_mode) {
    current_combat_mode = new_combat_mode;
}

void Kurt::CalculateCombatMode() {
    PRINT("Dynamic combat mode")
    strategy_manager->CalculateCombatMode();
}

void Kurt::CalculateBuildOrder() {
    PRINT("Dynamic build order")
    strategy_manager->SetBuildGoal();
}

bool Kurt::TryBuildStructure(ABILITY_ID ability_type_for_structure,
    Point2D location,
    const Unit* unit) {
    const ObservationInterface* observation = Observation();

    // If a unit already is building a supply structure of this type, do nothing.
    // Also get an scv to build the structure.
    std::vector<const Unit*> units_to_build;
    Units units = observation->GetUnits(Unit::Alliance::Self);

    Actions()->UnitCommand(unit,
        ability_type_for_structure,
        location);
    return true;
}


const Unit* Kurt::getUnitOfType(UNIT_TYPEID unit_typeid) {
    const ObservationInterface* observation = Observation();
    Units units = observation->GetUnits(Unit::Alliance::Self);
    return units.front();
}


Point2D Kurt::randomLocationNearUnit(const Unit* unit) {
    float rx = GetRandomScalar();
    float ry = GetRandomScalar();
    Point2D location = Point2D(unit->pos.x + rx * 15.0f, unit->pos.y + ry * 15.0f);
    return location;

}


bool Kurt::TryBuildSupplyDepot() {
    const ObservationInterface* observation = Observation();

    // If we are not supply capped, don't build a supply depot.
    if (observation->GetFoodUsed() <= observation->GetFoodCap() - 2) {
        return false;
    }
    // Try and build a depot. Find a random SCV and give it the order.
    const Unit* unit = getUnitOfType(UNIT_TYPEID::TERRAN_SCV);
    return TryBuildStructure(ABILITY_ID::BUILD_SUPPLYDEPOT, randomLocationNearUnit(unit), unit);
}

bool Kurt::TryBuildRefinary() {
    const ObservationInterface* observation = Observation();

    // As soon as we have enough gold we build a refinary
    if (observation->GetMinerals() < 100) {
        return false;
    }
    const Unit* vespene_target = FindNearestVespeneGeyser();
    // Try and build a depot. Find a random SCV and give it the order.

    // rmove false
    return false;

}

const Unit* Kurt::FindNearestMineralPatch(const Point2D& start) {
    Units units = Observation()->GetUnits(Unit::Alliance::Neutral);
    float distance = std::numeric_limits<float>::max();
    const Unit* target = nullptr;
    for (const auto& u : units) {
        if (u->unit_type == UNIT_TYPEID::NEUTRAL_MINERALFIELD) {
            float d = DistanceSquared2D(u->pos, start);
            if (d < distance) {
                distance = d;
                target = u;
            }
        }
    }
    return target;
}

const Unit* Kurt::FindNearestVespeneGeyser() {
    Units units = Observation()->GetUnits(Unit::Alliance::Neutral);
    Units allied_units = Observation()->GetUnits(Unit::Alliance::Ally);
    float distance = std::numeric_limits<float>::max();
    const Unit* command_center = nullptr;
    const Unit* vespene_geyser = nullptr;
    // Look for a command center among allied units
    for (const auto& ally : allied_units) {
        if (ally->unit_type == UNIT_TYPEID::TERRAN_COMMANDCENTER) {
            command_center = ally;
            // look for closest free vespene gayser of that command center
            for (const auto& unit : units) {
                if (unit->build_progress == 0) {
                    float d = DistanceSquared2D(unit->pos, command_center->pos);
                    if (d < distance) {
                        distance = d;
                        vespene_geyser = unit;
                    }
                }
            }
        }
        // return if we have found a suitable vespene geyser close to a command center
        if (command_center && vespene_geyser) {
            return vespene_geyser;
        }
    }
    //        std::cout << "NO VESPENE FOUND" << std::endl;
    return nullptr;
}

std::map<sc2::UNIT_TYPEID, sc2::UnitTypeData> Kurt::unit_types;
std::map<sc2::ABILITY_ID, sc2::AbilityData> Kurt::abilities;
std::map<sc2::UNIT_TYPEID, std::vector<sc2::ABILITY_ID>> Kurt::unit_ability_map;

void Kurt::SetUpDataMaps(const sc2::ObservationInterface *observation) {
    for (auto unit : observation->GetUnitTypeData()) {
        unit_types[(sc2::UNIT_TYPEID) unit.unit_type_id] = unit;
    }
    for (sc2::AbilityData ability : observation->GetAbilityData()) {
        abilities[(sc2::ABILITY_ID) ability.ability_id] = ability;
    }
}

AbilityData *Kurt::GetAbility(ABILITY_ID id) {
    return &abilities.at(id);
}

UnitTypeData *Kurt::GetUnitType(UNIT_TYPEID id) {
    return &unit_types.at(id);
}

#undef DEBUG // Stop debug prints from leaking
#undef TEST
#undef PRINT
