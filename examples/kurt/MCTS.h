#pragma once
#include "BPPlan.h"
#include "BPAction.h"
#include "BPState.h"

class MCTS {
public:
	/* Constructors */

	/* Creates an MCTS object with the given initial state */
	MCTS(BPState);

	/* Destructors */
	//~MCTS();

	void Search(int);
	BPPlan BestPlan();
};