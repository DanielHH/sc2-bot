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
    if (kurt->Observation()->GetGameLoop() % 24 == 0) {
        scoutCellPriorityQueue->Update();
        armyCellPriorityQueue->Update();
        defendCellPriorityQueue->Update();
    }
    
    if (kurt->scouts.size() < 2) {
        ArmyManager::TryGetScout();
    } else {
        //ArmyManager::ScoutSmartPath();
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

bool operator<(const sc2::Point2D & lhs, const sc2::Point2D & rhs) {
    if (lhs.x < rhs.x) {
        return true;
    } else if (lhs.x == rhs.x) {
        return lhs.y < rhs.y;
    } else {
        return false;
    }
}

void ArmyManager::PlanSmartScoutPath(){
    // THREAT MAP & A*
    for (const Unit* enemy: kurt->Observation()->GetUnits(Unit::Alliance::Enemy)) {
        if (kurt->IsArmyUnit(enemy)) {
            danger_points.push_back(new DangerPoint(enemy->pos, kurt->Observation()->GetGameLoop()));
        }
    }
    // pick a point and try to do A* to it
    std::vector<sc2::Point2D> points_to_visit;
    std::vector<sc2::Point2D> picked_points;
    for (int i = 0; i < scoutCellPriorityQueue->queue.size(); ++i) {
        points_to_visit.push_back(scoutCellPriorityQueue->queue.at(i)->GetCellLocationAs2DPoint(kurt->world_rep->chunk_size));
    }
    for (const Unit* scout : kurt->scouts) {
        // pick a good point
        Point2D picked_point;
        if (picked_points.empty()) {
            picked_point = points_to_visit.front();
        } else {
            for (sc2::Point2D possible_vis : points_to_visit) {
                for (sc2::Point2D picked : picked_points) {
                    if (Distance2D(possible_vis, picked) > 6*kurt->world_rep->chunk_size) {
                        picked_point = possible_vis;
                        goto picked;
                    }
                }
            }
        }
    picked:
        picked_points.push_back(picked_point);
        kurt->Actions()->UnitCommand(scout, ABILITY_ID::MOVE, picked_point);
        
        
        // do A* to reach picked point
        struct Node {
            Point2D pos;
            float score;
            Node(Point2D _pos, float _score) {
                pos = _pos;
                score = _score;
            }
        };
        
        struct NodeComp {
            bool operator()(Node lhs, Node rhs) {
                return lhs.score > rhs.score;
            }
        };
        
        
        Point2D start_pos = Point2D((int)scout->pos.x, (int)scout->pos.y); // convert float pos to grid pos
        Point2D goal = Point2D((int)picked_point.x, (int)picked_point.y); // convert float pos to grid pos
        std::vector<Node> open_list; // needs to be sorted after insert/update, best should be at back!
        std::vector<Point2D> closed_list;
        //bstd::less<Point2D> foo;
        //foo.operator() = [](sc2::Point2D &lhs, sc2::Point2D &rhs){return true;};
        auto cmp = [](const Point2D&a, const Point2D& b) { return a.x < b.x; };
        std::map<Point2D, float, decltype(cmp)> g_score(cmp);
        std::map<Point2D, float, decltype(cmp)> f_score(cmp);
        std::map<Point2D, Point2D, decltype(cmp)> camefrom(cmp);
        ImageData actual_world = kurt->Observation()->GetGameInfo().pathing_grid;
        for (int x = 0; x < actual_world.width; x++) {
            for (int y = 0; y < actual_world.height; y++) {
                
                //g_score.insert(std::pair<Point2D, float>(Point2D(x, y), INFINITY));
                //f_score.insert(std::pair<Point2D, float>(Point2D(x, y), INFINITY));
                g_score[Point2D(x, y)] = INFINITY;
                g_score[Point2D(x, y)] = INFINITY;
            }
        }
        //add start (current) node to open
        f_score[start_pos] = Distance2D(start_pos, goal);
        open_list.push_back(Node(start_pos, f_score[start_pos]));
        g_score[start_pos] = 0;
        while (!open_list.empty()) {
            Point2D current_pos = (open_list.back()).pos;
            if (current_pos == goal) {
                // done, walk to it
            }
            open_list.erase(std::find(open_list.begin(), open_list.end(), current_pos));
            closed_list.push_back(current_pos);
            // loop over neighbours
            for (int i = -1; i < 2; i++) {
                for (int j = -1; j > 2; j++) {
                    Point2D neighbour = Point2D(current_pos.x+i, current_pos.x+j);
                    // check if neighbour in closed
                    if (std::find_if(begin(closed_list), end(closed_list), [&](const Point2D &f) { return f.x == neighbour.x && f.y == neighbour.y; }) != end(closed_list)) {
                        continue;
                    }
                    float distance = kurt->Query()->PathingDistance(scout, neighbour);
                    if (distance > 0.1f) { // pathable
                        float tentative_g_score = g_score[current_pos] + distance;
                        if (tentative_g_score >= g_score[neighbour]) {
                            continue;		// This is not a better path.
                        }
                        // This path is the best until now. Record it!
                        g_score[neighbour] = tentative_g_score;
                        camefrom[neighbour] = current_pos;
                        f_score[neighbour] = g_score[neighbour] + Distance2D(neighbour, goal);
                        // check if neighbour not in open
                        if (!(std::find_if(begin(open_list), end(open_list), [&](const Point2D &f) { return f.x == neighbour.x && f.y == neighbour.y; }) != end(open_list))) {
                            
                            open_list.push_back(Node(neighbour, f_score[neighbour]));
                            std::sort(open_list.begin(), open_list.end(), NodeComp());
                        }
                    }
                }
            }
        }
    }
    
    //  try to avoid danger points
    for (const Unit* scout : kurt->scouts) {
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
        Point2D point_to_visit = scoutCellPriorityQueue->queue.at(i)->GetCellLocationAs2DPoint(kurt->world_rep->chunk_size);
        
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
            cell_to_attack = armyCellPriorityQueue->queue.at(i % 2);
            point_to_attack = (cell_to_attack)->GetCellLocationAs2DPoint(kurt->world_rep->chunk_size);
            squad->attackMove(point_to_attack);
        }
    }
}

void ArmyManager::Harass() {
    // TODO: implement Harass
}

// Returns true if a scout was found. Scout precedence: REAPER -> MARINE -> SCV
bool ArmyManager::TryGetScout() {
    bool scout_found = false;
    Squad* squad;
    const Unit* scout;
    if (!squads.empty()) {
        for (int i = squads.size()-1; i >= 0; i--) {
            Squad* tmp_squad = squads.at(i);
            if (tmp_squad->members.empty()) { // remove empty squad
                squads.erase(std::find(squads.begin(), squads.end(), tmp_squad));
                delete tmp_squad;
            } else {
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
    if (squads.empty()) {
        Squad* new_squad = new Squad(kurt);
        squads.push_back(new_squad);
    }
    if (unit->unit_type == UNIT_TYPEID::TERRAN_REAPER) {
        bool reaper_in_squad = false;
        for (int i = squads.size()-1; i >= 0; i--) {
            Squad* tmp_squad = squads.at(i);
            if (tmp_squad->members.size() < Squad::SQUAD_SIZE && !tmp_squad->members.empty() && tmp_squad->members.at(0)->unit_type == UNIT_TYPEID::TERRAN_REAPER) {
                tmp_squad->members.push_back(unit);
                reaper_in_squad = true;
            }
        }
        if (!reaper_in_squad) {
            if (squads.back()->members.empty()) {
                squads.back()->members.push_back(unit);
            } else {
                Squad* new_squad = new Squad(kurt);
                squads.push_back(new_squad);
                new_squad->members.push_back(unit);
            }
        }
    } else {
        bool unit_in_squad = false;
        for (int i = squads.size()-1; i >= 0; i--) {
            Squad* tmp_squad = squads.at(i);
            if (tmp_squad->members.empty()) {
                tmp_squad->filled_up = false;
                tmp_squad->members.push_back(unit);
                unit_in_squad = true;
            } else if (tmp_squad->members.size() < Squad::SQUAD_SIZE && tmp_squad->members.at(0)->unit_type != UNIT_TYPEID::TERRAN_REAPER && !tmp_squad->filled_up) {
                tmp_squad->members.push_back(unit);
                unit_in_squad = true;
                if (tmp_squad->members.size() == Squad::SQUAD_SIZE) {
                    tmp_squad->filled_up = true;
                }
            }
        }
        if (!unit_in_squad) {
            Squad* new_squad = new Squad(kurt);
            squads.push_back(new_squad);
            new_squad->members.push_back(unit);
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
