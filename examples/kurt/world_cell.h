#pragma once

#include <sc2api/sc2_api.h>

class WorldCell {
    
private:
    
    float mineral_amount;
    float gas_amount;
    float enemy_dps;
    float last_seen;
    std::vector<const sc2::Unit*> buildings;
    std::vector<const sc2::Unit*> troops;
    
    
public:
        
    float GetMineralAmount();
    float GetGasAmount();
    float GetEnemyDps();
    float GetLastSeen();
    float GetScoutPriority();
    std::vector<const sc2::Unit*> GetTroops();
    float GetRelativeStrength(std::vector<const sc2::Unit*> allied_troops);
    
    void SetMineralAmount(float amount);
    void SetGasAmount(float amount);
    void SetEnemyDps(float dps);
    void SetLastSeen(float time);
};
