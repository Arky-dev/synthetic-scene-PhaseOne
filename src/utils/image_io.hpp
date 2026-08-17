#pragma once

#include <string>
#include <vector>

void save_tiff_with_metadata(
    const char* filename,
    int width,
    int height,
    const std::vector<unsigned char>& image_data,
    const std::string& metadata
);