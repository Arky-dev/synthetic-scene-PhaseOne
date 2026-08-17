#pragma once
#include "cgp/cgp.hpp"
#include "scene/environment.hpp"
#include <string>
#include <functional>

namespace scene_capture {
    void capture_high_res(
        int width, int height,
        const std::string& output_folder,
        const std::string& current_id,
        cgp::camera_projection_perspective& camera_projection,
        cgp::camera_controller_2d_displacement& camera_control,
        environment_structure& environment,
        window_structure& window,
        int focal_length,
        bool dof,
        int f_stop,
        int focus_distance,
        float world_scale,
        float pixel_size,
        std::function<void(const environment_structure&)> render_scene_func
    );
}