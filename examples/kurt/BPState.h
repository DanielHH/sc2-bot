#pragma once

#include <sc2api/sc2_api.h>
#include <map>
#include <vector>

class BPAction;

class BPState {
public:
    /* Creates a empty BPState */
    BPState();

    /* Creates a BPState representing the current gamestate */
    BPState(const sc2::ObservationInterface*);

    /* Creates a copy of given BPState */
    BPState(BPState * const);

    /* Calculates the state obtained after performing
     * a given action when in a given state.
     */
    BPState(BPState const * const, BPAction const * const);

    /* Destructors */
    ~BPState();

    /* Returns a list of actions that can be performed
     * from this state.
     */
    std::vector<BPAction *> AvailableActions() const;

    /* Returns the amount of given units in this BPState */
    int GetUnitAmount(sc2::UNIT_TYPEID);

    /* Set this BPState to contain given amount of given unit */
    void SetUnitAmount(sc2::UNIT_TYPEID, int);

    /* Returns an iterator to the begining of all Units in this state */
    std::map<sc2::UNIT_TYPEID, int>::iterator UnitsBegin();

    /* Returns an iterator to the past last element of all Units in this state */
    std::map<sc2::UNIT_TYPEID, int>::iterator UnitsEnd();

    int GetMinerals() const;
    int GetVespen() const;
    int GetFood() const;

private:
    std::map<sc2::UNIT_TYPEID, int> unit_amount;
    int minerals;
    int vespene;
    int food;
};
