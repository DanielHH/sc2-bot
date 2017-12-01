#if (defined(_WIN64)||defined(__APPLE__))
int main(void) { return 1; }
#endif

void Initialize(const char* title, int x, int y, int w, int h, unsigned int flags = 0);
void Shutdown();
void Matrix1BPP(const char* bytes, int w_mat, int h_mat, int off_x, int off_y, int px_w, int px_h);
void Matrix8BPPHeightMap(const char* bytes, int w_mat, int h_mat, int off_x, int off_y, int px_w, int px_h);
void Matrix8BPPPlayers(const char* bytes, int w_mat, int h_mat, int off_x, int off_y, int px_w, int px_h);
void ImageRGB(const char* bytes, int width, int height, int off_x, int off_y);
void Render();

#include "SDL.h"

#include <cassert>
#include <iostream>

namespace {
    SDL_Window* window_;
    SDL_Renderer* renderer_;

    SDL_Rect CreateRect(int x, int y, int w, int h) {
        SDL_Rect r;
        r.x = x;
        r.y = y;
        r.w = w;
        r.h = h;
        return r;
    }
}

void Initialize(const char* title, int x, int y, int w, int h, unsigned int flags) {
    int init_result = SDL_Init(SDL_INIT_VIDEO);
    assert(!init_result);

    window_ = SDL_CreateWindow(title, x, y, w, h, flags == 0 ? SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE : flags);
    assert(window_);

    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
    assert(renderer_);

    // Clear window to black.
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
    SDL_RenderClear(renderer_);
    SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);
}

void Shutdown() {
    SDL_DestroyRenderer(renderer_);
    SDL_DestroyWindow(window_);
    SDL_Quit();
}

void Matrix8BPPHeightMap(const char* bytes, int w_mat, int h_mat, int off_x, int off_y, int px_w, int px_h) {
    assert(renderer_);
    assert(window_);

    SDL_Rect rect = CreateRect(0, 0, px_w, px_h);
    for (size_t y = 0; y < h_mat; ++y) {
        for (size_t x = 0; x < w_mat; ++x) {
            rect.x = off_x + (int(x) * rect.w);
            rect.y = off_y + (int(h_mat - 1 - y) * rect.h);

            // Renders the height map in grayscale [0-255]
            size_t index = x + y * w_mat;
            SDL_SetRenderDrawColor(renderer_, bytes[index], bytes[index], bytes[index], 255);
            SDL_RenderFillRect(renderer_, &rect);
        }
    }
}

void ImageRGB(const char* bytes, int width, int height, int off_x, int off_y) {
    assert(renderer_);
    assert(window_);

    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom((void*)bytes, width, height, 24, 3 * width, 0xFF0000, 0x00FF00, 0x0000FF, 0x000000);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);
    SDL_FreeSurface(surface);

    SDL_Rect dstRect = CreateRect(off_x, off_y, width, height);
    SDL_RenderCopy(renderer_, texture, NULL, &dstRect);

    SDL_DestroyTexture(texture);
}

void Render() {
    assert(renderer_);
    assert(window_);

    // Clear window to black.
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);

    SDL_RenderPresent(renderer_);
    SDL_RenderClear(renderer_);
}

#include "kurt/kurt.h"

#include "sc2api/sc2_api.h"
#include "sc2utils/sc2_manage_process.h"

//const float kCameraWidth = 44.0f; // default 24.0f
//const int kFeatureLayerSize = 80;
//const int kPixelDrawSize = 4;
//const int kDrawSize = kFeatureLayerSize * kPixelDrawSize;
const int sw = 720;
const int sh = 720;

class RenderAgent : public Kurt {
public:
    virtual void OnGameStart() final {
        Kurt::OnGameStart();
        Initialize("StarCraft 1.5", 550, 0, sw, sh);
    }

    virtual void OnStep() final {
        Kurt::OnStep();

        // Set scale used when render, easier but also less details.
        const sc2::GameInfo& GameInfo = Observation()->GetGameInfo();
        float scale_x = sw / (float) GameInfo.width;
        float scale_y = sh / (float) GameInfo.height;
        SDL_RenderSetScale(renderer_, scale_x, scale_y);

        // Draw height map.
        auto height_map = Observation()->GetRawObservation()->feature_layer_data().minimap_renders().height_map();
        assert(height_map.bits_per_pixel() == 8);
        int hm_width = height_map.size().x();
        int hm_height = height_map.size().y();
        Matrix8BPPHeightMap(height_map.data().c_str(), hm_width, hm_height, 0, 0, 2, 2);

        // Draw fog of war.
//        auto units = Observation()->GetUnits(sc2::Unit::Alliance::Self);
//        SDL_SetRenderDrawColor(renderer_, 10, 10, 50, 255);
//        for (auto unit : units) {
//            int x = unit->pos.x;
//            int y = unit->pos.y;
//            int r = 4;
//            int d = 8;
//            SDL_Rect rect = CreateRect(x - r, y - r, d, d);
//            SDL_RenderFillRect(renderer_, &rect);
//        }

        // Draw all units.
        auto units = Observation()->GetUnits();
        for (auto unit : units) {
            int red[] = {123, 20, 50, 25, 250};
            int green[] = {123, 50, 100, 250, 25};
            int blue[] = {123, 250, 200, 25, 15};
            int al = unit->alliance;
            SDL_SetRenderDrawColor(renderer_, red[al], green[al], blue[al], 180);
            int x = unit->pos.x;
            int y = unit->pos.y;
            int r = unit->radius;
            int d = r <= 0 ? 1 : r * 2;
            SDL_Rect rect = CreateRect(x - r, y - r, d, d);
            SDL_RenderFillRect(renderer_, &rect);
        }

        Render();
    }

    virtual void OnGameEnd() final {
        Kurt::OnGameEnd();
        Shutdown();
    }
};

int main(int argc, char* argv[]) {
    sc2::Coordinator coordinator;
    if (!coordinator.LoadSettings(argc, argv)) {
        return 1;
    }

    const float kCameraWidth = 24.0f;
    const int kFeatureLayerSize = 80;
    sc2::FeatureLayerSettings settings(kCameraWidth, kFeatureLayerSize, kFeatureLayerSize, kFeatureLayerSize, kFeatureLayerSize);
    coordinator.SetFeatureLayers(settings);

    // Add the custom bot, it will control the players.
    RenderAgent bot;

    coordinator.SetParticipants({
        CreateParticipant(sc2::Race::Terran, &bot),
        CreateComputer(sc2::Race::Zerg)
    });

    // Start the game.
    coordinator.LaunchStarcraft();
    coordinator.StartGame(sc2::kMapBelShirVestigeLE);

    SDL_Event event;
    while (coordinator.Update()) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT: {
                    Shutdown();
                    return 0;
                }
                case SDL_KEYDOWN: {
                    break;
                }
                case SDL_KEYUP: {
                    break;
                }
                default: {
                    break;
                }
            }
        }
        if (sc2::PollKeyPress()) {
            break;
        }
    }

    return 0;
}
