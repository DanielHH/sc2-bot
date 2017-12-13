#pragma once

#include <iostream>

enum class ACTION {
    INVALID,

    //Resource gathering
    SCV_GATHER_MINERALS,
    SCV_GATHER_VESPENE,
    MULE_GATHER_MINERALS,
    MULE_GATHER_VESPENE,

    // Building construction
    BUILD_COMMAND_CENTER,
    BUILD_PLANETARY_FORTRESS,
    BUILD_ORBITAL_COMMAND,
    BUILD_SUPPLY_DEPOT,
    BUILD_REFINERY,
    BUILD_BARRACKS,
    BUILD_ENGINEERING_BAY,
    BUILD_BUNKER,
    BUILD_SENSOR_TOWER,
    BUILD_MISSILE_TURRET,
    BUILD_FACTORY,
    BUILD_GHOST_ACADEMY,
    BUILD_STARPORT,
    BUILD_ARMORY,
    BUILD_FUSION_CORE,
    BUILD_BARRACKS_TECH_LAB,
    BUILD_FACTORY_TECH_LAB,
    BUILD_STARPORT_TECH_LAB,
    BUILD_BARRACKS_REACTOR,
    BUILD_FACTORY_REACTOR,
    BUILD_STARPORT_REACTOR,

    // Unit training
    TRAIN_SCV,
    TRAIN_MARINE,
    TRAIN_REAPER,
    TRAIN_MARAUDER,
    TRAIN_GHOST,
    TRAIN_HELLION,
    TRAIN_HELLBAT,
    TRAIN_WIDOW_MINE,
    TRAIN_SIEGE_TANK,
    TRAIN_CYCLONE,
    TRAIN_THOR,
    TRAIN_VIKING,
    TRAIN_MEDIVAC,
    TRAIN_LIBERATOR,
    TRAIN_RAVEN,
    TRAIN_BANSHEE,
    TRAIN_BATTLECRUISER,
    TRAIN_POINT_DEFENSE_DRONE,
    TRAIN_AUTO_TURRET,

    // The FLY commands is NOT meaant to be used in the search
    // but only in the simulation of a current state
    FLY_COMMAND_CENTER,
    FLY_ORBITAL_COMMAND,
    FLY_BARRACKS,
    FLY_FACTORY,
    FLY_STARPORT,
};

std::string ActionToName(ACTION action);

std::ostream& operator<<(std::ostream& os, const ACTION & action);

class ActiveAction {
public:
    ActiveAction(ACTION);
    ACTION action;
    double time_left;
};

bool operator<(ActiveAction const & lhs, ActiveAction const & rhs);
