#pragma once
#include "cgp/cgp.hpp"
#include <fstream>
#include <string>

struct automation_settings {
    int camera_mode = 1; // 1 = Manual, 2 = File, 3 = Random
    int image_counter = 1;
    int target_random_images = 108;
    bool automation_finished = false;

    std::string output_folder = "";
    std::string input_csv_path = "";

    // Boundaries
    float wall_min_x = -5.f, wall_max_x = 5.f;
    float wall_min_z = 3.f, wall_max_z = 9.f;
    float wall_y = -10.0f;
    float cam_min_y = 25.0f, cam_max_y = 35.0f;
    float cam_min_x = -10.0f, cam_max_x = 10.0f;
    float cam_min_z = 2.0f, cam_max_z = 10.0f;
};

class camera_automator {
    private:
        std::ifstream csv_input_file;
        bool csv_is_open = false;

    public:
        void record_camera_state(const std::string& id, const std::string& folder, cgp::vec3 pos, cgp::vec3 front, cgp::vec3 up, float fov);
        bool read_next_camera_from_csv(automation_settings& settings, cgp::camera_controller_2d_displacement& camera, cgp::camera_projection_perspective& proj);
        void generate_random_camera_pose(const automation_settings& settings, cgp::camera_controller_2d_displacement& camera);
};

float rand_interval(float min, float max);

std::string format_id(int counter);