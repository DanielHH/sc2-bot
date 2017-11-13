#include <sc2api/sc2_api.h>
#include <list>
using namespace sc2;

class SharedResources {
public:
    std::list<const Unit*> workers;
    std::list<const Unit*> scouts;
    std::list<const Unit*> army;
};
