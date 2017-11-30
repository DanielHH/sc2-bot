#include "BPState.h"

#include "constants.h"
#include "kurt.h"

#include <sc2api/sc2_api.h>

#include <iostream>
#include <fstream>
#include <iterator>
#include <map>

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

BPState::BPState() {
}

BPState::BPState(BPState * const state) {
    for (auto it = state->UnitsBegin(); it != state->UnitsEnd(); ++it) {
        SetUnitAmount(it->first, it->second);
    }
    minerals = state->GetMinerals();
    vespene = state->GetVespene();
    food_cap = state->GetFoodCap();
    food_used = state->GetFoodUsed();
}

BPState::BPState(const ObservationInterface* observation, Kurt * const kurt) {
    for (auto unit : observation->GetUnits(Unit::Alliance::Self)) {
        UNIT_TYPEID type = unit->unit_type.ToType();
        SetUnitAmount(type, GetUnitAmount(type) + 1);
    }
    SetUnitAmount(UNIT_FAKEID::TERRAN_SCV_MINERALS, kurt->scv_minerals.size());
    SetUnitAmount(UNIT_FAKEID::TERRAN_SCV_VESPENE, kurt->scv_vespene.size());
    minerals = observation->GetMinerals();
    vespene = observation->GetVespene();
    food_cap = observation->GetFoodCap();
    food_used = observation->GetFoodUsed();
}

BPState::BPState(BPState const * const initial, BPAction const * const step) {
    // TODO
}

BPState::~BPState() {
    // TODO
}

std::vector<BPAction *> BPState::AvailableActions() const {
    std::vector<BPAction *> tmp;
    std::ifstream abilities_file;
    TEST(( abilities_file.open("sc2-gamedata/v3.19.1.58600/units.json");


    std::map<UNIT_TYPEID, std::vector<ABILITY_ID>> ability_map;

    for (std::string line; abilities_file >> line;) {

    } ))
    

    
    return tmp;
}

int BPState::GetUnitAmount(UNIT_TYPEID type) {
    // Need to test if element exist to prevent allocating more values
    if (unit_amount.count(type) == 0) {
        return 0;
    } else {
        return unit_amount[type];
    }
}

void BPState::SetUnitAmount(UNIT_TYPEID type, int amount) {
    unit_amount[type] = amount;
}

std::map<sc2::UNIT_TYPEID, int>::iterator BPState::UnitsBegin() {
    return unit_amount.begin();
}

std::map<sc2::UNIT_TYPEID, int>::iterator BPState::UnitsEnd() {
    return unit_amount.end();
}

int BPState::GetMinerals() const {
    return minerals;
}

int BPState::GetVespene() const {
    return vespene;
}

int BPState::GetFoodCap() const {
    return food_cap;
}

int BPState::GetFoodUsed() const {
    return food_used;
}

void BPState::Print() {
    std::cout << ">>> BPState" << std::endl;
    std::cout << "Minerals: " << GetMinerals();
    std::cout << ", Vespene: " << GetVespene();
    std::cout << ", Food: " << GetFoodUsed();
    std::cout << "/" << GetFoodCap() << std::endl;
    for (auto it = UnitsBegin(); it != UnitsEnd(); ++it) {
        UNIT_TYPEID type = it->first;
        int amount = it->second;
        std::string name;
        if (type == UNIT_FAKEID::TERRAN_SCV_MINERALS) {
            name = "TERRAN_SCV_MINERALS";
        } else if (type == UNIT_FAKEID::TERRAN_SCV_VESPENE) {
            name = "TERRAN_SCV_VESPENE";
        } else {
            name = UnitTypeToName(type);
        }
        std::cout << name << ": " << amount << std::endl;
    }
    std::cout << "BPState <<<" << std::endl;
}

bool BPState::operator<(BPState const &other) const {
    if (minerals < other.minerals) return true;
    if (vespene < other.vespene) return true;
    if (food_cap - food_used < other.food_cap - other.food_used) return true;
    if (food_cap < other.food_cap) return true;
    for (auto pair : unit_amount) {
        if (other.unit_amount.find(pair.first) != other.unit_amount.cend()) {
            if (pair.second < other.unit_amount.at(pair.first)) return true;
        }
    }
    return false;
}

#undef DEBUG
#undef PRINT
#undef TEST
