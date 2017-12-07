#pragma once

#include <sc2api/sc2_api.h>

class Squad {
private:
    sc2::Units members;
    void groupUp() {
    }
public:
    void attackMove(sc2::Point2D point);
};
