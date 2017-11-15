#include <sc2api/sc2_api.h>
#include <iostream>
#include <sharedResources.cc>

using namespace sc2;

class ArmyManager{

    // Public variables
public:
    enum CombatMode { DEFEND, ATTACK, HARASS };

    // Private variables
private:
    CombatMode current_combat_mode;

    // Public methods
public:
    ArmyManager(SharedResources* shared_resources) {
        ArmyManager::shared_resources = shared_resources;
    }
    void OnStep (const ObservationInterface* observation) {
        // DO ALL DE ARMY STUFF
        
    }
    void GroupNewUnit(const Unit* unit, const ObservationInterface* observation) {
        if (unit->unit_type.ToType() == UNIT_TYPEID::TERRAN_SCV) {
            shared_resources->workers.push_back(unit);
        } else if (IsArmyUnit(unit, observation)) {
            shared_resources->army.push_back(unit);
        }
    }

    CombatMode GetCombatMode() {
        return current_combat_mode;
    }

    void SetCombatMode(CombatMode new_combat_mode) {
        current_combat_mode = new_combat_mode;
    }


    // Private methods
private:
    SharedResources* shared_resources;
    
    bool IsArmyUnit(const Unit* unit, const ObservationInterface* observation) {
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
    
    bool IsStructure(const Unit* unit, const ObservationInterface* observation) {
        bool is_structure = false;
        auto& attributes = observation->GetUnitTypeData().at(unit->unit_type).attributes;
        for (const auto& attribute : attributes) {
            if (attribute == Attribute::Structure) {
                is_structure = true;
            }
        }
        return is_structure;
    }
};
