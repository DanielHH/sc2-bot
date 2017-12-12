#include "squad.h"

Squad::Squad(Kurt* parent_kurt) {
    kurt = parent_kurt;
}

void Squad::attackMove(sc2::Point2D point) {
    if (isGrouped()) {
        // attack
        for (const sc2::Unit* unit: members) {
            kurt->Actions()->UnitCommand(unit, sc2::ABILITY_ID::ATTACK, point);
        }
    } else if (!members.empty()) {
        groupUp();
    }
}

bool Squad::isGrouped() {
    bool squad_grouped = true;
    for (int i = members.size()-1; i >= 0; i--) {
        const sc2::Unit* unit = members.at(i);
        if (!unit->is_alive) {
            members.erase(std::find(members.begin(), members.end(), unit));
        }
    }
    if (!members.empty()) {
        const sc2::Unit* unit_leader = members.front();
        for (const sc2::Unit* unit_a : members) {
            if (unit_a != unit_leader) {
                // check distance
                if (sc2::Distance2D(unit_a->pos, unit_leader->pos) > SQUAD_DISTANCE) {
                    squad_grouped = false;
                    break;
                }
            }
        }
    }
    return squad_grouped;
}

void Squad::groupUp() {
    float tot_x = 0;
    float tot_y = 0;
    const sc2::Unit* ground_unit;
    for (const sc2::Unit* unit : members) {
        tot_x += unit->pos.x;
        tot_y += unit->pos.y;
        if (!unit->is_flying) {
            ground_unit = unit;
        }
    }
    sc2::Point2D meetup_point = sc2::Point2D(tot_x / members.size(), tot_y / members.size());
    if (ground_unit != nullptr) {
        // squad has a ground unit
        if (!kurt->CanPathToLocation(ground_unit, meetup_point)) {
            // point not pathable by the ground unit, meetup at the unit closest to it
            float shortest_distance = INFINITY;
            sc2::Point2D new_meetup_point;
            float unit_distance;
            for (const sc2::Unit * unit : members) {
                unit_distance = sc2::Distance2D(unit->pos, meetup_point);
                if (unit_distance < shortest_distance) {
                    shortest_distance = unit_distance;
                    new_meetup_point = unit->pos;
                }
            }
            meetup_point = new_meetup_point;
        }
    }
    
    for (const sc2::Unit* unit : members) {
        if (sc2::Distance2D(unit->pos, meetup_point) > SQUAD_DISTANCE) {
            kurt->Actions()->UnitCommand(unit, sc2::ABILITY_ID::MOVE, meetup_point);
        }
    }
}
