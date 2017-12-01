#include<queue>
#include "world_representation.h"
#include "kurt.h"

class WorldCell;

class CellPriorityQueue {
    
public:
    CellPriorityQueue(Kurt* kurt);
    std::vector<WorldCell*> queue;
    CellPriorityQueue(WorldRepresentation* world_representation);
};
