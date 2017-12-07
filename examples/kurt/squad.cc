#include "squad.h"

Squad::Squad(sc2::Units units, Kurt* parent_kurt) {
    members = units;
    kurt = parent_kurt;
}

void Squad::attackMove(sc2::Point2D point) {
    if (isGrouped()) {
        // attack
        for (const sc2::Unit* unit: members) {
            kurt->Actions()->UnitCommand(unit, sc2::ABILITY_ID::ATTACK, point);
        }
    } else {
        groupUp();
    }
}

bool Squad::isGrouped() {
    bool squad_grouped = true;
    if (!members.empty()) {
        const sc2::Unit* unit_leader = members.front();
        for (const sc2::Unit* unit_a : members) {
            if (unit_a != unit_leader) {
                // check distance
                if (sc2::Distance2D(unit_a->pos, unit_leader->pos) > 4) {
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
    for (const sc2::Unit* unit : members) {
        tot_x += unit->pos.x;
        tot_y += unit->pos.y;
    }
    sc2::Point2D meetup_point = sc2::Point2D(tot_x / members.size(), tot_y / members.size());
    for (const sc2::Unit* unit : members) {
        kurt->Actions()->UnitCommand(unit, sc2::ABILITY_ID::MOVE, meetup_point);
    }
}
