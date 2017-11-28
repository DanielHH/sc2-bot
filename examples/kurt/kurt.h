#pragma once

#include <sc2api/sc2_api.h>
#include <list>
#include "BPState.h"


class Kurt : public sc2::Agent {

public:
    std::list<const sc2::Unit*> workers;
    std::list<const sc2::Unit*> scouts;
    std::list<const sc2::Unit*> army;

    /* Called once when the game starts */
    virtual void OnGameStart();

    /* Called every step in the game */
    virtual void OnStep();

    /* Called every time a new unit is created*/
    virtual void OnUnitCreated(const sc2::Unit* unit);

    /* Called when a unit is idle */
    virtual void OnUnitIdle(const sc2::Unit* unit);

    /* Gives the build manager a new goal to work against */
    void SendBuildOrder(const BPState* build_order);

    bool TryBuildStructure(sc2::ABILITY_ID ability_type_for_structure,
        sc2::Point2D location,
        const sc2::Unit* unit);
    const sc2::Unit* getUnitOfType(sc2::UNIT_TYPEID unit_typeid);
    sc2::Point2D randomLocationNearUnit(const sc2::Unit* unit);
    bool TryBuildSupplyDepot();
    bool TryBuildRefinary();
    const sc2::Unit* FindNearestMineralPatch(const sc2::Point2D& start);
    const sc2::Unit* FindNearestVespeneGeyser();

    /* Returns data about an ability */
    static sc2::AbilityData *GetAbility(sc2::ABILITY_ID);

    /* Returns data about a certain type of unit */
    static sc2::UnitTypeData *GetUnitType(sc2::UNIT_TYPEID);

private:
    static std::map<sc2::UNIT_TYPEID, sc2::UnitTypeData> unit_types;
    static std::map<sc2::ABILITY_ID, sc2::AbilityData> abilities;
    static void SetUpDataMaps(const sc2::ObservationInterface *);

};
