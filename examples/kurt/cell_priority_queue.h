#include<queue>
#include "world_representation.h"

class WorldCell;

class CellPriorityQueue {
    
public:
    std::vector<WorldCell*> queue;
    CellPriorityQueue(WorldRepresentation* world_representation);
    void Sort();
};
