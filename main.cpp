#include <SDL2/SDL.h> 
#include <iostream>
#include "constants.h"
#include <math.h>

struct point {
    float x;
    float y;
    float z;
};

struct flying_object {
    point points[points_per_object];
    float azimuth; 
    float inclination;
    float velocity;
};

struct movement_charcteristics {
    float azimuth;
    float inclination;
    float velocity;
};

point to_cartesian(movement_charcteristics* angular) {
    point out;
    out.x = angular->velocity * sin(angular->azimuth) * cos(angular->inclination);
    out.y = angular->velocity * sin(angular->azimuth) * sin(angular->inclination);
    out.z = angular->velocity * sin(angular->inclination);
    return out;
};

float sign(float a) {
    return a > 0 ? 1.0 : a < 0 ? -1.0 : 0.0;
};

int max(int a, int b) {
    return a > b ? a : b;
};

movement_charcteristics to_polar(point* cartesian) {
    movement_charcteristics out;
    float r = sqrt(cartesian->x * cartesian->x + cartesian->y * cartesian->y + cartesian->z * cartesian->z);
    out.velocity = r;
    out.inclination = r == 0 ? 0 : acos(cartesian->z/r);
    out.azimuth = (cartesian->x == 0 && cartesian->y == 0) ? 0 : sign(cartesian->y) * 
        acos(cartesian->x / sqrt(cartesian->x * cartesian->x + cartesian->y * cartesian->y));
    return out;
};

SDL_Vertex to_renderable(movement_charcteristics* angles, int h, int w) {
    if (abs(angles->azimuth) >= PI / 2 || abs(angles->inclination) >= PI / 2) {
        SDL_Vertex nothing_point = {
            position: {
                x: static_cast<float>(w + 1),
                y: static_cast<float>(h + 1)
            },
            color: {0, 0, 0, 0}
        };
        return nothing_point;
    };
    float scale = lens_modifier * static_cast<float>(max(h, w));
    SDL_Vertex out = {
        position: {
            x: (scale * sin(angles->azimuth) / sin((PI / 2) - angles->azimuth)) + static_cast<float>(w / 2),
            y: (scale * sin(angles->inclination) / sin((PI / 2) - angles->inclination)) + static_cast<float>(h / 2)
        },
        color: {
            r: 255,
            g: 0,
            b : 255,
            a : 255
        }
    };
    return out;
};

class gameState {
    public:
        float player_x; 
        float player_y;
        float player_z;
        bool accelerating;
        bool turning_left;
        bool turning_right;
        bool turning_up;
        bool turning_down;
        void render(SDL_Renderer* renderer, SDL_Window* window) {
            int h;
            int w;
            SDL_GetWindowSize(window, &w, &h);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            for (int i = 0; i < this->object_count; i++) {
                SDL_Vertex vertices[points_per_object];
                for (int j = 0; j < points_per_object; j++) {
                    point separation_vector = {
                        x: this->objects[i].points[j].x - this->player_x,
                        y: this->objects[i].points[j].y - this->player_y,
                        z: this->objects[i].points[j].z - this->player_z
                    };
                    movement_charcteristics polar = to_polar(&separation_vector);
                    vertices[j] = to_renderable(&polar, h, w);
                };
                SDL_RenderGeometry(renderer, NULL, vertices, points_per_object, NULL, 0);
            };
            SDL_RenderPresent(renderer);
        };
        void update(float delay) {
            if (this->turning_down || this->turning_left || this->turning_right || this->turning_up) {

            };
            for (int i = 0; i < this->object_count; i++) {};
        };
        void initialise() {
            this->player_x = 0;
            this->player_y = 0;
            this->player_z = 0;
            this->player_direction_azimuth = 0;
            this->player_direction_inclination = 0;
            this->player_velocity = 0;
            this->object_count = 1;
            this->objects[0] = {
                points: {{1.0, 0.5, 6.0}, {2.0, 1.5, 12.0}, {1.5, 0.0, 9.0}},
                azimuth: 0.0,
                inclination: 0.0,
                velocity: 0.0
            };
            this->accelerating = false;
            this->turning_down = false;
            this->turning_up = false;
            this->turning_left = false;
            this->turning_right = false;
        };
    float player_direction_azimuth;
    float player_direction_inclination;
    float player_velocity;
    flying_object objects[scenario_max_objects];
    int object_count;
};

void handle_event(gameState *state) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                exit(0);
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.scancode) {
                    case SDL_SCANCODE_SPACE:
                        state->accelerating = true;
                        break;
                    case SDL_SCANCODE_UP:
                        state->turning_up = true;
                        break;
                    case SDL_SCANCODE_DOWN:
                        state->turning_down = true;
                        break;
                    case SDL_SCANCODE_LEFT:
                        state->turning_left = true;
                        break;
                    case SDL_SCANCODE_RIGHT:
                        state->turning_right = true;
                        break;
                    default:
                        break;
                };
                break;
            case SDL_KEYUP:
                switch (event.key.keysym.scancode) {
                    case SDL_SCANCODE_SPACE:
                        state->accelerating = false;
                        break;
                    case SDL_SCANCODE_UP:
                        state->turning_up = false;
                        break;
                    case SDL_SCANCODE_DOWN:
                        state->turning_down = false;
                        break;
                    case SDL_SCANCODE_LEFT:
                        state->turning_left = false;
                        break;
                    case SDL_SCANCODE_RIGHT:
                        state->turning_right = false;
                        break;
                    default:
                        break;
                };
                break;
            default:
                break;
        };
    };
};

int main(int argc, char *argv[]) {  
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {  
        printf("error initializing SDL: %s\n", SDL_GetError());  
    };
    SDL_DisplayMode DisplayMode;
    SDL_GetCurrentDisplayMode(0, &DisplayMode);
    SDL_Window* window = SDL_CreateWindow("Normallight",  
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        DisplayMode.w, DisplayMode.h, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED); 
    gameState state;
    state.initialise();
    while (1) {
        handle_event(&state);
        state.update(millisecond_frame_delay);
        state.render(renderer, window);
        SDL_Delay(millisecond_frame_delay);
    };

    return 0;  
};