#pragma once

#include <iostream>

class BPState;

class BPAction {
public:
    static const int USE_ABILITY = 0;
    static const int GATHER_MINERALS = 1;
    static const int GATHER_VESPENE = 2;

    int action_type; // 0: Use ability. 1: Gather minerals. 2: Gather vespene. Anything else: invalid
    sc2::ABILITY_ID ability; // The ability to use.

    /* Constructors */
    BPAction();
    BPAction(sc2::ABILITY_ID);
    BPAction(bool, int const); // ambiguous if only int

    /* Destrutors */
    ~BPAction();

    /* Executes this action */
    void Execute();

    /* Checks if this action can be executed from the current gamestate */
    bool CanExecute() const;

    /* Checks if this action can be executed from the given BPState */
    bool CanExecuteInState(BPState const * const) const;

    /* Creates a new BPAction that can be used to create given unit. */
    static BPAction CreatesUnit(sc2::UNIT_TYPEID unit_type);

    std::string ToString() const;
};

std::ostream& operator<<(std::ostream& os, const BPAction & obj);
