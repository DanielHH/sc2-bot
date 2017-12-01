#pragma once

#include <map>
#include <vector>
#include "sc2api/sc2_api.h"

struct UnitReq {
    sc2::UNIT_TYPEID type;
    bool mustnt_be_busy;
    int amount;
};

class ActionRepr {
public:
    ActionRepr(int, int, int, std::vector<struct UnitReq> const &, std::vector<struct UnitReq> const &, int, int, int, int, std::vector<struct UnitReq> const &);

    int mineral_cost;
    int gas_cost;
    int food_cost;

    std::vector<struct UnitReq> non_busy_unit_costs;
    std::vector<struct UnitReq> maybe_busy_unit_reqs;

    int time_required;

    int mineral_reward;
    int gas_reward;
    int food_reward;

    std::vector<struct UnitReq> unit_rewards;

    enum class ACTION {
        //Resource gathering
        SCV_GATHER_MINERALS,
        SCV_GATHER_GAS,
        SCV_STOP_GATHER_MINERALS,
        MULE_GATHER_MINERALS,
        MULE_GATHER_GAS,
        MULE_STOP_GATHER_MINERALS,
        MULE_STOP_GATHER_GAS,

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
        BUILD_TECH_LAB,
        BUILD_REACTOR,

        // Unit training
        TRAIN_SCV,
        TRAIN_MULE,
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

        // Other
    };

    static std::map<ActionRepr::ACTION, ActionRepr> values;
};
