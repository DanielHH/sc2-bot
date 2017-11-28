#pragma once

#include <sc2api/sc2_api.h>
#include "kurt.h"


class ArmyManager {

private:
    Kurt* kurt;

public:
    ArmyManager(Kurt* kurt);

    // Gets called every game step
    void OnStep(const sc2::ObservationInterface* observation);

    void GroupNewUnit(const sc2::Unit* unit, const sc2::ObservationInterface* observation);

private:
    bool IsArmyUnit(const sc2::Unit* unit, const sc2::ObservationInterface* observation);

    bool IsStructure(const sc2::Unit* unit, const sc2::ObservationInterface* observation);
};
