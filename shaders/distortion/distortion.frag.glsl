#version 330 core

in vec2 v_texcoord;
out vec4 fragColor;

uniform sampler2D undistorted_texture;

uniform int image_width;
uniform int image_height;

uniform float pixel_size_mm;

uniform float cx_px;
uniform float cy_px;

uniform int inverse_iterations;

// Your synthetic forward-distortion coefficients
uniform float K1;
uniform float K2;
uniform float K3;

// Convert pixel coordinate to mm relative to principal point/image centre
vec2 pixel_to_mm(vec2 pix)
{
    return vec2(
        (pix.x - cx_px) * pixel_size_mm,
        (pix.y - cy_px) * pixel_size_mm
    );
}

// Convert mm coordinate back to pixel coordinate
vec2 mm_to_pixel(vec2 xy_mm)
{
    return vec2(
        xy_mm.x / pixel_size_mm + cx_px,
        xy_mm.y / pixel_size_mm + cy_px
    );
}

// Forward distortion in mm:
// x_d = x_u + dx
// y_d = y_u + dy
vec2 distortion_mm(vec2 xy_mm)
{
    float x = xy_mm.x;
    float y = xy_mm.y;

    float r2 = x*x + y*y;
    float r4 = r2*r2;
    float r6 = r4*r2;

    float radial_factor = K1*r2 + K2*r4 + K3*r6;

    vec2 d;
    d.x = radial_factor * x;
    d.y = radial_factor * y;

    return d;
}

void main()
{
    // Current output pixel in distorted image
    vec2 pix_d = vec2(
        v_texcoord.x * float(image_width),
        v_texcoord.y * float(image_height)
    );

    // Initial inverse guess
    vec2 pix_u = pix_d;

    // Fixed-point inverse mapping
    for (int i = 0; i < inverse_iterations; ++i)
    {
        vec2 xy_mm = pixel_to_mm(pix_u);
        vec2 d_mm = distortion_mm(xy_mm);
        vec2 d_px = d_mm / pixel_size_mm;

        pix_u = pix_d - d_px;
    }

    // Convert source pixel coordinate back to texture coordinate
    vec2 uv_u = vec2(
        pix_u.x / float(image_width),
        pix_u.y / float(image_height)
    );

    // If outside image, output black
    if (uv_u.x < 0.0 || uv_u.x > 1.0 || uv_u.y < 0.0 || uv_u.y > 1.0)
    {
        fragColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
    else
    {
        fragColor = texture(undistorted_texture, uv_u);
    }
}