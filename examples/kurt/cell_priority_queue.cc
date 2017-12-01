#include "cell_priority_queue.h"
#include "world_cell.h"
#include "cell_comp.cc"


CellPriorityQueue::CellPriorityQueue(Kurt* parent_kurt) {
    kurt = parent_kurt;
    for (int y = 0; y < kurt->world_rep->world_representation.size(); y++) {
        for (int x = 0; x < kurt->world_rep->world_representation.at(y).size(); x++) {
            WorldCell* temp_cell = kurt->world_rep->world_representation.at(y).at(x);
            queue.push_back(temp_cell);
        }
    }
    Update();
}

void CellPriorityQueue::Update() {
    std::sort(queue.begin(), queue.end(), CellComp(kurt));
}
