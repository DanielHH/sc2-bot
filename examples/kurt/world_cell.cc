#include "world_cell.h"

float WorldCell::GetRelativeStrength(std::vector<const sc2::Unit *> allied_troops) {
    for (const sc2::Unit* allied_troop : allied_troops) {
        
        for (const sc2::Unit* enemy_troop: GetTroops()) {
            
        }
    }
}

std::vector<const sc2::Unit*>WorldCell::GetTroops(){return troops;}
float WorldCell::GetMineralAmount() {return mineral_amount;}
float WorldCell::GetGasAmount() {return gas_amount;}
float WorldCell::GetEnemyDps() {return enemy_dps;}
float WorldCell::GetLastSeen() {return last_seen;}

void WorldCell::SetMineralAmount(float amount) {mineral_amount = amount;}
void WorldCell::SetGasAmount(float amount) {gas_amount = amount;}
void WorldCell::SetEnemyDps(float dps) {enemy_dps = dps;}
void WorldCell::SetLastSeen(float time) {last_seen = time;}
