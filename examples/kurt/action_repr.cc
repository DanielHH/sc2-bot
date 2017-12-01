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

std::map<ActionRepr::ACTION, ActionRepr> ActionRepr::values = {
    //Resource gathering
    { ACTION::SCV_GATHER_MINERALS, ActionRepr(0, 0, 0,{},{}, 0, 0, 0, 0,{}) },
    { ACTION::SCV_GATHER_VESPENE, ActionRepr(0, 0, 0,{},{}, 0, 0, 0, 0,{}) },
    { ACTION::MULE_GATHER_MINERALS, ActionRepr(0, 0, 0,{},{}, 0, 0, 0, 0,{}) },
    { ACTION::MULE_GATHER_VESPENE, ActionRepr(0, 0, 0,{},{}, 0, 0, 0, 0,{}) },

    // Building construction
    { ACTION::BUILD_COMMAND_CENTER, ActionRepr(0, 0, 0,{},{}, 0, 0, 0, 0,{}) },
    { ACTION::BUILD_PLANETARY_FORTRESS, ActionRepr(0, 0, 0,{},{}, 0, 0, 0, 0,{}) },
    { ACTION::BUILD_ORBITAL_COMMAND, ActionRepr(0, 0, 0,{},{}, 0, 0, 0, 0,{}) },
    { ACTION::BUILD_SUPPLY_DEPOT, ActionRepr(0, 0, 0,{},{}, 0, 0, 0, 0,{}) },
    { ACTION::BUILD_REFINERY, ActionRepr(0, 0, 0,{},{}, 0, 0, 0, 0,{}) },
    { ACTION::BUILD_BARRACKS, ActionRepr(0, 0, 0,{},{}, 0, 0, 0, 0,{}) },
    { ACTION::BUILD_ENGINEERING_BAY, ActionRepr(0, 0, 0,{},{}, 0, 0, 0, 0,{}) },
    { ACTION::BUILD_BUNKER, ActionRepr(0, 0, 0,{},{}, 0, 0, 0, 0,{}) },
    { ACTION::BUILD_SENSOR_TOWER, ActionRepr(0, 0, 0,{},{}, 0, 0, 0, 0,{}) },
    { ACTION::BUILD_MISSILE_TURRET, ActionRepr(0, 0, 0,{},{}, 0, 0, 0, 0,{}) },
    { ACTION::BUILD_FACTORY, ActionRepr(0, 0, 0,{},{}, 0, 0, 0, 0,{}) },
    { ACTION::BUILD_GHOST_ACADEMY, ActionRepr(0, 0, 0,{},{}, 0, 0, 0, 0,{}) },
    { ACTION::BUILD_STARPORT, ActionRepr(0, 0, 0,{},{}, 0, 0, 0, 0,{}) },
    { ACTION::BUILD_ARMORY, ActionRepr(0, 0, 0,{},{}, 0, 0, 0, 0,{}) },
    { ACTION::BUILD_FUSION_CORE, ActionRepr(0, 0, 0,{},{}, 0, 0, 0, 0,{}) },
    { ACTION::BUILD_TECH_LAB, ActionRepr(0, 0, 0,{},{}, 0, 0, 0, 0,{}) },
    { ACTION::BUILD_REACTOR, ActionRepr(0, 0, 0,{},{}, 0, 0, 0, 0,{}) },

    // Unit training
    { ACTION::TRAIN_SCV, ActionRepr(0, 0, 0,{},{}, 0, 0, 0, 0,{}) },
    { ACTION::TRAIN_MULE, ActionRepr(0, 0, 0,{},{}, 0, 0, 0, 0,{}) },
    { ACTION::TRAIN_MARINE, ActionRepr(0, 0, 0,{},{}, 0, 0, 0, 0,{}) },
    { ACTION::TRAIN_REAPER, ActionRepr(0, 0, 0,{},{}, 0, 0, 0, 0,{}) },
    { ACTION::TRAIN_MARAUDER, ActionRepr(0, 0, 0,{},{}, 0, 0, 0, 0,{}) },
    { ACTION::TRAIN_GHOST, ActionRepr(0, 0, 0,{},{}, 0, 0, 0, 0,{}) },
    { ACTION::TRAIN_HELLION, ActionRepr(0, 0, 0,{},{}, 0, 0, 0, 0,{}) },
    { ACTION::TRAIN_HELLBAT, ActionRepr(0, 0, 0,{},{}, 0, 0, 0, 0,{}) },
    { ACTION::TRAIN_WIDOW_MINE, ActionRepr(0, 0, 0,{},{}, 0, 0, 0, 0,{}) },
    { ACTION::TRAIN_SIEGE_TANK, ActionRepr(0, 0, 0,{},{}, 0, 0, 0, 0,{}) },
    { ACTION::TRAIN_CYCLONE, ActionRepr(0, 0, 0,{},{}, 0, 0, 0, 0,{}) },
    { ACTION::TRAIN_THOR, ActionRepr(0, 0, 0,{},{}, 0, 0, 0, 0,{}) },
    { ACTION::TRAIN_VIKING, ActionRepr(0, 0, 0,{},{}, 0, 0, 0, 0,{}) },
    { ACTION::TRAIN_MEDIVAC, ActionRepr(0, 0, 0,{},{}, 0, 0, 0, 0,{}) },
    { ACTION::TRAIN_LIBERATOR, ActionRepr(0, 0, 0,{},{}, 0, 0, 0, 0,{}) },
    { ACTION::TRAIN_RAVEN, ActionRepr(0, 0, 0,{},{}, 0, 0, 0, 0,{}) },
    { ACTION::TRAIN_BANSHEE, ActionRepr(0, 0, 0,{},{}, 0, 0, 0, 0,{}) },
    { ACTION::TRAIN_BATTLECRUISER, ActionRepr(0, 0, 0,{},{}, 0, 0, 0, 0,{}) },
    { ACTION::TRAIN_POINT_DEFENSE_DRONE, ActionRepr(0, 0, 0,{},{}, 0, 0, 0, 0,{}) },
    { ACTION::TRAIN_AUTO_TURRET, ActionRepr(0, 0, 0,{},{}, 0, 0, 0, 0,{}) }
    
    // Other
};


#undef DEBUG
#undef PRINT
#undef TEST
