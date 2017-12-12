#pragma once

#include <sc2api/sc2_api.h>
#include "kurt.h"

class Squad {
private:
    Kurt* kurt;
    bool isGrouped();
    void groupUp();
    
public:
    Squad(Kurt* parent_kurt);
    sc2::Units members;
    const static int SQUAD_SIZE = 6;
    const static int SQUAD_DISTANCE = 5;
    bool filled_up = false;
    void attackMove(sc2::Point2D point);
};
