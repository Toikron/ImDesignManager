# ImGui based Design Manager

A lightweight, single-header library for managing custom UI elements with **Dear ImGui**.

## Features
- **Shape Management**: Define and customize shapes (rectangles, circles, etc.).
- **Child Window Integration**: Manage multiple child windows per shape.
- **Animation & Chain Animations**: Easily animate UI elements.
- **Global Logic Mapping**: Define relationships between UI elements.
  
<img src="https://github.com/user-attachments/assets/05cb57a1-e197-4d94-837d-e9476407a33d" width="200"> | <img src="https://github.com/user-attachments/assets/dac9e7ff-de4f-4e55-bd82-624fe03e172d" width="200"> | <img src="https://github.com/user-attachments/assets/02a6c4fe-5572-44ae-848b-80da43185e2c" width="200"> |
|---|---|---|
| <img src="https://github.com/user-attachments/assets/adf6fc34-cc35-462c-b7f9-7b2597c3c1ab" width="200"> | <img src="https://github.com/user-attachments/assets/36e5171d-ec67-43bd-993c-13f0aa7aca7f" width="200"> | <img src="https://github.com/user-attachments/assets/40365b18-1b8b-4632-adad-79bf04e08d23" width="200"> |
|---|---|---|




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
Whatever you like actually but here is mine.

your_project/
├── external/
│   └── imgui/                # Dear ImGui source files
│       ├── main.cpp              # Main application file
├── src/
│   └── design_manager.h      # Single-header Design Manager
├── include/                  # Additional headers
└── README.md                 # This file
```

## HTML/CSS OR React to ImGui (Experimental Ultra Mega Super Alpha Beta Gamma Zeta)
- **Planned Features**:
  - Full HTML parser (Gumbo integration)
  - CSS property mapping
  - Typescript react code to Design manager.
  - Responsive design handling
  - Live UI updates for parser

## Contributing
- Open issues for bugs and suggestions.
- Submit pull requests with a clear description.

## License
[MIT License](LICENSE)

