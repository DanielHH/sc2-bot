#pragma once

class BPState;

class BPAction {
public:
    /* Constructors */
    BPAction();

    /* Destrutors */
    ~BPAction();

    /* Executes this action */
    void Execute();

    /* Checks if this action can be executed from the current gamestate */
    bool CanExecute() const;

    /* Checks if this action can be executed from the given BPState */
    bool CanExecuteInState(BPState const * const) const;

    int action_type; // 0: Use ability. 1: Gather minerals. 2: Gather gas. Anything else: invalid
    sc2::AbilityData *ability; // The ability to use.
};
