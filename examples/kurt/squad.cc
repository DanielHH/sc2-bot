#include "squad.h"

void Squad::attackMove(sc2::Point2D point) {
    bool squad_grouped = true;
    for (const sc2::Unit* unit_a : members) {
        for (const sc2::Unit*  unit_b: members) {
            if (unit_a != unit_b) {
                // check distance
            }
        }
    }
}
