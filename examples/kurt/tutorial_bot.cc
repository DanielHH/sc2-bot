#include <sc2api/sc2_api.h>
#include <iostream>
#include "army_manager.cc"
#include "build_manager.cc"
#include "strategy_manager.cc"
using namespace sc2;

struct IsVespeneGeyser {
    bool operator()(const Unit& unit) {
        switch (unit.unit_type.ToType()) {
            case UNIT_TYPEID::NEUTRAL_VESPENEGEYSER: return true;
            case UNIT_TYPEID::NEUTRAL_SPACEPLATFORMGEYSER: return true;
            case UNIT_TYPEID::NEUTRAL_PROTOSSVESPENEGEYSER: return true;
            default: return false;
        }
    }
};

class Bot : public Agent {
public:
    ArmyManager* army_manager;
    BuildManager* build_manager;
    StrategyManager* strategy_manager;
    const ObservationInterface* observation_interface;
    virtual void OnGameStart() {
        std::cout << "Hello, World!" << std::endl;
        observation_interface = Observation();
        army_manager = new ArmyManager();
        build_manager = new BuildManager();
        strategy_manager = new StrategyManager();
    }
    
    virtual void OnStep() {
        observation_interface = Observation();
        army_manager->OnStep(observation_interface);
        build_manager->OnStep(observation_interface);
        strategy_manager->OnStep(observation_interface);
        TryBuildSupplyDepot();
        TryBuildRefinary();
    }
    virtual void OnUnitCreated(const Unit* unit) {
        std::cout << unit->tag << std::endl;
    }
    
    virtual void OnUnitIdle(const Unit* unit) {
        switch(unit->unit_type.ToType()) {
            case UNIT_TYPEID::TERRAN_COMMANDCENTER: {
                Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_SCV);
                break;
            }
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
    
    bool TryBuildStructure(ABILITY_ID ability_type_for_structure,
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
    
    
    const Unit* getUnitOfType(UNIT_TYPEID unit_typeid) {
        const ObservationInterface* observation = Observation();
        Units units = observation->GetUnits(Unit::Alliance::Self);
        return units.front();
    }
    
    
    static Point2D randomLocationNearUnit(const Unit* unit) {
        float rx = GetRandomScalar();
        float ry = GetRandomScalar();
        Point2D location = Point2D(unit->pos.x + rx * 15.0f, unit->pos.y + ry * 15.0f);
        return location;
        
    }
    
    
    bool TryBuildSupplyDepot() {
        const ObservationInterface* observation = Observation();
        
        // If we are not supply capped, don't build a supply depot.
        if (observation->GetFoodUsed() <= observation->GetFoodCap() - 2) {
            return false;
        }
        // Try and build a depot. Find a random SCV and give it the order.
        const Unit* unit = getUnitOfType(UNIT_TYPEID::TERRAN_SCV);
        return TryBuildStructure(ABILITY_ID::BUILD_SUPPLYDEPOT, randomLocationNearUnit(unit), unit);
    }
    
    bool TryBuildRefinary() {
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
    
    const Unit* FindNearestMineralPatch(const Point2D& start) {
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
    
    const Unit* FindNearestVespeneGeyser() {
        Units units = Observation()->GetUnits(Unit::Alliance::Neutral);
        Units allied_units = Observation()->GetUnits(Unit::Alliance::Ally);
        float distance = std::numeric_limits<float>::max();
        const Unit* command_center = nullptr;
        const Unit* vespene_geyser = nullptr;
        // Look for a command center among allied units
        for (const auto& ally : allied_units) {
            if (ally->unit_type == UNIT_TYPEID::TERRAN_COMMANDCENTER){
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
            if(command_center && vespene_geyser) {
                return vespene_geyser;
            }
        }
        std::cout << "NO VESPENE FOUND" << std::endl;
        return nullptr;
    }
};
