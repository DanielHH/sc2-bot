#include <sc2api/sc2_api.h>
#include <iostream>
#include <map>

using namespace sc2;

class BuildManager{
public:
    BuildManager() {
        
    }

    void SetUpTechTree(const ObservationInterface* observation) {
    }

    vector<UnitTypeData*> GetRequirements(UnitTypeData unit) {
        if (unit.tech_requirement != UNIT_TYPEID::INVALID) {
        }
    }

    void OnStep (const ObservationInterface* observation) {
        // DO ALL DE ARMY STUFF
    }
    void OnGameStart(const ObservationInterface* observation) {
        // Set up build tree
        SetUpUnitDataMap(observation);
    }

    std::map<UnitTypeID, UnitTypeData> unit_types;
private:
    void SetUpUnitDataMap(const ObservationInterface *observation) {
        for (UnitTypeData id : observation->GetUnitTypeData()) {
            unit_types[id.unit_type_id] = id;
        }
    }
};

