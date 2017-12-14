#include "strategy_manager.h"
#include "plans.h"
#include <algorithm>
#include "observed_units.h"

//TODO: Remove destroyed structures from enemy_structures (or our_strucutreS)

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

UNIT_TYPEID StrategyManager::current_best_counter_type;

StrategyManager::StrategyManager(Kurt* parent_kurt) {
    kurt = parent_kurt;
    progression_mode = false;

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

        const ObservedUnits::CombatPower* our_cp = our_units.GetCombatPower();
        const ObservedUnits::CombatPower* enemy_cp = enemy_units.GetCombatPower();

        PRINT("-----------Enemy units-----------")
            PRINT("|Air health: " << to_string(enemy_units.GetAirHealth()) << "\t\t|")
            PRINT("|Ground health: " << to_string(enemy_units.GetGroundHealth()) << "\t|")
            PRINT("|Air DPS: " << to_string(enemy_cp->GetAirCp()) << "\t\t|")
            PRINT("|Ground DPS: " << to_string(enemy_cp->GetGroundCp()) << "\t\t|")
        PRINT(enemy_units.ToString())
        /*PRINT("------Enemy structures-------")
        PRINT(enemy_structures.ToString())*/
        PRINT("-----------Our units-------------")
            PRINT("|Air health: " << to_string(our_units.GetAirHealth()) << "\t\t|")
            PRINT("|Ground health: " << to_string(our_units.GetGroundHealth()) << "\t|")
            PRINT("|Air DPS: " << to_string(our_cp->GetAirCp()) << "\t\t|")
            PRINT("|Ground DPS: " << to_string(our_cp->GetGroundCp()) << "\t\t|")
            PRINT(our_units.ToString())
            /*PRINT("------Our structures---------")
            PRINT(our_structures.ToString())*/
            PRINT("---------------------------------\n")

            CalculateCombatMode();
    } 

}

void StrategyManager::OnUnitEnterVision(const Unit* unit) {
    // Save how much health each type of new unit have
    if (ObservedUnits::unit_max_health.count(unit->unit_type) == 0) {
        ObservedUnits::unit_max_health.insert(pair<UNIT_TYPEID, float>(unit->unit_type, unit->health_max));
        UpdateCurrentBestCounterType();
        if (current_best_counter_type != ObservedUnits::current_best_counter_type) { //TODO: Check if this works correctly.
            progression_mode = false;
            CalculateNewPlan();
        }
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
        else if(Kurt::IsArmyUnit(unit)) {
            PRINT("ENEMY UNIT KILLED")
            enemy_units.RemoveUnit(unit);
        }
    }
    else if (unit->alliance == Unit::Alliance::Self) {
        if (kurt->IsStructure(unit)) {
            PRINT("OUR BUILDING DESTROYED")
            our_structures.RemoveUnit(unit);
        }
        else if (Kurt::IsArmyUnit(unit)) {
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
        else if (Kurt::IsArmyUnit(*observed_unit)) { // Only add military to the observation
            observed_units.push_back(*observed_unit); //TODO: Adding all units may be preferable.
        }
    }

    // Save any newly observed structures
    enemy_structures.AddUnits(&observed_structures);

    //Save any newly observed units
    enemy_units.AddUnits(&observed_units);
};

void StrategyManager::CalculateCombatMode() {
    Kurt::CombatMode current_combat_mode = kurt->GetCombatMode();
    const ObservedUnits::CombatPower* const our_cp = our_units.GetCombatPower();
    const ObservedUnits::CombatPower* const enemy_cp = enemy_units.GetCombatPower();
    const float attack_const = 10; // Lower values make the ai more agressive, but with riskier attacks
    const float defend_const = 7; // Higher value makes the ai retreat more quickly when outnumbered
    float c;
    int attack_score = 0;

    if (current_combat_mode == Kurt::ATTACK) {
        c = defend_const;
    }
    else {
        c = attack_const;
    }
    PRINT("\n-------Calculate CombatMode---------")

    // Do our air units have much health relative to the enemy air DPS? 
    if (our_units.GetAirHealth() > enemy_cp->GetAirCp() * c) {
        PRINT("We have good air health!")
        attack_score++;
    }
    // Do our ground units have much health relative to the enemy ground DPS? 
    if (our_units.GetGroundHealth() > enemy_cp->GetGroundCp() * c) {
        PRINT("We have good ground health!")
        attack_score++;
    }
    // Do we have high air DPS relative to the enemy's air units' health?
    if (enemy_units.GetAirHealth() < our_cp->GetAirCp() * c) {
        PRINT("We have good air DPS!")
        attack_score++;
    }
    // Do we have high ground DPS relative to the enemy's ground units' health?
    if (enemy_units.GetGroundHealth() < our_cp->GetGroundCp() * c) {
        PRINT("We have good ground DPS!")
        attack_score++;
    }

    // If we win in at least 2 aspects of combat, we can try to attack.
    // Else we have to retreat/continue to defend, and build up our army
    if (attack_score >= 2 && current_combat_mode != Kurt::ATTACK) {
        PRINT("ATTACK!")
        kurt->SetCombatMode(Kurt::ATTACK);
    }
    else if (attack_score < 2 && current_combat_mode != Kurt::DEFEND) {
        PRINT("RETREAT!")
        kurt->SetCombatMode(Kurt::DEFEND);
    }
    
    PRINT("---------------------\n")
};

void StrategyManager::SetBuildGoal() {
    const ObservedUnits::CombatPower* const our_cp = our_units.GetCombatPower();
    const ObservedUnits::CombatPower* const enemy_cp = enemy_units.GetCombatPower();
    int number_of_missile_turrets = our_structures.GetnumberOfUnits(UNIT_TYPEID::TERRAN_MISSILETURRET);
    BPState* new_goal_state = new BPState();

    // Add some amount of the currently best counter unit to the build order
    new_goal_state = enemy_units.GetStrongestUnit(our_units, kurt);
    current_best_counter_type = ObservedUnits::current_best_counter_type; //TODO: check if this works correctly.

    // Add 1 missle turret for every 100 hp the enemy air units have
    while (number_of_missile_turrets < enemy_units.GetAirHealth() / 100) {
        number_of_missile_turrets++;
        new_goal_state->IncreaseUnitAmount(UNIT_TYPEID::TERRAN_MISSILETURRET, 1);
    }

    kurt->SendBuildOrder(new_goal_state);
};

void StrategyManager::SetProgressionMode(bool new_progression_mode) {
    progression_mode = new_progression_mode;
}

bool StrategyManager::GetProgressionMode() {
    return progression_mode;
}

void StrategyManager::UpdateCurrentBestCounterType() {
    enemy_units.GetStrongestUnit(our_units, kurt);
}


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

    //ObservedUnits* strongest_enemy_unit = enemy_units.GetStrongestUnit(our_units);
    //ObservedUnits* best_counter_unit = our_units.GetBestCounterUnit();

    BPState* counter_order = enemy_units.GetStrongestUnit(our_units, kurt);
    current_best_counter_type = ObservedUnits::current_best_counter_type;
    //counter_order->SetUnitAmount(UNIT_TYPEID::TERRAN_THOR, 3);
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
        
    PRINT("----------------------------");
    PRINT("diff_cp: " << diff_cp);
    PRINT("final_enemy_cp: " << final_enemy_cp);
    PRINT("enemy_max_health: " << enemy_max_health);

    }
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
