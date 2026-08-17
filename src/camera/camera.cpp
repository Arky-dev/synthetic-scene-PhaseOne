#include "camera.hpp"

#include <random>
#include <sstream>
#include <iomanip>

using namespace cgp;

void camera_automator::record_camera_state(const std::string& id, const std::string& folder, cgp::vec3 pos, cgp::vec3 front, cgp::vec3 up, float fov) {
	std::string csv_path = folder + "/dataset_cameras.csv";
	std::ofstream file(csv_path, std::ios::app);

	file.seekp(0, std::ios::end);
	if (file.tellp() == 0) {
		// Updated header for bulletproof vectors
		file << "id,pos_x,pos_y,pos_z,front_x,front_y,front_z,up_x,up_y,up_z,fov\n";
	}

	file << id << ","
		<< pos.x << "," << pos.y << "," << pos.z << ","
		<< front.x << "," << front.y << "," << front.z << ","
		<< up.x << "," << up.y << "," << up.z << "," << fov << "\n";
	file.close();
}

bool camera_automator::read_next_camera_from_csv(automation_settings& settings, cgp::camera_controller_2d_displacement& camera, cgp::camera_projection_perspective& proj) {
	std::string csv_path = settings.input_csv_path;

	if (!csv_is_open) {
		csv_input_file.open(csv_path);
		if (!csv_input_file.is_open()) {
			std::cout << "Error: Could not open " << csv_path << " for reading.\n";
			return false;
		}
		csv_is_open = true;
		std::string header;
		std::getline(csv_input_file, header); // Skip the header row
	}

	std::string line;
	if (std::getline(csv_input_file, line)) {
		std::stringstream ss(line);
		std::string item;
		std::vector<std::string> tokens;
		while (std::getline(ss, item, ',')) { tokens.push_back(item); }

		if (tokens.size() >= 11) {
			// Reconstruct Pos, Front, Up
			cgp::vec3 pos = { std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]) };
			cgp::vec3 front = { std::stof(tokens[4]), std::stof(tokens[5]), std::stof(tokens[6]) };
			cgp::vec3 up = { std::stof(tokens[7]), std::stof(tokens[8]), std::stof(tokens[9]) };

			// Reconstruct camera orientation exactly
			camera.camera_model.position() = pos;

			// Rebuilding the view matrix orientation
			cgp::vec3 right = cgp::normalize(cgp::cross(front, up));

			// Depending on CGP internals, you can set the frame directly or use look_at
			// The safest method is look_at using the pos + front as the target
			camera.camera_model.look_at(pos, pos + front);

			proj.field_of_view = std::stof(tokens[10]);
			return true;
		}
	}

	// EOF reached
	csv_input_file.close();
	return false;
}

void camera_automator::generate_random_camera_pose(const automation_settings& settings, cgp::camera_controller_2d_displacement& camera) {

	// 1. Pick a random camera position within bounds
	vec3 cam_pos = {
		rand_interval(settings.cam_min_x, settings.cam_max_x),
		rand_interval(settings.cam_min_y, settings.cam_max_y),
		rand_interval(settings.cam_min_z, settings.cam_max_z)
	};

	// 2. Pick a random focus point ON the wall
	vec3 target_point = {
		rand_interval(settings.wall_min_x, settings.wall_max_x),
		settings.wall_y,
		rand_interval(settings.wall_min_z, settings.wall_max_z)
	};

	// 4. Force the camera to look at the target point
	camera.camera_model.look_at(cam_pos, target_point);

	float random_roll_rad = rand_interval(-cgp::Pi / 2, cgp::Pi / 2);
	camera.camera_model.roll = random_roll_rad;

}


float rand_interval(float min, float max)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());

    std::uniform_real_distribution<float> distrib(min, max);
    return distrib(gen);
}

std::string format_id(int counter)
{
    std::ostringstream ss;
    ss << "img_"
        << std::setw(4)
        << std::setfill('0')
        << counter;

    return ss.str();
}