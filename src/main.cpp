#include "cgp/cgp.hpp" 
#include "application.hpp"
#include "scene/scene.hpp"
#include "utils/cmd_io.hpp"

#include <filesystem>

int main(int, char* argv[])
{
    std::cout << "===== Screenshot & Camera Setup =====\n";

    int capture_width = read_int("Enter image capture width", 13468);
    int capture_height = read_int("Enter image capture height", 9564);
    int focal_length = read_int("Enter your desired focal length (in mm)", 80);
    float sensor_size = read_float("Enter your desired sensor size (in mm)", 25.0509582f);
    float world_scale = read_float("Enter your desired world scale (m/unit)", 0.125f);
    
    std::string dof_choice = "";
    bool dof = false;
    int f_stop = 22;
    int focus_distance = 800;
    dof_choice = read_string("Do you wish to simulate Depth of Field ? (y/N)", "N");
    if (dof_choice == "y") {
        dof = true;
        f_stop = read_int("Enter the desired f-stop", 22);
        focus_distance = read_int("Enter the desired focus distance (in m)", 800);
    }

    std::cout << "\nSelect Camera Control Mode:\n";
    std::cout << "1. Manual Control (Press 'P' to take pictures)\n";
    std::cout << "2. Automated: Read coordinates from a text file\n";
    std::cout << "3. Automated: Randomly generated positions\n";
    int mode_choice = read_int("Choice (1, 2, or 3)", 1);


    std::string input_csv = "";
    int random_count = 50;

    if (mode_choice == 2) {
        std::cout << "\n--- Mode 2 Setup ---\n";
        input_csv = read_string("Enter path to input CSV file", "dataset_cameras.csv");
    }
    else if (mode_choice == 3) {
        std::cout << "\n--- Mode 3 Setup ---\n";
        random_count = read_int("Number of random pictures to take", 108);
    }

    std::cout << "\n--- Output Setup ---\n";
    std::string output_folder = read_string("Enter output folder path (subfolder of project directory)", "output_images");
    std::filesystem::create_directories(output_folder);
    std::filesystem::create_directories(output_folder + "/undistorted");
    std::filesystem::create_directories(output_folder + "/distorted");

    std::cout << "\nSaving all data to: " << output_folder << "\n";
    std::cout << "Starting up the 3D scene...\n";


    scene_structure scene;
    scene.capture_width = capture_width;
    scene.capture_height = capture_height;
    scene.focal_length = focal_length;
    scene.sensor_size = sensor_size;
    scene.dof = dof;
    scene.f_stop = f_stop;
    scene.focus_distance = focus_distance;
    scene.world_scale = world_scale;

    scene.auto_settings.camera_mode = mode_choice;
    scene.auto_settings.output_folder = output_folder;
    scene.auto_settings.target_random_images = random_count;
    scene.auto_settings.input_csv_path = input_csv;

    // Start the application
    application_structure app;
    app.initialize(argv[0], &scene);
    app.start_loop();

    return 0;
}