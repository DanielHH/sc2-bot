#include "kurt.h"
#include <iostream>
#include "world_cell.h"
#include "world_representation.h"

struct CellCompArmy {
    Kurt* kurt;
    float gas_factor = 1;
    float mineral_factor = 1;
    float seen_ago_factor = 1;
    float enemy_unit_factor = 100;
    float enemy_building_factor = 100;
    float neutral_watch_tower_factor = 1;
    bool one_time_bonus = true;
    
    CellCompArmy(Kurt* parent_kurt) {
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
        float enemy_troops_worth = 0;
        float enemy_start_position_boost = 0;
        float final_worth = 0;
        bool is_enemy_start = false;
        for(sc2::Point2D point : kurt->Observation()->GetGameInfo().enemy_start_locations) {
            if ((int) point.x/kurt->world_rep->chunk_size == (int) cell->GetCellRealX()/kurt->world_rep->chunk_size &&
                (int) point.y/kurt->world_rep->chunk_size == (int) cell->GetCellRealY()/kurt->world_rep->chunk_size) {
                is_enemy_start = true;
                
            }
        }
        for (const sc2::Unit* building :  cell->GetBuildings()) {
            auto& attributes = kurt->Observation()->GetUnitTypeData().at(building->unit_type);
            building_worth += attributes.mineral_cost + attributes.vespene_cost;
        }
        for (const sc2::Unit* trooper :  cell->GetTroops()) {
            auto& attributes = kurt->Observation()->GetUnitTypeData().at(trooper->unit_type);
            enemy_troops_worth += attributes.mineral_cost + attributes.vespene_cost;
        }
        
        if(gas > 0) {
            final_worth +=10;
        }
        if(mineral > 0) {
            final_worth += 10;
        }
        if(building_worth > 0) {
            final_worth += 5000;
        }
        if (enemy_troops_worth > 0) {
            //final_worth += enemy_troops_worth;
        }
        if(is_enemy_start) {
            final_worth += (1000000.0 / (kurt->Observation()->GetGameLoop()));
        }
        
        return final_worth * seen_ago_factor*seen_game_steps_ago;
    }
};
