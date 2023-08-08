

const float boundary_x_low = -1000.0;
const float boundary_y_low = -1000.0;
const float boundary_z_low = -1000.0;

const float boundary_x_high = 1000.0;
const float boundary_y_high = 1000.0;
const float boundary_z_high = 1000.0;

const float forward_thruster_power = 250.0;
const float angular_thruster_power = 0.5;

const int scenario_max_objects = 512;
const int points_per_object = 16;
const int millisecond_frame_delay = 10;

const float PI = 3.141592;

const float view_max_angle = PI / 2.5;

const float min_velocity = 10.0;
const float max_velocity = 25.0;

const float object_min_radius = 10.0;
const float object_max_radius = 20.0;

const float deceleration_rate = 0.4;

const bool ignore_losing = true;

const float angular_change = angular_thruster_power * millisecond_frame_delay / 1000.0f;