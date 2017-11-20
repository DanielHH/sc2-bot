#pragma once

#include <sc2api/sc2_api.h>

class BPState;

class BuildManager {
public:
    BuildManager();
    static sc2::UnitTypeData* EnumToData(sc2::UNIT_TYPEID id);
    static std::vector<sc2::UnitTypeData*> GetRequirements(sc2::UnitTypeData* unit);
    void OnStep(const sc2::ObservationInterface* observation);
    void OnGameStart(const sc2::ObservationInterface* observation);
    /* Removes all potential other goals and set given goal as the goal */
    void SetGoal(BPState const * const);

private:
    static std::map<sc2::UNIT_TYPEID, std::vector<sc2::UNIT_TYPEID> > tech_tree_2;
    static std::map<sc2::UNIT_TYPEID, sc2::UnitTypeData> unit_types;
    static void SetUpUnitDataMap(const sc2::ObservationInterface *observation);
    static void SetUpTechTree(const sc2::ObservationInterface* observation);
    static bool setup_finished;
};
