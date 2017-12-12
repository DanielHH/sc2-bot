#include "strategy_manager.h"
#include "plans.h"
#include <algorithm>
#include "observed_units.h"

//TODO: Remove destroyed structures from enemy_structures (or our_strucutreS)

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

ObservedUnits our_units;
ObservedUnits our_structures;
ObservedUnits enemy_units;
ObservedUnits enemy_structures;

StrategyManager::StrategyManager(Kurt* parent_kurt) {
    kurt = parent_kurt;

    //current_plan = CreateDefaultGamePlan(kurt);
    current_plan = RushPlan(kurt);
    //current_plan = VespeneGasTycoon(kurt);
    //current_plan = DynamicGamePlan(kurt);
    //current_plan = DefendGamePlan(kurt);
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
        our_structures.AddUnits(unit);
    }
    else if(unit->unit_type != UNIT_TYPEID::TERRAN_SCV) { // Don't add SCVs because they are "created" when exiting refineries
        our_units.AddUnits(unit);
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
    current_plan = DynamicGamePlan(kurt);
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

    if (our_cp->g2g >= enemy_cp->g2g && our_cp->g2a >= enemy_cp->a2g && our_cp->a2g >= enemy_cp->g2a && our_cp->a2a >= enemy_cp->a2a) {
        kurt->SetCombatMode(Kurt::ATTACK);
        PRINT("COMBAT MODE: ATTACK")
    }
    else if (our_cp->g2g < enemy_cp->g2g && our_cp->g2a < enemy_cp->a2g && our_cp->a2g < enemy_cp->g2a && our_cp->a2a < enemy_cp->a2a) {
        kurt->SetCombatMode(Kurt::DEFEND);
        PRINT("COMBAT MODE: DEFEND")
    }
    else {
        kurt->SetCombatMode(Kurt::HARASS);
        PRINT("COMBAT MODE: HARASS")
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

   /* if (our_cp->g2g < 80 || our_cp->g2a < 80) {
        new_goal_state->SetUnitAmount(UNIT_TYPEID::TERRAN_MARINE, 10);
    }
    else {
        new_goal_state = CounterEnemyUnit();
    }*/
    PRINT("---------------------------")
    PRINT("In SetBuildGoal")
    new_goal_state = CounterEnemyUnit();
    kurt->SendBuildOrder(new_goal_state);
};


BPState* StrategyManager::CounterEnemyUnit() { //TODO: Fixa så att vi inte har enemy x, enemy y counterunit-problemet
    PRINT("---------------------------")
    PRINT("In CounterEnemyUnit")
    /*BPState* new_goal_state = new BPState();
    UNIT_TYPEID final_counter_unit;
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
    float final_our_cp = 0;
    float our_weapon_dps = 0;

    float tmp_our_cp = 0;
    float tmp_our_air_cp = 0;
    float tmp_our_ground_cp = 0;

    float tmp_diff_cp = 0;
    float diff_cp = 0;

    UNIT_TYPEID final_unit_to_counter;
    UNIT_TYPEID unit_to_counter;

    UnitTypeData* unit_data;

    float weapon_dps;

    bool final_is_flying;
    bool is_flying;*/

    map <UNIT_TYPEID, int> *const current_enemy_units = enemy_units.GetSavedUnits();
    map <UNIT_TYPEID, int> *const curr_our_units = our_units.GetSavedUnits();
    vector<sc2::UNIT_TYPEID> counter_units;
    float max_cp_difference;
    int number_of_strongest_units;

    ObservedUnits* strongest_enemy_unit = enemy_units.GetStrongestUnit(our_units);

    BPState* counter_order = new BPState();
    counter_order->SetUnitAmount(UNIT_TYPEID::TERRAN_THOR, 3);
    return counter_order;
}

        /*unit_to_counter = enemy_unit->first;
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

        int our_number_of_units;
        int weapon_dps;

        // Summerize the total DPS of all our current counter units
        is_flying = count(ObservedUnits::flying_units.begin(), ObservedUnits::flying_units.end(), unit_to_counter) == 1;
        for (auto counter_unit = counter_units.begin(); counter_unit != counter_units.end(); ++counter_unit) {
            if ((curr_our_units->count(*counter_unit)) == 1) {
                our_number_of_units = curr_our_units->at(*counter_unit);
                unit_data = Kurt::GetUnitType(*counter_unit);
                for (auto weapon : unit_data->weapons) {
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

                    if (is_flying && tmp_our_air_cp > tmp_our_cp) {
                        tmp_our_cp = tmp_our_air_cp;
                    }
                    else if (tmp_our_ground_cp > tmp_our_cp) {
                        tmp_our_cp = tmp_our_ground_cp;
                    }
                }
                our_cp += tmp_our_cp;
                our_health += our_units.CalculateUnitTypeMaxHealth(*counter_unit);
            }
        }

        tmp_diff_cp = enemy_cp - our_cp;
        if (tmp_diff_cp > diff_cp) {
            diff_cp = tmp_diff_cp;
            final_enemy_cp = enemy_cp;
            enemy_max_health = enemy_units.CalculateUnitTypeMaxHealth(unit_to_counter);

            our_weapon_dps = weapon_dps;
            final_our_cp = tmp_our_cp;
            our_final_health = our_health;
            final_counter_unit = counter_units.back(); //TODO: FöRBÄTTRA COUNTERUNIT-VALET. t ex kolla enemy anti-ground vs anti-air. GÖr den unit som skulle ta minst damage.
            final_is_flying = is_flying;
            final_unit_to_counter = unit_to_counter;
        }
<<<<<<< HEAD
        
    PRINT("----------------------------");
    PRINT("diff_cp: " << diff_cp);
    PRINT("final_enemy_cp: " << final_enemy_cp);
    PRINT("enemy_max_health: " << enemy_max_health);
=======
    }

>>>>>>> 49db4116d0b2219b7a5f1835636690e9dcc6ace7
    if (diff_cp > 0) {
        string str_futc = Kurt::GetUnitType(final_unit_to_counter)->name;
        PRINT("----------------------------")
        PRINT("Unit to counter: " << str_futc)
        PRINT("diff_cp: " << diff_cp);
        PRINT("final_enemy_cp: " << final_enemy_cp);
        PRINT("enemy_max_health: " << enemy_max_health);
        while (our_final_health/final_enemy_cp < (3*enemy_max_health/final_our_cp)) { // TODO: testa. eventuellt ändra till our_final_health * final_our_cp < 3*enemy_max_health*final_enemy_cp
            PRINT("----------------------------")
            PRINT("our_final_health/final_enemy_cp: " << our_final_health/final_enemy_cp)
            PRINT("enemy_max_health/final_our_cp: " << enemy_max_health/final_our_cp)
            PRINT("3*enemy_max_health/final_our_cp: " << 3*enemy_max_health/final_our_cp)
            PRINT("----------------------------")
            PRINT("diff_cp: " << diff_cp);
            PRINT("final_enemy_cp: " << final_enemy_cp);
            number_of_units += 1;
            PRINT("----------------------------");
            PRINT("IN WHILE! " << number_of_units);
            our_final_health += ObservedUnits::unit_max_health.at(final_counter_unit);
            if (final_is_flying) {
                final_our_cp += our_weapon_dps;
            }
            else {
                final_our_cp += our_weapon_dps;
            }
        }
        PRINT("Total cp on added units: " << final_our_cp)
    }
    else {
        PRINT("-------------------")
        PRINT("In Progression mode!")
        PRINT("ATTACK")
        PRINT("--------------------")
        kurt->SetCombatMode(Kurt::ATTACK);
        // TODO: Exempelvis (1) kalla på en funktion som jobbar mot Battlecruisers. Eller (2) göra en progressionfunction, t ex vi har marines, och
        // vikings, men inga liberators => Gör liberators, eller (3) satsa på units som är bra på att förstöra structures.
        if ((curr_our_units->count(UNIT_TYPEID::TERRAN_MARINE) == 0) || curr_our_units->at(UNIT_TYPEID::TERRAN_MARINE) < 5) {
            new_goal_state->SetUnitAmount(UNIT_TYPEID::TERRAN_MARINE, 2);
        }
        else if ((curr_our_units->count(UNIT_TYPEID::TERRAN_REAPER) == 0) || curr_our_units->at(UNIT_TYPEID::TERRAN_REAPER) < 3) {
            new_goal_state->SetUnitAmount(UNIT_TYPEID::TERRAN_REAPER, 2);
        }
        else if ((curr_our_units->count(UNIT_TYPEID::TERRAN_HELLION) == 0) || curr_our_units->at(UNIT_TYPEID::TERRAN_HELLION) < 2) {
            new_goal_state->SetUnitAmount(UNIT_TYPEID::TERRAN_MARINE, 2);
            new_goal_state->SetUnitAmount(UNIT_TYPEID::TERRAN_HELLION, 2);
        }
        else if ((curr_our_units->count(UNIT_TYPEID::TERRAN_MEDIVAC) == 0) || curr_our_units->at(UNIT_TYPEID::TERRAN_MEDIVAC) < 2) {
            new_goal_state->SetUnitAmount(UNIT_TYPEID::TERRAN_MARINE, 6);
            new_goal_state->SetUnitAmount(UNIT_TYPEID::TERRAN_MEDIVAC, 2);
        }
        else if ((curr_our_units->count(UNIT_TYPEID::TERRAN_VIKINGFIGHTER) == 0) || curr_our_units->at(UNIT_TYPEID::TERRAN_VIKINGFIGHTER) < 2) {
            new_goal_state->SetUnitAmount(UNIT_TYPEID::TERRAN_VIKINGFIGHTER, 3);
            new_goal_state->SetUnitAmount(UNIT_TYPEID::TERRAN_MARINE, 4);
        }
        else {
            new_goal_state->SetUnitAmount(UNIT_TYPEID::TERRAN_VIKINGFIGHTER, 1);
            new_goal_state->SetUnitAmount(UNIT_TYPEID::TERRAN_MARINE, 3);
            new_goal_state->SetUnitAmount(UNIT_TYPEID::TERRAN_MEDIVAC, 1);
        }
        /*
        final_counter_unit = UNIT_TYPEID::TERRAN_REAPER;
        number_of_units = 2;
        
        return new_goal_state;
    }
    PRINT("-------------------")
    PRINT("In countermode")
    PRINT("HARASS")
    PRINT("--------------------")
    kurt->SetCombatMode(Kurt::HARASS);
    PRINT("----------------------------");
    string str_fcu = Kurt::GetUnitType(final_counter_unit)->name;
    PRINT("Final Counter Unit: " << str_fcu)
    PRINT("number of units: " << number_of_units)
    PRINT("----------------------------");
    new_goal_state->SetUnitAmount(final_counter_unit, number_of_units);
    return new_goal_state;
    */

#undef DEBUG
#undef PRINT
#undef TEST
