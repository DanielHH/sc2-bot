#include "cell_priority_queue.h"
#include "world_cell.h"
#include "cell_comp_scout.cc"
#include "cell_comp_army.cc"

CellPriorityQueue::CellPriorityQueue(Kurt* parent_kurt, CellPriorityMode _mode) {
    kurt = parent_kurt;
    mode = _mode;
    for (int y = 0; y < kurt->world_rep->world_representation.size(); y++) {
        for (int x = 0; x < kurt->world_rep->world_representation.at(y).size(); x++) {
            WorldCell* temp_cell = kurt->world_rep->world_representation.at(y).at(x);
            queue.push_back(temp_cell);
        }
    }
}

void CellPriorityQueue::Update() {
    switch (mode) {
        case SCOUT:
            std::sort(queue.begin(), queue.end(), CellCompScout(kurt));
            break;
        case ARMY:
        case HARASS:
        default:
            std::sort(queue.begin(), queue.end(), CellCompArmy(kurt));
            break;
    }
    
}
