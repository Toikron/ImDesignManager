# Dear-Design-Manager

Design Manager
Design Manager is a lightweight, single-header library that helps you create and manage custom UI elements with Dear ImGui. It provides a straightforward builder API for shapes, child windows, and buttons, and includes features like multi-child window integration, chain animations, and an experimental HTML/CSS-to-ImGui code converter. Thanks to this library, you can design interactive UIs without getting bogged down in tons of boilerplate code.

Note: Design Manager depends on Dear ImGui. Make sure you have Dear ImGui set up as an external dependency in your project.

Features
Shape Management
Easily define shapes (like rectangles or circles) and adjust properties such as position, size, rotation, border styling, and color.

Child Window Integration
Manage multiple child windows for each shape. You can use combo boxes on buttons to decide which child window to open.

Animation & Chain Animations
Built-in builder classes let you animate shapes and buttons, even allowing you to chain multiple animations together.

Global Logic Mapping
Set up logical relationships between buttons (using AND, OR, XOR, NAND, IF_THEN, IFF, and so on) so child windows open, close, or toggle automatically based on these rules.

HTML/CSS to ImGui Conversion (Experimental)
A work-in-progress module that converts basic HTML/CSS code into matching ImGui code through a simple parser.

To Do:

 Robust HTML Parser: Integrate a full-featured HTML parser (e.g., Gumbo) to handle nested and possibly malformed HTML correctly.
 Comprehensive CSS Parser: Add or develop a parser for CSS that covers a wide array of properties (including responsive design, media queries, and advanced selectors).
 Mapping Engine: Build a thorough mapping layer that translates HTML elements (like div, button, img) and CSS properties (such as width, height, background-color, border, margin, and padding) into ImGui builder calls.
 Support for Nested Structures: Make sure nested elements show up properly in ImGui layouts, preserving parent-child relationships.
 Responsive Layouts: Implement logic for responsive design—similar to web layouts—that supports percentage-based sizes and media queries.
 Dynamic Updates: Allow live UI updates when the HTML/CSS source changes (like hot-reloading in many modern web frameworks).
 Error Handling and Warnings: Provide helpful messages or warnings for unsupported or partially implemented HTML/CSS features.
 Extensibility: Make the converter easy to extend, so new HTML tags or CSS properties can be added with minimal hassle.
Project Structure
Below is a recommended layout for your project when integrating Design Manager alongside Dear ImGui:

graphql
Kopyala
Düzenle
your_project/
├── external/
│   └── imgui/                # Dear ImGui source files (or a Git submodule)
├── src/
│   ├── main.cpp              # Your main application file
│   └── design_manager.h      # The single-header Design Manager library
├── include/                  # Additional headers, if needed
├── build/                    # Build directory (for Makefiles, CMake, etc.)
└── README.md                 # This file
Installation and Setup
1. Clone the Repository
First, clone your project repository and add Dear ImGui as a submodule (or just download it separately):

bash
Kopyala
Düzenle
git clone https://github.com/yourusername/design_manager.git
cd design_manager
git submodule add https://github.com/ocornut/imgui.git external/imgui
2. Configure Your Build System
If you use CMake, here’s a simple CMakeLists.txt you can place at the project’s root:

cmake
Kopyala
Düzenle
cmake_minimum_required(VERSION 3.10)
project(DesignManagerApp)

# Add Dear ImGui (assuming it's in external/imgui)
add_subdirectory(external/imgui)

# Add your source files
add_executable(DesignManagerApp src/main.cpp)

# Link ImGui with your application
target_link_libraries(DesignManagerApp PRIVATE imgui)
Make sure your build setup knows where to find both design_manager.h and Dear ImGui’s header files.

3. Main Application Setup
Inside your main.cpp, initialize ImGui and Design Manager. Here’s a quick example:

cpp
Kopyala
Düzenle
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include "design_manager.h" // Your Design Manager header

int main(int, char**)
{
    // Initialize GLFW
    if (!glfwInit())
        return 1;
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Design Manager App", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Initialize your Design Manager (set up layers, windows, etc.)
    DesignManager::Init(1280, 720, window);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Render Design Manager UI
        DesignManager::ShowUI(window);
        DesignManager::RenderAllRegisteredWindows();
        DesignManager::EvaluateGlobalLogicMappings();

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
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
Usage
Designing UI Elements
Take advantage of the builder classes (like ShapeBuilder, AnimationBuilder, and ShapeKeyBuilder) to create shapes, buttons, and child windows. You can also auto-generate code via built-in code generation functions.

Dynamic Child Window Management
Configure buttons to open or toggle child windows, or even switch them based on user selections. You can also set up global logic rules so that, for example, “if Button 1 and Button 2 are both active, open ChildWindow_3” without writing extra code.

HTML/CSS Conversion (Experimental)
If you want to generate ImGui builder code from HTML/CSS, you can try out the experimental converter module. Check out the To Do list above to see what’s coming next for this feature.

How to Organize Your Code on GitHub
Repository Structure
Single Header File
Everything in design_manager.h keeps your library self-contained and easy to drop into any project.

Main Application
Put your main.cpp (and any other integration logic) inside the src/ folder.

External Dependencies
Store Dear ImGui and any other libraries (like GLFW, OpenGL, stb_image) under external/. If possible, manage these as Git submodules for smooth updates.

Branching and Feature Development
Main Branch
Your library’s stable version lives here.

Feature Branches
For each new feature (like improvements to the HTML/CSS converter), create a separate branch. Merge into main once everything is tested and stable.

Issues and Pull Requests
Use GitHub Issues to track bugs or ideas for new features. Pull requests should have clear descriptions explaining what’s changing and why.

Documentation
README.md
Keep this file updated with details on what Design Manager does, how to integrate it, and how to use it.

Wiki (Optional)
If you need more detailed explanations, consider using GitHub’s Wiki for deeper documentation, API details, and step-by-step tutorials.

Future Enhancements
Enhanced HTML/CSS Parser
Integrate robust parsing tools (e.g., Gumbo for HTML) so Design Manager can handle a broader range of HTML tags and CSS properties.

Responsive Layouts
Add support for layouts that adjust automatically based on window size—similar to responsive web design.

Improved Animation Tools
Extend the chain animation system and add more interpolation options.

UI Customization
Give developers extra control over the appearance and behavior of child windows and buttons in the builder API.

HTML/CSS to ImGui Converter (Experimental)

 Integrate an advanced HTML parser to manage nested elements and errors.
 Incorporate a complete CSS parser to map style properties accurately.
 Enhance the mapping engine that ties HTML tags/CSS properties to ImGui builder calls.
 Implement logic for responsiveness, media queries, and live layout updates.
 Provide clear error handling for unsupported or ambiguous cases.
Contributing
We love contributions!
If you have suggestions or discover any bugs, feel free to open an issue or submit a pull request. Make sure your code follows the current coding style and include any relevant documentation updates.
