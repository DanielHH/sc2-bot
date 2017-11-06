#include <sc2api/sc2_api.h>
#include <iostream>

using namespace sc2;

class Bot : public Agent {
public:
    virtual void OnGameStart() final {
        std::cout << "Hello, World!" << std::endl;
    }
    
    virtual void OnStep() final {
        /* TUTORIAL 1
         int gameLoop = Observation()->GetGameLoop();
         if (gameLoop%20 == 0) {
         std::cout << gameLoop << std::endl;
         std::cout << Observation()->GetVespene() << std::endl;
         std::cout << Observation()->GetMinerals() << std::endl;
         }*/
        
        /* TUTORIAL 2 */
        TryBuildSupplyDepot();
    }
    virtual void OnUnitCreated(const Unit* unit) final {
        std::cout << unit->tag << std::endl;
    }
    
    virtual void OnUnitIdle(const Unit* unit) final {
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
    
    bool TryBuildStructure(ABILITY_ID ability_type_for_structure, UNIT_TYPEID unit_type = UNIT_TYPEID::TERRAN_SCV) {
        const ObservationInterface* observation = Observation();
        
        // If a unit already is building a supply structure of this type, do nothing.
        // Also get an scv to build the structure.
        std::vector<const Unit*> units_to_build;
        
        const Unit* unit_to_build = nullptr;
        Units units = observation->GetUnits(Unit::Alliance::Self);
        for (const auto& unit : units) {
            for (const auto& order : unit->orders) {
                if (order.ability_id != ability_type_for_structure  &&
                    unit->unit_type == unit_type                    &&
                    units_to_build.size() < 3) {
                    units_to_build.push_back(unit);
                } else {
                    break;
                }
            }
            if (units_to_build.size() > 2) {
                break;
            }
        }
        
        for (int i  = 0; i < units_to_build.size(); i++) {
            float rx = GetRandomScalar();
            float ry = GetRandomScalar();
            unit_to_build = units_to_build[i];
            Actions()->UnitCommand(unit_to_build,
                                   ability_type_for_structure,
                                   Point2D(unit_to_build->pos.x + rx * 15.0f, unit_to_build->pos.y + ry * 15.0f));
            
        }
        
        return true;
    }
    
    bool TryBuildSupplyDepot() {
        const ObservationInterface* observation = Observation();
        
        // If we are not supply capped, don't build a supply depot.
        if (observation->GetFoodUsed() <= observation->GetFoodCap() - 2) {
            return false;
        }
        
        // Try and build a depot. Find a random SCV and give it the order.
        return TryBuildStructure(ABILITY_ID::BUILD_SUPPLYDEPOT);
    }
    
    const Unit* FindNearestMineralPatch(const Point2D& start){
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
};
