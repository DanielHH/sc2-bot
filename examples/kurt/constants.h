#pragma once

#include "sc2api/sc2_api.h"

const uint32_t STEPS_PER_SEC = 24;

const double MINERALS_PER_SEC_PER_SCV = 1.08;
const double VESPENE_PER_SEC_PER_SCV = 1.68;

/* All minerals and vespene in a cluster "should" be within this range
 * from the potential commandcenter in the middle.
 * This value is squared.
 */
const double BASE_RESOURCE_TEST_RANGE2 = 100;

/* Some bindings for UNIT_TYPEID to make the modelling of the game easier.
 * The class is used to make the usage of a fake id similar to a real one.
 */
class UNIT_FAKEID {
public:
    // Resource gatherers
    const static sc2::UNIT_TYPEID TERRAN_SCV_MINERALS = sc2::UNIT_TYPEID::ZERG_DRONE;
    const static sc2::UNIT_TYPEID TERRAN_SCV_VESPENE = sc2::UNIT_TYPEID::ZERG_EGG;
    const static sc2::UNIT_TYPEID TERRAN_MULE_MINERALS = sc2::UNIT_TYPEID::ZERG_EVOLUTIONCHAMBER;
    const static sc2::UNIT_TYPEID TERRAN_MULE_VESPENE = sc2::UNIT_TYPEID::ZERG_GREATERSPIRE;

    // Resources
    const static sc2::UNIT_TYPEID MINERALS = sc2::UNIT_TYPEID::ZERG_BROODLORD;
    const static sc2::UNIT_TYPEID VESPENE = sc2::UNIT_TYPEID::ZERG_CORRUPTOR;
    const static sc2::UNIT_TYPEID FOOD_USED = sc2::UNIT_TYPEID::ZERG_CREEPTUMOR;
    const static sc2::UNIT_TYPEID FOOD_CAP = sc2::UNIT_TYPEID::ZERG_CREEPTUMORQUEEN;

    // Multi-representation units/structures
    const static sc2::UNIT_TYPEID TERRAN_UPGRADABLE_COMMAND_CENTER = sc2::UNIT_TYPEID::ZERG_BANELINGCOCOON;
    const static sc2::UNIT_TYPEID TERRAN_UPGRADABLE_BARRACKS = sc2::UNIT_TYPEID::ZERG_BANELINGNEST;
    const static sc2::UNIT_TYPEID TERRAN_UPGRADABLE_FACTORY = sc2::UNIT_TYPEID::ZERG_BROODLORDCOCOON;
    const static sc2::UNIT_TYPEID TERRAN_UPGRADABLE_STARPORT = sc2::UNIT_TYPEID::ZERG_OVERLORDCOCOON;
};
