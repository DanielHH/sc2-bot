#pragma once

#include "game_plan.h"
#include <sc2api/sc2_api.h>

/* The default plan */
GamePlan* CreateDefaultGamePlan(Kurt* kurt);

/* Creates a small combat force and trires to rush the enemy */
GamePlan* RushPlan(Kurt* kurt);

/* Creates a small defence force and
starts working towards 2 battlecruisers */
GamePlan* CruiserPlan(Kurt* kurt);

/* Creates the biggest miningcompany the universe has ever seen */
GamePlan* VespeneGasTycoon(Kurt* kurt);

/* Automatically decides what units to build, and then decides
whether to attack, defend or harass with the new units */
GamePlan* DynamicGamePlan(Kurt* kurt);

// Teste defend plan
GamePlan* DefendGamePlan(Kurt* kurt);
