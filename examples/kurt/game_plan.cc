#include "game_plan.h";

#define DEBUG // Comment out to disable debug prints in this file.
#ifdef DEBUG
#include <iostream>
#define PRINT(s) std::cout << s << std::endl;
#define TEST(s) s
#else
#define PRINT(s)
#define TEST(s)
#endif // DEBUG

GamePlan::GamePlan(Kurt* _kurt) {
    kurt = _kurt;
    head_node = nullptr;
}


GamePlan::~GamePlan() {
    this->Clear();
}

void GamePlan::AddCombatNode(Kurt::CombatMode combat_order) {
    CombatNode* new_node = new CombatNode(kurt);
    AddNode(new_node);
}

void GamePlan::AddBuildOrderNode(BPState* build_order) {
    BuildOrderNode* new_node = new BuildOrderNode(kurt, build_order);

    AddNode(new_node);
}

void GamePlan::ExecuteNextNode() {
    if (head_node != nullptr) {
        Node* next_node = head_node->next;

        bool nextecute = head_node->Nextecute();
        PRINT(nextecute)

        head_node->Execute();
        delete head_node;
        head_node = next_node;

        if (nextecute) {
            ExecuteNextNode();
        }
    }
    else {
        // TODO: ask for new plan
        std::cout << "Nothing to execute!" << std::endl;
    }
}

void GamePlan::Clear() {
    Node* current_node = head_node;
    Node* next_node;

    while (current_node != nullptr) {
        next_node = current_node->next;
        delete current_node;
        current_node = next_node;
    }

    head_node = nullptr;
}

void GamePlan::AddNode(Node* new_node) {
    // If plan is empty, add the node as head_node
    if (head_node == nullptr) {
        head_node = new_node;
    }
    else {
        Node* current_node = head_node;

        // Get the tail_node
        while (current_node->next != nullptr) {
            current_node = current_node->next;
        }

        // Add the new node as tail
        current_node->next = new_node;
    }
}