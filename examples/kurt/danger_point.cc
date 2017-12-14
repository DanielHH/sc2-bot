#include <sc2api/sc2_api.h>

struct DangerPoint {
private:
    sc2::Point2D point;
    int last_game_step_seen;
    
public:
    DangerPoint(sc2::Point2D m_point, int seen_at) {
        point = m_point;
        last_game_step_seen = seen_at;
    }
    
    int SeenGameStepsAgo(int current_game_step) {
        return current_game_step-last_game_step_seen;
    }
    
    sc2::Point2D GetPoint() {
        return point;
    }
    int GetLastGameStepSeen() {
        return last_game_step_seen;
    }
};
