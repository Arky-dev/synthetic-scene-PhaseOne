#version 330 core

// 1. This struct MUST match exactly what CGP's mesh.vert.glsl outputs
in struct fragment_data
{
    vec3 position;
    vec3 normal;
    vec3 color;
    vec2 uv;
} fragment;

out vec4 FragColor;

// 2. Your textures
uniform sampler2D base_texture;        
uniform sampler2D replacement_texture; 

void main()
{
    // 3. Notice we are using `fragment.uv` now instead of just `uv`
    vec4 base = texture(base_texture, fragment.uv);
    vec4 replacement = texture(replacement_texture, fragment.uv);

    // Detect white pixels
    float threshold = 0.95;

    bool is_white =
        base.r > threshold &&
        base.g > threshold &&
        base.b > threshold;

    if (is_white){
        // --- THE ATTENUATION FACTOR ---
        // 0.0 = 100% Original Image (White)
        // 1.0 = 100% Replacement Image
        // 0.35 = 35% Replacement, 65% Original
        float blend_strength = 0.35; 
        
        // mix(A, B, factor) smoothly blends from A to B
        FragColor = mix(base, replacement, blend_strength);
        }
    else
        FragColor = base;
}