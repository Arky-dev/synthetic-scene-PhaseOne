#pragma once
#include "cgp/cgp.hpp"
#include "environment.hpp"
#include "geometry.hpp"
#include "camera/camera.hpp"

struct gui_parameters {
    bool display_frame = false;
    bool display_wireframe = false;
    cgp::vec3 light_color = { 1,1,1 };
};

struct scene_structure : cgp::scene_inputs_generic {

    environment_structure environment;
    window_structure window;
    input_devices inputs;
    gui_parameters gui;

    scene_geometry geometry;
    automation_settings auto_settings;
    camera_automator automator;

    cgp::camera_controller_2d_displacement camera_control;
    cgp::camera_projection_perspective camera_projection;

    // State
    bool request_screenshot = false;
    int capture_width = 13468;
    int capture_height = 9564;
    int focal_length = 80;
    float sensor_size = 25.0509582f;
    bool dof = false;
    int f_stop = 22;
    int focus_distance = 800;
    float world_scale = 0.125f;

    // Core loops
    void initialize();
    void render_scene(const environment_structure& env);
    void display_frame();
    void display_gui();
    void display_info();

    // Callbacks
    void mouse_move_event();
    void mouse_click_event();
    void keyboard_event();
    void idle_frame();
};