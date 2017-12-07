#pragma once

#include "sc2api/sc2_api.h"
#include "sc2lib/sc2_lib.h"

#include "action_enum.h"

#include <vector>

class Kurt;

class ExecAction {
public:

    static bool Exec(Kurt * const, ACTION);

    static bool ExecAbility(Kurt * const, sc2::ABILITY_ID); 

    static sc2::Unit const * FindNearestUnitOfType(
            sc2::UNIT_TYPEID type,
            sc2::Point2D const &location,
            sc2::ObservationInterface const *obs,
            sc2::Unit::Alliance alliance);

    static void Init(Kurt * const kurt);

private:

    static std::set<sc2::Point3D> commandcenter_locations;

    static std::set<sc2::Unit*> vespene_geysers;
};
