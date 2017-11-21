#include "strategy_manager.h"
#include "kurt.h"

#include <iostream>

using namespace sc2;
using namespace std;


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

void StrategyManager::CalculateCombatPower(CombatPower *cp) {
    //reset cp-data.
    cp->g2g = 0;
    cp->g2a = 0;
    cp->a2g = 0;
    cp->a2a = 0;

    if (cp->alliance == "our_cp") {
        CalculateCPHelp(cp, our_units);
    }
    else if (cp->alliance == "enemy_cp") {
        CalculateCPHelp(cp, enemy_units);
    }
};

void StrategyManager::CalculateCPHelp(CombatPower *cp, Units team) {
    UnitTypeData* unit_data;
    float weapon_dps;
    for (auto unit : team) {
        if (unit->is_alive) { //This check can be removed if we remove dead units from vectors.
            unit_data = Kurt::GetUnitType(unit->unit_type);
            for (auto weapon : unit_data->weapons) {
                weapon_dps = weapon.damage_ / weapon.speed; // This is correct assuming damage_ == damage_ per attack
                if (weapon.type == Weapon::TargetType::Ground) {
                    //GroundToGround
                    if (!unit->is_flying) {
                        cp->g2g += weapon_dps;
                    }
                    //AirToGround
                    else if (unit->is_flying) {
                        cp->a2g += weapon_dps;
                    }
                }
                if (weapon.type == Weapon::TargetType::Air) {
                    //GroundToAir
                    if (!unit->is_flying) {
                        cp->g2a += weapon_dps;
                    }
                    //AirToAir
                    if (unit->is_flying) {
                        cp->a2a += weapon_dps;
                    }
                }
            }
        }
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

void StrategyManager::DecideBuildGoal() {

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




