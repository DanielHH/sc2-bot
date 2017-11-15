#include <sc2api/sc2_api.h>
#include <iostream>
#include <sharedResources.cc>

using namespace sc2;

class ArmyManager{
public:
    ArmyManager(SharedResources* shared_resources) {
        ArmyManager::shared_resources = shared_resources;
    }
    void OnStep (const ObservationInterface* observation) {
        // DO ALL DE ARMY STUFF
    }
    
    void putUnitInGroup(const Unit* unit){
        
    }
    void groupNewUnit(const Unit* unit, const ObservationInterface* observation) {
        if (unit->unit_type.ToType() == UNIT_TYPEID::TERRAN_SCV) {
            shared_resources->workers.push_back(unit);
        } else if (isArmyUnit(unit, observation)) {
            shared_resources->army.push_back(unit);
        }
    }

private:
    SharedResources* shared_resources;
    
    bool isArmyUnit(const Unit* unit, const ObservationInterface* observation) {
        if (isStructure(unit, observation)) {
            return false;
        }
        switch (unit->unit_type.ToType()) {
            case UNIT_TYPEID::TERRAN_SCV: return false;
            case UNIT_TYPEID::TERRAN_MULE: return false;
            case UNIT_TYPEID::TERRAN_NUKE: return false;
            default: return true;
        }
    }
    
    bool isStructure(const Unit* unit, const ObservationInterface* observation) {
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
