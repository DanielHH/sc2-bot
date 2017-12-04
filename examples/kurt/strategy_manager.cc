#include "strategy_manager.h"
#include "plans.h"
#include "countertable.h"

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
using namespace std;

Units our_units;
Units our_structures;
Units enemy_units;
Units enemy_structures;

StrategyManager::StrategyManager(Kurt* parent_kurt) {
    kurt = parent_kurt;
    our_cp.alliance = "our_cp";
    enemy_cp.alliance = "enemy_cp";

    //current_plan = CreateDefaultGamePlan(kurt);
    current_plan = RushPlan(kurt);
    //current_plan = DynamicGamePlan(kurt);
    current_plan->ExecuteNextNode();
}

void StrategyManager::OnStep(const ObservationInterface* observation) {
    int current_game_loop = observation->GetGameLoop();

    SaveSpottedEnemyUnits(observation);

    if (current_game_loop % 500 == 0) {
        PRINT("------Enemy units-------")
        for (int i = 0; i < enemy_units.size(); ++i) {
            PRINT(kurt->GetUnitType(enemy_units[i]->unit_type)->name)
        }
        PRINT("------Enemy structures-------")
        for (int i = 0; i < enemy_structures.size(); ++i) {
            PRINT(kurt->GetUnitType(enemy_structures[i]->unit_type)->name)
        }
        PRINT("------------------------")
    }
}

void StrategyManager::SaveOurUnits(const Unit* unit) {
    our_units.push_back(unit);
    //update combatpower. TODO: Make more efficient.
    CalculateCombatPower(&our_cp);
}

void StrategyManager::ExecuteSubplan() {
    current_plan->ExecuteNextNode();
}

void StrategyManager::SaveSpottedEnemyUnits(const ObservationInterface* observation) {
    Units observed_enemy_units = observation->GetUnits(Unit::Alliance::Enemy);
    Units observed_structures;
    Units observed_units;

    // Split observation into structures and units
    for (auto observed_unit = observed_enemy_units.begin(); observed_unit != observed_enemy_units.end(); ++observed_unit) {
        if (kurt->IsStructure(*observed_unit)) {
            observed_structures.push_back(*observed_unit);
        }
        else {
            observed_units.push_back(*observed_unit);
        }
    }

    // Save any newly observed structures
    SaveSpottedEnemyUnitsHelper(&observed_structures, &enemy_structures);

    //Save any newly observed units
    SaveSpottedEnemyUnitsHelper(&observed_units, &enemy_units);

    //Update combatpower. TODO: Make more efficient.
    CalculateCombatPower(&enemy_cp);
};

void StrategyManager::SaveSpottedEnemyUnitsHelper(Units* new_units, Units* saved_units) {
    // For every observed enemy, check if a unit of the same type is already saved in saved_units.
    // If there is, count the new unit as already seen and don't add it to the saved_units vector.
    for (auto saved_unit = saved_units->begin(); saved_unit != saved_units->end(); ++saved_unit) {
        for (auto new_unit = new_units->begin(); new_unit != new_units->end(); ++new_unit) {
            if ((*saved_unit)->unit_type == (*new_unit)->unit_type) {
                new_units->erase(new_unit);
                break;
            }
        }
    }

    // Save the observed units that didn't get filtered out as already seen.
    saved_units->insert(saved_units->end(), new_units->begin(), new_units->end());
}

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
        kurt->SetCombatMode(Kurt::ATTACK);
    }
    else if (our_cp.g2g < enemy_cp.g2g && our_cp.g2a < enemy_cp.a2g && our_cp.a2g < enemy_cp.g2a && our_cp.a2a < enemy_cp.a2a) {
        kurt->SetCombatMode(Kurt::DEFEND);
    }
    else {
        kurt->SetCombatMode(Kurt::HARASS);
    }
};

void StrategyManager::SetGamePlan() {
    delete current_plan;
    current_plan = DynamicGamePlan(kurt);
}

void StrategyManager::SetBuildGoal() {
    BPState* new_goal_state = new BPState();

    if (our_cp.g2g < 80 || our_cp.g2a < 80) {
        new_goal_state->SetUnitAmount(UNIT_TYPEID::TERRAN_MARINE, 5);
    }
    else if (our_cp.a2a < 50) {
        new_goal_state->SetUnitAmount(UNIT_TYPEID::TERRAN_VIKINGASSAULT, 5);
    }
    else if (our_cp.a2a < enemy_cp.a2a || our_cp.g2a < enemy_cp.g2a) {
        new_goal_state->SetUnitAmount(UNIT_TYPEID::TERRAN_MARINE, 5);
        new_goal_state->SetUnitAmount(UNIT_TYPEID::TERRAN_LIBERATOR, 3);
    }

    kurt->SendBuildOrder(new_goal_state);
};

void StrategyManager::CounterEnemyUnits() {
    Units tmp = our_units;
    tuple<UNIT_TYPEID> counter_units;
    for (auto unit : enemy_units) {
        counter_units = zerg_countertable.at(unit->unit_type);
        for (int i = 0; sizeof(counter_units), ++i) {
            counter_units.get
        }
       
    }
}

//NOT CURRENTLY USED!
/*
void StrategyManager::CheckCombatStyle(const Unit* unit, map<string, Units> map) {
    //GroundToGround
    if (!unit->is_flying && unit->is_alive) {
        map["g2g"].push_back(unit);
>>>>>>> 81559c0b9f8398d5db4a9ca3346fc67c32e49833
    }
    else if (our_cp.g2g < enemy_cp.g2g || our_cp.a2g < enemy_cp.a2g) {
        new_goal_state->SetUnitAmount(UNIT_TYPEID::TERRAN_VIKINGASSAULT, 5);
    }
    kurt->SendBuildOrder(new_goal_state);
};


*/

#undef DEBUG
#undef PRINT
#undef TEST
