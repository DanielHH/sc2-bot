#include "kurt.h"
#include <iostream>
#include "world_cell.h"

struct CellComp {
    Kurt* kurt;
    
    CellComp(Kurt* parent_kurt) {
        kurt = parent_kurt;
    }
    
    bool SmartComp(WorldCell* a, WorldCell* b) {
        float a_gas = a->GetGasAmount();
        float a_mineral = a->GetMineralAmount();
        float a_seen_game_steps_ago = kurt->Observation()->GetGameLoop() - a->GetSeenOnGameStep();
        
        float b_gas = b->GetGasAmount();
        float b_mineral = b->GetMineralAmount();
        float b_seen_game_steps_ago = kurt->Observation()->GetGameLoop() - b->GetSeenOnGameStep();
        
        return (a_gas + a_mineral) + a_seen_game_steps_ago > (b_gas + b_mineral) + b_seen_game_steps_ago;
    }
    
    bool operator()(WorldCell* lhs, WorldCell* rhs) {
        return SmartComp(lhs, rhs);
    }
};
