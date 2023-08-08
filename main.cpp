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

void render_object(flying_object* object, int h, int w, SDL_Renderer* renderer, float x, float y, float z, point* forward, point* right, point* up) {

    SDL_Vertex centre;

    float scale = static_cast<float>(max(h, w));

    point separation_vector = {
        x: object->centre.x - x,
        y: object->centre.y - y,
        z: object->centre.z - z
    };

    //Order is forward, right then up

    // float determinant = forward->x * right->y * up->z + right->x * up->y * forward->z + up->x * forward->y * right->z - 
    //     forward->x * up->y * right->z - right->x * forward->y * up->z - up->x * right->y * forward->z; Is always 1 if normalising properly

    float adjoint[3][3] = {{
        right->y * up->z - right->z * up->y,
        forward->z * up->y - forward->y * up->z,
        forward->y * right->z - forward->z * right->y
    }, {
        right->z * up->x - right->x * up->z,
        forward->x * up->z - forward->z * up->x,
        forward->z * right->x - forward->x * right->z
    }, {
        right->x * up->y - right->y * up->x,
        forward->y * up->x - forward->x * up->y,
        forward->x * right->y - forward->y * right->x
    }};

    point converted_separation_vector = {
        x: (separation_vector.x * adjoint[0][0] + separation_vector.y * adjoint[1][0] + separation_vector.z  * adjoint[2][0]),
        y: (separation_vector.x * adjoint[0][1] + separation_vector.y * adjoint[1][1] + separation_vector.z  * adjoint[2][1]),
        z: (separation_vector.x * adjoint[0][2] + separation_vector.y * adjoint[1][2] + separation_vector.z  * adjoint[2][2])
    };

    if (converted_separation_vector.x <= 0) {
        return;
    };

    centre = {
        position: {
            x: (scale * converted_separation_vector.y / sqrt(converted_separation_vector.x * converted_separation_vector.x + converted_separation_vector.z * converted_separation_vector.z)) + static_cast<float>(w / 2),
            y: (scale * converted_separation_vector.z / sqrt(converted_separation_vector.x * converted_separation_vector.x + converted_separation_vector.y * converted_separation_vector.y)) + static_cast<float>(h / 2)
        },
        color: {
            r: 0,
            g: 0,
            b : 255,
            a : 255
        }
    };
    SDL_Vertex to_print[points_per_object * 3];
    float distance = scale * object->radius / sqrt(converted_separation_vector.x * converted_separation_vector.x + 
        converted_separation_vector.y * converted_separation_vector.y + 
        converted_separation_vector.z * converted_separation_vector.z);
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

void normalise(point* a) {
    float divisor = sqrt(a->x * a->x + a->y * a->y + a->z * a->z);
    a->x = a->x / divisor;
    a->y = a->y / divisor;
    a->z = a->z / divisor;
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
        point forward_belief;
        point right_belief;
        point up_belief;
        void render(SDL_Renderer* renderer, SDL_Window* window) {
            SDL_RenderClear(renderer);
            int h;
            int w;
            SDL_GetWindowSize(window, &w, &h);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            for (int i = 0; i < object_count; i++) {
                render_object(&objects[i], h, w, renderer, player_x, player_y, player_z, &forward_belief, &right_belief, &up_belief);
            };
            SDL_RenderPresent(renderer);
        };
        void update(bool* game_over) {
            if (accelerating) {
                player_velocity += forward_thruster_power * static_cast<float>(millisecond_frame_delay) / 1000.0f;
            } else {
                player_velocity = player_velocity * (1 - deceleration_rate * static_cast<float>(millisecond_frame_delay) / 1000.0f);
            };
            float sin_diff = sin(angular_change);
            float cos_diff = cos(angular_change);
            if (turning_left != turning_right) {
                point temp_right = {
                    x: right_belief.x,
                    y: right_belief.y,
                    z: right_belief.z
                };
                right_belief = {
                    x: cos_diff * right_belief.x + sin_diff * (turning_left - turning_right) * forward_belief.x,
                    y: cos_diff * right_belief.y + sin_diff * (turning_left - turning_right) * forward_belief.y,
                    z: cos_diff * right_belief.z + sin_diff * (turning_left - turning_right) * forward_belief.z
                };
                forward_belief = {
                    x: cos_diff * forward_belief.x + sin_diff * (turning_right - turning_left) * temp_right.x,
                    y: cos_diff * forward_belief.y + sin_diff * (turning_right - turning_left) * temp_right.y,
                    z: cos_diff * forward_belief.z + sin_diff * (turning_right - turning_left) * temp_right.z
                };
                normalise(&right_belief);
                normalise(&forward_belief);
            };
            if (turning_down != turning_up) {
                point temp_up = {
                    x: up_belief.x,
                    y: up_belief.y,
                    z: up_belief.z
                };
                up_belief = {
                    x: cos_diff * up_belief.x + sin_diff * (turning_up - turning_down) * forward_belief.x,
                    y: cos_diff * up_belief.y + sin_diff * (turning_up - turning_down) * forward_belief.y,
                    z: cos_diff * up_belief.z + sin_diff * (turning_up - turning_down) * forward_belief.z
                };
                forward_belief = {
                    x: cos_diff * forward_belief.x + sin_diff * (turning_down - turning_up) * temp_up.x,
                    y: cos_diff * forward_belief.y + sin_diff * (turning_down - turning_up) * temp_up.y,
                    z: cos_diff * forward_belief.z + sin_diff * (turning_down - turning_up) * temp_up.z
                };
                normalise(&up_belief);
                normalise(&forward_belief);
            };
            player_x += player_velocity * forward_belief.x * static_cast<float>(millisecond_frame_delay) / 1000.0f;
            player_y += player_velocity * forward_belief.y * static_cast<float>(millisecond_frame_delay) / 1000.0f;
            player_z += player_velocity * forward_belief.z * static_cast<float>(millisecond_frame_delay) / 1000.0f;
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
            forward_belief = {1, 0, 0};
            right_belief = {0, 1, 0};
            up_belief = {0, 0, 1};
            player_velocity = 0;
            object_count = 0;
            accelerating = false;
            turning_down = false;
            turning_up = false;
            turning_left = false;
            turning_right = false;
        };
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
        state.update(&game_over);
        state.render(renderer, window);
        SDL_Delay(millisecond_frame_delay);
    };

    return 0;  
};