#include "kurt.h"

#include <list>
#include <algorithm>

#include "army_manager.h"
#include "build_manager.h"
#include "strategy_manager.h"
#include "world_representation.h"
#include "exec_action.h"

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
    int step = observation->GetGameLoop();
    ExecAction::OnStep(this);
    world_rep->UpdateWorldRep();
    army_manager->OnStep(observation);
    build_manager->OnStep(observation);
    strategy_manager->OnStep(observation);
    assert(step == observation->GetGameLoop());
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
    ExecAction::OnUnitIdle(unit, this);
}

void Kurt::OnUnitDestroyed(const Unit *destroyed_unit) {

    strategy_manager->RemoveDeadUnit(destroyed_unit);

    scv_building.remove(destroyed_unit);
    scv_idle.remove(destroyed_unit);
    scv_minerals.remove(destroyed_unit);
    scv_vespene.remove(destroyed_unit);
    scouts.remove(destroyed_unit);
    army_units.remove(destroyed_unit);

    if(destroyed_unit->alliance != Unit::Alliance::Self) return;
    
    if (IsStructure(destroyed_unit)) {
        build_manager->InitNewPlan();
        return;
    }
    for (auto it = build_manager->goal->UnitsBegin(); it != build_manager->goal->UnitsEnd(); ++it) {
        if ((*it).first == destroyed_unit->unit_type.ToType()) {
            build_manager->InitNewPlan();
            break;
        }
    }
}

void Kurt::OnUnitEnterVision(const Unit* unit) {
    strategy_manager->OnUnitEnterVision(unit);
}

bool Kurt::UnitInList(std::list<const Unit*>& list, const Unit* unit) {
    return std::find(list.begin(), list.end(), unit) != list.end();
}

bool Kurt::UnitAlreadyStored(const sc2::Unit* unit) {
    return
        UnitInList(scv_building, unit) ||
        UnitInList(scv_idle, unit) ||
        UnitInList(scv_minerals, unit) ||
        UnitInList(scv_vespene, unit) ||
        UnitInList(scouts, unit) ||
        UnitInList(army_units, unit);
}

bool Kurt::UnitInScvMinerals(const sc2::Unit* unit) {
    return UnitInList(scv_minerals, unit);
}

bool Kurt::UnitInScvVespene(const sc2::Unit* unit) {
    return UnitInList(scv_vespene, unit);
}

void Kurt::ExecuteSubplan() {
    strategy_manager->ExecuteSubplan();
}

void Kurt::SendBuildOrder(BPState* const build_order) {
    build_manager->SetGoal(build_order);
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

void Kurt::CalculateNewPlan() {
    PRINT("Creating new plan...")
        strategy_manager->CalculateNewPlan();
}

bool Kurt::IsArmyUnit(const Unit* unit) {
    if (IsStructure(unit)) {
        return false;
    }
    switch (unit->unit_type.ToType()) {
        case UNIT_TYPEID::PROTOSS_PROBE: return false;
        case UNIT_TYPEID::TERRAN_SCV: return false;
        case UNIT_TYPEID::TERRAN_MULE: return false;
        case UNIT_TYPEID::TERRAN_NUKE: return false;
        case UNIT_TYPEID::ZERG_OVERLORD: return false;
        case UNIT_TYPEID::ZERG_DRONE: return false;
        case UNIT_TYPEID::ZERG_QUEEN: return false;
        case UNIT_TYPEID::ZERG_LARVA: return false;
        case UNIT_TYPEID::ZERG_EGG: return false;
        default: return true;
    }
}

bool Kurt::IsStructure(const Unit* unit) {
    bool is_structure = false;
    auto& attributes = GetUnitType(unit->unit_type)->attributes;
    for (const auto& attribute : attributes) {
        if (attribute == Attribute::Structure) {
            is_structure = true;
        }
    }
    return is_structure;
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
