#include "plans.h"

#define DEBUG // Comment out to disable debug prints in this file.
#ifdef DEBUG
#include <iostream>
#define PRINT(s) std::cout << s << std::endl;
#define TEST(s) s
#else
#define PRINT(s)
#define TEST(s)
#endif // DEBUG

using namespace sc2;
using namespace std;

GamePlan* CreateDefaultGamePlan(Kurt* kurt) {
    PRINT("DefaultGamePlan")
    GamePlan* plan = new GamePlan(kurt);

    // Build order of 3 marines
    BPState* first_build_order= new BPState();
    BPState* second_build_order = new BPState();
    BPState* third_build_order = new BPState();
    BPState* fourth_build_order = new BPState();
    BPState* fifth_build_order = new BPState();

    //first_build
    first_build_order->SetUnitAmount(UNIT_TYPEID::TERRAN_MARINE, 5);
    first_build_order->SetUnitAmount(UNIT_TYPEID::TERRAN_REAPER, 2);
    //second_build
    second_build_order->SetUnitAmount(UNIT_TYPEID::TERRAN_MARINE, 8);
    second_build_order->SetUnitAmount(UNIT_TYPEID::TERRAN_MEDIVAC, 3);
    //third_build
    third_build_order->SetUnitAmount(UNIT_TYPEID::TERRAN_SIEGETANK, 5);
    third_build_order->SetUnitAmount(UNIT_TYPEID::TERRAN_LIBERATOR, 8);
    //fourth_build
    fourth_build_order->SetUnitAmount(UNIT_TYPEID::TERRAN_MARINE, 10);
    fourth_build_order->SetUnitAmount(UNIT_TYPEID::TERRAN_BATTLECRUISER, 1);
    //fourth_build
    fifth_build_order->SetUnitAmount(UNIT_TYPEID::TERRAN_MARINE, 10);
    fifth_build_order->SetUnitAmount(UNIT_TYPEID::TERRAN_BATTLECRUISER, 1);


    plan->AddStatBuildOrderNode(first_build_order);
    plan->AddDynCombatNode();
    plan->AddStatBuildOrderNode(second_build_order);
    plan->AddDynCombatNode();
    plan->AddStatBuildOrderNode(third_build_order);
    plan->AddDynCombatNode();
    plan->AddStatBuildOrderNode(fourth_build_order);
    plan->AddDynCombatNode();
    plan->AddStatBuildOrderNode(fifth_build_order);

    return plan;
}

GamePlan* RushPlan(Kurt* kurt) {
    GamePlan* plan = new GamePlan(kurt);

    BPState* first_build_order = new BPState();
    BPState* second_build_order = new BPState();
    BPState* third_build_order = new BPState();

    // Create 6 marines to use in the rush
    first_build_order->SetUnitAmount(UNIT_TYPEID::TERRAN_MARINE, 6);
    
    // Continue produce marines and reapers as reinforcements to the rush force
    second_build_order->SetUnitAmount(UNIT_TYPEID::TERRAN_COMMANDCENTER, 1);
    third_build_order->SetUnitAmount(UNIT_TYPEID::TERRAN_MARINE, 12);
    third_build_order->SetUnitAmount(UNIT_TYPEID::TERRAN_REAPER, 12);

    plan->AddStatBuildOrderNode(first_build_order);
    plan->AddStatCombatNode(Kurt::ATTACK); // Send the marines to attack
    plan->AddStatBuildOrderNode(second_build_order);
    plan->AddStatCombatNode(Kurt::DEFEND); // Send the marines to defend
    plan->AddStatBuildOrderNode(third_build_order);

    return plan;
}

GamePlan* CruiserPlan(Kurt* kurt) {
    GamePlan* plan = new GamePlan(kurt);

    BPState* first_build_order = new BPState();
    BPState* second_build_order = new BPState();
    BPState* third_build_order = new BPState();

    // Create some more svcs and a barracks at start
    first_build_order->SetUnitAmount(UNIT_TYPEID::TERRAN_SCV, 16);
    first_build_order->SetUnitAmount(UNIT_TYPEID::TERRAN_BARRACKS, 1);

    // Create 10 marines as a base defence army
    second_build_order->SetUnitAmount(UNIT_TYPEID::TERRAN_MARINE, 10);

    // Start work agains 2 battlecruisers
    third_build_order->SetUnitAmount(UNIT_TYPEID::TERRAN_BATTLECRUISER, 2);

    plan->AddStatCombatNode(Kurt::DEFEND); // Defend base until both battlecruisers are done
    plan->AddStatBuildOrderNode(first_build_order);
    plan->AddStatBuildOrderNode(second_build_order);
    plan->AddStatBuildOrderNode(third_build_order);
    plan->AddStatCombatNode(Kurt::ATTACK); //ATTACK!

    return plan;
}

GamePlan* VespeneGasTycoon(Kurt* kurt) {
    GamePlan* plan = new GamePlan(kurt);

    BPState* first_build_order = new BPState();
    first_build_order->SetUnitAmount(UNIT_TYPEID::TERRAN_MARINE, 1);

    plan->AddStatBuildOrderNode(first_build_order);

    return plan;
}

GamePlan* DynamicGamePlan(Kurt* kurt) {
    GamePlan* plan = new GamePlan(kurt);

    plan->AddDynBuildOrderNode();
    plan->AddDynCombatNode();

    return plan; 
}

GamePlan* DefendGamePlan(Kurt* kurt) {
    GamePlan* plan = new GamePlan(kurt);
    BPState* first_build_order = new BPState();
    BPState* second_build_order = new BPState();
    BPState* third_build_order = new BPState();

    first_build_order->SetUnitAmount(UNIT_TYPEID::TERRAN_REAPER, 1);
    plan->AddStatBuildOrderNode(first_build_order);
    plan->AddStatCombatNode(Kurt::DEFEND);
    second_build_order->SetUnitAmount(UNIT_TYPEID::TERRAN_REAPER, 1);
    plan->AddStatBuildOrderNode(second_build_order);
    third_build_order->SetUnitAmount(UNIT_TYPEID::TERRAN_BATTLECRUISER, 1);
    plan->AddStatBuildOrderNode(second_build_order);
    return plan;
}
#undef DEBUG // Stop debug prints from leaking
#undef TEST
#undef PRINT
