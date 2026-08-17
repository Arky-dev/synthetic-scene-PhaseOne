#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D colorTex;
uniform sampler2D depthTex;

// Camera parameters
uniform float z_near;
uniform float z_far;
uniform float focal_length;
uniform float f_number;
uniform float focus_dist;
uniform float pixel_size;
uniform float scene_to_mm;
uniform int max_radius;
uniform vec2 texel_size;

float linearize_depth(float d) {
    float z_ndc = 2.0 * d - 1.0;
    return (2.0 * z_near * z_far) / (z_far + z_near - z_ndc * (z_far - z_near));
}

float get_blur_radius(float depth_val) {
    float z_scene = linearize_depth(depth_val);
    float z_mm = z_scene * scene_to_mm;

    float coc_mm = (focal_length * focal_length * abs(z_mm - focus_dist)) /
                   (f_number * z_mm * (focus_dist - focal_length));

    float pupil_magnification = 1.0;
    coc_mm /= pupil_magnification;

    float circular_correction = 0.866;
    float r = ((coc_mm / pixel_size) / 2.0) * circular_correction;
    
    return min(r, float(max_radius));
}

void main() {
    float center_depth = texture(depthTex, TexCoords).r;
    float center_r = get_blur_radius(center_depth);
    
    vec3 color_sum = vec3(0.0);
    float weight_sum = 0.0;

    // Search bounding box capped by MAX_RADIUS
    for(int y = -max_radius; y <= max_radius; ++y) {
        for(int x = -max_radius; x <= max_radius; ++x) {
            
            vec2 offset = vec2(float(x), float(y)) * texel_size;
            vec2 sample_uv = TexCoords + offset;

            // Prevent sampling outside the image
            if(sample_uv.x < 0.0 || sample_uv.x > 1.0 || sample_uv.y < 0.0 || sample_uv.y > 1.0) {
                continue;
            }

            float sample_depth = texture(depthTex, sample_uv).r;
            float neighbor_r = get_blur_radius(sample_depth);

            // DEPTH-AWARE LOGIC: 
            // In OpenGL standard depth, a smaller value means it is closer.
            float effective_r = (sample_depth < center_depth) ? neighbor_r : center_r;
            float dist = length(vec2(float(x), float(y)));

            if(dist <= effective_r && effective_r > 0.0) {
                vec3 sample_color = texture(colorTex, sample_uv).rgb;
                
                // Convert sRGB to Linear Space (~2.2 gamma)
                sample_color = pow(sample_color, vec3(2.2));

                color_sum += sample_color;
                weight_sum += 1.0;
            }
        }
    }

    vec3 final_color = texture(colorTex, TexCoords).rgb; // Fallback sharp pixel
    
    if(weight_sum > 0.0) {
        // Convert back to sRGB
        final_color = pow(color_sum / weight_sum, vec3(1.0 / 2.2));
    }

    FragColor = vec4(final_color, 1.0);
}