# ImGui based Design Manager

A lightweight, single-header library for managing custom UI elements with **Dear ImGui**.

## Features
- **Shape Management**: Define and customize shapes (rectangles, circles, etc.).
- **Child Window Integration**: Manage multiple child windows per shape.
- **Animation & Chain Animations**: Easily animate UI elements.
- **Global Logic Mapping**: Define relationships between UI elements.

## Installation
### 1. Clone Repository
```sh
git clone https://github.com/yourusername/design_manager.git
cd design_manager
git submodule add https://github.com/ocornut/imgui.git external/imgui
```
### 2. Just include the design_manager.h file.

## Usage
### Initialize in `main.cpp`
```cpp
int main()
{
     IMGUI_CHECKVERSION();
     ImGui::CreateContext();

     GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "EduMektep", nullptr, nullptr);
     if (window == nullptr) {
         glfwTerminate();
         return 1;
     }
    glfwMakeContextCurrent(window);

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
    
    DesignManager::Init(windowWidth, windowHeight, window);
    DesignManager::GeneratedCode();
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        double currentTime = glfwGetTime();
        float deltaTime = (float)(currentTime - lastTime);
        lastTime = currentTime;
        ImGuiViewport* main_viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(main_viewport->Pos);
        ImGui::SetNextWindowSize(main_viewport->Size);
        ImGui::Begin("MainWindow", NULL,
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoFocusOnAppearing |
            ImGuiWindowFlags_NoNavFocus |
            ImGuiWindowFlags_NoBringToFrontOnFocus
        );
        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImVec2 wp = ImGui::GetWindowPos();
        ImVec2 ws = ImGui::GetWindowSize();
        float scale = 1.0f;
        DesignManager::UpdateShapeTransforms_Unified(window, deltaTime);
        DesignManager::UpdateChainAnimations(deltaTime);
        DesignManager::ShowChainAnimationGUI();
        DesignManager::ShowUI(window);
        DesignManager::DrawAll(dl);
        
        
        for (auto& func : DesignManager::windowRenderFunctions) {
            func();
        }
        
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
│       ├── main.cpp              # Main application file
├── src/
│   └── design_manager.h      # Single-header Design Manager
├── include/                  # Additional headers
└── README.md                 # This file
```

## HTML/CSS to ImGui (Experimental Ultra Mega Super Alpha Beta Gamma Zeta)
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

