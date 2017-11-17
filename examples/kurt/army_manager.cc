#include "army_manager.h"
#include <iostream>
#include <sc2api/sc2_map_info.h>

using namespace sc2;

ArmyManager::ArmyManager(Kurt* parent_kurt) {
    kurt = parent_kurt;
}

void ArmyManager::OnStep(const ObservationInterface* observation) {
    // DO ALL DE ARMY STUFF
    // Find a scout if we have none
    if (kurt->scouts.empty()){
        if(ArmyManager::TryGetScout()) {
            std::cout << "Number of scouts: " + kurt->scouts.size() << std::endl;
            // kommentar branch stuffu
        }
    }
    
    ArmyManager::PlanScoutPath();
    
    switch (current_combat_mode) {
        case DEFEND:
            ArmyManager::Defend();
            break;
        case ATTACK:
            ArmyManager::Attack();
            break;
        case HARASS:
        default:
            ArmyManager::Harass();
            break;
    }
}

void ArmyManager::PlanScoutPath() {
    // TODO: implement pathplanning for scout
    for (Point2D point : kurt->Observation()->GetGameInfo().enemy_start_locations) {
        
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

CombatMode ArmyManager::GetCombatMode() {
    return current_combat_mode;
}

void ArmyManager::SetCombatMode(CombatMode new_combat_mode) {
    current_combat_mode = new_combat_mode;
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
