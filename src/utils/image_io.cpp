#include "image_io.hpp"

#include <tiffio.h>
#include <iostream>

void save_tiff_with_metadata(
    const char* filename,
    int width,
    int height,
    const std::vector<unsigned char>& image_data,
    const std::string& metadata)
{
    TIFF* tif = TIFFOpen(filename, "w");
    if (!tif) {
        std::cerr << "Error: Could not open " << filename << " for writing." << std::endl;
        return;
    }

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 3);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 8);
    TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);

    TIFFSetField(tif, TIFFTAG_IMAGEDESCRIPTION, metadata.c_str());

    for (uint32_t row = 0; row < static_cast<uint32_t>(height); ++row) {
        const unsigned char* row_pointer =
            &image_data[row * width * 3];

        TIFFWriteScanline(tif, (void*)row_pointer, row, 0);
    }

    TIFFClose(tif);

    std::cout << "Successfully saved: "
        << filename
        << " with metadata!"
        << std::endl;
}
