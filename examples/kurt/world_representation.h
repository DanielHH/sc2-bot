#pragma once

#include <sc2api/sc2_api.h>
#include "kurt.h"
#include "world_cell.h"
#include <iostream>


class WorldRepresentation {

private:
    void PopulateNeutralUnits(Kurt* kurt);

public:
    std::vector<std::vector<WorldCell*>> world_representation;
    WorldRepresentation(Kurt* kurt);
    int chunk_size = 16;
    void PrintWorld();
};
