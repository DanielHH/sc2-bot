#pragma once

#include "sc2api/sc2_api.h"
#include "sc2lib/sc2_lib.h"

#include "action_enum.h"

#include <vector>
#include <map>

class Kurt;

class ExecAction {
public:

    static double TimeSinceOrderSent(sc2::Unit const * unit, Kurt * kurt);

    static void OnStep(Kurt * kurt);

    static void OnUnitIdle(
            sc2::Unit const * unit,
            Kurt * kurt);

    static bool Exec(Kurt * const, ACTION);

    static bool ExecAbility(Kurt * const, sc2::ABILITY_ID);
    static bool ExecAbility(Kurt * const, sc2::ABILITY_ID, sc2::Unit const *);

    static sc2::Unit const * FindNearestUnitOfType(
            sc2::UNIT_TYPEID type,
            sc2::Point2D const &location,
            sc2::ObservationInterface const *obs,
            sc2::Unit::Alliance alliance);

    static void Init(Kurt * const kurt);

    static sc2::Unit const * FindNextVespeneGeyser(
            sc2::ObservationInterface const * obs);

    static sc2::Unit const * FindNextRefinery(
            sc2::ObservationInterface const * obs);

    static sc2::Unit const * FindNextMineralField(
            sc2::ObservationInterface const * obs);

    static bool FindNextCommandcenterLoc(
            sc2::ObservationInterface const * obs,
            sc2::QueryInterface * query,
            sc2::Point2D & location);

private:

    static std::map<sc2::Unit const *, int> sent_order_time;
    static std::map<sc2::Unit const *, int> built_refinery_time;

    static std::vector<sc2::Point3D> commandcenter_locations;

    static int scv_gather_vespene_delay;
    static int scv_gather_minerals_delay;
};
