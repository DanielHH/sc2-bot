#include "observed_units.h"
#include "kurt.h"
#include "countertable.h"
#include <algorithm>

using namespace std;
using namespace sc2;

#define DEBUG // Comment out to disable debug prints in this file.
#ifdef DEBUG
#include <iostream>
#define PRINT(s) std::cout << s << std::endl;
#define TEST(s) s
#else
#define PRINT(s)
#define TEST(s)
#endif // DEBUG

const vector<UNIT_TYPEID> ObservedUnits::flying_units = {
    UNIT_TYPEID::TERRAN_VIKINGFIGHTER, UNIT_TYPEID::TERRAN_MEDIVAC, UNIT_TYPEID::TERRAN_LIBERATOR, UNIT_TYPEID::TERRAN_LIBERATORAG,
    UNIT_TYPEID::TERRAN_RAVEN, UNIT_TYPEID::TERRAN_BANSHEE, UNIT_TYPEID::TERRAN_BATTLECRUISER, UNIT_TYPEID::TERRAN_POINTDEFENSEDRONE,
    UNIT_TYPEID::ZERG_OVERLORD, UNIT_TYPEID::ZERG_OVERLORDCOCOON, UNIT_TYPEID::ZERG_OVERLORDTRANSPORT, UNIT_TYPEID::ZERG_TRANSPORTOVERLORDCOCOON,
    UNIT_TYPEID::ZERG_OVERSEER, UNIT_TYPEID::ZERG_MUTALISK, UNIT_TYPEID::ZERG_CORRUPTOR, UNIT_TYPEID::ZERG_BROODLORD,
    UNIT_TYPEID::ZERG_BROODLORDCOCOON, UNIT_TYPEID::ZERG_VIPER, UNIT_TYPEID::PROTOSS_OBSERVER, UNIT_TYPEID::PROTOSS_WARPPRISM,
    UNIT_TYPEID::PROTOSS_PHOENIX, UNIT_TYPEID::PROTOSS_VOIDRAY, UNIT_TYPEID::PROTOSS_ORACLE, UNIT_TYPEID::PROTOSS_CARRIER,
    UNIT_TYPEID::PROTOSS_TEMPEST, UNIT_TYPEID::PROTOSS_MOTHERSHIPCORE, UNIT_TYPEID::PROTOSS_MOTHERSHIP
};

map <UNIT_TYPEID, float> ObservedUnits::unit_max_health;
UNIT_TYPEID ObservedUnits::current_best_counter_type;

ObservedUnits::ObservedUnits() {
    cp = CombatPower();
}

void ObservedUnits::AddUnits(const Units* units) {
    map<UNIT_TYPEID, int> new_units;

    // Save all units in the vector in a temporary map
    for (auto new_unit = units->begin(); new_unit != units->end(); ++new_unit) {
        new_units[(*new_unit)->unit_type] += 1;
    }

    // If the amount of new units are more than the previously saved amount, update the value
    for (auto unit_id = new_units.begin(); unit_id != new_units.end(); ++unit_id) {
        if (new_units[unit_id->first] > saved_units[unit_id->first]) {
            saved_units[unit_id->first] = new_units[unit_id->first];
        }
    }

    calculateCP();
}

void ObservedUnits::AddUnits(const sc2::UNIT_TYPEID unit_type, const int amount) {
    saved_units[unit_type] += amount;
    calculateCP();
}

void ObservedUnits::AddUnits(const Unit* unit, const int amount) {
    saved_units[unit->unit_type] += amount;
    calculateCP();
}

void ObservedUnits::RemoveUnit(const Unit* unit) {
    saved_units[unit->unit_type] -= 1;
    calculateCP();
}

void ObservedUnits::RemoveUnit(const sc2::UNIT_TYPEID unit_type) {
    saved_units[unit_type] -= 1;
    calculateCP();
}

// TODO: Check if calculation is wrong
void ObservedUnits::calculateCP() { //TODO: Remove DPS with dead units.
    // Reset cp-data.
    cp.g2g = 0;
    cp.g2a = 0;
    cp.a2g = 0;
    cp.a2a = 0;

    UnitTypeData* unit_data;
    float weapon_dps = 0;

    for (auto unit = saved_units.begin(); unit != saved_units.end(); ++unit) {
        unit_data = Kurt::GetUnitType(unit->first);
        bool is_flying = count(flying_units.begin(), flying_units.end(), unit->first) == 1;

        for (auto weapon : unit_data->weapons) {
            weapon_dps = weapon.damage_ * weapon.attacks / weapon.speed; // This is correct assuming damage_ == damage_ per attack
            if (weapon.type == Weapon::TargetType::Any) { // Kolla upp om targettype::any är samma sak som air och ground, och det kommer dubbleras eller ej.
                // GroundToBoth
                if (!is_flying) {
                    cp.g2a += weapon_dps * unit->second;
                    cp.g2g += weapon_dps * unit->second;
                }
                // AirToBoth
                if (is_flying) {
                    cp.a2a += weapon_dps * unit->second;
                    cp.a2g += weapon_dps * unit->second;
                }
            }
            else if (weapon.type == Weapon::TargetType::Ground) {
                // GroundToGround
                if (!is_flying) {
                    cp.g2g += weapon_dps * unit->second;
                }
                // AirToGround
                else if (is_flying) {
                    cp.a2g += weapon_dps * unit->second;
                }
            }
            else if (weapon.type == Weapon::TargetType::Air) {
                // GroundToAir
                if (!is_flying) {
                    cp.g2a += weapon_dps * unit->second;
                }
                // AirToAir
                if (is_flying) {
                    cp.a2a += weapon_dps * unit->second;
                }
            }
        }
    }
}

const ObservedUnits::CombatPower* const ObservedUnits::GetCombatPower() {
    return &cp;
}

float ObservedUnits::GetTotalMaxHealth() {
    float total_max_health = 0;

    // Get total max health for all unit types and summerize
    for (auto unit = saved_units.begin(); unit != saved_units.end(); ++unit) {
        total_max_health += CalculateUnitTypeMaxHealth(unit->first);
    }

    return total_max_health;
}

float ObservedUnits::CalculateUnitTypeMaxHealth(UNIT_TYPEID unit_type) {
    if (unit_max_health.count(unit_type) == 1) {
        int number_of_units = saved_units.at(unit_type);
        float max_health_per_unit = unit_max_health.at(unit_type);
        return number_of_units * max_health_per_unit;
    }
    else {
        return 0;
    }
}

float ObservedUnits::GetAirHealth() {
    float air_health = 0;
    UNIT_TYPEID unit_type;

    for (auto saved_unit = saved_units.begin(); saved_unit != saved_units.end(); ++saved_unit) {
        unit_type = saved_unit->first;
        if (count(flying_units.begin(), flying_units.end(), unit_type) == 1) {
            air_health += CalculateUnitTypeMaxHealth(unit_type);
        }
    }

    return air_health;
}

float ObservedUnits::GetGroundHealth() {
    float ground_health = 0;
    UNIT_TYPEID unit_type;

    for (auto saved_unit = saved_units.begin(); saved_unit != saved_units.end(); ++saved_unit) {
        unit_type = saved_unit->first;
        if (count(flying_units.begin(), flying_units.end(), unit_type) == 0) {
            ground_health += CalculateUnitTypeMaxHealth(unit_type);
        }
    }

    return ground_health;
}

map <UNIT_TYPEID, int> *const ObservedUnits::GetSavedUnits() {
    return &saved_units;
}

BPState* ObservedUnits::GetStrongestUnit(ObservedUnits enemy_units, Kurt* kurt) {
    ObservedUnits* strongest_unit = nullptr;
    map<UNIT_TYPEID, int> *const enemy_units_map = enemy_units.GetSavedUnits();
    float max_cp_difference = 0;
    UNIT_TYPEID strongest_unit_type;
    bool current_unit_is_flying;

    PRINT("---------Finding counter unit---------")
    // Go through all unit types and find which type is the strongest against the units in enemy_units
    for (auto saved_unit = saved_units.begin(); saved_unit != saved_units.end(); ++saved_unit) {
        UNIT_TYPEID current_unit_type = saved_unit->first;
        int number_of_units = saved_unit->second;

        // Create a new ObservedUnits for only the current type of unit, to get their cp and health easy
        ObservedUnits* current_unit = new ObservedUnits();
        current_unit->AddUnits(current_unit_type, number_of_units);

        const CombatPower* current_unit_cp = current_unit->GetCombatPower();

        // If no counter units are implemented for the current unit, skip it
        if (countertable.count(current_unit_type) == 0) {
            delete current_unit;
            continue;
        }

        // Get all units good at countering the current unit
        vector<UNIT_TYPEID> counter_unit_types = countertable.at(current_unit_type);
        ObservedUnits counter_units = ObservedUnits();

        // Save the amount of each counter unit the enemy currently have
        for (auto counter_unit = counter_unit_types.begin(); counter_unit != counter_unit_types.end(); ++counter_unit) {
            if ((enemy_units_map->count(*counter_unit)) == 1) {
                int number_of_counter_units = enemy_units_map->at(*counter_unit);
                counter_units.AddUnits(*counter_unit, number_of_counter_units);
            }
        }

        const ObservedUnits::CombatPower* counter_units_cp = counter_units.GetCombatPower();

        current_unit_is_flying = count(flying_units.begin(), flying_units.end(), current_unit_type) == 1;

        float current_unit_total_cp = 0;
        float counter_units_total_cp = 0;
        float cp_difference = 0;

        // Combine the combat power of all our counter units
        if (current_unit_is_flying) {
            counter_units_total_cp = counter_units_cp->GetAirCp();
        }
        else {
            counter_units_total_cp = counter_units_cp->GetGroundCp();
        }

        // Combine the combat power of the current unit
        current_unit_total_cp = current_unit_cp->GetAirCp() + current_unit_cp->GetGroundCp();

        cp_difference = current_unit_total_cp - counter_units_total_cp;

        if (cp_difference > max_cp_difference) {
            if (strongest_unit != nullptr) {
                delete strongest_unit;
            }
            max_cp_difference = cp_difference;
            strongest_unit = current_unit;
            strongest_unit_type = current_unit_type; //Just for print
        }
        else {
            delete current_unit;
        }
    }

    BPState* best_counter_unit = new BPState();
    if (strongest_unit != nullptr) {
        if (kurt->GetProgressionMode()) {
            kurt->SetProgressionMode(false);
            current_best_counter_type = UNIT_TYPEID::INVALID;
        }

        bool strongest_unit_is_flying = count(flying_units.begin(), flying_units.end(), strongest_unit_type) == 1;
        PRINT("Enemys strongest unit: " << Kurt::GetUnitType(strongest_unit_type)->name)
        PRINT("Enemy is flying: " << to_string(strongest_unit_is_flying))
        best_counter_unit = GetBestCounterUnit(strongest_unit, strongest_unit_type, max_cp_difference, kurt, strongest_unit_is_flying);
    }
    else {
        // TODO: progression mode!
        PRINT("Enemys strongest unit: Nothing to counter!")
            if (kurt->GetProgressionMode()) {
                best_counter_unit->SetUnitAmount(current_best_counter_type, 1);
                best_counter_unit->SetUnitAmount(UNIT_TYPEID::TERRAN_MARINE, 1); //They're really good anti-air
            }
            else {
                kurt->SetProgressionMode(true);
                best_counter_unit->SetUnitAmount(UNIT_TYPEID::TERRAN_BATTLECRUISER, 10);
            }
    }

    PRINT("--------------------------------------\n")

    delete strongest_unit; // strongest_unit not needed any more

    return best_counter_unit;
}

BPState* ObservedUnits::GetBestCounterUnit(ObservedUnits* strongest_enemy, UNIT_TYPEID strongest_enemy_type, float cp_difference, Kurt* kurt, bool strongest_unit_is_flying) {
    ObservedUnits* best_counter_unit = nullptr;
    const ObservedUnits::CombatPower* strongest_enemy_cp = strongest_enemy->GetCombatPower();
    vector<UNIT_TYPEID> counter_unit_types = countertable.at(strongest_enemy_type);
    bool counter_unit_is_flying;

    float enemy_air_cp = strongest_enemy_cp->a2a + strongest_enemy_cp->g2a;
    float enemy_ground_cp = strongest_enemy_cp->a2g + strongest_enemy_cp->g2g;

    // Loop through counter units and find the strongest one
    for (auto counter_unit_type = counter_unit_types.begin(); counter_unit_type != counter_unit_types.end(); ++counter_unit_type) {
        ObservedUnits* current_counter_unit = new ObservedUnits();
        current_counter_unit->AddUnits(*counter_unit_type, 1); 
        counter_unit_is_flying = count(flying_units.begin(), flying_units.end(), *counter_unit_type) == 1;

        // Try to find a flying counter unit if the enemy has weak anti-air
        // and a ground counter unit if they have weak ground cp
        if (((enemy_air_cp < enemy_ground_cp) && counter_unit_is_flying) || ((enemy_air_cp > enemy_ground_cp) && !counter_unit_is_flying)) { // check if there is a weakness, and add.
            if (best_counter_unit != nullptr) {
                delete best_counter_unit;
            }
            best_counter_unit = current_counter_unit;
        }
        // If no "good" counter unit was found, just take the first one in the list
        else if (best_counter_unit == nullptr) {
            best_counter_unit = current_counter_unit;
        }
        else {
            delete current_counter_unit;
        }
    }

    const ObservedUnits::CombatPower* counter_unit_cp = best_counter_unit->GetCombatPower();
    float add_cp = 0;

    if (strongest_unit_is_flying) {
        add_cp = counter_unit_cp->GetAirCp();
    }
    else {
        add_cp = counter_unit_cp->GetGroundCp();
    }

    int number_of_counter_units = 0;
    const float buffer_cp = -10;
    while (cp_difference > buffer_cp) {
        number_of_counter_units += 1;
        cp_difference -= add_cp;
    }

    UNIT_TYPEID best_counter_type = best_counter_unit->saved_units.begin()->first;
    delete best_counter_unit; // best_counter_units not needed any more
    PRINT("Best counter unit found: " << Kurt::GetUnitType(best_counter_type)->name)
    PRINT("Number of counter units needed: " << to_string(number_of_counter_units))
    BPState* counter_order = new BPState(); //TODO: Add a BPState that saves all former build_order, and just add on to that when creating a new buildorder.
    if (current_best_counter_type != best_counter_type) {
        current_best_counter_type = best_counter_type;
    }
    counter_order->SetUnitAmount(best_counter_type, number_of_counter_units);
    return counter_order;
}

int ObservedUnits::GetNumberOfAirUnits() {
    int number_of_air_units = 0;
    UNIT_TYPEID unit_type;
    int amount;
    bool is_flying;

    for (auto saved_unit = saved_units.begin(); saved_unit != saved_units.end(); ++saved_unit) {
       unit_type = saved_unit->first;
       amount = saved_unit->second;
       is_flying = count(flying_units.begin(), flying_units.end(), unit_type) == 1;

       if (is_flying) {
           number_of_air_units += amount;
       }
    }

    return number_of_air_units;
}

int ObservedUnits::GetNumberOfGroundUnits() {
    int number_of_ground_units = 0;
    UNIT_TYPEID unit_type;
    int amount;
    bool is_flying;

    for (auto saved_unit = saved_units.begin(); saved_unit != saved_units.end(); ++saved_unit) {
        unit_type = saved_unit->first;
        amount = saved_unit->second;
        is_flying = count(flying_units.begin(), flying_units.end(), unit_type) == 1;

        if (!is_flying) {
            number_of_ground_units += amount;
        }
    }

    return number_of_ground_units;
}

int ObservedUnits::GetnumberOfUnits(sc2::UNIT_TYPEID unit_type) {
    int number_of_units = 0;
    number_of_units += saved_units[unit_type];
    return number_of_units;
}

string ObservedUnits::ToString() {
    string str;

    for (auto unit = saved_units.begin(); unit != saved_units.end(); ++unit) {
        str += Kurt::GetUnitType(unit->first)->name + ": ";
        str += to_string(unit->second);
        str += "\n";
    }

    return str;

}

#undef DEBUG // Stop debug prints from leaking
#undef TEST
#undef PRINT