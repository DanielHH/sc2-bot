#include "army_manager.h"
#include <iostream>

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

ArmyManager::ArmyManager(Kurt* parent_kurt) {
    kurt = parent_kurt;
}

void ArmyManager::OnStep(const ObservationInterface* observation) {
    // DO ALL DE ARMY STUFF
}

void ArmyManager::GroupNewUnit(const Unit* unit, const ObservationInterface* observation) {
    if (unit->unit_type.ToType() == UNIT_TYPEID::TERRAN_SCV) {
        kurt->workers.push_back(unit);
    }
    else if (IsArmyUnit(unit, observation)) {
        kurt->army.push_back(unit);
    }
}

CombatMode ArmyManager::GetCombatMode() {
    return current_combat_mode;
}

void ArmyManager::SetCombatMode(CombatMode new_combat_mode) {
    current_combat_mode = new_combat_mode;
}

bool ArmyManager::IsArmyUnit(const Unit* unit, const ObservationInterface* observation) {
    if (IsStructure(unit, observation)) {
        return false;
    }
    switch (unit->unit_type.ToType()) {
    case UNIT_TYPEID::TERRAN_SCV: return false;
    case UNIT_TYPEID::TERRAN_MULE: return false;
    case UNIT_TYPEID::TERRAN_NUKE: return false;
    default: return true;
    }
}

bool ArmyManager::IsStructure(const Unit* unit, const ObservationInterface* observation) {
    bool is_structure = false;
    auto& attributes = observation->GetUnitTypeData().at(unit->unit_type).attributes;
    for (const auto& attribute : attributes) {
        if (attribute == Attribute::Structure) {
            is_structure = true;
        }
    }
    return is_structure;
}

#undef DEBUG
#undef TEST
#undef PRINT
