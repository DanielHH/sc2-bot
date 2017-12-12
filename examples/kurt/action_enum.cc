#include "action_enum.h"

#include "action_repr.h"

std::string ActionToName(ACTION action) {
std::cout << "action_enum: 6" << std::endl;
    switch (action) {
std::cout << "action_enum: 7" << std::endl;
    case ACTION::INVALID:
std::cout << "action_enum: 8" << std::endl;
        return "INVALID";
std::cout << "action_enum: 9" << std::endl;
    case ACTION::SCV_GATHER_MINERALS:
std::cout << "action_enum: 10" << std::endl;
        return "SCV_GATHER_MINERALS";
std::cout << "action_enum: 11" << std::endl;
    case ACTION::SCV_GATHER_VESPENE:
std::cout << "action_enum: 12" << std::endl;
        return "SCV_GATHER_VESPENE";
std::cout << "action_enum: 13" << std::endl;
    case ACTION::MULE_GATHER_MINERALS:
std::cout << "action_enum: 14" << std::endl;
        return "MULE_GATHER_MINERALS";
std::cout << "action_enum: 15" << std::endl;
    case ACTION::MULE_GATHER_VESPENE:
std::cout << "action_enum: 16" << std::endl;
        return "MULE_GATHER_VESPENE";
std::cout << "action_enum: 17" << std::endl;
    case ACTION::BUILD_COMMAND_CENTER:
std::cout << "action_enum: 18" << std::endl;
        return "BUILD_COMMAND_CENTER";
std::cout << "action_enum: 19" << std::endl;
    case ACTION::BUILD_PLANETARY_FORTRESS:
std::cout << "action_enum: 20" << std::endl;
        return "BUILD_PLANETARY_FORTRESS";
std::cout << "action_enum: 21" << std::endl;
    case ACTION::BUILD_ORBITAL_COMMAND:
std::cout << "action_enum: 22" << std::endl;
        return "BUILD_ORBITAL_COMMAND";
std::cout << "action_enum: 23" << std::endl;
    case ACTION::BUILD_SUPPLY_DEPOT:
std::cout << "action_enum: 24" << std::endl;
        return "BUILD_SUPPLY_DEPOT";
std::cout << "action_enum: 25" << std::endl;
    case ACTION::BUILD_REFINERY:
std::cout << "action_enum: 26" << std::endl;
        return "BUILD_REFINERY";
std::cout << "action_enum: 27" << std::endl;
    case ACTION::BUILD_BARRACKS:
std::cout << "action_enum: 28" << std::endl;
        return "BUILD_BARRACKS";
std::cout << "action_enum: 29" << std::endl;
    case ACTION::BUILD_ENGINEERING_BAY:
std::cout << "action_enum: 30" << std::endl;
        return "BUILD_ENGINEERING_BAY";
std::cout << "action_enum: 31" << std::endl;
    case ACTION::BUILD_BUNKER:
std::cout << "action_enum: 32" << std::endl;
        return "BUILD_BUNKER";
std::cout << "action_enum: 33" << std::endl;
    case ACTION::BUILD_SENSOR_TOWER:
std::cout << "action_enum: 34" << std::endl;
        return "BUILD_SENSOR_TOWER";
std::cout << "action_enum: 35" << std::endl;
    case ACTION::BUILD_MISSILE_TURRET:
std::cout << "action_enum: 36" << std::endl;
        return "BUILD_MISSILE_TURRET";
std::cout << "action_enum: 37" << std::endl;
    case ACTION::BUILD_FACTORY:
std::cout << "action_enum: 38" << std::endl;
        return "BUILD_FACTORY";
std::cout << "action_enum: 39" << std::endl;
    case ACTION::BUILD_GHOST_ACADEMY:
std::cout << "action_enum: 40" << std::endl;
        return "BUILD_GHOST_ACADEMY";
std::cout << "action_enum: 41" << std::endl;
    case ACTION::BUILD_STARPORT:
std::cout << "action_enum: 42" << std::endl;
        return "BUILD_STARPORT";
std::cout << "action_enum: 43" << std::endl;
    case ACTION::BUILD_ARMORY:
std::cout << "action_enum: 44" << std::endl;
        return "BUILD_ARMORY";
std::cout << "action_enum: 45" << std::endl;
    case ACTION::BUILD_FUSION_CORE:
std::cout << "action_enum: 46" << std::endl;
        return "BUILD_FUSION_CORE";
std::cout << "action_enum: 47" << std::endl;
    case ACTION::BUILD_BARRACKS_TECH_LAB:
std::cout << "action_enum: 48" << std::endl;
        return "BUILD_BARRACKS_TECH_LAB";
std::cout << "action_enum: 49" << std::endl;
    case ACTION::BUILD_FACTORY_TECH_LAB:
std::cout << "action_enum: 50" << std::endl;
        return "BUILD_FACTORY_TECH_LAB";
std::cout << "action_enum: 51" << std::endl;
    case ACTION::BUILD_STARPORT_TECH_LAB:
std::cout << "action_enum: 52" << std::endl;
        return "BUILD_STARPORT_TECH_LAB";
std::cout << "action_enum: 53" << std::endl;
    case ACTION::BUILD_BARRACKS_REACTOR:
std::cout << "action_enum: 54" << std::endl;
        return "BUILD_BARRACKS_REACTOR";
std::cout << "action_enum: 55" << std::endl;
    case ACTION::BUILD_FACTORY_REACTOR:
std::cout << "action_enum: 56" << std::endl;
        return "BUILD_FACTORY_REACTOR";
std::cout << "action_enum: 57" << std::endl;
    case ACTION::BUILD_STARPORT_REACTOR:
std::cout << "action_enum: 58" << std::endl;
        return "BUILD_STARPORT_REACTOR";
std::cout << "action_enum: 59" << std::endl;
    case ACTION::TRAIN_SCV:
std::cout << "action_enum: 60" << std::endl;
        return "TRAIN_SCV";
std::cout << "action_enum: 61" << std::endl;
    case ACTION::TRAIN_MARINE:
std::cout << "action_enum: 62" << std::endl;
        return "TRAIN_MARINE";
std::cout << "action_enum: 63" << std::endl;
    case ACTION::TRAIN_REAPER:
std::cout << "action_enum: 64" << std::endl;
        return "TRAIN_REAPER";
std::cout << "action_enum: 65" << std::endl;
    case ACTION::TRAIN_MARAUDER:
std::cout << "action_enum: 66" << std::endl;
        return "TRAIN_MARAUDER";
std::cout << "action_enum: 67" << std::endl;
    case ACTION::TRAIN_GHOST:
std::cout << "action_enum: 68" << std::endl;
        return "TRAIN_GHOST";
std::cout << "action_enum: 69" << std::endl;
    case ACTION::TRAIN_HELLION:
std::cout << "action_enum: 70" << std::endl;
        return "TRAIN_HELLION";
std::cout << "action_enum: 71" << std::endl;
    case ACTION::TRAIN_HELLBAT:
std::cout << "action_enum: 72" << std::endl;
        return "TRAIN_HELLBAT";
std::cout << "action_enum: 73" << std::endl;
    case ACTION::TRAIN_WIDOW_MINE:
std::cout << "action_enum: 74" << std::endl;
        return "TRAIN_WIDOW_MINE";
std::cout << "action_enum: 75" << std::endl;
    case ACTION::TRAIN_SIEGE_TANK:
std::cout << "action_enum: 76" << std::endl;
        return "TRAIN_SIEGE_TANK";
std::cout << "action_enum: 77" << std::endl;
    case ACTION::TRAIN_CYCLONE:
std::cout << "action_enum: 78" << std::endl;
        return "TRAIN_CYCLONE";
std::cout << "action_enum: 79" << std::endl;
    case ACTION::TRAIN_THOR:
std::cout << "action_enum: 80" << std::endl;
        return "TRAIN_THOR";
std::cout << "action_enum: 81" << std::endl;
    case ACTION::TRAIN_VIKING:
std::cout << "action_enum: 82" << std::endl;
        return "TRAIN_VIKING";
std::cout << "action_enum: 83" << std::endl;
    case ACTION::TRAIN_MEDIVAC:
std::cout << "action_enum: 84" << std::endl;
        return "TRAIN_MEDIVAC";
std::cout << "action_enum: 85" << std::endl;
    case ACTION::TRAIN_LIBERATOR:
std::cout << "action_enum: 86" << std::endl;
        return "TRAIN_LIBERATOR";
std::cout << "action_enum: 87" << std::endl;
    case ACTION::TRAIN_RAVEN:
std::cout << "action_enum: 88" << std::endl;
        return "TRAIN_RAVEN";
std::cout << "action_enum: 89" << std::endl;
    case ACTION::TRAIN_BANSHEE:
std::cout << "action_enum: 90" << std::endl;
        return "TRAIN_BANSHEE";
std::cout << "action_enum: 91" << std::endl;
    case ACTION::TRAIN_BATTLECRUISER:
std::cout << "action_enum: 92" << std::endl;
        return "TRAIN_BATTLECRUISER";
std::cout << "action_enum: 93" << std::endl;
    case ACTION::TRAIN_POINT_DEFENSE_DRONE:
std::cout << "action_enum: 94" << std::endl;
        return "TRAIN_POINT_DEFENSE_DRONE";
std::cout << "action_enum: 95" << std::endl;
    case ACTION::TRAIN_AUTO_TURRET:
std::cout << "action_enum: 96" << std::endl;
        return "TRAIN_AUTO_TURRET";
std::cout << "action_enum: 97" << std::endl;
    case ACTION::FLY_COMMAND_CENTER:
std::cout << "action_enum: 98" << std::endl;
        return "FLY_COMMAND_CENTER";
std::cout << "action_enum: 99" << std::endl;
    case ACTION::FLY_ORBITAL_COMMAND:
std::cout << "action_enum: 100" << std::endl;
        return "FLY_ORBITAL_COMMAND";
std::cout << "action_enum: 101" << std::endl;
    case ACTION::FLY_BARRACKS:
std::cout << "action_enum: 102" << std::endl;
        return "FLY_BARRACKS";
std::cout << "action_enum: 103" << std::endl;
    case ACTION::FLY_FACTORY:
std::cout << "action_enum: 104" << std::endl;
        return "FLY_FACTORY";
std::cout << "action_enum: 105" << std::endl;
    case ACTION::FLY_STARPORT:
std::cout << "action_enum: 106" << std::endl;
        return "FLY_STARPORT";
std::cout << "action_enum: 107" << std::endl;
    default:
std::cout << "action_enum: 108" << std::endl;
        return "UNKNOWN";
std::cout << "action_enum: 109" << std::endl;
    }
}

std::ostream& operator<<(std::ostream& os, const ACTION & action) {
std::cout << "action_enum: 113" << std::endl;
    return os << ActionToName(action);
}

ActiveAction::ActiveAction(ACTION action_) : action(action_) {
std::cout << "action_enum: 117" << std::endl;
    time_left = ActionRepr::values.at(action_).time_required;
}

bool operator<(ActiveAction const & lhs, ActiveAction const & rhs) {
std::cout << "action_enum: 121" << std::endl;
    return lhs.time_left < rhs.time_left;
}
