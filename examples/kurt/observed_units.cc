#include "observed_units.h"
#include "kurt.h"

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
}

void ObservedUnits::AddUnit(const Unit* unit) {
    saved_units[unit->unit_type] += 1;
}

void ObservedUnits::RemoveUnit(const Unit* unit) {
    saved_units[unit->unit_type] -= 1;
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