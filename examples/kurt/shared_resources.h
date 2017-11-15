#pragma once

#include <sc2api/sc2_api.h>
#include <list>

class SharedResources {
public:
    std::list<const sc2::Unit*> workers;
    std::list<const sc2::Unit*> scouts;
    std::list<const sc2::Unit*> army;
};