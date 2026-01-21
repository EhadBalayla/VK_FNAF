# VK_FNAF

a Five Nights at Freddy's fangame made using C and Vulkan, in development obviously but yeah.

# How to install

## Linux (Ubuntu/Debian)
make sure you have Make installed, CMake (version 3.16 minimum) and GIT installed.

- download the Vulkan development libraries
```bash
    sudo apt-get install libvulkan-dev vulkan-validationlayers-dev spirv-tools
```
- download GLFW3
```bash
    sudo apt-get install libglfw3-dev
```
- download FreeType
```bash
    sudo apt-get install libfreetype-dev
```

-then clone the repo
```bash
    git clone https://github.com/EhadBalayla/VK_FNAF.git
```

-then open a terminal inside the folder of the repo and run
```bash
    ./BuildRelease.sh
```
which will run the script to build Cmake with release build type and compile with make and open the game

## Windows
make sure you have Visual Studio installed (with the "Desktop development with C++" package), and also CMake (version 3.16 minimum) and GIT installed.

download the Vulkan SDK (the Windows version specifically) - https://vulkan.lunarg.com/sdk/home

also make sure you have VCPKG installed (follow only the 1st step of this installation - https://learn.microsoft.com/en-us/vcpkg/get_started/get-started?pivots=shell-cmd#1---set-up-vcpkg)

open a CMD in VCPKG's root folder and type this command:
```bash
    vcpkg.exe install glfw3 freetype --triplet=x64-windows-static
```

then clone the repo
```bash
    git clone https://github.com/EhadBalayla/VK_FNAF.git
```

then open a CMD inside the folder of the repo and type this command:
```bash
    cmake -B build -DCMAKE_TOOLCHAIN_FILE=[VCPKG_ROOT]/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows-static -DCMAKE_PREFIX_PATH=[VCPKG_ROOT]/installed/x64-windows-static/
```
and make sure to replace every [VCPKG_ROOT] with the path to the root folder of VCPKG

and then after that in the same CMD type this command:
```bash
    cmake --build build --config release
```

then open the visual studio repository inside the generated build folder inside the folder of the repository and compile the ALL_BUILD project inside it

and then in [REPO_FOLDER]/Game/Compiled there should be a "Release" folder with the EXE inside it, move the EXE into the "Compiled" folder above.

## MacOS
IDK i dont use it and also fuck MacOS and fuck Apple i'd rather use Windows 11 than being stuck in Apple's ecosystem
