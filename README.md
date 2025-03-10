# Design Manager

A lightweight, single-header library for managing custom UI elements with **Dear ImGui**.

## Features
- **Shape Management**: Define and customize shapes (rectangles, circles, etc.).
- **Child Window Integration**: Manage multiple child windows per shape.
- **Animation & Chain Animations**: Easily animate UI elements.
- **Global Logic Mapping**: Define relationships between UI elements.
- **HTML/CSS to ImGui Conversion (Experimental)**: Convert basic HTML/CSS to ImGui code.

## Installation
### 1. Clone Repository
```sh
git clone https://github.com/yourusername/design_manager.git
cd design_manager
git submodule add https://github.com/ocornut/imgui.git external/imgui
```

### 2. Build Setup (CMake)
```cmake
cmake_minimum_required(VERSION 3.10)
project(DesignManagerApp)

add_subdirectory(external/imgui)
add_executable(DesignManagerApp src/main.cpp)
target_link_libraries(DesignManagerApp PRIVATE imgui)
```

## Usage
### Initialize in `main.cpp`
```cpp
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include "design_manager.h"

int main()
{
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Design Manager", NULL, NULL);
    glfwMakeContextCurrent(window);
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
    
    DesignManager::Init(1280, 720, window);
    
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        DesignManager::ShowUI(window);
        
        ImGui::Render();
        glfwSwapBuffers(window);
    }
    
    glfwTerminate();
    return 0;
}
```

## Project Structure
```
your_project/
├── external/
│   └── imgui/                # Dear ImGui source files
├── src/
│   ├── main.cpp              # Main application file
│   └── design_manager.h      # Single-header Design Manager
├── include/                  # Additional headers
├── build/                    # Build directory
└── README.md                 # This file
```

## HTML/CSS to ImGui (Experimental)
- **Planned Features**:
  - Full HTML parser (Gumbo integration)
  - CSS property mapping
  - Responsive design handling
  - Live UI updates

## Contributing
- Open issues for bugs and suggestions.
- Submit pull requests with a clear description.

## License
[MIT License](LICENSE)

