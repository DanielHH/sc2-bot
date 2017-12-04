#include "plans.h"



using namespace sc2;
using namespace std;

GamePlan* Plans::CreateDefaultGamePlan(Kurt* kurt) {
    GamePlan* plan;
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

GamePlan* Plans::RushPlan(Kurt* kurt) {
    GamePlan* plan = new GamePlan(kurt);
    // Build order of 3 marines
    BPState* first_build_order = new BPState();
    BPState* second_build_order = new BPState();
    BPState* third_build_order = new BPState();

    //first_build
    
    first_build_order->SetUnitAmount(UNIT_TYPEID::TERRAN_SCV, 4);
    first_build_order->SetUnitAmount(UNIT_TYPEID::TERRAN_BARRACKS, 1);
    
    //second_build
    second_build_order->SetUnitAmount(UNIT_TYPEID::TERRAN_MARINE, 7);
    //third_build
    third_build_order->SetUnitAmount(UNIT_TYPEID::TERRAN_REAPER, 20);
    third_build_order->SetUnitAmount(UNIT_TYPEID::TERRAN_MARINE, 20);


    plan->AddStatBuildOrderNode(first_build_order);
    plan->AddStatBuildOrderNode(second_build_order);
    plan->AddStatCombatNode(Kurt::ATTACK);
    plan->AddStatBuildOrderNode(third_build_order);


    return plan;
}

GamePlan* Plans::DynamicGamePlan(Kurt* kurt) {
    GamePlan* plan;

    plan->AddDynBuildOrderNode();
    plan->AddDynCombatNode();

    return plan; 

}
