#pragma once
#include "BPAction.h"
#include <vector>

class BPPlan : std::vector<BPAction> {
public:
	/* Apends a basic plan for going from a given state to some goal state */
	void AddBasicPlan(BPState const, BPState const);

	/* Returns the time required to execute this plan */
	float TimeRequired() const;

	/* Executes as much of this plan as possible during one game tick */
	void ExecuteStep();
};