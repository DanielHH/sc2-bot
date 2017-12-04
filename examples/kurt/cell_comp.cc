#include "kurt.h"
#include <iostream>
#include "world_cell.h"
#include "world_representation.h"

struct CellComp {
    Kurt* kurt;
    float gas_factor = 1;
    float mineral_factor = 1;
    float seen_ago_factor = 10;
    float enemy_unit_factor = 1;
    float enemy_building_factor = 5;
    float neutral_watch_tower_factor = 1;
   
    CellComp(Kurt* parent_kurt) {
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
        float gas = cell->GetGasAmount();
        float mineral = cell->GetMineralAmount();
        float seen_game_steps_ago = kurt->Observation()->GetGameLoop() - cell->GetSeenOnGameStep();
        float building_worth = 0;
        float enemy_start_position_boost = 0;
        for(sc2::Point2D point : kurt->Observation()->GetGameInfo().enemy_start_locations) {
            if ((int) point.x/kurt->world_rep->chunk_size == (int) cell->GetCellRealX()/kurt->world_rep->chunk_size &&
                (int) point.y/kurt->world_rep->chunk_size == (int) cell->GetCellRealY()/kurt->world_rep->chunk_size) {
                enemy_start_position_boost += 3000000;
                std::cout << "enemy start" << std::endl;
            }
        }
        for (const sc2::Unit* building :  cell->GetBuildings()) {
            auto& attributes = kurt->Observation()->GetUnitTypeData().at(building->unit_type);
            building_worth += attributes.mineral_cost + attributes.vespene_cost;
        }
        return gas_factor*gas + mineral_factor*mineral + seen_ago_factor*seen_game_steps_ago + enemy_building_factor*building_worth + enemy_start_position_boost/kurt->Observation()->GetGameLoop();
    }
};
