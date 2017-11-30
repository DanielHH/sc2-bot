#pragma once

#include "sc2api/sc2_api.h"

// Some bindings for UNIT_TYPEID to make the modelling of the game easier.
// The class is used to make the usage of a fake id similar to a real one.
class UNIT_FAKEID {
public:
    const static sc2::UNIT_TYPEID TERRAN_SCV_MINERALS = sc2::UNIT_TYPEID::ZERG_DRONE;
    const static sc2::UNIT_TYPEID TERRAN_SCV_VESPENE = sc2::UNIT_TYPEID::ZERG_EGG;
};
