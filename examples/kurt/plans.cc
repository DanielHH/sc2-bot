#include "game_plan.h"
#include <sc2api\sc2_api.h>


//default_plan = new GamePlan();

using namespace sc2;
using namespace std;

GamePlan* CreateDefaultGamePlan(Kurt* kurt) {
    GamePlan* plan;
    // Build order of 3 marines
    BPState* first_build_order= new BPState();
    BPState* second_build_order = new BPState();
    BPState* third_build_order = new BPState();
    BPState* fourth_build_order = new BPState();
    BPState* fifth_build_order = new BPState();
    //first_build
    first_build_order->SetUnitAmount(UNIT_TYPEID::TERRAN_MARINE, 3);
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


    plan->AddBuildOrderNode(first_build_order);
    plan->AddCombatNode(kurt);
    plan->AddBuildOrderNode(second_build_order);
    plan->AddCombatNode(kurt);
    plan->AddBuildOrderNode(third_build_order);
    plan->AddCombatNode(kurt);
    plan->AddBuildOrderNode(fourth_build_order);
    plan->AddCombatNode(kurt);
    plan->AddBuildOrderNode(fifth_build_order);

    return plan;
}