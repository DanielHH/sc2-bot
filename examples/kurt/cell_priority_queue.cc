#include "cell_priority_queue.h"
#include "world_cell.h"


CellPriorityQueue::CellPriorityQueue(WorldRepresentation* world_representation) {
    for (int y = 0; y < world_representation->world_representation.size(); y++) {
        for (int x = 0; x < world_representation->world_representation.at(y).size(); x++) {
            WorldCell* temp_cell = world_representation->world_representation.at(y).at(x);
            this->queue.push_back(temp_cell);
        }
    }
    this->Sort();
}

void CellPriorityQueue::Sort() {
    std::sort(this->queue.begin(), this->queue.end());
    WorldCell* pivot = this->queue.at(this->queue.size()/2);
    for (int i = 0; i < this->queue.size(); i++) {
        
    }
}
