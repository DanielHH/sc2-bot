#include "BPState.h"

#include "constants.h"
#include "action_enum.h"
#include "action_repr.h"
#include "exec_action.h"
#include "kurt.h"

#include <sc2api/sc2_api.h>

#include <iostream>
#include <fstream>
#include <iterator>
#include <map>
#include <vector>

//#define DEBUG // Comment out to disable debug prints in this file.
#ifdef DEBUG
#include <iostream>
#define PRINT(s) std::cout << s << std::endl;
#define TEST(s) s
#else
#define PRINT(s)
#define TEST(s)
#endif // DEBUG

#if __cplusplus == 201703L
#define FALLTHROUGH [[fallthrough]]
#else
#define FALLTHROUGH
#endif
#define UNREACHABLE PRINT(__FILE__ << "." << __LINE__ << ": UNREACHABLE statement reached");assert(false);

using namespace sc2;

BPState::BPState() {
}

BPState::BPState(BPState * const state) {
    for (auto it = state->UnitsBegin(); it != state->UnitsEnd(); ++it) {
        SetUnitAmount(it->first, it->second);
    }
    for (auto it = state->UnitsProdBegin(); it != state->UnitsProdEnd(); ++it){
        SetUnitProdAmount(it->first, it->second);
    }
    for (auto it = state->UnitsAvailableBegin(); it != state->UnitsAvailableEnd(); ++it) {
        SetUnitAvailableAmount(it->first, it->second);
    }
    for (auto it = state->actions.begin(); it != state->actions.end(); ++it){
        actions.push_back(*it);
    }
    time = state->GetTime();
}

BPState::BPState(Kurt * const kurt) {
    const ObservationInterface* observation = kurt->Observation();
    std::vector<const Unit*> commandcenters;
    Units units_self = observation->GetUnits(Unit::Alliance::Self);
    //
    // Add the majority of all our units
    //
    for (auto unit : units_self) {
        if (unit->build_progress < 1) {
            continue;
        }
        UNIT_TYPEID type = unit->unit_type.ToType();
        IncreaseUnitAmount(type, 1);
        IncreaseUnitAvailableAmount(type, 1);
        switch (type) {
        case UNIT_TYPEID::TERRAN_REFINERY:
            // TERRAN_REFINERY added later
            IncreaseUnitAmount(type, -1);
            IncreaseUnitAvailableAmount(type, -1);
            break;
        case UNIT_TYPEID::TERRAN_COMMANDCENTER:
        case UNIT_TYPEID::TERRAN_ORBITALCOMMAND:
        case UNIT_TYPEID::TERRAN_PLANETARYFORTRESS: {
            IncreaseUnitAmount(UNIT_FAKEID::TERRAN_ANY_COMMANDCENTER, 1);
            IncreaseUnitAvailableAmount(UNIT_FAKEID::TERRAN_ANY_COMMANDCENTER, 1);
            commandcenters.push_back(unit);
            int harvesters = std::min(unit->assigned_harvesters, unit->ideal_harvesters);
            int townhall_h = unit->ideal_harvesters - harvesters;
            IncreaseUnitAmount(UNIT_FAKEID::TERRAN_SCV_MINERALS, harvesters);
            IncreaseUnitAvailableAmount(UNIT_FAKEID::TERRAN_SCV_MINERALS, harvesters);
            IncreaseUnitAmount(UNIT_FAKEID::TERRAN_TOWNHALL_SCV_MINERALS, townhall_h);
            IncreaseUnitAvailableAmount(UNIT_FAKEID::TERRAN_TOWNHALL_SCV_MINERALS, townhall_h);
            break;
        }
        case UNIT_TYPEID::TERRAN_COMMANDCENTERFLYING:
            IncreaseUnitAmount(UNIT_TYPEID::TERRAN_COMMANDCENTER, 1);
            IncreaseUnitAvailableAmount(UNIT_TYPEID::TERRAN_COMMANDCENTER, 1);
            IncreaseUnitAmount(UNIT_FAKEID::TERRAN_ANY_COMMANDCENTER, 1);
            IncreaseUnitAvailableAmount(UNIT_FAKEID::TERRAN_ANY_COMMANDCENTER, 1);
            AddAction(ACTION::FLY_COMMAND_CENTER);
            break;
        case UNIT_TYPEID::TERRAN_ORBITALCOMMANDFLYING:
            IncreaseUnitAmount(UNIT_TYPEID::TERRAN_ORBITALCOMMAND, 1);
            IncreaseUnitAvailableAmount(UNIT_TYPEID::TERRAN_ORBITALCOMMAND, 1);
            IncreaseUnitAmount(UNIT_FAKEID::TERRAN_ANY_COMMANDCENTER, 1);
            IncreaseUnitAvailableAmount(UNIT_FAKEID::TERRAN_ANY_COMMANDCENTER, 1);
            AddAction(ACTION::FLY_ORBITAL_COMMAND);
            break;
        case UNIT_TYPEID::TERRAN_BARRACKS:
            if (unit->add_on_tag != NullTag) {
                IncreaseUnitAmount(UNIT_TYPEID::TERRAN_BARRACKS, -1);
                IncreaseUnitAvailableAmount(UNIT_TYPEID::TERRAN_BARRACKS, -1);
            }
            FALLTHROUGH;
        case UNIT_TYPEID::TERRAN_BARRACKSREACTOR:
            IncreaseUnitAmount(UNIT_FAKEID::TERRAN_ANY_BARRACKS, 1);
            IncreaseUnitAvailableAmount(UNIT_FAKEID::TERRAN_ANY_BARRACKS, 1);
            break;
        case UNIT_TYPEID::TERRAN_BARRACKSFLYING:
            IncreaseUnitAmount(UNIT_TYPEID::TERRAN_BARRACKS, 1);
            IncreaseUnitAvailableAmount(UNIT_TYPEID::TERRAN_BARRACKS, 1);
            IncreaseUnitAmount(UNIT_FAKEID::TERRAN_ANY_BARRACKS, 1);
            IncreaseUnitAvailableAmount(UNIT_FAKEID::TERRAN_ANY_BARRACKS, 1);
            AddAction(ACTION::FLY_BARRACKS);
            break;
        case UNIT_TYPEID::TERRAN_FACTORY:
            if (unit->add_on_tag != NullTag) {
                IncreaseUnitAmount(UNIT_TYPEID::TERRAN_FACTORY, -1);
                IncreaseUnitAvailableAmount(UNIT_TYPEID::TERRAN_FACTORY, -1);
            }
            FALLTHROUGH;
        case UNIT_TYPEID::TERRAN_FACTORYREACTOR:
            IncreaseUnitAmount(UNIT_FAKEID::TERRAN_ANY_FACTORY, 1);
            IncreaseUnitAvailableAmount(UNIT_FAKEID::TERRAN_ANY_FACTORY, 1);
            break;
        case UNIT_TYPEID::TERRAN_FACTORYFLYING:
            IncreaseUnitAmount(UNIT_TYPEID::TERRAN_FACTORY, 1);
            IncreaseUnitAvailableAmount(UNIT_TYPEID::TERRAN_FACTORY, 1);
            IncreaseUnitAmount(UNIT_FAKEID::TERRAN_ANY_FACTORY, 1);
            IncreaseUnitAvailableAmount(UNIT_FAKEID::TERRAN_ANY_FACTORY, 1);
            AddAction(ACTION::FLY_FACTORY);
            break;
        case UNIT_TYPEID::TERRAN_STARPORT:
            if (unit->add_on_tag != NullTag) {
                IncreaseUnitAmount(UNIT_TYPEID::TERRAN_STARPORT, -1);
                IncreaseUnitAvailableAmount(UNIT_TYPEID::TERRAN_STARPORT, -1);
            }
            FALLTHROUGH;
        case UNIT_TYPEID::TERRAN_STARPORTREACTOR:
            IncreaseUnitAmount(UNIT_FAKEID::TERRAN_ANY_STARPORT, 1);
            IncreaseUnitAvailableAmount(UNIT_FAKEID::TERRAN_ANY_STARPORT, 1);
            break;
        case UNIT_TYPEID::TERRAN_STARPORTFLYING:
            IncreaseUnitAmount(UNIT_TYPEID::TERRAN_STARPORT, 1);
            IncreaseUnitAvailableAmount(UNIT_TYPEID::TERRAN_STARPORT, 1);
            IncreaseUnitAmount(UNIT_FAKEID::TERRAN_ANY_STARPORT, 1);
            IncreaseUnitAvailableAmount(UNIT_FAKEID::TERRAN_ANY_STARPORT, 1);
            AddAction(ACTION::FLY_STARPORT);
            break;
        }
    }
    //
    // Only add refineries that are close to our base
    //
    for (auto unit : units_self) {
        if (unit->build_progress < 1) {
            continue;
        }
        UNIT_TYPEID type = unit->unit_type.ToType();
        if (type == UNIT_TYPEID::TERRAN_REFINERY) {
            for (auto center : commandcenters) {
                if (DistanceSquared3D(center->pos, unit->pos) <
                        BASE_RESOURCE_TEST_RANGE2) {
                    IncreaseUnitAmount(type, 1);
                    IncreaseUnitAvailableAmount(type, 1);
                    int harvesters = std::min(unit->assigned_harvesters, unit->ideal_harvesters);
                    int townhall_h = unit->ideal_harvesters - harvesters;
                    IncreaseUnitAmount(UNIT_FAKEID::TERRAN_SCV_VESPENE, harvesters);
                    IncreaseUnitAvailableAmount(UNIT_FAKEID::TERRAN_SCV_VESPENE, harvesters);
                    IncreaseUnitAmount(UNIT_FAKEID::TERRAN_TOWNHALL_SCV_VESPENE, townhall_h);
                    IncreaseUnitAvailableAmount(UNIT_FAKEID::TERRAN_TOWNHALL_SCV_VESPENE, townhall_h);
                    break;
                }
            }
        }
    }
    //
    // Add nearby neutral resources
    //
    for (auto neutral : observation->GetUnits(Unit::Alliance::Neutral)) {
        UNIT_TYPEID type = neutral->unit_type.ToType();
        for (auto center : commandcenters) {
            if (DistanceSquared3D(center->pos, neutral->pos) <
                    BASE_RESOURCE_TEST_RANGE2) {
                IncreaseUnitAmount(type, 1);
                IncreaseUnitAvailableAmount(type, 1);
                break;
            }
        }
    }

    //
    // Add other non unit units
    //
    SetUnitAmount(UNIT_FAKEID::MINERALS, observation->GetMinerals());
    SetUnitAmount(UNIT_FAKEID::VESPENE, observation->GetVespene());
    SetUnitAmount(UNIT_FAKEID::FOOD_CAP, observation->GetFoodCap());
    SetUnitAmount(UNIT_FAKEID::FOOD_USED, observation->GetFoodUsed());
    SetUnitAvailableAmount(UNIT_FAKEID::MINERALS, observation->GetMinerals());
    SetUnitAvailableAmount(UNIT_FAKEID::VESPENE, observation->GetVespene());
    SetUnitAvailableAmount(UNIT_FAKEID::FOOD_CAP, observation->GetFoodCap());
    SetUnitAvailableAmount(UNIT_FAKEID::FOOD_USED, observation->GetFoodUsed());

    // SCV building structures are assigned as harvesters it seems
//    IncreaseUnitAmount(UNIT_FAKEID::TERRAN_SCV_MINERALS, kurt->scv_building.size());

    //
    // Add active actions
    //
    double start_time = observation->GetGameLoop() / (double) STEPS_PER_SEC;
    time = start_time;
    for (auto unit : observation->GetUnits(Unit::Alliance::Self)) {
        for (UnitOrder order : unit->orders) {
            AbilityID ability = order.ability_id;
            if (ActionRepr::convert_api_our.count(ability) == 0) {
                continue;
            }
            ACTION action = ActionRepr::convert_api_our.at(ability);
            ActiveAction aa(action);
            ActionRepr ar = ActionRepr::values.at(action);
            for (auto pair : ar.consumed) {
                UNIT_TYPEID type = pair.first;
                int amount = pair.second;
                IncreaseUnitAmount(type, amount);
                IncreaseUnitAvailableAmount(type, amount);
            }
            double time = aa.time_left - ExecAction::TimeSinceOrderSent(unit, kurt);
            if (time < 0) {
                time = 0;
            }
            if (! CanExecuteNowOrSoon(action)) {
                Print();
                std::cout << "Error: BPState: BPState(KURT): " <<
                    "Could not add action to model, action: " << action << std::endl;
//                throw std::runtime_error("BPPlan: BPPlan(KURT), add action failed");
                for (auto pair : ar.consumed) {
                    UNIT_TYPEID type = pair.first;
                    int amount = pair.second;
                    IncreaseUnitAmount(type, -amount);
                    IncreaseUnitAvailableAmount(type, -amount);
                }
                continue;
            }
            AddAction(action, time);
        }
    }

    //
    // More special units
    //
    int geyser_amount = GetUnitAmount(UNIT_TYPEID::NEUTRAL_VESPENEGEYSER);
    int refinery_amount = GetUnitAmount(UNIT_TYPEID::TERRAN_REFINERY);
    int refinery_prod_amount = GetUnitProdAmount(UNIT_TYPEID::TERRAN_REFINERY);
    geyser_amount -= refinery_amount + refinery_prod_amount;
    SetUnitAmount(UNIT_TYPEID::NEUTRAL_VESPENEGEYSER, geyser_amount);
    SetUnitAvailableAmount(UNIT_TYPEID::NEUTRAL_VESPENEGEYSER, geyser_amount);

    //
    // Test if time did increase during adding active actions
    //
    if (start_time != time) {
        Print();
        std::cout << "Error: BPState: Constructor(Kurt*): " <<
            "Adding action updated time on state" << std::endl;
        throw std::runtime_error("BPState: Invalid constructor.");
    }
}

BPState::~BPState() {
    for (int i = 0; i < children.size(); ++i) {
        delete children[i];
    }
}

void BPState::UpdateUntilAvailable(ACTION action) {
    ActionRepr ar = ActionRepr::values.at(action);
    UNIT_TYPEID minerals = UNIT_FAKEID::MINERALS;
    UNIT_TYPEID vespene = UNIT_FAKEID::VESPENE;
    unsigned long int num_iterations_waited = 0;
    unsigned long int last_print = 0;
    while (! CanExecuteNow(action)) {
        if (num_iterations_waited++ > 2000 && num_iterations_waited > last_print + 100) {
            last_print = num_iterations_waited;
            Print();
            std::cerr << "UpdateUntilAvailable: " << num_iterations_waited << " iterations\n";
        }
        if (!CanExecuteNowOrSoon(action)) {
            Print();
            std::cout << "Error: BPState: UpdateUntilAvailable: " <<
                "Action never available (test), action: " << action << std::endl;
            throw std::runtime_error("BPPlan: Update never available");
        }
        double minerals_time = 0;
        if (ar.consumed.count(minerals) != 0) {
            minerals_time = 1 / GetMineralRate() *
                std::max(0, ar.consumed.at(minerals) - GetUnitAvailableAmount(minerals));
        }
        double vespene_time = 0;
        if (ar.consumed.count(vespene) != 0) {
            vespene_time = 1 / GetVespeneRate() *
                std::max(0, ar.consumed.at(vespene) - GetUnitAvailableAmount(vespene));
        }
        double delta_time = std::max(minerals_time, vespene_time);
        if (! actions.empty()) {
            if (actions.front().time_left <= delta_time ||
                    delta_time == 0 || delta_time == INFINITY) {
                CompleteFirstAction();
                continue;
            }
        }
        if (delta_time == INFINITY) {
            Print();
            std::cout << "Error: BPState: UpdateUntilAvailable: " <<
                "Infinity, action: " << action << std::endl;
            throw std::runtime_error("BPPlan: Update INFINITY time");
        } else if (delta_time == 0) {
            Print();
            std::cout << "Error: BPState: UpdateUntilAvailable: " <<
                "Zero, action: " << action << std::endl;
            throw std::runtime_error("BPPlan: Update zero time");
        }
        SimpleUpdate(delta_time);
    }
}

void BPState::SimpleUpdate(double delta_time) {
    if (delta_time < 0) {
        std::cout << "Warning: BPState: SimpleUpdate: delta_time: "
            << delta_time << std::endl;
        delta_time = 0;
    }
    for (auto it = actions.begin(); it != actions.end(); ++it) {
        it->time_left -= delta_time;
    }
    int minerals = GetMinerals() +
        delta_time * GetMineralRate();
    SetUnitAvailableAmount(UNIT_FAKEID::MINERALS, minerals);
    int vespene = GetVespene() +
        delta_time * GetVespeneRate();
    SetUnitAvailableAmount(UNIT_FAKEID::VESPENE, vespene);
    time += delta_time;
}

bool BPState::SimulatePlan(BPPlan & plan) {
    for (auto it = plan.begin(); it != plan.end(); ++it) {
        ACTION action = *it;
        if (CanExecuteNowOrSoon(action)) {
            AddAction(action);
        } else {
            return false;
        }
    }
    CompleteAllActions();
    return true;
}

bool BPState::SimulatePlan(BPPlan * plan) {
    for (auto it = plan->begin(); it != plan->end(); ++it) {
        ACTION action = *it;
        if (CanExecuteNowOrSoon(action)) {
            AddAction(action);
        } else {
            return false;
        }
    }
    CompleteAllActions();
    return true;
}

void BPState::AddAction(ACTION action, double time) {
    UpdateUntilAvailable(action);
    ActionRepr ar = ActionRepr::values.at(action);
    for (auto pair : ar.consumed) {
        UNIT_TYPEID type = pair.first;
        int amount = pair.second;
        IncreaseUnitAmount(type, -amount);
        IncreaseUnitAvailableAmount(type, -amount);
    }
    for (auto pair : ar.borrowed) {
        UNIT_TYPEID type = pair.first;
        int amount = pair.second;
        IncreaseUnitAvailableAmount(type, -amount);
    }
    for (auto pair : ar.produced) {
        UNIT_TYPEID type = pair.first;
        int amount = pair.second;
        IncreaseUnitProdAmount(type, amount);
    }
    int food_diff = GetUnitProdAmount(UNIT_FAKEID::FOOD_CAP) +
        GetUnitAmount(UNIT_FAKEID::FOOD_CAP) - 200;
    food_diff = std::max(0, food_diff);
    IncreaseUnitProdAmount(UNIT_FAKEID::FOOD_CAP, -food_diff);

    ActiveAction aa(action);
    if (time >= 0) {
        aa.time_left = time;
    }
    for (auto it = actions.begin(); it != actions.end(); ++it) {
        ActiveAction other = *it;
        if (aa < other) {
            actions.insert(it, aa);
            return;
        }
    }
    actions.push_back(aa);
}

void BPState::CompleteAllActions() {
    while (CompleteFirstAction()) {}
}

bool BPState::CompleteFirstAction() {
    if (actions.empty()) {
        return false;
    }
    ActiveAction aa = actions.front();
    ACTION action = aa.action;
    actions.pop_front();
    SimpleUpdate(aa.time_left);
    ActionRepr ar = ActionRepr::values.at(action);
    for (auto pair : ar.borrowed) {
        UNIT_TYPEID type = pair.first;
        int amount = pair.second;
        IncreaseUnitAvailableAmount(type, amount);
    }
    for (auto pair : ar.produced) {
        UNIT_TYPEID type = pair.first;
        int amount = pair.second;
        IncreaseUnitAmount(type, amount);
        IncreaseUnitAvailableAmount(type, amount);
        IncreaseUnitProdAmount(type, -amount);
    }
    int food_cap = std::min(200, GetUnitAmount(UNIT_FAKEID::FOOD_CAP));
    SetUnitAmount(UNIT_FAKEID::FOOD_CAP, food_cap);
    SetUnitAvailableAmount(UNIT_FAKEID::FOOD_CAP, food_cap);
    return true;
}

bool BPState::CanExecuteNow(ACTION action) const {
    ActionRepr ar = ActionRepr::values.at(action);
    for (auto pair : ar.required) {
        if (pair.second > GetUnitAmount(pair.first)) {
            return false;
        }
    }
    for (auto pair : ar.consumed) {
        UNIT_TYPEID type = pair.first;
        int amount = pair.second;
        if (type == UNIT_FAKEID::FOOD_USED) {
            // subtract amount because negative food_used requirements
            if (GetUnitAvailableAmount(UNIT_FAKEID::FOOD_USED) - amount >
                    GetUnitAvailableAmount(UNIT_FAKEID::FOOD_CAP)) {
                return false;
            }
        } else {
            if (amount > GetUnitAvailableAmount(type)) {
                return false;
            }
        }
    }
    for (auto pair : ar.borrowed) {
        if (pair.second > GetUnitAvailableAmount(pair.first)) {
            return false;
        }
    }
    return true;
}

bool BPState::CanExecuteNowOrSoon(ACTION action) const {
    ActionRepr ar = ActionRepr::values.at(action);
    for (auto p : ar.required) {
        if (p.second > GetUnitAmount(p.first) + GetUnitProdAmount(p.first)) {
            return false;
        }
    }
    for (auto pair : ar.consumed) {
        UNIT_TYPEID type = pair.first;
        int amount = pair.second;
        if (type == UNIT_FAKEID::FOOD_USED) {
            // subtract amount because negative food_used requirements
            if (GetUnitAmount(UNIT_FAKEID::FOOD_USED) - amount >
                    GetUnitAvailableAmount(UNIT_FAKEID::FOOD_CAP) +
                    GetUnitProdAmount(UNIT_FAKEID::FOOD_CAP)) {
                return false;
            }
        } else if (amount > GetUnitAmount(type) + GetUnitProdAmount(type)) {
            if (type == UNIT_FAKEID::MINERALS &&
                    GetMineralRate() > 0) {
                continue;
            }
            if (type == UNIT_FAKEID::VESPENE &&
                    GetVespeneRate() > 0) {
                continue;
            }
            return false;
        }
    }
    for (auto p : ar.borrowed) {
        if (p.second > GetUnitAmount(p.first) + GetUnitProdAmount(p.first)) {
            return false;
        }
    }
    return true;
}

std::vector<ACTION> BPState::AvailableActions() const {
    std::vector<ACTION> aa;
    for (auto pair : ActionRepr::values) {
        ACTION action = pair.first;
        if (CanExecuteNowOrSoon(action)) {
            aa.push_back(action);
        }
    }
    return aa;
}

std::vector<ACTION> BPState::AvailableActions(
        std::set<ACTION> & selectable_actions) const {
    std::vector<ACTION> aa;
    for (auto action : selectable_actions) {
        if (CanExecuteNowOrSoon(action)) {
            aa.push_back(action);
        }
    }
    return aa;
}

int BPState::GetUnitAmount(UNIT_TYPEID type) const {
    if (unit_amount.count(type) == 0) {
        return 0;
    } else {
        return unit_amount.at(type);
    }
}

void BPState::SetUnitAmount(UNIT_TYPEID type, int amount) {
    unit_amount[type] = amount;
}

void BPState::IncreaseUnitAmount(UNIT_TYPEID type, int amount) {
    SetUnitAmount(type, GetUnitAmount(type) + amount);
}

int BPState::GetUnitProdAmount(UNIT_TYPEID type) const {
    if (unit_being_produced.count(type) == 0) {
        return 0;
    } else {
        return unit_being_produced.at(type);
    }
}

void BPState::IncreaseUnitProdAmount(UNIT_TYPEID type, int amount) {
    SetUnitProdAmount(type, GetUnitProdAmount(type) + amount);
}

void BPState::SetUnitProdAmount(UNIT_TYPEID type, int amount) {
    unit_being_produced[type] = amount;
}

/* Returns the amount of given units begining produced in this BPState */
int BPState::GetUnitAvailableAmount(sc2::UNIT_TYPEID type) const {
    if (unit_nonbusy.count(type) == 0) return 0;
    else return unit_nonbusy.at(type);
}

/* Set this BPState to think that is produces given amount of given unit */
void BPState::SetUnitAvailableAmount(sc2::UNIT_TYPEID type, int amount) {
    unit_nonbusy[type] = amount;
}

/* Increases the number of a certain unit being produced in this BPState */
void BPState::IncreaseUnitAvailableAmount(sc2::UNIT_TYPEID type, int amount) {
    unit_nonbusy[type] += amount;
}

std::map<sc2::UNIT_TYPEID, int>::iterator BPState::UnitsBegin() {
    return unit_amount.begin();
}

std::map<sc2::UNIT_TYPEID, int>::iterator BPState::UnitsEnd() {
    return unit_amount.end();
}

std::map<sc2::UNIT_TYPEID, int>::iterator BPState::UnitsProdBegin() {
    return unit_being_produced.begin();
}

std::map<sc2::UNIT_TYPEID, int>::iterator BPState::UnitsProdEnd() {
    return unit_being_produced.end();
}

std::map<sc2::UNIT_TYPEID, int>::iterator BPState::UnitsAvailableBegin() {
    return unit_nonbusy.begin();
}

std::map<sc2::UNIT_TYPEID, int>::iterator BPState::UnitsAvailableEnd() {
    return unit_nonbusy.end();
}

double BPState::GetTime() const {
    return time;
}

int BPState::GetMinerals() const {
    return GetUnitAvailableAmount(UNIT_FAKEID::MINERALS);
}

double BPState::GetMineralRate() const {
    return GetUnitAvailableAmount(UNIT_FAKEID::TERRAN_SCV_MINERALS) *
        MINERALS_PER_SEC_PER_SCV;
}

int BPState::GetVespene() const {
    return GetUnitAvailableAmount(UNIT_FAKEID::VESPENE);
}

double BPState::GetVespeneRate() const {
    return GetUnitAvailableAmount(UNIT_FAKEID::TERRAN_SCV_VESPENE) *
        VESPENE_PER_SEC_PER_SCV;
}

int BPState::GetFoodCap() const {
    return GetUnitAmount(UNIT_FAKEID::FOOD_CAP);
}

int BPState::GetFoodUsed() const {
    return GetUnitAmount(UNIT_FAKEID::FOOD_USED);
}

void BPState::Print() {
    std::cout << ">>> BPState" << std::endl;
    std::cout << "Gametime: " << GetTime() << std::endl;
    std::cout << "Minerals: " << GetMinerals();
    std::cout << ", Vespene: " << GetVespene();
    std::cout << ", Food: " << GetFoodUsed();
    std::cout << "/" << GetFoodCap();
    std::cout << " (" << GetUnitProdAmount(UNIT_FAKEID::FOOD_CAP) << ")";
    std::cout << std::endl;

    auto PrintUnit = [] (BPState * state, UNIT_TYPEID type) {
        if (    type == UNIT_FAKEID::MINERALS ||
                type == UNIT_FAKEID::VESPENE ||
                type == UNIT_FAKEID::FOOD_CAP ||
                type == UNIT_FAKEID::FOOD_USED) {
            return;
        }
        std::string name;
        switch (type) {
        case UNIT_FAKEID::TERRAN_SCV_MINERALS:
            name = "TERRAN_SCV_MINERALS";
            break;
        case UNIT_FAKEID::TERRAN_SCV_VESPENE:
            name = "TERRAN_SCV_VESPENE";
            break;
        case UNIT_FAKEID::TERRAN_MULE_MINERALS:
            name = "TERRAN_MULE_MINERALS";
            break;
        case UNIT_FAKEID::TERRAN_MULE_VESPENE:
            name = "TERRAN_MULE_VESPENE";
            break;
        case UNIT_FAKEID::TERRAN_ANY_COMMANDCENTER:
            name = "TERRAN_ANY_COMMANDCENTER";
            break;
        case UNIT_FAKEID::TERRAN_ANY_BARRACKS:
            name = "TERRAN_ANY_BARRACKS";
            break;
        case UNIT_FAKEID::TERRAN_ANY_FACTORY:
            name = "TERRAN_ANY_FACTORY";
            break;
        case UNIT_FAKEID::TERRAN_ANY_STARPORT:
            name = "TERRAN_ANY_STARPORT";
            break;
        case UNIT_FAKEID::TERRAN_TOWNHALL_SCV_MINERALS:
            name = "TERRAN_TOWNHALL_SCV_MINERALS";
            break;
        case UNIT_FAKEID::TERRAN_TOWNHALL_SCV_VESPENE:
            name = "TERRAN_TOWNHALL_SCV_VESPENE";
            break;
        default:
            name = UnitTypeToName(type);
            break;
        }
        std::cout << name << ": " << state->GetUnitAvailableAmount(type) << "/"
            << state->GetUnitAmount(type) << " ("
            << state->GetUnitProdAmount(type) << ")" << std::endl;
    };
    for (auto it = UnitsBegin(); it != UnitsEnd(); ++it) {
        UNIT_TYPEID type = it->first;
        PrintUnit(this, type);
    }
    for (auto it = UnitsProdBegin(); it != UnitsProdEnd(); ++it) {
        UNIT_TYPEID type = it->first;
        if (unit_amount.count(type) == 0) {
            PrintUnit(this, type);
        }
    }
    for (auto it = UnitsAvailableBegin(); it != UnitsAvailableEnd(); ++it) {
        UNIT_TYPEID type = it->first;
        if (unit_amount.count(type) == 0 && unit_being_produced.count(type) == 0) {
            PrintUnit(this, type);
        }
    }
    if (! actions.empty()) {
        std::cout << "Active actions:" << std::endl;
        for (ActiveAction aa : actions) {
            std::cout << aa.action << ", time left: " << aa.time_left << std::endl;
        }
    }
    std::cout << "BPState <<<" << std::endl;
}

bool BPState::operator<(BPState const &other) const {
    for (auto pair : unit_amount) {
        if (other.unit_amount.find(pair.first) != other.unit_amount.cend()) {
            if (pair.second < other.unit_amount.at(pair.first)) return true;
        }
    }
    return GetTime() > other.GetTime();
}

bool BPState::ContainsAllUnitsOf(BPState const &other) const {
    for (auto pair : other.unit_amount) {
        if (GetUnitAmount(pair.first) < pair.second) {
            return false;
        }
    }
    return true;
}

double BPState::ContainsPercentOf(BPState const * other) const {
    int tot, part;
    for (auto pair : other->unit_amount) {
        tot += pair.second;
        part += std::min(pair.second, GetUnitAmount(pair.first));
    }
    return part / (double) tot;
}

#undef DEBUG
#undef PRINT
#undef TEST
