#pragma once
#include <vector>
#include "BPAction.h"

class BPState {
public:
	/* Creates a BPState representing the current gamestate */
	BPState();

	/* Destructors */
	~BPState();

	/* Calculates the state obtained after performing
	 * a given action when in a given state.
	 */
	BPState(BPState const, BPAction const);

	/* Returns a list of actions that can be performed
	 * from this state.
	 */
	std::vector<BPAction> AvailableActions() const;
};
