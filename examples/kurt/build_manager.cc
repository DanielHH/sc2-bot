#include "build_manager.h"
#include <iostream>
#include <vector>
#include <map>

using namespace sc2;

class BuildManager{
public:
    BuildManager() {
        
    }

    UnitTypeData* EnumToData(UNIT_TYPEID id) {
        return &unit_types[id];
    }

    std::vector<UnitTypeData*> GetRequirements(UnitTypeData* unit) {
        std::vector<UnitTypeData*> requirements;
        if (unit->tech_requirement != UNIT_TYPEID::INVALID) {
            requirements.push_back(EnumToData(unit->tech_requirement));
        } else {
            // TODO Implement missing parts of tech-tree
        }
        return requirements;
    }

    void OnStep (const ObservationInterface* observation) {
        // DO ALL DE ARMY STUFF
    }

    void OnGameStart(const ObservationInterface* observation) {
        // Set up build tree
        SetUpUnitDataMap(observation);
        SetUpTechTree(observation);

        // Testing som functions...
        UNIT_TYPEID bar = UNIT_TYPEID::TERRAN_BARRACKS;
        UnitTypeData* barData = EnumToData(bar);
        std::cout << barData->name << " require: ";
        for (UnitTypeData* r : GetRequirements(barData)) {
            std::cout << r->name << " ";
        }
        std::cout << std::endl;
    }

private:
    std::map<UNIT_TYPEID, UnitTypeData> unit_types;

    void SetUpUnitDataMap(const ObservationInterface *observation) {
        for (UnitTypeData id : observation->GetUnitTypeData()) {
            unit_types[id.unit_type_id.ToType()] = id;
        }
    }

    void SetUpTechTree(const ObservationInterface* observation) {
        // TODO Implement
    }
};

