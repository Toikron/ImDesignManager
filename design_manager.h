#pragma once
#define IDI_APP_ICON 2
#include <fstream>
#include <filesystem>
#include <thread>
#include <chrono>
#ifndef IMGUI_IMPL_OPENGL_ES2
#endif
#include <imgui.h>
#include <unordered_set>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <imgui_internal.h>
#include <map>
#include <stb_image.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <functional>
#include <sstream> // for stringstream usage



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////FOR IMAGE IMPORTS//////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
using EmbeddedDataFunc = const std::vector<unsigned char>& (*)();
inline const std::vector<unsigned char>& GetShape0_embeddedData() {
    static std::vector<unsigned char> data(Shape0_embeddedData,
        Shape0_embeddedData + sizeof(Shape0_embeddedData)); 
    return data;
}
inline const std::vector<unsigned char>& GetShape1_embeddedData() {
    static std::vector<unsigned char> data(Shape1_embeddedData,
        Shape1_embeddedData + sizeof(Shape1_embeddedData));
    return data;
}

static const char* g_embeddedImageFunctions[] = {
    "GetShape0_embeddedData",
    "GetShape1_embeddedData",
    // "GetShape2_embeddedData",
};
static const EmbeddedDataFunc g_embeddedImageFuncs[] = {
    &GetShape0_embeddedData,
    &GetShape1_embeddedData,
	// can be added more
};
static const int g_embeddedImageFunctionsCount = sizeof(g_embeddedImageFunctions) / sizeof(g_embeddedImageFunctions[0]);
static const int g_embeddedImageFuncsCount = sizeof(g_embeddedImageFuncs) / sizeof(g_embeddedImageFuncs[0]);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////FOR IMAGE IMPORTS//////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



static int oldWindowWidth = 0;
static int oldWindowHeight = 0;
static bool shouldCaptureScene = false;
static int newLayerCount = 0;

inline bool operator==(const ImVec2& lhs, const ImVec2& rhs)
{
    return (lhs.x == rhs.x) && (lhs.y == rhs.y);
}
inline bool operator!=(const ImVec2& lhs, const ImVec2& rhs)
{
    return !(lhs == rhs);
}
inline bool operator==(const ImVec4& lhs, const ImVec4& rhs)
{
    return (lhs.x == rhs.x) && (lhs.y == rhs.y) &&
        (lhs.z == rhs.z) && (lhs.w == rhs.w);
}
inline bool operator!=(const ImVec4& lhs, const ImVec4& rhs)
{
    return !(lhs == rhs);
}
inline ImVec2& operator+=(ImVec2& a, const ImVec2& b)
{
    a.x += b.x;
    a.y += b.y;
    return a;
}
inline ImVec2 operator+(const ImVec2& a, const ImVec2& b) { return ImVec2(a.x + b.x, a.y + b.y); }
inline ImVec2 operator-(const ImVec2& a, const ImVec2& b) { return ImVec2(a.x - b.x, a.y - b.y); }
inline ImVec2 AddV(const ImVec2& a, const ImVec2& b) { return ImVec2(a.x + b.x, a.y + b.y); }
inline ImVec2 SubV(const ImVec2& a, const ImVec2& b) { return ImVec2(a.x - b.x, a.y - b.y); }
inline ImVec2 MulF(const ImVec2& a, float f) { return ImVec2(a.x * f, a.y * f); }
inline float  LengthV(const ImVec2& a) { return sqrtf(a.x * a.x + a.y * a.y); }
inline ImVec2 NormV(const ImVec2& a) { float l = LengthV(a); return (l > 0.000001f) ? ImVec2(a.x / l, a.y / l) : ImVec2(0, 0); }

inline ImVec2 RotateP(const ImVec2& p, const ImVec2& c, float a)
{
    float s = sinf(a), co = cosf(a);
    ImVec2 pt = SubV(p, c);
    float x = pt.x * co - pt.y * s;
    float y = pt.x * s + pt.y * co;
    return ImVec2(c.x + x, c.y + y);
}
inline float DegToRad(float d) { return d * IM_PI / 180.0f; }

inline float Lerp(float a, float b, float t)
{
    return a + t * (b - a);
}
inline ImVec2 Lerp(const ImVec2& a, const ImVec2& b, float t)
{
    return ImVec2(
        Lerp(a.x, b.x, t),
        Lerp(a.y, b.y, t)
    );
}
inline ImVec4 LerpC(const ImVec4& A, const ImVec4& B, float t)
{
    return ImVec4(Lerp(A.x, B.x, t),
        Lerp(A.y, B.y, t),
        Lerp(A.z, B.z, t),
        Lerp(A.w, B.w, t));
}

inline ImU32 ColU32(const ImVec4& c)
{
    return IM_COL32((int)(c.x * 255), (int)(c.y * 255),
        (int)(c.z * 255), (int)(c.w * 255));
}
inline ImVec2 UV(const ImVec2& p, const ImVec2& s, const ImVec2& c, const ImVec2& pos, float rot)
{
    ImVec2 rot_p = RotateP(p, c, -rot);
    float u = (rot_p.x - pos.x) / s.x;
    float v = (rot_p.y - pos.y) / s.y;
    return ImVec2(u, v);
}

inline ImVec4 CardinalSpline(const ImVec4& p0, const ImVec4& p1,
    const ImVec4& p2, const ImVec4& p3, float t, float tension = 0.5f)
{
    float t2 = t * t, t3 = t * t2;
    float h1 = -tension * t + 2 * tension * t2 - tension * t3;
    float h2 = 1 + (tension - 3 * tension) * t2 + (2 * tension - tension) * t3;
    float h3 = tension * t - tension * t2;
    float h4 = -tension * t2 + tension * t3;
    return ImVec4(h1 * p0.x + h2 * p1.x + h3 * p2.x + h4 * p3.x,
        h1 * p0.y + h2 * p1.y + h3 * p2.y + h4 * p3.y,
        h1 * p0.z + h2 * p1.z + h3 * p2.z + h4 * p3.z,
        h1 * p0.w + h2 * p1.w + h3 * p2.w + h4 * p3.w);
}
inline ImVec4 BSpline(const ImVec4& p0, const ImVec4& p1,
    const ImVec4& p2, const ImVec4& p3, float t)
{
    float t2 = t * t, t3 = t * t2;
    float h1 = (-t3 + 3 * t2 - 3 * t + 1) / 6.0f;
    float h2 = (3 * t3 - 6 * t2 + 4) / 6.0f;
    float h3 = (-3 * t3 + 3 * t2 + 3 * t + 1) / 6.0f;
    float h4 = t3 / 6.0f;
    return ImVec4(h1 * p0.x + h2 * p1.x + h3 * p2.x + h4 * p3.x,
        h1 * p0.y + h2 * p1.y + h3 * p2.y + h4 * p3.y,
        h1 * p0.z + h2 * p1.z + h3 * p2.z + h4 * p3.z,
        h1 * p0.w + h2 * p1.w + h3 * p2.w + h4 * p3.w);
}

inline float SmoothStep(float t)
{
    return t * t * (3.0f - 2.0f * t);
}

struct ShapeKeyAnimation
{
    std::string name = "New Animation";
    float duration = 1.0f;
    ImVec2 startValue = ImVec2(0, 0);
    ImVec2 endValue = ImVec2(0, 0);
};

const float DEG2RAD = 3.14159265359f / 180.0f;

namespace DesignManager
{
    enum class ShapeKeyType
    {
        SizeX,
        SizeY,
        PositionX,
        PositionY,
        Rotation
    };

    inline std::map<std::string, std::function<void()>> windowDesigns;
    inline std::vector<std::function<void()>> windowRenderFunctions;
    inline void RegisterWindowRenderFunction(const std::function<void()>& func) { windowRenderFunctions.emplace_back(func); }

    static bool sceneUpdated = false;
    static int  nextLayerID = 1, nextShapeID = 1;
    static int  selectedLayerID = -1, selectedShapeID = -1;
    static bool snapEnabled = true;
    static float snapGridSize = 10.0f;
    static int layerCount = -1;

    static ImTextureID black_texture_id = 0;
    static std::map<int, ImTextureID> gradientTextureCache;
    static void ClearGradientTextureCache()
    {
        for (auto& pair : gradientTextureCache)
        {
            glDeleteTextures(1, (GLuint*)&pair.second);
        }
        gradientTextureCache.clear();
    }

    static std::string generatedCodeForSingleShape;
    static std::string generatedCodeForWindow;
    static std::string generatedCodeForButton;

    struct ShapeKey
    {
        std::string name = "New Shape Key";
        ShapeKeyType type = ShapeKeyType::SizeX;
        ImVec2 startWindowSize = ImVec2(800.0f, 600.0f);
        ImVec2 endWindowSize = ImVec2(1200.0f, 900.0f);
        ImVec2 targetValue = ImVec2(100.0f, 100.0f);
        float  targetRotation = 0.0f;
        float  value = 0.0f;
        ImVec2 offset = ImVec2(0.0f, 0.0f);
        float  rotationOffset = 0.0f;
    };

    enum class PlaybackOrder
    {
        Sirali = 0,
        HemenArkasina = 1
    };


    struct ButtonAnimation
    {
        std::string name = "New Animation";
        float       duration = 0.5f;
        float       progress = 0.0f;
        bool        isPlaying = false;
        float       startTime = 0.0f;
        float       speed = 1.0f;
        bool        toggleState = false;
        ImVec2      animationTargetPosition;
        ImVec2      animationTargetSize;
        float       transformRotation;
        ImVec2      persistentPositionOffset;
        ImVec2      persistentSizeOffset;
        float       persistentRotationOffset;
        float       playbackDirection = 1.0f;
        int         repeatCount = 1;
        bool        hasStartedRepeatCount = false;
        int         remainingRepeats = 0;
        PlaybackOrder playbackOrder = PlaybackOrder::Sirali;

        enum class TriggerMode
        {
            OnClick = 0,
            OnHover = 1
        };
        TriggerMode triggerMode = TriggerMode::OnClick;

        enum class InterpolationMethod
        {
            Linear,
            EaseInOut
        };
        InterpolationMethod interpolationMethod = InterpolationMethod::Linear;

        enum class AnimationBehavior
        {
            PlayOnceAndStay,
            PlayOnceAndReverse,
            Toggle,
            PlayWhileHoldingAndReverseOnRelease,
            PlayWhileHoldingAndStay
        };
        AnimationBehavior behavior = AnimationBehavior::PlayOnceAndStay;
    };
    struct ChainAnimationStep {
        ButtonAnimation animation;
        // This callback will be executed when the step is completed (e.g., you can add additional actions)
        std::function<void()> onStepComplete;
    };

    struct ChainAnimation {
        std::vector<ChainAnimationStep> steps;
        int currentStep = 0;
        bool isPlaying = false;
        bool reverseMode = false;  // false: forward, true: backward
        bool toggled = false;      // false: original position, true: target position according to animation
    };
    inline ImVec2 GetWindowSize(GLFWwindow* window)
    {
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        return ImVec2(static_cast<float>(width), static_cast<float>(height));
    }

    enum class ShapeType { Rectangle, Circle };
    enum class ChildWindowToggleBehavior {
        WindowOnly,    // Only toggles the child window visibility
        ShapeAndWindow // Toggles both shape and child window visibility
    };
    struct ShapeItem
    {
        int id;
        ShapeType type;
        std::string name;
        ImVec2 position, size;
        ImVec2 basePosition = position;
        ImVec2 baseSize = size;
        float rotation, baseRotation = 0.0f;
        ImVec2 minSize = ImVec2(50, 50);
        ImVec2 maxSize = ImVec2(99999, 99999);
        bool openWindow;
        bool isChildWindow = false;           // New field: Is this shape a child window?
        bool childWindowSync = false;
        bool toggleChildWindow = false; // Feature to show child window when button is clicked
        ChildWindowToggleBehavior toggleBehavior = ChildWindowToggleBehavior::WindowOnly;
        int childWindowGroupId = -1; // Group ID for child window, -1: no group


        int targetShapeID = 0;
        int triggerGroupID = 0;
        
       
        
        bool allowItemOverlap = false;
        bool forceOverlap = false;
        bool blockUnderlying = true;
        bool hasText = false;
        std::string text = "";
        ImVec4 textColor = ImVec4(0, 0, 0, 1);
        float textSize = 16.0f;
        int textFont = 0;
        ImVec2 textPosition;
        float textRotation = 0.0f;
        int textAlignment;
        bool dynamicTextSize;
        float baseTextSize = 0.0f;
        float minTextSize = 8.0f;
        float maxTextSize = 72.0f;
        std::vector<ShapeKey> shapeKeys;
        std::vector<ShapeKeyAnimation> shapeKeyAnimations;
        float shapeKeyValue = 0.0f;
        std::vector<ButtonAnimation> onClickAnimations;
        ChainAnimation chainAnimation; // Chain animation system added!
        bool isPressed = false;
        int groupId = 0;
        ButtonAnimation* currentAnimation = nullptr;
        bool updateAnimBaseOnResize = false;
        bool isHeld = false;
        bool isAnimating = false;
        float animationProgress = 0.0f;
        ImVec2 baseKeyOffset = ImVec2(0, 0);
        ImVec2 baseKeySizeOffset = ImVec2(0, 0);
        float baseKeyRotationOffset = 0.0f;
        float cornerRadius, borderThickness;
        ImVec4 fillColor, borderColor, shadowColor, shadowSpread;
        ImVec2 shadowOffset;
        bool shadowUseCornerRadius;
        float blurAmount;
        bool visible, locked, useGradient;
        float gradientRotation;
        enum class GradientInterpolation { Linear, Ease, Constant, Cardinal, BSpline };
        GradientInterpolation gradientInterpolation;
        std::vector<std::pair<float, ImVec4>> colorRamp;
        float shadowRotation;
        bool useGlass;
        float glassBlur, glassAlpha;
        ImVec4 glassColor;
        GLuint glassBlurFBO[2], glassBlurTex[2];
        int zOrder;
        bool isButton = false;

        std::vector<std::string> assignedChildWindows; // Key of the child window that the button will open (if empty, default: s.name is used)
        int selectedChildWindowIndex = 0;
        std::string logicExpression;       // For example, "1 && 2", to specify conditions based on the state of the relevant button IDs.
        std::string logicAction;           // For example, "open:ChildWindow_1" or "close:ChildWindow_2", to specify the action to be taken when the condition is met.
        int clickCounter = 0;
        enum ButtonBehavior { SingleClick, Toggle, Hold } buttonBehavior = SingleClick;
        bool triggerEvent = false;
        bool buttonState = false, shouldCallOnClick = false;
        bool isClicked = false;
        ImVec4 hoverColor = ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
        ImVec4 clickedColor = ImVec4(0.6f, 0.6f, 0.6f, 1.0f);
        bool useOnClick = false;
        std::function<void()> onClick;
        std::function<void()> storedOnClick;
        ShapeItem* parent = nullptr;
        std::vector<ShapeItem*> children;
        ImVec2 originalPosition;
        float originalRotation;
        ImVec2 originalSize;
        std::string ownerWindow;
        struct Transformation
        {
            ImVec2 animationTargetPosition;
            ImVec2 animationTargetSize;
            float transformRotation;
            float animationDuration;
        };
        ImVec2 toggledStatePositionOffset = ImVec2(0, 0);
        ImVec2 toggledStateSizeOffset = ImVec2(0, 0);
        float toggledStateRotationOffset = 0.0f;
        std::vector<std::pair<ImVec2, Transformation>> sizeThresholds;
        bool hasEmbeddedImage = false;
        std::vector<unsigned char> embeddedImageData;
        int embeddedImageWidth = 0;
        int embeddedImageHeight = 0;
        int embeddedImageChannels = 0;
        int embeddedImageIndex = -1;
        ImTextureID embeddedImageTexture = 0;
        bool imageDirty = false;
        struct EventHandler {
            std::string eventType;  // "onClick", "onHover", "onRelease", etc.
            std::string name;       // Name of the handler for UI display
            std::function<void(ShapeItem&)> handler;  // The actual callback function
        };
        std::vector<EventHandler> eventHandlers;
        ShapeItem()
            : id(nextShapeID++),
            type(ShapeType::Rectangle),
            name("Shape"),
            position(ImVec2(100, 100)),
            size(ImVec2(200, 100)),
            cornerRadius(10),
            borderThickness(2),
            fillColor(ImVec4(0.932f, 0.932f, 0.932f, 1)),
            borderColor(ImVec4(0, 0, 0, 0.8f)),
            shadowColor(ImVec4(0, 0, 0, 0.2f)),
            shadowSpread(ImVec4(2, 2, 2, 2)),
            shadowOffset(ImVec2(2, 2)),
            shadowUseCornerRadius(true),
            rotation(0),
            blurAmount(0),
            visible(true),
            locked(false),
            useGradient(false),
            gradientRotation(0.0f),
            gradientInterpolation(GradientInterpolation::Linear),
            colorRamp(),
            shadowRotation(0),
            useGlass(false),
            glassBlur(10.0f),
            glassAlpha(0.7f),
            glassColor(ImVec4(1, 1, 1, 0.3f)),
            zOrder(0),
            openWindow(false)
        {
            colorRamp.emplace_back(0.0f, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
            colorRamp.emplace_back(1.0f, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
        }
    };

    inline std::vector<ShapeItem*> selectedShapes;
    inline std::string GenerateSingleShapeCode(const ShapeItem& shape);
    inline std::string GenerateCodeForSingleButton(const ShapeItem& buttonShape);
    inline std::map<int, bool> temporaryWindowsOpen;
    inline bool exclusiveChildWindowMode = true;

    struct Layer
    {
        int id;
        std::string name;
        std::vector<ShapeItem> shapes;
        bool visible, locked;
        int zOrder;
        Layer(const std::string& n)
        {
            id = 0;
            name = n;
            visible = true;
            locked = false;
            zOrder = 0;
        }
    };
    struct ChildToChildMapping {
        int buttonId;
        std::string childWindowKey;
    };
    struct ShapeChildMapping {
        int shapeId;
        std::string childWindowKey;
        int triggerButtonId;
    };
    inline std::vector<std::tuple<std::vector<int>, std::string, std::vector<std::string>>> g_logicMappings;
    inline std::vector<std::tuple<std::string, std::vector<int>, std::vector<std::string>>> g_childWindowMappings;

    inline std::unordered_map<int, std::string> g_childToChildButtonMapping;
    inline std::unordered_map<int, std::string> g_globalButtonChildWindowMapping;
    struct CombinedMapping {
        int shapeId;                      // Hedef shape'in ID'si
        std::string logicOp;              // "AND", "OR", "XOR", "NAND", "IF_THEN", "IFF" vb.
        std::vector<int> buttonIds;       // Mapping'e dahil buton ID'leri
        std::vector<std::string> childWindowKeys; // Atanacak pencere isimleri (g_windowsMap'teki)
    };
    inline std::vector<CombinedMapping> g_combinedChildWindowMappings;
    inline std::vector<ChildToChildMapping> g_childToChildMappings;


    inline ImVec2 triggerWindowSize = ImVec2(0, 0);
    inline ImVec2 initialWindowSize = ImVec2(0, 0);
    inline float transitionProgress = 0.0f;
    inline float transitionDuration = 0.5f;
    inline float transitionTimer = 0.0f;

    inline ImVec2 minWindowSize = ImVec2(800, 600);
    inline ImVec2 maxWindowSize = ImVec2(3840, 2160);

    static bool CompareShapesByZOrder(const ShapeItem& a, const ShapeItem& b) { return a.zOrder < b.zOrder; }
    static bool CompareLayersByZOrder(const Layer& a, const Layer& b) { return a.zOrder < b.zOrder; }

    struct WindowData
    {
        bool isOpen = false;
        bool isChildWindow = false;
        std::function<void()> renderFunc;
        std::vector<Layer> layers;
        int associatedShapeId = -1;
        int groupId = -1;
    };

    inline std::map<std::string, WindowData> g_windowsMap;
    inline std::vector<ShapeItem*> GetAllShapes() {
        std::vector<ShapeItem*> shapes;
        for (auto& [winName, windowData] : g_windowsMap) {
            for (auto& layer : windowData.layers) {
                for (auto& shape : layer.shapes) {
                    shapes.push_back(&shape);
                }
            }
        }
        return shapes;
    }
    inline std::vector<ShapeItem*> GetAllButtonShapes() {
        std::vector<ShapeItem*> buttonShapes;
        for (auto& [winName, winData] : g_windowsMap) {
            for (auto& layer : winData.layers) {
                for (auto& shape : layer.shapes) {
                    if (shape.isButton)
                        buttonShapes.push_back(&shape);
                }
            }
        }
        return buttonShapes;
    }
    inline void RegisterWindow(std::string name, std::function<void()> renderFunc)
    {
        if (name.empty())
        {
            static int childWindowCounter = 0;
            name = "ChildWindow_" + std::to_string(++childWindowCounter);
        }
        g_windowsMap[name].renderFunc = renderFunc;
        ImGui::SetNextItemAllowOverlap();
    }

    inline void SetWindowOpen(const std::string& name, bool open)
    {
        auto it = g_windowsMap.find(name);
        if (it == g_windowsMap.end())
            return;
        if (open && it->second.groupId > 0)
        {
            int groupId = it->second.groupId;
            for (auto& [winName, windowData] : g_windowsMap)
            {
                if (winName == name)
                    continue;
                if (windowData.groupId == groupId && windowData.isOpen)
                    windowData.isOpen = false;
            }
        }
        it->second.isOpen = open;
        for (auto shape : GetAllShapes())
        {
            if (shape->name == name)
            {
                shape->isChildWindow = true;
                break;
            }
        }
    }

    inline bool IsWindowOpen(const std::string& name)
    {
        auto it = g_windowsMap.find(name);
        if (it != g_windowsMap.end())
            return it->second.isOpen;
        return false;
    }

    inline float globalScaleFactor = 1.0f;

    inline ShapeItem* FindShapeByID(int shapeID) {
        for (auto& [winName, windowData] : DesignManager::g_windowsMap) {
            for (auto& layer : windowData.layers) {
                for (auto& shape : layer.shapes) {
                    if (shape.id == shapeID) {
                        return &shape;
                    }
                }
            }
        }
        return nullptr;
    }

    inline void UpdateGlobalScaleFactor(int currentW, int currentH)
    {
        float scaleX = currentW / minWindowSize.x;
        float scaleY = currentH / minWindowSize.y;
        globalScaleFactor = std::min(scaleX, scaleY);
    }

    static void RemoveParent(ShapeItem* child)
    {
        if (child->parent == nullptr) return;
        auto& children = child->parent->children;
        children.erase(std::remove(children.begin(), children.end(), child), children.end());
        child->parent = nullptr;
    }

    static void SetParent(ShapeItem* child, ShapeItem* parent)
    {
        if (child == nullptr || parent == nullptr || child == parent) return;
        if (child->parent != nullptr) { RemoveParent(child); }
        child->parent = parent;
        parent->children.push_back(child);
        ImVec2 childWorldPos = child->position;
        ShapeItem* p = child->parent;
        while (p != nullptr)
        {
            childWorldPos = childWorldPos + p->position;
            p = p->parent;
        }
        ImVec2 parentWorldPos = parent->position;
        p = parent->parent;
        while (p != nullptr)
        {
            parentWorldPos = parentWorldPos + p->position;
            p = p->parent;
        }
        child->originalPosition = childWorldPos - parentWorldPos;
        child->originalRotation = child->rotation;
        child->originalSize = child->size;
    }

    static void UpdateTransformations(ShapeItem* item)
    {
        if (item == nullptr) return;
        if (item->parent != nullptr)
        {
            item->position = item->parent->position + item->originalPosition;
            item->rotation = item->parent->rotation + item->originalRotation;
        }
        for (ShapeItem* child : item->children)
        {
            UpdateTransformations(child);
        }
    }

    static void RemoveParentKeepTransform(ShapeItem* child)
    {
        if (child->parent == nullptr) return;
        auto& children = child->parent->children;
        children.erase(std::remove(children.begin(), children.end(), child), children.end());
        UpdateTransformations(child);
        child->parent = nullptr;
    }

    inline std::string selectedGuiWindow = "Main";

    static void DrawShapeTreeNode(ShapeItem* shape)
    {
        ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
        if (std::find(selectedShapes.begin(), selectedShapes.end(), shape) != selectedShapes.end())
            node_flags |= ImGuiTreeNodeFlags_Selected;
        std::string id_str = shape->name + "##" + selectedGuiWindow + "_" + std::to_string(shape->id);
        bool node_open = ImGui::TreeNodeEx(id_str.c_str(), node_flags);
        bool item_clicked = ImGui::IsItemClicked();
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SHAPE_ITEM"))
            {
                IM_ASSERT(payload->DataSize == sizeof(ShapeItem*));
                ShapeItem* dragged_shape = *(ShapeItem**)payload->Data;
                SetParent(dragged_shape, shape);
            }
            ImGui::EndDragDropTarget();
        }
        if (ImGui::BeginDragDropSource())
        {
            ImGui::SetDragDropPayload("SHAPE_ITEM", &shape, sizeof(ShapeItem*));
            if (shape->parent == nullptr)
                ImGui::Text("Sürükle: %s (Kök Düğüm)", shape->name.c_str());
            else
                ImGui::Text("Sürükle: %s (Alt Düğüm)", shape->name.c_str());
            ImGui::EndDragDropSource();
        }
        if (node_open)
        {
            for (ShapeItem* child : shape->children)
                DrawShapeTreeNode(child);
            ImGui::TreePop();
        }
        if (item_clicked)
        {
            if (ImGui::GetIO().KeyCtrl)
            {
                auto it = std::find(selectedShapes.begin(), selectedShapes.end(), shape);
                if (it == selectedShapes.end())
                    selectedShapes.push_back(shape);
                else
                    selectedShapes.erase(std::remove(selectedShapes.begin(), selectedShapes.end(), shape), selectedShapes.end());
            }
            else
            {
                selectedShapes.clear();
                selectedShapes.push_back(shape);
            }
        }
    }

    static void BuildRectPoly(std::vector<ImVec2>& poly, ImVec2 pos, ImVec2 size, float r, ImVec2 c, float rot)
    {
        poly.clear();
        float mx = size.x * 0.5f, my = size.y * 0.5f;
        if (r > mx) r = mx;
        if (r > my) r = my;
        float x1 = pos.x, y1 = pos.y, x2 = pos.x + size.x, y2 = pos.y + size.y;
        auto Arc = [&](float cx, float cy, float start, float end, float rot_offset)
            {
                int seg = (int)std::max(4.0f, r * 12.0f);
                float step = (end - start) / (float)seg;
                for (int i = 0; i <= seg; i++)
                {
                    float ang = start + i * step;
                    ImVec2 p = RotateP(ImVec2(cx + cosf(ang) * r, cy + sinf(ang) * r), c, rot);
                    poly.push_back(p);
                }
            };
        if (r > 0)
        {
            Arc(x1 + r, y1 + r, IM_PI, IM_PI * 1.5f, rot);
            Arc(x2 - r, y1 + r, -IM_PI * 0.5f, 0.0f, rot);
            Arc(x2 - r, y2 - r, 0.0f, IM_PI * 0.5f, rot);
            Arc(x1 + r, y2 - r, IM_PI * 0.5f, IM_PI, rot);
        }
        else
        {
            poly.push_back(RotateP(ImVec2(x1, y1), c, rot));
            poly.push_back(RotateP(ImVec2(x2, y1), c, rot));
            poly.push_back(RotateP(ImVec2(x2, y2), c, rot));
            poly.push_back(RotateP(ImVec2(x1, y2), c, rot));
        }
    }

    static void BuildCirclePoly(std::vector<ImVec2>& poly, ImVec2 c, float rx, float ry, float rot)
    {
        poly.clear();
        int segments = 64;
        for (int i = 0; i < segments; i++)
        {
            float a = (2 * 3.14159265f * (float)i) / (float)segments;
            float x = cosf(a) * rx, y = sinf(a) * ry;
            poly.push_back(RotateP(ImVec2(c.x + x, c.y + y), c, rot));
        }
    }

    static ImTextureID CreateWhiteMaskTexture(int width, int height)
    {
        unsigned char* pixels = new unsigned char[width * height * 4];
        for (int i = 0; i < width * height * 4; i += 4)
        {
            pixels[i] = 255;
            pixels[i + 1] = 255;
            pixels[i + 2] = 255;
            pixels[i + 3] = 255;
        }
        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        glBindTexture(GL_TEXTURE_2D, 0);
        delete[] pixels;
        return (ImTextureID)(intptr_t)textureID;
    }

    static ImTextureID CreateGradientTexture(const ImVec2& size, float gradRotation, const std::vector<std::pair<float, ImVec4>>& colorRamp, DesignManager::ShapeItem::GradientInterpolation interpolationType)
    {
        int width = (int)size.x, height = (int)size.y;
        unsigned char* pixels = new unsigned char[width * height * 4];
        std::vector<std::pair<float, ImVec4>> sortedRamp = colorRamp;
        std::sort(sortedRamp.begin(), sortedRamp.end(), [](const auto& a, const auto& b) { return a.first < b.first; });
        if (sortedRamp.empty())
        {
            for (int i = 0; i < width * height * 4; i += 4)
            {
                pixels[i] = 255;
                pixels[i + 1] = 255;
                pixels[i + 2] = 255;
                pixels[i + 3] = 255;
            }
            GLuint textureID;
            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
            glBindTexture(GL_TEXTURE_2D, 0);
            delete[] pixels;
            return (ImTextureID)(intptr_t)textureID;
        }
        if (sortedRamp[0].first != 0.0f)
        {
            sortedRamp.insert(sortedRamp.begin(), { 0.0f, sortedRamp[0].second });
        }
        if (sortedRamp.back().first != 1.0f)
        {
            sortedRamp.push_back({ 1.0f, sortedRamp.back().second });
        }
        for (size_t i = 0; i < sortedRamp.size(); i++)
        {
            sortedRamp[i].first = ImClamp(sortedRamp[i].first, 0.0f, 1.0f);
        }
        float radRotation = DegToRad(gradRotation);
        ImVec2 center = ImVec2(size.x * 0.5f, size.y * 0.5f);
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                ImVec2 pixelPos = ImVec2(x, y);
                ImVec2 rot_p = RotateP(pixelPos, center, -radRotation);
                float local_x = rot_p.x - center.x;
                float t = ImClamp(local_x / size.x + 0.5f, 0.0f, 1.0f);
                ImVec4 color;
                if (sortedRamp.size() == 1)
                {
                    color = sortedRamp[0].second;
                }
                else if (t <= sortedRamp[0].first)
                {
                    color = sortedRamp[0].second;
                }
                else if (t >= sortedRamp.back().first)
                {
                    color = sortedRamp.back().second;
                }
                else
                {
                    for (size_t i = 0; i < sortedRamp.size() - 1; i++)
                    {
                        if (t >= sortedRamp[i].first && t <= sortedRamp[i + 1].first)
                        {
                            float localT = (t - sortedRamp[i].first) / (sortedRamp[i + 1].first - sortedRamp[i].first);
                            switch (interpolationType)
                            {
                            case DesignManager::ShapeItem::GradientInterpolation::Linear:
                                color = LerpC(sortedRamp[i].second, sortedRamp[i + 1].second, localT);
                                break;
                            case DesignManager::ShapeItem::GradientInterpolation::Ease:
                                color = LerpC(sortedRamp[i].second, sortedRamp[i + 1].second, SmoothStep(localT));
                                break;
                            case DesignManager::ShapeItem::GradientInterpolation::Constant:
                                color = sortedRamp[i].second;
                                break;
                            case DesignManager::ShapeItem::GradientInterpolation::Cardinal:
                            {
                                int p0_index = (i > 0) ? i - 1 : 0;
                                int p2_index = i + 1;
                                int p3_index = (i < sortedRamp.size() - 2) ? i + 2 : sortedRamp.size() - 1;
                                float localT = (t - sortedRamp[i].first) / (sortedRamp[i + 1].first - sortedRamp[i].first);
                                localT = ImClamp(localT, 0.0f, 1.0f);
                                ImVec4 p0 = sortedRamp[p0_index].second;
                                ImVec4 p1 = sortedRamp[i].second;
                                ImVec4 p2 = sortedRamp[p2_index].second;
                                ImVec4 p3 = sortedRamp[p3_index].second;
                                color = CardinalSpline(p0, p1, p2, p3, localT, 0.5f);
                            }
                            break;
                            case DesignManager::ShapeItem::GradientInterpolation::BSpline:
                            {
                                int p0_index = (i > 0) ? i - 1 : 0;
                                int p2_index = i + 1;
                                int p3_index = (i < sortedRamp.size() - 2) ? i + 2 : sortedRamp.size() - 1;
                                float p1_t = localT;
                                ImVec4 p0 = sortedRamp[p0_index].second;
                                ImVec4 p1 = sortedRamp[i].second;
                                ImVec4 p2 = sortedRamp[p2_index].second;
                                ImVec4 p3 = sortedRamp[p3_index].second;
                                color = BSpline(p0, p1, p2, p3, p1_t);
                            }
                            break;
                            }
                            break;
                        }
                    }
                }
                int index = (y * width + x) * 4;
                pixels[index] = (unsigned char)(color.x * 255);
                pixels[index + 1] = (unsigned char)(color.y * 255);
                pixels[index + 2] = (unsigned char)(color.z * 255);
                pixels[index + 3] = (unsigned char)(color.w * 255);
            }
        }
        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        glBindTexture(GL_TEXTURE_2D, 0);
        delete[] pixels;
        return (ImTextureID)(intptr_t)textureID;
    }

    static void DrawGradient(ImDrawList* dl, const std::vector<ImVec2>& poly, float gradRotation, const std::vector<std::pair<float, ImVec4>>& colorRamp, const ShapeItem& s)
    {
        size_t n = poly.size();
        if (n < 3)
        {
            ImVec4 color = colorRamp.size() > 0 ? colorRamp[0].second : ImVec4(1, 1, 1, 1);
            dl->AddConvexPolyFilled(poly.data(), (int)n, ColU32(color));
            return;
        }
        ImVec2 c = ImVec2(s.position.x + s.size.x * 0.5f, s.position.y + s.size.y * 0.5f);
        ImVec2 pos = s.position;
        int hash = s.size.x * s.size.y * (int)(gradRotation * 100);
        hash = (hash * 31 + (int)s.gradientInterpolation);
        for (auto& pair : colorRamp)
        {
            hash = (hash * 31 + (int)(pair.first * 1000)) * 31 + ColU32(pair.second);
        }
        ImTextureID gradient_texture_id;
        if (gradientTextureCache.find(hash) == gradientTextureCache.end())
            gradient_texture_id = CreateGradientTexture(s.size, gradRotation, colorRamp, s.gradientInterpolation);
        else
        {
            gradient_texture_id = gradientTextureCache[hash];
        }
        if (!gradient_texture_id)
        {
            fprintf(stderr, "Failed to create gradient texture\n");
            return;
        }
        gradientTextureCache[hash] = gradient_texture_id;
        dl->PushTextureID(gradient_texture_id);
        std::vector<ImVec2> uvCoords(n);
        for (size_t i = 0; i < n; i++)
        {
            uvCoords[i] = UV(poly[i], s.size, AddV(c, ImGui::GetWindowPos()), AddV(pos, ImGui::GetWindowPos()), s.rotation);
        }
        for (size_t i = 1; i < n - 1; i++)
        {
            dl->PrimReserve(3, 3);
            dl->PrimVtx(poly[0], uvCoords[0], 0xffffffff);
            dl->PrimVtx(poly[i], uvCoords[i], 0xffffffff);
            dl->PrimVtx(poly[i + 1], uvCoords[i + 1], 0xffffffff);
        }
        dl->PopTextureID();
    }

    static std::vector<Layer> layers;

    inline void RenderTemporaryWindows()
    {
        for (auto& [wName, WindowData] : g_windowsMap)
        {
            for (auto& layer : WindowData.layers)
            {
                for (auto& shape : layer.shapes)
                {
                    if (shape.childWindowSync)
                        return;
                }
            }
        }

        for (auto& kv : temporaryWindowsOpen)
        {
            int shapeId = kv.first;
            bool isOpen = kv.second;
            if (!isOpen)
                continue;
            ShapeItem* theShape = nullptr;
            for (auto& [wName, WindowData] : g_windowsMap)
            {
                for (auto& layer : WindowData.layers)
                {
                    for (auto& shape : layer.shapes)
                    {
                        if (shape.id == shapeId)
                        {
                            theShape = &shape;
                            break;
                        }
                    }
                    if (theShape) break;
                }
                if (theShape) break;
            }
            if (!theShape)
                continue;
            std::string tempWinTitle = "TempWindow - " + theShape->name;
            ImGui::Begin(tempWinTitle.c_str(), &kv.second, ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::Text("Bu pencere, onClick tanımlanmamış bir butona tıklama sonucu açıldı.");
            ImGui::Separator();
            if (ImGui::Button("Bana Kod Ekle (OnClick)"))
            {
                theShape->useOnClick = true;
                theShape->onClick = [&]()
                    {
                        std::cout << "[User Defined] Butona sonradan OnClick eklendi!\n";
                    };
                kv.second = false;
            }
            if (ImGui::Button("Bu geçici pencereyi kapat"))
            {
                kv.second = false;
            }
            if (!kv.second && theShape != nullptr)
            {
                theShape->buttonState = false;
            }
            ImGui::End();
        }
    }


    static void AddTextRotated(
        ImDrawList* draw_list,
        ImFont* font,
        float font_size,
        const ImVec2& pos,
        ImU32 col,
        const char* text,
        float angle_radians,
        const ImVec2& pivot_norm
    )
    {
        if (!text || !text[0])
            return;
        if (!draw_list)
            return;
        if (!font)
            font = ImGui::GetFont();
        if (font_size <= 0.0f)
            font_size = ImGui::GetFontSize();
        int vtx_start_idx = draw_list->VtxBuffer.Size;
        draw_list->AddText(font, font_size, pos, col, text);
        int vtx_end_idx = draw_list->VtxBuffer.Size;
        if (vtx_end_idx <= vtx_start_idx)
            return;
        ImVec2 bb_min(FLT_MAX, FLT_MAX), bb_max(-FLT_MAX, -FLT_MAX);
        for (int i = vtx_start_idx; i < vtx_end_idx; i++)
        {
            ImVec2 p = draw_list->VtxBuffer[i].pos;
            bb_min.x = ImMin(bb_min.x, p.x);
            bb_min.y = ImMin(bb_min.y, p.y);
            bb_max.x = ImMax(bb_max.x, p.x);
            bb_max.y = ImMax(bb_max.y, p.y);
        }
        ImVec2 size = ImVec2(bb_max.x - bb_min.x, bb_max.y - bb_min.y);
        ImVec2 pivot_in = ImVec2(
            bb_min.x + size.x * pivot_norm.x,
            bb_min.y + size.y * pivot_norm.y
        );
        ImVec2 pivot_out = pivot_in;
        float s = sinf(angle_radians);
        float c = cosf(angle_radians);
        ImGui::ShadeVertsTransformPos(draw_list, vtx_start_idx, vtx_end_idx, pivot_in, c, s, pivot_out);
    }


    inline void DrawShape(ImDrawList* dl, ShapeItem& s, ImVec2 wp);
    inline void DrawShape_RenderChildWindow(ShapeItem& s, ImVec2 wp, float scaleFactor, const std::vector<ImVec2>& poly);


    inline void DrawShape_Shadow(ImDrawList* dlEffective, ShapeItem& s, ImVec2 wp, float scaleFactor, ImVec2 c, float totalrot) {
        float cr = s.shadowUseCornerRadius ? s.cornerRadius * scaleFactor : 0.0f;
        ImVec2 spos = ImVec2(
            s.position.x * scaleFactor - s.shadowSpread.x * scaleFactor + s.shadowOffset.x * scaleFactor,
            s.position.y * scaleFactor - s.shadowSpread.y * scaleFactor + s.shadowOffset.y * scaleFactor
        );
        ImVec2 ssize = ImVec2(
            s.size.x * scaleFactor + s.shadowSpread.x * scaleFactor + s.shadowSpread.z * scaleFactor,
            s.size.y * scaleFactor + s.shadowSpread.y * scaleFactor + s.shadowSpread.w * scaleFactor
        );
        std::vector<ImVec2> shpoly;
        if (s.type == ShapeType::Rectangle) {
            BuildRectPoly(shpoly, ImVec2(wp.x + spos.x, wp.y + spos.y), ssize, cr, c, totalrot);
        }
        else {
            float rx = s.size.x * 0.5f * scaleFactor;
            float ry = s.size.y * 0.5f * scaleFactor;
            BuildCirclePoly(shpoly, ImVec2(wp.x + spos.x + rx, wp.y + spos.y + ry), rx, ry, totalrot);
        }
        dlEffective->AddConvexPolyFilled(shpoly.data(), (int)shpoly.size(), ColU32(s.shadowColor));
    }

    inline void DrawShape_Blur(ImDrawList* dlEffective, ShapeItem& s, ImVec2 wp, float scaleFactor, ImVec2 c) {
        int blurpasses = (int)floorf(s.blurAmount * scaleFactor);
        float blurAlpha = 0.05f;
        for (int i = 0; i < blurpasses; i++) {
            float off = i * 0.5f * scaleFactor;
            ImVec4 col = s.fillColor;
            col.w *= blurAlpha;
            std::vector<ImVec2> bpoly;
            if (s.type == ShapeType::Rectangle) {
                BuildRectPoly(
                    bpoly,
                    ImVec2(wp.x + s.position.x * scaleFactor + off, wp.y + s.position.y * scaleFactor + off),
                    ImVec2(s.size.x * scaleFactor, s.size.y * scaleFactor),
                    s.cornerRadius * scaleFactor,
                    c,
                    s.rotation
                );
            }
            else {
                float rx = s.size.x * 0.5f * scaleFactor;
                float ry = s.size.y * 0.5f * scaleFactor;
                BuildCirclePoly(bpoly, c, rx, ry, s.rotation);
            }
            dlEffective->AddConvexPolyFilled(bpoly.data(), (int)bpoly.size(), ColU32(col));
        }
    }

    inline void BuildMainShapePoly(ShapeItem& s, ImVec2 wp, float scaleFactor, ImVec2 c, std::vector<ImVec2>& poly) {
        if (s.type == ShapeType::Rectangle) {
            BuildRectPoly(
                poly,
                ImVec2(wp.x + s.position.x * scaleFactor, wp.y + s.position.y * scaleFactor),
                ImVec2(s.size.x * scaleFactor, s.size.y * scaleFactor),
                s.cornerRadius * scaleFactor,
                c,
                s.rotation
            );
        }
        else {
            float rx = s.size.x * 0.5f * scaleFactor;
            float ry = s.size.y * 0.5f * scaleFactor;
            BuildCirclePoly(poly, c, rx, ry, s.rotation);
        }
    }

    inline void DrawShape_LoadEmbeddedImageIfNeeded(ShapeItem& item) {
        // Gömülü resim yoksa veya zaten texture oluşturulmuşsa devam etme
        if (!item.hasEmbeddedImage) return;
        if (item.embeddedImageTexture != 0) return;

        // Veri boşsa ve geçerli bir index varsa, otomatik olarak yükle
        if (item.embeddedImageData.empty() && item.embeddedImageIndex >= 0 && item.embeddedImageIndex < g_embeddedImageFuncsCount) {
            // İlgili fonksiyonu diziden al ve çağır
            item.embeddedImageData = g_embeddedImageFuncs[item.embeddedImageIndex]();
        }

        // Veri hâlâ boşsa devam etme
        if (item.embeddedImageData.empty()) return;

        // Resmi belleğe yükle ve texture oluştur
        int w = 0, h = 0, ch = 0;
        unsigned char* decoded = stbi_load_from_memory(
            item.embeddedImageData.data(),
            (int)item.embeddedImageData.size(),
            &w, &h, &ch, 4
        );
        if (!decoded) {
            std::cerr << "Resim yüklenemedi: " << item.name << std::endl;
            return;
        }

        GLuint tex = 0;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, decoded);
        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(decoded);

        // Texture bilgilerini güncelle
        item.embeddedImageWidth = w;
        item.embeddedImageHeight = h;
        item.embeddedImageChannels = 4;
        item.embeddedImageTexture = (ImTextureID)(intptr_t)tex;
    }

    inline void DrawShape_DrawEmbeddedImageIfAny(ImDrawList* dlEffective, ShapeItem& s, float scaleFactor, ImVec2 cImage, std::vector<ImVec2>& poly) {
        if (s.hasEmbeddedImage && s.embeddedImageTexture) {
            dlEffective->PushTextureID(s.embeddedImageTexture);
            std::vector<ImVec2> uvCoords(poly.size());
            for (size_t i = 0; i < poly.size(); i++) {
                uvCoords[i] = UV(
                    poly[i],
                    ImVec2(s.size.x * scaleFactor, s.size.y * scaleFactor),
                    AddV(cImage, ImGui::GetWindowPos()),
                    AddV(s.position, ImGui::GetWindowPos()),
                    s.rotation
                );
            }
            for (size_t i = 1; i < poly.size() - 1; i++) {
                dlEffective->PrimReserve(3, 3);
                dlEffective->PrimVtx(poly[0], uvCoords[0], IM_COL32_WHITE);
                dlEffective->PrimVtx(poly[i], uvCoords[i], IM_COL32_WHITE);
                dlEffective->PrimVtx(poly[i + 1], uvCoords[i + 1], IM_COL32_WHITE);
            }
            dlEffective->PopTextureID();
        }
    }

    inline void DispatchEvent(ShapeItem& shape, const std::string& eventType) {
        for (auto& handler : shape.eventHandlers) {
            if (handler.eventType == eventType) {
                handler.handler(shape);
            }
        }
    }
    inline void DrawShape_ProcessButtonLogic(ImDrawList* dlEffective, ShapeItem& s, float scaleFactor, ImVec2 wp, ImVec4& drawColor) {
        ImVec2 button_abs_pos = ImVec2(wp.x + s.position.x * scaleFactor, wp.y + s.position.y * scaleFactor);
        ImVec2 button_size = ImVec2(s.size.x * scaleFactor, s.size.y * scaleFactor);
        std::string button_id = "##button_" + std::to_string(s.id);
        if (s.forceOverlap) ImGui::SetNextItemAllowOverlap();
        ImGui::SetCursorScreenPos(button_abs_pos);
        ImGui::InvisibleButton(button_id.c_str(), button_size);
        ImGui::SetCursorPos(ImVec2(0, 10));
        bool is_hovered = ImGui::IsItemHovered();
        bool is_active = ImGui::IsItemActive();
        bool is_released = ImGui::IsMouseReleased(0);
        if (!s.allowItemOverlap) {
            ImGuiID myID = ImGui::GetID(button_id.c_str());
            if (ImGui::GetHoveredID() != myID) {
                is_hovered = false;
                is_active = false;
            }
        }
        if (s.allowItemOverlap) {
            ImVec2 mousePos = ImGui::GetIO().MousePos;
            ImRect buttonRect(button_abs_pos, ImVec2(button_abs_pos.x + button_size.x, button_abs_pos.y + button_size.y));
            if (buttonRect.Contains(mousePos)) {
                is_hovered = true;
                if (ImGui::IsMouseDown(0)) is_active = true;
            }
        }
        if (s.forceOverlap && s.allowItemOverlap && s.blockUnderlying) {
            ImGui::SetNextWindowPos(button_abs_pos);
            ImGui::SetNextWindowSize(button_size);
            std::string overlay_id = "BlockOverlay_" + std::to_string(s.id);
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
            ImGui::BeginChild(overlay_id.c_str(), button_size, false,
                ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground);
            ImGui::Dummy(button_size);
            ImGui::EndChild();
            ImGui::PopStyleColor();
        }
        if (is_hovered) drawColor = s.hoverColor;
        for (auto& anim : s.onClickAnimations) {
            if (anim.triggerMode == ButtonAnimation::TriggerMode::OnHover) {
                if (is_hovered) {
                    if (!anim.isPlaying) {
                        if (anim.behavior == ButtonAnimation::AnimationBehavior::Toggle) {
                            if (!anim.toggleState) {
                                anim.isPlaying = true;
                                anim.progress = 0.0f;
                                anim.speed = fabs(anim.speed);
                                s.currentAnimation = &anim;
                                anim.startTime = ImGui::GetTime();
                            }
                        }
                        else {
                            anim.isPlaying = true;
                            anim.progress = 0.0f;
                            anim.speed = fabs(anim.speed);
                            s.currentAnimation = &anim;
                            anim.startTime = ImGui::GetTime();
                        }
                    }
                }
                else {
                    if (anim.isPlaying) {
                        if (anim.behavior == ButtonAnimation::AnimationBehavior::PlayOnceAndReverse && anim.progress > 0.0f && anim.speed > 0.0f) {
                            anim.speed = -fabs(anim.speed);
                        }
                        if (anim.behavior == ButtonAnimation::AnimationBehavior::Toggle) {
                            if (anim.toggleState) {
                                anim.isPlaying = true;
                                anim.speed = -fabs(anim.speed);
                            }
                        }
                    }
                }
            }
        }
        if (s.buttonBehavior == ShapeItem::ButtonBehavior::SingleClick) {
            if (is_active) {
                if (!s.buttonState) s.buttonState = true;
                drawColor = s.clickedColor;
            }
            else if (!is_active && s.buttonState) {
                s.shouldCallOnClick = true;
                s.buttonState = false;
            }
            else if (is_hovered) {
                drawColor = s.hoverColor;
            }
        }
        else if (s.buttonBehavior == ShapeItem::ButtonBehavior::Toggle) {
            if (ImGui::IsItemClicked()) {
                bool newState = !s.buttonState;
                // If toggling on and a group is set (groupId > 0), disable other buttons in the same group.
                if (newState && s.groupId > 0) {
                    auto allButtons = GetAllButtonShapes();
                    for (auto* otherButton : allButtons) {
                        if (otherButton->id != s.id && otherButton->groupId == s.groupId) {
                            otherButton->buttonState = false;
                            // Their associated child windows will be closed via SetWindowOpen later.
                        }
                    }
                }
                s.buttonState = newState;
                s.shouldCallOnClick = true;
            }
            if (s.buttonState) drawColor = s.clickedColor;
            else if (!s.buttonState && !s.openWindow) {
                if (!is_hovered) drawColor = s.fillColor;
            }
        }
        else if (s.buttonBehavior == ShapeItem::ButtonBehavior::Hold) {
            if (is_active) {
                drawColor = s.clickedColor;
                if (!s.buttonState) {
                    s.shouldCallOnClick = true;
                    s.buttonState = true;
                }
            }
            if (is_released && s.buttonState) {
                s.shouldCallOnClick = true;
                s.buttonState = false;
            }
            if (s.buttonState) drawColor = s.clickedColor;
            else if (is_hovered) drawColor = s.hoverColor;
        }
        if (s.shouldCallOnClick) {
            DispatchEvent(s, "onClick");
            if (!s.onClickAnimations.empty()) {
                std::vector<int> onClickIndices;
                onClickIndices.reserve(s.onClickAnimations.size());
                for (int i = 0; i < (int)s.onClickAnimations.size(); i++) {
                    if (s.onClickAnimations[i].triggerMode == ButtonAnimation::TriggerMode::OnClick)
                        onClickIndices.push_back(i);
                }
                static int currentAnimIndex = -1;
                if (!onClickIndices.empty()) {
                    if (currentAnimIndex < 0) currentAnimIndex = 0;
                    else currentAnimIndex = (currentAnimIndex + 1) % onClickIndices.size();
                    int idx = onClickIndices[currentAnimIndex];
                    auto& anim = s.onClickAnimations[idx];
                    anim.progress = 0.0f;
                    anim.isPlaying = true;
                    anim.startTime = ImGui::GetTime();
                    s.currentAnimation = &anim;
                }
            }
            s.shouldCallOnClick = false;
        }
        s.isHeld = ImGui::IsItemActive();
    }




    inline void DrawShape_FillWithGradient(ImDrawList* dlEffective, const std::vector<ImVec2>& poly, float gradRotation, const std::vector<std::pair<float, ImVec4>>& colorRamp, const ShapeItem& shape, float scaleFactor) {
        size_t n = poly.size();
        if (n < 3) {
            ImVec4 color = colorRamp.size() > 0 ? colorRamp[0].second : ImVec4(1, 1, 1, 1);
            dlEffective->AddConvexPolyFilled(poly.data(), (int)n, ColU32(shape.fillColor));
            return;
        }
        ImVec2 cc = ImVec2(
            shape.position.x * scaleFactor + shape.size.x * 0.5f * scaleFactor,
            shape.position.y * scaleFactor + shape.size.y * 0.5f * scaleFactor
        );
        ImVec2 pos = shape.position;
        int hash = (int)(shape.size.x * shape.size.y * (int)(gradRotation * 100));
        hash = (hash * 31 + (int)(shape.gradientInterpolation));
        for (auto& pair_ : colorRamp) {
            hash = (hash * 31 + (int)(pair_.first * 1000)) * 31 + ColU32(pair_.second);
        }
        ImTextureID gradient_texture_id;
        if (gradientTextureCache.find(hash) == gradientTextureCache.end()) {
            gradient_texture_id = CreateGradientTexture(
                ImVec2(shape.size.x * scaleFactor, shape.size.y * scaleFactor),
                gradRotation,
                colorRamp,
                shape.gradientInterpolation
            );
        }
        else {
            gradient_texture_id = gradientTextureCache[hash];
        }
        if (!gradient_texture_id) {
            dlEffective->AddConvexPolyFilled(poly.data(), (int)poly.size(), ColU32(shape.fillColor));
            return;
        }
        gradientTextureCache[hash] = gradient_texture_id;
        dlEffective->PushTextureID(gradient_texture_id);
        std::vector<ImVec2> uvCoords(n);
        for (size_t i = 0; i < n; i++) {
            uvCoords[i] = UV(
                poly[i],
                ImVec2(shape.size.x * scaleFactor, shape.size.y * scaleFactor),
                AddV(cc, ImGui::GetWindowPos()),
                AddV(pos, ImGui::GetWindowPos()),
                shape.rotation
            );
        }
        for (size_t i = 1; i < n - 1; i++) {
            dlEffective->PrimReserve(3, 3);
            dlEffective->PrimVtx(poly[0], uvCoords[0], 0xffffffff);
            dlEffective->PrimVtx(poly[i], uvCoords[i], 0xffffffff);
            dlEffective->PrimVtx(poly[i + 1], uvCoords[i + 1], 0xffffffff);
        }
        dlEffective->PopTextureID();
    }

    inline void DrawShape_Fill(ImDrawList* dlEffective, ShapeItem& s, const std::vector<ImVec2>& poly, float scaleFactor, ImVec4 drawColor) {
        if (s.useGradient) {
            DrawShape_FillWithGradient(dlEffective, poly, s.gradientRotation, s.colorRamp, s, scaleFactor);
        }
        else {
            dlEffective->AddConvexPolyFilled(poly.data(), (int)poly.size(), ColU32(drawColor));
        }
    }

    inline void DrawShape_RenderChildWindow(ShapeItem& s, ImVec2 wp, float scaleFactor, const std::vector<ImVec2>& poly)
    {
        for (auto& mapping : g_combinedChildWindowMappings)
        {
            if (mapping.shapeId != s.id)
                continue;

            s.isChildWindow = true;

            // If using "None", process each button-child window pair individually.
            if (mapping.logicOp == "None")
            {
                // For each pair, assign the group id so that SetWindowOpen can enforce exclusivity.
                for (size_t i = 0; i < mapping.buttonIds.size(); i++)
                {
                    bool btnState = false;
                    // Get the current state of the button.
                    for (auto& [wName, winData] : g_windowsMap)
                    {
                        for (auto& layer : winData.layers)
                        {
                            for (auto& sh : layer.shapes)
                            {
                                if (sh.isButton && sh.id == mapping.buttonIds[i])
                                {
                                    btnState = sh.buttonState;
                                    break;
                                }
                            }
                        }
                    }

                    const std::string& childKey = mapping.childWindowKeys[i];
                    // Set the group id for the child window to enable exclusivity.
                    g_windowsMap[childKey].groupId = s.childWindowGroupId;
                    // Open or close the window based on the button state.
                    SetWindowOpen(childKey, btnState);
                }
            }
            else
            {
                int activeCount = 0;
                for (int btnId : mapping.buttonIds)
                {
                    for (auto& [wName, winData] : g_windowsMap)
                    {
                        for (auto& layer : winData.layers)
                        {
                            for (auto& sh : layer.shapes)
                            {
                                if (sh.isButton && sh.id == btnId && sh.buttonState)
                                    activeCount++;
                            }
                        }
                    }
                }
                int totalCount = (int)mapping.buttonIds.size();
                bool conditionMet = false;
                if (totalCount > 0) {
                    if (mapping.logicOp == "AND")
                        conditionMet = (activeCount == totalCount);
                    else if (mapping.logicOp == "OR")
                        conditionMet = (activeCount > 0);
                    else if (mapping.logicOp == "XOR")
                        conditionMet = (activeCount % 2 == 1);
                    else if (mapping.logicOp == "NAND")
                        conditionMet = !(activeCount == totalCount);
                    else if (mapping.logicOp == "IF_THEN") {
                        if (totalCount == 2) {
                            bool first = false, second = false;
                            for (auto& [wName, winData] : g_windowsMap) {
                                for (auto& layer : winData.layers) {
                                    for (auto& sh : layer.shapes) {
                                        if (sh.isButton && sh.id == mapping.buttonIds[0])
                                            first = sh.buttonState;
                                        if (sh.isButton && sh.id == mapping.buttonIds[1])
                                            second = sh.buttonState;
                                    }
                                }
                            }
                            conditionMet = (!first) || (first && second);
                        }
                    }
                    else if (mapping.logicOp == "IFF") {
                        if (totalCount == 1) {
                            bool state = false;
                            for (auto& [wName, winData] : g_windowsMap) {
                                for (auto& layer : winData.layers) {
                                    for (auto& sh : layer.shapes) {
                                        if (sh.isButton && sh.id == mapping.buttonIds[0]) {
                                            state = sh.buttonState;
                                            break;
                                        }
                                    }
                                }
                            }
                            conditionMet = state;
                        }
                        else {
                            bool firstState = false;
                            bool found = false;
                            for (auto& [wName, winData] : g_windowsMap) {
                                for (auto& layer : winData.layers) {
                                    for (auto& sh : layer.shapes) {
                                        if (sh.isButton && sh.id == mapping.buttonIds[0]) {
                                            firstState = sh.buttonState;
                                            found = true;
                                            break;
                                        }
                                    }
                                    if (found) break;
                                }
                                if (found) break;
                            }
                            bool allEqual = true;
                            for (int btnId : mapping.buttonIds) {
                                bool state = false;
                                for (auto& [wName, winData] : g_windowsMap) {
                                    for (auto& layer : winData.layers) {
                                        for (auto& sh : layer.shapes) {
                                            if (sh.isButton && sh.id == btnId) {
                                                state = sh.buttonState;
                                                break;
                                            }
                                        }
                                    }
                                }
                                if (state != firstState) { allEqual = false; break; }
                            }
                            conditionMet = allEqual && firstState;
                        }
                    }
                }
                // Set group id for each child window before opening.
                for (auto& childKey : mapping.childWindowKeys)
                {
                    g_windowsMap[childKey].groupId = s.childWindowGroupId;
                    SetWindowOpen(childKey, conditionMet);
                }
            }

            // Draw the child windows that are open.
            for (auto& childKey : mapping.childWindowKeys)
            {
                if (IsWindowOpen(childKey))
                {
                    if (s.childWindowSync)
                    {
                        ImVec2 syncPos = ImVec2(s.position.x * globalScaleFactor, s.position.y * globalScaleFactor);
                        ImGui::SetNextWindowPos(AddV(syncPos, ImGui::GetWindowPos()));
                        ImGui::SetNextWindowSize(ImVec2(s.size.x * globalScaleFactor, s.size.y * globalScaleFactor));
                    }
                    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 15));
                    ImGui::BeginChild(childKey.c_str(), ImVec2(s.size.x * globalScaleFactor, s.size.y * globalScaleFactor),
                        false, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysUseWindowPadding);
                    if (g_windowsMap.find(childKey) != g_windowsMap.end())
                    {
                        g_windowsMap[childKey].isChildWindow = true;
                        g_windowsMap[childKey].associatedShapeId = s.id;
                        if (g_windowsMap[childKey].renderFunc)
                            g_windowsMap[childKey].renderFunc();
                        auto& childWindowData = g_windowsMap[childKey];
                        std::stable_sort(childWindowData.layers.begin(), childWindowData.layers.end(), CompareLayersByZOrder);
                        for (auto& layer : childWindowData.layers)
                        {
                            if (!layer.visible) continue;
                            std::stable_sort(layer.shapes.begin(), layer.shapes.end(), CompareShapesByZOrder);
                            for (auto& childShape : layer.shapes)
                            {
                                if (childShape.ownerWindow == childKey && childShape.parent == nullptr)
                                    DrawShape(ImGui::GetWindowDrawList(), childShape, ImGui::GetWindowPos());
                            }
                        }
                    }
                    else
                    {
                        ShapeItem* targetShape = nullptr;
                        auto allShapes = GetAllShapes();
                        for (auto sh : allShapes)
                        {
                            if (sh->name == childKey)
                            {
                                targetShape = sh;
                                break;
                            }
                        }
                        if (targetShape)
                            DrawShape(ImGui::GetWindowDrawList(), *targetShape, ImGui::GetWindowPos());
                        else
                            ImGui::Text("Child window content not registered.");
                    }
                    ImGui::EndChild();
                    ImGui::PopStyleVar();
                }
            }
        }
        if (s.borderThickness > 0.0f)
        {
            ImDrawList* dl = ImGui::GetWindowDrawList();
            float sf = globalScaleFactor;
            ImVec2 rectMin = ImVec2(wp.x + s.position.x * sf, wp.y + s.position.y * sf);
            ImVec2 rectMax = ImVec2(rectMin.x + s.size.x * sf, rectMin.y + s.size.y * sf);
            dl->AddRect(rectMin, rectMax, ColU32(s.borderColor), s.cornerRadius * sf, 0, s.borderThickness * sf);
        }
    }







    inline void DrawShape_DrawBorder(ImDrawList* dlEffective, ShapeItem& s, float scaleFactor, ImVec2 c, ImVec2 wp) {
        if (s.borderThickness > 0.0f) {
            if (s.type == ShapeType::Rectangle) {
                std::vector<ImVec2> borderPoly;
                BuildRectPoly(
                    borderPoly,
                    ImVec2(wp.x + s.position.x * scaleFactor, wp.y + s.position.y * scaleFactor),
                    ImVec2(s.size.x * scaleFactor, s.size.y * scaleFactor),
                    s.cornerRadius * scaleFactor,
                    c,
                    s.rotation
                );
                dlEffective->AddPolyline(borderPoly.data(), (int)borderPoly.size(), ColU32(s.borderColor), true, s.borderThickness * scaleFactor);
            }
            else if (s.type == ShapeType::Circle) {
                float rx = s.size.x * 0.5f * scaleFactor;
                float ry = s.size.y * 0.5f * scaleFactor;
                std::vector<ImVec2> borderPoly;
                BuildCirclePoly(borderPoly, c, rx, ry, s.rotation);
                dlEffective->AddPolyline(borderPoly.data(), (int)borderPoly.size(), ColU32(s.borderColor), true, s.borderThickness * scaleFactor);
            }
        }
    }

    inline void DrawShape_DrawText(ImDrawList* dlEffective, ShapeItem& s, float scaleFactor, ImVec2 wp) {
        if (s.hasText) {
            float computedTextSize = s.textSize * scaleFactor;
            if (s.dynamicTextSize && s.baseSize.x > 0.0f) {
                computedTextSize = s.textSize * scaleFactor * (s.size.x / s.baseSize.x);
            }
            ImVec2 textPos = ImVec2(
                wp.x + s.position.x * scaleFactor + s.textPosition.x * scaleFactor,
                wp.y + s.position.y * scaleFactor + s.textPosition.y * scaleFactor
            );
            ImFont* font = nullptr;
            ImGuiIO& io = ImGui::GetIO();
            if (s.textFont >= 0 && s.textFont < io.Fonts->Fonts.Size) {
                font = io.Fonts->Fonts[s.textFont];
            }
            ImVec2 textDimensions;
            if (font) {
                textDimensions = font->CalcTextSizeA(computedTextSize, FLT_MAX, 0.0f, s.text.c_str());
                if (s.textAlignment == 1) {
                    textPos.x = wp.x + s.position.x * scaleFactor + (s.size.x * scaleFactor - textDimensions.x) * 0.5f;
                }
                else if (s.textAlignment == 2) {
                    textPos.x = wp.x + s.position.x * scaleFactor + (s.size.x * scaleFactor - textDimensions.x);
                }
            }
            float angleRadians = s.textRotation * (3.1415926535f / 180.0f);
            ImU32 col = ColU32(s.textColor);
            if (font) ImGui::PushFont(font);
            if (fabs(angleRadians) < 1e-4f) {
                dlEffective->AddText(font ? font : ImGui::GetFont(), computedTextSize, textPos, col, s.text.c_str());
            }
            else {
                AddTextRotated(dlEffective, font, computedTextSize, textPos, col, s.text.c_str(), angleRadians, ImVec2(0.5f, 0.5f));
            }
            if (font) ImGui::PopFont();
        }
    }


    inline void DrawShape_FinalOnClick(ShapeItem& s) {
        if (s.shouldCallOnClick) {
            DispatchEvent(s, "onClick");
            s.shouldCallOnClick = false;
        }
    }







    inline void DrawShape(ImDrawList* dl, ShapeItem& s, ImVec2 wp) {
        if (!s.visible) return;
        float scaleFactor = globalScaleFactor;
        ImDrawList* dlEffective = (s.forceOverlap && s.allowItemOverlap) ? ImGui::GetForegroundDrawList() : dl;
        ImVec2 c = ImVec2(
            wp.x + s.position.x * scaleFactor + s.size.x * 0.5f * scaleFactor,
            wp.y + s.position.y * scaleFactor + s.size.y * 0.5f * scaleFactor
        );
        ImVec2 cImage = ImVec2(
            s.position.x * scaleFactor + s.size.x * 0.5f * scaleFactor,
            s.position.y * scaleFactor + s.size.y * 0.5f * scaleFactor
        );
        float sr = s.shadowRotation;
        float totalrot = s.rotation + sr;
        ImVec4 drawColor = s.fillColor;
        {
            DrawShape_Shadow(dlEffective, s, wp, scaleFactor, c, totalrot);
        }
        {
            DrawShape_Blur(dlEffective, s, wp, scaleFactor, c);
        }
        std::vector<ImVec2> poly;
        {
            BuildMainShapePoly(s, wp, scaleFactor, c, poly);
        }
        {
            DrawShape_LoadEmbeddedImageIfNeeded(s);
        }
        {
            DrawShape_DrawEmbeddedImageIfAny(dlEffective, s, scaleFactor, cImage, poly);
        }
        if (s.isButton) {
            DrawShape_ProcessButtonLogic(dlEffective, s, scaleFactor, wp, drawColor);
        }
        {
            DrawShape_Fill(dlEffective, s, poly, scaleFactor, drawColor);
        }
        if (s.openWindow) {
            DrawShape_RenderChildWindow(s, wp, scaleFactor, poly);
            return;
        }
        {
            DrawShape_DrawBorder(dlEffective, s, scaleFactor, c, wp);
        }
        if (s.useGlass) {
        }
        {
            DrawShape_DrawText(dlEffective, s, scaleFactor, wp);
        }
        {
            DrawShape_FinalOnClick(s);
        }
    }




    //static std::vector<ShapeItem*> selectedShapes;
    static int lastSelectedLayerIndex = -1;
    static int lastSelectedShapeIndex = -1;
    static int selectedLayerIndex = 0, selectedShapeIndex = -1;



    inline void RenderAllRegisteredWindows()
    {
        for (auto& [winName, windowData] : g_windowsMap)
        {
            if (windowData.isChildWindow)
                continue;
            if (!windowData.isOpen)
                continue;
            ImGui::Begin(winName.c_str(), &windowData.isOpen);
            if (windowData.renderFunc)
                windowData.renderFunc();
            std::stable_sort(windowData.layers.begin(), windowData.layers.end(), CompareLayersByZOrder);
            for (auto& layer : windowData.layers)
            {
                if (!layer.visible)
                    continue;
                std::stable_sort(layer.shapes.begin(), layer.shapes.end(), CompareShapesByZOrder);
                for (auto& shape : layer.shapes)
                {
                    if (shape.ownerWindow == winName && shape.parent == nullptr)
                        DrawShape(ImGui::GetWindowDrawList(), shape, ImGui::GetWindowPos());
                }
            }
            ImGui::End();
        }
    }




    inline void MarkSceneUpdated() {
        //
        DesignManager::sceneUpdated = false;
        shouldCaptureScene = true;
        glFinish();
    }
    //
    inline float NormalizeProgress(float progress) {
        return progress / 100.0f;
    }

    //
//
//
    inline void UpdateShapeTransforms_Unified(GLFWwindow* window, float deltaTime)
    {
        // Loop through each window: g_windowsMap is now <std::string, WindowData>
        for (auto& [winName, windowData] : g_windowsMap)
        {
            ImGuiWindow* imguiWindow = ImGui::FindWindowByName(winName.c_str());
            ImVec2 currentWindowSize = (imguiWindow != nullptr) ? imguiWindow->Size : GetWindowSize(window);

            // Layers belonging to the window: windowData.layers
            for (auto& layer : windowData.layers)
            {
                // Loop for each shape in the layer
                for (auto& shape : layer.shapes)
                {
                    // Calculate base values:
                    ImVec2 effectiveBasePos = shape.basePosition + shape.baseKeyOffset;
                    ImVec2 effectiveBaseSize = shape.baseSize + shape.baseKeySizeOffset;
                    float effectiveBaseRot = shape.baseRotation + shape.baseKeyRotationOffset;

                    // Variables to accumulate the values to be blended:
                    float blendedPosX = 0.0f, blendedPosY = 0.0f, blendedSizeX = 0.0f, blendedSizeY = 0.0f, blendedRot = 0.0f;
                    int countPosX = 0, countPosY = 0, countSizeX = 0, countSizeY = 0, countRot = 0;

                    // Loop through shape keys to perform calculations:
                    for (auto& key : shape.shapeKeys)
                    {
                        float currentDim = 0.0f, startDim = 0.0f, endDim = 0.0f;
                        float baseVal = 0.0f, targetVal = 0.0f;

                        if (key.type == ShapeKeyType::PositionX)
                        {
                            currentDim = currentWindowSize.x;
                            startDim = key.startWindowSize.x;
                            endDim = key.endWindowSize.x;
                            baseVal = effectiveBasePos.x;
                            targetVal = key.targetValue.x;
                        }
                        else if (key.type == ShapeKeyType::PositionY)
                        {
                            currentDim = currentWindowSize.y;
                            startDim = key.startWindowSize.y;
                            endDim = key.endWindowSize.y;
                            baseVal = effectiveBasePos.y;
                            targetVal = key.targetValue.y;
                        }
                        else if (key.type == ShapeKeyType::SizeX)
                        {
                            currentDim = currentWindowSize.x;
                            startDim = key.startWindowSize.x;
                            endDim = key.endWindowSize.x;
                            baseVal = effectiveBaseSize.x;
                            targetVal = key.targetValue.x;
                        }
                        else if (key.type == ShapeKeyType::SizeY)
                        {
                            currentDim = currentWindowSize.y;
                            startDim = key.startWindowSize.y;
                            endDim = key.endWindowSize.y;
                            baseVal = effectiveBaseSize.y;
                            targetVal = key.targetValue.y;
                        }
                        else if (key.type == ShapeKeyType::Rotation)
                        {
                            currentDim = currentWindowSize.x; // In this case, we use the window width.
                            startDim = key.startWindowSize.x;
                            endDim = key.endWindowSize.x;
                            baseVal = effectiveBaseRot;
                            targetVal = key.targetRotation;
                        }

                        // t value: normalized progress based on the window size range
                        float t = (endDim != startDim) ? std::clamp((currentDim - startDim) / (endDim - startDim), 0.0f, 1.0f)
                            : ((currentDim < startDim) ? 0.0f : 1.0f);
                        key.value = t * 100.0f;

                        // Lerp function: performs interpolation between baseVal and targetVal by t.
                        float computedVal = Lerp(baseVal, targetVal, t);
                        if (key.type == ShapeKeyType::PositionX || key.type == ShapeKeyType::SizeX)
                            computedVal += key.offset.x;
                        else if (key.type == ShapeKeyType::PositionY || key.type == ShapeKeyType::SizeY)
                            computedVal += key.offset.y;
                        else if (key.type == ShapeKeyType::Rotation)
                            computedVal += key.rotationOffset;

                        // Add the computed value
                        if (key.type == ShapeKeyType::PositionX) { blendedPosX += computedVal; countPosX++; }
                        else if (key.type == ShapeKeyType::PositionY) { blendedPosY += computedVal; countPosY++; }
                        else if (key.type == ShapeKeyType::SizeX) { blendedSizeX += computedVal; countSizeX++; }
                        else if (key.type == ShapeKeyType::SizeY) { blendedSizeY += computedVal; countSizeY++; }
                        else if (key.type == ShapeKeyType::Rotation) { blendedRot += computedVal;  countRot++; }
                    }

                    // Shape key results based on the calculated averages:
                    ImVec2 shapeKeyResultPosition = (countPosX > 0 || countPosY > 0)
                        ? ImVec2((countPosX > 0 ? blendedPosX / countPosX : effectiveBasePos.x),
                            (countPosY > 0 ? blendedPosY / countPosY : effectiveBasePos.y))
                        : effectiveBasePos;

                    ImVec2 shapeKeyResultSize = (countSizeX > 0 || countSizeY > 0)
                        ? ImVec2((countSizeX > 0 ? blendedSizeX / countSizeX : effectiveBaseSize.x),
                            (countSizeY > 0 ? blendedSizeY / countSizeY : effectiveBaseSize.y))
                        : effectiveBaseSize;

                    float shapeKeyResultRotation = (countRot > 0) ? (blendedRot / countRot) : effectiveBaseRot;

                    // Animation bases: if updating, we use the shape key results.
                    ImVec2 animationBasePos = shape.updateAnimBaseOnResize ? shapeKeyResultPosition : effectiveBasePos;
                    ImVec2 animationBaseSize = shape.updateAnimBaseOnResize ? shapeKeyResultSize : effectiveBaseSize;
                    float animationBaseRot = shape.updateAnimBaseOnResize ? shapeKeyResultRotation : effectiveBaseRot;

                    // If there is a current animation:
                    if (shape.currentAnimation && shape.currentAnimation->isPlaying)
                    {
                        auto* anim = shape.currentAnimation;
                        switch (anim->behavior)
                        {
                        case ButtonAnimation::AnimationBehavior::PlayOnceAndStay:
                            anim->progress = std::min(1.0f, anim->progress + deltaTime * std::fabs(anim->speed));
                            if (anim->progress >= 1.0f)
                            {
                                if (!anim->hasStartedRepeatCount)
                                {
                                    anim->hasStartedRepeatCount = true;
                                    anim->remainingRepeats = (anim->repeatCount <= 0) ? -1 : anim->repeatCount;
                                }
                                if (anim->remainingRepeats < 0)
                                    anim->progress = 0.0f;
                                else
                                {
                                    anim->remainingRepeats--;
                                    if (anim->remainingRepeats > 0)
                                        anim->progress = 0.0f;
                                    else
                                    {
                                        anim->isPlaying = false;
                                        shape.currentAnimation = nullptr;
                                        anim->hasStartedRepeatCount = false;
                                    }
                                }
                                anim->persistentPositionOffset = (anim->animationTargetPosition - animationBasePos);
                                anim->persistentSizeOffset = (anim->animationTargetSize - animationBaseSize);
                                anim->persistentRotationOffset = anim->transformRotation - animationBaseRot;
                            }
                            break;
                        case ButtonAnimation::AnimationBehavior::PlayOnceAndReverse:
                            anim->progress += deltaTime * anim->speed;
                            if (anim->progress >= 1.0f)
                            {
                                anim->progress = 1.0f;
                                anim->speed = -std::fabs(anim->speed);
                            }
                            if (anim->progress <= 0.0f)
                            {
                                anim->progress = 0.0f;
                                if (!anim->hasStartedRepeatCount)
                                {
                                    anim->hasStartedRepeatCount = true;
                                    anim->remainingRepeats = (anim->repeatCount <= 0) ? -1 : anim->repeatCount;
                                }
                                if (anim->remainingRepeats < 0)
                                    anim->speed = std::fabs(anim->speed);
                                else
                                {
                                    anim->remainingRepeats--;
                                    if (anim->remainingRepeats > 0)
                                        anim->speed = std::fabs(anim->speed);
                                    else
                                    {
                                        anim->speed = std::fabs(anim->speed);
                                        anim->isPlaying = false;
                                        shape.currentAnimation = nullptr;
                                        anim->hasStartedRepeatCount = false;
                                    }
                                }
                            }
                            break;
                        case ButtonAnimation::AnimationBehavior::Toggle:
                            if (!anim->toggleState)
                            {
                                anim->progress = std::min(1.0f, anim->progress + deltaTime * std::fabs(anim->speed));
                                if (anim->progress >= 1.0f)
                                {
                                    anim->toggleState = true;
                                    anim->isPlaying = false;
                                    shape.currentAnimation = nullptr;
                                    anim->persistentPositionOffset = (anim->animationTargetPosition - animationBasePos);
                                    anim->persistentSizeOffset = (anim->animationTargetSize - animationBaseSize);
                                    anim->persistentRotationOffset = anim->transformRotation - animationBaseRot;
                                }
                            }
                            else
                            {
                                anim->progress = std::max(0.0f, anim->progress - deltaTime * std::fabs(anim->speed));
                                if (anim->progress <= 0.0f)
                                {
                                    anim->toggleState = false;
                                    anim->isPlaying = false;
                                    shape.currentAnimation = nullptr;
                                    anim->persistentPositionOffset = ImVec2(0, 0);
                                    anim->persistentSizeOffset = ImVec2(0, 0);
                                    anim->persistentRotationOffset = 0.0f;
                                }
                            }
                            break;
                        case ButtonAnimation::AnimationBehavior::PlayWhileHoldingAndReverseOnRelease:
                            if (shape.isHeld)
                                anim->progress = std::min(1.0f, anim->progress + deltaTime * std::fabs(anim->speed));
                            else
                            {
                                anim->progress = std::max(0.0f, anim->progress - deltaTime * std::fabs(anim->speed));
                                if (anim->progress <= 0.0f)
                                {
                                    anim->isPlaying = false;
                                    shape.currentAnimation = nullptr;
                                }
                            }
                            break;
                        case ButtonAnimation::AnimationBehavior::PlayWhileHoldingAndStay:
                            if (shape.isHeld)
                            {
                                anim->progress += deltaTime * std::fabs(anim->speed) * anim->playbackDirection;
                                if (anim->progress >= 1.0f)
                                {
                                    anim->progress = 1.0f;
                                    anim->playbackDirection = -1.0f;
                                }
                                else if (anim->progress <= 0.0f)
                                {
                                    anim->progress = 0.0f;
                                    anim->playbackDirection = 1.0f;
                                }
                            }
                            else
                            {
                                anim->isPlaying = false;
                                shape.currentAnimation = nullptr;
                                float t = anim->progress;
                                if (anim->interpolationMethod == ButtonAnimation::InterpolationMethod::EaseInOut)
                                    t = t * t * (3.0f - 2.0f * t);
                                anim->persistentPositionOffset = ImVec2(
                                    Lerp(0.0f, anim->animationTargetPosition.x - animationBasePos.x, t),
                                    Lerp(0.0f, anim->animationTargetPosition.y - animationBasePos.y, t)
                                );
                                anim->persistentSizeOffset = ImVec2(
                                    Lerp(0.0f, anim->animationTargetSize.x - animationBaseSize.x, t),
                                    Lerp(0.0f, anim->animationTargetSize.y - animationBaseSize.y, t)
                                );
                                anim->persistentRotationOffset = Lerp(0.0f, anim->transformRotation - animationBaseRot, t);
                            }
                            break;
                        }

                        // Calculate animation offsets by applying interpolation.
                        float tInterp = anim->progress;
                        if (anim->interpolationMethod == ButtonAnimation::InterpolationMethod::EaseInOut)
                            tInterp = tInterp * tInterp * (3.0f - 2.0f * tInterp);
                        ImVec2 animPosOffset = ImVec2(
                            Lerp(0.0f, anim->animationTargetPosition.x - animationBasePos.x, tInterp),
                            Lerp(0.0f, anim->animationTargetPosition.y - animationBasePos.y, tInterp)
                        );
                        ImVec2 animSizeOffset = ImVec2(
                            Lerp(0.0f, anim->animationTargetSize.x - animationBaseSize.x, tInterp),
                            Lerp(0.0f, anim->animationTargetSize.y - animationBaseSize.y, tInterp)
                        );
                        float animRotOffset = Lerp(0.0f, anim->transformRotation - animationBaseRot, tInterp);

                        // Final shape transform values:
                        shape.position = shapeKeyResultPosition + animPosOffset;
                        shape.size = shapeKeyResultSize + animSizeOffset;
                        shape.rotation = shapeKeyResultRotation + animRotOffset;
                    }
                    else
                    {
                        // If animation is not playing, apply offsets from onClick animations cumulatively.
                        ImVec2 cumulativePos = shapeKeyResultPosition;
                        ImVec2 cumulativeSize = shapeKeyResultSize;
                        float cumulativeRot = shapeKeyResultRotation;
                        for (auto& a : shape.onClickAnimations)
                        {
                            bool toggleOn = (a.behavior == ButtonAnimation::AnimationBehavior::Toggle && a.toggleState);
                            bool playedOnce = (a.behavior == ButtonAnimation::AnimationBehavior::PlayOnceAndStay && a.progress >= 1.0f);
                            bool heldStay = (a.behavior == ButtonAnimation::AnimationBehavior::PlayWhileHoldingAndStay && !a.isPlaying && a.progress > 0.0f);
                            if (toggleOn || playedOnce || heldStay)
                            {
                                cumulativePos += a.persistentPositionOffset;
                                cumulativeSize += a.persistentSizeOffset;
                                cumulativeRot += a.persistentRotationOffset;
                            }
                        }
                        shape.position = cumulativePos;
                        shape.size = cumulativeSize;
                        shape.rotation = cumulativeRot;
                    }
                } // end for each shape
            } // end for each layer
        } // end for each window
    }



    inline ImVec2 ComputeChainOffset(const ShapeItem& shape) {
        const ChainAnimation& chain = shape.chainAnimation;
        ImVec2 offset(0, 0);
        if (!chain.isPlaying) {
            return offset; // If the animation is not playing, return zero offset
        }
        if (!chain.reverseMode) {
            // Forward animation
            for (int i = 0; i <= chain.currentStep; i++) {
                const ButtonAnimation& anim = chain.steps[i].animation;
                float t = (i < chain.currentStep) ? 1.0f : anim.progress;
                offset.x += Lerp(0.0f, anim.animationTargetPosition.x - shape.basePosition.x, t);
                offset.y += Lerp(0.0f, anim.animationTargetPosition.y - shape.basePosition.y, t);
            }
        }
        else {
            // Backward animation
            for (int i = chain.steps.size() - 1; i >= chain.currentStep; i--) {
                const ButtonAnimation& anim = chain.steps[i].animation;
                float t = (i > chain.currentStep) ? 1.0f : anim.progress;
                offset.x += Lerp(0.0f, anim.animationTargetPosition.x - shape.basePosition.x, t);
                offset.y += Lerp(0.0f, anim.animationTargetPosition.y - shape.basePosition.y, t);
            }
        }
        return offset;
    }

    inline void UpdateChainAnimations(float deltaTime)
    {
        for (auto& [winName, windowData] : DesignManager::g_windowsMap)
        {
            for (auto& layer : windowData.layers)
            {
                for (auto& shape : layer.shapes)
                {
                    ChainAnimation& chain = shape.chainAnimation;
                    if (chain.isPlaying && !chain.steps.empty())
                    {
                        ButtonAnimation& anim = chain.steps[chain.currentStep].animation;
                        float stepProgress = anim.progress;
                        ImVec2 startPos, endPos;
                        if (!chain.reverseMode) {
                            if (chain.currentStep == 0)
                                startPos = shape.basePosition;
                            else
                                startPos = chain.steps[chain.currentStep - 1].animation.animationTargetPosition;
                            endPos = anim.animationTargetPosition;
                            shape.position = Lerp(startPos, endPos, stepProgress);
                            anim.progress += deltaTime * std::fabs(anim.speed);
                            if (anim.progress >= 1.0f)
                            {
                                anim.progress = 1.0f;
                                if (chain.steps[chain.currentStep].onStepComplete)
                                    chain.steps[chain.currentStep].onStepComplete();
                                if (chain.currentStep < (int)chain.steps.size() - 1)
                                {
                                    chain.currentStep++;
                                    chain.steps[chain.currentStep].animation.progress = 0.0f;
                                }
                                else
                                {
                                    chain.isPlaying = false;
                                    chain.toggled = true;
                                    shape.position = endPos;
                                }
                            }
                        }
                        else {
                            startPos = anim.animationTargetPosition;
                            if (chain.currentStep == 0)
                                endPos = shape.basePosition;
                            else
                                endPos = chain.steps[chain.currentStep - 1].animation.animationTargetPosition;
                            shape.position = Lerp(startPos, endPos, 1.0f - stepProgress);
                            anim.progress -= deltaTime * std::fabs(anim.speed);
                            if (anim.progress <= 0.0f)
                            {
                                anim.progress = 0.0f;
                                if (chain.currentStep > 0)
                                {
                                    chain.currentStep--;
                                    chain.steps[chain.currentStep].animation.progress = 1.0f;
                                }
                                else
                                {
                                    chain.isPlaying = false;
                                    chain.toggled = false;
                                    shape.position = shape.basePosition;
                                }
                            }
                        }
                    }
                    else if (chain.toggled)
                    {
                        if (!chain.steps.empty())
                            shape.position = chain.steps.back().animation.animationTargetPosition;
                    }
                    else
                    {
                        shape.position = shape.basePosition;
                    }
                }
            }
        }
    }

    static int GetUniqueShapeID() {
        static int nextShapeID = 1000; // Starting value
        static std::unordered_set<int> usedIDs;
        // If nextShapeID is already used, increment until a suitable value is found.
        while (usedIDs.count(nextShapeID))
            ++nextShapeID;
        usedIDs.insert(nextShapeID);
        return nextShapeID++;
    }


    inline void ShowChainAnimationGUI() {
        ImGui::Begin("Animation Manager");

        std::vector<ShapeItem*> allShapes;
        for (auto& [winName, windowData] : DesignManager::g_windowsMap) {
            for (auto& layer : windowData.layers) {
                for (auto& shape : layer.shapes) {
                    allShapes.push_back(&shape);
                }
            }
        }

        static ShapeItem* selectedShape = nullptr;
        ImGui::Text("Select Animation Owner Shape:");
        if (ImGui::BeginListBox("##ShapeList", ImVec2(-1, 150))) {
            for (int i = 0; i < allShapes.size(); i++) {
                bool isSelected = (selectedShape == allShapes[i]);
                if (ImGui::Selectable(allShapes[i]->name.c_str(), isSelected)) {
                    selectedShape = allShapes[i];
                }
                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndListBox();
        }

        if (selectedShape) {
            ImGui::Separator();
            ImGui::Text("Selected Shape: %s", selectedShape->name.c_str());

            ChainAnimation& chain = selectedShape->chainAnimation;
            if (chain.isPlaying) {
                ImGui::Text("Animation %s: Step %d/%d",
                    (!chain.reverseMode ? "Forward" : "Backward"),
                    chain.currentStep + 1,
                    (int)chain.steps.size());
                if (chain.currentStep < chain.steps.size()) {
                    float prog = chain.steps[chain.currentStep].animation.progress;
                    ImGui::ProgressBar(prog, ImVec2(-1, 0));
                }
            }
            else {
                ImGui::Text("Animation Inactive (Status: %s)",
                    (chain.toggled ? "On" : "Off"));
            }

            if (ImGui::CollapsingHeader("Animation Steps", ImGuiTreeNodeFlags_DefaultOpen)) {
                for (int i = 0; i < chain.steps.size(); i++) {
                    ImGui::PushID(i);
                    ChainAnimationStep& step = chain.steps[i];
                    if (ImGui::TreeNode(("Step " + std::to_string(i + 1)).c_str())) {
                        char nameBuf[128];
                        std::strncpy(nameBuf, step.animation.name.c_str(), sizeof(nameBuf));
                        if (ImGui::InputText("Step Name", nameBuf, sizeof(nameBuf)))
                            step.animation.name = nameBuf;
                        ImGui::DragFloat("Duration", &step.animation.duration, 0.1f, 0.1f, 20.0f);
                        ImGui::DragFloat("Speed", &step.animation.speed, 0.1f, 0.1f, 10.0f);
                        ImGui::DragFloat2("Target Position", (float*)&step.animation.animationTargetPosition, 1.0f);
                        ImGui::TreePop();
                    }
                    ImGui::PopID();
                }
                if (ImGui::Button("Add New Step")) {
                    ChainAnimationStep newStep;
                    newStep.animation.name = "New Step";
                    newStep.animation.duration = 1.0f;
                    newStep.animation.speed = 1.0f;
                    newStep.animation.animationTargetPosition = ImVec2(100, 100);
                    newStep.animation.progress = 0.0f;
                    chain.steps.push_back(newStep);
                }
            }

            ImGui::Separator();
            ImGui::Text("Trigger Button Group");
            static std::map<int, std::vector<ShapeItem*>> triggerGroups;
            std::vector<ShapeItem*>& triggerGroup = triggerGroups[selectedShape->id];

            ImGui::Text("Buttons in Group:");
            for (int i = 0; i < triggerGroup.size(); i++) {
                ImGui::PushID(i);
                ImGui::Text("%s", triggerGroup[i]->name.c_str());
                ImGui::SameLine();
                if (ImGui::Button("Remove")) {
                    ShapeItem* buttonToRemove = triggerGroup[i];
                    triggerGroup.erase(triggerGroup.begin() + i);
                    buttonToRemove->targetShapeID = 0;
                    i--;
                }
                ImGui::PopID();
            }

            std::vector<ShapeItem*> buttonShapes;
            for (auto shape : allShapes) {
                if (shape->isButton) {
                    buttonShapes.push_back(shape);
                }
            }

            static int selectedButtonIndex = -1;
            if (!buttonShapes.empty()) {
                ImGui::Text("Add Button:");
                if (ImGui::BeginCombo("##AddButton", selectedButtonIndex >= 0 ? buttonShapes[selectedButtonIndex]->name.c_str() : "Select")) {
                    for (int i = 0; i < buttonShapes.size(); i++) {
                        bool isSelected = (selectedButtonIndex == i);
                        if (ImGui::Selectable(buttonShapes[i]->name.c_str(), isSelected)) {
                            selectedButtonIndex = i;
                        }
                        if (isSelected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
                if (ImGui::Button("Add to Group") && selectedButtonIndex >= 0) {
                    ShapeItem* newButton = buttonShapes[selectedButtonIndex];
                    bool alreadyAdded = false;
                    for (auto btn : triggerGroup) {
                        if (btn->id == newButton->id) {
                            alreadyAdded = true;
                            break;
                        }
                    }
                    if (!alreadyAdded) {
                        triggerGroup.push_back(newButton);
                        newButton->targetShapeID = selectedShape->id;
                    }
                }
            }

            if (ImGui::Button("Trigger Group Animation")) {
                for (auto btn : triggerGroup) {
                    if (btn->targetShapeID != 0) {
                        ShapeItem* target = FindShapeByID(btn->targetShapeID);
                        if (target && !target->chainAnimation.steps.empty()) {
                            ChainAnimation& chain = target->chainAnimation;
                            if (!chain.isPlaying) {
                                if (!chain.toggled) {
                                    chain.reverseMode = false;
                                    chain.currentStep = 0;
                                    chain.steps[0].animation.progress = 0.0f;
                                }
                                else {
                                    chain.reverseMode = true;
                                    chain.currentStep = (int)chain.steps.size() - 1;
                                    chain.steps[chain.currentStep].animation.progress = 1.0f;
                                }
                                chain.isPlaying = true;
                            }
                        }
                    }
                }
            }
        }

        ImGui::End();
    }







    inline void DrawAll(ImDrawList* dl)
    {
        //
        const char* currentWindowName = ImGui::GetCurrentWindow()->Name;
        std::string windowNameStr = currentWindowName;

        //
        if (g_windowsMap.empty())
            return;

        //
        bool didHideGlass = false;

        //
        if (shouldCaptureScene)
        {
            //
            for (auto& [oWindowName, WindowData] : g_windowsMap)
            {
                for (auto& layer : WindowData.layers)
                {
                    for (auto& shape : layer.shapes)
                    {
                        if (shape.useGlass && shape.visible)
                        {
                            shape.visible = false;
                            didHideGlass = true;
                        }
                    }
                }
            }
        }

        for (auto& [oWindowName, windowData] : g_windowsMap)
        {
            if (windowNameStr.find(oWindowName) == std::string::npos)
                continue; // Skip the loop if the related window is not found.

            // Sort the layers of the related window:
            std::stable_sort(windowData.layers.begin(), windowData.layers.end(), CompareLayersByZOrder);

            for (auto& layer : windowData.layers)
            {
                if (!layer.visible)
                    continue;

                // Sort the shapes in the layer:
                std::stable_sort(layer.shapes.begin(), layer.shapes.end(), CompareShapesByZOrder);

                for (auto& shape : layer.shapes)
                {
                    // If the shape's owner matches the window name and the shape does not have any sub-shapes:
                    if (shape.ownerWindow == oWindowName && shape.parent == nullptr)
                    {
                        DrawShape(dl, shape, ImGui::GetWindowPos());
                    }
                }
            }
        }

        //
        if (shouldCaptureScene)
        {
            if (didHideGlass)
            {
                for (auto& [oWindowName, WindowData] : g_windowsMap)
                {
                    for (auto& layer : WindowData.layers)
                    {
                        for (auto& shape : layer.shapes)
                        {
                            if (shape.useGlass && !shape.visible)
                                shape.visible = true;
                        }
                    }
                }
            }
            shouldCaptureScene = false;
        }
    }









    inline std::string SanitizeVariableName(const std::string& name) {
        std::string varName = name;
        varName.erase(std::remove(varName.begin(), varName.end(), ' '),
            varName.end());
        varName.erase(std::remove_if(varName.begin(), varName.end(),
            [](char c) {
                return !isalnum(c) && c != '_';
            }),
            varName.end());
        if (!varName.empty() && isdigit(varName[0])) {
            varName = "_" + varName;
        }
        if (varName.empty()) {
            varName = "Var";
        }
        return varName;
    }
    


    inline std::string escapeNewlines(const std::string& input) {
        std::string output;
        for (char ch : input) {
            if (ch == '\n') {
                output += "\\n";
            }
            // If the text contains '\r' and you want to handle it as well:
            else if (ch == '\r') {
                // On Windows systems, the "\r\n" pair may occur, so take that into account.
                output += "\\r";
            }
            else {
                output += ch;
            }
        }
        return output;
    }

    inline void CopyToClipboard(const std::string& text) {
        ImGui::SetClipboardText(text.c_str());
    }

    // -------------------------
    // Helper: Function to generate ButtonAnimation code (Updated)
    // -------------------------

    inline std::string GenerateButtonAnimationCode(const ButtonAnimation& anim)
    {
        std::string code;
        code += "ShapeBuilder().createAnimation()\n";
        code += "    .setName(\"" + anim.name + "\")\n";
        code += "    .setDuration(" + std::to_string(anim.duration) + "f)\n";
        code += "    .setSpeed(" + std::to_string(anim.speed) + "f)\n";
        code += "    .setTargetPosition(ImVec2(" + std::to_string(anim.animationTargetPosition.x) + "f, " +
            std::to_string(anim.animationTargetPosition.y) + "f))\n";
        code += "    .setTargetSize(ImVec2(" + std::to_string(anim.animationTargetSize.x) + "f, " +
            std::to_string(anim.animationTargetSize.y) + "f))\n";
        code += "    .setTransformRotation(" + std::to_string(anim.transformRotation) + "f)\n";
        code += "    .setRepeatCount(" + std::to_string(anim.repeatCount) + ")\n";
        code += "    .setPlaybackOrder(PlaybackOrder::";
        switch (anim.playbackOrder)
        {
        case PlaybackOrder::Sirali:
            code += "Sirali"; break;
        case PlaybackOrder::HemenArkasina:
            code += "HemenArkasina"; break;
        default:
            code += "Sirali"; break;
        }
        code += ")\n";
        code += "    .setInterpolationMethod(ButtonAnimation::InterpolationMethod::";
        switch (anim.interpolationMethod)
        {
        case ButtonAnimation::InterpolationMethod::Linear:
            code += "Linear"; break;
        case ButtonAnimation::InterpolationMethod::EaseInOut:
            code += "EaseInOut"; break;
        default:
            code += "Linear"; break;
        }
        code += ")\n";
        code += "    .setTriggerMode(ButtonAnimation::TriggerMode::";
        switch (anim.triggerMode)
        {
        case ButtonAnimation::TriggerMode::OnClick:
            code += "OnClick"; break;
        case ButtonAnimation::TriggerMode::OnHover:
            code += "OnHover"; break;
        default:
            code += "OnClick"; break;
        }
        code += ")\n";
        code += "    .setBehavior(ButtonAnimation::AnimationBehavior::";
        switch (anim.behavior)
        {
        case ButtonAnimation::AnimationBehavior::PlayOnceAndStay:
            code += "PlayOnceAndStay"; break;
        case ButtonAnimation::AnimationBehavior::PlayOnceAndReverse:
            code += "PlayOnceAndReverse"; break;
        case ButtonAnimation::AnimationBehavior::Toggle:
            code += "Toggle"; break;
        case ButtonAnimation::AnimationBehavior::PlayWhileHoldingAndReverseOnRelease:
            code += "PlayWhileHoldingAndReverseOnRelease"; break;
        case ButtonAnimation::AnimationBehavior::PlayWhileHoldingAndStay:
            code += "PlayWhileHoldingAndStay"; break;
        default:
            code += "PlayOnceAndStay"; break;
        }
        code += ")\n";
        code += "    .build()";
        return code;
    }


    // -------------------------
    // Helper: Function that generates the ShapeKey code
    // -------------------------
    inline std::string GenerateShapeKeyCode(const ShapeKey& key)
    {
        std::string code;
        code += "ShapeBuilder::createShapeKey()\n";
        code += "    .setName(\"" + key.name + "\")\n";
        code += "    .setType(ShapeKeyType::";
        switch (key.type)
        {
        case ShapeKeyType::SizeX:
            code += "SizeX"; break;
        case ShapeKeyType::SizeY:
            code += "SizeY"; break;
        case ShapeKeyType::PositionX:
            code += "PositionX"; break;
        case ShapeKeyType::PositionY:
            code += "PositionY"; break;
        case ShapeKeyType::Rotation:
            code += "Rotation"; break;
        default:
            code += "SizeX"; break;
        }
        code += ")\n";
        code += "    .setStartWindowSize(ImVec2(" + std::to_string(key.startWindowSize.x) + "f, " +
            std::to_string(key.startWindowSize.y) + "f))\n";
        code += "    .setEndWindowSize(ImVec2(" + std::to_string(key.endWindowSize.x) + "f, " +
            std::to_string(key.endWindowSize.y) + "f))\n";
        if (key.type == ShapeKeyType::Rotation)
        {
            code += "    .setTargetRotation(" + std::to_string(key.targetRotation) + "f)\n";
            code += "    .setRotationOffset(" + std::to_string(key.rotationOffset) + "f)\n";
        }
        else
        {
            code += "    .setTargetValue(ImVec2(" + std::to_string(key.targetValue.x) + "f, " +
                std::to_string(key.targetValue.y) + "f))\n";
            code += "    .setOffset(ImVec2(" + std::to_string(key.offset.x) + "f, " +
                std::to_string(key.offset.y) + "f))\n";
        }
        code += "    .setValue(" + std::to_string(key.value) + "f)\n";
        code += "    .build()";
        return code;
    }

    // -------------------------
// Updated version of the GenerateSingleShapeCode function
// -------------------------
    inline std::string GenerateSingleShapeCode(const DesignManager::ShapeItem& shape)
    {
        using namespace DesignManager;
        std::string shapeVar = SanitizeVariableName(shape.name);
        std::string code;
        code += "// Single Shape: " + shape.name + "\n";
        int uniqueID = (shape.id > 0) ? shape.id : GetUniqueShapeID();

        code += "auto " + shapeVar + " = ShapeBuilder()\n";
        code += "    .setId(" + std::to_string(uniqueID) + ")\n";
        code += "    .setName(\"" + shape.name + "\")\n";
        code += "    .setOwnerWindow(\"" + shape.ownerWindow + "\")\n";
        if (shape.groupId != 0)
            code += "    .setGroupId(" + std::to_string(shape.groupId) + ")\n";
        // Base Transform
        code += "    .setBasePosition(ImVec2(" + std::to_string(shape.basePosition.x) + "f, " +
            std::to_string(shape.basePosition.y) + "f))\n";
        code += "    .setBaseSize(ImVec2(" + std::to_string(shape.baseSize.x) + "f, " +
            std::to_string(shape.baseSize.y) + "f))\n";
        code += "    .setBaseRotation(" + std::to_string(shape.baseRotation) + "f)\n";

        // If the final transform is different, optional:
        if (shape.position.x != shape.basePosition.x || shape.position.y != shape.basePosition.y)
            code += "    .setPosition(ImVec2(" + std::to_string(shape.position.x) + "f, " +
            std::to_string(shape.position.y) + "f))\n";
        if (shape.size.x != shape.baseSize.x || shape.size.y != shape.baseSize.y)
            code += "    .setSize(ImVec2(" + std::to_string(shape.size.x) + "f, " +
            std::to_string(shape.size.y) + "f))\n";

        // Style and appearance settings:
        if (shape.cornerRadius != 0.0f)
            code += "    .setCornerRadius(" + std::to_string(shape.cornerRadius) + "f)\n";
        if (shape.borderThickness != 0.0f)
            code += "    .setBorderThickness(" + std::to_string(shape.borderThickness) + "f)\n";

        auto writeColorLine = [&](const std::string& methodName, const ImVec4& color) -> std::string {
            return "    ." + methodName + "(ImVec4(" +
                std::to_string(color.x) + "f, " +
                std::to_string(color.y) + "f, " +
                std::to_string(color.z) + "f, " +
                std::to_string(color.w) + "f))\n";
            };
        code += writeColorLine("setFillColor", shape.fillColor);
        code += writeColorLine("setBorderColor", shape.borderColor);
        code += writeColorLine("setShadowColor", shape.shadowColor);

        if (shape.shadowSpread != ImVec4(0, 0, 0, 0))
            code += "    .setShadowSpread(ImVec4(" + std::to_string(shape.shadowSpread.x) + "f, " +
            std::to_string(shape.shadowSpread.y) + "f, " +
            std::to_string(shape.shadowSpread.z) + "f, " +
            std::to_string(shape.shadowSpread.w) + "f))\n";
        if (shape.shadowOffset != ImVec2(0, 0))
            code += "    .setShadowOffset(ImVec2(" + std::to_string(shape.shadowOffset.x) + "f, " +
            std::to_string(shape.shadowOffset.y) + "f))\n";
        if (shape.shadowUseCornerRadius)
            code += "    .setShadowUseCornerRadius(true)\n";
        if (shape.rotation != 0.0f)
            code += "    .setRotation(" + std::to_string(shape.rotation) + "f)\n";
        if (shape.blurAmount != 0.0f)
            code += "    .setBlurAmount(" + std::to_string(shape.blurAmount) + "f)\n";

        code += "    .setVisible(" + std::string(shape.visible ? "true" : "false") + ")\n";
        code += "    .setLocked(" + std::string(shape.locked ? "true" : "false") + ")\n";

        // Gradient settings:
        if (shape.useGradient)
            code += "    .setUseGradient(true)\n";
        if (shape.gradientRotation != 0.0f)
            code += "    .setGradientRotation(" + std::to_string(shape.gradientRotation) + "f)\n";
        if (shape.gradientInterpolation != ShapeItem::GradientInterpolation::Linear)
        {
            code += "    .setGradientInterpolation(DesignManager::ShapeItem::GradientInterpolation::";
            switch (shape.gradientInterpolation)
            {
            case ShapeItem::GradientInterpolation::Ease:     code += "Ease"; break;
            case ShapeItem::GradientInterpolation::Constant: code += "Constant"; break;
            case ShapeItem::GradientInterpolation::Cardinal: code += "Cardinal"; break;
            case ShapeItem::GradientInterpolation::BSpline:  code += "BSpline"; break;
            default: break;
            }
            code += ")\n";
        }
        if (!shape.colorRamp.empty())
        {
            code += "    .setColorRamp({\n";
            for (auto& ramp : shape.colorRamp)
            {
                code += "        {" + std::to_string(ramp.first) + "f, ImVec4(" +
                    std::to_string(ramp.second.x) + "f, " +
                    std::to_string(ramp.second.y) + "f, " +
                    std::to_string(ramp.second.z) + "f, " +
                    std::to_string(ramp.second.w) + "f)},\n";
            }
            code += "    })\n";
        }
        if (shape.useGlass)
        {
            code += "    .setUseGlass(true)\n";
            if (shape.glassBlur != 0.0f)
                code += "    .setGlassBlur(" + std::to_string(shape.glassBlur) + "f)\n";
            if (shape.glassAlpha != 0.0f)
                code += "    .setGlassAlpha(" + std::to_string(shape.glassAlpha) + "f)\n";
            code += writeColorLine("setGlassColor", shape.glassColor);
        }
        if (shape.zOrder != 0)
            code += "    .setZOrder(" + std::to_string(shape.zOrder) + ")\n";

        // Button properties:
        if (shape.isButton)
        {
            code += "    .setIsButton(true)\n";
            code += "    .setButtonBehavior(DesignManager::ShapeItem::ButtonBehavior::";
            switch (shape.buttonBehavior)
            {
            case ShapeItem::ButtonBehavior::SingleClick: code += "SingleClick"; break;
            case ShapeItem::ButtonBehavior::Toggle:      code += "Toggle"; break;
            case ShapeItem::ButtonBehavior::Hold:        code += "Hold"; break;
            }
            code += ")\n";
            code += "    .setUseOnClick(" + std::string(shape.useOnClick ? "true" : "false") + ")\n";
            code += writeColorLine("setHoverColor", shape.hoverColor);
            code += writeColorLine("setClickedColor", shape.clickedColor);
        }

        // Text properties:
        if (shape.hasText)
        {
            code += "    .setHasText(true)\n";
            code += "    .setText(\"" + escapeNewlines(shape.text) + "\")\n";
            code += writeColorLine("setTextColor", shape.textColor);
            code += "    .setTextSize(" + std::to_string(shape.textSize) + "f)\n";
            code += "    .setTextFont(" + std::to_string(shape.textFont) + ")\n";
            code += "    .setTextPosition(ImVec2(" + std::to_string(shape.textPosition.x) + "f, " +
                std::to_string(shape.textPosition.y) + "f))\n";
            code += "    .setTextRotation(" + std::to_string(shape.textRotation) + "f)\n";
            code += "    .setTextAlignment(" + std::to_string(shape.textAlignment) + ")\n";
        }

        // Child Window settings:
        code += "    .setOpenWindow(" + std::string(shape.isChildWindow ? "true" : "false") + ")\n";
        // Child Window settings:
        code += "    .setIsChildWindow(" + std::string(shape.isChildWindow ? "true" : "false") + ")\n";
        if (shape.isChildWindow)
        {
            code += "    .setChildWindowSync(" + std::string(shape.childWindowSync ? "true" : "false") + ")\n";
            code += "    .setToggleChildWindow(" + std::string(shape.toggleChildWindow ? "true" : "false") + ")\n";
            code += "    .setChildWindowGroupId(" + std::to_string(shape.childWindowGroupId) + ")\n";
            if (shape.targetShapeID > 0)
                code += "    .setTargetShapeID(" + std::to_string(shape.targetShapeID) + ")\n";
        }

        // Newly added properties:
        if (shape.updateAnimBaseOnResize) // Previous incorrect call was "setUpdateAnimationBaseOnResize"
            code += "    .setUpdateAnimBaseOnResize(true)\n";
        if (shape.hasEmbeddedImage)
            code += "    .setHasEmbeddedImage(true)\n";
        if (shape.embeddedImageIndex >= 0)
            code += "    .setEmbeddedImageIndex(" + std::to_string(shape.embeddedImageIndex) + ")\n";
        // Overlap/overlay options:
        code += "    .setAllowItemOverlap(" + std::string(shape.allowItemOverlap ? "true" : "false") + ")\n";
        if (shape.forceOverlap)
            code += "    .setForceOverlap(true)\n";
        if (shape.blockUnderlying)
            code += "    .setBlockUnderlying(true)\n";
        // If the shape type is not Rectangle:
        if (shape.type != ShapeType::Rectangle)
        {
            code += "    .setType(ShapeType::";
            switch (shape.type)
            {
            case ShapeType::Circle: code += "Circle"; break;
            default: code += "Rectangle"; break;
            }
            code += ")\n";
        }
        if (shape.dynamicTextSize)
            code += "    .setDynamicTextSize(true)\n";

        // OnClick animations (if any)
        if (!shape.onClickAnimations.empty())
        {
            for (const auto& anim : shape.onClickAnimations)
            {
                code += "    .addOnClickAnimation(" + GenerateButtonAnimationCode(anim) + ")\n";
            }
        }
        // Shape Keys (if any)
        if (!shape.shapeKeys.empty())
        {
            for (const auto& key : shape.shapeKeys)
            {
                code += "    .addShapeKey(" + GenerateShapeKeyCode(key) + ")\n";
            }
        }



        if (shape.isButton && shape.useOnClick)
        {
            std::string eventHandlerName = SanitizeVariableName(shape.name) + "_OnClick";
            code += "    .addEventHandler(\"onClick\", \"" + eventHandlerName + "\", [](ShapeItem& s) { " + eventHandlerName + "(); })\n";
        }
        code += "    .build();\n";

        return code;
    }
    // -------------------------
// New: Function to generate global Child Window Mappings code
// -------------------------
    inline std::string GenerateChildWindowMappingsCode() {
        std::string code;
        code += "// Global Child Window Mappings\n";
        code += "g_combinedChildWindowMappings.clear();\n";
        for (const auto& mapping : g_combinedChildWindowMappings) {
            code += "g_combinedChildWindowMappings.push_back(CombinedMapping{\n";
            code += "    " + std::to_string(mapping.shapeId) + ",\n";
            code += "    \"" + mapping.logicOp + "\",\n";
            code += "    {";
            for (size_t i = 0; i < mapping.buttonIds.size(); i++) {
                code += std::to_string(mapping.buttonIds[i]);
                if (i < mapping.buttonIds.size() - 1)
                    code += ", ";
            }
            code += "},\n";
            code += "    {";
            for (size_t i = 0; i < mapping.childWindowKeys.size(); i++) {
                code += "\"" + mapping.childWindowKeys[i] + "\"";
                if (i < mapping.childWindowKeys.size() - 1)
                    code += ", ";
            }
            code += "}\n";
            code += "});\n";
        }
        return code;
    }
    // -------------------------
// Updated version of the GenerateCodeForWindow function (using GenerateSingleShapeCode calls)
// -------------------------
    inline std::string GenerateCodeForWindow(const std::string& windowName)
    {
        using namespace DesignManager;
        if (g_windowsMap.find(windowName) == g_windowsMap.end()) {
            return "// No layer found in this window or window does not exist.\n";
        }

        std::string code;
        code += "// Generated code for window: " + windowName + "\n\n";

        std::string centralHeaderFileName = "GeneratedButtons_" + SanitizeVariableName(windowName) + ".h";
        std::ofstream centralHeader(centralHeaderFileName, std::ios::trunc);
        if (!centralHeader.is_open()) {
            return "// Unable to open central header file.\n";
        }
        centralHeader << "// This file was automatically generated for the " << windowName << " window. Please do not modify it directly.\n\n";

        const auto& winData = g_windowsMap.at(windowName);
        for (auto& layer : winData.layers)
        {
            std::string layerVar = SanitizeVariableName(layer.name);
            code += "// Layer: " + layer.name + "\n";
            code += "DesignManager::Layer " + layerVar + "(\"" + layer.name + "\");\n";
            if (layer.zOrder != 0)
                code += layerVar + ".zOrder = " + std::to_string(layer.zOrder) + ";\n";
            if (!layer.visible)
                code += layerVar + ".visible = false;\n";
            if (layer.locked)
                code += layerVar + ".locked = true;\n";
            code += "\n";

            for (auto& shape : layer.shapes)
            {
                code += GenerateSingleShapeCode(shape);
                code += layerVar + ".shapes.push_back(" + SanitizeVariableName(shape.name) + ");\n\n";
            }
            code += "g_windowsMap[\"" + windowName + "\"].layers.push_back(" + layerVar + ");\n\n";
        }
        // Append the generated code for global child window mappings.
        code += GenerateChildWindowMappingsCode();
        centralHeader << code;
        centralHeader.close();
        return code;
    }

    // -------------------------
    // Updated version of the GenerateCodeForSingleButton function
    // -------------------------
    inline std::string GenerateCodeForSingleButton(const DesignManager::ShapeItem& buttonShape)
    {
        std::string safeName = SanitizeVariableName(buttonShape.name);
        std::string onClickFunctionName = safeName + "_OnClick";

        std::string hCode;
        hCode += "// " + safeName + ".h\n\n";
        hCode += "#pragma once\n\n";
        hCode += "void " + onClickFunctionName + "();\n";
        hCode += "void generated" + safeName + "();\n";
        hCode += "void Init" + safeName + "LayersOnce();\n";
        hCode += "void Render" + safeName + "Window();\n";

        std::string cppCode;
        cppCode += "// " + safeName + ".cpp\n\n";
        cppCode += "#include \"" + safeName + ".h\"\n";
        cppCode += "#include \"design_manager.h\"\n";
        cppCode += "#include <iostream>\n\n";
        cppCode += "bool show" + safeName + "Window = false;\n\n";
        cppCode += "void " + onClickFunctionName + "() {\n";
        cppCode += "    show" + safeName + "Window = !show" + safeName + "Window;\n";
        cppCode += "    std::cout << \"[" + onClickFunctionName + "] show" + safeName + "Window = \" << show" + safeName + "Window << \"\\n\";\n";
        cppCode += "}\n\n";
        cppCode += "void generated" + safeName + "() {\n";
        cppCode += "    // Code generated for the " + safeName + " window\n";
        cppCode += "}\n\n";
        cppCode += "static bool g_" + safeName + "LayersAdded = false;\n\n";
        cppCode += "void Init" + safeName + "LayersOnce() {\n";
        cppCode += "    if (g_" + safeName + "LayersAdded)\n        return;\n\n";
        cppCode += "    g_" + safeName + "LayersAdded = true;\n\n";
        cppCode += "    if (g_windowsMap.find(\"" + safeName + " Window\") == g_windowsMap.end())\n";
        cppCode += "        g_windowsMap[\"" + safeName + " Window\"] = {};\n";
        cppCode += "    generated" + safeName + "();\n";
        cppCode += "}\n\n";
        cppCode += "void Render" + safeName + "Window() {\n";
        cppCode += "    if (!show" + safeName + "Window) return;\n";
        cppCode += "    ImGui::Begin(\"" + safeName + " Window\", &show" + safeName + "Window);\n";
        cppCode += "    Init" + safeName + "LayersOnce();\n";
        cppCode += "    // Render window content\n";
        cppCode += "    ImGui::End();\n";
        cppCode += "}\n";

        std::string fullCode =
            "/* --- Automatically generated code for " + buttonShape.name + " --- */\n\n" +
            "/* ========== " + safeName + ".h ========== */\n" + hCode + "\n\n" +
            "/* ========== " + safeName + ".cpp ========== */\n" + cppCode + "\n\n" +
            "/* --- You can copy these into two separate files. --- */\n";

        return fullCode;
    }










    //
    inline void RefreshLayerIDs()
    {
        auto& windowData = g_windowsMap[DesignManager::selectedGuiWindow];
        for (int i = 0; i < static_cast<int>(windowData.layers.size()); i++)
        {
            windowData.layers[i].id = i;
        }
    }


    inline void ShowUI_WindowSelectionAndGroupSettings(WindowData& windowData)
    {
        ImGui::TextUnformatted("Select Target ImGui Window:");
        if (ImGui::BeginCombo("##SelectedImGuiWindow", DesignManager::selectedGuiWindow.c_str()))
        {
            for (const auto& [winName, winData] : g_windowsMap)
            {
                bool is_sel = (DesignManager::selectedGuiWindow == winName);
                if (ImGui::Selectable(winName.c_str(), is_sel))
                {
                    DesignManager::selectedGuiWindow = winName;
                    MarkSceneUpdated();
                }
                if (is_sel)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        if (!g_windowsMap.empty())
        {
            ImGui::Separator();
            ImGui::TextUnformatted("Window Group Settings:");

            int groupId = windowData.groupId;
            if (ImGui::InputInt("Window Group ID", &groupId))
            {
                if (groupId < 0) groupId = 0;
                windowData.groupId = groupId;
                MarkSceneUpdated();
            }

            ImGui::TextWrapped("Windows with the same group ID (>0) will behave exclusively - only one window in the group can be open at a time.");

            if (ImGui::Button("Test Open Window"))
            {
                if (windowData.groupId > 0)
                {
                    for (auto& [winName, winData] : g_windowsMap)
                    {
                        if (winName == DesignManager::selectedGuiWindow)
                            continue;
                        if (winData.groupId == windowData.groupId && winData.isOpen)
                        {
                            winData.isOpen = false;
                        }
                    }
                }
                SetWindowOpen(DesignManager::selectedGuiWindow, true);
            }
            ImGui::SameLine();
            if (ImGui::Button("Test Close Window"))
            {
                SetWindowOpen(DesignManager::selectedGuiWindow, false);
            }
        }
    }

    inline void ShowUI_LayerManagement(WindowData& windowData, int& selectedLayerIndex, int& selectedShapeIndex)
    {
        if (ImGui::Button("Add New Layer"))
        {
            std::string newLayerName = "Layer " + std::to_string(windowData.layers.size() + 1);
            windowData.layers.emplace_back(newLayerName);
            windowData.layers.back().zOrder = static_cast<int>(windowData.layers.size()) - 1;
            MarkSceneUpdated();
        }
        ImGui::Separator();

        for (int i = 0; i < static_cast<int>(windowData.layers.size()); i++)
        {
            ImGui::PushID(i);
            Layer& layer = windowData.layers[i];

            if (ImGui::TreeNodeEx((layer.name + "##Layer_" + std::to_string(i)).c_str(),
                ImGuiTreeNodeFlags_DefaultOpen))
            {
                if (ImGui::Button(("Add Shape to " + layer.name).c_str()))
                {
                    ShapeItem newShape;
                    newShape.id = DesignManager::nextShapeID++;
                    newShape.name = "Shape " + std::to_string(newShape.id);
                    newShape.ownerWindow = DesignManager::selectedGuiWindow;
                    newShape.position = ImVec2(100.0f, 100.0f);
                    newShape.size = ImVec2(200.0f, 150.0f);
                    newShape.rotation = 0.0f;
                    newShape.basePosition = newShape.position;
                    newShape.baseSize = newShape.size;
                    newShape.baseRotation = newShape.rotation;

                    ButtonAnimation sizeXAnim;
                    sizeXAnim.name = "SizeX Animation";
                    sizeXAnim.duration = 1.0f;
                    sizeXAnim.speed = 1.0f;
                    sizeXAnim.animationTargetPosition = newShape.basePosition;
                    sizeXAnim.animationTargetSize = ImVec2(300.0f, newShape.baseSize.y);
                    sizeXAnim.transformRotation = newShape.baseRotation;
                    newShape.onClickAnimations.push_back(sizeXAnim);

                    ButtonAnimation sizeYAnim;
                    sizeYAnim.name = "SizeY Animation";
                    sizeYAnim.duration = 1.0f;
                    sizeYAnim.speed = 1.0f;
                    sizeYAnim.animationTargetPosition = newShape.basePosition;
                    sizeYAnim.animationTargetSize = ImVec2(newShape.baseSize.x, 300.0f);
                    sizeYAnim.transformRotation = newShape.baseRotation;
                    newShape.onClickAnimations.push_back(sizeYAnim);

                    layer.shapes.push_back(newShape);
                    selectedLayerIndex = i;
                    selectedShapeIndex = static_cast<int>(layer.shapes.size()) - 1;
                    MarkSceneUpdated();
                }

                ImGui::Separator();

                for (int j = 0; j < static_cast<int>(layer.shapes.size()); j++)
                {
                    ImGui::PushID(j);
                    ShapeItem& shape = layer.shapes[j];
                    bool isSelected = (selectedLayerIndex == i && selectedShapeIndex == j);

                    ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow;
                    if (isSelected)
                        nodeFlags |= ImGuiTreeNodeFlags_Selected;

                    if (ImGui::TreeNodeEx((shape.name + "##Shape_" + std::to_string(j)).c_str(),
                        nodeFlags))
                    {
                        if (ImGui::IsItemClicked())
                        {
                            selectedLayerIndex = i;
                            selectedShapeIndex = j;
                            MarkSceneUpdated();
                        }
                        ImGui::TreePop();
                    }
                    ImGui::PopID();
                }

                ImGui::TreePop();
            }
            ImGui::PopID();
        }
    }

    inline void ShowUI_ShapeDetails(WindowData& windowData, int selectedLayerIndex, int selectedShapeIndex)
    {
        if (selectedLayerIndex >= 0 && selectedShapeIndex >= 0 &&
            selectedLayerIndex < static_cast<int>(windowData.layers.size()))
        {
            Layer& selLayer = windowData.layers[selectedLayerIndex];
            if (selectedShapeIndex < static_cast<int>(selLayer.shapes.size()))
            {
                ShapeItem& s = selLayer.shapes[selectedShapeIndex];

                ImGui::Text("Selected Shape: %s", s.name.c_str());
                ImGui::Separator();

                if (ImGui::Button("Reset Shape"))
                {
                    s.position = s.basePosition;
                    s.size = s.baseSize;
                    s.rotation = s.baseRotation;
                    for (auto& anim : s.onClickAnimations)
                    {
                        anim.progress = 0.0f;
                        anim.isPlaying = false;
                    }
                    s.currentAnimation = nullptr;
                    MarkSceneUpdated();
                }
                ImGui::SameLine();
                if (ImGui::Button("Apply Offset to Base"))
                {
                    s.basePosition = s.position;
                    s.baseSize = s.size;
                    s.baseRotation = s.rotation;
                    for (auto& anim : s.onClickAnimations)
                    {
                        anim.progress = 0.0f;
                        anim.isPlaying = false;
                    }
                    s.currentAnimation = nullptr;
                    MarkSceneUpdated();
                }
                ImGui::Separator();

                if (ImGui::CollapsingHeader("Original (Base) Transform", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    if (ImGui::DragFloat2("Base Position", (float*)&s.basePosition, 1.0f))
                        MarkSceneUpdated();
                    if (ImGui::DragFloat2("Base Size", (float*)&s.baseSize, 1.0f))
                        MarkSceneUpdated();

                    float baseRotDeg = s.baseRotation * (180.0f / IM_PI);
                    if (ImGui::DragFloat("Base Rotation (deg)", &baseRotDeg, 1.0f, 0.0f, 360.0f))
                    {
                        s.baseRotation = baseRotDeg * (IM_PI / 180.0f);
                        MarkSceneUpdated();
                    }
                }

                if (ImGui::CollapsingHeader("Final Transform", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    ImVec2 posOffset = ImVec2(s.position.x - s.basePosition.x, s.position.y - s.basePosition.y);
                    ImVec2 sizeOffset = ImVec2(s.size.x - s.baseSize.x, s.size.y - s.baseSize.y);
                    float baseRotDeg = s.baseRotation * (180.0f / IM_PI);
                    float finalRotDeg = s.rotation * (180.0f / IM_PI);
                    float rotOffsetDeg = finalRotDeg - baseRotDeg;

                    ImGui::Text("Position (Final): (%.1f, %.1f) = Base (%.1f, %.1f) + Offset (%.1f, %.1f)", s.position.x, s.position.y, s.basePosition.x, s.basePosition.y, posOffset.x, posOffset.y);
                    ImGui::Text("Size (Final): (%.1f, %.1f) = Base (%.1f, %.1f) + Offset (%.1f, %.1f)", s.size.x, s.size.y, s.baseSize.x, s.baseSize.y, sizeOffset.x, sizeOffset.y);
                    ImGui::Text("Rotation (Final): %.1f° = Base (%.1f°) + Offset (%.1f°)", finalRotDeg, baseRotDeg, rotOffsetDeg);
                }

                ImGui::Separator();

                if (ImGui::CollapsingHeader("Shape Keys", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    for (int iKey = 0; iKey < (int)s.shapeKeys.size(); iKey++)
                    {
                        ImGui::PushID(iKey);
                        auto& sk = s.shapeKeys[iKey];
                        if (ImGui::TreeNode(("ShapeKey: " + sk.name + "##" + std::to_string(iKey)).c_str()))
                        {
                            char nameBuf[128];
                            strncpy(nameBuf, sk.name.c_str(), sizeof(nameBuf));
                            if (ImGui::InputText("Key Name", nameBuf, sizeof(nameBuf)))
                            {
                                sk.name = nameBuf;
                                MarkSceneUpdated();
                            }
                            const char* types[] = { "SizeX", "SizeY", "PositionX", "PositionY", "Rotation" };
                            int currentType = (int)sk.type;
                            if (ImGui::Combo("Type", &currentType, types, IM_ARRAYSIZE(types)))
                            {
                                sk.type = (ShapeKeyType)currentType;
                                MarkSceneUpdated();
                            }
                            if (ImGui::DragFloat2("Start Window Size", (float*)&sk.startWindowSize, 10.0f, 300.0f, 8000.0f, "%.0f"))
                            {
                                MarkSceneUpdated();
                            }
                            if (ImGui::DragFloat2("End Window Size", (float*)&sk.endWindowSize, 10.0f, 300.0f, 8000.0f, "%.0f"))
                            {
                                MarkSceneUpdated();
                            }
                            if (sk.type == ShapeKeyType::Rotation)
                            {
                                float targetRotDeg = sk.targetRotation;
                                if (ImGui::DragFloat("Target Rotation (deg)", &targetRotDeg, 1.0f, 0.0f, 360.0f))
                                {
                                    sk.targetRotation = targetRotDeg;
                                    MarkSceneUpdated();
                                }
                                float rotationOffsetDeg = sk.rotationOffset;
                                if (ImGui::DragFloat("Rotation Offset (deg)", &rotationOffsetDeg, 1.0f, -360.0f, 360.0f))
                                {
                                    sk.rotationOffset = rotationOffsetDeg;
                                    MarkSceneUpdated();
                                }
                            }
                            else
                            {
                                if (ImGui::DragFloat2("Target Value", (float*)&sk.targetValue, 1.0f, 0.0f, 10000.0f, "%.1f"))
                                {
                                    MarkSceneUpdated();
                                }
                                if (ImGui::DragFloat2("Offset", (float*)&sk.offset, 1.0f, -1000.0f, 1000.0f, "%.1f"))
                                {
                                    MarkSceneUpdated();
                                }
                            }
                            if (ImGui::Button("Remove This Key"))
                            {
                                s.shapeKeys.erase(s.shapeKeys.begin() + iKey);
                                ImGui::TreePop();
                                ImGui::PopID();
                                iKey--;
                                continue;
                            }
                            ImGui::Text("Shape Key Progress = %.1f%%", sk.value);
                            ImGui::ProgressBar(sk.value / 100.0f, ImVec2(-1, 0), (std::to_string((int)sk.value) + "%").c_str());
                            ImGui::TreePop();
                        }
                        ImGui::PopID();
                    }
                    if (ImGui::CollapsingHeader("Base Offsets", ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        if (ImGui::DragFloat2("Base Position Offset", (float*)&s.baseKeyOffset, 1.0f))
                            MarkSceneUpdated();
                        if (ImGui::DragFloat2("Base Size Offset", (float*)&s.baseKeySizeOffset, 1.0f))
                            MarkSceneUpdated();

                        float baseOffsetRotDeg = s.baseKeyRotationOffset * (180.0f / IM_PI);
                        if (ImGui::DragFloat("Base Rotation Offset (deg)", &baseOffsetRotDeg, 1.0f, -360.0f, 360.0f))
                        {
                            s.baseKeyRotationOffset = baseOffsetRotDeg * (IM_PI / 180.0f);
                            MarkSceneUpdated();
                        }
                        if (ImGui::Button("Reset Base Offsets"))
                        {
                            s.baseKeyOffset = ImVec2(0, 0);
                            s.baseKeySizeOffset = ImVec2(0, 0);
                            s.baseKeyRotationOffset = 0.0f;
                            MarkSceneUpdated();
                        }
                    }
                    if (ImGui::Button("Add New Shape Key"))
                    {
                        ShapeKey newKey;
                        newKey.name = "New Shape Key";
                        newKey.type = ShapeKeyType::SizeX;
                        newKey.startWindowSize = ImVec2(800.0f, 600.0f);
                        newKey.endWindowSize = ImVec2(1200.0f, 900.0f);
                        newKey.targetValue = ImVec2(100.0f, 100.0f);
                        newKey.targetRotation = 0.0f;
                        newKey.value = 0.0f;
                        s.shapeKeys.push_back(newKey);
                        MarkSceneUpdated();
                    }
                }

                ImGui::Checkbox("Update Animation Base On Resize", &s.updateAnimBaseOnResize);

                if (ImGui::CollapsingHeader("Button Animations", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    for (int aIdx = 0; aIdx < (int)s.onClickAnimations.size(); aIdx++)
                    {
                        ImGui::PushID(aIdx);
                        ButtonAnimation& anim = s.onClickAnimations[aIdx];
                        if (ImGui::TreeNode((anim.name + "##Anim_" + std::to_string(aIdx)).c_str()))
                        {
                            char animNameBuf[128];
                            strncpy(animNameBuf, anim.name.c_str(), sizeof(animNameBuf));
                            if (ImGui::InputText("Name", animNameBuf, sizeof(animNameBuf)))
                            {
                                anim.name = animNameBuf;
                                MarkSceneUpdated();
                            }
                            ImGui::DragFloat("Duration (s)", &anim.duration, 0.1f, 0.1f, 20.0f);
                            ImGui::DragFloat("Speed", &anim.speed, 0.1f, 0.1f, 10.0f);

                            ImGui::DragFloat2("Target Pos", (float*)&anim.animationTargetPosition, 1.0f);
                            ImGui::DragFloat2("Target Size", (float*)&anim.animationTargetSize, 1.0f);
                            float targetRotDeg = anim.transformRotation * (180.0f / IM_PI);
                            if (ImGui::DragFloat("Target Rotation (deg)", &targetRotDeg, 1.0f, 0.0f, 360.0f))
                            {
                                anim.transformRotation = targetRotDeg * DEG2RAD;
                                MarkSceneUpdated();
                            }

                            ImGui::InputInt("Repeat Count (0=inf)", &anim.repeatCount);

                            const char* poItems[] = { "Sıralı", "HemenArkasina" };
                            int poIdx = (int)anim.playbackOrder;
                            if (ImGui::Combo("Playback Order", &poIdx, poItems, IM_ARRAYSIZE(poItems)))
                            {
                                anim.playbackOrder = (PlaybackOrder)poIdx;
                                MarkSceneUpdated();
                            }
                            const char* interpItems[] = { "Linear", "EaseInOut" };
                            int interpIdx = (int)anim.interpolationMethod;
                            if (ImGui::Combo("Interpolation", &interpIdx, interpItems, IM_ARRAYSIZE(interpItems)))
                            {
                                anim.interpolationMethod = (ButtonAnimation::InterpolationMethod)interpIdx;
                                MarkSceneUpdated();
                            }
                            const char* triggerItems[] = { "OnClick", "OnHover" };
                            int trigIdx = (int)anim.triggerMode;
                            if (ImGui::Combo("Trigger Mode", &trigIdx, triggerItems, IM_ARRAYSIZE(triggerItems)))
                            {
                                anim.triggerMode = (ButtonAnimation::TriggerMode)trigIdx;
                                MarkSceneUpdated();
                            }
                            const char* behaviorItems[] = { "Play Once and Stay", "Play Once and Reverse", "Toggle", "Play While Holding and Reverse on Release", "Play While Holding and Stay" };
                            int behIdx = (int)anim.behavior;
                            if (ImGui::Combo("Behavior", &behIdx, behaviorItems, IM_ARRAYSIZE(behaviorItems)))
                            {
                                anim.behavior = (ButtonAnimation::AnimationBehavior)behIdx;
                                MarkSceneUpdated();
                            }

                            if (ImGui::Button("Remove This Animation"))
                            {
                                s.onClickAnimations.erase(s.onClickAnimations.begin() + aIdx);
                                ImGui::TreePop();
                                ImGui::PopID();
                                aIdx--;
                                continue;
                            }

                            ImGui::Text("Animation Progress: %.0f%%", anim.progress * 100.0f);
                            ImGui::ProgressBar(anim.progress, ImVec2(-1, 0));
                            ImGui::TreePop();
                        }
                        ImGui::PopID();
                    }
                    if (ImGui::Button("Add New Animation"))
                    {
                        ButtonAnimation newAnim;
                        newAnim.name = "New Animation";
                        newAnim.duration = 1.0f;
                        newAnim.speed = 1.0f;
                        newAnim.animationTargetPosition = ImVec2(150, 150);
                        newAnim.animationTargetSize = ImVec2(300, 200);
                        newAnim.transformRotation = 45.0f * DEG2RAD;
                        newAnim.interpolationMethod = ButtonAnimation::InterpolationMethod::Linear;
                        newAnim.behavior = ButtonAnimation::AnimationBehavior::PlayOnceAndStay;
                        newAnim.progress = 0.0f;
                        newAnim.isPlaying = false;

                        s.onClickAnimations.push_back(newAnim);
                        MarkSceneUpdated();
                    }
                }

                if (ImGui::CollapsingHeader("Event Handlers", ImGuiTreeNodeFlags_DefaultOpen)) {
                    // Display existing handlers
                    for (int i = 0; i < s.eventHandlers.size(); i++) {
                        ImGui::PushID(i);
                        auto& handler = s.eventHandlers[i];
                        ImGui::Text("%s: %s", handler.eventType.c_str(), handler.name.c_str());
                        if (ImGui::Button("Remove")) {
                            s.eventHandlers.erase(s.eventHandlers.begin() + i);
                            i--;
                        }
                        ImGui::PopID();
                    }
                    
                    // Add new handler
                    static char eventTypeBuffer[128] = "onClick";
                    static char nameBuffer[128] = "handleEvent";
                    ImGui::InputText("Event Type", eventTypeBuffer, sizeof(eventTypeBuffer));
                    ImGui::InputText("Handler Name", nameBuffer, sizeof(nameBuffer));
                    if (ImGui::Button("Add Handler")) {
                        s.eventHandlers.push_back({eventTypeBuffer, nameBuffer, [](ShapeItem&) {}});
                    }
                }
            }
        }
    }

    inline void ShowUI_ParentTreeWindow()
    {
        ImGui::SetNextWindowSize(ImVec2(400, 600), ImGuiCond_FirstUseEver);
        ImVec2 windowSize = ImGui::GetWindowSize();
        float buttonAreaHeight = ImGui::GetFrameHeight() * 5 + ImGui::GetStyle().ItemSpacing.y * 4;
        float scrollAreaHeight = windowSize.y - buttonAreaHeight - ImGui::GetStyle().WindowPadding.y;

        ImGui::Begin("Parent Tree");
        {
            ImVec2 windowSize = ImGui::GetWindowSize();
            float buttonAreaHeight = ImGui::GetFrameHeight() * 5 + ImGui::GetStyle().ItemSpacing.y * 4;
            float scrollAreaHeight = windowSize.y - buttonAreaHeight - ImGui::GetStyle().WindowPadding.y;

            ImGui::BeginChild("ScrollArea", ImVec2(0, scrollAreaHeight), true, ImGuiWindowFlags_HorizontalScrollbar);
            for (auto& [windowName, winData] : g_windowsMap)
            {
                if (ImGui::TreeNodeEx((windowName + "##Window").c_str(), ImGuiTreeNodeFlags_DefaultOpen))
                {
                    for (int i = 0; i < static_cast<int>(winData.layers.size()); i++)
                    {
                        Layer& layer = winData.layers[i];
                        if (ImGui::TreeNodeEx((layer.name + "##" + windowName + std::to_string(layer.id)).c_str(),
                            ImGuiTreeNodeFlags_DefaultOpen))
                        {
                            for (int j = 0; j < static_cast<int>(layer.shapes.size()); j++)
                            {
                                ShapeItem* shape = &layer.shapes[j];
                                if (shape->parent == nullptr)
                                    DrawShapeTreeNode(shape);
                            }
                            ImGui::TreePop();
                        }
                    }
                    ImGui::TreePop();
                }
            }
            ImGui::EndChild();

            ImGui::SetCursorPosY(ImGui::GetWindowHeight() - buttonAreaHeight + 80);
            ImGui::BeginChild("Buttons", ImVec2(0, buttonAreaHeight), true);
            {
                if (ImGui::Button("Set Parent") && DesignManager::selectedShapes.size() >= 2)
                {
                    ShapeItem* parent = DesignManager::selectedShapes.back();
                    for (ShapeItem* child : DesignManager::selectedShapes)
                    {
                        if (child != parent)
                            SetParent(child, parent);
                    }
                }
                if (ImGui::Button("Unparent") && !DesignManager::selectedShapes.empty())
                {
                    for (ShapeItem* child : DesignManager::selectedShapes)
                    {
                        if (child->parent != nullptr)
                            RemoveParent(child);
                    }
                }
                if (ImGui::Button("Unparent (Keep Transform)") && !DesignManager::selectedShapes.empty())
                {
                    for (ShapeItem* child : DesignManager::selectedShapes)
                    {
                        if (child->parent != nullptr)
                            RemoveParentKeepTransform(child);
                    }
                }
            }
            ImGui::EndChild();
        }
        ImGui::End();
    }

    inline void ShowUI_LayerShapeManager_ChildWindowMappings()
    {
        if (ImGui::CollapsingHeader("Child Window Mappings", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::SetCursorPosX(10);
            int mappingIndex = 0;
            for (auto& mapping : g_combinedChildWindowMappings)
            {
                ImGui::PushID(mappingIndex);
                std::vector<ShapeItem*> allShapes = GetAllShapes();
                int currentShapeIndex = 0;
                for (int i = 0; i < (int)allShapes.size(); i++) {
                    if (allShapes[i]->id == mapping.shapeId) {
                        currentShapeIndex = i;
                        break;
                    }
                }
                if (ImGui::Combo("Shape", &currentShapeIndex, [](void* data, int idx, const char** out_text) -> bool {
                    auto* vec = static_cast<std::vector<ShapeItem*>*>(data);
                    if (idx < (int)vec->size()) {
                        *out_text = (*vec)[idx]->name.c_str();
                        return true;
                    }
                    return false;
                    }, static_cast<void*>(&allShapes), (int)allShapes.size()))
                {
                    mapping.shapeId = allShapes[currentShapeIndex]->id;
                    allShapes[currentShapeIndex]->isChildWindow = true;
                }
                const char* opOptions[] = { "None", "AND", "OR", "XOR", "NAND", "IF_THEN", "IFF" };
                int opCount = IM_ARRAYSIZE(opOptions);
                int currentOpIndex = 0;
                for (int i = 0; i < opCount; i++) {
                    if (mapping.logicOp == opOptions[i]) {
                        currentOpIndex = i;
                        break;
                    }
                }
                if (ImGui::Combo("Operator", &currentOpIndex, opOptions, opCount))
                    mapping.logicOp = opOptions[currentOpIndex];
                int pairIndex = 0;
                for (size_t j = 0; j < mapping.buttonIds.size(); j++)
                {
                    ImGui::PushID(pairIndex);
                    std::vector<ShapeItem*> availableButtons = GetAllButtonShapes();
                    int currentButtonIndex = 0;
                    for (int i = 0; i < (int)availableButtons.size(); i++) {
                        if (availableButtons[i]->id == mapping.buttonIds[j]) {
                            currentButtonIndex = i;
                            break;
                        }
                    }
                    if (ImGui::Combo("Button", &currentButtonIndex, [](void* data, int idx, const char** out_text) -> bool {
                        auto* vec = static_cast<std::vector<ShapeItem*>*>(data);
                        if (idx < (int)vec->size()) {
                            *out_text = (*vec)[idx]->name.c_str();
                            return true;
                        }
                        return false;
                        }, static_cast<void*>(&availableButtons), (int)availableButtons.size()))
                    {
                        mapping.buttonIds[j] = availableButtons[currentButtonIndex]->id;
                    }
                    ImGui::SameLine();
                    std::vector<std::string> availableChildWindows;
                    for (auto& [key, winData] : g_windowsMap)
                        availableChildWindows.push_back(key);
                    for (auto sh : GetAllShapes()) {
                        if (std::find(availableChildWindows.begin(), availableChildWindows.end(), sh->name) == availableChildWindows.end())
                            availableChildWindows.push_back(sh->name);
                    }
                    int currentChildIndex = 0;
                    for (int i = 0; i < (int)availableChildWindows.size(); i++) {
                        if (availableChildWindows[i] == mapping.childWindowKeys[j]) {
                            currentChildIndex = i;
                            break;
                        }
                    }
                    if (ImGui::Combo("Window", &currentChildIndex, [](void* data, int idx, const char** out_text) -> bool {
                        auto* vec = static_cast<std::vector<std::string>*>(data);
                        if (idx < (int)vec->size()) {
                            *out_text = (*vec)[idx].c_str();
                            return true;
                        }
                        return false;
                        }, static_cast<void*>(&availableChildWindows), (int)availableChildWindows.size()))
                    {
                        mapping.childWindowKeys[j] = availableChildWindows[currentChildIndex];
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Remove Pair"))
                    {
                        mapping.buttonIds.erase(mapping.buttonIds.begin() + j);
                        mapping.childWindowKeys.erase(mapping.childWindowKeys.begin() + j);
                        ImGui::PopID();
                        break;
                    }
                    ImGui::PopID();
                    pairIndex++;
                }
                if (ImGui::Button("Add Button Mapping"))
                {
                    std::vector<ShapeItem*> availableButtons = GetAllButtonShapes();
                    int defaultButtonId = (!availableButtons.empty()) ? availableButtons[0]->id : 0;
                    std::vector<std::string> availableChildWindows;
                    for (auto& [key, winData] : g_windowsMap)
                        availableChildWindows.push_back(key);
                    for (auto sh : GetAllShapes()) {
                        if (std::find(availableChildWindows.begin(), availableChildWindows.end(), sh->name) == availableChildWindows.end())
                            availableChildWindows.push_back(sh->name);
                    }
                    std::string defaultChild = (!availableChildWindows.empty()) ? availableChildWindows[0] : "";
                    mapping.buttonIds.push_back(defaultButtonId);
                    mapping.childWindowKeys.push_back(defaultChild);
                }
                if (ImGui::Button("Remove Mapping"))
                {
                    g_combinedChildWindowMappings.erase(g_combinedChildWindowMappings.begin() + mappingIndex);
                    ImGui::PopID();
                    break;
                }
                ImGui::Separator();
                ImGui::PopID();
                mappingIndex++;
            }
            if (ImGui::Button("Add New Mapping"))
            {
                std::vector<ShapeItem*> allShapes = GetAllShapes();
                int defaultShapeId = (!allShapes.empty()) ? allShapes[0]->id : 0;
                std::vector<ShapeItem*> availableButtons = GetAllButtonShapes();
                int defaultButtonId = (!availableButtons.empty()) ? availableButtons[0]->id : 0;
                std::vector<std::string> availableChildWindows;
                for (auto& [key, winData] : g_windowsMap)
                    availableChildWindows.push_back(key);
                for (auto sh : GetAllShapes()) {
                    if (std::find(availableChildWindows.begin(), availableChildWindows.end(), sh->name) == availableChildWindows.end())
                        availableChildWindows.push_back(sh->name);
                }
                std::string defaultChild = (!availableChildWindows.empty()) ? availableChildWindows[0] : "";
                CombinedMapping newMapping;
                newMapping.shapeId = defaultShapeId;
                newMapping.logicOp = "AND";
                newMapping.buttonIds.push_back(defaultButtonId);
                newMapping.childWindowKeys.push_back(defaultChild);
                if (!allShapes.empty()) allShapes[0]->isChildWindow = true;
                g_combinedChildWindowMappings.push_back(newMapping);
            }
        }
    }

    inline void ShowUI_LayerShapeManager_LayerList(WindowData& windowData, int& selectedLayerIndex, int& selectedShapeIndex)
    {
        ImGui::Separator();
        ImGui::Separator();

        if (ImGui::BeginCombo("Selected ImGui Window", DesignManager::selectedGuiWindow.c_str()))
        {
            for (auto const& [windowName, winData] : g_windowsMap)
            {
                bool is_selected = (DesignManager::selectedGuiWindow == windowName);
                if (ImGui::Selectable(windowName.c_str(), is_selected))
                {
                    DesignManager::selectedGuiWindow = windowName;
                    MarkSceneUpdated();
                }
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        if (ImGui::Button("Add New Layer"))
        {
            std::string newLayerName = "Layer " + std::to_string(windowData.layers.size());
            windowData.layers.emplace_back(newLayerName);
            RefreshLayerIDs();
            windowData.layers.back().zOrder = (int)windowData.layers.size() - 1;
            selectedLayerIndex = (int)windowData.layers.size() - 1;
            selectedShapeIndex = -1;
            MarkSceneUpdated();
        }
        ImGui::Separator();

        for (int i = 0; i < (int)windowData.layers.size(); i++)
        {
            ImGui::PushID(i);
            if (ImGui::Button(("Up##" + std::to_string(i)).c_str()))
            {
                if (i > 0)
                {
                    std::swap(windowData.layers[i], windowData.layers[i - 1]);
                    std::swap(windowData.layers[i].zOrder, windowData.layers[i - 1].zOrder);
                    MarkSceneUpdated();
                }
            }
            ImGui::SameLine();
            if (ImGui::Button(("Down##" + std::to_string(i)).c_str()))
            {
                if (i < (int)windowData.layers.size() - 1)
                {
                    std::swap(windowData.layers[i], windowData.layers[i + 1]);
                    std::swap(windowData.layers[i].zOrder, windowData.layers[i + 1].zOrder);
                    MarkSceneUpdated();
                }
            }
            ImGui::SameLine();
            if (ImGui::Button(("Delete##" + std::to_string(i)).c_str()))
            {
                windowData.layers.erase(windowData.layers.begin() + i);
                RefreshLayerIDs();
                if (windowData.layers.empty())
                    selectedLayerIndex = -1;
                else if (selectedLayerIndex >= (int)windowData.layers.size())
                    selectedLayerIndex = (int)windowData.layers.size() - 1;
                MarkSceneUpdated();
                ImGui::PopID();
                i--;
                continue;
            }
            ImGui::SameLine();
            if (ImGui::IsItemClicked())
            {
                selectedLayerIndex = i;
                selectedShapeIndex = -1;
                DesignManager::selectedShapes.clear();
            }
            if (ImGui::Button(("Rename Layer##" + std::to_string(i)).c_str()))
            {
                static char renameLayerBuffer[128];
                strncpy(renameLayerBuffer, windowData.layers[i].name.c_str(), 128);
                renameLayerBuffer[127] = '\0';
                ImGui::OpenPopup(("RenameLayerPopup##" + DesignManager::selectedGuiWindow + std::to_string(i)).c_str());
            }
            if (ImGui::BeginPopupModal(("RenameLayerPopup##" + DesignManager::selectedGuiWindow + std::to_string(i)).c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                static char renameLayerBuffer2[128];
                ImGui::Text("Enter new layer name:");
                ImGui::InputText("##NewLayerName", renameLayerBuffer2, IM_ARRAYSIZE(renameLayerBuffer2));
                if (ImGui::Button("OK"))
                {
                    windowData.layers[i].name = renameLayerBuffer2;
                    ImGui::CloseCurrentPopup();
                    MarkSceneUpdated();
                }
                ImGui::SameLine();
                if (ImGui::Button("Cancel"))
                    ImGui::CloseCurrentPopup();
                ImGui::EndPopup();
            }
            if (ImGui::Checkbox(("Visible##" + std::to_string(i)).c_str(), &windowData.layers[i].visible))
            {
                MarkSceneUpdated();
            }
            ImGui::SameLine();
            ImGui::Checkbox(("Locked##" + std::to_string(i)).c_str(), &windowData.layers[i].locked);
            ImGui::PushItemWidth(100);
            ImGui::SameLine();
            if (ImGui::DragInt(("Layer Z-Order##" + std::to_string(i)).c_str(), &windowData.layers[i].zOrder, 0.1f))
            {
                for (int j = 0; j < (int)windowData.layers.size(); j++)
                {
                    if (j != i && windowData.layers[j].zOrder == windowData.layers[i].zOrder)
                        windowData.layers[j].zOrder += (windowData.layers[i].zOrder > windowData.layers[j].zOrder ? 1 : -1);
                }
                MarkSceneUpdated();
            }
            ImGui::PopItemWidth();

            if (ImGui::TreeNodeEx((windowData.layers[i].name + "##" + DesignManager::selectedGuiWindow + std::to_string(windowData.layers[i].id)).c_str(),
                (selectedLayerIndex == i ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow))
            {
                Layer& L = windowData.layers[i];
                for (int j = 0; j < (int)L.shapes.size(); j++)
                {
                    ImGui::PushID(j + 1000);
                    bool isSelected = (std::find(DesignManager::selectedShapes.begin(), DesignManager::selectedShapes.end(), &L.shapes[j]) != DesignManager::selectedShapes.end());
                    if (ImGui::Selectable((L.shapes[j].name + "##" + std::to_string(j)).c_str(), isSelected))
                    {
                        ImGuiIO& io = ImGui::GetIO();
                        if (!io.KeyCtrl && !io.KeyShift)
                        {
                            DesignManager::selectedShapes.clear();
                            DesignManager::selectedShapes.push_back(&L.shapes[j]);
                            lastSelectedLayerIndex = i;
                            lastSelectedShapeIndex = j;
                        }
                        else if (io.KeyCtrl)
                        {
                            auto it = std::find(DesignManager::selectedShapes.begin(), DesignManager::selectedShapes.end(), &L.shapes[j]);
                            if (it != DesignManager::selectedShapes.end())
                                DesignManager::selectedShapes.erase(it);
                            else
                                DesignManager::selectedShapes.push_back(&L.shapes[j]);
                            lastSelectedLayerIndex = i;
                            lastSelectedShapeIndex = j;
                        }
                        else if (io.KeyShift)
                        {
                            if (lastSelectedLayerIndex == i && lastSelectedShapeIndex != -1)
                            {
                                int start = std::min(lastSelectedShapeIndex, j);
                                int end = std::max(lastSelectedShapeIndex, j);
                                DesignManager::selectedShapes.clear();
                                for (int k = start; k <= end; k++)
                                    DesignManager::selectedShapes.push_back(&L.shapes[k]);
                            }
                            else
                            {
                                DesignManager::selectedShapes.push_back(&L.shapes[j]);
                                lastSelectedLayerIndex = i;
                                lastSelectedShapeIndex = j;
                            }
                        }
                        selectedLayerIndex = i;
                        selectedShapeIndex = j;
                    }
                    ImGui::PopID();
                }
                if (ImGui::Button(("Add New Shape##" + std::to_string(i)).c_str()))
                {
                    static int newShapeCount = 0;
                    std::string newShapeName;
                    while (true)
                    {
                        newShapeName = "Shape " + std::to_string(newShapeCount);
                        bool alreadyExists = false;
                        for (auto& lay : windowData.layers)
                        {
                            for (auto& shp : lay.shapes)
                            {
                                if (shp.name == newShapeName)
                                {
                                    alreadyExists = true;
                                    break;
                                }
                            }
                            if (alreadyExists) break;
                        }
                        if (!alreadyExists) break;
                        newShapeCount++;
                    }
                    newShapeCount++;

                    ShapeItem s;
                    s.name = newShapeName;
                    s.zOrder = 0;
                    s.ownerWindow = DesignManager::selectedGuiWindow;
                    s.id = DesignManager::nextShapeID++;
                    s.sizeThresholds.emplace_back(ImVec2(800.0f, 600.0f), ShapeItem::Transformation{ ImVec2(100.0f, 100.0f), ImVec2(200.0f, 150.0f), 45.0f, 1.0f });
                    L.shapes.push_back(s);
                    selectedShapeIndex = (int)L.shapes.size() - 1;
                    selectedLayerIndex = i;
                    MarkSceneUpdated();
                }
                ImGui::TreePop();
            }
            ImGui::PopID();
        }
    }
    inline void ShowUI_EventHandlers(ShapeItem& shape) {
        if (ImGui::CollapsingHeader("Event Handlers", ImGuiTreeNodeFlags_DefaultOpen)) {
            // Display existing handlers
            for (int i = 0; i < shape.eventHandlers.size(); i++) {
                ImGui::PushID(i);
                auto& handler = shape.eventHandlers[i];
                ImGui::Text("%s: %s", handler.eventType.c_str(), handler.name.c_str());
                if (ImGui::Button("Remove")) {
                    shape.eventHandlers.erase(shape.eventHandlers.begin() + i);
                    i--;
                }
                ImGui::PopID();
            }
            
            // Add new handler
            static char eventTypeBuffer[128] = "onClick";
            static char nameBuffer[128] = "handleEvent";
            ImGui::InputText("Event Type", eventTypeBuffer, sizeof(eventTypeBuffer));
            ImGui::InputText("Handler Name", nameBuffer, sizeof(nameBuffer));
            if (ImGui::Button("Add Handler")) {
                shape.eventHandlers.push_back({eventTypeBuffer, nameBuffer, [](ShapeItem&) {}});
            }
        }
    }
    inline void ShowUI_LayerShapeManager_ShapeProperties(WindowData& windowData, int& selectedLayerIndex, int& selectedShapeIndex)
    {
        if (DesignManager::selectedShapes.size() == 1 && selectedLayerIndex >= 0 && selectedShapeIndex >= 0 && selectedShapeIndex < (int)windowData.layers[selectedLayerIndex].shapes.size())
        {
            ImGui::Separator();
            ShapeItem& s = windowData.layers[selectedLayerIndex].shapes[selectedShapeIndex];
            if (ImGui::Button("Delete Shape"))
            {
                windowData.layers[selectedLayerIndex].shapes.erase(windowData.layers[selectedLayerIndex].shapes.begin() + selectedShapeIndex);
                selectedShapeIndex = -1;
                DesignManager::selectedShapes.clear();
                MarkSceneUpdated();
            }
            ImGui::SameLine();
            if (ImGui::Button("Duplicate Shape"))
            {
                ShapeItem duplicatedShape = s;
                duplicatedShape.id = DesignManager::nextShapeID++;
                duplicatedShape.name += " Copy";
                duplicatedShape.ownerWindow = s.ownerWindow;
                windowData.layers[selectedLayerIndex].shapes.push_back(duplicatedShape);
                selectedShapeIndex = (int)windowData.layers[selectedLayerIndex].shapes.size() - 1;
                DesignManager::selectedShapes.clear();
                DesignManager::selectedShapes.push_back(&windowData.layers[selectedLayerIndex].shapes.back());
                MarkSceneUpdated();
            }
            ImGui::SameLine();
            static char renameBuffer[128];
            if (ImGui::Button("Rename Shape"))
            {
                strncpy(renameBuffer, s.name.c_str(), 128);
                renameBuffer[127] = '\0';
                ImGui::OpenPopup("RenameShapePopup");
            }
            if (ImGui::BeginPopupModal("RenameShapePopup", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::InputText("New Name", renameBuffer, IM_ARRAYSIZE(renameBuffer));
                if (ImGui::Button("OK"))
                {
                    s.name = renameBuffer;
                    ImGui::CloseCurrentPopup();
                    MarkSceneUpdated();
                }
                ImGui::SameLine();
                if (ImGui::Button("Cancel"))
                    ImGui::CloseCurrentPopup();
                ImGui::EndPopup();
            }
            if (ImGui::Checkbox("Glass Effect", &s.useGlass))
                MarkSceneUpdated();
            if (s.useGlass)
            {
                ImGui::SliderFloat("Glass Blur", &s.glassBlur, 1.0f, 100.0f);
                ImGui::SliderFloat("Glass Alpha", &s.glassAlpha, 0.0f, 1.0f);
                ImGui::ColorEdit4("Glass Tint Color", (float*)&s.glassColor);
                MarkSceneUpdated();
            }
            if (ImGui::TreeNode("Move to Layer"))
            {
                for (int li = 0; li < (int)windowData.layers.size(); li++)
                {
                    if (li == selectedLayerIndex)
                        continue;
                    if (ImGui::Button(windowData.layers[li].name.c_str()))
                    {
                        windowData.layers[li].shapes.push_back(s);
                        windowData.layers[selectedLayerIndex].shapes.erase(windowData.layers[selectedLayerIndex].shapes.begin() + selectedShapeIndex);
                        selectedShapeIndex = -1;
                        DesignManager::selectedShapes.clear();
                        MarkSceneUpdated();
                        break;
                    }
                }
                ImGui::TreePop();
            }
            ImGui::Separator();
            if (ImGui::CollapsingHeader("Child Window Settings", ImGuiTreeNodeFlags_DefaultOpen))
            {
                if (ImGui::Checkbox("Is Child Window", &s.isChildWindow))
                {
                    if (!s.isChildWindow)
                    {
                        s.childWindowSync = false;
                        s.toggleChildWindow = false;
                        s.childWindowGroupId = 0;
                    }
                    MarkSceneUpdated();
                }
                if (s.isChildWindow)
                {
                    if (ImGui::Checkbox("Sync with Shape", &s.childWindowSync))
                        MarkSceneUpdated();
                    if (ImGui::Checkbox("Toggle Child Window On Click", &s.toggleChildWindow))
                        MarkSceneUpdated();
                    if (ImGui::CollapsingHeader("Assigned Child Windows", ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        int index = 0;
                        for (auto& childKey : s.assignedChildWindows)
                        {
                            ImGui::Text("Child Window %d: %s", index, childKey.c_str());
                            ImGui::SameLine();
                            std::string removeLabel = "Remove##" + std::to_string(index);
                            if (ImGui::Button(removeLabel.c_str()))
                            {
                                s.assignedChildWindows.erase(s.assignedChildWindows.begin() + index);
                                MarkSceneUpdated();
                                break;
                            }
                            index++;
                        }
                        std::vector<std::string> availableChildWindows;
                        for (auto& [key, windowData] : g_windowsMap)
                            availableChildWindows.push_back(key);
                        if (!availableChildWindows.empty())
                        {
                            static int selectedChildIndex = 0;
                            if (ImGui::Combo("Add Child Window", &selectedChildIndex, [](void* data, int idx, const char** out_text) -> bool {
                                auto& vec = *static_cast<std::vector<std::string>*>(data);
                                if (idx >= 0 && idx < vec.size()) {
                                    *out_text = vec[idx].c_str();
                                    return true;
                                }
                                return false;
                                }, static_cast<void*>(&availableChildWindows), availableChildWindows.size()))
                            {
                                if (std::find(s.assignedChildWindows.begin(), s.assignedChildWindows.end(), availableChildWindows[selectedChildIndex]) == s.assignedChildWindows.end())
                                {
                                    s.assignedChildWindows.push_back(availableChildWindows[selectedChildIndex]);
                                    MarkSceneUpdated();
                                }
                            }
                        }
                        if (!s.assignedChildWindows.empty())
                        {
                            if (ImGui::Combo("Active Child Window", &s.selectedChildWindowIndex, [](void* data, int idx, const char** out_text) -> bool {
                                auto& vec = *static_cast<std::vector<std::string>*>(data);
                                if (idx >= 0 && idx < vec.size()) {
                                    *out_text = vec[idx].c_str();
                                    return true;
                                }
                                return false;
                                }, static_cast<void*>(&s.assignedChildWindows), s.assignedChildWindows.size()))
                            {
                                MarkSceneUpdated();
                            }
                        }
                    }
                    if (s.toggleChildWindow)
                    {
                        const char* toggle_behaviors[] = { "Window Only", "Shape and Window" };
                        int current_behavior = static_cast<int>(s.toggleBehavior);
                        if (ImGui::Combo("Toggle Behavior", &current_behavior, toggle_behaviors, IM_ARRAYSIZE(toggle_behaviors)))
                        {
                            s.toggleBehavior = static_cast<ChildWindowToggleBehavior>(current_behavior);
                            MarkSceneUpdated();
                        }
                    }
                    int groupId = s.childWindowGroupId;
                    if (ImGui::InputInt("Child Window Group ID", &groupId))
                    {
                        s.childWindowGroupId = groupId;
                        MarkSceneUpdated();
                    }
                    if (s.childWindowSync)
                    {
                        std::vector<ShapeItem*> buttonShapes = GetAllButtonShapes();
                        if (!buttonShapes.empty())
                        {
                            std::vector<std::string> buttonNames;
                            int currentButtonIndex = -1;
                            for (size_t i = 0; i < buttonShapes.size(); i++)
                            {
                                buttonNames.push_back(buttonShapes[i]->name + " (ID: " + std::to_string(buttonShapes[i]->id) + ")");
                                if (buttonShapes[i]->id == s.targetShapeID)
                                    currentButtonIndex = static_cast<int>(i);
                            }
                            buttonNames.insert(buttonNames.begin(), "None");
                            if (s.targetShapeID == 0)
                                currentButtonIndex = 0;
                            else
                                currentButtonIndex++;
                            if (ImGui::Combo("Toggle Button", &currentButtonIndex, [](void* data, int idx, const char** out_text) -> bool {
                                auto& names = *static_cast<std::vector<std::string>*>(data);
                                if (idx >= 0 && idx < static_cast<int>(names.size()))
                                {
                                    *out_text = names[idx].c_str();
                                    return true;
                                }
                                return false;
                                }, &buttonNames, static_cast<int>(buttonNames.size())))
                            {
                                if (currentButtonIndex == 0)
                                    s.targetShapeID = 0;
                                else
                                    s.targetShapeID = buttonShapes[currentButtonIndex - 1]->id;
                                MarkSceneUpdated();
                            }
                        }
                        else
                        {
                            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), "No buttons available");
                        }
                    }
                    if (ImGui::Button("Toggle Child Window"))
                    {
                        std::string targetWindowKey;
                        if (!s.assignedChildWindows.empty())
                        {
                            int idx = s.selectedChildWindowIndex;
                            if (idx < 0 || idx >= static_cast<int>(s.assignedChildWindows.size()))
                                idx = 0;
                            targetWindowKey = s.assignedChildWindows[idx];
                        }
                        else
                        {
                            std::vector<std::string> globalChildWindows;
                            for (auto& [key, windowData] : g_windowsMap)
                            {
                                if (windowData.isChildWindow || key.find("ChildWindow_") != std::string::npos)
                                    globalChildWindows.push_back(key);
                            }
                            if (!globalChildWindows.empty())
                            {
                                targetWindowKey = globalChildWindows[0];
                                s.assignedChildWindows.push_back(targetWindowKey);
                                s.selectedChildWindowIndex = 0;
                                MarkSceneUpdated();
                            }
                        }
                        bool newState = !IsWindowOpen(targetWindowKey);
                        if (newState && DesignManager::exclusiveChildWindowMode && s.childWindowGroupId > 0)
                        {
                            for (const auto& otherKey : s.assignedChildWindows)
                            {
                                if (otherKey != targetWindowKey)
                                    SetWindowOpen(otherKey, false);
                            }
                        }
                        SetWindowOpen(targetWindowKey, newState);
                        MarkSceneUpdated();
                    }
                }
                if (ImGui::Checkbox("Exclusive Child Window Mode", &DesignManager::exclusiveChildWindowMode))
                    MarkSceneUpdated();
            }
            ImGui::Separator();
            if (ImGui::InputInt("Group Id", &s.groupId))
                MarkSceneUpdated();
            ImGui::Separator();
            if (ImGui::Checkbox("Has Embedded Image", &s.hasEmbeddedImage))
                MarkSceneUpdated();
            if (s.hasEmbeddedImage)
            {
                if (ImGui::Combo("Embedded Image", &s.embeddedImageIndex, g_embeddedImageFunctions, g_embeddedImageFunctionsCount))
                {
                    MarkSceneUpdated();
                }
                if (s.embeddedImageIndex >= 0)
                {
                    ImGui::TextWrapped("Embedded Image from function: %s", g_embeddedImageFunctions[s.embeddedImageIndex]);
                }
                else
                {
                    ImGui::TextWrapped("No global embedded image selected. (Inline bin2c output...)");
                }
            }
            if (ImGui::Checkbox("Is Button", &s.isButton))
                MarkSceneUpdated();
            ImGui::Checkbox("Allow Item Overlap", &s.allowItemOverlap);
            ImGui::BeginDisabled(!s.allowItemOverlap);
            ImGui::Checkbox("Force Overlay (Foreground)", &s.forceOverlap);
            if (!s.allowItemOverlap && ImGui::IsItemHovered())
                ImGui::SetTooltip("Enable 'Allow Item Overlap' to activate Force Overlay");
            ImGui::EndDisabled();
            ImGui::Checkbox("Block Underlying Items", &s.blockUnderlying);
            if (s.isButton)
            {
                if (ImGui::ColorEdit4("Hover Color", (float*)&s.hoverColor))
                    MarkSceneUpdated();
                if (ImGui::ColorEdit4("Click Color", (float*)&s.clickedColor))
                    MarkSceneUpdated();
                const char* buttonBehaviors[] = { "SingleClick", "Toggle", "Hold" };
                int currentBehavior = static_cast<int>(s.buttonBehavior);
                if (ImGui::Combo("Button Behavior", &currentBehavior, buttonBehaviors, IM_ARRAYSIZE(buttonBehaviors)))
                {
                    s.buttonBehavior = (ShapeItem::ButtonBehavior)currentBehavior;
                    MarkSceneUpdated();
                }
                bool oldUseOnClick = s.useOnClick;
                if (ImGui::Checkbox("Use OnClick", &s.useOnClick))
                {
                    if (!oldUseOnClick && s.useOnClick)
                    {
                        if (!s.storedOnClick)
                            s.onClick = [&]() {};
                        else
                            s.onClick = s.storedOnClick;
                    }
                    else if (oldUseOnClick && !s.useOnClick)
                    {
                        s.storedOnClick = s.onClick;
                        s.onClick = nullptr;
                    }
                    MarkSceneUpdated();
                }
                if (ImGui::Button("Generate Code For This Button"))
                {
                    DesignManager::generatedCodeForButton = GenerateCodeForSingleButton(s);
                }
                if (!DesignManager::generatedCodeForButton.empty())
                {
                    ImGui::Separator();
                    ImGui::TextUnformatted("Auto-Generated Button Code (.h + .cpp):");
                    ImGui::BeginChild("ButtonCodeChild", ImVec2(-FLT_MIN, 120.0f), true, ImGuiWindowFlags_HorizontalScrollbar);
                    ImGui::TextUnformatted(DesignManager::generatedCodeForButton.c_str());
                    ImGui::EndChild();
                    if (ImGui::Button("Copy Button Code"))
                    {
                        ImGui::SetClipboardText(DesignManager::generatedCodeForButton.c_str());
                    }
                }
            }
            if (ImGui::CollapsingHeader("Original (Base) Transform", ImGuiTreeNodeFlags_DefaultOpen))
            {
                if (ImGui::DragFloat2("Base Position", (float*)&s.basePosition, 1.0f))
                    MarkSceneUpdated();
                if (ImGui::DragFloat2("Base Size", (float*)&s.baseSize, 1.0f))
                    MarkSceneUpdated();
                float baseRotDeg = s.baseRotation * (180.0f / IM_PI);
                if (ImGui::DragFloat("Base Rotation (deg)", &baseRotDeg, 1.0f, 0.0f, 360.0f))
                {
                    s.baseRotation = baseRotDeg * (IM_PI / 180.0f);
                    MarkSceneUpdated();
                }
            }
            if (ImGui::Checkbox("Has Text", &s.hasText))
                MarkSceneUpdated();
            if (s.hasText)
            {
                static char textBuffer[1024];
                strncpy(textBuffer, s.text.c_str(), sizeof(textBuffer) - 1);
                textBuffer[sizeof(textBuffer) - 1] = '\0';
                if (ImGui::InputTextMultiline("Text", textBuffer, sizeof(textBuffer), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 3)))
                {
                    s.text = textBuffer;
                    MarkSceneUpdated();
                }
                if (ImGui::ColorEdit4("Text Color", (float*)&s.textColor))
                    MarkSceneUpdated();
                if (ImGui::DragFloat("Text Size", &s.textSize, 0.1f, 6.0f, 64.0f))
                    MarkSceneUpdated();
                if (ImGui::DragFloat2("Text Position", (float*)&s.textPosition, 1.0f))
                    MarkSceneUpdated();
                if (ImGui::DragFloat("Text Rotation (deg)", &s.textRotation, 1.0f, 0.0f, 360.0f))
                {
                    MarkSceneUpdated();
                }
                static const char* items[] = { "Default", "SmallFont", "LargeFont", "SmallFont2", "TinyFont", "PaneuropaFont", "PaneuropaFont_tiny" };
                int font_idx = s.textFont;
                if (ImGui::Combo("Font", &font_idx, items, IM_ARRAYSIZE(items)))
                {
                    s.textFont = font_idx;
                    MarkSceneUpdated();
                }
                if (ImGui::Checkbox("Dynamic Text Size", &s.dynamicTextSize))
                    MarkSceneUpdated();
                const char* alignItems[] = { "Left", "Center", "Right" };
                int textAlign = s.textAlignment;
                if (ImGui::Combo("Text Alignment", &textAlign, alignItems, IM_ARRAYSIZE(alignItems)))
                {
                    s.textAlignment = textAlign;
                    MarkSceneUpdated();
                }
            }
            if (ImGui::DragFloat("Corner Radius", &s.cornerRadius, 0.5f, 0.0f, 200.0f))
                MarkSceneUpdated();
            if (ImGui::DragFloat("Border Thickness", &s.borderThickness, 0.1f, 0.0f, 20.0f))
                MarkSceneUpdated();
            if (ImGui::ColorEdit4("Fill Color", (float*)&s.fillColor))
                MarkSceneUpdated();
            if (ImGui::ColorEdit4("Border Color", (float*)&s.borderColor))
                MarkSceneUpdated();
            if (ImGui::ColorEdit4("Shadow Color", (float*)&s.shadowColor))
                MarkSceneUpdated();
            if (ImGui::DragFloat4("Shadow Spread", (float*)&s.shadowSpread, 0.1f, 0.0f, 100.0f))
                MarkSceneUpdated();
            float so[2] = { s.shadowOffset.x, s.shadowOffset.y };
            if (ImGui::DragFloat2("Shadow Offset", so, 0.5f, -100, 100))
            {
                s.shadowOffset.x = so[0];
                s.shadowOffset.y = so[1];
                MarkSceneUpdated();
            }
            if (ImGui::Checkbox("Shadow Use Corner Radius", &s.shadowUseCornerRadius))
                MarkSceneUpdated();
            int st = (int)s.type;
            if (ImGui::Combo("Type", &st, "Rectangle\0Circle\0"))
            {
                s.type = (ShapeType)st;
                MarkSceneUpdated();
            }
            float shadow_rd = s.shadowRotation * (180.0f / 3.1415926535f);
            if (ImGui::DragFloat("Shadow Rotation (deg)", &shadow_rd, 1.0f, 0.0f, 360.0f))
            {
                s.shadowRotation = shadow_rd * (3.1415926535f / 180.0f);
                MarkSceneUpdated();
            }
            if (ImGui::DragFloat("Blur Amount", &s.blurAmount, 0.1f, 0.0f, 20.0f))
                MarkSceneUpdated();
            if (ImGui::Checkbox("Visible", &s.visible))
                MarkSceneUpdated();
            ImGui::Checkbox("Locked", &s.locked);
            if (ImGui::DragInt("Shape Z-Order", &s.zOrder, 0.1f))
                MarkSceneUpdated();
            if (ImGui::Checkbox("Use Gradient", &s.useGradient))
                MarkSceneUpdated();
            if (s.useGradient)
            {
                float gradr = s.gradientRotation;
                if (ImGui::DragFloat("Gradient Rotation (deg)", &gradr, 1.0f, 0.0f, 360.0f))
                {
                    s.gradientRotation = gradr;
                    MarkSceneUpdated();
                }
                int interpolation_type = (int)s.gradientInterpolation;
                const char* interpItems[] = { "Linear", "Ease", "Constant", "Cardinal", "BSpline" };
                if (ImGui::Combo("Interpolation", &interpolation_type, interpItems, IM_ARRAYSIZE(interpItems)))
                {
                    s.gradientInterpolation = (ShapeItem::GradientInterpolation)interpolation_type;
                    MarkSceneUpdated();
                }
                ImGui::Text("Color Ramp");
                for (int ci = 0; ci < (int)s.colorRamp.size(); ci++)
                {
                    ImGui::PushID(ci + 3000);
                    if (ImGui::SliderFloat(("Pos##" + std::to_string(ci)).c_str(), &s.colorRamp[ci].first, 0.0f, 1.0f, "%.3f"))
                    {
                        MarkSceneUpdated();
                    }
                    if (ImGui::ColorEdit4(("Color##" + std::to_string(ci)).c_str(), (float*)&s.colorRamp[ci].second))
                    {
                        MarkSceneUpdated();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button(("Delete##" + std::to_string(ci)).c_str()) && s.colorRamp.size() > 1)
                    {
                        s.colorRamp.erase(s.colorRamp.begin() + ci);
                        ci--;
                        MarkSceneUpdated();
                    }
                    ImGui::PopID();
                }
                if (ImGui::Button("Add Color"))
                {
                    s.colorRamp.emplace_back(0.5f, ImVec4(1, 1, 1, 1));
                    MarkSceneUpdated();
                }
            }
            ShowUI_EventHandlers(s);
        }
    }

    inline void ShowUI_LayerShapeManager_MultiShapeEdit()
    {
        if (DesignManager::selectedShapes.size() > 1)
        {
            ImGui::Separator();
            ImGui::Text("Multiple shapes selected.");

            static bool rotOffsetMode = false;
            static bool colOffsetMode = false;

            static float deltaPos[2] = { 0.0f, 0.0f };
            if (ImGui::DragFloat2("Delta Position", deltaPos, 1.0f))
            {
                for (ShapeItem* shape : DesignManager::selectedShapes)
                {
                    shape->basePosition.x += deltaPos[0];
                    shape->basePosition.y += deltaPos[1];
                }
                deltaPos[0] = deltaPos[1] = 0.0f;
                MarkSceneUpdated();
            }

            ImGui::Checkbox("Offset Mode (Rotation)", &rotOffsetMode);
            if (!rotOffsetMode)
            {
                ShapeItem* first = DesignManager::selectedShapes.front();
                float rot = first->baseRotation * (180.0f / IM_PI);
                if (ImGui::DragFloat("Rotation", &rot, 1.0f, 0.0f, 360.0f))
                {
                    for (ShapeItem* shape : DesignManager::selectedShapes)
                    {
                        shape->baseRotation = rot * (IM_PI / 180.0f);
                    }
                    MarkSceneUpdated();
                }
            }
            else
            {
                static float deltaRot = 0.0f;
                if (ImGui::DragFloat("Delta Rotation", &deltaRot, 1.0f, -360.0f, 360.0f))
                {
                    for (ShapeItem* shape : DesignManager::selectedShapes)
                    {
                        shape->baseRotation += (deltaRot * (IM_PI / 180.0f));
                    }
                    deltaRot = 0.0f;
                    MarkSceneUpdated();
                }
            }

            ImGui::Checkbox("Offset Mode (Fill Color)", &colOffsetMode);
            if (!colOffsetMode)
            {
                ShapeItem* first = DesignManager::selectedShapes.front();
                float col[4] = { first->fillColor.x, first->fillColor.y, first->fillColor.z, first->fillColor.w };
                if (ImGui::ColorEdit4("Fill Color", col))
                {
                    for (ShapeItem* shape : DesignManager::selectedShapes)
                    {
                        shape->fillColor = ImVec4(col[0], col[1], col[2], col[3]);
                    }
                    MarkSceneUpdated();
                }
            }
            else
            {
                float deltaCol[4] = { 0, 0, 0, 0 };
                if (ImGui::DragFloat4("Delta Fill Color", deltaCol, 0.01f))
                {
                    for (ShapeItem* shape : DesignManager::selectedShapes)
                    {
                        shape->fillColor.x += deltaCol[0];
                        shape->fillColor.y += deltaCol[1];
                        shape->fillColor.z += deltaCol[2];
                        shape->fillColor.w += deltaCol[3];
                    }
                    MarkSceneUpdated();
                }
            }
        }
    }

    inline void ShowUI_LayerShapeManager_CodeGeneration(WindowData& windowData, int selectedLayerIndex, int selectedShapeIndex)
    {
        ImGui::Separator();
        if (ImGui::Button("Generate Code for Selected Shape"))
        {
            if (DesignManager::selectedShapes.size() == 1 && selectedLayerIndex >= 0 && selectedShapeIndex >= 0 && selectedShapeIndex < (int)windowData.layers[selectedLayerIndex].shapes.size())
            {
                ShapeItem& shape = windowData.layers[selectedLayerIndex].shapes[selectedShapeIndex];
                generatedCodeForSingleShape = GenerateSingleShapeCode(shape);
            }
        }
        if (!generatedCodeForSingleShape.empty())
        {
            ImGui::Separator();
            ImGui::TextUnformatted("Single Shape Code:");
            ImGui::BeginChild("SingleShapeCode", ImVec2(-FLT_MIN, 100.0f), true, ImGuiWindowFlags_HorizontalScrollbar);
            ImGui::TextUnformatted(generatedCodeForSingleShape.c_str());
            ImGui::EndChild();
            if (ImGui::Button("Copy Single Shape Code"))
                ImGui::SetClipboardText(generatedCodeForSingleShape.c_str());
        }

        if (ImGui::Button(("Generate Code for " + DesignManager::selectedGuiWindow).c_str()))
            generatedCodeForWindow = GenerateCodeForWindow(DesignManager::selectedGuiWindow);
        ImGui::SameLine();
        if (ImGui::Button("Copy Code"))
        {
            if (!generatedCodeForWindow.empty())
                CopyToClipboard(generatedCodeForWindow);
        }
        ImGui::BeginChild("GeneratedCodeWindow", ImVec2(-FLT_MIN, 200.0f), true, ImGuiWindowFlags_HorizontalScrollbar);
        ImGui::TextUnformatted(generatedCodeForWindow.c_str());
        ImGui::EndChild();
    }


    inline void ShowUI(GLFWwindow* window)
    {
        auto& windowData = g_windowsMap[DesignManager::selectedGuiWindow];

        if (selectedLayerIndex >= windowData.layers.size())
            selectedLayerIndex = windowData.layers.empty() ? -1 : 0;
        if (selectedLayerIndex != -1 && selectedShapeIndex >= windowData.layers[selectedLayerIndex].shapes.size())
        {
            selectedShapeIndex = windowData.layers[selectedLayerIndex].shapes.empty() ? -1 : 0;
        }

        ImGui::Begin("Shape Key & Layer Manager");
        {
            ShowUI_WindowSelectionAndGroupSettings(windowData);
        }
        ImGui::Separator();

        if (!g_windowsMap.empty() && g_windowsMap.find(DesignManager::selectedGuiWindow) != g_windowsMap.end())
        {
            ShowUI_LayerManagement(windowData, selectedLayerIndex, selectedShapeIndex);
        }

        ImGui::Separator();
        ShowUI_ShapeDetails(windowData, selectedLayerIndex, selectedShapeIndex);

        ImGui::End();

        ShowUI_ParentTreeWindow();

        ImGui::Begin("Layer & Shape Manager");
        {
            ShowUI_LayerShapeManager_ChildWindowMappings();
            ShowUI_LayerShapeManager_LayerList(windowData, selectedLayerIndex, selectedShapeIndex);
            ShowUI_LayerShapeManager_ShapeProperties(windowData, selectedLayerIndex, selectedShapeIndex);
            ShowUI_LayerShapeManager_MultiShapeEdit();
            ShowUI_LayerShapeManager_CodeGeneration(windowData, selectedLayerIndex, selectedShapeIndex);

            DesignManager::RenderTemporaryWindows();
            RenderAllRegisteredWindows();
        }
        ImGui::End();
    }

    inline std::string SaveConfiguration() {
        return GenerateCodeForWindow(DesignManager::selectedGuiWindow);
    }


    // 2. DrawAllForWindow function (if applicable; here it takes a single layer as parameter):
    inline void DrawAllForWindow(const std::string& windowName, const Layer& layer) {
        if (windowName == ImGui::GetCurrentWindow()->Name) {
            std::vector<Layer> sortedLayers = { layer };
            std::stable_sort(sortedLayers.begin(), sortedLayers.end(), CompareLayersByZOrder);
            for (auto& layer_ : sortedLayers) {
                if (!layer_.visible)
                    continue;
                for (auto& shape : layer_.shapes) {
                    if (shape.parent == nullptr) {
                        DrawShape(ImGui::GetWindowDrawList(), shape, ImGui::GetWindowPos());
                    }
                }
            }
        }
    }

    inline void RenderChildWindowForShape1()
    {
        ImGui::Text("Child Window for Shape 1");
        ImGui::Separator();
        ImGui::Text("Add your child window content here.");

        if (ImGui::Button("Example Button"))
        {
            // Button processing code...
        }

        static float sliderValue = 0.5f;
        ImGui::SliderFloat("Example Slider", &sliderValue, 0.0f, 1.0f);
    }

    inline void RenderChildWindowForShape2()
    {
        ImGui::Text("Child Window for Shape 2");
        ImGui::Separator();
        ImGui::Text("Add your child window content here.");

        if (ImGui::Button("Example Button"))
        {
            // Button processing code...
        }

        static float sliderValue = 0.5f;
        ImGui::SliderFloat("Example Slider", &sliderValue, 0.0f, 1.0f);
    }
    inline void RenderChildWindowForShape3()
    {
        ImGui::Text("Child Window for Shape 3");
        ImGui::Separator();
        ImGui::Text("Add your child window content here.");

        if (ImGui::Button("Example Button"))
        {
            // Button processing code...
        }

        static float sliderValue = 0.5f;
        ImGui::SliderFloat("Example Slider", &sliderValue, 0.0f, 1.0f);
    }
    // 3. Init function:
    inline void Init(int width, int height, GLFWwindow* window) {
        // Initialize global variables appropriately:
        selectedLayerIndex = 0;
        selectedShapeIndex = -1;

        if (black_texture_id) {
            glDeleteTextures(1, (GLuint*)&black_texture_id);
            black_texture_id = 0; // Use 0 instead of nullptr
        }
        ClearGradientTextureCache();

        // Layers are no longer global; we now look at the WindowData of a specific window.
        // For example, we control it via DesignManager::selectedGuiWindow:
        auto it = g_windowsMap.find(DesignManager::selectedGuiWindow);
        if (it != g_windowsMap.end() && !it->second.layers.empty())
            selectedLayerIndex = 0;
        else
            selectedLayerIndex = -1;
        selectedShapeIndex = -1;

        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->AddFontDefault();

        // Register windows:
        RegisterWindow("ChildWindow_1", RenderChildWindowForShape1);
        RegisterWindow("ChildWindow_2", RenderChildWindowForShape2);
    }


    // --------------------------------------------------------------------------------
// Macro Definitions (For builder classes with two parameters: Method Name, Actual Member)
// --------------------------------------------------------------------------------
#define DEFINE_ANIM_SETTER(METHOD, MEMBER) \
        AnimationBuilder& set##METHOD(const decltype(anim.MEMBER)& value) { \
            anim.MEMBER = value; \
            return *this; \
        }

#define DEFINE_SHAPEKEY_SETTER(METHOD, MEMBER) \
        ShapeKeyBuilder& set##METHOD(const decltype(key.MEMBER)& value) { \
            key.MEMBER = value; \
            return *this; \
        }

#define DEFINE_SETTER(METHOD, MEMBER) \
        ShapeBuilder& set##METHOD(const decltype(shape.MEMBER)& value) { \
            shape.MEMBER = value; \
            return *this; \
        }

// --------------------------------------------------------------------------------
// Builder Classes
// --------------------------------------------------------------------------------

// AnimationBuilder
    class AnimationBuilder {
    public:
        ButtonAnimation anim;

        DEFINE_ANIM_SETTER(Name, name)
            DEFINE_ANIM_SETTER(Duration, duration)
            DEFINE_ANIM_SETTER(Speed, speed)
            DEFINE_ANIM_SETTER(AnimationTargetPosition, animationTargetPosition)
            DEFINE_ANIM_SETTER(AnimationTargetSize, animationTargetSize)
            DEFINE_ANIM_SETTER(TransformRotation, transformRotation)
            DEFINE_ANIM_SETTER(RepeatCount, repeatCount)
            DEFINE_ANIM_SETTER(PlaybackOrder, playbackOrder)
            DEFINE_ANIM_SETTER(InterpolationMethod, interpolationMethod)
            DEFINE_ANIM_SETTER(TriggerMode, triggerMode)
            DEFINE_ANIM_SETTER(Behavior, behavior)

            ButtonAnimation build() {
            return anim;
        }
    };

    // ShapeKeyBuilder
    class ShapeKeyBuilder {
    public:
        ShapeKey key;

        DEFINE_SHAPEKEY_SETTER(Name, name)
            DEFINE_SHAPEKEY_SETTER(Type, type)
            DEFINE_SHAPEKEY_SETTER(StartWindowSize, startWindowSize)
            DEFINE_SHAPEKEY_SETTER(EndWindowSize, endWindowSize)
            DEFINE_SHAPEKEY_SETTER(TargetRotation, targetRotation)
            DEFINE_SHAPEKEY_SETTER(RotationOffset, rotationOffset)
            DEFINE_SHAPEKEY_SETTER(TargetValue, targetValue)
            DEFINE_SHAPEKEY_SETTER(Offset, offset)
            DEFINE_SHAPEKEY_SETTER(Value, value)

            ShapeKey build() {
            return key;
        }
    };

    // Extended ShapeBuilder
    class ShapeBuilder {
    public:
        ShapeItem shape;

        // Basic properties
        DEFINE_SETTER(Id, id)
            DEFINE_SETTER(Name, name)
            DEFINE_SETTER(OwnerWindow, ownerWindow)
            DEFINE_SETTER(GroupId, groupId) // <-- New: groupId setter is added.
            DEFINE_SETTER(Position, position)
            DEFINE_SETTER(Size, size)
            DEFINE_SETTER(Rotation, rotation)

            // Base transform properties
            DEFINE_SETTER(BasePosition, basePosition)
            DEFINE_SETTER(BaseSize, baseSize)
            DEFINE_SETTER(BaseRotation, baseRotation)

            // Appearance and style settings
            DEFINE_SETTER(CornerRadius, cornerRadius)
            DEFINE_SETTER(BorderThickness, borderThickness)
            DEFINE_SETTER(FillColor, fillColor)
            DEFINE_SETTER(BorderColor, borderColor)
            DEFINE_SETTER(ShadowColor, shadowColor)
            DEFINE_SETTER(ShadowSpread, shadowSpread)
            DEFINE_SETTER(ShadowOffset, shadowOffset)
            DEFINE_SETTER(ShadowUseCornerRadius, shadowUseCornerRadius)
            DEFINE_SETTER(Visible, visible)
            DEFINE_SETTER(Locked, locked)
            DEFINE_SETTER(ColorRamp, colorRamp)
            // Separate function to add a ramp entry
            ShapeBuilder& addColorRampEntry(float pos, const ImVec4& color) {
            shape.colorRamp.emplace_back(pos, color);
            return *this;
        }

        // Glass effect
        DEFINE_SETTER(GlassBlur, glassBlur)
            DEFINE_SETTER(GlassAlpha, glassAlpha)
            DEFINE_SETTER(GlassColor, glassColor)

            // Button properties
            DEFINE_SETTER(IsButton, isButton)
            DEFINE_SETTER(ButtonBehavior, buttonBehavior)
            DEFINE_SETTER(HoverColor, hoverColor)
            DEFINE_SETTER(ClickedColor, clickedColor)
            DEFINE_SETTER(UseOnClick, useOnClick)
            // To add OnClick Animations
            ShapeBuilder& addOnClickAnimation(const ButtonAnimation& anim) {
            shape.onClickAnimations.push_back(anim);
            return *this;
        }

        // Text properties
        DEFINE_SETTER(HasText, hasText)
            DEFINE_SETTER(Text, text)
            DEFINE_SETTER(TextColor, textColor)
            DEFINE_SETTER(TextSize, textSize)
            DEFINE_SETTER(TextFont, textFont)
            DEFINE_SETTER(TextPosition, textPosition)
            DEFINE_SETTER(TextRotation, textRotation)
            DEFINE_SETTER(TextAlignment, textAlignment)

            // Z-order and gradient settings
            DEFINE_SETTER(ZOrder, zOrder)
            DEFINE_SETTER(UseGradient, useGradient)
            DEFINE_SETTER(GradientRotation, gradientRotation)
            DEFINE_SETTER(GradientInterpolation, gradientInterpolation)

            // To add Shape Keys
            ShapeBuilder& addShapeKey(const ShapeKey& key) {
            shape.shapeKeys.push_back(key);
            return *this;
        }

        // Child Window settings
        DEFINE_SETTER(OpenWindow, openWindow)
            DEFINE_SETTER(IsChildWindow, isChildWindow)
            DEFINE_SETTER(ChildWindowSync, childWindowSync)
            DEFINE_SETTER(ChildWindowGroupId, childWindowGroupId)
            DEFINE_SETTER(ToggleChildWindow, toggleChildWindow)
            DEFINE_SETTER(TargetShapeID, targetShapeID)

            // Additional properties:
            // Update animation base when window is resized
            DEFINE_SETTER(UpdateAnimBaseOnResize, updateAnimBaseOnResize)
            // Embedded image properties
            DEFINE_SETTER(HasEmbeddedImage, hasEmbeddedImage)
            DEFINE_SETTER(EmbeddedImageIndex, embeddedImageIndex)
            // Overlap / overlay settings
            DEFINE_SETTER(AllowItemOverlap, allowItemOverlap)
            DEFINE_SETTER(ForceOverlap, forceOverlap)
            DEFINE_SETTER(BlockUnderlying, blockUnderlying)
            // Additional shadow settings
            DEFINE_SETTER(ShadowRotation, shadowRotation)
            DEFINE_SETTER(BlurAmount, blurAmount)
            // Shape type
            DEFINE_SETTER(Type, type)
            // Dynamic text size
            DEFINE_SETTER(DynamicTextSize, dynamicTextSize)

            // Helper: Return an AnimationBuilder for chained usage
            AnimationBuilder createAnimation() {
            return AnimationBuilder();
        }
        // Helper: Return a ShapeKeyBuilder
        static ShapeKeyBuilder createShapeKey() {
            return ShapeKeyBuilder();
        }

        // Add event handler
        ShapeBuilder& addEventHandler(const std::string& eventType, const std::string& name, const std::function<void(ShapeItem&)>& handler) {
            shape.eventHandlers.push_back({eventType, name, handler});
            return *this;
        }

        // Returns the ShapeItem when all settings are complete.
        ShapeItem build() {
            return shape;
        }
    };

    // Undefine the macros (optional)
#undef DEFINE_ANIM_SETTER
#undef DEFINE_SHAPEKEY_SETTER
#undef DEFINE_SETTER



    inline void GeneratedCode()
    {


        //Generated code will be pasted here
        // Layer: EduMEKTEP_ButtonShapes_layer
        DesignManager::Layer First_layer("First_layer");
        First_layer.zOrder = 0;

        // Single Shape: Example_ButtonShape
        auto Example_ButtonShape = ShapeBuilder()
            .setId(1)
            .setName("Example_ButtonShape")
            .setOwnerWindow("Main") // you can change the owner window any time
            .setBasePosition(ImVec2(44.000000f, 152.000000f))
            .setBaseSize(ImVec2(293.000000f, 47.000000f))
            .setBaseRotation(0.000000f)
            .setCornerRadius(17.000000f)
            .setBorderThickness(2.000000f)
            .setFillColor(ImVec4(1.000000f, 1.000000f, 1.000000f, 0.160784f))
            .setBorderColor(ImVec4(0.000000f, 0.000000f, 0.000000f, 0.337255f))
            .setShadowColor(ImVec4(0.000000f, 0.000000f, 0.000000f, 0.200000f))
            .setShadowSpread(ImVec4(2.000000f, 2.000000f, 2.000000f, 2.000000f))
            .setShadowOffset(ImVec2(2.000000f, 2.000000f))
            .setShadowUseCornerRadius(true)
            .setVisible(true)
            .setLocked(false)
            .setColorRamp({
                {0.000000f, ImVec4(1.000000f, 1.000000f, 1.000000f, 1.000000f)},
                {1.000000f, ImVec4(0.500000f, 0.500000f, 0.500000f, 1.000000f)},
                })
                .setIsButton(true)
            .setButtonBehavior(DesignManager::ShapeItem::ButtonBehavior::SingleClick)
            .setUseOnClick(true)
            .setHoverColor(ImVec4(0.580392f, 0.666667f, 0.556863f, 0.450980f))
            .setClickedColor(ImVec4(0.332308f, 0.332308f, 0.332308f, 0.031373f))
            .setHasText(true)
            .setText("DERSLER")
            .setTextColor(ImVec4(0.766154f, 0.915815f, 1.000000f, 1.000000f))
            .setTextSize(38.000000f)
            .setTextFont(4)
            .setTextPosition(ImVec2(0.000000f, 0.000000f))
            .setTextRotation(0.000000f)
            .setTextAlignment(1)
            .setOpenWindow(false)
            .setIsChildWindow(false)
            .setAllowItemOverlap(false)
            .setBlockUnderlying(true)
            .build();
        Example_ButtonShape.onClick = [&]() { Example_ButtonShape_OnClick(); };
        First_layer.shapes.push_back(Example_ButtonShape);


        // Layer: Layer 2
        DesignManager::Layer Layer2("Layer 2");
        Layer2.zOrder = 1;

        // Single Shape: ChildWindow_1
        auto ChildWindow_1 = ShapeBuilder()
            .setId(2)
            .setName("ChildWindow_1")
            .setOwnerWindow("Main")
            .setBasePosition(ImVec2(381.000000f, 180.000000f))
            .setBaseSize(ImVec2(566.000000f, 317.000000f))
            .setBaseRotation(0.000000f)
            .setCornerRadius(10.000000f)
            .setBorderThickness(2.000000f)
            .setFillColor(ImVec4(0.050980f, 0.050980f, 0.050980f, 0.129412f))
            .setBorderColor(ImVec4(0.486275f, 0.996078f, 0.000000f, 0.384314f))
            .setShadowColor(ImVec4(0.000000f, 0.000000f, 0.000000f, 0.027451f))
            .setShadowSpread(ImVec4(2.000000f, 2.000000f, 2.000000f, 2.000000f))
            .setShadowOffset(ImVec2(2.000000f, 2.000000f))
            .setShadowUseCornerRadius(true)
            .setVisible(true)
            .setLocked(false)
            .setColorRamp({
                {0.000000f, ImVec4(1.000000f, 1.000000f, 1.000000f, 1.000000f)},
                {1.000000f, ImVec4(0.500000f, 0.500000f, 0.500000f, 1.000000f)},
                })
                .setOpenWindow(true)
            .setIsChildWindow(true)
            .setChildWindowSync(true)
            .setToggleChildWindow(false)
            .setChildWindowGroupId(1)
            .setTargetShapeID(2)
            .setAllowItemOverlap(false)
            .setBlockUnderlying(true)
            .build();
        Layer2.shapes.push_back(ChildWindow_1);
        // Single Shape: Shape 3
        auto Shape3 = ShapeBuilder()
            .setId(2)
            .setName("Shape 3")
            .setOwnerWindow("Main")
            .setBasePosition(ImVec2(552.000000f, 598.000000f))
            .setBaseSize(ImVec2(200.000000f, 100.000000f))
            .setBaseRotation(0.000000f)
            .setCornerRadius(10.000000f)
            .setBorderThickness(2.000000f)
            .setFillColor(ImVec4(0.932000f, 0.932000f, 0.932000f, 1.000000f))
            .setBorderColor(ImVec4(0.000000f, 0.000000f, 0.000000f, 0.800000f))
            .setShadowColor(ImVec4(0.000000f, 0.000000f, 0.000000f, 0.200000f))
            .setShadowSpread(ImVec4(2.000000f, 2.000000f, 2.000000f, 2.000000f))
            .setShadowOffset(ImVec2(2.000000f, 2.000000f))
            .setShadowUseCornerRadius(true)
            .setVisible(true)
            .setLocked(false)
            .setColorRamp({
                {0.000000f, ImVec4(1.000000f, 1.000000f, 1.000000f, 1.000000f)},
                {1.000000f, ImVec4(0.500000f, 0.500000f, 0.500000f, 1.000000f)},
                })
                .setIsButton(true)
            .setButtonBehavior(DesignManager::ShapeItem::ButtonBehavior::SingleClick)
            .setUseOnClick(false)
            .setHoverColor(ImVec4(0.800000f, 0.800000f, 0.800000f, 1.000000f))
            .setClickedColor(ImVec4(0.600000f, 0.600000f, 0.600000f, 1.000000f))
            .setOpenWindow(false)
            .setIsChildWindow(false)
            .setAllowItemOverlap(false)
            .setBlockUnderlying(true)
            .setDynamicTextSize(true)
            .build();
        Layer2.shapes.push_back(Shape3);
        g_windowsMap["Main"].layers.push_back(Layer2);
    }
}

