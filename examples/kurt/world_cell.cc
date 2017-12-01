#include "world_cell.h"

WorldCell::WorldCell(int x, int y, Kurt* parent_kurt){
    real_world_x = x;
    real_world_y = y;
    gas_amount = 0;
    mineral_amount = 0;
    seen_on_game_step = 0;
    kurt = parent_kurt;
}

float WorldCell::GetRelativeStrength(sc2::Units allied_troops, Kurt* kurt) {
    float total_enemy_dmg = 0;
    float total_allied_dmg = 1; // init to 1 to avoid division by zero
    for (const sc2::Unit* enemy_trooper: GetTroops()) {
        total_enemy_dmg += UnitDamageVSSquad(enemy_trooper, allied_troops, kurt);
    }
    for (const sc2::Unit* allied_trooper : allied_troops) {
        total_allied_dmg += UnitDamageVSSquad(allied_trooper, GetTroops(), kurt);
    }
    return total_enemy_dmg / total_allied_dmg;
}


float WorldCell::UnitDamageVSSquad(const sc2::Unit* unit, sc2::Units units, Kurt* kurt) {
    float total_unit_dmg = 0;
    sc2::UnitTypeData* unit_type_data = kurt->GetUnitType(unit->unit_type);
    std::vector<sc2::Weapon> weapons = unit_type_data->weapons;
    for (auto weapon : unit_type_data->weapons) {
        float unit_dmg = weapon.damage_ / weapon.speed; // This is correct assuming damage_ == damage_ per attack
        for (const sc2::Unit* enemy_trooper : units) {
            if (weapon.type == sc2::Weapon::TargetType::Any) {
                //To Air And Ground
                total_unit_dmg += unit_dmg;
            }
            else if (weapon.type == sc2::Weapon::TargetType::Ground) {
                //To Ground
                if (!enemy_trooper->is_flying) {
                    total_unit_dmg += unit_dmg;
                }
            }
            else if (weapon.type == sc2::Weapon::TargetType::Air) {
                //To Air
                if (enemy_trooper->is_flying) {
                    total_unit_dmg += unit_dmg;
                }
            }
        }
    }
    return total_unit_dmg / units.size();
}

bool WorldCell::SmartComp(WorldCell* a, WorldCell* b) {
    float a_gas = a->GetGasAmount();
    float a_mineral = a->GetMineralAmount();
    float a_seen_game_steps_ago = kurt->Observation()->GetGameLoop() - a->GetSeenOnGameStep();
    
    float b_gas = b->GetGasAmount();
    float b_mineral = b->GetMineralAmount();
    float b_seen_game_steps_ago = kurt->Observation()->GetGameLoop() - b->GetSeenOnGameStep();
    
    return (a_gas + a_mineral) + a_seen_game_steps_ago < (b_gas + b_mineral) + b_seen_game_steps_ago;
}

bool WorldCell::operator < (WorldCell* rhs) {
    return SmartComp(this, rhs);
}

sc2::Point2D WorldCell::GetCellLocationAs2DPoint(int chunk_size) {
    return sc2::Point2D(real_world_x-chunk_size/2,real_world_y-chunk_size/2);
}

sc2::Units WorldCell::GetTroops(){return troops;}
float WorldCell::GetMineralAmount() {return mineral_amount;}
float WorldCell::GetGasAmount() {return gas_amount;}
float WorldCell::GetEnemyDps() {return enemy_dps;}
float WorldCell::GetSeenOnGameStep() {return seen_on_game_step;}
const sc2::Unit* WorldCell::GetScout() {
    return scout;
}

void WorldCell::SetMineralAmount(float amount) {mineral_amount = amount;}
void WorldCell::SetGasAmount(float amount) {gas_amount = amount;}
void WorldCell::SetEnemyDps(float dps) {enemy_dps = dps;}
void WorldCell::SetSeenOnGameStep(float step) {seen_on_game_step = step;}
const sc2::Unit* WorldCell::SetScout(const sc2::Unit* unit) {
    scout = unit;
}
