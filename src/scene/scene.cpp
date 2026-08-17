#include "scene.hpp"
#include "renderer.hpp"
#include "camera/camera.hpp"

using namespace cgp;

void scene_structure::initialize() {
    camera_control.initialize(inputs, window);
    camera_control.set_rotation_axis_z();
    camera_control.look_at({ 3.0f, 500.0f, 2.0f }, { 0,0,0 }, { 0,0,1 });

    display_info();
    geometry.initialize();

    camera_projection.field_of_view = 2.0f * std::atan(sensor_size / (2.0*focal_length));
}

void scene_structure::render_scene(const environment_structure& env) {
    if (gui.display_frame) {
        draw(geometry.global_frame, env);
    }
    geometry.render(env);
}

void scene_structure::display_frame() {

    if (!auto_settings.automation_finished) {
        if (auto_settings.camera_mode == 2) {
            if (automator.read_next_camera_from_csv(auto_settings, camera_control, camera_projection))
                request_screenshot = true;
            else
                auto_settings.automation_finished = true;
        }
        else if (auto_settings.camera_mode == 3) {
            if (auto_settings.image_counter <= auto_settings.target_random_images) {
                automator.generate_random_camera_pose(auto_settings, camera_control);
                request_screenshot = true;
            }
            else {
                auto_settings.automation_finished = true;
            }
        }
    }

    // 2. Render normal frame to screen
    camera_projection.aspect_ratio = window.aspect_ratio();
    environment.camera_projection = camera_projection.matrix();
    environment.camera_view = camera_control.camera_model.matrix_view();
    environment.light = camera_control.camera_model.position();

    render_scene(environment);

    // 3. Handle Screenshots
    if (request_screenshot) {
        std::string current_id = format_id(auto_settings.image_counter);

        auto render_func = [this](const environment_structure& e) { this->render_scene(e); };

        scene_capture::capture_high_res(
            capture_width, capture_height,
            auto_settings.output_folder, current_id,
            camera_projection, camera_control, environment, window,
            focal_length, dof, f_stop, focus_distance, world_scale, sensor_size/ static_cast<float>(capture_height),
            render_func
        );

        if (auto_settings.camera_mode == 1 || auto_settings.camera_mode == 3) {
            automator.record_camera_state(current_id, auto_settings.output_folder,
                camera_control.camera_model.position(),
                camera_control.camera_model.front(),
                camera_control.camera_model.up(),
                camera_projection.field_of_view);
        }

        auto_settings.image_counter++;
        request_screenshot = false;
    }
}

void scene_structure::display_gui()
{
	ImGui::Checkbox("Frame", &gui.display_frame);
	ImGui::Checkbox("Wireframe", &gui.display_wireframe);

	// Lens and projection
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Text("Lens & Projection");
	ImGui::SliderAngle("Field of View", &camera_projection.field_of_view, 1.0f, 160.0f);
	ImGui::SliderFloat("Near Clipping", &camera_projection.depth_min, 0.001f, 2.0f, "%.3f");
	ImGui::SliderFloat("Far Clipping", &camera_projection.depth_max, 10.0f, 10000.0f);

	// Position
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Text("Spatial Position");
	ImGui::SliderFloat("Camera X", &camera_control.camera_model.position_camera.x, -20.f, 20.0f);
	ImGui::SliderFloat("Camera Y", &camera_control.camera_model.position_camera.y, -20.f, 2000.0f);
	ImGui::SliderFloat("Camera Z", &camera_control.camera_model.position_camera.z, -5.0f, 20.0f);

	// Pitch, Yaw, Roll
	cgp::vec3 front = camera_control.camera_model.front();
	cgp::vec3 right = camera_control.camera_model.right();
	cgp::vec3 up = camera_control.camera_model.up();
	float pitch_rad = std::asin(front.z);
	float yaw_rad = std::atan2(front.y, front.x);
	float roll_rad = 0.0f;

	// This logic is in case of gimbal lock
	if (std::abs(front.z) < 0.999f) { 
		cgp::vec3 world_up = { 0.0f, 0.0f, 1.0f };
		cgp::vec3 ref_right = cgp::normalize(cgp::cross(world_up, front));
		cgp::vec3 ref_up = cgp::cross(front, ref_right);
		roll_rad = std::atan2(cgp::dot(right, ref_up), cgp::dot(right, ref_right));
	}
	else {
		roll_rad = std::atan2(right.y, right.x);
	}

	float pitch_deg = pitch_rad * 180.0f / Pi;
	float yaw_deg = yaw_rad * 180.0f / Pi;
	float roll_deg = roll_rad * 180.0f / Pi;
	ImGui::Text("Pitch: %6.1f °", pitch_deg);
	ImGui::Text("Yaw:   %6.1f °", yaw_deg);
	ImGui::Text("Roll:  %6.1f °", roll_deg);
}

void scene_structure::mouse_move_event(){if (!inputs.keyboard.shift) camera_control.action_mouse_move();}
void scene_structure::mouse_click_event(){camera_control.action_mouse_click();}
void scene_structure::idle_frame(){camera_control.idle_frame();}
void scene_structure::keyboard_event(){
	camera_control.action_keyboard();
	if (inputs.keyboard.is_pressed('p')) {
		request_screenshot = true;
	}
}

void scene_structure::display_info()
{
	std::cout << "\nCAMERA CONTROL:" << std::endl;
	std::cout << "-----------------------------------------------" << std::endl;
	std::cout << camera_control.doc_usage() << std::endl;
	std::cout << "-----------------------------------------------\n" << std::endl;
}
