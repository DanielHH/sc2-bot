#include "game_plan.h";

GamePlan::GamePlan(Kurt* _kurt) {
    kurt = _kurt;
    head_node = nullptr;
}

void GamePlan::AddCombatNode(ArmyManager::CombatMode combat_order) {
    CombatNode* new_node = new CombatNode(kurt, combat_order);

    AddNode(new_node);
}

void GamePlan::AddBuildOrderNode(BPState* build_order) {
    BuildOrderNode* new_node = new BuildOrderNode(kurt, build_order);

    AddNode(new_node);
}

void GamePlan::ExecuteNextNode() {
    if (head_node != nullptr) {
        Node* next_node = head_node->next;

        head_node->Execute();
        delete head_node;
        head_node = next_node;
    }
    else {
        // TODO: ask for new plan
        std::cout << "Nothing to execute!" << std::endl;
    }
}

void GamePlan::AddNode(Node* new_node) {
    // If plan is empty, add the node as head_node
    if (head_node == nullptr) {
        std::cout << "Inserting first node" << std::endl;
        head_node = new_node;
    }
    else {
        Node* current_node = head_node;

        // Get the tail_node
        std::cout << "Iterating through nodes" << std::endl;
        while (current_node->next != nullptr) {
            current_node = current_node->next;
            std::cout << "..." << std::endl;
        }

        // Add the new node as tail
        current_node->next = new_node;
        std::cout << "Node added!" << std::endl;
    }
}