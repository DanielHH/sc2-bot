#include "strategy_manager.h"
#include <iostream>

using namespace sc2;
using namespace std;

//map<string, Units> our_units;
//map<string, Units> enemy_units;
Units our_units;
Units enemy_units;

StrategyManager::StrategyManager() {
    our_cp.alliance = "our_cp";
    enemy_cp.alliance = "enemy_cp";

}

void StrategyManager::OnStep(const ObservationInterface* observation) {
    // DO ALL DE STRATEGY STUFF
}

void StrategyManager::SaveOurUnits(const Unit* unit) {
    our_units.push_back(unit);
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
};

// There will be a lot of duplicated code beacuse our_cp and enemy_cp
// is basically doing the same thing but with different vectors and structs. This should be optimized.
void StrategyManager::CalculateCombatPower(CombatPower *cp) {
    if (cp->alliance == "our_cp") {
        for (auto unit : our_units) {
            //GroundToGround
            if (!unit->is_flying && unit->is_alive /* && check for groundweapon */) {
                //cp->g2g += unit.dps for groundweapon. Similar thing has to be done in all if-cases.
            }
            //GroundToAir
            if (!unit->is_flying && unit->is_alive /* && check for airweapon */) {
                
            }
            //AirToGround
            if (unit->is_flying && unit->is_alive /* && check for groundweapon */) {
                
            }
            //AirToAir
            if (unit->is_flying && unit->is_alive /* && check for airweapon */) {
                
            }
        }
    }
    else if (cp->alliance == "enemy_cp") {

    }

};

void StrategyManager::ChooseCombatMode() {
    if (our_cp.g2g > enemy_cp.g2g && our_cp.g2a > enemy_cp.a2g && our_cp.a2g > enemy_cp.g2a && our_cp.a2a > enemy_cp.a2a) {
        SetCombatMode("Attack!");
    }
    else if (our_cp.g2g < enemy_cp.g2g && our_cp.g2a < enemy_cp.a2g && our_cp.a2g < enemy_cp.g2a && our_cp.a2a < enemy_cp.a2a) {
        SetCombatMode("Defend!");
    }
    else {
        SetCombatMode("Harrass!");
    }
};

void StrategyManager::SetCombatMode(std::string new_mode) {
    combat_mode = new_mode;
};

string StrategyManager::GetCombatMode() {
    return combat_mode;
};


//NOT CURRENTLY USED!
void StrategyManager::CheckCombatStyle(const Unit* unit, map<string, Units> map) {
    //GroundToGround
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




