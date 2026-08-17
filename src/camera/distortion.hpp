#pragma once

#include <vector>

struct DistortionConfig {
    bool enabled = false;

    // Sensor/camera convention
    double pixel_size_mm = 0.0026193;
    double cx_px = -1.0;
    double cy_px = -1.0;

    // Inverse-mapping iterations
    int inverse_iterations = 5;

};

// Global config used by the scene.
// You can later replace this with a scene_structure member if preferred.
extern DistortionConfig distortion_config;

void distortion_function_mm(
    double x_mm,
    double y_mm,
    int width,
    int height,
    const DistortionConfig& cfg,
    double& dx_mm,
    double& dy_mm
);

std::vector<unsigned char> distort_image_inverse_mapping(
    const std::vector<unsigned char>& undistorted,
    int width,
    int height,
    const DistortionConfig& cfg
);