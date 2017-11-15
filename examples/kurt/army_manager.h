#pragma once

#include <sc2api\sc2_api.h>
#include "shared_resources.h"

enum CombatMode { DEFEND, ATTACK, HARASS };

class ArmyManager {

private:
    SharedResources* shared_resources;
    CombatMode current_combat_mode;

public:
    ArmyManager(SharedResources* shared_resources);

    // Gets called every game step
    void OnStep(const sc2::ObservationInterface* observation);

    void GroupNewUnit(const sc2::Unit* unit, const sc2::ObservationInterface* observation);

    CombatMode GetCombatMode();

    void SetCombatMode(CombatMode new_combat_mode);

private:
    bool IsArmyUnit(const sc2::Unit* unit, const sc2::ObservationInterface* observation);

    bool IsStructure(const sc2::Unit* unit, const sc2::ObservationInterface* observation);
};