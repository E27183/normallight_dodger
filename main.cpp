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
    out.x = angular->velocity * sin(angular->inclination) * cos(angular->azimuth);
    out.y = angular->velocity * sin(angular->inclination) * sin(angular->azimuth);
    out.z = angular->velocity * cos(angular->inclination);
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

void render_object(flying_object* object, int h, int w, SDL_Renderer* renderer, float x, float y, float z, float azimuth, float inclination) {
    SDL_Vertex vertices[points_per_object];
    int illegal_points = 0;
    float scale = lens_modifier * static_cast<float>(max(h, w));
    for (int i = 0; i < points_per_object; i++) {
        point separation_vector = {
            x: object->points[i].x - x,
            y: object->points[i].y - y,
            z: object->points[i].z - z
        };
        movement_charcteristics polar = to_polar(&separation_vector);
        polar.azimuth -= azimuth;
        polar.inclination -= inclination;
        if (polar.azimuth < -PI) {
            polar.azimuth += 2 * PI;
        } else if (polar.azimuth > PI) {
            polar.azimuth -= 2 * PI;
        };
        if (polar.inclination < -PI) {
            polar.inclination += 2 * PI;
        } else if (polar.inclination > PI) {
            polar.inclination -= 2 * PI;
        };
        if (abs(polar.azimuth) >= PI / 2 || abs(polar.inclination) >= PI / 2) {
            vertices[i] = {
                position: {
                    x: 0,
                    y: 0
                },
                color: {0, 0, 0, 0}
            };
            illegal_points++;
        } else {
            vertices[i] = {
                position: {
                    x: (scale * sin(polar.azimuth) / sin((PI / 2) - polar.azimuth)) + static_cast<float>(w / 2),
                    y: (scale * sin(polar.inclination) / sin((PI / 2) - polar.inclination)) + static_cast<float>(h / 2)
                },
                color: {
                    r: static_cast<unsigned char>(255.0f - (static_cast<float>(255 * i) / points_per_object)),
                    g: static_cast<unsigned char>(static_cast<float>(255 * i) / points_per_object),
                    b : 255,
                    a : 255
                }
            };
        };
    };
    SDL_Vertex to_print[3 * max((points_per_object - illegal_points) * (points_per_object - illegal_points - 1) * (points_per_object - illegal_points - 2), 0)];
    int index = 0;
    for (int i = 0; i < points_per_object; i++) {
        for (int j = 0; j < points_per_object; j++) {
            for (int k = 0; k < points_per_object; k++) {
                if (i != j && i != k && j != k && vertices[i].color.r > 0 && vertices[j].color.r > 0 && vertices[k].color.r > 0) {
                    to_print[index] = vertices[i];
                    to_print[index+1] = vertices[j];
                    to_print[index+2] = vertices[k];
                    index += 3;
                };
            };
        };
    };
    SDL_RenderGeometry(renderer, NULL, to_print, index, NULL, 0);
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
    for (int i = 0; i < points_per_object; i++) {
        movement_charcteristics vectorized = {
            azimuth: (2.0f * PI * static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) - PI,
            inclination: (PI * static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) - (PI / 2.0f),
            velocity: object_min_radius + (object_max_radius - object_min_radius) * static_cast<float>(rand()) / static_cast<float>(RAND_MAX)
        };
        point pointified = to_cartesian(&vectorized);
        shell->points[i] = {
            x: centre.x + pointified.x,
            y: centre.y + pointified.y,
            z: centre.z + pointified.z
        };
    };
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
        void render(SDL_Renderer* renderer, SDL_Window* window) {
            SDL_RenderClear(renderer);
            int h;
            int w;
            SDL_GetWindowSize(window, &w, &h);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            for (int i = 0; i < object_count; i++) {
                render_object(&objects[i], h, w, renderer, player_x, player_y, player_z, player_direction_azimuth, player_direction_inclination);
            };
            SDL_RenderPresent(renderer);
        };
        void update(float delay) {
            if (turning_down) { //if 2 conflicting directions are held they just cancel each other out
                player_direction_inclination -= angular_thruster_power * static_cast<float>(millisecond_frame_delay) / 1000.0f;
                if (player_direction_inclination < -PI) {
                    player_direction_inclination += 2 * PI;
                };
            };
            if (turning_up) {
                player_direction_inclination += angular_thruster_power * static_cast<float>(millisecond_frame_delay) / 1000.0f;
                if (player_direction_inclination > PI) {
                    player_direction_inclination -= 2 * PI;
                };
            };
            if (turning_left) {
                player_direction_azimuth -= angular_thruster_power * static_cast<float>(millisecond_frame_delay) / 1000.0f;
                if (player_direction_azimuth < -PI) {
                    player_direction_azimuth += 2 * PI;
                };
            };
            if (turning_right) {
                player_direction_azimuth += angular_thruster_power * static_cast<float>(millisecond_frame_delay) / 1000.0f;
                if (player_direction_azimuth > PI) {
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
                for (int j = 0; j < points_per_object; j++) {
                    // objects[i].points[j].x += motion.x;
                    // objects[i].points[j].y += motion.y;
                    // objects[i].points[j].z += motion.z;
                    if (out_of_bounds(objects[i].points[j].x, objects[i].points[j].y, objects[i].points[j].z, object_max_radius)) {
                        create_object(&objects[i]); //Automatically overwrites the old one
                    };
                };
            };
        };
        void initialise() {
            srand(time(NULL));
            player_x = 0;
            player_y = 0;
            player_z = 0;
            player_direction_azimuth = 0;
            player_direction_inclination = 0;
            player_velocity = 0;
            object_count = 0;
            accelerating = false;
            turning_down = false;
            turning_up = false;
            turning_left = false;
            turning_right = false;
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
    while (1) {
        handle_event(&state);
        state.update(millisecond_frame_delay);
        state.render(renderer, window);
        SDL_Delay(millisecond_frame_delay);
    };

    return 0;  
};