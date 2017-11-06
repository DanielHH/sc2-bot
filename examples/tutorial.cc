#include "kurt/tutorial_bot.cc"

#include "sc2api/sc2_api.h"

int main(int argc, char* argv[]) {
    sc2::Coordinator coordinator;
    coordinator.LoadSettings(argc, argv);

    Bot bot;
    coordinator.SetParticipants({
            CreateParticipant(sc2::Race::Terran, &bot),
            CreateComputer(sc2::Race::Zerg)
    });

    coordinator.LaunchStarcraft();
    coordinator.StartGame(sc2::kMapBelShirVestigeLE);

    while (coordinator.Update()) {
    }
    return 0;
}
