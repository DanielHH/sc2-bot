#include "kurt/kurt.cc"
#include <sc2api/sc2_api.h>

using namespace sc2;

int main(int argc, char* argv[]) {
    Coordinator coordinator;
    coordinator.LoadSettings(argc, argv);

    Kurt bot;
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
