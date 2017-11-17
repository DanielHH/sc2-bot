#pragma once

class BPState {
public:
	/* Creates a BPState representing the current gamestate */
	BPState();

	/* Destructors */
	~BPState();

	/* Calculates the state obtained after performing
	 * a given action when in a given state.
	 */
	BPState(BPState, BPAction);

	/* Returns a list of actions that can be performed
	 * from this state.
	 */
	vector<BPAction> AvailableActions();
};
