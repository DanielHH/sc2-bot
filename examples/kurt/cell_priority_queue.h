#include<queue>
#include "world_representation.h"
#include "kurt.h"
#include "cell_priority_enum.cc"

class WorldCell;

class CellPriorityQueue {
    
public:
    CellPriorityQueue(Kurt* kurt, CellPriorityMode mode);
    std::vector<WorldCell*> queue;
    void Update();
    
private:
    Kurt* kurt;
    CellPriorityMode mode;
};
