#version 330 core

// 1. Inputs from the CGP mesh buffers
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord; // CGP usually puts UVs at location 2

// 2. Output to send to your burst.frag.glsl
out vec2 uv;

// 3. Standard Camera and Model matrices sent by CGP's draw() function
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    // Pass the texture coordinates directly to the fragment shader
    uv = texcoord;

    // Calculate the final screen position of the vertex
    gl_Position = projection * view * model * vec4(position, 1.0);
}