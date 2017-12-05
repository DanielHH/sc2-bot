#include "observed_units.h"
#include "kurt.h"
#include <algorithm>

using namespace std;
using namespace sc2;

//#define DEBUG // Comment out to disable debug prints in this file.
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
    UNIT_TYPEID::ZERG_BROODLORDCOCOON, UNIT_TYPEID::ZERG_VIPER
};

ObservedUnits::ObservedUnits() {}

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

void ObservedUnits::AddUnit(const Unit* unit) {
    saved_units[unit->unit_type] += 1;
    calculateCP();
}

void ObservedUnits::RemoveUnit(const Unit* unit) {
    saved_units[unit->unit_type] -= 1;
}

void ObservedUnits::calculateCP() {
    // Reset cp-data.
    cp.g2g = 0;
    cp.g2a = 0;
    cp.a2g = 0;
    cp.a2a = 0;

    UnitTypeData* unit_data;
    float weapon_dps;

    for (auto unit = saved_units.begin(); unit != saved_units.end(); ++unit) {
        unit_data = Kurt::GetUnitType(unit->first);
        for (auto weapon : unit_data->weapons) {
            bool is_flying = count(flying_units.begin(), flying_units.end(), unit->first) == 1;
            weapon_dps = weapon.damage_ / weapon.speed; // This is correct assuming damage_ == damage_ per attack
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

string ObservedUnits::ToString() {
    string str;

    for (auto unit_id = saved_units.begin(); unit_id != saved_units.end(); ++unit_id) {
        str += Kurt::GetUnitType(unit_id->first)->name + ": ";
        str += to_string(unit_id->second);
        str += "\n";
    }

    return str;
}

#undef DEBUG // Stop debug prints from leaking
#undef TEST
#undef PRINT