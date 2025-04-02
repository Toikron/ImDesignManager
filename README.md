# ImGui Visual Design Manager

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
<!-- Add other relevant badges here, e.g., build status, version -->

**ImGui Visual Design Manager is a single-header C++ library that adds a visual design workflow to Dear ImGui.**

**It lets you build and arrange UI components interactively—similar to tools like Figma—directly within your ImGui-based application. Designed components can be exported and used as standard ImGui elements with minimal code changes.**

![Showcase GIF - Replace with a good GIF showing the editor in action](https://github.com/user-attachments/assets/cf74a7a0-8bb4-42a8-8feb-1e3d250f013e)

*(Caption: Visually design shapes, layers, and properties directly within your ImGui application)*

---

## Key Features

*   🎨 **Figma-like Visual Canvas:**
    *   Design UI layouts using **shapes** (rectangles, circles) with customizable properties (position, size, rotation, corner radius, fill, border, shadow).
    *   Organize elements using a familiar **layer system** with Z-ordering, visibility, and locking.
    *   Style elements with solid colors, **gradients** (linear, radial - configurable interpolation), and **embedded images**.
    *   Add and style **text** elements with font selection, alignment, and rotation.
*   🔗 **Bridge Design and Code:**
    *   Instantly transform visual shapes into functional **ImGui Buttons** with configurable behavior (Toggle, Hold, SingleClick) and styling (Hover, Clicked colors).
    *   Attach **ImGui Child Windows** to shapes, allowing you to embed complex ImGui content within your designed layouts. Child windows can optionally sync position/size with their parent shape.
    *   Trigger standard ImGui windows or custom logic from designed buttons.
*   ✨ **Dynamic & Responsive UIs:**
    *   Create sophisticated **animations** on shapes triggered by clicks or hovers. Define behaviors like play-once, reverse, toggle, loop, and play-while-holding.
    *   Build complex **chained animations**, sequencing multiple animation steps for intricate UI transitions.
    *   Implement responsive layouts using **Shape Keys**, automatically adjusting position, size, or rotation based on the parent window's dimensions.
*   🧩 **Modular & Organized Workflow:**
    *   Manage multiple independent design canvases, each associated with a specific ImGui window (`DesignManager::g_windowsMap`).
    *   Group buttons and child windows for **exclusive visibility** (e.g., tabbed interfaces).
    *   Define **logical relationships** between button states and child window visibility (AND, OR, XOR, IF, etc.).
*   ⚙️ **Developer Focused:**
    *   **Built-in Visual Editor:** Use `DesignManager::ShowUI()` to access the powerful editor for real-time design and modification.
    *   **Code Generation:** Automatically generate C++ code (using a clean Builder pattern) representing your visual designs, ready to be integrated into your project.
    *   **Single-Header Library:** Easy integration - just drop `design_manager.h` into your project.
    *   **Parenting System:** Group shapes hierarchically for complex component creation and coordinated transformations.
*   *   **Easy to Implement SDL, DirectX. Just change in order GLFWwindow* to SDL_Window* and glfwGetWindowSize to SDL_GetWindowSize . Seamless Integration 🧩
## Why Use ImGui Visual Design Manager?

*   **Rapid Prototyping:** Visually iterate on UI designs much faster than coding them manually.
*   **Designer-Friendly:** Empowers developers (and potentially designers) with a visual workflow within the target environment.
*   **Seamless Integration:** Designs translate directly to functional ImGui code and widgets.
*   **Consistency:** Maintain a consistent look and feel by designing reusable components visually.
*   **Dynamic Interfaces:** Easily add complex animations and responsiveness without extensive manual coding.

## Visual Examples

*(These are the images you provided - captions added for context)*

<img src="https://github.com/user-attachments/assets/05cb57a1-e197-4d94-837d-e9476407a33d" width="200"> | <img src="https://github.com/user-attachments/assets/dac9e7ff-de4f-4e55-bd82-624fe03e172d" width="200"> | <img src="https://github.com/user-attachments/assets/02a6c4fe-5572-44ae-848b-80da43185e2c" width="200"> |
|---|---|---|
| |  |  |
| <img src="https://github.com/user-attachments/assets/adf6fc34-cc35-462c-b7f9-7b2597c3c1ab" width="200"> | <img src="https://github.com/user-attachments/assets/36e5171d-ec67-43bd-993c-13f0aa7aca7f" width="200"> | <img src="https://github.com/user-attachments/assets/40365b18-1b8b-4632-adad-79bf04e08d23" width="200"> |
|  |  |  |

## Getting Started

### Installation

1.  **Clone the repository:**
    ```sh
    git clone https://github.com/yourusername/design_manager.git # Replace with your actual repo URL
    cd design_manager
    ```
2.  **Add Dear ImGui as a submodule (or ensure it's available):**
    ```sh
    # If you haven't already got ImGui in your project
    git submodule add https://github.com/ocornut/imgui.git external/imgui
    git submodule update --init --recursive
    ```
3.  **Include the header:** Simply include `design_manager.h` in your C++ project. Ensure Dear ImGui's headers are also accessible.

### Basic Usage

1.  **Initialize DesignManager:** Call `DesignManager::Init()` after setting up ImGui and your rendering backend (like GLFW/OpenGL).
2.  **Load Your Design:** Call `DesignManager::GeneratedCode()` (or your custom function that uses the generated code) to load the UI elements you created with the visual editor.
3.  **Update & Draw:** In your main loop:
    *   Call `DesignManager::UpdateShapeTransforms_Unified()` to handle responsiveness (Shape Keys).
    *   Call `DesignManager::UpdateChainAnimations()` to process chained animations.
    *   Call `DesignManager::DrawAll(dl)` within the target ImGui window's `Begin`/`End` block to render the designed elements.
    *   (Optional) Call `DesignManager::ShowUI()` to display the visual editor itself.
    *   (Optional) Call `DesignManager::ShowChainAnimationGUI()` to manage chained animations.

```cpp
// --- main.cpp Example Snippet ---
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h> // Or your backend header
#include "design_manager.h" // Include the library

// Your existing ImGui setup code... (Context creation, backend init)

int main() {
    // ... GLFW/Window setup ...
    glfwMakeContextCurrent(window);

    // ... ImGui backend setup (ImGui_ImplGlfw_Init*, ImGui_ImplOpenGL3_Init*) ...

    // Initialize the Design Manager
    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);
    DesignManager::Init(windowWidth, windowHeight, window);

    // Load the design created visually (this function is generated/filled by the editor)
    DesignManager::GeneratedCode(); // IMPORTANT!

    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // --- Update Design Manager ---
        double currentTime = glfwGetTime();
        float deltaTime = (float)(currentTime - lastTime);
        lastTime = currentTime;
        DesignManager::UpdateShapeTransforms_Unified(window, deltaTime); // Handles Shape Keys/Responsiveness
        DesignManager::UpdateChainAnimations(deltaTime); // Handles chained animations

        // --- Show the Visual Editor (Optional) ---
        DesignManager::ShowUI(window); // The main editor window
        DesignManager::ShowChainAnimationGUI(); // Editor for chained animations

        // --- Render Your Main Application Window ---
        ImGuiViewport* main_viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(main_viewport->Pos);
        ImGui::SetNextWindowSize(main_viewport->Size);
        ImGuiWindowFlags main_window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNavFocus;
        ImGui::Begin("MainAppWindow", NULL, main_window_flags); // Your main window

        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        // --- Draw Designed Elements ---
        // This draws shapes/buttons/etc. belonging to "MainAppWindow"
        DesignManager::DrawAll(draw_list);

        ImGui::End(); // End your main window

        // --- Render other registered ImGui windows (including those opened by designed buttons) ---
        DesignManager::RenderAllRegisteredWindows(); // Handles windows defined via RegisterWindow
        DesignManager::RenderTemporaryWindows(); // Handles temporary windows for debugging

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    // ... ImGui_ImplOpenGL3_Shutdown, ImGui_ImplGlfw_Shutdown, ImGui::DestroyContext ...
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

// --- Example of using the Builder Pattern (if NOT using visual editor output) ---
void SetupMyProgrammaticUI() {
    // Get the layer you want to add to (assuming "Main" window and layer 0 exist)
    if (!DesignManager::g_windowsMap["Main"].layers.empty()) {
        DesignManager::Layer& targetLayer = DesignManager::g_windowsMap["Main"].layers[0];

        auto coolButton = DesignManager::ShapeBuilder()
            .setName("ProgrammaticButton")
            .setOwnerWindow("Main") // Associate with the correct ImGui window
            .setBasePosition({ 50, 50 })
            .setBaseSize({ 180, 40 })
            .setFillColor({ 0.1f, 0.8f, 0.2f, 1.0f })
            .setCornerRadius(8.0f)
            .setIsButton(true)
            .setHoverColor({ 0.2f, 0.9f, 0.3f, 1.0f })
            .setClickedColor({ 0.05f, 0.7f, 0.1f, 1.0f })
            .setHasText(true)
            .setText("I was made in code!")
            .setTextColor({ 1, 1, 1, 1 })
            .setTextAlignment(1) // Center alignment
            .addEventHandler("onClick", "MyButtonClickHandler", [](DesignManager::ShapeItem& shape) {
                printf("Button '%s' (ID: %d) clicked!\n", shape.name.c_str(), shape.id);
             })
            .build();

        targetLayer.shapes.push_back(coolButton);
    }
}
```

## Core Concepts

*   **Shapes (`ShapeItem`):** The fundamental visual building blocks. Can be styled, positioned, animated, and turned into interactive elements.
*   **Layers:** Organize shapes within a window, controlling draw order and visibility.
*   **Windows (`g_windowsMap`):** Each key represents an ImGui window context containing its own layers and shapes. This allows designing UIs for different parts of your application.
*   **Visual Editor (`ShowUI`):** The primary interface for designing. Manipulate shapes, layers, properties, animations, and generate the corresponding C++ code.
*   **Builder Pattern (`ShapeBuilder`, etc.):** Provides a fluent API for creating and configuring shapes, animations, and shape keys programmatically.
*   **Code Generation:** The editor outputs C++ code using the builder pattern, typically into a function like `DesignManager::GeneratedCode()`, which you call at initialization.

## 🧪 Experimental: HTML/CSS/React to ImGui Parser

This library includes a highly experimental feature aimed at bridging the gap between web design mockups and ImGui interfaces.

*   **Goal:** Parse simple HTML/CSS or potentially even React/TSX component structures and attempt to translate them into ImGui Visual Design Manager elements.
*   **Current State:** **Ultra Mega Super Alpha Beta Gamma Zeta** (as the original author put it!) - This is **highly experimental and incomplete**.
*   **Planned Features (Long Term):**
    *   HTML parsing (using Gumbo or similar).
    *   Basic CSS property mapping (layout, color, text).
    *   Potential TypeScript/React code analysis for component structure.
    *   Handling basic responsive concepts.
*   **Contributions Welcome:** This area needs significant development. If you have experience with parsing, compilers, or web technologies, your help would be invaluable!

## Contributing

Contributions are highly welcome! Whether it's bug fixes, feature enhancements, documentation improvements, or tackling the experimental parser, please feel free to:

1.  **Open an Issue:** Discuss bugs, suggest features, or ask questions.
2.  **Submit a Pull Request:**
    *   Fork the repository.
    *   Create a new branch for your feature or fix.
    *   Make your changes.
    *   Ensure your code compiles and adheres to the project's style.
    *   Write a clear description of your changes in the pull request.

**Areas needing help:**

*   Improving the experimental HTML/CSS parser.
*   Adding more shape types and styling options.
*   Optimizing rendering performance.
*   Writing more examples and documentation.
*   Testing across different platforms and backends.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
