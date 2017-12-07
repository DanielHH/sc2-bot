#pragma once

#include <sc2api/sc2_api.h>

struct SquadUnit {
    const sc2::Unit* unit;
    bool belongsToSquad = false;
};
