#include "kurt.h"

#include <iostream>
#include <list>
#include <algorithm>

#include "army_manager.h"
#include "build_manager.h"
#include "strategy_manager.h"
#include "world_cell.h"

using namespace sc2;

ArmyManager* army_manager;
BuildManager* build_manager;
StrategyManager* strategy_manager;

void Kurt::OnGameStart() {
    const ObservationInterface *observation = Observation();
    SetUpDataMaps(observation);
    army_manager = new ArmyManager(this);
    build_manager = new BuildManager(this);
    build_manager->OnGameStart(Observation());
    strategy_manager = new StrategyManager();
    PopulateWorldRepresentation();
    std::cout << "Width: " << observation->GetGameInfo().pathing_grid.width << std::endl;
    std::cout << "Height: " << observation->GetGameInfo().pathing_grid.height << std::endl;
    std::cout << "world_rep Width " << (world_representation.back()).size() << std::endl;
    std::cout << "world_rep height " << world_representation.size() << std::endl;
    
    
    for (int y = 0; y < world_representation.size(); y++) {
        for (int x = 0; x < world_representation.at(y).size(); x++) {
            std::cout << "Cell X: " << x << ", Y: " << y << std::endl;
            std::cout << "Minerals: " << world_representation.at(y).at(x)->GetMineralAmount() << std::endl;
            std::cout << "Gas: " << world_representation.at(y).at(x)->GetGasAmount() << std::endl;
        }
    }
    
}

void Kurt::OnStep() {
    const ObservationInterface* observation = Observation();
    army_manager->OnStep(observation);
    build_manager->OnStep(observation);
    strategy_manager->OnStep(observation);
}
void Kurt::OnUnitCreated(const Unit* unit) {
    const ObservationInterface* observation = Observation();
    army_manager->GroupNewUnit(unit, observation);
}

void Kurt::OnUnitIdle(const Unit* unit) {
    switch (unit->unit_type.ToType()) {
    case UNIT_TYPEID::TERRAN_SCV: {
        const Unit* mineral_target = FindNearestMineralPatch(unit->pos);
        if (!mineral_target) {
            break;
        }
        Actions()->UnitCommand(unit, ABILITY_ID::SMART, mineral_target);
        break;
    }
    default: {
        break;
    }

    }
}

void Kurt::OnUnitDestroyed(const Unit *destroyed_unit){
    bool found = (std::find(scouts.begin(), scouts.end(), destroyed_unit) != scouts.end());
    if (found) {
        scouts.remove(destroyed_unit);
        return;
    }
    
    found = (std::find(army.begin(), army.end(), destroyed_unit) !=army.end());
    army.remove(destroyed_unit);
}

bool Kurt::TryBuildStructure(ABILITY_ID ability_type_for_structure,
    Point2D location,
    const Unit* unit) {
    const ObservationInterface* observation = Observation();

    // If a unit already is building a supply structure of this type, do nothing.
    // Also get an scv to build the structure.
    std::vector<const Unit*> units_to_build;
    Units units = observation->GetUnits(Unit::Alliance::Self);

    Actions()->UnitCommand(unit,
        ability_type_for_structure,
        location);
    return true;
}


const Unit* Kurt::getUnitOfType(UNIT_TYPEID unit_typeid) {
    const ObservationInterface* observation = Observation();
    Units units = observation->GetUnits(Unit::Alliance::Self);
    return units.front();
}


Point2D Kurt::randomLocationNearUnit(const Unit* unit) {
    float rx = GetRandomScalar();
    float ry = GetRandomScalar();
    Point2D location = Point2D(unit->pos.x + rx * 15.0f, unit->pos.y + ry * 15.0f);
    return location;

}


bool Kurt::TryBuildSupplyDepot() {
    const ObservationInterface* observation = Observation();

    // If we are not supply capped, don't build a supply depot.
    if (observation->GetFoodUsed() <= observation->GetFoodCap() - 2) {
        return false;
    }
    // Try and build a depot. Find a random SCV and give it the order.
    const Unit* unit = getUnitOfType(UNIT_TYPEID::TERRAN_SCV);
    return TryBuildStructure(ABILITY_ID::BUILD_SUPPLYDEPOT, randomLocationNearUnit(unit), unit);
}

bool Kurt::TryBuildRefinary() {
    const ObservationInterface* observation = Observation();

    // As soon as we have enough gold we build a refinary
    if (observation->GetMinerals() < 100) {
        return false;
    }
    const Unit* vespene_target = FindNearestVespeneGeyser();
    // Try and build a depot. Find a random SCV and give it the order.

    // rmove false
    return false;

}

const Unit* Kurt::FindNearestMineralPatch(const Point2D& start) {
    Units units = Observation()->GetUnits(Unit::Alliance::Neutral);
    float distance = std::numeric_limits<float>::max();
    const Unit* target = nullptr;
    for (const auto& u : units) {
        if (u->unit_type == UNIT_TYPEID::NEUTRAL_MINERALFIELD) {
            float d = DistanceSquared2D(u->pos, start);
            if (d < distance) {
                distance = d;
                target = u;
            }
        }
    }
    return target;
}

const Unit* Kurt::FindNearestVespeneGeyser() {
    Units units = Observation()->GetUnits(Unit::Alliance::Neutral);
    Units allied_units = Observation()->GetUnits(Unit::Alliance::Ally);
    float distance = std::numeric_limits<float>::max();
    const Unit* command_center = nullptr;
    const Unit* vespene_geyser = nullptr;
    // Look for a command center among allied units
    for (const auto& ally : allied_units) {
        if (ally->unit_type == UNIT_TYPEID::TERRAN_COMMANDCENTER) {
            command_center = ally;
            // look for closest free vespene gayser of that command center
            for (const auto& unit : units) {
                if (unit->build_progress == 0) {
                    float d = DistanceSquared2D(unit->pos, command_center->pos);
                    if (d < distance) {
                        distance = d;
                        vespene_geyser = unit;
                    }
                }
            }
        }
        // return if we have found a suitable vespene geyser close to a command center
        if (command_center && vespene_geyser) {
            return vespene_geyser;
        }
    }
    //        std::cout << "NO VESPENE FOUND" << std::endl;
    return nullptr;
}

void Kurt::PopulateWorldRepresentation(){
    ImageData actual_world = Observation()->GetGameInfo().pathing_grid;
    
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
                (world_representation.back()).push_back(new WorldCell(x,y));
            } else if (x == actual_world.width && rest_width > 0 && y % chunk_size == 0) {
                (world_representation.back()).push_back(new WorldCell(x, y));
            } else if (x % chunk_size == 0 && y == actual_world.height && rest_height > 0) {
                (world_representation.back()).push_back(new WorldCell(x, y));
            } else if (x == actual_world.width && rest_width > 0 && y == actual_world.height && rest_height > 0) {
                (world_representation.back()).push_back(new WorldCell(x, y));
            }
        }
    }
    
    // put in all neutral units
    for (const Unit* neutral_unit: Observation()->GetUnits(Unit::Alliance::Neutral)) {
        int x_pos = neutral_unit->pos.x / chunk_size;
        int y_pos  = neutral_unit->pos.y / chunk_size;
        float mineral_amount = world_representation.at(y_pos).at(x_pos)->GetMineralAmount();
        float gas_amount = world_representation.at(y_pos).at(x_pos)->GetGasAmount();
        
        /*
        std::cout << "x: " << neutral_unit->pos.x << " y: " << neutral_unit->pos.y << std::endl;
        std::cout << "X_pos: " << x_pos << " Y_pos: " << y_pos << std::endl;
        */
        
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
                std::cout << "Unknown type: " << neutral_unit->unit_type << std::endl;
                break;
        }
    }
}

std::map<sc2::UNIT_TYPEID, sc2::UnitTypeData> Kurt::unit_types;
std::map<sc2::ABILITY_ID, sc2::AbilityData> Kurt::abilities;
void Kurt::SetUpDataMaps(const sc2::ObservationInterface *observation) {
    for (auto unit : observation->GetUnitTypeData()) {
        unit_types[(sc2::UNIT_TYPEID) unit.unit_type_id] = unit;
    }
    for (sc2::AbilityData ability : observation->GetAbilityData()) {
        abilities[(sc2::ABILITY_ID) ability.ability_id] = ability;
    }
}

AbilityData *Kurt::GetAbility(ABILITY_ID id) {
    return &abilities.at(id);
}

UnitTypeData *Kurt::GetUnitType(UNIT_TYPEID id) {
    return &unit_types.at(id);
}
