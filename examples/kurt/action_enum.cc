#include "action_enum.h"

#include "action_repr.h"

std::string ActionToName(ACTION action) {
    switch (action) {
    case ACTION::SCV_GATHER_MINERALS:
        return "SCV_GATHER_MINERALS";
    case ACTION::SCV_GATHER_VESPENE:
        return "SCV_GATHER_VESPENE";
    case ACTION::MULE_GATHER_MINERALS:
        return "MULE_GATHER_MINERALS";
    case ACTION::MULE_GATHER_VESPENE:
        return "MULE_GATHER_VESPENE";
    case ACTION::BUILD_COMMAND_CENTER:
        return "BUILD_COMMAND_CENTER";
    case ACTION::BUILD_PLANETARY_FORTRESS:
        return "BUILD_PLANETARY_FORTRESS";
    case ACTION::BUILD_ORBITAL_COMMAND:
        return "BUILD_ORBITAL_COMMAND";
    case ACTION::BUILD_SUPPLY_DEPOT:
        return "BUILD_SUPPLY_DEPOT";
    case ACTION::BUILD_REFINERY:
        return "BUILD_REFINERY";
    case ACTION::BUILD_BARRACKS:
        return "BUILD_BARRACKS";
    case ACTION::BUILD_ENGINEERING_BAY:
        return "BUILD_ENGINEERING_BAY";
    case ACTION::BUILD_BUNKER:
        return "BUILD_BUNKER";
    case ACTION::BUILD_SENSOR_TOWER:
        return "BUILD_SENSOR_TOWER";
    case ACTION::BUILD_MISSILE_TURRET:
        return "BUILD_MISSILE_TURRET";
    case ACTION::BUILD_FACTORY:
        return "BUILD_FACTORY";
    case ACTION::BUILD_GHOST_ACADEMY:
        return "BUILD_GHOST_ACADEMY";
    case ACTION::BUILD_STARPORT:
        return "BUILD_STARPORT";
    case ACTION::BUILD_ARMORY:
        return "BUILD_ARMORY";
    case ACTION::BUILD_FUSION_CORE:
        return "BUILD_FUSION_CORE";
    case ACTION::BUILD_BARRACKS_TECH_LAB:
        return "BUILD_BARRACKS_TECH_LAB";
    case ACTION::BUILD_FACTORY_TECH_LAB:
        return "BUILD_FACTORY_TECH_LAB";
    case ACTION::BUILD_STARPORT_TECH_LAB:
        return "BUILD_STARPORT_TECH_LAB";
    case ACTION::BUILD_BARRACKS_REACTOR:
        return "BUILD_BARRACKS_REACTOR";
    case ACTION::BUILD_FACTORY_REACTOR:
        return "BUILD_FACTORY_REACTOR";
    case ACTION::BUILD_STARPORT_REACTOR:
        return "BUILD_STARPORT_REACTOR";
    case ACTION::TRAIN_SCV:
        return "TRAIN_SCV";
    case ACTION::TRAIN_MULE:
        return "TRAIN_MULE";
    case ACTION::TRAIN_MARINE:
        return "TRAIN_MARINE";
    case ACTION::TRAIN_REAPER:
        return "TRAIN_REAPER";
    case ACTION::TRAIN_MARAUDER:
        return "TRAIN_MARAUDER";
    case ACTION::TRAIN_GHOST:
        return "TRAIN_GHOST";
    case ACTION::TRAIN_HELLION:
        return "TRAIN_HELLION";
    case ACTION::TRAIN_HELLBAT:
        return "TRAIN_HELLBAT";
    case ACTION::TRAIN_WIDOW_MINE:
        return "TRAIN_WIDOW_MINE";
    case ACTION::TRAIN_SIEGE_TANK:
        return "TRAIN_SIEGE_TANK";
    case ACTION::TRAIN_CYCLONE:
        return "TRAIN_CYCLONE";
    case ACTION::TRAIN_THOR:
        return "TRAIN_THOR";
    case ACTION::TRAIN_VIKING:
        return "TRAIN_VIKING";
    case ACTION::TRAIN_MEDIVAC:
        return "TRAIN_MEDIVAC";
    case ACTION::TRAIN_LIBERATOR:
        return "TRAIN_LIBERATOR";
    case ACTION::TRAIN_RAVEN:
        return "TRAIN_RAVEN";
    case ACTION::TRAIN_BANSHEE:
        return "TRAIN_BANSHEE";
    case ACTION::TRAIN_BATTLECRUISER:
        return "TRAIN_BATTLECRUISER";
    case ACTION::TRAIN_POINT_DEFENSE_DRONE:
        return "TRAIN_POINT_DEFENSE_DRONE";
    case ACTION::TRAIN_AUTO_TURRET:
        return "TRAIN_AUTO_TURRET";
    default:
        return "UNKNOWN";
    }
}

std::ostream& operator<<(std::ostream& os, const ACTION & action) {
    return os << ActionToName(action);
}

ActiveAction::ActiveAction(ACTION action_) : action(action_) {
    time_left = ActionRepr::values.at(action_).time_required;
}

bool operator<(ActiveAction const & lhs, ActiveAction const & rhs) {
    return lhs.time_left < rhs.time_left;
}
