#pragma once

#include <sc2api/sc2_api.h>
#include <map>
#include <vector>
#include <list>

#include "action_enum.h"
#include "BPAction.h"

class Kurt;

class BPState {
public:
    /* Creates a empty BPState */
    BPState();

    /* Creates a BPState representing the current gamestate */
    BPState(Kurt * const kurt);

    /* Creates a copy of given BPState */
    BPState(BPState * const);

    /* Calculates the state obtained after performing
     * a given action when in a given state.
     */
    BPState(BPState const * const, BPAction const * const);

    /* Destructors */
    ~BPState();

    /* Updates this state given time (in seconds). */
    void Update(double);

    /* Updates this state until given action can be executed in this state.
     * If the action can be executed now, this update does nothing.
     */
    void UpdateUntilAvailable(BPAction);

    /* Checks if given action can be executed from this state,
     * without needing to wait for more resources or
     * other ongoing actions to finish.
     */
    bool CanExecuteNow(ACTION) const;

    /* Checks if given action can be executed from this state,
     * may require waiting for more resources or/and
     * other ongoing actions to finish but not creating other new actions.
     */
    bool CanExecuteNowOrSoon(ACTION) const;

    /* Returns a list of actions that can be performed
     * from this state.
     */
    std::vector<BPAction *> AvailableActions() const;

    /* Returns the amount of given units in this BPState */
    int GetUnitAmount(sc2::UNIT_TYPEID) const;

    /* Set this BPState to contain given amount of given unit */
    void SetUnitAmount(sc2::UNIT_TYPEID, int);

    /* Returns the amount of given units begining produced in this BPState */
    int GetUnitProdAmount(sc2::UNIT_TYPEID) const;

    /* Set this BPState to think that is produces given amount of given unit */
    void SetUnitProdAmount(sc2::UNIT_TYPEID, int);

    /* Returns an iterator to the begining of all Units in this state */
    std::map<sc2::UNIT_TYPEID, int>::iterator UnitsBegin();

    /* Returns an iterator to the past last element of all Units in this state */
    std::map<sc2::UNIT_TYPEID, int>::iterator UnitsEnd();

    /* Returns an iterator to the begining of
     * all Units being produced int this state
     */
    std::map<sc2::UNIT_TYPEID, int>::iterator UnitsProdBegin();

    /* Returns an iterator to the past last element of
     * all Units begining produced in this state
     */
    std::map<sc2::UNIT_TYPEID, int>::iterator UnitsProdEnd();

    int GetMinerals() const;
    double GetMineralRate() const;
    int GetVespene() const;
    double GetVespeneRate() const;
    int GetFoodCap() const;
    int GetFoodUsed() const;

    int GetTime() const;

    /* Prints this BPState to std::cout */
    void Print();

    bool operator<(BPState const &other) const;

private:
    /* Updates this state given time (in seconds)
     * without updating active actions.
     */
    void SimpleUpdate(double);

    std::map<sc2::UNIT_TYPEID, int> unit_amount;
    std::map<sc2::UNIT_TYPEID, int> unit_being_produced;

    std::list<ActiveAction> actions;

    double time;
};
