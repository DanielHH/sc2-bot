#pragma once

#include <sc2api/sc2_api.h>
#include "kurt.h"
#include "world_cell.h"
#include <iostream>


class WorldRepresentation {

private:
    void PopulateNeutralUnits();
    Kurt* kurt;
public:
    WorldRepresentation(Kurt* kurt);
    std::vector<std::vector<WorldCell*>> world_representation;
    const static int chunk_size = 4;
    void PrintWorld();
    void UpdateWorldRep();
};
