#pragma once

#include <sc2api/sc2_api.h>
#include "kurt.h"
#include <queue>
#include "cell_priority_queue.h"


//using namespace sc2;


class ArmyManager {

public:
    ArmyManager(Kurt* kurt);

    // Gets called every game step
    void OnStep(const sc2::ObservationInterface* observation);

    void GroupNewUnit(const sc2::Unit* unit, const sc2::ObservationInterface* observation);

private:
    Kurt* kurt;
    CellPriorityQueue* cellPriorityQueue;
    
    bool IsArmyUnit(const sc2::Unit* unit, const class sc2::ObservationInterface* observation);
    bool IsStructure(const sc2::Unit* unit, const sc2::ObservationInterface* observation);
    
    void Harass();
    void Attack();
    void Defend();
    void PlanSmartScoutPath();
    bool TryGetScout();
    bool CanPathToLocation(const sc2::Unit* unit, sc2::Point2D& target_pos);
    void PutUnitInGroup(const sc2::Unit* unit);
    void ScoutSmartPath();
};
