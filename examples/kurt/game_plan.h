#pragma once

#include "BPState.h"
#include "army_manager.h"
#include <iostream>

/*
    A sequence of build orders(BPStates) and attack orders (CombatMode) that can be executed in order.
*/
class GamePlan {

private:
    /* Superclass for more specific nodes */
    struct Node {
        Node* next;

        Node() {
            next = nullptr;
            return;
        }

        /* Execute the part of the plan this node represents */
        virtual void Execute() {
            return;
        }
    };

    /* Switches the combat mode of the army manager when executed */
    struct CombatNode : Node {
        ArmyManager::CombatMode combat_order;

        CombatNode(ArmyManager::CombatMode _combat_order) {
            Node();
            combat_order = _combat_order;
        }

        void Execute() {
            // SetCombatMode(combat_order) to army_manager
            std::cout << "Set CombatMode: " << combat_order << std::endl;
        }
    };

    /* Requests a group of units from the build manager when executed*/
    struct BuildOrderNode : Node {
        BPState* build_order;

        BuildOrderNode(BPState* _build_order) {
            Node();
            build_order = _build_order;
        }

        void Execute() {
            // Give build_order to build_manager
            std::cout << "Build something!" << std::endl;
        }
    };

public:
    GamePlan();

    /* Adds a new CombatNode to the end of the plan */
    void AddCombatNode(ArmyManager::CombatMode combat_order);

    /* Adds a new BuildOrderNode to the end of the plan */
    void AddBuildOrderNode(BPState* build_order);

    /* Executes the head_node in the plan */
    void ExecuteNextNode();

private:
    Node* head_node; // The first node in the plan

    /* Adds a Node as the tail of the plan */
    void AddNode(Node* new_node);
};