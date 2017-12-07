#pragma once

#include "BPState.h"
#include "kurt.h"

/*
    A sequence of build orders(BPStates) and attack orders (CombatMode) that can be executed in order.
*/
class GamePlan {

private:
    /* Superclass for more specific nodes */
    class Node {
    protected:
        Kurt* kurt;

        Node(Kurt* _kurt) {
            kurt = _kurt;
            next = nullptr;
            return;
        }

    public:
        Node* next;

        /* If this node executes directly, return true to execute the next node aswell */
        virtual bool Nextecute() {
            return false;
        }

        /* Execute the part of the plan this node represents */
        virtual void Execute() {
            return;
        }
    };

    /* Switches the combat mode of the army manager when executed */
    class StatCombatNode : public Node {
        Kurt::CombatMode combat_order;

    public:

        StatCombatNode(Kurt* kurt, Kurt::CombatMode _combat_order) : Node(kurt) {
            combat_order = _combat_order;
        }

        bool Nextecute() {
            return true;
        }

        void Execute() {
            kurt->SetCombatMode(combat_order);
        }
    };

    /* Switches the combat mode of the army manager when executed */
    class DynCombatNode : public Node {
    public:
        DynCombatNode(Kurt* kurt) : Node(kurt) {}

        bool Nextecute() {
            return true;
        }

        void Execute() {
            kurt->CalculateCombatMode();
        }
    };

    /* Requests a group of units from the build manager when executed*/
    class StatBuildOrderNode : public Node {
        BPState* build_order;

    public:
        StatBuildOrderNode(Kurt* kurt, BPState* _build_order) : Node(kurt) {
            build_order = _build_order;
        }

        void Execute() {            
            kurt->SendBuildOrder(build_order);
        }
    };

    /* Requests a group of units from the build manager when executed*/
    class DynBuildOrderNode : public Node {
    public:
        DynBuildOrderNode(Kurt* kurt) : Node(kurt) {}

        void Execute() {
            kurt->CalculateBuildOrder();
        }
    };

public:
    Kurt* kurt;

    GamePlan(Kurt* _kurt);

    ~GamePlan();

    /* Adds a new static CombatNode to the end of the plan */
    void AddStatCombatNode(Kurt::CombatMode combat_order);

    /* Adds a new dynamic CombatNode to the end of the plan */
    void AddDynCombatNode();

    /* Adds a new static BuildOrderNode to the end of the plan */
    void AddStatBuildOrderNode(BPState* build_order);

    /* Adds a new dynamic BuildOrderNode to the end of the plan */
    void AddDynBuildOrderNode();

    /* Executes the head_node in the plan */
    void ExecuteNextNode();

    /* Clears the GamePlan by deleting all its nodes */
    void Clear();

private:
    Node* head_node; // The first node in the plan

    /* Adds a Node as the tail of the plan */
    void AddNode(Node* new_node);
};
