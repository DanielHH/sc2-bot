#pragma once

#include <sc2api/sc2_api.h>
#include <list>
#include "BPState.h"

class WorldRepresentation;

class Kurt : public sc2::Agent {

public:
    std::list<const sc2::Unit*> workers;
    std::list<const sc2::Unit*> scv_minerals;
    std::list<const sc2::Unit*> scv_vespene;
    std::list<const sc2::Unit*> scouts;
    std::list<const sc2::Unit*> army;
    
    WorldRepresentation* world_rep;
    enum CombatMode { DEFEND, ATTACK, HARASS };

    /* Called once when the game starts */
    virtual void OnGameStart();

    /* Called every step in the game */
    virtual void OnStep();

    /* Called every time a new unit is created*/
    virtual void OnUnitCreated(const sc2::Unit* unit);

    /* Called when a unit is idle */
    virtual void OnUnitIdle(const sc2::Unit* unit);

    /* Called when a unit is destroyes */
    virtual void OnUnitDestroyed(const sc2::Unit* unit);

    /* Test if given unit exist in given list. */
    bool UnitInList(std::list<const sc2::Unit*>& list, const sc2::Unit* unit);

    /* Tests if given unit already has been stored in a shared list. */
    bool UnitAlreadyStored(const sc2::Unit* unit);

    /* Test if given unit is in the list scv_minerals. */
    bool UnitInScvMinerals(const sc2::Unit* unit);

    /* Test if given unit is in the list scv_vespene. */
    bool UnitInScvVespene(const sc2::Unit* unit);

    /* Executes the next part of the current plan */
    void ExecuteSubplan();

    /* Gives the build manager a new goal to work against */
    void SendBuildOrder(BPState* const build_order);

    /* Returns current combat mode*/
    CombatMode GetCombatMode();

    /* Sets combat mode */
    void SetCombatMode(CombatMode new_combat_mode);

    /* Calculates which combat mode is best to be in in the moment */
    void CalculateCombatMode();

    /* Calculates what is best to build in the moment */
    void CalculateBuildOrder();

    /* Returns true if unit is an terran army unit */
    bool IsArmyUnit(const sc2::Unit* unit);

    /* Returns true if unit is a structure */
    bool IsStructure(const sc2::Unit* unit);

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
    CombatMode current_combat_mode;

    static std::map<sc2::UNIT_TYPEID, sc2::UnitTypeData> unit_types;
    static std::map<sc2::ABILITY_ID, sc2::AbilityData> abilities;
    static std::map<sc2::UNIT_TYPEID, std::vector<sc2::ABILITY_ID>> unit_ability_map;
    static void SetUpDataMaps(const sc2::ObservationInterface *);
};
