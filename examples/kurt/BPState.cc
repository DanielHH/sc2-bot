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
    for (auto unit : observation->GetUnits(Unit::Alliance::Self)) {
        if (unit->build_progress < 1) {
            continue;
        }
        UNIT_TYPEID type = unit->unit_type.ToType();
        IncreaseUnitAmount(type, 1);
        IncreaseUnitAvailableAmount(type, 1);
        switch (type) {
        case UNIT_TYPEID::TERRAN_COMMANDCENTER:
        case UNIT_TYPEID::TERRAN_ORBITALCOMMAND:
        case UNIT_TYPEID::TERRAN_PLANETARYFORTRESS:
            IncreaseUnitAmount(UNIT_FAKEID::TERRAN_ANY_COMMANDCENTER, 1);
            IncreaseUnitAvailableAmount(UNIT_FAKEID::TERRAN_ANY_COMMANDCENTER, 1);
            commandcenters.push_back(unit);
            break;
        case UNIT_TYPEID::TERRAN_BARRACKSREACTOR:
            IncreaseUnitAmount(UNIT_FAKEID::TERRAN_ANY_BARRACKS, 2);
            IncreaseUnitAvailableAmount(UNIT_FAKEID::TERRAN_ANY_BARRACKS, 2);
            break;
        case UNIT_TYPEID::TERRAN_BARRACKS:
        case UNIT_TYPEID::TERRAN_BARRACKSTECHLAB:
            IncreaseUnitAmount(UNIT_FAKEID::TERRAN_ANY_BARRACKS, 1);
            IncreaseUnitAvailableAmount(UNIT_FAKEID::TERRAN_ANY_BARRACKS, 1);
            break;
        case UNIT_TYPEID::TERRAN_FACTORYREACTOR:
            IncreaseUnitAmount(UNIT_FAKEID::TERRAN_ANY_FACTORY, 2);
            IncreaseUnitAvailableAmount(UNIT_FAKEID::TERRAN_ANY_FACTORY, 2);
            break;
        case UNIT_TYPEID::TERRAN_FACTORY:
        case UNIT_TYPEID::TERRAN_FACTORYTECHLAB:
            IncreaseUnitAmount(UNIT_FAKEID::TERRAN_ANY_FACTORY, 1);
            IncreaseUnitAvailableAmount(UNIT_FAKEID::TERRAN_ANY_FACTORY, 1);
            break;
        case UNIT_TYPEID::TERRAN_STARPORTREACTOR:
            IncreaseUnitAmount(UNIT_FAKEID::TERRAN_ANY_STARPORT, 2);
            IncreaseUnitAvailableAmount(UNIT_FAKEID::TERRAN_ANY_STARPORT, 2);
            break;
        case UNIT_TYPEID::TERRAN_STARPORT:
        case UNIT_TYPEID::TERRAN_STARPORTTECHLAB:
            IncreaseUnitAmount(UNIT_FAKEID::TERRAN_ANY_STARPORT, 1);
            IncreaseUnitAvailableAmount(UNIT_FAKEID::TERRAN_ANY_STARPORT, 1);
            break;
        }
    }
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

    int refinery_amount = GetUnitAmount(UNIT_TYPEID::TERRAN_REFINERY);
    SetUnitAmount(UNIT_FAKEID::TERRAN_SCV_MINERALS, kurt->scv_minerals.size());
    SetUnitAmount(UNIT_FAKEID::TERRAN_SCV_VESPENE, kurt->scv_vespene.size());
    SetUnitAmount(UNIT_FAKEID::MINERALS, observation->GetMinerals());
    SetUnitAmount(UNIT_FAKEID::VESPENE, observation->GetVespene());
    SetUnitAmount(UNIT_FAKEID::FOOD_CAP, observation->GetFoodCap());
    SetUnitAmount(UNIT_FAKEID::FOOD_USED, observation->GetFoodUsed());
    SetUnitAmount(UNIT_FAKEID::TERRAN_TOWNHALL_SCV_VESPENE, 3 * refinery_amount - kurt->scv_vespene.size());
    SetUnitAvailableAmount(UNIT_FAKEID::TERRAN_SCV_MINERALS, kurt->scv_minerals.size());
    SetUnitAvailableAmount(UNIT_FAKEID::TERRAN_SCV_VESPENE, kurt->scv_vespene.size());
    SetUnitAvailableAmount(UNIT_FAKEID::MINERALS, observation->GetMinerals());
    SetUnitAvailableAmount(UNIT_FAKEID::VESPENE, observation->GetVespene());
    SetUnitAvailableAmount(UNIT_FAKEID::FOOD_CAP, observation->GetFoodCap());
    SetUnitAvailableAmount(UNIT_FAKEID::FOOD_USED, observation->GetFoodUsed());
    SetUnitAvailableAmount(UNIT_FAKEID::TERRAN_TOWNHALL_SCV_VESPENE, 3 * refinery_amount - kurt->scv_vespene.size());
    int num_mineral_worker_slots = 16 * GetUnitAvailableAmount(UNIT_FAKEID::TERRAN_ANY_COMMANDCENTER) -
        GetUnitAvailableAmount(UNIT_FAKEID::TERRAN_SCV_MINERALS);
    SetUnitAmount(UNIT_FAKEID::TERRAN_TOWNHALL_SCV_MINERALS, num_mineral_worker_slots);
    SetUnitAvailableAmount(UNIT_FAKEID::TERRAN_TOWNHALL_SCV_MINERALS, num_mineral_worker_slots);

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
            AddAction(action, time);
        }
    }

    int geyser_amount = GetUnitAmount(UNIT_TYPEID::NEUTRAL_VESPENEGEYSER);
    int refinery_prod_amount = GetUnitProdAmount(UNIT_TYPEID::TERRAN_REFINERY);
    geyser_amount -= refinery_amount + refinery_prod_amount;
    SetUnitAmount(UNIT_TYPEID::NEUTRAL_VESPENEGEYSER, geyser_amount);
    SetUnitAvailableAmount(UNIT_TYPEID::NEUTRAL_VESPENEGEYSER, geyser_amount);

    // Currently, an scv that starts building a refinery is added to the scv_vespene list.
    IncreaseUnitAmount(UNIT_FAKEID::TERRAN_SCV_VESPENE, -refinery_prod_amount);
    IncreaseUnitAvailableAmount(UNIT_FAKEID::TERRAN_SCV_VESPENE, -refinery_prod_amount);
    IncreaseUnitAmount(UNIT_FAKEID::TERRAN_TOWNHALL_SCV_VESPENE, refinery_prod_amount);
    IncreaseUnitAvailableAmount(UNIT_FAKEID::TERRAN_TOWNHALL_SCV_VESPENE, refinery_prod_amount);

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
    while (! CanExecuteNow(action)) {
        double minerals_time = 0;
        if (ar.consumed.count(minerals) != 0) {
            minerals_time = 1 / GetMineralRate() *
                std::max(0, ar.consumed[minerals] - GetUnitAvailableAmount(minerals));
        }
        double vespene_time = 0;
        if (ar.consumed.count(vespene) != 0) {
            vespene_time = 1 / GetVespeneRate() *
                std::max(0, ar.consumed[vespene] - GetUnitAvailableAmount(vespene));
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
                "infinity, action: " << action << std::endl;
            throw std::runtime_error("BPPlan: Update INFINITY time");
        } else if (delta_time == 0) {
            Print();
            std::cout << "Error: BPState: UpdateUntilAvailable: " <<
                "Action never available, action: " << action << std::endl;
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

void BPState::SimulatePlan(BPPlan & plan) {
    for (auto it = plan.begin(); it != plan.end(); ++it) {
        AddAction(*it);
    }
    CompleteAllActions();
}

void BPState::SimulatePlan(BPPlan * plan) {
    for (auto it = plan->begin(); it != plan->end(); ++it) {
        AddAction(*it);
    }
    CompleteAllActions();
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
        if (pair.second > GetUnitAvailableAmount(pair.first)) {
            return false;
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
        if (amount > GetUnitAmount(type) + GetUnitProdAmount(type)) {
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
    std::cout << "/" << GetFoodCap() << std::endl;

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

#undef DEBUG
#undef PRINT
#undef TEST
