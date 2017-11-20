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
    // Måste på nåt sätt ta hänsyn till om man har sett uniten innan eller inte.
    for (const auto unit : observed_enemy_units) {
        CheckCombatStyle(unit, enemy_units);
    }
}

void StrategyManager::CheckCombatStyle(const Unit* unit, map<string, Units> map) {
    //GroundToGround
    /*if(unit->unit_type == UNIT_TYPEID::TERRAN_MARINE ||
        unit->unit_type == UNIT_TYPEID::TERRAN_REAPER ||
        unit->unit_type == UNIT_TYPEID::TERRAN_MARAUDER) {
        map["gg"].push_back(unit);
    }*/
    if (!unit->is_flying && unit->is_alive /* && check for groundweapon */) {
        map["g2g"].push_back(unit);
    }
    //GroundToAir
    if (!unit->is_flying && unit->is_alive /* && check for airweapon */) {
        map["g2a"].push_back(unit);
    }
    //AirToGround
    if (unit->is_flying && unit->is_alive /* && check for groundweapon */) {
        map["a2g"].push_back(unit);
    }
    //AirToAir
    if (unit->is_flying && unit->is_alive /* && check for airweapon */) {
        map["a2a"].push_back(unit);
    }
};

    float  StrategyManager::CalculateCombatPower(string combat_style) {
        /*for (enemy_units[combat_style]) {

        }*/
        return -1;
};




