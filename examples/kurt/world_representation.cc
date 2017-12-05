#include "world_representation.h"

using namespace sc2;

WorldRepresentation::WorldRepresentation(Kurt* parent_kurt) {
    kurt = parent_kurt;
    ImageData actual_world = kurt->Observation()->GetGameInfo().pathing_grid;
    int rest_height = actual_world.height % chunk_size;
    int rest_width = actual_world.width % chunk_size;
    
    // add cells
    for (int y = 1; y <= actual_world.height; ++y) {
        if (y % chunk_size == 0) {
            world_representation.push_back(std::vector<WorldCell*>());
        } else if (y == actual_world.height && rest_height > 0) {
            world_representation.push_back(std::vector<WorldCell*>());
        }
        for (int x = 1; x <= actual_world.width; ++x) {
            if (x % chunk_size == 0 && y % chunk_size == 0) {
                (world_representation.back()).push_back(new WorldCell(x,y,kurt));
            } else if (x == actual_world.width && rest_width > 0 && y % chunk_size == 0) {
                (world_representation.back()).push_back(new WorldCell(x, y, kurt));
            } else if (x % chunk_size == 0 && y == actual_world.height && rest_height > 0) {
                (world_representation.back()).push_back(new WorldCell(x, y, kurt));
            } else if (x == actual_world.width && rest_width > 0 && y == actual_world.height && rest_height > 0) {
                (world_representation.back()).push_back(new WorldCell(x, y, kurt));
            }
        }
    }
}

void WorldRepresentation::UpdateWorldRep() {
    sc2::Units observed_enemy_units = kurt->Observation()->GetUnits(Unit::Alliance::Enemy);
    sc2::Units observed_allied_units = kurt->Observation()->GetUnits(Unit::Alliance::Self);
    for (int y  = 0; y < world_representation.size(); ++y) {
        for (int x = 0; x < world_representation.at(y).size(); ++x) {
            WorldCell* cell = world_representation.at(y).at(x);
            world_representation.at(y).at(x)->SetMineralAmount(0);
            world_representation.at(y).at(x)->SetGasAmount(0);
            cell->ClearBuildings();
            cell->ClearTroops();
        }
    }
    for (const Unit* enemy : observed_enemy_units) {
        int unit_world_rep_x_pos = (enemy->pos.x) / chunk_size;
        int unit_world_rep_y_pos = (enemy->pos.y) / chunk_size;
        WorldCell* cell = world_representation.at(unit_world_rep_y_pos).at(unit_world_rep_x_pos);
        if (kurt->IsStructure(enemy)) {
            cell->AddBuilding(enemy);
        } else if(kurt->IsArmyUnit(enemy)) {
            cell->AddTrooper(enemy);
        }
        /*
        if(enemy->display_type == Unit::Visible) {
            cell->SetSeenOnGameStep(kurt->Observation()->GetGameLoop());
            if (kurt->IsStructure(enemy)) {
                cell->AddBuilding(enemy);
            } else if(kurt->IsArmyUnit(enemy)) {
                cell->AddTrooper(enemy);
            }
        } else if (enemy->display_type == Unit::Snapshot) {
            if (kurt->IsStructure(enemy)) {
                // add building hidden by fog of war
                cell->AddBuilding(enemy);
            }
        }*/
    }
    for (const Unit* ally : observed_allied_units){
        int ally_cell_x_pos = ally->pos.x / chunk_size;
        int ally_cell_y_pos = ally->pos.y / chunk_size;
        WorldCell* cell = kurt->world_rep->world_representation.at(ally_cell_y_pos).at(ally_cell_x_pos);
        cell->SetSeenOnGameStep(kurt->Observation()->GetGameLoop());
    }
    PopulateNeutralUnits();
}

void WorldRepresentation::PopulateNeutralUnits() {
    // put in all neutral units
    for (const Unit* neutral_unit: kurt->Observation()->GetUnits(Unit::Alliance::Neutral)) {
        int x_pos = neutral_unit->pos.x / chunk_size;
        int y_pos  = neutral_unit->pos.y / chunk_size;
        float mineral_amount = world_representation.at(y_pos).at(x_pos)->GetMineralAmount();
        float gas_amount = world_representation.at(y_pos).at(x_pos)->GetGasAmount();
        
        switch (neutral_unit->unit_type.ToType()) {
            case UNIT_TYPEID::NEUTRAL_VESPENEGEYSER:
                if (neutral_unit->vespene_contents > 0) {
                    world_representation.at(y_pos).at(x_pos)->SetGasAmount(neutral_unit->vespene_contents + gas_amount);
                } else { // cant see. maybe set to an approximation or inf ?
                    world_representation.at(y_pos).at(x_pos)->SetGasAmount(2000 + gas_amount);
                }
                break;
            case UNIT_TYPEID::NEUTRAL_MINERALFIELD:
            case UNIT_TYPEID::NEUTRAL_MINERALFIELD750:
            case UNIT_TYPEID::NEUTRAL_RICHMINERALFIELD:
            case UNIT_TYPEID::NEUTRAL_RICHMINERALFIELD750:
                if (neutral_unit->mineral_contents > 0) {
                    world_representation.at(y_pos).at(x_pos)->SetMineralAmount(neutral_unit->mineral_contents + mineral_amount);
                } else { // cant see. maybe set to an approximation or inf ?
                    world_representation.at(y_pos).at(x_pos)->SetMineralAmount(1200 + mineral_amount);
                }
                break;
            default:
                // what todo with these?
                //std::cout << "Unknown type: " << neutral_unit->unit_type << std::endl;
                break;
        }
    }
}

void WorldRepresentation::PrintWorld() {
    /*std::cout << "world_rep Width " << (world_representation.back()).size() << std::endl;
    std::cout << "world_rep height " << world_representation.size() << std::endl;
    for (int y = 0; y < world_representation.size(); y++) {
        for (int x = 0; x < world_representation.at(y).size(); x++) {
            std::cout << "Cell X: " << x << ", Y: " << y << std::endl;
            std::cout << "Minerals: " << world_representation.at(y).at(x)->GetMineralAmount() << std::endl;
            std::cout << "Gas: " << world_representation.at(y).at(x)->GetGasAmount() << std::endl;
        }
    }*/
}
