#pragma once

#include <sc2api/sc2_api.h>


class Kurt : public sc2::Agent {

public:


    virtual void OnGameStart();
    virtual void OnStep();
    virtual void OnUnitCreated(const sc2::Unit* unit);
    virtual void OnUnitIdle(const sc2::Unit* unit);
    bool TryBuildStructure(sc2::ABILITY_ID ability_type_for_structure,
        sc2::Point2D location,
        const sc2::Unit* unit);
    const sc2::Unit* getUnitOfType(sc2::UNIT_TYPEID unit_typeid);
    sc2::Point2D randomLocationNearUnit(const sc2::Unit* unit);
    bool TryBuildSupplyDepot();
    bool TryBuildRefinary();
    const sc2::Unit* FindNearestMineralPatch(const sc2::Point2D& start);
    const sc2::Unit* FindNearestVespeneGeyser();

};
