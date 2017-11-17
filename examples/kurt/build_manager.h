#pragma once

#include <sc2api/sc2_api.h>

class BuildManager {
private:
    std::map<sc2::UNIT_TYPEID, std::vector<sc2::UNIT_TYPEID> > tech_tree_2;
    std::map<sc2::UNIT_TYPEID, sc2::UnitTypeData> unit_types;

public:
    BuildManager();
    sc2::UnitTypeData* EnumToData(sc2::UNIT_TYPEID id);
    std::vector<sc2::UnitTypeData*> GetRequirements(sc2::UnitTypeData* unit);
    void OnStep(const sc2::ObservationInterface* observation);
    void OnGameStart(const sc2::ObservationInterface* observation);
    void SetUpUnitDataMap(const sc2::ObservationInterface *observation);
    void SetUpTechTree(const sc2::ObservationInterface* observation);
};

