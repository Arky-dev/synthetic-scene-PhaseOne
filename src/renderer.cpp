#include "renderer.hpp"
#include "camera/distortion.hpp"
#include "utils/image_io.hpp"
#include "utils/metadata.hpp"

using namespace cgp;

void scene_capture::capture_high_res(
    int width, int height,
    const std::string& output_folder,
    const std::string& current_id,
    cgp::camera_projection_perspective& camera_projection,
    cgp::camera_controller_2d_displacement& camera_control,
    environment_structure& environment,
    window_structure& window,
	int focal_length,
	bool dof, 
	int f_stop, 
	int focus_distance,
	float world_scale,
	float pixel_size,
    std::function<void(const environment_structure&)> render_scene_func)
{
	float original_aspect = camera_projection.aspect_ratio;
	camera_projection.aspect_ratio = static_cast<float>(width) / static_cast<float>(height);

	// ------------------------------------------------------------------------
	// 1. Setup Scene Framebuffer (Color Texture + Depth Texture)
	// ------------------------------------------------------------------------
	GLuint fbo_scene, scene_color, scene_depth;
	glGenFramebuffers(1, &fbo_scene);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_scene);

	// Sharp color attachment
	glGenTextures(1, &scene_color);
	glBindTexture(GL_TEXTURE_2D, scene_color);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, scene_color, 0);

	// Depth attachment (Rendered as texture so we can sample it in the shader)
	glGenTextures(1, &scene_depth);
	glBindTexture(GL_TEXTURE_2D, scene_depth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, scene_depth, 0);

	// ------------------------------------------------------------------------
	// 2. Render Sharp Image
	// ------------------------------------------------------------------------
	glViewport(0, 0, width, height);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Instead of calling scene.render_scene(), call the injected function:
    environment.camera_projection = camera_projection.matrix();
    environment.camera_view = camera_control.camera_model.matrix_view();

    render_scene_func(environment);

	// ------------------------------------------------------------------------
	// 3. Setup DoF Post-Processing Framebuffer
	// ------------------------------------------------------------------------
	GLuint fbo_dof, dof_color;
	glGenFramebuffers(1, &fbo_dof);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_dof);

	glGenTextures(1, &dof_color);
	glBindTexture(GL_TEXTURE_2D, dof_color);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dof_color, 0);

	// ------------------------------------------------------------------------
	// 4. Render DoF Pass on GPU
	// ------------------------------------------------------------------------
	// Load shader lazily once to avoid needing it in initialization steps
	static cgp::opengl_shader_structure dof_shader;
	static bool shader_loaded = false;
	static GLuint empty_vao = 0;

	if (!shader_loaded) {
		dof_shader.load(project::path + "shaders/dof/dof_postprocess.vert.glsl", project::path + "shaders/dof/dof_postprocess.frag.glsl");
		glGenVertexArrays(1, &empty_vao);
		shader_loaded = true;
	}

	glUseProgram(dof_shader.id);

	// Set Uniforms
	glUniform1i(glGetUniformLocation(dof_shader.id, "colorTex"), 0);
	glUniform1i(glGetUniformLocation(dof_shader.id, "depthTex"), 1);

	glUniform1f(glGetUniformLocation(dof_shader.id, "z_near"), camera_projection.depth_min);
	glUniform1f(glGetUniformLocation(dof_shader.id, "z_far"), camera_projection.depth_max);
	glUniform1f(glGetUniformLocation(dof_shader.id, "focal_length"), focal_length);
	glUniform1f(glGetUniformLocation(dof_shader.id, "f_number"), f_stop);
	glUniform1f(glGetUniformLocation(dof_shader.id, "focus_dist"), static_cast<float>(focus_distance) * 1000.0f);
	glUniform1f(glGetUniformLocation(dof_shader.id, "pixel_size"), pixel_size);
	glUniform1f(glGetUniformLocation(dof_shader.id, "scene_to_mm"), world_scale * 1000.0f);
	glUniform1i(glGetUniformLocation(dof_shader.id, "max_radius"), 40);
	glUniform2f(glGetUniformLocation(dof_shader.id, "texel_size"), 1.0f / width, 1.0f / height);

	// Bind Textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, scene_color);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, scene_depth);

	glBindVertexArray(empty_vao);

	// Make sure viewport covers the full image so the coordinate space isn't squished
	glViewport(0, 0, width, height);

	// Enable Scissor Test to mask out rendering to horizontal tiles
	glEnable(GL_SCISSOR_TEST);

	const int NUM_TILES = 8;
	int tile_height = height / NUM_TILES;

	for (int i = 0; i < NUM_TILES; ++i) {
		int y_offset = i * tile_height;
		int current_height = (i == NUM_TILES - 1) ? (height - y_offset) : tile_height;

		// Scissor box restricts which pixels are processed (no squishing)
		glScissor(0, y_offset, width, current_height);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		// Force the GPU driver to evaluate this tile immediately
		glFlush();
	}

	// --- BULLETPROOF STATE CLEANUP ---
	// This prevents CGP's internal engine from crashing on the next frame

	glDisable(GL_SCISSOR_TEST);

	// Reset texture states so CGP's internal state tracker doesn't get confused
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Unbind VAO and Shader
	glBindVertexArray(0);
	glUseProgram(0);

	// ------------------------------------------------------------------------
	// 5. Extract Final Image to CPU
	// ------------------------------------------------------------------------
	std::vector<unsigned char> pixels(width * height * 3);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

	// Clean up GPU memory immediately
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &fbo_scene);
	glDeleteTextures(1, &scene_color);
	glDeleteTextures(1, &scene_depth);
	glDeleteFramebuffers(1, &fbo_dof);
	glDeleteTextures(1, &dof_color);

	glViewport(0, 0, window.width, window.height);
	camera_projection.aspect_ratio = original_aspect;

	// ------------------------------------------------------------------------
	// 6. Flip the image vertically (OpenGL vs Image format mismatch)
	// ------------------------------------------------------------------------
	int row_stride = width * 3;
	std::vector<unsigned char> temp_row(row_stride);

	for (int y = 0; y < height / 2; ++y) {
		int top_offset = y * row_stride;
		int bottom_offset = (height - 1 - y) * row_stride;

		std::copy(pixels.begin() + top_offset, pixels.begin() + top_offset + row_stride, temp_row.begin());
		std::copy(pixels.begin() + bottom_offset, pixels.begin() + bottom_offset + row_stride, pixels.begin() + top_offset);
		std::copy(temp_row.begin(), temp_row.end(), pixels.begin() + bottom_offset);
	}

	// ------------------------------------------------------------------------
	// 7. Gather Metadata & Save TIFF
	// ------------------------------------------------------------------------
	cgp::vec3 pos = camera_control.camera_model.position();
	cgp::vec3 front = camera_control.camera_model.front();

	float pitch = std::asin(front.z) * 180.0f / Pi;
	float yaw = std::atan2(front.y, front.x) * 180.0f / Pi;
	float roll = 0.0f;

	// Save undistorted image
	std::string metadata_undistorted = generate_camera_metadata(
		pos, pitch, yaw, roll, camera_projection.field_of_view,
		width, height, distortion_config, false
	);

	std::string undistorted_path = output_folder + "/undistorted/" + current_id + ".tif";

	save_tiff_with_metadata(
		undistorted_path.c_str(), width, height, pixels, metadata_undistorted
	);

	// Apply and save synthetic distortion mapping
	if (distortion_config.enabled) {
		std::cout << "Applying synthetic distortion..." << std::endl;

		std::vector<unsigned char> distorted_pixels = distort_image_inverse_mapping(
			pixels, width, height, distortion_config
		);

		std::string metadata_distorted = generate_camera_metadata(
			pos, pitch, yaw, roll, camera_projection.field_of_view,
			width, height, distortion_config, true
		);

		std::string distorted_path = output_folder + "/distorted/" + current_id + ".tif";

		save_tiff_with_metadata(
			distorted_path.c_str(), width, height, distorted_pixels, metadata_distorted
		);
	}

	// Restore projection matrix state
	environment.camera_projection = camera_projection.matrix();
}