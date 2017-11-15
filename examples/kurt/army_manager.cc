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
        
        // If we have no scout take an SCV if no reapers exist
        if (shared_resources->scouts.empty()){
            if(TryGetScout()) {
                std::cout << "Number of scouts = " + shared_resources->scouts.size() + "Should be 1" << std_endl;
            }
        }
    }
    
    // Returns true if a scout was found. Scout precedence: REAPER -> MARINE -> SCV
    bool TryGetScout(){
        bool scout_found = false;
        const Unit* scout;
        
        for (const Unit* unit : shared_resources->army){
            if (unit->unit_type.ToType() == UNIT_TYPEID::TERRAN_MARINE) {
                // Marine found, but keep looking.
                scout = unit;
                scout_found = true;
            } else if (unit->unit_type.ToType() == UNIT_TYPEID::TERRAN_REAPER) {
                // We found a reaper, we are done!
                shared_resources->scouts.push_back(unit);
                shared_resources->army.remove(unit);
                return true;
            }
        }
        
        // if no reaper or marine is found look for a SCV.
        if (!scout_found) {
            for(const Unit* unit : shared_resources->workers) {
                // Find a SCV remove it from workers and put it in scouts.
                shared_resources->scouts.push_back(unit);
                shared_resources->workers.remove(unit);
                scout_found = true;
                return true;
            }
            // If we have no SCV:s
            return false;
        } else {
            // Add the marine in scouts and remove it from army
            shared_resources->scouts.push_back(scout);
            shared_resources->army.remove(scout);
            return true;
        }
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
