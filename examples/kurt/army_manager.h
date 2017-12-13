#pragma once

#include <sc2api/sc2_api.h>
#include "kurt.h"
#include <queue>
#include "cell_priority_queue.h"
#include "danger_point.cc"
#include <list>
#include "squad.h"


//using namespace sc2;


class ArmyManager {

public:
    ArmyManager(Kurt* kurt);

    // Gets called every game step
    void OnStep(const sc2::ObservationInterface* observation);
    void GroupNewUnit(const sc2::Unit* unit, const sc2::ObservationInterface* observation);
    


private:
    Kurt* kurt;
    CellPriorityQueue* scoutCellPriorityQueue;
    CellPriorityQueue* armyCellPriorityQueue;
    CellPriorityQueue* defendCellPriorityQueue;
    std::vector<Squad*> squads;
    std::vector<DangerPoint*> danger_points;
    float scout_safe_distance = 15;
    int danger_time = 1;
    
    void Harass();
    void Attack();
    void Defend();
    void PlanSmartScoutPath();
    bool TryGetScout();
    void PutUnitInSquad(const sc2::Unit* unit);
    void ScoutSmartPath();
};
