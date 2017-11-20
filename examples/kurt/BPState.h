#pragma once

#include <sc2api/sc2_api.h>
#include <map>
#include <vector>

class BPAction;

class BPState {
private:
    std::map<sc2::UNIT_TYPEID, int> unit_amount;

public:
    /* Creates a empty BPState */
    BPState();

    /* Creates a BPState representing the current gamestate */
    BPState(const sc2::ObservationInterface*);

    /* Destructors */
    ~BPState();

    /* Calculates the state obtained after performing
     * a given action when in a given state.
     */
    BPState(BPState const * const, BPAction const * const);

    /* Returns a list of actions that can be performed
     * from this state.
     */
    std::vector<BPAction *> AvailableActions() const;

    /* Returns the amount of given units in this BPState */
    int GetUnitAmount(sc2::UNIT_TYPEID);

    /* Set this BPState to contain given amount of given unit */
    void SetUnitAmount(sc2::UNIT_TYPEID, int);
};
