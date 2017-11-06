#include "sc2api/sc2_api.h"

#include <iostream>

using namespace sc2;

class Bot : public Agent {
public:
    virtual void OnGameStart() {
        std::cout << "Hellow world!" << std::endl;
    }

    virtual void OnStep() {
        auto obs = Observation();
        std::cout << "GameLoop " << obs->GetGameLoop() << std::endl;
        std::cout << "Minerals " << obs->GetMinerals() << std::endl;
        std::cout << "Vespene  " << obs->GetVespene() << std::endl;
    }

    virtual void OnUnitIdle(const Unit* unit) {
        switch (unit->unit_type.ToType()) {
            case sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER: {
                Actions()->UnitCommand(unit, sc2::ABILITY_ID::TRAIN_SCV);
                break;
            }
            default: {
                break;
            }
        }
    }
};
