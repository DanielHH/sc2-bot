#include "kurt.h"
#include <iostream>
#include "world_cell.h"
#include "world_representation.h"

struct CellCompDefend {
    Kurt* kurt;
    
    CellCompDefend(Kurt* parent_kurt) {
        kurt = parent_kurt;
    }
    
    bool SmartComp(WorldCell* a, WorldCell* b) {
        float a_worth = CalculateCellWorth(a);
        float b_worth = CalculateCellWorth(b);
        return a_worth > b_worth;
    }
    
    bool operator()(WorldCell* lhs, WorldCell* rhs) {
        return SmartComp(lhs, rhs);
    }
    
    float CalculateCellWorth(WorldCell* cell) {
        float seen_game_steps_ago = kurt->Observation()->GetGameLoop() - cell->GetSeenOnGameStep();
        
        float building_worth = 0;
        float building_dmg = 0;
        //float allied_troops_worth = 0;
        float enemy_troops_worth = 0;
        float final_worth = 0;
        
        float building_dmg_factor = 20;
        
        if (!cell->GetAlliedBuildings().empty()) {
            for (const sc2::Unit* building : cell->GetAlliedBuildings()) {
                building_dmg_factor = 20;
                auto& attributes = kurt->Observation()->GetUnitTypeData().at(building->unit_type);
                building_worth += attributes.mineral_cost + attributes.vespene_cost;
                building_dmg += building->health_max - building->health;
                if (building->build_progress < 1) {
                    building_dmg_factor = building_dmg_factor / 2;
                }
                final_worth += 1 + building_worth + building_dmg_factor*building_dmg;
            }
            for (const sc2::Unit* trooper :  cell->GetTroops()) {
                auto& attributes = kurt->Observation()->GetUnitTypeData().at(trooper->unit_type);
                enemy_troops_worth += attributes.mineral_cost + attributes.vespene_cost;
                final_worth += enemy_troops_worth;
            }
            return final_worth;
        } else return 0;

    }
};
