#include "build_manager.h"

#include <vector>
#include <iostream>

#include "kurt.h"
#include "constants.h"
#include "action_repr.h"
#include "action_enum.h"
#include "exec_action.h"
#include "BPState.h"
#include "BPPlan.h"
#include "MCTS.h"

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

std::map<sc2::UNIT_TYPEID, std::vector<sc2::UNIT_TYPEID> > BuildManager::tech_tree_2;
bool BuildManager::setup_finished = false;

BuildManager::BuildManager(Kurt *const agent_) : agent(agent_) {
std::cout << "build_manager: 30" << std::endl;
    
}

std::vector<UnitTypeData*> BuildManager::GetRequirements(UnitTypeData* unit) {
std::cout << "build_manager: 34" << std::endl;
    assert(setup_finished);
std::cout << "build_manager: 35" << std::endl;
    std::vector<UnitTypeData*> requirements;
std::cout << "build_manager: 36" << std::endl;
    UNIT_TYPEID type = unit->unit_type_id.ToType();
std::cout << "build_manager: 37" << std::endl;
    if (unit->tech_requirement != UNIT_TYPEID::INVALID) {
std::cout << "build_manager: 38" << std::endl;
        requirements.push_back(Kurt::GetUnitType(unit->tech_requirement));
std::cout << "build_manager: 39" << std::endl;
    } else if (tech_tree_2.count(type) > 0) {
std::cout << "build_manager: 40" << std::endl;
        for (UNIT_TYPEID req_elem : tech_tree_2[type]) {
std::cout << "build_manager: 41" << std::endl;
            requirements.push_back(Kurt::GetUnitType(req_elem));
std::cout << "build_manager: 42" << std::endl;
        }
std::cout << "build_manager: 43" << std::endl;
    }
std::cout << "build_manager: 44" << std::endl;
    return requirements;
}

std::vector<UNIT_TYPEID> BuildManager::GetRequirements(UNIT_TYPEID unit) {
std::cout << "build_manager: 48" << std::endl;
    assert(setup_finished);
std::cout << "build_manager: 49" << std::endl;
    std::vector<UNIT_TYPEID> requirements;
std::cout << "build_manager: 50" << std::endl;
    UnitTypeData *data = Kurt::GetUnitType(unit);
std::cout << "build_manager: 51" << std::endl;
    if (tech_tree_2.count(unit) > 0) {
std::cout << "build_manager: 52" << std::endl;
        return tech_tree_2.at(unit);
std::cout << "build_manager: 53" << std::endl;
    } else if (data->tech_requirement != UNIT_TYPEID::INVALID) {
std::cout << "build_manager: 54" << std::endl;
        requirements.push_back(data->tech_requirement);
std::cout << "build_manager: 55" << std::endl;
    }
std::cout << "build_manager: 56" << std::endl;
    return requirements;
}

void TestMCTS(BPState * const start, BPState * const goal) {
std::cout << "build_manager: 60" << std::endl;
    std::cout << "----------------------------" << std::endl;

std::cout << "build_manager: 62" << std::endl;
    MCTS mcts(start, goal);
std::cout << "build_manager: 63" << std::endl;
    mcts.Search(5000);
std::cout << "build_manager: 64" << std::endl;
    BPPlan best_plan = mcts.BestPlan();
std::cout << "build_manager: 65" << std::endl;
    BPState tmp1(start);
std::cout << "build_manager: 66" << std::endl;
    tmp1.SimulatePlan(best_plan);
std::cout << "build_manager: 67" << std::endl;
    double best_time = tmp1.GetTime() - start->GetTime();
std::cout << "build_manager: 68" << std::endl;
    double best_mineral_rate = tmp1.GetMineralRate();
std::cout << "build_manager: 69" << std::endl;
    double best_vespene_rate = tmp1.GetVespeneRate();

std::cout << "build_manager: 71" << std::endl;
    BPPlan basic_plan;
std::cout << "build_manager: 72" << std::endl;
    basic_plan.AddBasicPlan(start, goal);
std::cout << "build_manager: 73" << std::endl;
    BPState tmp2(start);
std::cout << "build_manager: 74" << std::endl;
    tmp2.SimulatePlan(basic_plan);
std::cout << "build_manager: 75" << std::endl;
    double basic_time = tmp2.GetTime() - start->GetTime();
std::cout << "build_manager: 76" << std::endl;
    double basic_mineral_rate = tmp2.GetMineralRate();
std::cout << "build_manager: 77" << std::endl;
    double basic_vespene_rate = tmp2.GetVespeneRate();

std::cout << "build_manager: 79" << std::endl;
    std::cout << best_plan << std::endl;
std::cout << "build_manager: 80" << std::endl;
    std::cout << "vespene rate basic:  " << basic_vespene_rate << std::endl;
std::cout << "build_manager: 81" << std::endl;
    std::cout << "vespene rate best: " << best_vespene_rate << std::endl;
std::cout << "build_manager: 82" << std::endl;
    std::cout << "mineral rate basic:  " << basic_mineral_rate << std::endl;
std::cout << "build_manager: 83" << std::endl;
    std::cout << "mineral rate best: " << best_mineral_rate << std::endl;
std::cout << "build_manager: 84" << std::endl;
    std::cout << "time basic:  " << basic_time << std::endl;
std::cout << "build_manager: 85" << std::endl;
    std::cout << "time best: " << best_time << std::endl;
std::cout << "build_manager: 86" << std::endl;
    std::cout << "----------------------------" << std::endl;
}

void BuildManager::OnStep(const ObservationInterface* observation) {
std::cout << "build_manager: 90" << std::endl;
    // If there is no goal in life, what is the point of living?
std::cout << "build_manager: 91" << std::endl;
    if (goal == nullptr) {
std::cout << "build_manager: 92" << std::endl;
        return;
std::cout << "build_manager: 93" << std::endl;
    }
std::cout << "build_manager: 94" << std::endl;
    BPState current_state(agent);
std::cout << "build_manager: 95" << std::endl;
    //
std::cout << "build_manager: 96" << std::endl;
    // Do the searching for a better plan.
std::cout << "build_manager: 97" << std::endl;
    //
std::cout << "build_manager: 98" << std::endl;
    if (steps_until_replan <= -1) {
std::cout << "build_manager: 99" << std::endl;
        if (mcts != nullptr) {
std::cout << "build_manager: 100" << std::endl;
            delete mcts;
std::cout << "build_manager: 101" << std::endl;
            mcts = nullptr;
std::cout << "build_manager: 102" << std::endl;
        }
std::cout << "build_manager: 103" << std::endl;
        steps_until_replan = STEPS_BETWEEN_REPLAN_MIN;
std::cout << "build_manager: 104" << std::endl;
        old_steps_until_replan = STEPS_BETWEEN_REPLAN_MIN;
std::cout << "build_manager: 105" << std::endl;
        current_plan.resize(0);
std::cout << "build_manager: 106" << std::endl;
        mcts = new MCTS(&current_state, goal);
std::cout << "build_manager: 107" << std::endl;
    } else if (steps_until_replan == 0) {
std::cout << "build_manager: 108" << std::endl;
        int next_steps_until_replan;
std::cout << "build_manager: 109" << std::endl;
        if (current_plan.empty()) {
std::cout << "build_manager: 110" << std::endl;
            next_steps_until_replan = old_steps_until_replan * 2;
std::cout << "build_manager: 111" << std::endl;
        } else {
std::cout << "build_manager: 112" << std::endl;
            next_steps_until_replan = old_steps_until_replan - 1;
std::cout << "build_manager: 113" << std::endl;
        }
std::cout << "build_manager: 114" << std::endl;
        next_steps_until_replan = std::max(
                next_steps_until_replan, STEPS_BETWEEN_REPLAN_MIN);
std::cout << "build_manager: 116" << std::endl;
        next_steps_until_replan = std::min(
                next_steps_until_replan, STEPS_BETWEEN_REPLAN_MAX);
std::cout << "build_manager: 118" << std::endl;
        old_steps_until_replan = next_steps_until_replan;
std::cout << "build_manager: 119" << std::endl;
        steps_until_replan = next_steps_until_replan;

std::cout << "build_manager: 121" << std::endl;
        BPPlan next_plan;
std::cout << "build_manager: 122" << std::endl;
        next_plan.insert(next_plan.end(), current_plan.begin(), current_plan.end());
std::cout << "build_manager: 123" << std::endl;
        current_plan.resize(0);
std::cout << "build_manager: 124" << std::endl;
        BPPlan additional_plan = mcts->BestPlan();
std::cout << "build_manager: 125" << std::endl;
        next_plan.insert(next_plan.end(),
                additional_plan.begin(), additional_plan.end());
std::cout << "build_manager: 127" << std::endl;
        delete mcts;
std::cout << "build_manager: 128" << std::endl;
        mcts = nullptr;

std::cout << "build_manager: 130" << std::endl;
        bool abort = false;
std::cout << "build_manager: 131" << std::endl;
        double search_start = current_state.GetTime() +
            steps_until_replan * SEC_PER_STEP;
std::cout << "build_manager: 133" << std::endl;
        BPState test_state(current_state);
std::cout << "build_manager: 134" << std::endl;
        for (int i = 0; i < next_plan.size(); ++i) {
std::cout << "build_manager: 135" << std::endl;
            ACTION action = next_plan[i];
std::cout << "build_manager: 136" << std::endl;
            if (test_state.CanExecuteNowOrSoon(action)) {
std::cout << "build_manager: 137" << std::endl;
                test_state.AddAction(action);
std::cout << "build_manager: 138" << std::endl;
                if (test_state.GetTime() <= search_start) {
std::cout << "build_manager: 139" << std::endl;
                    current_plan.push_back(action);
std::cout << "build_manager: 140" << std::endl;
                } else {
std::cout << "build_manager: 141" << std::endl;
                    break;
std::cout << "build_manager: 142" << std::endl;
                }
std::cout << "build_manager: 143" << std::endl;
            } else {
std::cout << "build_manager: 144" << std::endl;
                abort = true;
std::cout << "build_manager: 145" << std::endl;
                break;
std::cout << "build_manager: 146" << std::endl;
            }
std::cout << "build_manager: 147" << std::endl;
        }
std::cout << "build_manager: 148" << std::endl;
        if (! abort) {
std::cout << "build_manager: 149" << std::endl;
            BPState search_from(current_state);
std::cout << "build_manager: 150" << std::endl;
            if (search_from.SimulatePlan(current_plan)) {
std::cout << "build_manager: 151" << std::endl;
                mcts = new MCTS(&search_from, goal);
std::cout << "build_manager: 152" << std::endl;
            } else {
std::cout << "build_manager: 153" << std::endl;
                abort = true;
std::cout << "build_manager: 154" << std::endl;
            }
std::cout << "build_manager: 155" << std::endl;
        }
std::cout << "build_manager: 156" << std::endl;
        if (abort) {
std::cout << "build_manager: 157" << std::endl;
            steps_until_replan = STEPS_BETWEEN_REPLAN_MIN;
std::cout << "build_manager: 158" << std::endl;
            old_steps_until_replan = STEPS_BETWEEN_REPLAN_MIN;
std::cout << "build_manager: 159" << std::endl;
            current_plan.resize(0);
std::cout << "build_manager: 160" << std::endl;
            mcts = new MCTS(&current_state, goal);
std::cout << "build_manager: 161" << std::endl;
        }

std::cout << "build_manager: 163" << std::endl;
        PRINT("--- Created new plan ---")
std::cout << "build_manager: 164" << std::endl;
        TEST(current_state.Print();)
std::cout << "build_manager: 165" << std::endl;
        PRINT(current_plan)
std::cout << "build_manager: 166" << std::endl;
        PRINT("next_steps_until_replan: " << next_steps_until_replan)
std::cout << "build_manager: 167" << std::endl;
    }
std::cout << "build_manager: 168" << std::endl;
    mcts->Search(SEARCH_ITER_PER_STEP);
std::cout << "build_manager: 169" << std::endl;
    --steps_until_replan;
std::cout << "build_manager: 170" << std::endl;
    //
std::cout << "build_manager: 171" << std::endl;
    // Execute the current plan
std::cout << "build_manager: 172" << std::endl;
    //
std::cout << "build_manager: 173" << std::endl;
    current_plan.ExecuteStep(agent, &current_state);
std::cout << "build_manager: 174" << std::endl;
    //
std::cout << "build_manager: 175" << std::endl;
    // Test if a goal is reached
std::cout << "build_manager: 176" << std::endl;
    //
std::cout << "build_manager: 177" << std::endl;
    if (current_state.ContainsAllUnitsOf(goal)) {
std::cout << "build_manager: 178" << std::endl;
        goal = nullptr;
std::cout << "build_manager: 179" << std::endl;
        PRINT("--- Goal is reached, gametime: " << current_state.GetTime() << " ---")
std::cout << "build_manager: 180" << std::endl;
        agent->ExecuteSubplan();
std::cout << "build_manager: 181" << std::endl;
    }

std::cout << "build_manager: 183" << std::endl;
    //
std::cout << "build_manager: 184" << std::endl;
    // TESTING
std::cout << "build_manager: 185" << std::endl;
    //
std::cout << "build_manager: 186" << std::endl;
    /*
std::cout << "build_manager: 187" << std::endl;
    BPState * curr = new BPState(agent);

std::cout << "build_manager: 189" << std::endl;
    BPState * goal = new BPState();
std::cout << "build_manager: 190" << std::endl;
    goal->SetUnitAmount(UNIT_TYPEID::TERRAN_GHOST, 9);
std::cout << "build_manager: 191" << std::endl;
    TestMCTS(curr, goal);
std::cout << "build_manager: 192" << std::endl;
    goal->SetUnitAmount(UNIT_TYPEID::TERRAN_GHOST, 0);
std::cout << "build_manager: 193" << std::endl;
    goal->SetUnitAmount(UNIT_TYPEID::TERRAN_BATTLECRUISER, 5);
std::cout << "build_manager: 194" << std::endl;
    TestMCTS(curr, goal);
std::cout << "build_manager: 195" << std::endl;
    delete curr;

std::cout << "build_manager: 197" << std::endl;
    throw std::runtime_error("hehehe");
std::cout << "build_manager: 198" << std::endl;
    */
}

void BuildManager::OnGameStart(const ObservationInterface* observation) {
std::cout << "build_manager: 202" << std::endl;
    SetUpTechTree(observation);
std::cout << "build_manager: 203" << std::endl;
    ActionRepr::InitConvertMap();
std::cout << "build_manager: 204" << std::endl;
    ExecAction::Init(agent);
std::cout << "build_manager: 205" << std::endl;
    setup_finished = true;
}

void BuildManager::SetGoal(BPState * const goal_) {
std::cout << "build_manager: 209" << std::endl;
    if (goal != nullptr) {
std::cout << "build_manager: 210" << std::endl;
        delete goal;
std::cout << "build_manager: 211" << std::endl;
    }
std::cout << "build_manager: 212" << std::endl;
    goal = goal_;
std::cout << "build_manager: 213" << std::endl;
    BPState curr(agent);
std::cout << "build_manager: 214" << std::endl;
    for (auto it = goal->UnitsBegin(); it != goal->UnitsEnd(); ++it) {
std::cout << "build_manager: 215" << std::endl;
        goal->IncreaseUnitAmount(it->first, curr.GetUnitAmount(it->first));
std::cout << "build_manager: 216" << std::endl;
    }
std::cout << "build_manager: 217" << std::endl;
    InitNewPlan();
}

void BuildManager::InitNewPlan() {
std::cout << "build_manager: 221" << std::endl;
    steps_until_replan = -1;
}

void BuildManager::SetUpTechTree(const ObservationInterface* observation) {
std::cout << "build_manager: 225" << std::endl;
    // The lines without push_back is for to specify no requirements

std::cout << "build_manager: 227" << std::endl;
    tech_tree_2[UNIT_TYPEID::TERRAN_COMMANDCENTER];
std::cout << "build_manager: 228" << std::endl;
    tech_tree_2[UNIT_TYPEID::TERRAN_PLANETARYFORTRESS].push_back(UNIT_TYPEID::TERRAN_COMMANDCENTER);
std::cout << "build_manager: 229" << std::endl;
    tech_tree_2[UNIT_TYPEID::TERRAN_PLANETARYFORTRESS].push_back(UNIT_TYPEID::TERRAN_ENGINEERINGBAY);
std::cout << "build_manager: 230" << std::endl;
    tech_tree_2[UNIT_TYPEID::TERRAN_ORBITALCOMMAND].push_back(UNIT_TYPEID::TERRAN_COMMANDCENTER);
std::cout << "build_manager: 231" << std::endl;
    tech_tree_2[UNIT_TYPEID::TERRAN_ORBITALCOMMAND].push_back(UNIT_TYPEID::TERRAN_BARRACKS);

std::cout << "build_manager: 233" << std::endl;
    tech_tree_2[UNIT_TYPEID::TERRAN_SUPPLYDEPOT];

std::cout << "build_manager: 235" << std::endl;
    tech_tree_2[UNIT_TYPEID::TERRAN_BARRACKS].push_back(UNIT_TYPEID::TERRAN_SUPPLYDEPOT);
std::cout << "build_manager: 236" << std::endl;
    tech_tree_2[UNIT_TYPEID::TERRAN_BARRACKSTECHLAB].push_back(UNIT_TYPEID::TERRAN_BARRACKS);
std::cout << "build_manager: 237" << std::endl;
    tech_tree_2[UNIT_TYPEID::TERRAN_BARRACKSREACTOR].push_back(UNIT_TYPEID::TERRAN_BARRACKS);

std::cout << "build_manager: 239" << std::endl;
    tech_tree_2[UNIT_TYPEID::TERRAN_ENGINEERINGBAY].push_back(UNIT_TYPEID::TERRAN_COMMANDCENTER);

std::cout << "build_manager: 241" << std::endl;
    tech_tree_2[UNIT_TYPEID::TERRAN_BUNKER];

std::cout << "build_manager: 243" << std::endl;
    tech_tree_2[UNIT_TYPEID::TERRAN_SENSORTOWER].push_back(UNIT_TYPEID::TERRAN_ENGINEERINGBAY);

std::cout << "build_manager: 245" << std::endl;
    tech_tree_2[UNIT_TYPEID::TERRAN_MISSILETURRET].push_back(UNIT_TYPEID::TERRAN_ENGINEERINGBAY);

std::cout << "build_manager: 247" << std::endl;
    tech_tree_2[UNIT_TYPEID::TERRAN_FACTORY].push_back(UNIT_TYPEID::TERRAN_BARRACKS);
std::cout << "build_manager: 248" << std::endl;
    tech_tree_2[UNIT_TYPEID::TERRAN_FACTORYTECHLAB].push_back(UNIT_TYPEID::TERRAN_FACTORY);
std::cout << "build_manager: 249" << std::endl;
    tech_tree_2[UNIT_TYPEID::TERRAN_FACTORYREACTOR].push_back(UNIT_TYPEID::TERRAN_FACTORY);

std::cout << "build_manager: 251" << std::endl;
    tech_tree_2[UNIT_TYPEID::TERRAN_GHOSTACADEMY].push_back(UNIT_TYPEID::TERRAN_BARRACKS);

std::cout << "build_manager: 253" << std::endl;
    tech_tree_2[UNIT_TYPEID::TERRAN_STARPORT].push_back(UNIT_TYPEID::TERRAN_FACTORY);
std::cout << "build_manager: 254" << std::endl;
    tech_tree_2[UNIT_TYPEID::TERRAN_STARPORTTECHLAB].push_back(UNIT_TYPEID::TERRAN_STARPORT);
std::cout << "build_manager: 255" << std::endl;
    tech_tree_2[UNIT_TYPEID::TERRAN_STARPORTREACTOR].push_back(UNIT_TYPEID::TERRAN_STARPORT);

std::cout << "build_manager: 257" << std::endl;
    tech_tree_2[UNIT_TYPEID::TERRAN_ARMORY].push_back(UNIT_TYPEID::TERRAN_FACTORY);

std::cout << "build_manager: 259" << std::endl;
    tech_tree_2[UNIT_TYPEID::TERRAN_FUSIONCORE].push_back(UNIT_TYPEID::TERRAN_STARPORT);

std::cout << "build_manager: 261" << std::endl;
    tech_tree_2[UNIT_TYPEID::TERRAN_SCV].push_back(UNIT_TYPEID::TERRAN_COMMANDCENTER);
std::cout << "build_manager: 262" << std::endl;
    tech_tree_2[UNIT_TYPEID::TERRAN_MULE].push_back(UNIT_TYPEID::TERRAN_ORBITALCOMMAND);

std::cout << "build_manager: 264" << std::endl;
    tech_tree_2[UNIT_TYPEID::TERRAN_MARINE].push_back(UNIT_TYPEID::TERRAN_BARRACKS);
std::cout << "build_manager: 265" << std::endl;
    tech_tree_2[UNIT_TYPEID::TERRAN_REAPER].push_back(UNIT_TYPEID::TERRAN_BARRACKS);
std::cout << "build_manager: 266" << std::endl;
    tech_tree_2[UNIT_TYPEID::TERRAN_MARAUDER].push_back(UNIT_TYPEID::TERRAN_BARRACKSTECHLAB);
std::cout << "build_manager: 267" << std::endl;
    tech_tree_2[UNIT_TYPEID::TERRAN_GHOST].push_back(UNIT_TYPEID::TERRAN_BARRACKSTECHLAB);
std::cout << "build_manager: 268" << std::endl;
    tech_tree_2[UNIT_TYPEID::TERRAN_GHOST].push_back(UNIT_TYPEID::TERRAN_GHOSTACADEMY);

std::cout << "build_manager: 270" << std::endl;
    tech_tree_2[UNIT_TYPEID::TERRAN_HELLIONTANK].push_back(UNIT_TYPEID::TERRAN_ARMORY);
std::cout << "build_manager: 271" << std::endl;
    tech_tree_2[UNIT_TYPEID::TERRAN_HELLION].push_back(UNIT_TYPEID::TERRAN_FACTORY);
std::cout << "build_manager: 272" << std::endl;
    tech_tree_2[UNIT_TYPEID::TERRAN_WIDOWMINE].push_back(UNIT_TYPEID::TERRAN_FACTORYREACTOR);
std::cout << "build_manager: 273" << std::endl;
    tech_tree_2[UNIT_TYPEID::TERRAN_SIEGETANK].push_back(UNIT_TYPEID::TERRAN_FACTORYTECHLAB);
std::cout << "build_manager: 274" << std::endl;
    tech_tree_2[UNIT_TYPEID::TERRAN_CYCLONE].push_back(UNIT_TYPEID::TERRAN_FACTORY);
std::cout << "build_manager: 275" << std::endl;
    tech_tree_2[UNIT_TYPEID::TERRAN_THOR].push_back(UNIT_TYPEID::TERRAN_FACTORYTECHLAB);
std::cout << "build_manager: 276" << std::endl;
    tech_tree_2[UNIT_TYPEID::TERRAN_THOR].push_back(UNIT_TYPEID::TERRAN_ARMORY);

std::cout << "build_manager: 278" << std::endl;
    tech_tree_2[UNIT_TYPEID::TERRAN_VIKINGFIGHTER].push_back(UNIT_TYPEID::TERRAN_STARPORT);
std::cout << "build_manager: 279" << std::endl;
    tech_tree_2[UNIT_TYPEID::TERRAN_MEDIVAC].push_back(UNIT_TYPEID::TERRAN_STARPORT);
std::cout << "build_manager: 280" << std::endl;
    tech_tree_2[UNIT_TYPEID::TERRAN_LIBERATOR].push_back(UNIT_TYPEID::TERRAN_STARPORT);
std::cout << "build_manager: 281" << std::endl;
    tech_tree_2[UNIT_TYPEID::TERRAN_RAVEN].push_back(UNIT_TYPEID::TERRAN_STARPORTTECHLAB);
std::cout << "build_manager: 282" << std::endl;
    tech_tree_2[UNIT_TYPEID::TERRAN_BANSHEE].push_back(UNIT_TYPEID::TERRAN_STARPORTTECHLAB);
std::cout << "build_manager: 283" << std::endl;
    tech_tree_2[UNIT_TYPEID::TERRAN_BATTLECRUISER].push_back(UNIT_TYPEID::TERRAN_STARPORTTECHLAB);
std::cout << "build_manager: 284" << std::endl;
    tech_tree_2[UNIT_TYPEID::TERRAN_BATTLECRUISER].push_back(UNIT_TYPEID::TERRAN_FUSIONCORE);
}

#undef DEBUG
#undef PRINT
#undef TEST
