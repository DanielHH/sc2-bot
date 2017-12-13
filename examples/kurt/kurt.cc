#include "kurt.h"

#include <list>
#include <algorithm>
#include <ctime>
#include <ratio>
#include <chrono>

#include "army_manager.h"
#include "build_manager.h"
#include "strategy_manager.h"
#include "world_representation.h"
#include "exec_action.h"
#include "constants.h"

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

ArmyManager* army_manager;
BuildManager* build_manager;
StrategyManager* strategy_manager;

void Kurt::OnGameStart() {
    const ObservationInterface *observation = Observation();
    SetUpDataMaps(observation);
    TimeNew();
    world_rep = new WorldRepresentation(this);
    army_manager = new ArmyManager(this);
    TimeNext(time_am);
    build_manager = new BuildManager(this);
    build_manager->OnGameStart(Observation());
    TimeNext(time_bm);
    strategy_manager = new StrategyManager(this);
    TimeNext(time_sm);
    world_rep->PrintWorld();
}

void Kurt::OnStep() {
    const ObservationInterface* observation = Observation();
    int step = observation->GetGameLoop();

    TimeNew();
    world_rep->UpdateWorldRep();
    if (step % 5 == 0) {
        army_manager->OnStep(observation);
    }
    TimeNext(time_am);
    ExecAction::OnStep(this);
    build_manager->OnStep(observation);
    TimeNext(time_bm);
    strategy_manager->OnStep(observation);
    TimeNext(time_sm);

    if (observation->GetGameLoop() % (time_interval * STEPS_PER_SEC) == 0 &&
            observation->GetGameLoop() != 0) {
        std::cout << "Exec time over " << time_interval <<" sec, ";
        std::cout << "Army: " << time_am << ", Build: " << time_bm;
        std::cout << ", Strategy: " << time_sm << std::endl;
        time_am = 0;
        time_bm = 0;
        time_sm = 0;
    }

    assert(step == observation->GetGameLoop());
}

void Kurt::OnUnitCreated(const Unit* unit) {
    if (UnitAlreadyStored(unit)) {
        return;
    }
    const ObservationInterface* observation = Observation();
    TimeNew();
    army_manager->GroupNewUnit(unit, observation);
    TimeNext(time_am);
    strategy_manager->SaveOurUnits(unit);
    TimeNext(time_sm);
}

void Kurt::OnUnitIdle(const Unit* unit) {
    TimeNew();
    ExecAction::OnUnitIdle(unit, this);
    TimeNext(time_bm);
}

void Kurt::OnUnitDestroyed(const Unit *destroyed_unit) {
    TimeNew();
    strategy_manager->RemoveDeadUnit(destroyed_unit);
    TimeNext(time_sm);

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
    TimeNew();
    for (auto it = build_manager->goal->UnitsBegin(); it != build_manager->goal->UnitsEnd(); ++it) {
        if ((*it).first == destroyed_unit->unit_type.ToType()) {
            build_manager->InitNewPlan();
            break;
        }
    }
    TimeNext(time_bm);
}

void Kurt::OnUnitEnterVision(const Unit* unit) {
    TimeNew();
    strategy_manager->OnUnitEnterVision(unit);
    TimeNext(time_sm);
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
    TimeNext(time_bm);
    strategy_manager->ExecuteSubplan();
    TimeNext(time_sm);
}

void Kurt::SendBuildOrder(BPState* const build_order) {
    TimeNext(time_sm);
    build_manager->SetGoal(build_order);
    TimeNext(time_bm);
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

void Kurt::SetProgressionMode(bool new_progression_mode) {
    strategy_manager->SetProgressionMode(new_progression_mode);
}

bool Kurt::GetProgressionMode() {
    bool progression_mode = strategy_manager->GetProgressionMode();
    return progression_mode;
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

void Kurt::TimeNew() {
    clock_start = std::chrono::steady_clock::now();
}

void Kurt::TimeNext(double & time_count) {
    clock_end = std::chrono::steady_clock::now();
    std::chrono::duration<double> time_span = clock_end - clock_start;
    time_count += time_span.count();
    clock_start = clock_end;
}

#undef DEBUG // Stop debug prints from leaking
#undef TEST
#undef PRINT
