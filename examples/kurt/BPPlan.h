#pragma once

#include "action_enum.h"

#include <iostream>
#include <vector>

class BPState;
class Kurt;

class BPPlan : public std::vector<ACTION> {
public:
    /* Apends a basic plan for going from a given state to some goal state */
    void AddBasicPlan(BPState * const, BPState * const);

    /* Returns the time required to execute this plan from given state. */
    float TimeRequired(BPState * const);

    /* Executes as much of this plan as possible during one game tick */
    void ExecuteStep(Kurt * const);

    std::string ToString() const;
};

std::ostream& operator<<(std::ostream&, const BPPlan&);
