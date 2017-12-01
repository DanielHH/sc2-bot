#include "action_repr.h"
#include <map>
#include <vector>

//#define DEBUG // Comment out to disable debug prints in this file.
#ifdef DEBUG
#include <iostream>
#define PRINT(s) std::cout << s << std::endl;
#define TEST(s) s
#else
#define PRINT(s)
#define TEST(s)
#endif // DEBUG

ActionRepr::ActionRepr(
    std::vector<struct UnitAmount> const & required_,
    std::vector<struct UnitAmount> const & consumed_,
    std::vector<struct UnitAmount> const & borrowed_,
    std::vector<struct UnitAmount> const & produced_) :
    required(required_),
    consumed(consumed_),
    borrowed(borrowed_),
    produced(produced_)
{

}

std::map<ACTION, ActionRepr> ActionRepr::values = {
    //Resource gathering
    { ACTION::SCV_GATHER_MINERALS, ActionRepr({},{},{},{}) },
    { ACTION::SCV_GATHER_VESPENE, ActionRepr({},{},{},{}) },
    { ACTION::MULE_GATHER_MINERALS, ActionRepr({},{},{},{}) },
    { ACTION::MULE_GATHER_VESPENE, ActionRepr({},{},{},{}) },

    // Building construction
    { ACTION::BUILD_COMMAND_CENTER, ActionRepr({},{},{},{}) },
    { ACTION::BUILD_PLANETARY_FORTRESS, ActionRepr({},{},{},{}) },
    { ACTION::BUILD_ORBITAL_COMMAND, ActionRepr({},{},{},{}) },
    { ACTION::BUILD_SUPPLY_DEPOT, ActionRepr({},{},{},{}) },
    { ACTION::BUILD_REFINERY, ActionRepr({},{},{},{}) },
    { ACTION::BUILD_BARRACKS, ActionRepr({},{},{},{}) },
    { ACTION::BUILD_ENGINEERING_BAY, ActionRepr({},{},{},{}) },
    { ACTION::BUILD_BUNKER, ActionRepr({},{},{},{}) },
    { ACTION::BUILD_SENSOR_TOWER, ActionRepr({},{},{},{}) },
    { ACTION::BUILD_MISSILE_TURRET, ActionRepr({},{},{},{}) },
    { ACTION::BUILD_FACTORY, ActionRepr({},{},{},{}) },
    { ACTION::BUILD_GHOST_ACADEMY, ActionRepr({},{},{},{}) },
    { ACTION::BUILD_STARPORT, ActionRepr({},{},{},{}) },
    { ACTION::BUILD_ARMORY, ActionRepr({},{},{},{}) },
    { ACTION::BUILD_FUSION_CORE, ActionRepr({},{},{},{}) },
    { ACTION::BUILD_TECH_LAB, ActionRepr({},{},{},{}) },
    { ACTION::BUILD_REACTOR, ActionRepr({},{},{},{}) },

    // Unit training
    { ACTION::TRAIN_SCV, ActionRepr({},{},{},{}) },
    { ACTION::TRAIN_MULE, ActionRepr({},{},{},{}) },
    { ACTION::TRAIN_MARINE, ActionRepr({},{},{},{}) },
    { ACTION::TRAIN_REAPER, ActionRepr({},{},{},{}) },
    { ACTION::TRAIN_MARAUDER, ActionRepr({},{},{},{}) },
    { ACTION::TRAIN_GHOST, ActionRepr({},{},{},{}) },
    { ACTION::TRAIN_HELLION, ActionRepr({},{},{},{}) },
    { ACTION::TRAIN_HELLBAT, ActionRepr({},{},{},{}) },
    { ACTION::TRAIN_WIDOW_MINE, ActionRepr({},{},{},{}) },
    { ACTION::TRAIN_SIEGE_TANK, ActionRepr({},{},{},{}) },
    { ACTION::TRAIN_CYCLONE, ActionRepr({},{},{},{}) },
    { ACTION::TRAIN_THOR, ActionRepr({},{},{},{}) },
    { ACTION::TRAIN_VIKING, ActionRepr({},{},{},{}) },
    { ACTION::TRAIN_MEDIVAC, ActionRepr({},{},{},{}) },
    { ACTION::TRAIN_LIBERATOR, ActionRepr({},{},{},{}) },
    { ACTION::TRAIN_RAVEN, ActionRepr({},{},{},{}) },
    { ACTION::TRAIN_BANSHEE, ActionRepr({},{},{},{}) },
    { ACTION::TRAIN_BATTLECRUISER, ActionRepr({},{},{},{}) },
    { ACTION::TRAIN_POINT_DEFENSE_DRONE, ActionRepr({},{},{},{}) },
    { ACTION::TRAIN_AUTO_TURRET, ActionRepr({},{},{},{}) }

    // Other
};


#undef DEBUG
#undef PRINT
#undef TEST
