#pragma once

#include <sc2api/sc2_api.h>
#include <list>
#include <vector>
#include <ctime>
#include <ratio>
#include <chrono>

#include "BPState.h"

class WorldRepresentation;

class Kurt : public sc2::Agent {

public:
    std::list<const sc2::Unit*> scv_building;
    std::list<const sc2::Unit*> scv_idle;
    std::list<const sc2::Unit*> scv_minerals;
    std::list<const sc2::Unit*> scv_vespene;
    std::list<const sc2::Unit*> scouts;
    std::list<const sc2::Unit*> army_units;
    
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

    /*  Called when an enemy unit enters vision from out of fog of war */
    virtual void OnUnitEnterVision(const sc2::Unit* unit);

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

    /* Calculate a new plan */
    void CalculateNewPlan();

    /* Returns true if unit is an terran army unit */
    static bool IsArmyUnit(const sc2::Unit* unit);

    /* Returns true if unit is a structure */
    static bool IsStructure(const sc2::Unit* unit);

    /* Returns data about an ability */
    static sc2::AbilityData *GetAbility(sc2::ABILITY_ID);

    /* Returns data about a certain type of unit */
    static sc2::UnitTypeData *GetUnitType(sc2::UNIT_TYPEID);

    /* Starts the timer. */
    void TimeNew();

    /* Count the time since last call to either TimeNew or TimeNext and
     * adds the duration to given variable sent in as parameter.
     */
    void TimeNext(double & time_count);

    /* Creates a Point within a certain distance of a given point.
     */
    static sc2::Point2D RandomPoint(sc2::Point2D const &, float, float);

private:
    CombatMode current_combat_mode;

    static std::map<sc2::UNIT_TYPEID, sc2::UnitTypeData> unit_types;
    static std::map<sc2::ABILITY_ID, sc2::AbilityData> abilities;
    static std::map<sc2::UNIT_TYPEID, std::vector<sc2::ABILITY_ID>> unit_ability_map;
    static void SetUpDataMaps(const sc2::ObservationInterface *);

    /* Clocks used for measuring the execution time. */
    std::chrono::steady_clock::time_point clock_start, clock_end;
    /* Time spent executing respective manager. */
    double time_am = 0, time_bm = 0, time_sm = 0;
    /* The time (in game seconds) between every print and reset of exec time.*/
    const int time_interval = 10;
};
