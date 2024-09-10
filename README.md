# Aether Engine

Aether Engine is a cross-platform, multi-API rendering engine designed to support both DirectX and Vulkan. It aims to provide a flexible and powerful foundation for game development. The project is very early days at the moment, as I aim to first port my old DX11/WIN32 game engine, before beginning to abstract out all platform-dependencies and then work on a Vulkan backend for the engine.

## Features

- **Cross-Platform:** Supports Windows, with plans to also support Linux, and macOS.
- **Multiple Rendering APIs:** Initial support for DirectX 11 with plans for Vulkan integration.
- **Modular Architecture:** Easily extendable with additional features and APIs.
- **Open Source:** Encouraging community contributions under the Apache License 2.0.

## Build Instructions

### Prerequisites

- **CMake** (version 3.15 or higher)
- **Ninja** (optional, for faster builds)
- **Visual Studio** (Windows) or **Clang/GCC** (Linux/macOS)
- **DirectX SDK** (Windows)
- **Vulkan SDK** (optional, for Vulkan support)

### Steps

1. **Clone the Repository:**

   ```sh
   git clone https://github.com/yourusername/AetherEngine.git
   cd AetherEngine
   ```

2. **Create a Build Directory:**
   ```sh
    mkdir build
    cd build
   ```

3. **Generate Build Files:**
   ```sh
    cmake -G "Ninja" .. # Ninja is specific to Windows, but allows for faster builds
   ```

3. **Build the Project:**
    ```sh
    ninja
    ```

3. **Run the Executable:**
    ```sh
    ./EngineApp/AetherEngine.exe
    ```

## Usage

Currently, the engine initializes a basic DirectX 11 renderer and creates a window with GLFW. Future updates will include more features and rendering capabilities.

## License

This project is licensed under the Apache License 2.0.
   

# Contributing to Aether Engine

Thank you for considering contributing to Aether Engine! I want this project to foster an open-source community, however it is currently incredibly early days for the project. Please instead add this to your watch for now, and I will send out an update once the project is far enough along that people can make more meaningful contributions!
