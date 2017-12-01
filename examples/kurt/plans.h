#pragma once

#include "game_plan.h"
#include <sc2api/sc2_api.h>

class Plans{
public:
    GamePlan* CreateDefaultGamePlan(Kurt* kurt);

    GamePlan* RushPlan(Kurt* kurt);

    GamePlan* CruiserPlan(Kurt* kurt);

    GamePlan* DynamicGamePlan(Kurt* kurt);
};
