#include "army_manager.h"
#include <iostream>
#include <sc2api/sc2_map_info.h>
#include "world_cell.h"
#include <cmath>
#include <algorithm>
#include "cell_priority_enum.cc"

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
    scoutCellPriorityQueue = new CellPriorityQueue(kurt, CellPriorityMode::SCOUT);
    armyCellPriorityQueue = new CellPriorityQueue(kurt, CellPriorityMode::ATTACK);
    defendCellPriorityQueue = new CellPriorityQueue(kurt, CellPriorityMode::DEFEND);
}

void ArmyManager::OnStep(const ObservationInterface* observation) {
    if (kurt->Observation()->GetGameLoop() % 48 == 0) {
        scoutCellPriorityQueue->Update();
        armyCellPriorityQueue->Update();
        defendCellPriorityQueue->Update();
    }
    
    if (kurt->scouts.size() < 2) {
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
    //const Unit* scout = kurt->scouts.front();
    
    for (const Unit* scout : kurt->scouts){
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
}

void ArmyManager::ScoutSmartPath(){
    int cell_index = 0;
    std::vector<sc2::Point2D> scout_points;
    
    for (int i = 0; i < scoutCellPriorityQueue->queue.size(); ++i) {
        cell_index = 0;
        Point2D point_to_visit = (scoutCellPriorityQueue->queue.at(i))->GetCellLocationAs2DPoint(kurt->world_rep->chunk_size);
        
        if (i == 0) {
            scout_points.push_back(point_to_visit);
            continue;
        }
        
        if (scout_points.size() < kurt->scouts.size()) {
            for (Point2D scout_point: scout_points) {
                if (Distance2D(point_to_visit, scout_point) > 3*kurt->world_rep->chunk_size) {
                    scout_points.push_back(point_to_visit);
                } else {
                    break;
                }
            }
        } else {
            for (const sc2::Unit* scout : kurt->scouts) {
                kurt->Actions()->UnitCommand(scout, ABILITY_ID::MOVE,scout_points.at(cell_index));
                cell_index += 1;
            }
            return;
        }
    }
    
    /*for (int i = 0; i < scoutCellPriorityQueue->queue.size(); ++i ) {
        for (const sc2::Unit* scout : kurt->scouts) {
            Point2D point_to_visit = (scoutCellPriorityQueue->queue.at(cell_index))->GetCellLocationAs2DPoint(kurt->world_rep->chunk_size);
            for (Point2D scout_point: scout_points) {
                if (Distance2D(point_to_visit, scout_point) > 2*kurt->world_rep->chunk_size) {
                    scout_points.push_back(point_to_visit);
                }
            }
            if (scout_points.size() > 2) {
                kurt->Actions()->UnitCommand(scout, ABILITY_ID::MOVE,point_to_visit);
            }
            cell_index += 1;
        }
    }*/
    
}

void ArmyManager::Defend() {
    // TODO: implement Defend
    
    if (!defendCellPriorityQueue->queue.empty()) {
        WorldCell* cell_to_attack = defendCellPriorityQueue->queue.at(0);
        Point2D point_to_attack = (cell_to_attack)->GetCellLocationAs2DPoint(kurt->world_rep->chunk_size);
        
        for(const Unit* unit: kurt->army_units){
            kurt->Actions()->UnitCommand(unit, ABILITY_ID::MOVE, point_to_attack);
        }
    }
    
    
}

void ArmyManager::Attack() {
    if (!armyCellPriorityQueue->queue.empty()) {
        WorldCell* cell_to_attack = armyCellPriorityQueue->queue.at(0);
        Point2D point_to_attack = (cell_to_attack)->GetCellLocationAs2DPoint(kurt->world_rep->chunk_size);
        
        for(const Unit* unit: kurt->army_units){
            kurt->Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, point_to_attack);
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
    
    for (const Unit* unit : kurt->army_units){
        if (unit->unit_type.ToType() == UNIT_TYPEID::TERRAN_MARINE) {
            // Marine found, but keep looking.
            scout = unit;
            scout_found = true;
        } else if (unit->unit_type.ToType() == UNIT_TYPEID::TERRAN_REAPER) {
            // We found a reaper, we are done!
            scout = unit;
            scout_found = true;
            break;
        }
    }
    if (scout_found) {
        // Add the found scout to scouts and remove it from army
        kurt->scouts.push_back(scout);
        kurt->army_units.remove(scout);
        scout_found = true;
    } else {
        // no army scout found look for an SCV.
        if (kurt->scv_minerals.size() > 0) {
            scout = kurt->scv_minerals.front();
            kurt->scouts.push_back(scout);
            kurt->scv_minerals.remove(scout);
            scout_found = true;
        }
    }
    return scout_found;
}

void ArmyManager::PutUnitInGroup(const Unit* unit) {
    
}

void ArmyManager::GroupNewUnit(const Unit* unit, const ObservationInterface* observation) {
    if (unit->unit_type.ToType() == UNIT_TYPEID::TERRAN_SCV) {
        if (!kurt->UnitInScvMinerals(unit)) {
            kurt->scv_minerals.push_back(unit);
        }
    }
    else if (kurt->IsArmyUnit(unit)) {
        kurt->army_units.push_back(unit);
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
