#pragma once

#include <sc2api/sc2_api.h>
#include "kurt.h"
#include "world_cell.h"
#include <iostream>


class WorldRepresentation {

private:
    int chunk_size = 8;
    std::vector<std::vector<WorldCell*>> world_representation;
    void PopulateNeutralUnits(Kurt* kurt);

public:
    WorldRepresentation(Kurt* kurt);
    void PrintWorld();
};
