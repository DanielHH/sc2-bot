#pragma once

#include <sc2api/sc2_api.h>
#include "kurt.h"

class WorldCell {
    
private:
    
    float mineral_amount;
    float gas_amount;
    float enemy_dps;
    float last_seen;
    sc2::Units buildings;
    sc2::Units troops;
    float UnitDamageVSSquad(const sc2::Unit* unit, sc2::Units units, Kurt* kurt);
    
public:
        
    float GetMineralAmount();
    float GetGasAmount();
    float GetEnemyDps();
    float GetLastSeen();
    float GetScoutPriority();
    sc2::Units GetTroops();
    float GetRelativeStrength(sc2::Units allied_troops, Kurt* kurt);
    
    void SetMineralAmount(float amount);
    void SetGasAmount(float amount);
    void SetEnemyDps(float dps);
    void SetLastSeen(float time);
};
