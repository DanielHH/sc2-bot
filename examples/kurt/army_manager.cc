#include "army_manager.h"
#include <iostream>
#include <sc2api/sc2_map_info.h>

//#define DEBUG // Comment out to disable debug prints in this file.
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
}

bool DistanceComp(Point2D a, Point2D b){
    Point2D bottom_left(0,0);
    float a_distance_from = Distance2D(bottom_left, a);
    float b_distance_from = Distance2D(bottom_left, b);
    return a_distance_from < b_distance_from;
}

std::priority_queue<sc2::Point2D, std::vector<sc2::Point2D>, std::function<bool(Point2D, Point2D)>> scout_path(DistanceComp);

bool ran = false;
void ArmyManager::OnStep(const ObservationInterface* observation) {
    // DO ALL DE ARMY STUFF
    // Find a scout if we have none
    if (kurt->scouts.empty()){
        if(ArmyManager::TryGetScout()) {
            //std::cout << "Number of scouts: " + kurt->scouts.size() << std::endl;
            // kommentar branch stuffu
        }
    }

    
    /*ImageData pathing_grid = kurt->Observation()->GetGameInfo().pathing_grid;
    std::cout << pathing_grid.height << std::endl;
    std::cout << pathing_grid.width << std::endl;*/
    if (!ran) {
        ArmyManager::PlanScoutPath();
        ran = true;
    }
    
    ArmyManager::ScoutPath();
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

void ArmyManager::PlanScoutPath() {
    // TODO: implement pathplanning for scout
    //scout_path = kurt->Observation()->GetGameInfo().enemy_start_locations;
    const Unit* scout = kurt->scouts.front();
    float longest_euk_dist = -INFINITY;
    float scout_x = scout->pos.x;
    float scout_y = scout->pos.y;
    
    for (const Unit* check_point: kurt->Observation()->GetUnits(Unit::Alliance::Neutral)) {
        float x_distance = abs(check_point->pos.x - scout_x);
        float y_distance = abs(check_point->pos.y - scout_y);
        float euk_distance_to_unit = sqrt(pow(x_distance, 2) + pow(y_distance, 2));
        if (euk_distance_to_unit > longest_euk_dist) {
            longest_euk_dist = euk_distance_to_unit;
        }
        scout_path.push(check_point->pos);
    }
    /*while (!scout_path.empty()) {
        Point2D point = scout_path.top();
        std::cout << point.x << std::endl;
        std::cout << point.y << std::endl;
        scout_path.pop();
    }*/
}

void ArmyManager::ScoutPath(){
    const Unit* scout = kurt->scouts.front();
    float scout_x = scout->pos.x;
    float scout_y = scout->pos.y;
    
    while (!scout_path.empty()){
        Point2D point_to_visit = scout_path.top();
        float x_distance = abs(point_to_visit.x - scout_x);
        float y_distance = abs(point_to_visit.y - scout_y);
        float euk_distance_to_unit = sqrt(pow(x_distance, 2) + pow(y_distance, 2));
        kurt->Actions()->UnitCommand(scout, ABILITY_ID::MOVE,point_to_visit);
        if(euk_distance_to_unit < 10) {
            scout_path.pop();
        }
        return;
    }
}

void ArmyManager::Defend() {
    // TODO: implement Defend
}

void ArmyManager::Attack() {
    // TODO: implement Attack
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
        for(const Unit* unit : kurt->workers) { // check scv order so we dont take a scv thats buidling
            // Find a SCV, remove it from workers and put it in scouts.
            kurt->scouts.push_back(unit);
            kurt->workers.remove(unit);
            scout_found = true;
            return true;
        }
        // If we have no SCV:s
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
        kurt->workers.push_back(unit);
    }
    else if (IsArmyUnit(unit, observation)) {
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

bool ArmyManager::IsArmyUnit(const Unit* unit, const ObservationInterface* observation) {
    if (IsStructure(unit, observation)) {
        return false;
    }
    switch (unit->unit_type.ToType()) {
        case UNIT_TYPEID::TERRAN_SCV: return false;
        case UNIT_TYPEID::TERRAN_MULE: return false;
        case UNIT_TYPEID::TERRAN_NUKE: return false;
        default: return true;
    }
}

bool ArmyManager::IsStructure(const Unit* unit, const ObservationInterface* observation) {
    bool is_structure = false;
    auto& attributes = observation->GetUnitTypeData().at(unit->unit_type).attributes;
    for (const auto& attribute : attributes) {
        if (attribute == Attribute::Structure) {
            is_structure = true;
        }
    }
    return is_structure;
}

#undef DEBUG
#undef TEST
#undef PRINT
