#pragma once

#include "sc2api/sc2_api.h"

class Squad {
Private:
    sc2::Units members;
    void groupUp() {
    }
Public:
    void attackMove(sc2::Point2D point) {
        bool squad_grouped = true;
        for (const sc2::Unit* unit_a : members) {
            for (const sc2::Unit*  unit_b: members) {
                if (unit_a != unit_b) {
                    // check distance
                }
            }
        }
    }
}
