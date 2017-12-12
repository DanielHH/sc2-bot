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
std::cout << "kurt: 33" << std::endl;
    const ObservationInterface *observation = Observation();
std::cout << "kurt: 34" << std::endl;
    SetUpDataMaps(observation);
std::cout << "kurt: 35" << std::endl;
    TimeNew();
std::cout << "kurt: 36" << std::endl;
    world_rep = new WorldRepresentation(this);
std::cout << "kurt: 37" << std::endl;
    army_manager = new ArmyManager(this);
std::cout << "kurt: 38" << std::endl;
    TimeNext(time_am);
std::cout << "kurt: 39" << std::endl;
    build_manager = new BuildManager(this);
std::cout << "kurt: 40" << std::endl;
    build_manager->OnGameStart(Observation());
std::cout << "kurt: 41" << std::endl;
    TimeNext(time_bm);
std::cout << "kurt: 42" << std::endl;
    strategy_manager = new StrategyManager(this);
std::cout << "kurt: 43" << std::endl;
    TimeNext(time_sm);
std::cout << "kurt: 44" << std::endl;
    world_rep->PrintWorld();
}

void Kurt::OnStep() {
std::cout << "kurt: 48" << std::endl;
    const ObservationInterface* observation = Observation();
std::cout << "kurt: 49" << std::endl;
    int step = observation->GetGameLoop();

std::cout << "kurt: 51" << std::endl;
    TimeNew();
std::cout << "kurt: 52" << std::endl;
    world_rep->UpdateWorldRep();
std::cout << "kurt: 53" << std::endl;
    if (step % 6 == 0)
std::cout << "kurt: 54" << std::endl;
        army_manager->OnStep(observation);
std::cout << "kurt: 55" << std::endl;
    TimeNext(time_am);
std::cout << "kurt: 56" << std::endl;
    ExecAction::OnStep(this);
std::cout << "kurt: 57" << std::endl;
    build_manager->OnStep(observation);
std::cout << "kurt: 58" << std::endl;
    TimeNext(time_bm);
std::cout << "kurt: 59" << std::endl;
    strategy_manager->OnStep(observation);
std::cout << "kurt: 60" << std::endl;
    TimeNext(time_sm);

std::cout << "kurt: 62" << std::endl;
    if (observation->GetGameLoop() % (time_interval * STEPS_PER_SEC) == 0 &&
            observation->GetGameLoop() != 0) {
std::cout << "kurt: 64" << std::endl;
        std::cout << "Exec time over " << time_interval <<" sec, ";
std::cout << "kurt: 65" << std::endl;
        std::cout << "Army: " << time_am << ", Build: " << time_bm;
std::cout << "kurt: 66" << std::endl;
        std::cout << ", Strategy: " << time_sm << std::endl;
std::cout << "kurt: 67" << std::endl;
        time_am = 0;
std::cout << "kurt: 68" << std::endl;
        time_bm = 0;
std::cout << "kurt: 69" << std::endl;
        time_sm = 0;
std::cout << "kurt: 70" << std::endl;
    }

std::cout << "kurt: 72" << std::endl;
    assert(step == observation->GetGameLoop());
}

void Kurt::OnUnitCreated(const Unit* unit) {
std::cout << "kurt: 76" << std::endl;
    if (UnitAlreadyStored(unit)) {
std::cout << "kurt: 77" << std::endl;
        return;
std::cout << "kurt: 78" << std::endl;
    }
std::cout << "kurt: 79" << std::endl;
    const ObservationInterface* observation = Observation();
std::cout << "kurt: 80" << std::endl;
    TimeNew();
std::cout << "kurt: 81" << std::endl;
    army_manager->GroupNewUnit(unit, observation);
std::cout << "kurt: 82" << std::endl;
    TimeNext(time_am);
std::cout << "kurt: 83" << std::endl;
    strategy_manager->SaveOurUnits(unit);
std::cout << "kurt: 84" << std::endl;
    TimeNext(time_sm);
}

void Kurt::OnUnitIdle(const Unit* unit) {
std::cout << "kurt: 88" << std::endl;
    TimeNew();
std::cout << "kurt: 89" << std::endl;
    ExecAction::OnUnitIdle(unit, this);
std::cout << "kurt: 90" << std::endl;
    TimeNext(time_bm);
}

void Kurt::OnUnitDestroyed(const Unit *destroyed_unit) {
std::cout << "kurt: 94" << std::endl;
    TimeNew();
std::cout << "kurt: 95" << std::endl;
    strategy_manager->RemoveDeadUnit(destroyed_unit);
std::cout << "kurt: 96" << std::endl;
    TimeNext(time_sm);

std::cout << "kurt: 98" << std::endl;
    scv_building.remove(destroyed_unit);
std::cout << "kurt: 99" << std::endl;
    scv_idle.remove(destroyed_unit);
std::cout << "kurt: 100" << std::endl;
    scv_minerals.remove(destroyed_unit);
std::cout << "kurt: 101" << std::endl;
    scv_vespene.remove(destroyed_unit);
std::cout << "kurt: 102" << std::endl;
    scouts.remove(destroyed_unit);
std::cout << "kurt: 103" << std::endl;
    army_units.remove(destroyed_unit);

std::cout << "kurt: 105" << std::endl;
    if(destroyed_unit->alliance != Unit::Alliance::Self) return;
std::cout << "kurt: 106" << std::endl;
    
std::cout << "kurt: 107" << std::endl;
    if (IsStructure(destroyed_unit)) {
std::cout << "kurt: 108" << std::endl;
        build_manager->InitNewPlan();
std::cout << "kurt: 109" << std::endl;
        return;
std::cout << "kurt: 110" << std::endl;
    }
std::cout << "kurt: 111" << std::endl;
    TimeNew();
std::cout << "kurt: 112" << std::endl;
    for (auto it = build_manager->goal->UnitsBegin(); it != build_manager->goal->UnitsEnd(); ++it) {
std::cout << "kurt: 113" << std::endl;
        if ((*it).first == destroyed_unit->unit_type.ToType()) {
std::cout << "kurt: 114" << std::endl;
            build_manager->InitNewPlan();
std::cout << "kurt: 115" << std::endl;
            break;
std::cout << "kurt: 116" << std::endl;
        }
std::cout << "kurt: 117" << std::endl;
    }
std::cout << "kurt: 118" << std::endl;
    TimeNext(time_bm);
}

void Kurt::OnUnitEnterVision(const Unit* unit) {
std::cout << "kurt: 122" << std::endl;
    TimeNew();
std::cout << "kurt: 123" << std::endl;
    strategy_manager->OnUnitEnterVision(unit);
std::cout << "kurt: 124" << std::endl;
    TimeNext(time_sm);
}

bool Kurt::UnitInList(std::list<const Unit*>& list, const Unit* unit) {
std::cout << "kurt: 128" << std::endl;
    return std::find(list.begin(), list.end(), unit) != list.end();
}

bool Kurt::UnitAlreadyStored(const sc2::Unit* unit) {
std::cout << "kurt: 132" << std::endl;
    return
        UnitInList(scv_building, unit) ||
        UnitInList(scv_idle, unit) ||
        UnitInList(scv_minerals, unit) ||
        UnitInList(scv_vespene, unit) ||
        UnitInList(scouts, unit) ||
        UnitInList(army_units, unit);
}

bool Kurt::UnitInScvMinerals(const sc2::Unit* unit) {
std::cout << "kurt: 142" << std::endl;
    return UnitInList(scv_minerals, unit);
}

bool Kurt::UnitInScvVespene(const sc2::Unit* unit) {
std::cout << "kurt: 146" << std::endl;
    return UnitInList(scv_vespene, unit);
}

void Kurt::ExecuteSubplan() {
std::cout << "kurt: 150" << std::endl;
    TimeNext(time_bm);
std::cout << "kurt: 151" << std::endl;
    strategy_manager->ExecuteSubplan();
std::cout << "kurt: 152" << std::endl;
    TimeNext(time_sm);
}

void Kurt::SendBuildOrder(BPState* const build_order) {
std::cout << "kurt: 156" << std::endl;
    TimeNext(time_sm);
std::cout << "kurt: 157" << std::endl;
    build_manager->SetGoal(build_order);
std::cout << "kurt: 158" << std::endl;
    TimeNext(time_bm);
}

Kurt::CombatMode Kurt::GetCombatMode() {
std::cout << "kurt: 162" << std::endl;
    return current_combat_mode;
}

void Kurt::SetCombatMode(CombatMode new_combat_mode) {
std::cout << "kurt: 166" << std::endl;
    current_combat_mode = new_combat_mode;
}

void Kurt::CalculateCombatMode() {
std::cout << "kurt: 170" << std::endl;
    PRINT("Dynamic combat mode")
std::cout << "kurt: 171" << std::endl;
    strategy_manager->CalculateCombatMode();
}

void Kurt::CalculateBuildOrder() {
std::cout << "kurt: 175" << std::endl;
    PRINT("Dynamic build order")
std::cout << "kurt: 176" << std::endl;
    strategy_manager->SetBuildGoal();
}

void Kurt::CalculateNewPlan() {
std::cout << "kurt: 180" << std::endl;
    PRINT("Creating new plan...")
std::cout << "kurt: 181" << std::endl;
        strategy_manager->CalculateNewPlan();
}

bool Kurt::IsArmyUnit(const Unit* unit) {
std::cout << "kurt: 185" << std::endl;
    if (IsStructure(unit)) {
std::cout << "kurt: 186" << std::endl;
        return false;
std::cout << "kurt: 187" << std::endl;
    }
std::cout << "kurt: 188" << std::endl;
    switch (unit->unit_type.ToType()) {
std::cout << "kurt: 189" << std::endl;
        case UNIT_TYPEID::PROTOSS_PROBE: return false;
std::cout << "kurt: 190" << std::endl;
        case UNIT_TYPEID::TERRAN_SCV: return false;
std::cout << "kurt: 191" << std::endl;
        case UNIT_TYPEID::TERRAN_MULE: return false;
std::cout << "kurt: 192" << std::endl;
        case UNIT_TYPEID::TERRAN_NUKE: return false;
std::cout << "kurt: 193" << std::endl;
        case UNIT_TYPEID::ZERG_OVERLORD: return false;
std::cout << "kurt: 194" << std::endl;
        case UNIT_TYPEID::ZERG_DRONE: return false;
std::cout << "kurt: 195" << std::endl;
        case UNIT_TYPEID::ZERG_QUEEN: return false;
std::cout << "kurt: 196" << std::endl;
        case UNIT_TYPEID::ZERG_LARVA: return false;
std::cout << "kurt: 197" << std::endl;
        case UNIT_TYPEID::ZERG_EGG: return false;
std::cout << "kurt: 198" << std::endl;
        default: return true;
std::cout << "kurt: 199" << std::endl;
    }
}

bool Kurt::IsStructure(const Unit* unit) {
std::cout << "kurt: 203" << std::endl;
    bool is_structure = false;
std::cout << "kurt: 204" << std::endl;
    auto& attributes = GetUnitType(unit->unit_type)->attributes;
std::cout << "kurt: 205" << std::endl;
    for (const auto& attribute : attributes) {
std::cout << "kurt: 206" << std::endl;
        if (attribute == Attribute::Structure) {
std::cout << "kurt: 207" << std::endl;
            is_structure = true;
std::cout << "kurt: 208" << std::endl;
        }
std::cout << "kurt: 209" << std::endl;
    }
std::cout << "kurt: 210" << std::endl;
    return is_structure;
}

std::map<sc2::UNIT_TYPEID, sc2::UnitTypeData> Kurt::unit_types;
std::map<sc2::ABILITY_ID, sc2::AbilityData> Kurt::abilities;
std::map<sc2::UNIT_TYPEID, std::vector<sc2::ABILITY_ID>> Kurt::unit_ability_map;

void Kurt::SetUpDataMaps(const sc2::ObservationInterface *observation) {
std::cout << "kurt: 218" << std::endl;
    for (auto unit : observation->GetUnitTypeData()) {
std::cout << "kurt: 219" << std::endl;
        unit_types[(sc2::UNIT_TYPEID) unit.unit_type_id] = unit;
std::cout << "kurt: 220" << std::endl;
    }
std::cout << "kurt: 221" << std::endl;
    for (sc2::AbilityData ability : observation->GetAbilityData()) {
std::cout << "kurt: 222" << std::endl;
        abilities[(sc2::ABILITY_ID) ability.ability_id] = ability;
std::cout << "kurt: 223" << std::endl;
    }
}

AbilityData *Kurt::GetAbility(ABILITY_ID id) {
std::cout << "kurt: 227" << std::endl;
    return &abilities.at(id);
}

UnitTypeData *Kurt::GetUnitType(UNIT_TYPEID id) {
std::cout << "kurt: 231" << std::endl;
    return &unit_types.at(id);
}

void Kurt::TimeNew() {
std::cout << "kurt: 235" << std::endl;
    clock_start = std::chrono::steady_clock::now();
}

void Kurt::TimeNext(double & time_count) {
std::cout << "kurt: 239" << std::endl;
    clock_end = std::chrono::steady_clock::now();
std::cout << "kurt: 240" << std::endl;
    std::chrono::duration<double> time_span = clock_end - clock_start;
std::cout << "kurt: 241" << std::endl;
    time_count += time_span.count();
std::cout << "kurt: 242" << std::endl;
    clock_start = clock_end;
}

#undef DEBUG // Stop debug prints from leaking
#undef TEST
#undef PRINT
