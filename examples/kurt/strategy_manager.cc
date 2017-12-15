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
bool StrategyManager::dynamic_flag = false;

StrategyManager::StrategyManager(Kurt* parent_kurt) {
    kurt = parent_kurt;
    progression_mode = false;

    //current_plan = CreateDefaultGamePlan(kurt);
    //current_plan = RushPlan(kurt);
    current_plan = VespeneGasTycoon(kurt);
    //current_plan = FlyingScout(kurt);
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
            if (dynamic_flag) {
                CalculateCombatMode();
                UpdateCurrentBestCounterType();
                if (progression_mode) {
                    AddToBuildGoal();
                }
                else if (current_best_counter_type != ObservedUnits::current_best_counter_type) {
                    AddToBuildGoal();
                }
        }
    } 

}

void StrategyManager::OnUnitEnterVision(const Unit* unit) {
    // Save how much health each type of new unit have
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
    else if(Kurt::IsArmyUnit(unit)) { // Don't add SCVs because they are "created" when exiting refineries
        our_units.AddUnits(unit);
    }
}

void StrategyManager::RemoveDeadUnit(const Unit* unit) {
    if (unit->alliance == Unit::Alliance::Enemy) {
        if (kurt->IsStructure(unit)) {
            enemy_structures.RemoveUnit(unit);
        }
        else {
            enemy_units.RemoveUnit(unit);
        }
    }
    else if (unit->alliance == Unit::Alliance::Self) {
        if (kurt->IsStructure(unit)) {
            our_structures.RemoveUnit(unit);
        }
        else if (Kurt::IsArmyUnit(unit)) {
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
    dynamic_flag = true;
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
        else { // Only add military to the observation
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
    // Don't set defend_const lower than attack_const, or the modes will just swith back and forth
    // High our_health_attack = bigger army before we attack, while lower attacks with smaller armies.
    // High our_health_defence = Retreat when only a little outnumbered, while lower means we will continue
    // High our_cp_attack = smaller army before we attack, while lower attacks with smaller armies.
    // High our_cp_defence = Retreat only when very outnumbered, while lower means we will continue
    // attack even though we take heavy losses.
    // Aggressive playstyle => Low our_health_attack, Lower our_health_defence, High our_cp_attack, Higher our_cp_defence
    // Defensive playstyle => High our_health_attack, High our_health_defence, Low our_cp_attack, Low our_cp_defence
    const float our_health_attack = 5;
    const float our_health_defence = 4;
    const float our_cp_attack = 7;
    const float our_cp_defence = 8;
    float c;
    float d;
    int attack_score = 0;

    if (current_combat_mode == Kurt::ATTACK) {
        c = our_health_defence;
        d = our_cp_defence;
    }
    else {
        c = our_health_attack;
        d = our_cp_attack;
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
    if ((enemy_units.GetAirHealth() < our_cp->GetAirCp() * d) && (enemy_units.GetNumberOfAirUnits() > 0)) { //TODO: Om vi t ex har a2a och fienden inte har några a2a eller a2g, lär vi ju inte attackera med dem.
        PRINT("We have good air DPS!")
        attack_score++;
    }
    // Do we have high ground DPS relative to the enemy's ground units' health?
    if ((enemy_units.GetGroundHealth() < our_cp->GetGroundCp() * d) && (enemy_units.GetNumberOfGroundUnits() > 0)) {
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
    BPState* new_goal_state = new BPState();

    // Add some amount of the currently best counter unit to the build order
    new_goal_state = enemy_units.GetStrongestUnit(our_units, kurt);
    current_best_counter_type = ObservedUnits::current_best_counter_type; //TODO: check if this works correctly.
    
    PRINT("###CONTACT BUILDGOAL (SET)###")
    kurt->SendBuildOrder(new_goal_state);
    StuffWeLikeToHave();
};

void StrategyManager::AddToBuildGoal() {
    const ObservedUnits::CombatPower* const our_cp = our_units.GetCombatPower();
    const ObservedUnits::CombatPower* const enemy_cp = enemy_units.GetCombatPower();
    BPState* new_goal_state = new BPState();
    BPState* stuff = new BPState();

    // Add some amount of the currently best counter unit to the build order
    new_goal_state = enemy_units.GetStrongestUnit(our_units, kurt);
    current_best_counter_type = ObservedUnits::current_best_counter_type; //TODO: check if this works correctly.

    PRINT("###CONTACT BUILDGOAL (ADD)###")
    kurt->AddToBuildOrder(new_goal_state);
    StuffWeLikeToHave();
};

void StrategyManager::StuffWeLikeToHave() {
    BPState* stuff = new BPState();
    int number_of_missile_turrets = our_structures.GetnumberOfUnits(UNIT_TYPEID::TERRAN_MISSILETURRET);
    // Add 1 missle turret for every 100 hp the enemy air units have
    while (number_of_missile_turrets < (enemy_units.GetAirHealth() / 100)) {
        number_of_missile_turrets++;
        stuff->IncreaseUnitAmount(UNIT_TYPEID::TERRAN_MISSILETURRET, 1);
    }
    int number_of_marines = our_units.GetnumberOfUnits(UNIT_TYPEID::TERRAN_MARINE);
    while (number_of_marines < (our_units.GetNumberOfGroundUnits() / 5)) {
        number_of_marines++;
        stuff->IncreaseUnitAmount(UNIT_TYPEID::TERRAN_MARINE, 1);
    }
    int number_of_medivacs = our_units.GetnumberOfUnits(UNIT_TYPEID::TERRAN_MEDIVAC);
    while (number_of_medivacs < (number_of_marines / 6)) {
        number_of_medivacs++;
        stuff->IncreaseUnitAmount(UNIT_TYPEID::TERRAN_MEDIVAC, 1);
    }
    kurt->AddToBuildOrder(stuff);
}

void StrategyManager::SetProgressionMode(bool new_progression_mode) {
    progression_mode = new_progression_mode;
}

bool StrategyManager::GetProgressionMode() {
    return progression_mode;
}

void StrategyManager::UpdateCurrentBestCounterType() {
    BPState* check_current_best = enemy_units.GetStrongestUnit(our_units, kurt);
    delete check_current_best;
}

#undef DEBUG
#undef PRINT
#undef TEST
