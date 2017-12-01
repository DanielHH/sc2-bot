#pragma once

#include "BPState.h"
#include "kurt.h"
#include "army_manager.h"
#include <iostream>

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

        /* Execute the part of the plan this node represents */
        virtual void Execute() {
            return;
        }
    };

    /* Switches the combat mode of the army manager when executed */
    class CombatNode : public Node {
        Kurt::CombatMode combat_order;

    public:
        CombatNode(Kurt* kurt) : Node(kurt) {

        }

        void Execute() {
            kurt->CalculateCombatMode();
        }
    };

    /* Requests a group of units from the build manager when executed*/
    class BuildOrderNode : public Node {
        BPState* build_order;

    public:
        BuildOrderNode(Kurt* kurt, BPState* _build_order) : Node(kurt) {
            build_order = _build_order;
        }

        void Execute() {            
            kurt->SendBuildOrder(build_order);
        }
    };

public:
    Kurt* kurt;

    GamePlan(Kurt* _kurt);

    ~GamePlan();

    /* Adds a new CombatNode to the end of the plan */
    void AddCombatNode(Kurt::CombatMode combat_order);

    /* Adds a new BuildOrderNode to the end of the plan */
    void AddBuildOrderNode(BPState* build_order);

    /* Executes the head_node in the plan */
    void ExecuteNextNode();

    /* Clears the GamePlan by deleting all its nodes */
    void Clear();

private:
    Node* head_node; // The first node in the plan

    /* Adds a Node as the tail of the plan */
    void AddNode(Node* new_node);
};