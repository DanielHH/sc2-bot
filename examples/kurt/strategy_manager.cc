#include "strategy_manager.h"
#include <iostream>

using namespace sc2;
using namespace std;

map<string, Units> our_units;
//map<string, Units> enemy_units;
Units enemy_units;

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
    Units tmp = enemy_units;
    for (auto known_unit = tmp.begin(); known_unit != tmp.end(); known_unit++) {
        for (auto new_unit = observed_enemy_units.begin(); new_unit != observed_enemy_units.end(); new_unit++) {
            if (*known_unit == *new_unit) {
                observed_enemy_units.erase(new_unit);
                break;
            }
        }
    }

    enemy_units.insert(enemy_units.end(), observed_enemy_units.begin(), observed_enemy_units.end());
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

    void StrategyManager::CalculateCombatPower(CombatPower *cp) {
     

};




