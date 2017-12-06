#include "army_manager.h"
#include <iostream>
#include <sc2api/sc2_map_info.h>
#include "world_cell.h"
#include <cmath>
#include <algorithm>

#define DEBUG // Comment out to disable debug prints in this file.
#ifdef DEBUG
#include <iostream>
#define PRINT(s) std::cout << s << std::endl;
#define TEST(s) s
#else
#define PRINT(s)
#define TEST(s)
#endif // DEBUG

using namespace sc2;
ArmyManager::ArmyManager(Kurt* parent_kurt) {
    kurt = parent_kurt;
    cellPriorityQueue = new CellPriorityQueue(kurt);
}

void ArmyManager::OnStep(const ObservationInterface* observation) {
    // DO ALL DE ARMY STUFF
    
    cellPriorityQueue->Update();
    if (kurt->scouts.empty()) {
        ArmyManager::TryGetScout();
    } else {
        ArmyManager::ScoutSmartPath();
        ArmyManager::PlanSmartScoutPath();
    }
    switch (kurt->GetCombatMode()) {
        case Kurt::DEFEND:
            ArmyManager::Defend();
            break;
        case Kurt::ATTACK:
            ArmyManager::Attack();
            break;
        case Kurt::HARASS:
        default:
            ArmyManager::Harass();
            break;
    }
}

void ArmyManager::PlanSmartScoutPath(){
    // THREAT MAP
    const Unit* scout = kurt->scouts.front();
    for (const Unit* enemy: kurt->Observation()->GetUnits(Unit::Alliance::Enemy)) {
        if (kurt->IsArmyUnit(enemy)) {
            danger_points.push_back(new DangerPoint(enemy->pos, kurt->Observation()->GetGameLoop()));
        }
    }
    sc2::Point2D target;
    float shortest_distance = INFINITY;
    for (int i = danger_points.size()-1; i >= 0; i--) {
        DangerPoint* point = danger_points.at(i);
        if (point->SeenGameStepsAgo(kurt->Observation()->GetGameLoop()) < danger_time * 24) {
            if(Distance2D(scout->pos, point->GetPoint()) < scout_safe_distance) {
                float scout_x = scout->pos.x;
                float scout_y = scout->pos.y;
                
                float enemy_x = point->GetPoint().x;
                float enemy_y = point->GetPoint().y;
                
                float delta_x =  scout_x - enemy_x;
                float delta_y = scout_y - enemy_y;
                float length_normal_enemy = sqrt(pow(delta_x, 2) + pow(delta_y, 2));
                float scalar_normal = scout_safe_distance/length_normal_enemy;
                if (length_normal_enemy < shortest_distance) {
                    target = Point2D(enemy_x + delta_x * scalar_normal, enemy_y + delta_y*scalar_normal);
                    shortest_distance = length_normal_enemy;
                }
            }
        } else {
            danger_points.erase(std::find(danger_points.begin(), danger_points.end(), point));
            delete point;
        }
    }
    if (shortest_distance < INFINITY) {
        kurt->Actions()->UnitCommand(scout, ABILITY_ID::MOVE,target);
    }
}

void ArmyManager::ScoutSmartPath(){
    const Unit* scout = kurt->scouts.front();
    float scout_x = scout->pos.x;
    float scout_y = scout->pos.y;
    
    for (int i = 0; i < cellPriorityQueue->queue.size(); i++) {
        Point2D point_to_visit = (cellPriorityQueue->queue.at(0))->GetCellLocationAs2DPoint(kurt->world_rep->chunk_size);
        float x_distance = abs(point_to_visit.x - scout_x);
        float y_distance = abs(point_to_visit.y - scout_y);
        float euk_distance_to_unit = sqrt(pow(x_distance, 2) + pow(y_distance, 2));
        kurt->Actions()->UnitCommand(scout, ABILITY_ID::MOVE,point_to_visit);
        /*if(euk_distance_to_unit < 5) {
         cellPriorityQueue->queue.at(0)->SetSeenOnGameStep((float) kurt->Observation()->GetGameLoop());
         cellPriorityQueue->Update();
         }*/
        return;
    }
}

void ArmyManager::Defend() {
    // TODO: implement Defend
}

void ArmyManager::Attack() {
    // TODO: implement Attack
    for(const Unit* unit: kurt->army){
        if (unit->orders.size() == 0) {
            sc2::Point2D target = kurt->Observation()->GetGameInfo().enemy_start_locations[0];
            kurt->Actions()->UnitCommand(unit, ABILITY_ID::ATTACK,target);
        }
    }
}

void ArmyManager::Harass() {
    // TODO: implement Harass
}

// Returns true if a scout was found. Scout precedence: REAPER -> MARINE -> SCV
bool ArmyManager::TryGetScout() {
    bool scout_found = false;
    const Unit* scout;
    
    for (const Unit* unit : kurt->army){
        if (unit->unit_type.ToType() == UNIT_TYPEID::TERRAN_MARINE) {
            // Marine found, but keep looking.
            scout = unit;
            scout_found = true;
        } else if (unit->unit_type.ToType() == UNIT_TYPEID::TERRAN_REAPER) {
            // We found a reaper, we are done!
            kurt->scouts.push_back(unit);
            kurt->army.remove(unit);
            return true;
        }
    }
    
    // if no reaper or marine is found look for a SCV.
    if (!scout_found) {
        for(const Unit* unit : kurt->scv_minerals) {
            kurt->scouts.push_back(unit);
            kurt->scv_minerals.remove(unit);
            scout_found = true;
            return true;
        }
        // no SCV:s exists that are allowed to be interrupted.
        return false;
    } else {
        // Add the marine in scouts and remove it from army
        kurt->scouts.push_back(scout);
        kurt->army.remove(scout);
        return true;
    }
}

void ArmyManager::PutUnitInGroup(const Unit* unit) {
    
}

void ArmyManager::GroupNewUnit(const Unit* unit, const ObservationInterface* observation) {
    if (unit->unit_type.ToType() == UNIT_TYPEID::TERRAN_SCV) {
        if (! kurt->UnitInScvMinerals(unit)) {
            kurt->scv_minerals.push_back(unit);
        }
    }
    else if (kurt->IsArmyUnit(unit)) {
        kurt->army.push_back(unit);
    }
}

bool ArmyManager::CanPathToLocation(const sc2::Unit* unit, sc2::Point2D& target_pos) {
    // Send a pathing query from the unit to that point. Can also query from point to point,
    // but using a unit tag wherever possible will be more accurate.
    // Note: This query must communicate with the game to get a result which affects performance.
    // Ideally batch up the queries (using PathingDistanceBatched) and do many at once.
    float distance = kurt->Query()->PathingDistance(unit, target_pos);
    return distance > 0.1f;
}

#undef DEBUG
#undef TEST
#undef PRINT
