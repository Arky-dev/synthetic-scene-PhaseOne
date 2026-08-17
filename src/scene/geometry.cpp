#include "geometry.hpp"

using namespace cgp;

void scene_geometry::initialize() {
    global_frame.initialize_data_on_gpu(mesh_primitive_frame());

	// HERE IS WHERE THE 3D SCENE IS CREATED

	// =============  Main Calibration Setup ===============


	// Calibration wall :
		mesh wall_mesh;
		wall_mesh.position = { {-10,-10,0}, { 10,-10,0}, { 10,-10,11.78259217027f}, {-10,-10,11.78259217027f} };
		wall_mesh.uv = { {0,0}, {1,0}, {1,1}, {0,1} };
		wall_mesh.connectivity = { {0,1,2}, {0,2,3} };

		wall_mesh.fill_empty_field();
		wall.initialize_data_on_gpu(wall_mesh);
		wall.shader.load(project::path + "shaders/mesh/mesh.vert.glsl", project::path + "shaders/burst/burst.frag.glsl");

		wall.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/target_wall_physical_smaller.png", // main wall texture
			GL_CLAMP_TO_BORDER,
			GL_CLAMP_TO_BORDER);

		wall_blend.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/background_texture.jpg", // Background texture for tie points
			GL_CLAMP_TO_BORDER,
			GL_CLAMP_TO_BORDER
		);

		wall.material.phong.ambient = 0.5f;
		wall.material.phong.diffuse = 0.9f;
		wall.material.phong.specular = 0.1f;

	// Second Calibration wall : 
		mesh wall_mesh_2;
		wall_mesh_2.position = { {-2,-8,5}, { 2,-8,5}, { 2,-8,7.6308594f}, {-2,-8,7.6308594f} };
		wall_mesh_2.uv = { {0,0}, {1,0}, {1,1}, {0,1} };
		wall_mesh_2.connectivity = { {0,1,2}, {0,2,3} };

		wall_mesh_2.fill_empty_field();
		wall2.initialize_data_on_gpu(wall_mesh_2);
		wall2.shader.load(project::path + "shaders/mesh/mesh.vert.glsl", project::path + "shaders/burst/burst.frag.glsl");

		wall2.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/medium_targets_2.png", // main wall texture
			GL_CLAMP_TO_BORDER,
			GL_CLAMP_TO_BORDER);

		wall2_blend.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/rock.jpg", // Background texture for tie points
			GL_CLAMP_TO_BORDER,
			GL_CLAMP_TO_BORDER
		);

		wall2.material.phong.ambient = 0.5f;
		wall2.material.phong.diffuse = 0.9f;
		wall2.material.phong.specular = 0.1f;


	// Additionnal Calibration lines :
		std::string filenames[4] = { "target_page_071.png", "target_page_072.png", "target_page_073.png", "target_page_074.png" };
		cgp::numarray<cgp::vec3> positions[6] = { { {-10,-8,0}, { -9.4043337f,-8,0}, { -9.4043337f,-8,13.154297f}, {-10,-8,13.154297f} },
												  { {-10,-6,0}, { -9.4043337f,-6,0}, { -9.4043337f,-6,13.154297f}, {-10,-6,13.154297f} },
												  { {10,-8,0}, { 9.4043337f,-8,0}, { 9.4043337f,-8,13.154297f}, {10,-8,13.154297f} },
												  { {10,-6,0}, { 9.4043337f,-6,0}, { 9.4043337f,-6,13.154297f}, {10,-6,13.154297f} },
		};

		for (int i = 0; i < 4; i++) {
			mesh line_mesh;
			line_mesh.position = positions[i];
			line_mesh.uv = { {0,0}, {1,0}, {1,1}, {0,1} };
			line_mesh.connectivity = { {0,1,2}, {0,2,3} };

			line_mesh.fill_empty_field();
			lines[i].initialize_data_on_gpu(line_mesh);
			lines[i].shader.load(project::path + "shaders/mesh/mesh.vert.glsl", project::path + "shaders/burst/burst.frag.glsl");

			lines[i].texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/target_lines/" + filenames[i], // main wall texture
				GL_CLAMP_TO_BORDER,
				GL_CLAMP_TO_BORDER);

			lines_blend[i].texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/background_texture.jpg", // Background texture for tie points
				GL_CLAMP_TO_BORDER,
				GL_CLAMP_TO_BORDER
			);

			lines[i].material.phong.ambient = 0.5f;
			lines[i].material.phong.diffuse = 1.f;
			lines[i].material.phong.specular = 0.1f;
		}


	// =============  Non Calibration Objects ===============


	// Floor :
		mesh quadrangle_mesh;
		quadrangle_mesh.position = { {-10,-10,0}, { 10,-10,0}, { 10, 100,0}, {-10, 100,0} };
		quadrangle_mesh.uv = { {0,0}, {1,0}, {1,1}, {0,1} };
		quadrangle_mesh.connectivity = { {0,1,2}, {0,2,3} };

		quadrangle_mesh.fill_empty_field();
		shape.initialize_data_on_gpu(quadrangle_mesh);

		shape.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/bricks.jpg",
			GL_CLAMP_TO_BORDER,
			GL_CLAMP_TO_BORDER);

	// Background
		mesh background_mesh;
		background_mesh.position = { {-100,-11,-50}, { 100,-11,-50}, { 100,-11, 50}, {-100, -11, 50} };
		background_mesh.uv = { {0,0}, {1,0}, {1,1}, {0,1} };
		background_mesh.connectivity = { {0,1,2}, {0,2,3} };

		background_mesh.fill_empty_field();
		background.initialize_data_on_gpu(background_mesh);

		background.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/ground.jpg",
			GL_CLAMP_TO_BORDER,
			GL_CLAMP_TO_BORDER);

	// Left wall
		mesh leftwall_mesh;
		leftwall_mesh.position = { {-100,-11,-50}, { -100,10,-50}, { -100,10, 50}, {-100, -11, 50} };
		leftwall_mesh.uv = { {0,0}, {1,0}, {1,1}, {0,1} };
		leftwall_mesh.connectivity = { {0,1,2}, {0,2,3} };

		leftwall_mesh.fill_empty_field();
		leftwall.initialize_data_on_gpu(leftwall_mesh);

		leftwall.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/leftwall.jpeg",
			GL_CLAMP_TO_BORDER,
			GL_CLAMP_TO_BORDER);

	// Right Wall
		mesh rightwall_mesh;
		rightwall_mesh.position = { {100,-11,-50}, { 100,10,-50}, { 100,10, 50}, {100, -11, 50} };
		rightwall_mesh.uv = { {0,0}, {1,0}, {1,1}, {0,1} };
		rightwall_mesh.connectivity = { {0,1,2}, {0,2,3} };

		rightwall_mesh.fill_empty_field();
		rightwall.initialize_data_on_gpu(rightwall_mesh);

		rightwall.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/rightwall.jpg",
			GL_CLAMP_TO_BORDER,
			GL_CLAMP_TO_BORDER);

	// Ceiling
		mesh ceiling_mesh;
		ceiling_mesh.position = { {-100,-11,50}, { 100,-11,50}, { 100,11, 50}, {-100, 11, 50} };
		ceiling_mesh.uv = { {0,0}, {1,0}, {1,1}, {0,1} };
		ceiling_mesh.connectivity = { {0,1,2}, {0,2,3} };

		ceiling_mesh.fill_empty_field();
		ceiling.initialize_data_on_gpu(ceiling_mesh);

		ceiling.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/ceiling.jpg",
			GL_CLAMP_TO_BORDER,
			GL_CLAMP_TO_BORDER);
	
}

void scene_geometry::render(const environment_structure& env) {
    draw(shape, env);

    // --- Wall Multi-texturing ---

	// Wall 0
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, wall_blend.texture.id);
    glUseProgram(wall.shader.id);
    glUniform1i(glGetUniformLocation(wall.shader.id, "base_texture"), 0);
    glUniform1i(glGetUniformLocation(wall.shader.id, "replacement_texture"), 1);
    glActiveTexture(GL_TEXTURE0);

	// Wall 1
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, wall2_blend.texture.id);
	glUseProgram(wall2.shader.id);
	glUniform1i(glGetUniformLocation(wall2.shader.id, "base_texture"), 0);
	glUniform1i(glGetUniformLocation(wall2.shader.id, "replacement_texture"), 1);
	glActiveTexture(GL_TEXTURE0);

    // Lines
    for (int i = 0; i < 4; i++) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, lines_blend[i].texture.id);
        glUseProgram(lines[i].shader.id);
        glUniform1i(glGetUniformLocation(lines[i].shader.id, "base_texture"), 0);
        glUniform1i(glGetUniformLocation(lines[i].shader.id, "replacement_texture"), 1);
        glActiveTexture(GL_TEXTURE0);
    }

	draw(wall, env);
	//draw(wall2, env);
    draw(background, env);
    draw(leftwall, env);
    draw(rightwall, env);
    draw(ceiling, env);
	for (int i = 0; i < 4; i++) {
		// draw(lines[i], env);
	}

}