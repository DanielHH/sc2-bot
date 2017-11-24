#pragma once

#include <sc2api/sc2_api.h>
#include <iostream>

class BPState;

class BPAction {
public:
    static const int USE_ABILITY = 0;
    static const int GATHER_MINERALS_SCV = 1;
    static const int GATHER_VESPENE_SCV = 2;

    int action_type; // What type of action, constants above.
    sc2::ABILITY_ID ability; // The ability to use.

    /* Constructors */
    BPAction();
    BPAction(sc2::ABILITY_ID);
    BPAction(bool, int const); // ambiguous if only int

    /* Destrutors */
    ~BPAction();

    /* Executes this action.
     * Returns true if this action was executed (start) in game, false otherwise.
     */
    bool Execute(sc2::ActionInterface *, sc2::QueryInterface *, sc2::ObservationInterface const *);

    /* Checks if this action can be executed from the current gamestate */
    bool CanExecute() const;

    /* Checks if this action can be executed from the given BPState,
     * without needing to wait for more resources or
     * other ongoing actions to finish.
     */
    bool CanExecuteInStateNow(BPState const * const) const;

    /* Checks if this action can be executed from the given BPState,
     * may require waiting for more resources or/and
     * other ongoing actions to finish but not creating other new actions.
     */
    bool CanExecuteInStateNowOrSoon(BPState const * const) const;

    /* Creates a new BPAction that can be used to create given unit. */
    static BPAction CreatesUnit(sc2::UNIT_TYPEID unit_type);

    std::string ToString() const;

    static std::set<sc2::ABILITY_ID> acceptable_to_interrupt;
};

std::ostream& operator<<(std::ostream& os, const BPAction & obj);
