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

bool second_scout = false;
int number_of_scouts = 1;

using namespace sc2;
ArmyManager::ArmyManager(Kurt* parent_kurt) {
    kurt = parent_kurt;
    scoutCellPriorityQueue = new CellPriorityQueue(kurt, CellPriorityMode::SCOUT);
    armyCellPriorityQueue = new CellPriorityQueue(kurt, CellPriorityMode::ATTACK);
    defendCellPriorityQueue = new CellPriorityQueue(kurt, CellPriorityMode::DEFEND);
}

void ArmyManager::OnStep(const ObservationInterface* observation) {
    if (kurt->Observation()->GetGameLoop() % 24 == 0) {
        scoutCellPriorityQueue->Update();
        armyCellPriorityQueue->Update();
        defendCellPriorityQueue->Update();
    }
    
    if(kurt->Observation()->GetGameLoop() > 4000) {
        // at around three minutes we add another scout
        number_of_scouts = 2;
    }
    
    if (kurt->scouts.size() < number_of_scouts) {
        ArmyManager::TryGetScout();
    } else {
        ArmyManager::ScoutSmartPath();
        ArmyManager::PlanSmartScoutPath();
    }
    switch (kurt->GetCombatMode()) {

        case Kurt::ATTACK:
            ArmyManager::Attack();
            break;
        case Kurt::HARASS:
        case Kurt::DEFEND:
        default:
            ArmyManager::Defend();
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
}

void ArmyManager::Defend() {
    // TODO: implement Defend
    
    if (!defendCellPriorityQueue->queue.empty()) {
        WorldCell* cell_to_attack = defendCellPriorityQueue->queue.at(0);
        Point2D point_to_attack = (cell_to_attack)->GetCellLocationAs2DPoint(kurt->world_rep->chunk_size);
        for (Squad* squad : squads) {
            squad->attackMove(point_to_attack);
        }
    }
    
    
}

void ArmyManager::Attack() {
    if (!armyCellPriorityQueue->queue.empty()) {
        WorldCell* cell_to_attack;
        Point2D point_to_attack;
        for (int i = 0; i < squads.size(); i++) {
            Squad* squad = squads.at(i);
            cell_to_attack = armyCellPriorityQueue->queue.at(i);
            point_to_attack = (cell_to_attack)->GetCellLocationAs2DPoint(kurt->world_rep->chunk_size);
            squad->attackMove(point_to_attack);
        }
    }
}

void ArmyManager::Harass() {
    // TODO: implement Harass
    Defend();
}

// Returns true if a scout was found. Scout precedence: REAPER -> MARINE -> SCV
bool ArmyManager::TryGetScout() {
    bool scout_found = false;
    Squad* squad = nullptr;
    const Unit* scout;
    if (!squads.empty()) {
        for (int i = squads.size()-1; i >= 0; i--) {
            Squad* tmp_squad = squads.at(i);
            if (tmp_squad->members.empty()) { // remove empty squad
                squads.erase(std::find(squads.begin(), squads.end(), tmp_squad));
                delete tmp_squad;
            } else {
                goto REAPER_SCOUT;
                for (const Unit* unit : squads.at(i)->members){
                    if (unit->unit_type.ToType() == UNIT_TYPEID::TERRAN_MARINE) {
                        // Marine found, but keep looking.
                        scout = unit;
                        squad = squads.at(i);
                        scout_found = true;
                    } else if (unit->unit_type.ToType() == UNIT_TYPEID::TERRAN_REAPER) {
                        // We found a reaper, we are done!
                        scout = unit;
                        squad = squads.at(i);
                        scout_found = true;
                        goto REAPER_SCOUT;
                    }
                }
            }
        }
    }
    REAPER_SCOUT:
    if (scout_found) {
        // Add the found scout to scouts and remove it from army
        kurt->army_units.remove(scout);
        squad->members.erase(std::find(squad->members.begin(), squad->members.end(), scout));
        kurt->scouts.push_back(scout);
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

void ArmyManager::PutUnitInSquad(const Unit* unit) {
    if (unit->is_flying) {
        if (unit->unit_type == sc2::UNIT_TYPEID::TERRAN_MEDIVAC) {
            putRandomInSquad(unit);
        } else {
            putFlyingInSquad(unit);
        }
    } else {
        switch (unit->unit_type.ToType()) {
            case UNIT_TYPEID::TERRAN_REAPER:
                putLonelySpecificTypeInSquad(unit);
                break;
            default:
                putRandomInSquad(unit);
                break;
        }
    }
}

void ArmyManager::putLonelySpecificTypeInSquad(const Unit* unit) {
    if (squads.empty()) {
        Squad* new_squad = new Squad(kurt);
        squads.push_back(new_squad);
    }
    bool unit_in_squad = false;
    Squad* empty_squad = nullptr;
    for (int i = squads.size()-1; i >= 0; i--) {
        // look for a squad where the same type are in, and if it's ok put in the unit there
        Squad* tmp_squad = squads.at(i);
        if (!tmp_squad->members.empty()) {
            if (!tmp_squad->filled_up && tmp_squad->single_type && tmp_squad->members.at(0)->unit_type == unit->unit_type) {
                tmp_squad->members.push_back(unit);
                unit_in_squad = true;
                if (tmp_squad->members.size() == Squad::SQUAD_SIZE) {
                    tmp_squad->filled_up = true;
                }
                break;
            }
        
        } else {
            empty_squad = tmp_squad;
            empty_squad->filled_up = false;
            empty_squad->single_type = true;
        }
    }
    if (!unit_in_squad) {
        if (empty_squad != nullptr) {
            empty_squad->members.push_back(unit);
        } else {
            // new squad
            Squad* new_squad = new Squad(kurt);
            squads.push_back(new_squad);
            new_squad->members.push_back(unit);
            new_squad->filled_up = false;
            new_squad->single_type = true;
        }
    }
}
    
void ArmyManager::putFlyingInSquad(const Unit* unit) {
    if (squads.empty()) {
        Squad* new_squad = new Squad(kurt);
        squads.push_back(new_squad);
    }
    bool unit_in_squad = false;
    Squad* empty_squad = nullptr;
    for (int i = squads.size()-1; i >= 0; i--) {
        // look for a squad where the same type are in, and if it's ok put in the unit there
        Squad* tmp_squad = squads.at(i);
        if (!tmp_squad->members.empty()) {
            if (!tmp_squad->filled_up && tmp_squad->flying) {
                tmp_squad->members.push_back(unit);
                unit_in_squad = true;
                if (tmp_squad->members.size() == Squad::SQUAD_SIZE) {
                    tmp_squad->filled_up = true;
                }
                break;
            }
        } else {
            empty_squad = tmp_squad;
            empty_squad->filled_up = false;
            empty_squad->single_type = false;
            empty_squad->flying = true;
        }
    }
    if (!unit_in_squad) {
        if (empty_squad != nullptr) {
            empty_squad->members.push_back(unit);
        } else {
            // new squad
            Squad* new_squad = new Squad(kurt);
            squads.push_back(new_squad);
            new_squad->members.push_back(unit);
            new_squad->filled_up = false;
            new_squad->single_type = false;
            new_squad->flying = true;
        }
    }
}

void ArmyManager::putRandomInSquad(const Unit* unit) {
    if (squads.empty()) {
        Squad* new_squad = new Squad(kurt);
        squads.push_back(new_squad);
    }
    bool unit_in_squad = false;
    Squad* empty_squad = nullptr;
    for (int i = squads.size()-1; i >= 0; i--) {
        // look for a squad where there are not sametype and not flying
        Squad* tmp_squad = squads.at(i);
        if (!tmp_squad->members.empty()) {
            if (!tmp_squad->filled_up && !tmp_squad->single_type && !tmp_squad->flying) {
                tmp_squad->members.push_back(unit);
                unit_in_squad = true;
                if (tmp_squad->members.size() == Squad::SQUAD_SIZE) {
                    tmp_squad->filled_up = true;
                }
                break;
            }
        } else {
            empty_squad = tmp_squad;
            empty_squad->filled_up = false;
            empty_squad->single_type = false;
            empty_squad->flying = false;
        }
    }
    if (!unit_in_squad) {
        if (empty_squad != nullptr) {
            empty_squad->members.push_back(unit);
        } else {
            // new squad
            Squad* new_squad = new Squad(kurt);
            squads.push_back(new_squad);
            new_squad->members.push_back(unit);
            new_squad->filled_up = false;
            new_squad->single_type = false;
            new_squad->flying = false;
        }
    }
}

void ArmyManager::GroupNewUnit(const Unit* unit, const ObservationInterface* observation) {
    if (unit->unit_type.ToType() == UNIT_TYPEID::TERRAN_SCV) {
        if (!kurt->UnitInScvMinerals(unit)) {
            kurt->scv_minerals.push_back(unit);
        }
    }
    else if (kurt->IsArmyUnit(unit)) {
        kurt->army_units.push_back(unit);
        PutUnitInSquad(unit);
    }
}

#undef DEBUG
#undef TEST
#undef PRINT
