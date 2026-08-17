#include "metadata.hpp"

#include <sstream>
#include <iomanip>
#include <cmath>

using namespace cgp;

std::string generate_camera_metadata(
    cgp::vec3 pos,
    float pitch,
    float yaw,
    float roll,
    float fov,
    int width,
    int height,
    const DistortionConfig& cfg,
    bool distorted_image)
{
    std::stringstream ss;
    ss << std::fixed << std::setprecision(8);

    const double p = cfg.pixel_size_mm;

    const double f_px_y =
        static_cast<double>(height) /
        (2.0 * std::tan(static_cast<double>(fov) / 2.0));

    const double f_mm_y = f_px_y * p;

    ss << "{";
    ss << "\"position\": [" << pos.x << ", " << pos.y << ", " << pos.z << "], ";
    ss << "\"angles_deg\": {";
    ss << "\"pitch\": " << pitch << ", ";
    ss << "\"yaw\": " << yaw << ", ";
    ss << "\"roll\": " << roll << "}, ";

    ss << "\"fov_rad\": " << fov << ", ";
    ss << "\"image_width_px\": " << width << ", ";
    ss << "\"image_height_px\": " << height << ", ";
    ss << "\"pixel_size_mm\": " << p << ", ";
    ss << "\"focal_length_px_y\": " << f_px_y << ", ";
    ss << "\"focal_length_mm_y\": " << f_mm_y << ", ";

    ss << "\"distorted_image\": "
        << (distorted_image ? "true" : "false")
        << ", ";

    ss << "\"distortion_config\": {";
    ss << "\"enabled\": " << (cfg.enabled ? "true" : "false") << ", ";
    ss << "}";

    ss << "}";

    return ss.str();
}