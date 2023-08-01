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
    point points[16];
    float azimuth; //Properties are with respect to the reference frame and are converted to the players perspective in real time during updates
    float inclination;
    float velocity;
};

float accelerate(float old) {
    return old;
};

point* to_render_points(flying_object* obj, point* relative_distance, movement_charcteristics* facing, int screen_h, int screen_w) {
    point out[16];
    
};

class gameState {
    public:
        float player_x; //Distances are from the 0,0,0 point to the player, using the PLAYERS perspective on length
        float player_y;
        float player_z;
        bool accelerating; //All time and velocity is from the player's perspective assuming 0,0,0 is stationary
        bool turning_left;
        bool turning_right;
        bool turning_up;
        bool turning_down;
        void render(SDL_Renderer* renderer) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        };
        void update(float delay) {
            if (accelerating) {
                this->player_velocity = accelerate(this->player_velocity);
            };
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
            this->object_count = 0;
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

movement_charcteristics to_polar(point* cartesian) {
    movement_charcteristics out;
    float r = sqrt(cartesian->x * cartesian->x + cartesian->y * cartesian->y + cartesian->z * cartesian->z);
    out.velocity = r;
    out.inclination = r == 0 ? 0 : acos(cartesian->z/r);
    out.azimuth = (cartesian->x == 0 && cartesian->y == 0) ? 0 : sign(cartesian->y) * 
        acos(cartesian->x / sqrt(cartesian->x * cartesian->x + cartesian->y * cartesian->y));
    return out;
};

movement_charcteristics add_velocities(movement_charcteristics* a, movement_charcteristics* b) { //https://en.wikipedia.org/wiki/Velocity-addition_formula#Special_relativity
    point vector_a = to_cartesian(a);
    point vector_b = to_cartesian(b);
    float divisor = 1 + ((vector_a.x * vector_b.x + vector_a.y * vector_b.y + vector_a.z * vector_b.z)
     / (speed_of_light * speed_of_light));
    point vector_sum = {
        x: (vector_a.x + vector_b.x) / divisor,
        y: (vector_a.y + vector_b.y) / divisor,
        z: (vector_a.z + vector_b.z) / divisor
    };
    movement_charcteristics out = to_polar(&vector_sum);
    return out;
};

int main(int argc, char *argv[]) {  
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {  
        printf("error initializing SDL: %s\n", SDL_GetError());  
    };
    SDL_DisplayMode DisplayMode;
    SDL_GetCurrentDisplayMode(0, &DisplayMode);
    SDL_Window* window = SDL_CreateWindow("Slowlight",  
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        DisplayMode.w, DisplayMode.h, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED); 
    gameState state;
    state.initialise();
    while (1) {
        handle_event(&state);
        state.update(millisecond_frame_delay);
        state.render(renderer);
        SDL_Delay(millisecond_frame_delay);
    };

    return 0;  
};