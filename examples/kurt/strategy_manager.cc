#include "strategy_manager.h"
#include "plans.h"
#include "countertable.h"
#include <algorithm>
#include "observed_units.h"

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

ObservedUnits our_units;
ObservedUnits our_structures;
ObservedUnits enemy_units;
ObservedUnits enemy_structures;

StrategyManager::StrategyManager(Kurt* parent_kurt) {
    kurt = parent_kurt;

    //current_plan = CreateDefaultGamePlan(kurt);
    //current_plan = RushPlan(kurt);
    current_plan = VespeneGasTycoon(kurt);
    //current_plan = DynamicGamePlan(kurt);
    current_plan->ExecuteNextNode();
}

void StrategyManager::OnStep(const ObservationInterface* observation) {
    int current_game_loop = observation->GetGameLoop();

    SaveSpottedEnemyUnits(observation);

    if (current_game_loop % 400 == 0) {
        PRINT("------Enemy units-----------")
            PRINT("\t|Total max health: " + to_string(enemy_units.GetTotalMaxHealth()) + "\t|")
            PRINT("\t|g2g CP: " + to_string(enemy_units.GetCombatPower()->g2g) + "\t|")
            PRINT("\t|g2a CP: " + to_string(enemy_units.GetCombatPower()->g2a) + "\t|")
            PRINT("\t|a2g CP: " + to_string(enemy_units.GetCombatPower()->a2g) + "\t|")
            PRINT("\t|a2a CP: " + to_string(enemy_units.GetCombatPower()->a2a) + "\t|")
        PRINT(enemy_units.ToString())
        PRINT("------Enemy structures-------")
        PRINT(enemy_structures.ToString())
        PRINT("------Our units--------------")
            PRINT("\t|Total max health: " + to_string(our_units.GetTotalMaxHealth()))
            PRINT("\t|g2g CP: " + to_string(our_units.GetCombatPower()->g2g) + "\t|")
            PRINT("\t|g2a CP: " + to_string(our_units.GetCombatPower()->g2a) + "\t|")
            PRINT("\t|a2g CP: " + to_string(our_units.GetCombatPower()->a2g) + "\t|")
            PRINT("\t|a2a CP: " + to_string(our_units.GetCombatPower()->a2a) + "\t|")
        PRINT(our_units.ToString())
        PRINT("------Our structures---------")
        PRINT(our_structures.ToString())
        PRINT("-----------------------------\n\n")
    }
}

void StrategyManager::OnUnitEnterVision(const Unit* unit) {
    if (ObservedUnits::unit_max_health.count(unit->unit_type) == 0) {
        ObservedUnits::unit_max_health.insert(pair<UNIT_TYPEID, float>(unit->unit_type, unit->health_max));
    }
}

void StrategyManager::SaveOurUnits(const Unit* unit) {
    // Save healthdata about this unit type if no data is available
    if (ObservedUnits::unit_max_health.count(unit->unit_type) == 0) {
        ObservedUnits::unit_max_health.insert(pair<UNIT_TYPEID, float>(unit->unit_type, unit->health_max));
    }

    if (kurt->IsStructure(unit)) {
        our_structures.AddUnit(unit);
    }
    else if(unit->unit_type != UNIT_TYPEID::TERRAN_SCV) { // Don't add SCVs because they are "created" when exiting refineries
        our_units.AddUnit(unit);
    }
}

void StrategyManager::RemoveDeadUnit(const Unit* unit) {
    if (unit->alliance == Unit::Alliance::Enemy) {
        if (kurt->IsStructure(unit)) {
            PRINT("ENEMY BUILDING DESTROYED")
            enemy_structures.RemoveUnit(unit);
        }
        else {
            PRINT("ENEMY UNIT KILLED")
            enemy_units.RemoveUnit(unit);
        }
    }
    else if (unit->alliance == Unit::Alliance::Self) {
        if (kurt->IsStructure(unit)) {
            PRINT("OUR BUILDING DESTROYED")
            our_structures.RemoveUnit(unit);
        }
        else if (unit->unit_type != UNIT_TYPEID::TERRAN_SCV) {
            PRINT("OUR UNIT KILLED")
            our_units.RemoveUnit(unit);
        }
    }
}

void StrategyManager::ExecuteSubplan() {
    current_plan->ExecuteNextNode();
}

void StrategyManager::CalculateNewPlan() {
    delete current_plan;
    current_plan =DynamicGamePlan(kurt);
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
    enemy_structures.AddUnits(&observed_structures);
    //SaveSpottedEnemyUnitsHelper(&observed_structures, &enemy_structures);

    //Save any newly observed units
    enemy_units.AddUnits(&observed_units);
    //SaveSpottedEnemyUnitsHelper(&observed_units, &enemy_units);
};

//TODO: Not needed anny more?
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

void StrategyManager::CalculateCombatMode() {
    const ObservedUnits::CombatPower* const our_cp = our_units.GetCombatPower();
    const ObservedUnits::CombatPower* const enemy_cp = enemy_units.GetCombatPower();

    if (our_cp->g2g > enemy_cp->g2g && our_cp->g2a > enemy_cp->a2g && our_cp->a2g > enemy_cp->g2a && our_cp->a2a > enemy_cp->a2a) {
        kurt->SetCombatMode(Kurt::ATTACK);
    }
    else if (our_cp->g2g < enemy_cp->g2g && our_cp->g2a < enemy_cp->a2g && our_cp->a2g < enemy_cp->g2a && our_cp->a2a < enemy_cp->a2a) {
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
    const ObservedUnits::CombatPower* const our_cp = our_units.GetCombatPower();
    const ObservedUnits::CombatPower* const enemy_cp = enemy_units.GetCombatPower();

    BPState* new_goal_state = new BPState();

    if (our_cp->g2g < 80 || our_cp->g2a < 80) {
        new_goal_state->SetUnitAmount(UNIT_TYPEID::TERRAN_MARINE, 10);
    }
    else if (our_cp->a2a < 50) {
        new_goal_state->SetUnitAmount(UNIT_TYPEID::TERRAN_VIKINGFIGHTER, 5);
    }
    else if (our_cp->a2a < enemy_cp->a2a || our_cp->g2a < enemy_cp->g2a) {
        new_goal_state->SetUnitAmount(UNIT_TYPEID::TERRAN_MARINE, 5);
        new_goal_state->SetUnitAmount(UNIT_TYPEID::TERRAN_LIBERATOR, 3);
    }
    else {
        new_goal_state = CounterEnemyUnit();
    }

    kurt->SendBuildOrder(new_goal_state);
};


BPState* StrategyManager::CounterEnemyUnit() {
    BPState* new_goal_state = new BPState();
    Unit unit_to_create;
    int number_of_units = 0;

    map <UNIT_TYPEID, int> *const current_enemy_units = enemy_units.GetSavedUnits();

    vector<sc2::UNIT_TYPEID> counter_units;
    map <UNIT_TYPEID, int> *const curr_our_units = our_units.GetSavedUnits();

    float enemy_max_health = 0;
    float final_enemy_cp = 0;

    float enemy_cp = 0;
    float tmp_enemy_cp = 0;
    float tmp_enemy_air_cp = 0;
    float tmp_enemy_ground_cp = 0;

    float our_health = 0;

    float our_final_health = 0;
    float our_cp = 0;
    float our_weapon_dps = 0;
    UNIT_TYPEID final_counter_unit;

    float tmp_our_cp = 0;
    float tmp_our_air_cp = 0;
    float tmp_our_ground_cp = 0;

    float tmp_diff_cp = 0;
    float diff_cp = 0;

    UNIT_TYPEID unit_to_counter;

    UnitTypeData* unit_data;

    float weapon_dps;

    bool final_is_flying;
    bool is_flying;

    for (auto enemy_unit = current_enemy_units->begin(); enemy_unit != current_enemy_units->end(); ++enemy_unit) {
        unit_to_counter = enemy_unit->first;
        unit_data = Kurt::GetUnitType(enemy_unit->first);
        for (auto weapon : unit_data->weapons) {
            weapon_dps = weapon.damage_ / weapon.speed;

            if (weapon.type == Weapon::TargetType::Any) {
                tmp_enemy_air_cp = weapon_dps * enemy_unit->second;
                tmp_enemy_ground_cp = weapon_dps * enemy_unit->second;
            }
            else if (weapon.type == Weapon::TargetType::Ground) {
                tmp_enemy_ground_cp = weapon_dps * enemy_unit->second;
            }
            else if (weapon.type == Weapon::TargetType::Air) {
                tmp_enemy_air_cp = weapon_dps * enemy_unit->second;
            }

            tmp_enemy_cp = max(tmp_enemy_air_cp, tmp_enemy_ground_cp);

            if (tmp_enemy_cp > enemy_cp) {
                enemy_cp = tmp_enemy_cp;
            }
        }

        counter_units = zerg_countertable.at(unit_to_counter);
        int our_number_of_units;
        int weapon_dps;
        for (auto counter_unit = counter_units.begin(); counter_unit != counter_units.end(); ++counter_unit) {
            if ((curr_our_units->count(*counter_unit)) == 1) {
                our_number_of_units = curr_our_units->at(*counter_unit);
                unit_data = Kurt::GetUnitType(*counter_unit);
                for (auto weapon : unit_data->weapons) {
                    is_flying = count(ObservedUnits::flying_units.begin(), ObservedUnits::flying_units.end(), unit_to_counter) == 1;
                    weapon_dps = weapon.damage_ / weapon.speed;
                    if (weapon.type == Weapon::TargetType::Any) {
                        tmp_our_air_cp = weapon_dps * our_number_of_units;
                        tmp_our_ground_cp = weapon_dps * our_number_of_units;
                    }
                    else if (weapon.type == Weapon::TargetType::Ground) {
                        tmp_our_ground_cp = weapon_dps *our_number_of_units;
                    }
                    else if (weapon.type == Weapon::TargetType::Air) {
                        tmp_our_air_cp = weapon_dps * our_number_of_units;
                    }
                }
                if (is_flying) {
                    tmp_our_cp += tmp_our_air_cp;
                }
                else {
                    tmp_our_cp += tmp_our_ground_cp;
                }
                our_health += our_units.CalculateUnitTypeMaxHealth(*counter_unit);
            }
        }

        tmp_diff_cp = enemy_cp - tmp_our_cp;
        if (tmp_diff_cp > diff_cp) {
            diff_cp = tmp_diff_cp;
            final_enemy_cp = enemy_cp;
            enemy_max_health = enemy_units.CalculateUnitTypeMaxHealth(enemy_unit->first);

            our_weapon_dps = weapon_dps;
            our_cp = tmp_our_cp;
            our_final_health = our_health;
            final_counter_unit = counter_units.back();
            final_is_flying = is_flying;
        }
    }

    while (our_final_health / final_enemy_cp < (1.1 * enemy_max_health / our_cp)) {
        number_of_units += 1;
        our_health += ObservedUnits::unit_max_health.at(final_counter_unit);
        if (is_flying) {
            our_cp += our_weapon_dps;
        }
        else {
            our_cp += our_weapon_dps;
        }
    }
    new_goal_state->SetUnitAmount(counter_units.back(), number_of_units);
    return new_goal_state;
}



/*
void StrategyManager::CounterEnemyUnits() {
BPState* new_goal_state = new BPState();
Unit unit_to_create;
int number_of_units;

vector<sc2::UNIT_TYPEID> counter_units;

map <UNIT_TYPEID, int> *const current_our_units = our_units.GetSavedUnits();

float enemy_max_health;
float enemy_dps;
float our_health;
float our_dps;

map <UNIT_TYPEID, int> *const current_enemy_units = enemy_units.GetSavedUnits();

for (auto unit = current_enemy_units->begin(); unit != current_enemy_units->end(); ++unit) {
// CHECK ENEMY UNITS
number_of_units = current_enemy_units->at(unit->first);
//enemy_dps =
enemy_max_health = enemy_units.CalculateUnitTypeMaxHealth(unit->first);

// CHECK OUR UNITS
counter_units = zerg_countertable.at(unit->first);
for (auto unit = counter_units.begin(); unit != counter_units.end(); ++unit) {
if ((current_our_units->count(*unit)) == 1) {
// our_dps += Calculate total dps of(our) counter_units;
// our_health += Calculate total health of(our) counter_units;
// remove units, used to calculate our_dps, from tmp;
}
}

// decide which units are most suitable to be created;

while (our_health / enemy_dps < (1.1 * enemy_max_health / our_dps)) {
number_of_units += 1;
our_health += counter_unit.health;
our_dps += counter_unit.dps; //kan absolut inte skrivas så enkelt, men det är ju pseudo.
}
new_goal_state->SetUnitAmount(unit_to_create->unit_type, number_of_units);
}
}
*/
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
