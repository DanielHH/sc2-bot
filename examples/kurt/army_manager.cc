#include "army_manager.h"
#include <iostream>
#include <sc2api/sc2_map_info.h>
#include "BPAction.h"
#include "world_cell.h"

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
Kurt* comp_kurt;
ArmyManager::ArmyManager(Kurt* parent_kurt) {
    kurt = parent_kurt;
    comp_kurt = parent_kurt;
    cellPriorityQueue = new CellPriorityQueue(kurt);
}

void ArmyManager::OnStep(const ObservationInterface* observation) {
    // DO ALL DE ARMY STUFF
    if (kurt->scouts.empty()) {
        ArmyManager::TryGetScout();
    } else {
        ArmyManager::ScoutSmartPath();
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
        if(euk_distance_to_unit < 5) {
            cellPriorityQueue->queue.at(0)->SetSeenOnGameStep((float) kurt->Observation()->GetGameLoop());
            cellPriorityQueue->Update();
        }
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
