#include "strategy_manager.h"
#include <iostream>

using namespace sc2;

StrategyManager::StrategyManager() {

}

void StrategyManager::OnStep(const ObservationInterface* observation) {
    // DO ALL DE ARMY STUFF

}

//Save enemy units
void StrategyManager::SaveSpottedEnemyUnits(const ObservationInterface* observation) {
    Units enemy_units = observation->GetUnits(Unit::Alliance::Enemy);
    for (const auto unit : enemy_units) {

    }

}

// Returns true if Enemy structure is observed. Med ObservationInterface ser man väl allt?
bool StrategyManager::FindEnemyStructure(const ObservationInterface* observation, const Unit*& enemy_unit) {
    Units my_units = observation->GetUnits(Unit::Alliance::Enemy);
    for (const auto unit : my_units) {
        if (unit->unit_type == UNIT_TYPEID::TERRAN_COMMANDCENTER ||
            unit->unit_type == UNIT_TYPEID::TERRAN_SUPPLYDEPOT ||
            unit->unit_type == UNIT_TYPEID::TERRAN_BARRACKS) {
            enemy_unit = unit;
            return true;
        }
    }

    return false;
}


