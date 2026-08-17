# Synthetic Scene Creation Pipeline



This project is a C++/OpenGL synthetic scene generation software developed to evaluate photogrammetric calibration accuracy and repeatability for Phase One A/S. It aims to isolate optimization algorithms from physical environmental noise, such as thermal drift and sensor noise, by rendering high-resolution synthetic image sets with custom optical blur, lens distortion, and automated EXIF metadata injection.



### Prerequisites

To build and run this pipeline, your system requires the following:

* Standard C++17 compilation tools
* CMake ($\\ge 3.14$)
* OpenGL 3.3 drivers
* GLFW3
* libtiff (for uncompressed 24-bit RGB TIFF export)

