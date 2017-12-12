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
std::cout << "army_manager: 21" << std::endl;
    kurt = parent_kurt;
std::cout << "army_manager: 22" << std::endl;
    scoutCellPriorityQueue = new CellPriorityQueue(kurt, CellPriorityMode::SCOUT);
std::cout << "army_manager: 23" << std::endl;
    armyCellPriorityQueue = new CellPriorityQueue(kurt, CellPriorityMode::ARMY);
}

void ArmyManager::OnStep(const ObservationInterface* observation) {
std::cout << "army_manager: 27" << std::endl;
    scoutCellPriorityQueue->Update();
std::cout << "army_manager: 28" << std::endl;
    armyCellPriorityQueue->Update();
std::cout << "army_manager: 29" << std::endl;
    
std::cout << "army_manager: 30" << std::endl;
    if (kurt->scouts.empty()) {
std::cout << "army_manager: 31" << std::endl;
        ArmyManager::TryGetScout();
std::cout << "army_manager: 32" << std::endl;
    } else {
std::cout << "army_manager: 33" << std::endl;
        ArmyManager::ScoutSmartPath();
std::cout << "army_manager: 34" << std::endl;
        ArmyManager::PlanSmartScoutPath();
std::cout << "army_manager: 35" << std::endl;
    }
std::cout << "army_manager: 36" << std::endl;
    switch (kurt->GetCombatMode()) {
std::cout << "army_manager: 37" << std::endl;
        case Kurt::DEFEND:
std::cout << "army_manager: 38" << std::endl;
            ArmyManager::Defend();
std::cout << "army_manager: 39" << std::endl;
            break;
std::cout << "army_manager: 40" << std::endl;
        case Kurt::ATTACK:
std::cout << "army_manager: 41" << std::endl;
            ArmyManager::Attack();
std::cout << "army_manager: 42" << std::endl;
            break;
std::cout << "army_manager: 43" << std::endl;
        case Kurt::HARASS:
std::cout << "army_manager: 44" << std::endl;
        default:
std::cout << "army_manager: 45" << std::endl;
            ArmyManager::Harass();
std::cout << "army_manager: 46" << std::endl;
            break;
std::cout << "army_manager: 47" << std::endl;
    }
}

void ArmyManager::PlanSmartScoutPath(){
std::cout << "army_manager: 51" << std::endl;
    // THREAT MAP
std::cout << "army_manager: 52" << std::endl;
    const Unit* scout = kurt->scouts.front();
std::cout << "army_manager: 53" << std::endl;
    for (const Unit* enemy: kurt->Observation()->GetUnits(Unit::Alliance::Enemy)) {
std::cout << "army_manager: 54" << std::endl;
        if (kurt->IsArmyUnit(enemy)) {
std::cout << "army_manager: 55" << std::endl;
            danger_points.push_back(new DangerPoint(enemy->pos, kurt->Observation()->GetGameLoop()));
std::cout << "army_manager: 56" << std::endl;
        }
std::cout << "army_manager: 57" << std::endl;
    }
std::cout << "army_manager: 58" << std::endl;
    sc2::Point2D target;
std::cout << "army_manager: 59" << std::endl;
    float shortest_distance = INFINITY;
std::cout << "army_manager: 60" << std::endl;
    for (int i = danger_points.size()-1; i >= 0; i--) {
std::cout << "army_manager: 61" << std::endl;
        DangerPoint* point = danger_points.at(i);
std::cout << "army_manager: 62" << std::endl;
        if (point->SeenGameStepsAgo(kurt->Observation()->GetGameLoop()) < danger_time * 24) {
std::cout << "army_manager: 63" << std::endl;
            if(Distance2D(scout->pos, point->GetPoint()) < scout_safe_distance) {
std::cout << "army_manager: 64" << std::endl;
                float scout_x = scout->pos.x;
std::cout << "army_manager: 65" << std::endl;
                float scout_y = scout->pos.y;
std::cout << "army_manager: 66" << std::endl;
                
std::cout << "army_manager: 67" << std::endl;
                float enemy_x = point->GetPoint().x;
std::cout << "army_manager: 68" << std::endl;
                float enemy_y = point->GetPoint().y;
std::cout << "army_manager: 69" << std::endl;
                
std::cout << "army_manager: 70" << std::endl;
                float delta_x =  scout_x - enemy_x;
std::cout << "army_manager: 71" << std::endl;
                float delta_y = scout_y - enemy_y;
std::cout << "army_manager: 72" << std::endl;
                float length_normal_enemy = sqrt(pow(delta_x, 2) + pow(delta_y, 2));
std::cout << "army_manager: 73" << std::endl;
                float scalar_normal = scout_safe_distance/length_normal_enemy;
std::cout << "army_manager: 74" << std::endl;
                if (length_normal_enemy < shortest_distance) {
std::cout << "army_manager: 75" << std::endl;
                    target = Point2D(enemy_x + delta_x * scalar_normal, enemy_y + delta_y*scalar_normal);
std::cout << "army_manager: 76" << std::endl;
                    shortest_distance = length_normal_enemy;
std::cout << "army_manager: 77" << std::endl;
                }
std::cout << "army_manager: 78" << std::endl;
            }
std::cout << "army_manager: 79" << std::endl;
        } else {
std::cout << "army_manager: 80" << std::endl;
            danger_points.erase(std::find(danger_points.begin(), danger_points.end(), point));
std::cout << "army_manager: 81" << std::endl;
            delete point;
std::cout << "army_manager: 82" << std::endl;
        }
std::cout << "army_manager: 83" << std::endl;
    }
std::cout << "army_manager: 84" << std::endl;
    if (shortest_distance < INFINITY) {
std::cout << "army_manager: 85" << std::endl;
        kurt->Actions()->UnitCommand(scout, ABILITY_ID::MOVE,target);
std::cout << "army_manager: 86" << std::endl;
    }
}

void ArmyManager::ScoutSmartPath(){
std::cout << "army_manager: 90" << std::endl;
    const Unit* scout = kurt->scouts.front();
std::cout << "army_manager: 91" << std::endl;
    float scout_x = scout->pos.x;
std::cout << "army_manager: 92" << std::endl;
    float scout_y = scout->pos.y;
std::cout << "army_manager: 93" << std::endl;
    
std::cout << "army_manager: 94" << std::endl;
    for (int i = 0; i < scoutCellPriorityQueue->queue.size(); i++) {
std::cout << "army_manager: 95" << std::endl;
        Point2D point_to_visit = (scoutCellPriorityQueue->queue.at(0))->GetCellLocationAs2DPoint(kurt->world_rep->chunk_size);
std::cout << "army_manager: 96" << std::endl;
        float x_distance = abs(point_to_visit.x - scout_x);
std::cout << "army_manager: 97" << std::endl;
        float y_distance = abs(point_to_visit.y - scout_y);
std::cout << "army_manager: 98" << std::endl;
        float euk_distance_to_unit = sqrt(pow(x_distance, 2) + pow(y_distance, 2));
std::cout << "army_manager: 99" << std::endl;
        kurt->Actions()->UnitCommand(scout, ABILITY_ID::MOVE,point_to_visit);
std::cout << "army_manager: 100" << std::endl;
        return;
std::cout << "army_manager: 101" << std::endl;
    }
}

void ArmyManager::Defend() {
std::cout << "army_manager: 105" << std::endl;
    // TODO: implement Defend
}

void ArmyManager::Attack() {
std::cout << "army_manager: 109" << std::endl;
    if (!armyCellPriorityQueue->queue.empty()) {
std::cout << "army_manager: 110" << std::endl;
        WorldCell* cell_to_attack = armyCellPriorityQueue->queue.at(0);
std::cout << "army_manager: 111" << std::endl;
        Point2D point_to_attack = (cell_to_attack)->GetCellLocationAs2DPoint(kurt->world_rep->chunk_size);
std::cout << "army_manager: 112" << std::endl;
        if (kurt->Observation()->GetGameLoop() % 240 == 0) {
std::cout << "army_manager: 113" << std::endl;
            std::cout << "Army at game step: " << kurt->Observation()->GetGameLoop() << std::endl;
std::cout << "army_manager: 114" << std::endl;
        }
std::cout << "army_manager: 115" << std::endl;
        for(const Unit* unit: kurt->army_units){
std::cout << "army_manager: 116" << std::endl;
            kurt->Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, point_to_attack);
std::cout << "army_manager: 117" << std::endl;
            if (kurt->Observation()->GetGameLoop() % 240 == 0) {
std::cout << "army_manager: 118" << std::endl;
                std::cout << "Army unit: " << unit->unit_type << std::endl;
std::cout << "army_manager: 119" << std::endl;
            }
std::cout << "army_manager: 120" << std::endl;
        }
std::cout << "army_manager: 121" << std::endl;
        /*
std::cout << "army_manager: 122" << std::endl;
        if (kurt->Observation()->GetGameLoop() % 240 == 0) {
std::cout << "army_manager: 123" << std::endl;
            std::cout << "Army Attack at game step: " << kurt->Observation()->GetGameLoop() << std::endl;
std::cout << "army_manager: 124" << std::endl;
            std::cout << "Cell to attack: X: " << cell_to_attack->GetCellRealX() << ", Y: " << cell_to_attack->GetCellRealY() << std::endl;
std::cout << "army_manager: 125" << std::endl;
            for (const Unit* unit : cell_to_attack->GetBuildings()) {
std::cout << "army_manager: 126" << std::endl;
                std::cout << "building: " << unit->unit_type << std::endl;
std::cout << "army_manager: 127" << std::endl;
            }
std::cout << "army_manager: 128" << std::endl;
            for (const Unit* unit : cell_to_attack->GetTroops()) {
std::cout << "army_manager: 129" << std::endl;
                std::cout << "trooper: " << unit->unit_type << std::endl;
std::cout << "army_manager: 130" << std::endl;
            }
std::cout << "army_manager: 131" << std::endl;
        }
std::cout << "army_manager: 132" << std::endl;
        */
std::cout << "army_manager: 133" << std::endl;
    }
}

void ArmyManager::Harass() {
std::cout << "army_manager: 137" << std::endl;
    // TODO: implement Harass
}

// Returns true if a scout was found. Scout precedence: REAPER -> MARINE -> SCV
bool ArmyManager::TryGetScout() {
std::cout << "army_manager: 142" << std::endl;
    bool scout_found = false;
std::cout << "army_manager: 143" << std::endl;
    const Unit* scout;
std::cout << "army_manager: 144" << std::endl;
    
std::cout << "army_manager: 145" << std::endl;
    for (const Unit* unit : kurt->army_units){
std::cout << "army_manager: 146" << std::endl;
        if (unit->unit_type.ToType() == UNIT_TYPEID::TERRAN_MARINE) {
std::cout << "army_manager: 147" << std::endl;
            // Marine found, but keep looking.
std::cout << "army_manager: 148" << std::endl;
            scout = unit;
std::cout << "army_manager: 149" << std::endl;
            scout_found = true;
std::cout << "army_manager: 150" << std::endl;
        } else if (unit->unit_type.ToType() == UNIT_TYPEID::TERRAN_REAPER) {
std::cout << "army_manager: 151" << std::endl;
            // We found a reaper, we are done!
std::cout << "army_manager: 152" << std::endl;
            scout = unit;
std::cout << "army_manager: 153" << std::endl;
            scout_found = true;
std::cout << "army_manager: 154" << std::endl;
            break;
std::cout << "army_manager: 155" << std::endl;
        }
std::cout << "army_manager: 156" << std::endl;
    }
std::cout << "army_manager: 157" << std::endl;
    if (scout_found) {
std::cout << "army_manager: 158" << std::endl;
        // Add the found scout to scouts and remove it from army
std::cout << "army_manager: 159" << std::endl;
        kurt->scouts.push_back(scout);
std::cout << "army_manager: 160" << std::endl;
        kurt->army_units.remove(scout);
std::cout << "army_manager: 161" << std::endl;
        scout_found = true;
std::cout << "army_manager: 162" << std::endl;
    } else {
std::cout << "army_manager: 163" << std::endl;
        // no army scout found look for an SCV.
std::cout << "army_manager: 164" << std::endl;
        if (kurt->scv_minerals.size() > 0) {
std::cout << "army_manager: 165" << std::endl;
            scout = kurt->scv_minerals.front();
std::cout << "army_manager: 166" << std::endl;
            kurt->scouts.push_back(scout);
std::cout << "army_manager: 167" << std::endl;
            kurt->scv_minerals.remove(scout);
std::cout << "army_manager: 168" << std::endl;
            scout_found = true;
std::cout << "army_manager: 169" << std::endl;
        }
std::cout << "army_manager: 170" << std::endl;
    }
std::cout << "army_manager: 171" << std::endl;
    return scout_found;
}

void ArmyManager::PutUnitInGroup(const Unit* unit) {
std::cout << "army_manager: 175" << std::endl;
    
}

void ArmyManager::GroupNewUnit(const Unit* unit, const ObservationInterface* observation) {
std::cout << "army_manager: 179" << std::endl;
    if (unit->unit_type.ToType() == UNIT_TYPEID::TERRAN_SCV) {
std::cout << "army_manager: 180" << std::endl;
        if (!kurt->UnitInScvMinerals(unit)) {
std::cout << "army_manager: 181" << std::endl;
            kurt->scv_minerals.push_back(unit);
std::cout << "army_manager: 182" << std::endl;
        }
std::cout << "army_manager: 183" << std::endl;
    }
    else if (kurt->IsArmyUnit(unit)) {
std::cout << "army_manager: 185" << std::endl;
        kurt->army_units.push_back(unit);
std::cout << "army_manager: 186" << std::endl;
    }
}

bool ArmyManager::CanPathToLocation(const sc2::Unit* unit, sc2::Point2D& target_pos) {
std::cout << "army_manager: 190" << std::endl;
    // Send a pathing query from the unit to that point. Can also query from point to point,
std::cout << "army_manager: 191" << std::endl;
    // but using a unit tag wherever possible will be more accurate.
std::cout << "army_manager: 192" << std::endl;
    // Note: This query must communicate with the game to get a result which affects performance.
std::cout << "army_manager: 193" << std::endl;
    // Ideally batch up the queries (using PathingDistanceBatched) and do many at once.
std::cout << "army_manager: 194" << std::endl;
    float distance = kurt->Query()->PathingDistance(unit, target_pos);
std::cout << "army_manager: 195" << std::endl;
    return distance > 0.1f;
}

#undef DEBUG
#undef TEST
#undef PRINT
