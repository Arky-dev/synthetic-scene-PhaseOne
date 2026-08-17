#include "distortion.hpp"

#include <cmath>
#include <algorithm>
#include <thread>
#include <functional>

#include "cgp/cgp.hpp" // Adjust if your project includes CGP differently

using namespace cgp;

DistortionConfig distortion_config;

void distortion_function_mm(
    double x_mm,
    double y_mm,
    int width,
    int height,
    const DistortionConfig& cfg,
    double& dx_mm,
    double& dy_mm)
{
    dx_mm = 0.0;
    dy_mm = 0.0;

    const double p = cfg.pixel_size_mm;

    const double half_w_mm = 0.5 * width * p;
    const double half_h_mm = 0.5 * height * p;
    const double r_max = std::sqrt(half_w_mm * half_w_mm + half_h_mm * half_h_mm);

    const double r = std::sqrt(x_mm * x_mm + y_mm * y_mm);

    //const double K1 = -1e-5;
    //const double K2 = 3e-9;
    //const double K3 = -1e-13;
    const double K4 = 7.571823e-14;

    double dr = K4 * std::pow(r, 8.0);

    dx_mm += dr * x_mm;
    dy_mm += dr * y_mm;

}

static void sample_rgb_bilinear(
    const std::vector<unsigned char>& image,
    int width,
    int height,
    double x,
    double y,
    unsigned char& r,
    unsigned char& g,
    unsigned char& b)
{
    if (x < 0.0 || y < 0.0 || x >= width - 1 || y >= height - 1) {
        r = g = b = 0;
        return;
    }

    const int x0 = static_cast<int>(std::floor(x));
    const int y0 = static_cast<int>(std::floor(y));
    const int x1 = x0 + 1;
    const int y1 = y0 + 1;

    const double ax = x - x0;
    const double ay = y - y0;

    auto get = [&](int px, int py, int c) -> double {
        const int idx = (py * width + px) * 3 + c;
        return static_cast<double>(image[idx]);
        };

    for (int c = 0; c < 3; ++c) {
        const double I00 = get(x0, y0, c);
        const double I10 = get(x1, y0, c);
        const double I01 = get(x0, y1, c);
        const double I11 = get(x1, y1, c);

        const double I0 = (1.0 - ax) * I00 + ax * I10;
        const double I1 = (1.0 - ax) * I01 + ax * I11;
        const double I = (1.0 - ay) * I0 + ay * I1;

        unsigned char value = static_cast<unsigned char>(
            std::clamp(std::round(I), 0.0, 255.0)
            );

        if (c == 0) r = value;
        if (c == 1) g = value;
        if (c == 2) b = value;
    }
}

std::vector<unsigned char> distort_image_inverse_mapping(
    const std::vector<unsigned char>& undistorted,
    int width,
    int height,
    const DistortionConfig& cfg)
{
    std::vector<unsigned char> distorted(width * height * 3, 0);

    const double p = cfg.pixel_size_mm;

    const double cx = (cfg.cx_px >= 0.0)
        ? cfg.cx_px
        : static_cast<double>(width) / 2.0;

    const double cy = (cfg.cy_px >= 0.0)
        ? cfg.cy_px
        : static_cast<double>(height) / 2.0;

    const unsigned int n_threads_hw = std::thread::hardware_concurrency();
    const unsigned int n_threads = std::max(
        1u,
        n_threads_hw == 0 ? 4u : n_threads_hw - 1u
    );

    auto worker = [&](int y_start, int y_end) {
        for (int y_d = y_start; y_d < y_end; ++y_d) {
            for (int x_d = 0; x_d < width; ++x_d) {

                double x_u = static_cast<double>(x_d);
                double y_u = static_cast<double>(y_d);

                for (int iter = 0; iter < cfg.inverse_iterations; ++iter) {
                    const double x_mm = (x_u - cx) * p;
                    const double y_mm = (y_u - cy) * p;

                    double dx_mm = 0.0;
                    double dy_mm = 0.0;

                    distortion_function_mm(
                        x_mm,
                        y_mm,
                        width,
                        height,
                        cfg,
                        dx_mm,
                        dy_mm
                    );

                    x_u = static_cast<double>(x_d) - dx_mm / p;
                    y_u = static_cast<double>(y_d) - dy_mm / p;
                }

                unsigned char r, g, b;
                sample_rgb_bilinear(
                    undistorted,
                    width,
                    height,
                    x_u,
                    y_u,
                    r,
                    g,
                    b
                );

                const int out_idx = (y_d * width + x_d) * 3;
                distorted[out_idx + 0] = r;
                distorted[out_idx + 1] = g;
                distorted[out_idx + 2] = b;
            }
        }
        };

    std::vector<std::thread> threads;
    threads.reserve(n_threads);

    const int rows_per_thread = height / static_cast<int>(n_threads);

    for (unsigned int t = 0; t < n_threads; ++t) {
        const int y_start = static_cast<int>(t) * rows_per_thread;
        const int y_end =
            (t == n_threads - 1)
            ? height
            : static_cast<int>(t + 1) * rows_per_thread;

        threads.emplace_back(worker, y_start, y_end);
    }

    for (auto& th : threads) {
        th.join();
    }

    return distorted;
}