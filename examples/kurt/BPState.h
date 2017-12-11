#pragma once

#include <sc2api/sc2_api.h>
#include <map>
#include <set>
#include <vector>
#include <list>

#include "action_enum.h"
#include "BPPlan.h"

class Kurt;

class BPState {
public:
    /* Creates a empty BPState */
    BPState();

    /* Creates a BPState representing the current gamestate */
    BPState(Kurt * const kurt);

    /* Creates a copy of given BPState */
    BPState(BPState * const);

    /* Destructors */
    ~BPState();

    /* Updates this state until given action can be executed in this state.
     * If the action can be executed now, this update does nothing.
     */
    void UpdateUntilAvailable(ACTION);

    /* Updates this state by adding all actions in given plan
     * and run CompleteAllActions.
     */
    void SimulatePlan(BPPlan &);
    void SimulatePlan(BPPlan * const);

    /* Updates this state and add the action. */
    void AddAction(ACTION action, double time_left = -1);

    /* Updates this state with completing all active actions. */
    void CompleteAllActions();

    /* Checks if given action can be executed from this state,
     * without needing to wait for more resources or
     * other ongoing actions to finish.
     */
    bool CanExecuteNow(ACTION) const;

    /* Checks if given action can be executed from this state,
     * may require waiting for more resources or/and
     * other ongoing actions to finish but not creating other new actions.
     */
    bool CanExecuteNowOrSoon(ACTION) const;

    /* Returns a list of actions that can be performed from this state. */
    std::vector<ACTION> AvailableActions() const;

    /* Returns a list of actions that can be performed from this state,
     * from given list of actions to choose from.
     */
    std::vector<ACTION> AvailableActions(
            std::set<ACTION> & selectable_actions) const;

    /* Returns the amount of given units in this BPState */
    int GetUnitAmount(sc2::UNIT_TYPEID) const;

    /* Set this BPState to contain given amount of given unit */
    void SetUnitAmount(sc2::UNIT_TYPEID, int);

    /* Increases the number of a certain unit this BPState contains */
    void IncreaseUnitAmount(sc2::UNIT_TYPEID, int);

    /* Returns the amount of given units begining produced in this BPState */
    int GetUnitProdAmount(sc2::UNIT_TYPEID) const;

    /* Set this BPState to think that is produces given amount of given unit */
    void SetUnitProdAmount(sc2::UNIT_TYPEID, int);

    /* Increases the number of a certain unit being produced in this BPState */
    void IncreaseUnitProdAmount(sc2::UNIT_TYPEID, int);

    /* Returns the amount of given units begining produced in this BPState */
    int GetUnitAvailableAmount(sc2::UNIT_TYPEID) const;

    /* Set this BPState to think that is produces given amount of given unit */
    void SetUnitAvailableAmount(sc2::UNIT_TYPEID, int);

    /* Increases the number of a certain unit being produced in this BPState */
    void IncreaseUnitAvailableAmount(sc2::UNIT_TYPEID, int);

    /* Returns an iterator to the begining of all Units in this state */
    std::map<sc2::UNIT_TYPEID, int>::iterator UnitsBegin();

    /* Returns an iterator to the past last element of all Units in this state */
    std::map<sc2::UNIT_TYPEID, int>::iterator UnitsEnd();

    /* Returns an iterator to the begining of
     * all Units being produced int this state
     */
    std::map<sc2::UNIT_TYPEID, int>::iterator UnitsProdBegin();

    /* Returns an iterator to the past last element of
     * all Units begining produced in this state
     */
    std::map<sc2::UNIT_TYPEID, int>::iterator UnitsProdEnd();

    std::map<sc2::UNIT_TYPEID, int>::iterator UnitsAvailableBegin();

    std::map<sc2::UNIT_TYPEID, int>::iterator UnitsAvailableEnd();

    int GetMinerals() const;
    double GetMineralRate() const;
    int GetVespene() const;
    double GetVespeneRate() const;
    int GetFoodCap() const;
    int GetFoodUsed() const;

    /* Returns the time since the game started. */
    double GetTime() const;

    /* Prints this BPState to std::cout */
    void Print();

    bool operator<(BPState const &other) const;

    /* USED BY MCTS, empty by default.
     * List containing all available actions from this state.
     */
    std::vector<ACTION> available_actions;

    /* USED BY MCTS, empty by default.
     * List containing all states reached from this state
     * when preforming an action (with same index in available_actions).
     */
    std::vector<BPState* > children;

    /* USED BY MCTS */
    BPState * parent;

    /* USED BY MCTS
     * The reward is the score for how good this state is, range [0, 1].
     * 1 is the best, 0 is the worst.
     * reward is the best score for this state and all its children and
     * reward_stop is the score for stopping at this state (and add basic plan)
     */
    double reward, reward_stop;

    /* USED BY MCTS
     * The amount of times a search iteration has passed through this state.
     */
    int iter_amount;

    /* Checks if this state contains all units that the
     * given state contains
     */
    bool ContainsAllUnitsOf(BPState const &) const;

private:
    /* Updates this state given time (in seconds)
     * without updating active actions.
     */
    void SimpleUpdate(double);

    /* Update this state with the first action to be completed and
     * removes it from the list of actions, if there is any active actions.
     * Returns true if some action was finished and false if no actions exist.
     */
    bool CompleteFirstAction();

    /* The amount of units per type. */
    std::map<sc2::UNIT_TYPEID, int> unit_amount;
    /* The amount of units being produced per type.
     * This map is used to know if an action is soon available.
     */
    std::map<sc2::UNIT_TYPEID, int> unit_being_produced;
    /* The amount of each unit currently available and performing
     * no action.
     */
    std::map<sc2::UNIT_TYPEID, int> unit_nonbusy;

    /* All active/ongoing actions, sorted with "first to finish" first. */
    std::list<ActiveAction> actions;

    /* The time since the game started. */
    double time;
};
