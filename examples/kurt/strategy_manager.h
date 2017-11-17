#pragma once

#include <sc2api/sc2_api.h>


class StrategyManager{
public:
    sc2::Units spotted_enemy_units;

    StrategyManager();

    void OnStep(const sc2::ObservationInterface* observation);

private:
    //Save enemy units
    void SaveSpottedEnemyUnits(const sc2::ObservationInterface* observation);

    // Returns true if Enemy structure is observed. Med ObservationInterface ser man väl allt?
    bool FindEnemyStructure(const sc2::ObservationInterface* observation, const sc2::Unit*& enemy_unit);

};