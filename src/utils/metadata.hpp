#pragma once

#include <string>

#include "cgp/cgp.hpp" // Adjust path if needed
#include "camera/distortion.hpp"

std::string generate_camera_metadata(
    cgp::vec3 pos,
    float pitch,
    float yaw,
    float roll,
    float fov,
    int width,
    int height,
    const DistortionConfig& cfg,
    bool distorted_image
);