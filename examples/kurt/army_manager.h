#pragma once

#include <sc2api/sc2_api.h>
#include "kurt.h"


class ArmyManager {

public:
    enum CombatMode { DEFEND, ATTACK, HARASS };

private:
    CombatMode current_combat_mode;
    Kurt* kurt;

public:
    ArmyManager(Kurt* kurt);

    // Gets called every game step
    void OnStep(const sc2::ObservationInterface* observation);

    void GroupNewUnit(const sc2::Unit* unit, const sc2::ObservationInterface* observation);

    CombatMode GetCombatMode();

    void SetCombatMode(CombatMode new_combat_mode);

private:
    bool IsArmyUnit(const sc2::Unit* unit, const sc2::ObservationInterface* observation);

    bool IsStructure(const sc2::Unit* unit, const sc2::ObservationInterface* observation);
};
