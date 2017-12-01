#include "BPState.h"

#include "constants.h"
#include "kurt.h"

#include <sc2api/sc2_api.h>

#include <iostream>
#include <fstream>
#include <iterator>
#include <map>
#include <vector>

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
    time = state->GetTime();
}

BPState::BPState(Kurt * const kurt) {
    const ObservationInterface* observation = kurt->Observation();
    std::vector<const Unit*> commandcenters;
    for (auto unit : observation->GetUnits(Unit::Alliance::Self)) {
        UNIT_TYPEID type = unit->unit_type.ToType();
        SetUnitAmount(type, GetUnitAmount(type) + 1);
        if (type == UNIT_TYPEID::TERRAN_COMMANDCENTER) {
            commandcenters.push_back(unit);
        }
    }
    for (auto neutral : observation->GetUnits(Unit::Alliance::Neutral)) {
        UNIT_TYPEID type = neutral->unit_type.ToType();
        for (auto center : commandcenters) {
            if (DistanceSquared3D(center->pos, neutral->pos) <
                    BASE_RESOURCE_TEST_RANGE2) {
                SetUnitAmount(type, GetUnitAmount(type) + 1);
                break;
            }
        }
    }
    int geyser_amount = GetUnitAmount(UNIT_TYPEID::NEUTRAL_VESPENEGEYSER);
    geyser_amount -= GetUnitAmount(UNIT_TYPEID::TERRAN_REFINERY);
    SetUnitAmount(UNIT_TYPEID::NEUTRAL_VESPENEGEYSER, geyser_amount);
    SetUnitAmount(UNIT_FAKEID::TERRAN_SCV_MINERALS, kurt->scv_minerals.size());
    SetUnitAmount(UNIT_FAKEID::TERRAN_SCV_VESPENE, kurt->scv_vespene.size());
    SetUnitAmount(UNIT_FAKEID::MINERALS, observation->GetMinerals());
    SetUnitAmount(UNIT_FAKEID::VESPENE, observation->GetVespene());
    SetUnitAmount(UNIT_FAKEID::FOOD_CAP, observation->GetFoodCap());
    SetUnitAmount(UNIT_FAKEID::FOOD_USED, observation->GetFoodUsed());
    time = observation->GetGameLoop() / (double) STEPS_PER_SEC;
}

BPState::BPState(BPState const * const initial, BPAction const * const step) {
    // TODO
}

BPState::~BPState() {
    // TODO
}

void BPState::Update(double delta_time) {
}

void BPState::UpdateUntilAvailable(BPAction action) {
}

void BPState::SimpleUpdate(double delta_time) {
    int minerals = GetMinerals() +
        delta_time * MINERALS_PER_SEC_PER_SCV *
        GetUnitAmount(UNIT_FAKEID::TERRAN_SCV_MINERALS);
    SetUnitAmount(UNIT_FAKEID::MINERALS, minerals);
    int vespene = GetVespene() +
        delta_time * VESPENE_PER_SEC_PER_SCV *
        GetUnitAmount(UNIT_FAKEID::TERRAN_SCV_VESPENE);
    SetUnitAmount(UNIT_FAKEID::VESPENE, vespene);
    time += delta_time;
}

bool BPState::CanExecuteNow(BPAction const) const {
}

bool BPState::CanExecuteNowOrSoon(BPAction const) const {
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

int BPState::GetUnitAmount(UNIT_TYPEID type) const {
    // Need to test if element exist to prevent allocating more values
    if (unit_amount.count(type) == 0) {
        return 0;
    } else {
        return unit_amount.at(type);
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

int BPState::GetTime() const {
    return time;
}

int BPState::GetMinerals() const {
    return GetUnitAmount(UNIT_FAKEID::MINERALS);
}

int BPState::GetVespene() const {
    return GetUnitAmount(UNIT_FAKEID::VESPENE);
}

int BPState::GetFoodCap() const {
    return GetUnitAmount(UNIT_FAKEID::FOOD_CAP);
}

int BPState::GetFoodUsed() const {
    return GetUnitAmount(UNIT_FAKEID::FOOD_USED);
}

void BPState::Print() {
    std::cout << ">>> BPState" << std::endl;
    std::cout << "Gametime: " << GetTime() << std::endl;
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
    if (GetTime() < other.GetTime()) return true;
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
