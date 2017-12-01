#pragma once

#include <sc2api/sc2_api.h>
#include "kurt.h"

class WorldCell {
    
private:
    int real_world_x;
    int real_world_y;
    Kurt* kurt;
    const sc2::Unit* scout;
    float mineral_amount;
    float gas_amount;
    float enemy_dps;
    float seen_on_game_step;
    sc2::Units buildings;
    sc2::Units troops;
    float UnitDamageVSSquad(const sc2::Unit* unit, sc2::Units units, Kurt* kurt);
    
public:
    WorldCell(int x, int y, Kurt* kurt);
    float GetMineralAmount();
    float GetGasAmount();
    float GetEnemyDps();
    float GetSeenOnGameStep();
    float GetScoutPriority();
    sc2::Point2D GetCellLocationAs2DPoint(int chunk_size);
    sc2::Units GetTroops();
    float GetRelativeStrength(sc2::Units allied_troops, Kurt* kurt);
    const sc2::Unit* GetScout();
    
    
    void SetMineralAmount(float amount);
    void SetGasAmount(float amount);
    void SetEnemyDps(float dps);
    void SetSeenOnGameStep(float game_step);
    const sc2::Unit* SetScout(const sc2::Unit* unit);
    bool operator < (WorldCell* rhs);
    bool SmartComp(WorldCell* a, WorldCell* b);
    void AddBuilding(const sc2::Unit* building);
    void AddTrooper(const sc2::Unit* trooper);
};
