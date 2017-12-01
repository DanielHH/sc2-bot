#include "strategy_manager.h"
#include "plans.cc"
#include <iostream>

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
using namespace std;

Units our_units;
Units enemy_units;


StrategyManager::StrategyManager(Kurt* parent_kurt) {
    kurt = parent_kurt;
    our_cp.alliance = "our_cp";
    enemy_cp.alliance = "enemy_cp";

    // Create a test plan
    current_plan = new GamePlan(parent_kurt);

    // Build order of 3 marines
    BPState* test_build = new BPState();
    test_build->SetUnitAmount(UNIT_TYPEID::TERRAN_MARINE, 3);
    current_plan->AddStatBuildOrderNode(test_build);
    current_plan->AddDynBuildOrderNode();

    // Attack order
    current_plan->AddStatCombatNode(Kurt::ATTACK);

    // Harass order
    current_plan->AddDynCombatNode();
}

void StrategyManager::OnStep(const ObservationInterface* observation) {
    int current_game_loop = observation->GetGameLoop();

    SaveSpottedEnemyUnits(observation);

    if (current_game_loop % 1000 == 0) {
        current_plan->ExecuteNextNode();
    }
}

void StrategyManager::SaveOurUnits(const Unit* unit) {
    our_units.push_back(unit);
}

void StrategyManager::ExecuteSubplan() {
    std::cout << "SM exec" << std::endl;
    current_plan->ExecuteNextNode();
}

/*
Save enemy units in vector spotted_enemy_units. This saves the minimal amount of units that
we know the enemy has, but the enemy might have more units in the fog of war.
*/
void StrategyManager::SaveSpottedEnemyUnits(const ObservationInterface* observation) { //TODO: Remove structures from this vector. Make a new vector for this.
    Units observed_enemy_units = observation->GetUnits(Unit::Alliance::Enemy);
    // Måste på nåt sätt ta hänsyn till om man har sett uniten innan eller inte.
    Units tmp = enemy_units;

    /*
    For every observed enemy, check if a unit of the same type is already saved in enemy_units.
    If there is, count the new unit as already seen and don't add it to the enemy_units vector.
    */
    for (auto known_unit = tmp.begin(); known_unit != tmp.end(); known_unit++) {
        for (auto new_unit = observed_enemy_units.begin(); new_unit != observed_enemy_units.end(); new_unit++) {
            if (*known_unit == *new_unit) {
                observed_enemy_units.erase(new_unit);
                break;
            }
        }
    }
    // Save the observed units that didn't get filtered out as already seen.
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
    UnitTypeData* unit_data = new UnitTypeData();
    float weapon_dps;

    for (auto unit : team) {
        if (unit->is_alive) { //This check can be removed if we remove dead units from vectors.
            unit_data = Kurt::GetUnitType(unit->unit_type);
            for (auto weapon : unit_data->weapons) {
                weapon_dps = weapon.damage_ / weapon.speed; // This is correct assuming damage_ == damage_ per attack
                if (weapon.type == Weapon::TargetType::Any) { //Kolla upp om targettype::any är samma sak som air och ground, och det kommer dubbleras eller ej.
                    //GroundToBoth
                    if (!unit->is_flying) {
                        cp->g2a += weapon_dps;
                        cp->g2g += weapon_dps;
                    }
                    //AirToBoth
                    if (unit->is_flying) {
                        cp->a2a += weapon_dps;
                        cp->a2g += weapon_dps;
                    }
                }
                else if (weapon.type == Weapon::TargetType::Ground) {
                    //GroundToGround
                    if (!unit->is_flying) {
                        cp->g2g += weapon_dps;
                    }
                    //AirToGround
                    else if (unit->is_flying) {
                        cp->a2g += weapon_dps;
                    }
                }
                else if (weapon.type == Weapon::TargetType::Air) {
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

void StrategyManager::CalculateCombatMode() {
    if (our_cp.g2g > enemy_cp.g2g && our_cp.g2a > enemy_cp.a2g && our_cp.a2g > enemy_cp.g2a && our_cp.a2a > enemy_cp.a2a) {
        //attack
    }
    else if (our_cp.g2g < enemy_cp.g2g && our_cp.g2a < enemy_cp.a2g && our_cp.a2g < enemy_cp.g2a && our_cp.a2a < enemy_cp.a2a) {
        //defend
    }
    else {
       //harrass
    }
};

void StrategyManager::SetGamePlan() {



}

void StrategyManager::SetBuildGoal() {

    BPState* new_goal_state = new BPState();

    if (our_cp.g2g < 80 || our_cp.g2a < 80) {
        new_goal_state->SetUnitAmount(UNIT_TYPEID::TERRAN_MARINE, 10);
    }
    else if (our_cp.a2a < 50) {
        new_goal_state->SetUnitAmount(UNIT_TYPEID::TERRAN_VIKINGASSAULT, 5);
    }

    //current_plan->AddBuildOrderNode(new_goal_state);
};


//NOT CURRENTLY USED!
/*
void StrategyManager::CheckCombatStyle(const Unit* unit, map<string, Units> map) {
    //GroundToGround
    if (!unit->is_flying && unit->is_alive) {
        map["g2g"].push_back(unit);
    }
    //GroundToAir
    if (!unit->is_flying && unit->is_alive) {
        map["g2a"].push_back(unit);
    }
    //AirToGround
    if (unit->is_flying && unit->is_alive) {
        map["a2g"].push_back(unit);
    }
    //AirToAir
    if (unit->is_flying && unit->is_alive) {
        map["a2a"].push_back(unit);
    }
};
*/




#undef DEBUG
#undef PRINT
#undef TEST
