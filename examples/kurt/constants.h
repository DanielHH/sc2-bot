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
    const static sc2::UNIT_TYPEID TERRAN_SCV_MINERALS = static_cast<sc2::UNIT_TYPEID>(520);
    const static sc2::UNIT_TYPEID TERRAN_SCV_VESPENE = static_cast<sc2::UNIT_TYPEID>(521);
    const static sc2::UNIT_TYPEID TERRAN_MULE_MINERALS = static_cast<sc2::UNIT_TYPEID>(522);
    const static sc2::UNIT_TYPEID TERRAN_MULE_VESPENE = static_cast<sc2::UNIT_TYPEID>(523);

    // Resources
    const static sc2::UNIT_TYPEID MINERALS = static_cast<sc2::UNIT_TYPEID>(524);
    const static sc2::UNIT_TYPEID VESPENE = static_cast<sc2::UNIT_TYPEID>(525);
    const static sc2::UNIT_TYPEID FOOD_USED = static_cast<sc2::UNIT_TYPEID>(525);
    const static sc2::UNIT_TYPEID FOOD_CAP = static_cast<sc2::UNIT_TYPEID>(526);

    // Multi-representation units/structures
    const static sc2::UNIT_TYPEID TERRAN_ANY_COMMANDCENTER = static_cast<sc2::UNIT_TYPEID>(527);
    const static sc2::UNIT_TYPEID TERRAN_ANY_BARRACKS = static_cast<sc2::UNIT_TYPEID>(528);
    const static sc2::UNIT_TYPEID TERRAN_ANY_FACTORY = static_cast<sc2::UNIT_TYPEID>(529);
    const static sc2::UNIT_TYPEID TERRAN_ANY_STARPORT = static_cast<sc2::UNIT_TYPEID>(530);

    // How many new scv that can be produced
    const static sc2::UNIT_TYPEID TERRAN_TOWNHALL_SCV_MINERALS = static_cast<sc2::UNIT_TYPEID>(531);
    const static sc2::UNIT_TYPEID TERRAN_TOWNHALL_SCV_VESPENE = static_cast<sc2::UNIT_TYPEID>(532);
};
