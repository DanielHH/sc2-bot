#include "MCTS.h"

#include "sc2api/sc2_api.h"

#include "BPPlan.h"
#include "BPState.h"
#include "action_enum.h"

#include <math.h>
#include <vector>
#include <iostream>

//#define DEBUG // Comment out to disable debug prints in this file.
#ifdef DEBUG
#include <iostream>
#define PRINT(s) std::cout << s << std::endl;
#define TEST(s) s
#else
#define PRINT(s)
#define TEST(s)
#endif // DEBUG

//#define PRINT_TREE // Comment out to disable printing the search tree.
#ifdef PRINT_TREE
#include <iostream>
#define PT(s); std::cout << s;
#define PTLN(s); std::cout << s << std::endl;
#else
#define PT(s);
#define PTLN(s);
#endif

MCTS::MCTS(BPState * const root_, BPState * const goal_) {
std::cout << "MCTS: 34" << std::endl;
    root = new BPState(root_);
std::cout << "MCTS: 35" << std::endl;
    goal = new BPState(goal_);

std::cout << "MCTS: 37" << std::endl;
    BPPlan basic_plan;
std::cout << "MCTS: 38" << std::endl;
    basic_plan.AddBasicPlan(root, goal);

std::cout << "MCTS: 40" << std::endl;
    BPState tmp(root);
std::cout << "MCTS: 41" << std::endl;
    double time = -1, mineral_rate = 0, vespene_rate = 0;
std::cout << "MCTS: 42" << std::endl;
    int mineral_stock = 0, vespene_stock = 0;
std::cout << "MCTS: 43" << std::endl;
    if (tmp.SimulatePlan(basic_plan)) {
std::cout << "MCTS: 44" << std::endl;
        time = tmp.GetTime() - root->GetTime();
std::cout << "MCTS: 45" << std::endl;
        mineral_rate = tmp.GetMineralRate();
std::cout << "MCTS: 46" << std::endl;
        vespene_rate = tmp.GetVespeneRate();
std::cout << "MCTS: 47" << std::endl;
        mineral_stock = tmp.GetMinerals();
std::cout << "MCTS: 48" << std::endl;
        vespene_stock = tmp.GetVespene();
std::cout << "MCTS: 49" << std::endl;
    }
std::cout << "MCTS: 50" << std::endl;
    // The min border is needed since the reward is relative the basic plan.
std::cout << "MCTS: 51" << std::endl;
    // If e.g. basic_vespene_rate is 0, all higher rates gets the same reward.
std::cout << "MCTS: 52" << std::endl;
    basic_time = std::max(40.0, time);
std::cout << "MCTS: 53" << std::endl;
    basic_mineral_rate = std::max(5.0, mineral_rate);
std::cout << "MCTS: 54" << std::endl;
    basic_vespene_rate = std::max(10.0, vespene_rate);
std::cout << "MCTS: 55" << std::endl;
    basic_mineral_stock = std::max(2000, mineral_stock);
std::cout << "MCTS: 56" << std::endl;
    basic_vespene_stock = std::max(2000, vespene_stock);

std::cout << "MCTS: 58" << std::endl;
    root->parent = nullptr;
std::cout << "MCTS: 59" << std::endl;
    root->iter_amount = 1;
std::cout << "MCTS: 60" << std::endl;
    if (time == -1) {
std::cout << "MCTS: 61" << std::endl;
        root->reward = 0;
std::cout << "MCTS: 62" << std::endl;
    } else {
std::cout << "MCTS: 63" << std::endl;
        root->reward = CalcReward(time, mineral_rate, vespene_rate,
                mineral_stock, vespene_stock);
std::cout << "MCTS: 65" << std::endl;
    }
std::cout << "MCTS: 66" << std::endl;
    root->reward_stop = root->reward;

std::cout << "MCTS: 68" << std::endl;
    BPState init_state;
std::cout << "MCTS: 69" << std::endl;
    init_state.SetUnitAmount(sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER, 1);
std::cout << "MCTS: 70" << std::endl;
    init_state.SetUnitAmount(sc2::UNIT_TYPEID::TERRAN_SCV, 12);
std::cout << "MCTS: 71" << std::endl;
    BPPlan init_basic_plan;
std::cout << "MCTS: 72" << std::endl;
    init_basic_plan.AddBasicPlan(&init_state, goal);
std::cout << "MCTS: 73" << std::endl;
    for (ACTION action : init_basic_plan) {
std::cout << "MCTS: 74" << std::endl;
        interesting_actions.insert(action);
std::cout << "MCTS: 75" << std::endl;
    }
std::cout << "MCTS: 76" << std::endl;
    interesting_actions.insert(ACTION::SCV_GATHER_MINERALS);
std::cout << "MCTS: 77" << std::endl;
    interesting_actions.insert(ACTION::SCV_GATHER_VESPENE);
std::cout << "MCTS: 78" << std::endl;
    interesting_actions.insert(ACTION::BUILD_REFINERY);
std::cout << "MCTS: 79" << std::endl;
    interesting_actions.insert(ACTION::BUILD_SUPPLY_DEPOT);
std::cout << "MCTS: 80" << std::endl;
    interesting_actions.insert(ACTION::BUILD_COMMAND_CENTER);
std::cout << "MCTS: 81" << std::endl;
    interesting_actions.insert(ACTION::TRAIN_SCV);
std::cout << "MCTS: 82" << std::endl;
    if (interesting_actions.count(ACTION::BUILD_BARRACKS) != 0) {
std::cout << "MCTS: 83" << std::endl;
        interesting_actions.insert(ACTION::BUILD_BARRACKS_REACTOR);
std::cout << "MCTS: 84" << std::endl;
    }
std::cout << "MCTS: 85" << std::endl;
    if (interesting_actions.count(ACTION::BUILD_FACTORY) != 0) {
std::cout << "MCTS: 86" << std::endl;
        interesting_actions.insert(ACTION::BUILD_FACTORY_REACTOR);
std::cout << "MCTS: 87" << std::endl;
    }
std::cout << "MCTS: 88" << std::endl;
    if (interesting_actions.count(ACTION::BUILD_STARPORT) != 0) {
std::cout << "MCTS: 89" << std::endl;
        interesting_actions.insert(ACTION::BUILD_STARPORT_REACTOR);
std::cout << "MCTS: 90" << std::endl;
    }
}

MCTS::~MCTS() {
std::cout << "MCTS: 94" << std::endl;
    delete root;
std::cout << "MCTS: 95" << std::endl;
    delete goal;
}

double MCTS::CalcReward(
        double time,
        double mineral_rate, double vespene_rate,
        int mineral_stock, int vespene_stock) {
std::cout << "MCTS: 102" << std::endl;
    return
        time_portion * basic_time / (time + basic_time) +
        m_rate_portion * mineral_rate / (mineral_rate + basic_mineral_rate) +
        v_rate_portion * vespene_rate / (vespene_rate + basic_vespene_rate) +
        m_stock_portion * mineral_stock / (mineral_stock+basic_mineral_stock) +
        v_stock_portion * vespene_stock / (vespene_stock+basic_vespene_stock);
}

void MCTS::Search(int num_iterations) {
std::cout << "MCTS: 111" << std::endl;
    for (int i = 0; i < num_iterations; ++i) {
std::cout << "MCTS: 112" << std::endl;
        SearchOnce();
std::cout << "MCTS: 113" << std::endl;
    }
}

void MCTS::SearchOnce() {
std::cout << "MCTS: 117" << std::endl;
    /*
std::cout << "MCTS: 118" << std::endl;
     * Select phase.
std::cout << "MCTS: 119" << std::endl;
     */
std::cout << "MCTS: 120" << std::endl;
    PRINT("Select phase.")
std::cout << "MCTS: 121" << std::endl;
    BPState * leaf = root;
std::cout << "MCTS: 122" << std::endl;
    while (true) {
std::cout << "MCTS: 123" << std::endl;
        if (leaf->available_actions.empty()) {
std::cout << "MCTS: 124" << std::endl;
            leaf->available_actions =
                leaf->AvailableActions(interesting_actions);
std::cout << "MCTS: 126" << std::endl;
            leaf->children.resize(leaf->available_actions.size(), nullptr);
std::cout << "MCTS: 127" << std::endl;
            if (leaf->available_actions.empty()) {
std::cout << "MCTS: 128" << std::endl;
                leaf->Print();
std::cout << "MCTS: 129" << std::endl;
                std::cout << "Warning: MCTS: Leaf node found" << std::endl;
std::cout << "MCTS: 130" << std::endl;
                return;
std::cout << "MCTS: 131" << std::endl;
            }
std::cout << "MCTS: 132" << std::endl;
            break;
std::cout << "MCTS: 133" << std::endl;
        }
std::cout << "MCTS: 134" << std::endl;
        if (leaf->iter_amount - 1 < leaf->available_actions.size()) {
std::cout << "MCTS: 135" << std::endl;
            break;
std::cout << "MCTS: 136" << std::endl;
        }
std::cout << "MCTS: 137" << std::endl;
        int best_index = -1;
std::cout << "MCTS: 138" << std::endl;
        double best_score = -1;
std::cout << "MCTS: 139" << std::endl;
        for (int i = 0; i < leaf->children.size(); ++i) {
std::cout << "MCTS: 140" << std::endl;
            BPState * child = leaf->children[i];
std::cout << "MCTS: 141" << std::endl;
            double score = child->reward + EXPLORATION_SCALE *
                std::sqrt(std::log(leaf->iter_amount) / child->iter_amount);
std::cout << "MCTS: 143" << std::endl;
            if (score > best_score) {
std::cout << "MCTS: 144" << std::endl;
                best_score = score;
std::cout << "MCTS: 145" << std::endl;
                best_index = i;
std::cout << "MCTS: 146" << std::endl;
            }
std::cout << "MCTS: 147" << std::endl;
        }
std::cout << "MCTS: 148" << std::endl;
        if (best_index == -1) {
std::cout << "MCTS: 149" << std::endl;
            leaf->Print();
std::cout << "MCTS: 150" << std::endl;
            std::cout << "Error: MCTS: Search: " <<
                "State with invalid children." << std::endl;
std::cout << "MCTS: 152" << std::endl;
            return;
std::cout << "MCTS: 153" << std::endl;
        }
std::cout << "MCTS: 154" << std::endl;
        leaf = leaf->children[best_index];
std::cout << "MCTS: 155" << std::endl;
    }
std::cout << "MCTS: 156" << std::endl;
    /*
std::cout << "MCTS: 157" << std::endl;
     * Expansion phase.
std::cout << "MCTS: 158" << std::endl;
     */
std::cout << "MCTS: 159" << std::endl;
    PRINT("Expansion phase.")
std::cout << "MCTS: 160" << std::endl;
    BPState * new_state = nullptr;
std::cout << "MCTS: 161" << std::endl;
    {
std::cout << "MCTS: 162" << std::endl;
        int index = leaf->iter_amount - 1;
std::cout << "MCTS: 163" << std::endl;
        ACTION action = leaf->available_actions[index];
std::cout << "MCTS: 164" << std::endl;
        new_state = new BPState(leaf);
std::cout << "MCTS: 165" << std::endl;
        new_state->AddAction(action);
std::cout << "MCTS: 166" << std::endl;
        new_state->reward = 0;
std::cout << "MCTS: 167" << std::endl;
        new_state->iter_amount = 0;
std::cout << "MCTS: 168" << std::endl;
        leaf->children[index] = new_state;
std::cout << "MCTS: 169" << std::endl;
        new_state->parent = leaf;
std::cout << "MCTS: 170" << std::endl;
    }
std::cout << "MCTS: 171" << std::endl;
    /*
std::cout << "MCTS: 172" << std::endl;
     * Simulation phase.
std::cout << "MCTS: 173" << std::endl;
     */
std::cout << "MCTS: 174" << std::endl;
    PRINT("Simulation phase.")
std::cout << "MCTS: 175" << std::endl;
    BPPlan tail;
std::cout << "MCTS: 176" << std::endl;
    tail.AddBasicPlan(new_state, goal);
std::cout << "MCTS: 177" << std::endl;
    BPState tmp(new_state);
std::cout << "MCTS: 178" << std::endl;
    double reward = 0;
std::cout << "MCTS: 179" << std::endl;
    if (tmp.SimulatePlan(tail)) {
std::cout << "MCTS: 180" << std::endl;
        double time = tmp.GetTime() - root->GetTime();
std::cout << "MCTS: 181" << std::endl;
        double mineral_rate = tmp.GetMineralRate();
std::cout << "MCTS: 182" << std::endl;
        double vespene_rate = tmp.GetVespeneRate();
std::cout << "MCTS: 183" << std::endl;
        int mineral_stock = tmp.GetMinerals();
std::cout << "MCTS: 184" << std::endl;
        int vespene_stock = tmp.GetVespene();
std::cout << "MCTS: 185" << std::endl;
        reward = CalcReward(time, mineral_rate, vespene_rate,
                mineral_stock, vespene_stock);
std::cout << "MCTS: 187" << std::endl;
    }
std::cout << "MCTS: 188" << std::endl;
    /*
std::cout << "MCTS: 189" << std::endl;
     * Backpropagation phase.
std::cout << "MCTS: 190" << std::endl;
     */
std::cout << "MCTS: 191" << std::endl;
    PRINT("Backpropagation phase.")
std::cout << "MCTS: 192" << std::endl;
    new_state->reward_stop = reward;
std::cout << "MCTS: 193" << std::endl;
    BPState * curr = new_state;
std::cout << "MCTS: 194" << std::endl;
    while (curr != nullptr) {
std::cout << "MCTS: 195" << std::endl;
        curr->reward = std::max(curr->reward, reward);
std::cout << "MCTS: 196" << std::endl;
        curr->iter_amount++;
std::cout << "MCTS: 197" << std::endl;
        curr = curr->parent;
std::cout << "MCTS: 198" << std::endl;
    }
}

BPPlan MCTS::BestPlan() {
std::cout << "MCTS: 202" << std::endl;
    auto spaces = [] (int len) {
std::cout << "MCTS: 203" << std::endl;
        std::string ans = "";
std::cout << "MCTS: 204" << std::endl;
        while (len-- > 0) {
std::cout << "MCTS: 205" << std::endl;
            ans += " ";
std::cout << "MCTS: 206" << std::endl;
        }
std::cout << "MCTS: 207" << std::endl;
        return ans;
std::cout << "MCTS: 208" << std::endl;
    };
std::cout << "MCTS: 209" << std::endl;
    auto plan_print = [] (BPState * curr, BPState * goal) {
std::cout << "MCTS: 210" << std::endl;
        BPPlan tmp;
std::cout << "MCTS: 211" << std::endl;
        tmp.AddBasicPlan(curr, goal);
std::cout << "MCTS: 212" << std::endl;
        return tmp;
std::cout << "MCTS: 213" << std::endl;
    };
std::cout << "MCTS: 214" << std::endl;
    BPPlan plan;
std::cout << "MCTS: 215" << std::endl;
    BPState * curr = root;
std::cout << "MCTS: 216" << std::endl;
    PTLN("");
std::cout << "MCTS: 217" << std::endl;
    PT("--> ROOT: (" << curr->iter_amount << ") ")
std::cout << "MCTS: 218" << std::endl;
    PTLN(curr->reward_stop << " / " << curr->reward);
std::cout << "MCTS: 219" << std::endl;
    int depth = 0;
std::cout << "MCTS: 220" << std::endl;
    while (true) {
std::cout << "MCTS: 221" << std::endl;
        ++depth;
std::cout << "MCTS: 222" << std::endl;
        if (curr->available_actions.empty()) {
std::cout << "MCTS: 223" << std::endl;
            PTLN(spaces(depth * 2) << "(No children expanded)");
std::cout << "MCTS: 224" << std::endl;
            break;
std::cout << "MCTS: 225" << std::endl;
        }
std::cout << "MCTS: 226" << std::endl;
        int best_index = -1;
std::cout << "MCTS: 227" << std::endl;
        double best_score = -1;
std::cout << "MCTS: 228" << std::endl;
        for (int i = 0; i < curr->children.size(); ++i) {
std::cout << "MCTS: 229" << std::endl;
            BPState * child = curr->children[i];
std::cout << "MCTS: 230" << std::endl;
            if (child == nullptr) {
std::cout << "MCTS: 231" << std::endl;
                PTLN(spaces(depth * 2) << "(Not all children expanded)");
std::cout << "MCTS: 232" << std::endl;
                break;
std::cout << "MCTS: 233" << std::endl;
            }
std::cout << "MCTS: 234" << std::endl;
            double score = child->reward;
std::cout << "MCTS: 235" << std::endl;
            PT(spaces(depth * 2) << curr->available_actions[i] << ": (");
std::cout << "MCTS: 236" << std::endl;
            PT(child->iter_amount << ") ")
std::cout << "MCTS: 237" << std::endl;
            PTLN(child->reward_stop << " / " << child->reward);
std::cout << "MCTS: 238" << std::endl;
            if (score > best_score) {
std::cout << "MCTS: 239" << std::endl;
                best_score = score;
std::cout << "MCTS: 240" << std::endl;
                best_index = i;
std::cout << "MCTS: 241" << std::endl;
            }
std::cout << "MCTS: 242" << std::endl;
        }
std::cout << "MCTS: 243" << std::endl;
        if (best_index == -1) {
std::cout << "MCTS: 244" << std::endl;
            curr->Print();
std::cout << "MCTS: 245" << std::endl;
            std::cout << "Error: MCTS: BestPlan: " <<
                "State with invalid children." << std::endl;
std::cout << "MCTS: 247" << std::endl;
            break;
std::cout << "MCTS: 248" << std::endl;
        }
std::cout << "MCTS: 249" << std::endl;
        if (curr->reward_stop >= best_score) {
std::cout << "MCTS: 250" << std::endl;
            PTLN(spaces(depth * 2) << "--> Choosing to stop here");
std::cout << "MCTS: 251" << std::endl;
            break;
std::cout << "MCTS: 252" << std::endl;
        }
std::cout << "MCTS: 253" << std::endl;
        PT(spaces(depth * 2) << "--> Choosing ");
std::cout << "MCTS: 254" << std::endl;
        PTLN(curr->available_actions[best_index]);
std::cout << "MCTS: 255" << std::endl;
        plan.push_back(curr->available_actions[best_index]);
std::cout << "MCTS: 256" << std::endl;
        curr = curr->children[best_index];
std::cout << "MCTS: 257" << std::endl;
    }
std::cout << "MCTS: 258" << std::endl;
    PTLN("MCTS (head): " << plan);
std::cout << "MCTS: 259" << std::endl;
    PTLN("BasicPlan (tail): " << plan_print(curr, goal));
std::cout << "MCTS: 260" << std::endl;
    plan.AddBasicPlan(curr, goal);
std::cout << "MCTS: 261" << std::endl;
    return plan;
}

#undef DEBUG
#undef PRINT
#undef TEST
