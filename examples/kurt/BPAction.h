#pragma once

#include "BPState.h"

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
    bool CanExecuteInState(BPState const) const;
};
