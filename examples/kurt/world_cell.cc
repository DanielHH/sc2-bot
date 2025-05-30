#include "world_cell.h"
#include <iostream>

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

sc2::Point2D WorldCell::GetCellLocationAs2DPoint(int chunk_size) {
    return sc2::Point2D(real_world_x-chunk_size/2,real_world_y-chunk_size/2);
}

void WorldCell::AddBuilding(const sc2::Unit* building) {
    if (std::find(buildings.begin(), buildings.end(), building) == buildings.end()) {
        buildings.push_back(building);
    }
}

void WorldCell::AddAlliedBuilding(const sc2::Unit* building) {
    if (std::find(buildings.begin(), buildings.end(), building) == buildings.end()) {
        allied_buildings.push_back(building);
    }
}

void WorldCell::AddTrooper(const sc2::Unit* trooper) {
    if (std::find(troops.begin(), troops.end(), trooper) == troops.end()) {
        troops.push_back(trooper);
    }
}

void WorldCell::AddAlliedTrooper(const sc2::Unit* trooper) {
    if (std::find(troops.begin(), troops.end(), trooper) == troops.end()) {
        allied_troops.push_back(trooper);
    }
}

void WorldCell::ClearBuildings() {
    buildings.clear();
}

void WorldCell::ClearTroops() {
    troops.clear();
}

void WorldCell::ClearAlliedBuildings() {
    allied_buildings.clear();
}

void WorldCell::ClearAlliedTroops() {
    allied_troops.clear();
}
sc2::Units WorldCell::GetTroops(){return troops;}
sc2::Units WorldCell::GetBuildings(){return buildings;}
sc2::Units WorldCell::GetAlliedTroops(){return allied_troops;}
sc2::Units WorldCell::GetAlliedBuildings(){return allied_buildings;}
float WorldCell::GetMineralAmount() {return mineral_amount;}
float WorldCell::GetGasAmount() {return gas_amount;}
float WorldCell::GetEnemyDps() {return enemy_dps;}
float WorldCell::GetSeenOnGameStep() {return seen_on_game_step;}
int WorldCell::GetCellRealX(){return real_world_x;}
int WorldCell::GetCellRealY(){return real_world_y;}
const sc2::Unit* WorldCell::GetScout() {
    return scout;
}

void WorldCell::SetMineralAmount(float amount) {mineral_amount = amount;}
void WorldCell::SetGasAmount(float amount) {gas_amount = amount;}
void WorldCell::SetEnemyDps(float dps) {enemy_dps = dps;}
void WorldCell::SetSeenOnGameStep(float step) {seen_on_game_step = step;}
