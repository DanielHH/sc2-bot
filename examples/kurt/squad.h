#pragma once

#include <sc2api/sc2_api.h>
#include "kurt.h"

class Squad {
private:
    sc2::Units members;
    Kurt* kurt;
    bool isGrouped();
    void groupUp();
    
public:
    Squad(sc2::Units units, Kurt* parent_kurt);
    void attackMove(sc2::Point2D point);
};
