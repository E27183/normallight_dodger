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
    point centre;
    float radius;
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
    out.x = angular->velocity * sin(angular->inclination) * cos(angular->azimuth);
    out.y = angular->velocity * sin(angular->inclination) * sin(angular->azimuth);
    out.z = angular->velocity * cos(angular->inclination);
    return out;
};

bool in_range(float a) {
    return a < PI / 2.0f && a > -PI / 2.0f;
};

float sign(float a) {
    return a > 0 ? 1.0 : a < 0 ? -1.0 : 0.0;
};

float normalise(float a) {
    return a > PI ? a - 2 * PI : a < -PI ? a + 2 * PI : a;
};

float mini_normalise(float a) {
    return a > PI / 2.0f ? a -  PI : a < -PI / 2.0f ? a + PI : a;
};

void perspective_transform_render(float perspective_azumith, float azumith, float perspective_inclination, float inclination, float* output) {
    if (abs(normalise(perspective_azumith - azumith)) < PI) {
        *(output) = normalise(azumith - perspective_azumith);
        *(output + 1) = normalise(inclination - perspective_inclination);
    } else {
        if (sign(inclination) != sign(perspective_inclination)) {
            *(output + 2) = -1.0f;
            return;
        };
        *(output) = normalise(azumith - perspective_azumith + PI);
        *(output + 1) = normalise(PI - abs(inclination) - abs(perspective_inclination)) * sign(perspective_inclination);
    };
    *(output + 2) = in_range(*(output)) && in_range(*(output + 1)) ? 0.0f : -1.0f;
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

void render_object(flying_object* object, int h, int w, SDL_Renderer* renderer, float x, float y, float z, float azimuth, float inclination, bool upside_down) {

    SDL_Vertex centre;

    float scale = lens_modifier * static_cast<float>(max(h, w));


    point separation_vector = {
        x: object->centre.x - x,
        y: object->centre.y - y,
        z: object->centre.z - z
    };
    movement_charcteristics polar = to_polar(&separation_vector);
    float perspective[3];
    perspective_transform_render(azimuth, polar.azimuth, inclination, polar.inclination, &perspective[0]);
    if (perspective[2] < 0) {
        return;
    };
    if (upside_down) {
        perspective[1] = -perspective[1];
    };
    centre = {
        position: {
            x: (scale * sin(perspective[0]) / sin((PI / 2) - perspective[0])) + static_cast<float>(w / 2),
            y: (scale * sin(perspective[1]) / sin((PI / 2) - perspective[1])) + static_cast<float>(h / 2)
        },
        color: {
            r: 0,
            g: 0,
            b : 255,
            a : 255
        }
    };
    SDL_Vertex to_print[points_per_object * 3];
    float distance = scale * object->radius / polar.velocity;
    for (int i = 0; i < points_per_object; i++) {
        to_print[i * 3] = {
            position: {
                x: centre.position.x + distance * sin(i * 2 * PI / static_cast<float>(points_per_object)),
                y: centre.position.y + distance * cos(i * 2 * PI / static_cast<float>(points_per_object)),
            },
            color: {255, 0, 0, 255},
        };
        to_print[i * 3 + 1] = {
            position: {
                x: centre.position.x + distance * sin((i + 1) * 2 * PI / static_cast<float>(points_per_object)),
                y: centre.position.y + distance * cos((i + 1) * 2 * PI / static_cast<float>(points_per_object)),
            },
            color: {255, 0, 0, 255},
        };
        to_print[i * 3 + 2] = centre;
    };
    SDL_RenderGeometry(renderer, NULL, to_print, points_per_object * 3, NULL, 0);
};

float random_place(float min, float max) {
    return min + (max - min) * static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
};

void create_object(flying_object* shell) {
    int flag = rand() % 6;
    point centre;
    switch (flag) {
        case 0:
            centre = {
                x: boundary_x_low,
                y: random_place(boundary_y_low, boundary_y_high),
                z: random_place(boundary_z_low, boundary_z_high)
            };
            break;
        case 1:
            centre = {
                x: boundary_x_high,
                y: random_place(boundary_y_low, boundary_y_high),
                z: random_place(boundary_z_low, boundary_z_high)
            };
            break;
        case 2:
            centre = {
                x: random_place(boundary_x_low, boundary_x_high),
                y: boundary_y_low,
                z: random_place(boundary_z_low, boundary_z_high)
            };
            break;
        case 3:
            centre = {
                x: random_place(boundary_x_low, boundary_x_high),
                y: boundary_y_high,
                z: random_place(boundary_z_low, boundary_z_high)
            };
            break;
        case 4:
            centre = {
                x: random_place(boundary_x_low, boundary_x_high),
                y: random_place(boundary_y_low, boundary_y_high),
                z: boundary_z_low
            };
            break;
        case 5:
            centre = {
                x: random_place(boundary_x_low, boundary_x_high),
                y: random_place(boundary_y_low, boundary_y_high),
                z: boundary_z_high
            };
            break;
    };
    shell->azimuth = (2.0f * PI * static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) - PI;
    shell->inclination = (PI * static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) - (PI / 2.0f);
    shell->velocity = min_velocity + (max_velocity - min_velocity) * static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    shell->centre = {
        x: centre.x,
        y: centre.y,
        z: centre.z
    };
    shell->radius = object_min_radius + (object_max_radius - object_min_radius) * (static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
};

bool out_of_bounds(float x, float y, float z, float tolerance) {
    return x > boundary_x_high + tolerance || x < boundary_x_low - tolerance || 
        y > boundary_y_high + tolerance || y < boundary_y_low - tolerance || 
        z > boundary_z_high + tolerance || z < boundary_z_low - tolerance;
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
        bool upside_down;
        void render(SDL_Renderer* renderer, SDL_Window* window) {
            SDL_RenderClear(renderer);
            int h;
            int w;
            SDL_GetWindowSize(window, &w, &h);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            for (int i = 0; i < object_count; i++) {
                render_object(&objects[i], h, w, renderer, player_x, player_y, player_z, player_direction_azimuth, player_direction_inclination, upside_down);
            };
            SDL_RenderPresent(renderer);
        };
        void update(float delay, bool* game_over) {
            if ((turning_down && !upside_down) || (turning_up && upside_down)) { //if 2 conflicting directions are held they just cancel each other out
                player_direction_inclination += angular_thruster_power * static_cast<float>(millisecond_frame_delay) / 1000.0f;
                if (player_direction_inclination > PI) {
                    player_direction_inclination = PI;
                    upside_down = !upside_down;
                    player_direction_azimuth = player_direction_azimuth > 0 ? player_direction_azimuth - PI : player_direction_azimuth + PI;
                };
            };
            if ((turning_up && !upside_down) || (turning_down && upside_down)) {
                player_direction_inclination -= angular_thruster_power * static_cast<float>(millisecond_frame_delay) / 1000.0f;
                if (player_direction_inclination < 0.0f) {
                    player_direction_inclination = 0.0f;
                    upside_down = !upside_down;
                    player_direction_azimuth = player_direction_azimuth > 0 ? player_direction_azimuth - PI : player_direction_azimuth + PI;
                };
            };
            if (turning_left) {
                player_direction_azimuth -= angular_thruster_power * static_cast<float>(millisecond_frame_delay) / 1000.0f;
                if (player_direction_azimuth < 0) {
                    player_direction_azimuth += 2 * PI;
                };
            };
            if (turning_right) {
                player_direction_azimuth += angular_thruster_power * static_cast<float>(millisecond_frame_delay) / 1000.0f;
                if (player_direction_azimuth > 2 * PI) {
                    player_direction_azimuth -= 2 * PI;
                };                
            };
            if (accelerating) {
                player_velocity += forward_thruster_power * static_cast<float>(millisecond_frame_delay) / 1000.0f;
            } else {
                player_velocity = player_velocity * (1 - deceleration_rate * static_cast<float>(millisecond_frame_delay) / 1000.0f);
            };
            movement_charcteristics player_movement = {
                azimuth: player_direction_azimuth,
                inclination: player_direction_inclination,
                velocity: player_velocity * static_cast<float>(millisecond_frame_delay) / 1000.0f
            };
            point movement_event = to_cartesian(&player_movement);
            player_x += movement_event.x;
            player_y += movement_event.y;
            player_z += movement_event.z;
            if (!ignore_losing && out_of_bounds(player_x, player_y, player_z, 0)) {
                    *game_over = true;
                    std::cout << "Game lost: player out of bounds\n";
                };
            if (object_count < scenario_max_objects) {
                create_object(&objects[object_count]);
                object_count++;
            };
            for (int i = 0; i < object_count; i++) {
                movement_charcteristics object_movement = {
                    azimuth: objects[i].azimuth,
                    inclination: objects[i].inclination,
                    velocity: objects[i].velocity * static_cast<float>(millisecond_frame_delay) / 1000.0f
                };
                point motion = to_cartesian(&object_movement);
                objects[i].centre.x += motion.x;
                objects[i].centre.y += motion.y;
                objects[i].centre.z += motion.z;
                if (out_of_bounds(objects[i].centre.x, objects[i].centre.y, objects[i].centre.z, objects[i].radius)) {
                    create_object(&objects[i]); //Automatically overwrites the old one
                };
            };
        };
        void initialise() {
            srand(time(NULL));
            player_x = 0;
            player_y = 0;
            player_z = 0;
            player_direction_azimuth = 0;
            player_direction_inclination = PI / 2.0f;
            player_velocity = 0;
            object_count = 0;
            accelerating = false;
            turning_down = false;
            turning_up = false;
            turning_left = false;
            turning_right = false;
            upside_down = false;
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

int min(int a, int b) {
    return a > b ? b : a;
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
        min(DisplayMode.w, DisplayMode.h), min(DisplayMode.w, DisplayMode.h), 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED); 
    gameState state;
    state.initialise();
    bool game_over = false;
    while (!game_over) {
        handle_event(&state);
        state.update(millisecond_frame_delay, &game_over);
        state.render(renderer, window);
        SDL_Delay(millisecond_frame_delay);
    };

    return 0;  
};