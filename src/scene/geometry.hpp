#pragma once
#include "cgp/cgp.hpp"
#include "environment.hpp"

struct scene_geometry {
    // 3D Models
    cgp::mesh_drawable global_frame;

    // Calibration
    cgp::mesh_drawable wall, wall_blend;
    cgp::mesh_drawable wall2, wall2_blend;
    cgp::mesh_drawable lines[4];
    cgp::mesh_drawable lines_blend[4];

    // Background
    cgp::mesh_drawable shape, background, leftwall, rightwall, ceiling;

    // Functions
    void initialize();
    void render(const environment_structure& env);
};