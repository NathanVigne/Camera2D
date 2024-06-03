# Camera2D

Camera2D is a Qt-based application designed for live viewing of scientifics camera.

## Installation

### Dependencies

Before installing Camera2D, make sure you have the following dependencies installed:

- [Qt](https://www.qt.io/)
- [GSL (GNU Scientific Library)](https://www.gnu.org/software/gsl/)
- [CMake](https://cmake.org/)

### Installation Steps

1. **Clone the Repository**: Clone the Camera2D repository to your local machine using Git:

    ```bash
    git clone https://github.com/NathanVigne/Camera2D.git
    ```

## Usage

- TO DO

## Contributing

Contributions are welcome! If you find any bugs or have suggestions for improvements, please open an issue or create a pull request.

### Adding support for another Camera API

1. Create a new Cmake library project targeting your new API

```cmake
add_library(NewAPI STATIC
    NewAPI.cpp
    NewAPI.h
)
target_link_libraries(NewAPI PRIVATE Qt6::Core NewAPI_lib)
target_include_directories(NewAPI INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(../app)
```

2. Inherit and implement the Camera interface "./app/ICamera.h" inside your new project

```cpp
#include "../app/icamera.h"
class NewAPI : public ICamera {};
```

3. Add support for your new API inside "./app/CameraManager.h"

```cpp
#include "NewAPI.h"
enum CAMERATYPE { THORLABS,
                  MIGHTEX,
                  ALLIEDVISION,
                  NEW_API,
                  CAMTYPE_LAST };
```

3. Add support for your new API inside "./app/CameraManager.cpp". Add support in function DiscoverCameras() and CameraConnect()

4. All set!

## License

This project is licensed under the [GPL License](LICENSE).

