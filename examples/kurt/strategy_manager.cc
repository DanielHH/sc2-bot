#include "strategy_manager.h"
#include <iostream>

using namespace sc2;
using namespace std;

map<string, Units> our_units;
map<string, Units> enemy_units;

StrategyManager::StrategyManager() {

}

void StrategyManager::OnStep(const ObservationInterface* observation) {
    // DO ALL DE STRATEGY STUFF
}

void StrategyManager::SortOurUnits(const Unit* unit) {
    CheckCombatStyle(unit, our_units);
}

//Save enemy units in vector spotted_enemy_units
void StrategyManager::SaveSpottedEnemyUnits(const ObservationInterface* observation) {
    Units observed_enemy_units = observation->GetUnits(Unit::Alliance::Enemy);
    for (const auto unit : observed_enemy_units) {
        CheckCombatStyle(unit, enemy_units);
    }
}

void StrategyManager::CheckCombatStyle(const Unit* unit, map<string, Units> map) {
    //GroundToGround
    if(unit->unit_type == UNIT_TYPEID::TERRAN_MARINE ||
        unit->unit_type == UNIT_TYPEID::TERRAN_REAPER ||
        unit->unit_type == UNIT_TYPEID::TERRAN_MARAUDER) {
        map["gg"].push_back(unit);
    }
   /* //GroundToAir
    else if() {

    }
    //AirToGround
    else if () {

    }
    //AirToAir
    else if () {

    }*/

}

// Returns true if Enemy structure is observed. Med ObservationInterface ser man väl allt?
bool StrategyManager::FindEnemyStructure(const ObservationInterface* observation, const Unit*& enemy_unit) {
    Units my_units = observation->GetUnits(Unit::Alliance::Enemy);
    for (const auto unit : my_units) {
        if (unit->unit_type == UNIT_TYPEID::TERRAN_COMMANDCENTER ||
            unit->unit_type == UNIT_TYPEID::TERRAN_SUPPLYDEPOT ||
            unit->unit_type == UNIT_TYPEID::TERRAN_BARRACKS) {
            enemy_unit = unit;
            return true;
        }
    }
    return false;
}


