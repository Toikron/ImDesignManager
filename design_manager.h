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

//For someone who use SDL3 just change GLFW GLFWwindow* to SDL_Window*  ---- And glfwGetWindowSize to  SDL_GetWindowSize

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////FOR IMAGE IMPORTS//////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//IT SHOULD BE BIN2C
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
inline ImVec2 operator*(const ImVec2& v, float f)
{
    return ImVec2(v.x * f, v.y * f);
}

inline ImVec2 operator*(float f, const ImVec2& v)
{
    return ImVec2(v.x * f, v.y * f);
}
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

        std::function<void()> onStepComplete;
    };

    struct ChainAnimation {
        std::vector<ChainAnimationStep> steps;
        int currentStep = 0;
        bool isPlaying = false;
        bool reverseMode = false;
        bool toggled = false;
    };
    inline ImVec2 GetWindowSize(GLFWwindow* window)
    {
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        return ImVec2(static_cast<float>(width), static_cast<float>(height));
    }

    enum class ShapeType { Rectangle, Circle };
    enum class ChildWindowToggleBehavior {
        WindowOnly,
        ShapeAndWindow
    };




    struct ShapeItem;
    class LayoutManager;


    enum class HAlignment { Fill, Left, Center, Right };
    enum class VAlignment { Fill, Top, Center, Bottom };


    class LayoutManager {
    public:
        float spacing = 5.0f;
        float paddingTop = 0.0f;
        float paddingBottom = 0.0f;
        float paddingLeft = 0.0f;
        float paddingRight = 0.0f;

        virtual ~LayoutManager() = default;


        virtual void doLayout(ShapeItem& container, const ImVec2& availableSize) = 0;
        virtual const char* getTypeName() const = 0;
    };


    class VerticalLayout : public LayoutManager {
    public:
        virtual ~VerticalLayout() override = default;
        virtual void doLayout(ShapeItem& container, const ImVec2& availableSize) override;
        virtual const char* getTypeName() const override;
    };

    class HorizontalLayout : public LayoutManager {
    public:
        virtual ~HorizontalLayout() override = default;
        virtual void doLayout(ShapeItem& container, const ImVec2& availableSize) override;
        virtual const char* getTypeName() const override;
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

        ImVec2 minSize = ImVec2(0, 0);
        ImVec2 maxSize = ImVec2(99999, 99999);

        bool isChildWindow = false;
        bool childWindowSync = false;
        bool toggleChildWindow = false;
        ChildWindowToggleBehavior toggleBehavior = ChildWindowToggleBehavior::WindowOnly;
        int childWindowGroupId = -1;


        int targetShapeID = 0;
        int triggerGroupID = 0;


        bool isImGuiContainer = false;
        std::function<void()> renderImGuiContent = nullptr;

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
        ChainAnimation chainAnimation;
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

        std::vector<std::string> assignedChildWindows;
        int selectedChildWindowIndex = 0;
        std::string logicExpression;
        std::string logicAction;
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
            std::string eventType;
            std::string name;
            std::function<void(ShapeItem&)> handler;
        };
        std::vector<EventHandler> eventHandlers;


        enum class AnchorMode {
            None,
            TopLeft, Top, TopRight,
            Left, Center, Right,
            BottomLeft, Bottom, BottomRight
        };
        AnchorMode anchorMode = AnchorMode::None;
        ImVec2 anchorMargin = ImVec2(0, 0);

        bool usePercentagePos = false;
        ImVec2 percentagePos = ImVec2(0, 0);

        bool usePercentageSize = false;
        ImVec2 percentageSize = ImVec2(10, 10);

        bool isLayoutContainer = false;
        LayoutManager* layoutManager = nullptr;
        float stretchFactor = 0.0f;
        HAlignment horizontalAlignment = HAlignment::Fill;
        VAlignment verticalAlignment = VAlignment::Fill;



        ShapeItem()
            : id(nextShapeID++),
            type(ShapeType::Rectangle),
            name("Shape"),
            position(ImVec2(100, 100)),
            size(ImVec2(200, 100)),
            minSize(ImVec2(0, 0)),
            maxSize(ImVec2(99999, 99999)),
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

            isChildWindow(false),
            isImGuiContainer(false),
            renderImGuiContent(nullptr),

            anchorMode(AnchorMode::None),
            anchorMargin(ImVec2(0, 0)),
            usePercentagePos(false),
            percentagePos(ImVec2(0, 0)),
            usePercentageSize(false),
            percentageSize(ImVec2(10, 10)),

            isLayoutContainer(false),
            layoutManager(nullptr),
            stretchFactor(0.0f),
            horizontalAlignment(HAlignment::Fill),
            verticalAlignment(VAlignment::Fill)

        {
            colorRamp.emplace_back(0.0f, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
            colorRamp.emplace_back(1.0f, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
            basePosition = position;
            baseSize = size;
            baseRotation = rotation;
        }



        ~ShapeItem() {
            delete layoutManager;
        }

    };
    inline const char* VerticalLayout::getTypeName() const { return "Vertical"; }
    inline void VerticalLayout::doLayout(ShapeItem& container, const ImVec2& availableSize)
    {
        float availableInnerWidth = std::max(0.0f, availableSize.x - paddingLeft - paddingRight);
        float availableInnerHeight = std::max(0.0f, availableSize.y - paddingTop - paddingBottom);

        float totalStretch = 0.0f;
        float totalFixedY = 0.0f;
        int visibleChildrenCount = 0;

        if (container.children.empty()) {
            return;
        }

        for (ShapeItem* child : container.children) {
            if (!child) {
                std::cerr << "  WARNING: Null child pointer found in container ID: " << container.id << std::endl;
                continue;
            }
            if (!child->visible) continue;
            visibleChildrenCount++;

            if (child->verticalAlignment != VAlignment::Fill && child->stretchFactor <= 0.0f) {
                totalFixedY += child->size.y;
            }
            else {
                totalStretch += std::max(0.0f, child->stretchFactor);
            }
        }

        float totalSpacing = (visibleChildrenCount > 1) ? (spacing * (visibleChildrenCount - 1)) : 0.0f;
        totalFixedY += totalSpacing;
        float availableStretchY = std::max(0.0f, availableInnerHeight - totalFixedY);

        float currentY = paddingTop;
        for (ShapeItem* child : container.children) {
            if (!child || !child->visible) continue;

            float childHeight = child->size.y;
            if (child->verticalAlignment == VAlignment::Fill || child->stretchFactor > 0.0f) {
                if (totalStretch > 1e-6f && child->stretchFactor > 0.0f) {
                    childHeight = (child->stretchFactor / totalStretch) * availableStretchY;
                }
                else if (child->verticalAlignment == VAlignment::Fill && totalStretch <= 1e-6f) {







                    childHeight = availableStretchY / std::max(1, (int)std::count_if(container.children.begin(), container.children.end(),
                        [](ShapeItem* c) { return c && c->visible && (c->verticalAlignment == VAlignment::Fill || c->stretchFactor > 0.0f); }));

                }
            }

            childHeight = std::max(child->minSize.y, std::min(childHeight, child->maxSize.y));
            childHeight = std::max(0.0f, childHeight);

            float childWidth = child->size.x;
            if (child->horizontalAlignment == HAlignment::Fill) {
                childWidth = availableInnerWidth;
            }

            childWidth = std::max(child->minSize.x, std::min(childWidth, child->maxSize.x));
            childWidth = std::max(0.0f, childWidth);


            float childPosX = paddingLeft;
            if (child->horizontalAlignment == HAlignment::Center) {
                childPosX = paddingLeft + (availableInnerWidth - childWidth) * 0.5f;
            }
            else if (child->horizontalAlignment == HAlignment::Right) {
                childPosX = paddingLeft + availableInnerWidth - childWidth;
            }

            float childPosY = currentY;


            child->originalPosition = ImVec2(childPosX, childPosY);

            child->size = ImVec2(childWidth, childHeight);



            currentY += childHeight + spacing;
        }
    }

    inline const char* HorizontalLayout::getTypeName() const { return "Horizontal"; }
    inline void HorizontalLayout::doLayout(ShapeItem& container, const ImVec2& availableSize)
    {
        float availableInnerWidth = std::max(0.0f, availableSize.x - paddingLeft - paddingRight);
        float availableInnerHeight = std::max(0.0f, availableSize.y - paddingTop - paddingBottom);

        float totalStretch = 0.0f;
        float totalFixedX = 0.0f;
        int visibleChildrenCount = 0;

        if (container.children.empty()) {
            return;
        }

        for (ShapeItem* child : container.children) {
            if (!child) {
                std::cerr << "  WARNING: Null child pointer found in container ID: " << container.id << std::endl;
                continue;
            }
            if (!child->visible) continue;
            visibleChildrenCount++;
            if (child->horizontalAlignment != HAlignment::Fill && child->stretchFactor <= 0.0f) {
                totalFixedX += child->size.x;
            }
            else {
                totalStretch += std::max(0.0f, child->stretchFactor);
            }
        }

        float totalSpacing = (visibleChildrenCount > 1) ? (spacing * (visibleChildrenCount - 1)) : 0.0f;
        totalFixedX += totalSpacing;
        float availableStretchX = std::max(0.0f, availableInnerWidth - totalFixedX);

        float currentX = paddingLeft;
        for (ShapeItem* child : container.children) {
            if (!child || !child->visible) continue;

            float childWidth = child->size.x;
            if (child->horizontalAlignment == HAlignment::Fill || child->stretchFactor > 0.0f) {
                if (totalStretch > 1e-6f && child->stretchFactor > 0.0f) {
                    childWidth = (child->stretchFactor / totalStretch) * availableStretchX;
                }
                else if (child->horizontalAlignment == HAlignment::Fill && totalStretch <= 1e-6f) {

                    childWidth = availableStretchX / std::max(1, (int)std::count_if(container.children.begin(), container.children.end(),
                        [](ShapeItem* c) { return c && c->visible && (c->horizontalAlignment == HAlignment::Fill || c->stretchFactor > 0.0f); }));
                }
            }

            childWidth = std::max(child->minSize.x, std::min(childWidth, child->maxSize.x));
            childWidth = std::max(0.0f, childWidth);

            float childHeight = child->size.y;
            if (child->verticalAlignment == VAlignment::Fill) {
                childHeight = availableInnerHeight;
            }

            childHeight = std::max(child->minSize.y, std::min(childHeight, child->maxSize.y));
            childHeight = std::max(0.0f, childHeight);

            float childPosY = paddingTop;
            if (child->verticalAlignment == VAlignment::Center) {
                childPosY = paddingTop + (availableInnerHeight - childHeight) * 0.5f;
            }
            else if (child->verticalAlignment == VAlignment::Bottom) {
                childPosY = paddingTop + availableInnerHeight - childHeight;
            }

            float childPosX = currentX;


            child->originalPosition = ImVec2(childPosX, childPosY);

            child->size = ImVec2(childWidth, childHeight);



            currentX += childWidth + spacing;
        }
    }








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
        int shapeId;
        std::string logicOp;
        std::vector<int> buttonIds;
        std::vector<std::string> childWindowKeys;
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

    struct ComponentShapeTemplate {
        ShapeItem item;
        int originalId = -1;
        int originalParentId = -1;
    };


    struct ComponentDefinition {
        std::string name;
        std::vector<ComponentShapeTemplate> shapeTemplates;
    };


    inline std::map<std::string, ComponentDefinition> g_componentDefinitions;

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



        bool forceAlwaysOpen = false;
        for (const auto& mapping : g_combinedChildWindowMappings) {
            for (size_t i = 0; i < mapping.childWindowKeys.size(); ++i) {
                if (mapping.childWindowKeys[i] == name && mapping.buttonIds[i] == -1) {
                    forceAlwaysOpen = true;
                    break;
                }
            }
            if (forceAlwaysOpen)
                break;
        }

        if (forceAlwaysOpen)
            open = true;


        if (!it->second.isChildWindow && open && it->second.groupId > 0)
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


        if (it->second.isChildWindow)
        {
            for (auto shape : GetAllShapes())
            {
                if (shape->name == name)
                {
                    shape->isChildWindow = open;
                    break;
                }
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

    inline bool IsAncestor(const ShapeItem* potentialAncestor, const ShapeItem* shape) {
        if (!potentialAncestor || !shape || !shape->parent) {
            return false;
        }
        const ShapeItem* current = shape->parent;
        while (current != nullptr) {
            if (current == potentialAncestor) {
                return true;
            }
            current = current->parent;
        }
        return false;
    }
    static void SetParent(ShapeItem* child, ShapeItem* parent)
    {
        if (child == nullptr || parent == nullptr || child == parent) return;
        if (IsAncestor(child, parent)) {
            std::cerr << "Error: Cannot set parent, would create a cycle." << std::endl;
            return;
        }



        if (child->parent != nullptr) {





            RemoveParent(child);
        }


        ImVec2 parentPos = parent->position;
        float parentRot = parent->rotation;
        ImVec2 childPos = child->position;
        float childRot = child->rotation;


        ImVec2 worldOffset = childPos - parentPos;


        ImVec2 localOffset = RotateP(worldOffset, ImVec2(0.0f, 0.0f), -parentRot);


        child->originalPosition = localOffset;


        child->baseRotation = childRot - parentRot;


        child->parent = parent;
        parent->children.push_back(child);



    }



    static void RemoveParentKeepTransform(ShapeItem* child)
    {
        if (child == nullptr || child->parent == nullptr) return;

        ShapeItem* parent = child->parent;



        ImVec2 parentCurrentPos = parent->position;
        float parentCurrentRot = parent->rotation;


        ImVec2 localOffset = child->originalPosition;


        ImVec2 rotatedLocalOffset = RotateP(localOffset, ImVec2(0.0f, 0.0f), parentCurrentRot);


        ImVec2 worldPosition = parentCurrentPos + rotatedLocalOffset;


        float worldRotation = parentCurrentRot + child->baseRotation;



        auto& children = parent->children;
        children.erase(std::remove(children.begin(), children.end(), child), children.end());
        child->parent = nullptr;


        child->position = worldPosition;
        child->rotation = worldRotation;
        child->basePosition = worldPosition;
        child->baseRotation = worldRotation;


        child->originalPosition = ImVec2(0, 0);
    }

    inline std::string selectedGuiWindow = "Main";







    inline void DrawShape_RenderImGuiContent(ImDrawList* dl, ShapeItem& s, ImVec2 wp, float scaleFactor) {
        if (!s.isImGuiContainer) return;




        if (std::fabs(s.rotation) > 1e-4f) {

            return;




        }


        std::string childId = "ImGuiContainer_" + std::to_string(s.id);
        ImVec2 childPos = ImVec2(wp.x + s.position.x * scaleFactor, wp.y + s.position.y * scaleFactor);
        ImVec2 childSize = ImVec2(s.size.x * scaleFactor, s.size.y * scaleFactor);






        ImGui::PushID(childId.c_str());

        ImGui::SetCursorScreenPos(childPos);





        ImGui::BeginChild(childId.c_str(),
            childSize,
            false,
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse
        );


        if (s.renderImGuiContent) {
            try {
                s.renderImGuiContent();
            }
            catch (const std::exception& e) {

                ImGui::TextColored(ImVec4(1, 0, 0, 1), "Error in renderImGuiContent:");
                ImGui::TextWrapped("%s", e.what());
            }
            catch (...) {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "Unknown error in renderImGuiContent");
            }

        }
        else {

            ImGui::TextDisabled("renderImGuiContent not set");
        }

        ImGui::EndChild();



        ImGui::PopID();
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
    inline void DrawShapeTreeNode(ShapeItem* shape, Layer& layer, int layerIndex, int shapeIndexInLayer, int& selectedLayerIndex, std::vector<ShapeItem*>& selectedShapes, ShapeItem*& lastClickedShape, int& lastClickedLayerIndex, int& layerToSort, bool& needsLayerSort);


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

        if (!item.hasEmbeddedImage) return;
        if (item.embeddedImageTexture != 0) return;


        if (item.embeddedImageData.empty() && item.embeddedImageIndex >= 0 && item.embeddedImageIndex < g_embeddedImageFuncsCount) {

            item.embeddedImageData = g_embeddedImageFuncs[item.embeddedImageIndex]();
        }


        if (item.embeddedImageData.empty()) return;


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

        if (s.forceOverlap)
            ImGui::SetNextItemAllowOverlap();
        ImGui::SetCursorScreenPos(button_abs_pos);

        ImGui::InvisibleButton(button_id.c_str(), button_size);




        bool is_hovered = ImGui::IsItemHovered();
        bool is_active = ImGui::IsItemActive();




        if (!s.allowItemOverlap && ImGui::GetHoveredID() != 0 && ImGui::GetHoveredID() != ImGui::GetID(button_id.c_str())) {

            is_hovered = false;
            is_active = false;
        }

        else if (s.allowItemOverlap) {
            ImVec2 mousePos = ImGui::GetIO().MousePos;
            ImRect buttonRect(button_abs_pos, button_abs_pos + button_size);
            if (buttonRect.Contains(mousePos)) {

                is_hovered = true;

                is_active = ImGui::IsMouseDown(0) && is_hovered;
            }
            else {

                is_hovered = false;
                is_active = false;
            }
        }



        if (s.forceOverlap && s.allowItemOverlap && s.blockUnderlying) {

            ImGui::SetNextWindowPos(button_abs_pos);
            ImGui::SetNextWindowSize(button_size);
            std::string overlay_id = "BlockOverlay_" + std::to_string(s.id);
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
            ImGui::BeginChild(overlay_id.c_str(), button_size, false,
                ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs);

            ImGui::EndChild();
            ImGui::PopStyleColor();
        }



        drawColor = s.fillColor;
        if (is_hovered) {
            drawColor = s.hoverColor;
        }


        for (auto& anim : s.onClickAnimations) {
            if (anim.triggerMode == ButtonAnimation::TriggerMode::OnHover) {
                if (is_hovered) {
                    if (!anim.isPlaying) {

                        if (anim.behavior == ButtonAnimation::AnimationBehavior::Toggle) {
                            if (!anim.toggleState) {
                                anim.isPlaying = true;
                                anim.progress = 0.0f;
                                anim.speed = std::fabs(anim.speed);
                                s.currentAnimation = &anim;
                                anim.startTime = (float)ImGui::GetTime();
                            }
                        }
                        else {
                            anim.isPlaying = true;
                            anim.progress = 0.0f;
                            anim.speed = std::fabs(anim.speed);
                            s.currentAnimation = &anim;
                            anim.startTime = (float)ImGui::GetTime();
                        }
                    }
                }
                else {

                    if (anim.isPlaying) {

                        if (anim.behavior == ButtonAnimation::AnimationBehavior::PlayOnceAndReverse && anim.progress > 0.0f && anim.speed > 0.0f) {

                            anim.speed = -std::fabs(anim.speed);
                        }
                        else if (anim.behavior == ButtonAnimation::AnimationBehavior::Toggle) {
                            if (anim.toggleState) {
                                anim.isPlaying = true;
                                anim.speed = -std::fabs(anim.speed);

                            }

                        }

                    }
                }
            }
        }


        if (s.buttonBehavior == ShapeItem::ButtonBehavior::SingleClick) {
            if (ImGui::IsItemClicked()) {

                s.shouldCallOnClick = true;
            }
            if (is_active) {
                drawColor = s.clickedColor;
            }


        }
        else if (s.buttonBehavior == ShapeItem::ButtonBehavior::Toggle) {
            if (ImGui::IsItemClicked()) {
                bool newState = !s.buttonState;

                if (newState && s.groupId > 0) {
                    auto allButtons = GetAllButtonShapes();
                    for (auto* otherButton : allButtons) {
                        if (otherButton && otherButton->id != s.id && otherButton->groupId == s.groupId) {
                            otherButton->buttonState = false;
                        }
                    }
                }
                s.buttonState = newState;
                s.shouldCallOnClick = true;
            }


            if (s.buttonState) {
                drawColor = s.clickedColor;
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
            else {
                if (s.buttonState) {
                    s.shouldCallOnClick = true;
                    s.buttonState = false;
                }

            }

            if (s.buttonState) {
                drawColor = s.clickedColor;
            }
        }


        if (s.shouldCallOnClick) {

            DispatchEvent(s, "onClick");


            if (!s.onClickAnimations.empty()) {
                std::vector<int> onClickIndices;
                onClickIndices.reserve(s.onClickAnimations.size());
                for (int i = 0; i < (int)s.onClickAnimations.size(); i++) {
                    if (s.onClickAnimations[i].triggerMode == ButtonAnimation::TriggerMode::OnClick) {
                        onClickIndices.push_back(i);
                    }
                }




                static int currentAnimIndexMap[10000];
                int& currentAnimIndex = currentAnimIndexMap[s.id % 10000];

                if (!onClickIndices.empty()) {
                    if (currentAnimIndex < 0 || currentAnimIndex >= onClickIndices.size()) {
                        currentAnimIndex = 0;
                    }

                    int animToPlayIdx = onClickIndices[currentAnimIndex];
                    ButtonAnimation& anim = s.onClickAnimations[animToPlayIdx];


                    if (anim.behavior == ButtonAnimation::AnimationBehavior::Toggle) {
                        if (!anim.isPlaying) {
                            anim.isPlaying = true;
                            anim.speed = anim.toggleState ? -std::fabs(anim.speed) : std::fabs(anim.speed);
                            anim.progress = anim.toggleState ? 1.0f : 0.0f;
                            s.currentAnimation = &anim;
                            anim.startTime = (float)ImGui::GetTime();
                        }
                    }
                    else {
                        anim.progress = 0.0f;
                        anim.speed = std::fabs(anim.speed);
                        anim.isPlaying = true;
                        s.currentAnimation = &anim;
                        anim.startTime = (float)ImGui::GetTime();
                    }



                    currentAnimIndex = (currentAnimIndex + 1) % onClickIndices.size();
                }
            }
            s.shouldCallOnClick = false;
        }


        s.isHeld = is_active;
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


            if (mapping.logicOp == "None")
            {

                for (size_t i = 0; i < mapping.buttonIds.size(); i++)
                {
                    bool btnState = false;

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

                    g_windowsMap[childKey].groupId = s.childWindowGroupId;

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

                for (auto& childKey : mapping.childWindowKeys)
                {
                    g_windowsMap[childKey].groupId = s.childWindowGroupId;
                    SetWindowOpen(childKey, conditionMet);
                }
            }


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




        DrawShape_RenderImGuiContent(dl, s, wp, scaleFactor);

        if (s.isChildWindow && !s.isImGuiContainer) {
            DrawShape_RenderChildWindow(s, wp, scaleFactor, poly);



        }
        else if (!s.isImGuiContainer) {
            DrawShape_DrawBorder(dlEffective, s, scaleFactor, c, wp);

            if (s.useGlass) {

            }
            DrawShape_DrawText(dlEffective, s, scaleFactor, wp);
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

        DesignManager::sceneUpdated = false;
        shouldCaptureScene = true;
        glFinish();
    }

    inline float NormalizeProgress(float progress) {
        return progress / 100.0f;
    }



    inline int FindShapeLayerIndex(int shapeId) {
        if (g_windowsMap.count(selectedGuiWindow)) {
            WindowData& currentWindowData = g_windowsMap.at(selectedGuiWindow);
            for (size_t i = 0; i < currentWindowData.layers.size(); ++i) {
                for (const auto& shape : currentWindowData.layers[i].shapes) {
                    if (shape.id == shapeId) {
                        return static_cast<int>(i);
                    }
                }
            }
        }
        return -1;
    }

    inline void UpdateShapeTransforms_Unified(GLFWwindow* window, float deltaTime)
    {
        for (auto& [winName, windowData] : g_windowsMap)
        {

            ImGuiWindow* imguiWindow = ImGui::FindWindowByName(winName.c_str());
            ImVec2 baseWindowSize = GetWindowSize(window);
            ImVec2 containerSizeForShapeKeys = (imguiWindow != nullptr && imguiWindow->Size.x > 0 && imguiWindow->Size.y > 0) ? imguiWindow->Size : baseWindowSize;
            containerSizeForShapeKeys.x = std::max(1.0f, containerSizeForShapeKeys.x);
            containerSizeForShapeKeys.y = std::max(1.0f, containerSizeForShapeKeys.y);


            std::vector<ShapeItem*> rootShapes;
            for (auto& layer : windowData.layers) {
                for (auto& shape : layer.shapes) {
                    if (shape.parent == nullptr) {
                        rootShapes.push_back(&shape);
                    }
                }
            }


            std::function<void(ShapeItem*, const ImVec2&, ShapeItem*)> processShapeRecursive =
                [&](ShapeItem* shape, const ImVec2& parentSize, ShapeItem* parentShape)
                {
                    if (!shape) {
                        return;
                    }

                    if (!shape->visible) {



                        return;
                    }


                    bool layer_is_locked = false;
                    int current_shape_layer_idx = FindShapeLayerIndex(shape->id);
                    if (current_shape_layer_idx != -1 && g_windowsMap.count(shape->ownerWindow) && current_shape_layer_idx < g_windowsMap[shape->ownerWindow].layers.size()) {
                        layer_is_locked = g_windowsMap[shape->ownerWindow].layers[current_shape_layer_idx].locked;
                    }
                    bool shape_effectively_locked = shape->locked || layer_is_locked;


                    ImVec2 currentContainerSize = parentSize;
                    currentContainerSize.x = std::max(1.0f, currentContainerSize.x);
                    currentContainerSize.y = std::max(1.0f, currentContainerSize.y);


                    ImVec2 effectiveBasePos = shape->basePosition + shape->baseKeyOffset;
                    ImVec2 effectiveBaseSize = shape->baseSize + shape->baseKeySizeOffset;
                    float effectiveBaseRot = shape->baseRotation + shape->baseKeyRotationOffset;


                    ImVec2 shapeKeyResultPosition = effectiveBasePos;
                    ImVec2 shapeKeyResultSize = effectiveBaseSize;
                    float shapeKeyResultRotation = effectiveBaseRot;

                    if (!shape_effectively_locked && !shape->shapeKeys.empty())
                    {
                        float blendedPosX = 0.0f, blendedPosY = 0.0f, blendedSizeX = 0.0f, blendedSizeY = 0.0f, blendedRot = 0.0f;
                        int countPosX = 0, countPosY = 0, countSizeX = 0, countSizeY = 0, countRot = 0;
                        for (auto& key : shape->shapeKeys) {
                            float currentDim = 0.0f, startDim = 0.0f, endDim = 0.0f;
                            float baseVal = 0.0f, targetVal = 0.0f;


                            ImVec2 currentWindowContextSize = containerSizeForShapeKeys;
                            switch (key.type) {
                            case ShapeKeyType::PositionX: currentDim = currentWindowContextSize.x; startDim = key.startWindowSize.x; endDim = key.endWindowSize.x; baseVal = effectiveBasePos.x; targetVal = key.targetValue.x; break;
                            case ShapeKeyType::PositionY: currentDim = currentWindowContextSize.y; startDim = key.startWindowSize.y; endDim = key.endWindowSize.y; baseVal = effectiveBasePos.y; targetVal = key.targetValue.y; break;
                            case ShapeKeyType::SizeX:     currentDim = currentWindowContextSize.x; startDim = key.startWindowSize.x; endDim = key.endWindowSize.x; baseVal = effectiveBaseSize.x; targetVal = key.targetValue.x; break;
                            case ShapeKeyType::SizeY:     currentDim = currentWindowContextSize.y; startDim = key.startWindowSize.y; endDim = key.endWindowSize.y; baseVal = effectiveBaseSize.y; targetVal = key.targetValue.y; break;
                            case ShapeKeyType::Rotation:  currentDim = currentWindowContextSize.x; startDim = key.startWindowSize.x; endDim = key.endWindowSize.x; baseVal = effectiveBaseRot; targetVal = key.targetRotation; break;
                            }
                            float t = (fabs(endDim - startDim) > 1e-6f) ? std::clamp((currentDim - startDim) / (endDim - startDim), 0.0f, 1.0f) : ((currentDim < startDim) ? 0.0f : 1.0f);
                            key.value = t;
                            float computedVal = Lerp(baseVal, targetVal, t);
                            if (key.type == ShapeKeyType::Rotation) computedVal += key.rotationOffset;
                            else computedVal += (key.type == ShapeKeyType::PositionX || key.type == ShapeKeyType::SizeX) ? key.offset.x : key.offset.y;
                            switch (key.type) {
                            case ShapeKeyType::PositionX: blendedPosX += computedVal; countPosX++; break;
                            case ShapeKeyType::PositionY: blendedPosY += computedVal; countPosY++; break;
                            case ShapeKeyType::SizeX:     blendedSizeX += computedVal; countSizeX++; break;
                            case ShapeKeyType::SizeY:     blendedSizeY += computedVal; countSizeY++; break;
                            case ShapeKeyType::Rotation:  blendedRot += computedVal; countRot++; break;
                            }
                        }
                        shapeKeyResultPosition = ImVec2((countPosX > 0 ? blendedPosX / countPosX : effectiveBasePos.x), (countPosY > 0 ? blendedPosY / countPosY : effectiveBasePos.y));
                        shapeKeyResultSize = ImVec2((countSizeX > 0 ? blendedSizeX / countSizeX : effectiveBaseSize.x), (countSizeY > 0 ? blendedSizeY / countSizeY : effectiveBaseSize.y));
                        shapeKeyResultRotation = (countRot > 0 ? blendedRot / countRot : effectiveBaseRot);
                    }


                    ImVec2 layoutResultPosition = shapeKeyResultPosition;
                    ImVec2 layoutResultSize = shapeKeyResultSize;
                    float layoutResultRotation = shapeKeyResultRotation;


                    bool parentIsLayout = (parentShape != nullptr && parentShape->isLayoutContainer && parentShape->layoutManager != nullptr);


                    if (!parentIsLayout && !shape_effectively_locked)
                    {

                        if (shape->usePercentageSize) {
                            layoutResultSize.x = currentContainerSize.x * (shape->percentageSize.x / 100.0f);
                            layoutResultSize.y = currentContainerSize.y * (shape->percentageSize.y / 100.0f);
                        }

                        layoutResultSize.x = std::max(shape->minSize.x, std::min(layoutResultSize.x, shape->maxSize.x));
                        layoutResultSize.y = std::max(shape->minSize.y, std::min(layoutResultSize.y, shape->maxSize.y));
                        layoutResultSize.x = std::max(0.0f, layoutResultSize.x);
                        layoutResultSize.y = std::max(0.0f, layoutResultSize.y);


                        if (shape->usePercentagePos && shape->anchorMode == ShapeItem::AnchorMode::None) {
                            layoutResultPosition.x = currentContainerSize.x * (shape->percentagePos.x / 100.0f);
                            layoutResultPosition.y = currentContainerSize.y * (shape->percentagePos.y / 100.0f);
                        }


                        if (shape->anchorMode != ShapeItem::AnchorMode::None) {
                            switch (shape->anchorMode) {
                            case ShapeItem::AnchorMode::TopLeft: layoutResultPosition = shape->anchorMargin; break;
                            case ShapeItem::AnchorMode::Top: layoutResultPosition = ImVec2((currentContainerSize.x - layoutResultSize.x) * 0.5f + shape->anchorMargin.x, shape->anchorMargin.y); break;
                            case ShapeItem::AnchorMode::TopRight: layoutResultPosition = ImVec2(currentContainerSize.x - layoutResultSize.x - shape->anchorMargin.x, shape->anchorMargin.y); break;
                            case ShapeItem::AnchorMode::Left: layoutResultPosition = ImVec2(shape->anchorMargin.x, (currentContainerSize.y - layoutResultSize.y) * 0.5f + shape->anchorMargin.y); break;
                            case ShapeItem::AnchorMode::Center: layoutResultPosition = ImVec2((currentContainerSize.x - layoutResultSize.x) * 0.5f + shape->anchorMargin.x, (currentContainerSize.y - layoutResultSize.y) * 0.5f + shape->anchorMargin.y); break;
                            case ShapeItem::AnchorMode::Right: layoutResultPosition = ImVec2(currentContainerSize.x - layoutResultSize.x - shape->anchorMargin.x, (currentContainerSize.y - layoutResultSize.y) * 0.5f + shape->anchorMargin.y); break;
                            case ShapeItem::AnchorMode::BottomLeft: layoutResultPosition = ImVec2(shape->anchorMargin.x, currentContainerSize.y - layoutResultSize.y - shape->anchorMargin.y); break;
                            case ShapeItem::AnchorMode::Bottom: layoutResultPosition = ImVec2((currentContainerSize.x - layoutResultSize.x) * 0.5f + shape->anchorMargin.x, currentContainerSize.y - layoutResultSize.y - shape->anchorMargin.y); break;
                            case ShapeItem::AnchorMode::BottomRight: layoutResultPosition = ImVec2(currentContainerSize.x - layoutResultSize.x - shape->anchorMargin.x, currentContainerSize.y - layoutResultSize.y - shape->anchorMargin.y); break;
                            case ShapeItem::AnchorMode::None: default: break;
                            }
                        }
                    }
                    else if (shape_effectively_locked) {

                        layoutResultPosition = shapeKeyResultPosition;
                        layoutResultSize = shapeKeyResultSize;
                        layoutResultRotation = shapeKeyResultRotation;
                    }




                    ImVec2 animationBasePos = shape->updateAnimBaseOnResize ? layoutResultPosition : effectiveBasePos;
                    ImVec2 animationBaseSize = shape->updateAnimBaseOnResize ? layoutResultSize : effectiveBaseSize;
                    float animationBaseRot = shape->updateAnimBaseOnResize ? layoutResultRotation : effectiveBaseRot;


                    ImVec2 finalPosition = layoutResultPosition;
                    ImVec2 finalSize = layoutResultSize;
                    float finalRotation = layoutResultRotation;


                    if (!shape_effectively_locked && shape->currentAnimation && shape->currentAnimation->isPlaying)
                    {
                        ButtonAnimation* anim = shape->currentAnimation;

                        switch (anim->behavior) {
                        case ButtonAnimation::AnimationBehavior::PlayOnceAndStay:
                            anim->progress = std::min(1.0f, anim->progress + deltaTime * std::fabs(anim->speed));
                            if (anim->progress >= 1.0f) {
                                if (!anim->hasStartedRepeatCount) { anim->hasStartedRepeatCount = true; anim->remainingRepeats = (anim->repeatCount <= 0) ? -1 : anim->repeatCount - 1; }
                                if (anim->remainingRepeats < 0) {
                                    anim->progress = 1.0f;
                                    anim->isPlaying = false;
                                    anim->persistentPositionOffset = Lerp(animationBasePos, anim->animationTargetPosition, 1.0f);
                                    anim->persistentSizeOffset = Lerp(animationBaseSize, anim->animationTargetSize, 1.0f);
                                    anim->persistentRotationOffset = Lerp(animationBaseRot, anim->transformRotation, 1.0f);
                                    shape->currentAnimation = nullptr;
                                }
                                else if (anim->remainingRepeats > 0) {
                                    anim->remainingRepeats--;
                                    anim->progress = 0.0f;
                                }
                                else {
                                    anim->progress = 1.0f;
                                    anim->isPlaying = false;
                                    anim->persistentPositionOffset = Lerp(animationBasePos, anim->animationTargetPosition, 1.0f);
                                    anim->persistentSizeOffset = Lerp(animationBaseSize, anim->animationTargetSize, 1.0f);
                                    anim->persistentRotationOffset = Lerp(animationBaseRot, anim->transformRotation, 1.0f);
                                    shape->currentAnimation = nullptr;
                                    anim->hasStartedRepeatCount = false;
                                }
                            }
                            break;
                        case ButtonAnimation::AnimationBehavior::PlayOnceAndReverse:
                            anim->progress += deltaTime * anim->speed;
                            anim->progress = std::clamp(anim->progress, 0.0f, 1.0f);
                            if (anim->speed > 0.0f && anim->progress >= 1.0f) {
                                anim->speed *= -1.0f;
                            }
                            else if (anim->speed < 0.0f && anim->progress <= 0.0f) {
                                if (!anim->hasStartedRepeatCount) { anim->hasStartedRepeatCount = true; anim->remainingRepeats = (anim->repeatCount <= 0) ? -1 : anim->repeatCount - 1; }
                                if (anim->remainingRepeats < 0) {
                                    anim->speed *= -1.0f;
                                    anim->progress = 0.0f;
                                    anim->isPlaying = false;
                                    anim->persistentPositionOffset = ImVec2(0, 0);
                                    anim->persistentSizeOffset = ImVec2(0, 0);
                                    anim->persistentRotationOffset = 0.0f;
                                    shape->currentAnimation = nullptr;
                                }
                                else if (anim->remainingRepeats > 0) {
                                    anim->remainingRepeats--;
                                    anim->speed *= -1.0f;
                                }
                                else {
                                    anim->progress = 0.0f;
                                    anim->speed = std::fabs(anim->speed);
                                    anim->isPlaying = false;
                                    anim->persistentPositionOffset = ImVec2(0, 0);
                                    anim->persistentSizeOffset = ImVec2(0, 0);
                                    anim->persistentRotationOffset = 0.0f;
                                    shape->currentAnimation = nullptr;
                                    anim->hasStartedRepeatCount = false;
                                }
                            }
                            break;
                        case ButtonAnimation::AnimationBehavior::Toggle:
                            if (!anim->toggleState) {
                                anim->progress = std::min(1.0f, anim->progress + deltaTime * std::fabs(anim->speed));
                                if (anim->progress >= 1.0f) {
                                    anim->toggleState = true;
                                    anim->isPlaying = false;
                                    anim->progress = 1.0f;
                                    anim->persistentPositionOffset = Lerp(animationBasePos, anim->animationTargetPosition, 1.0f);
                                    anim->persistentSizeOffset = Lerp(animationBaseSize, anim->animationTargetSize, 1.0f);
                                    anim->persistentRotationOffset = Lerp(animationBaseRot, anim->transformRotation, 1.0f);
                                    shape->currentAnimation = nullptr;
                                }
                            }
                            else {
                                anim->progress = std::max(0.0f, anim->progress - deltaTime * std::fabs(anim->speed));
                                if (anim->progress <= 0.0f) {
                                    anim->toggleState = false;
                                    anim->isPlaying = false;
                                    anim->progress = 0.0f;
                                    anim->persistentPositionOffset = ImVec2(0, 0);
                                    anim->persistentSizeOffset = ImVec2(0, 0);
                                    anim->persistentRotationOffset = 0.0f;
                                    shape->currentAnimation = nullptr;
                                }
                            }
                            break;
                        case ButtonAnimation::AnimationBehavior::PlayWhileHoldingAndReverseOnRelease:
                            if (shape->isHeld) {
                                anim->progress = std::min(1.0f, anim->progress + deltaTime * std::fabs(anim->speed));
                            }
                            else {
                                anim->progress = std::max(0.0f, anim->progress - deltaTime * std::fabs(anim->speed));

                                if (anim->progress <= 0.0f && anim->isPlaying) {
                                    anim->isPlaying = false;
                                    anim->progress = 0.0f;
                                    shape->currentAnimation = nullptr;
                                }
                            }

                            anim->persistentPositionOffset = ImVec2(0, 0);
                            anim->persistentSizeOffset = ImVec2(0, 0);
                            anim->persistentRotationOffset = 0.0f;
                            break;
                        case ButtonAnimation::AnimationBehavior::PlayWhileHoldingAndStay:
                            if (shape->isHeld) {
                                anim->progress = std::min(1.0f, anim->progress + deltaTime * std::fabs(anim->speed));
                            }
                            else {
                                if (anim->isPlaying) {
                                    anim->isPlaying = false;
                                    float t = anim->progress;
                                    if (anim->interpolationMethod == ButtonAnimation::InterpolationMethod::EaseInOut) t = t * t * (3.0f - 2.0f * t);

                                    anim->persistentPositionOffset = Lerp(animationBasePos, anim->animationTargetPosition, t);
                                    anim->persistentSizeOffset = Lerp(animationBaseSize, anim->animationTargetSize, t);
                                    anim->persistentRotationOffset = Lerp(animationBaseRot, anim->transformRotation, t);
                                    shape->currentAnimation = nullptr;
                                }

                            }
                            break;
                        }


                        float tInterp = anim->progress;
                        if (anim->interpolationMethod == ButtonAnimation::InterpolationMethod::EaseInOut) {
                            tInterp = tInterp * tInterp * (3.0f - 2.0f * tInterp);
                        }
                        finalPosition = Lerp(animationBasePos, anim->animationTargetPosition, tInterp);
                        finalSize = Lerp(animationBaseSize, anim->animationTargetSize, tInterp);
                        finalRotation = Lerp(animationBaseRot, anim->transformRotation, tInterp);
                    }

                    else if (!shape_effectively_locked)
                    {
                        bool appliedPersistentState = false;
                        for (auto& anim : shape->onClickAnimations) {
                            bool toggleIsOn = (anim.behavior == ButtonAnimation::AnimationBehavior::Toggle && anim.toggleState);

                            bool stayedAtEnd = (anim.behavior == ButtonAnimation::AnimationBehavior::PlayOnceAndStay && !anim.isPlaying && anim.progress >= 1.0f);

                            bool heldStayed = (anim.behavior == ButtonAnimation::AnimationBehavior::PlayWhileHoldingAndStay && !anim.isPlaying && anim.progress > 0.0f);

                            if (toggleIsOn || stayedAtEnd || heldStayed) {

                                finalPosition = anim.persistentPositionOffset;
                                finalSize = anim.persistentSizeOffset;
                                finalRotation = anim.persistentRotationOffset;
                                appliedPersistentState = true;
                                break;
                            }
                        }

                        if (!appliedPersistentState) {
                            finalPosition = layoutResultPosition;
                            finalSize = layoutResultSize;
                            finalRotation = layoutResultRotation;
                        }
                    }



                    shape->position = finalPosition;
                    shape->size = finalSize;
                    shape->rotation = finalRotation;


                    shape->size.x = std::max(shape->minSize.x, std::min(shape->size.x, shape->maxSize.x));
                    shape->size.y = std::max(shape->minSize.y, std::min(shape->size.y, shape->maxSize.y));
                    shape->size.x = std::max(0.0f, shape->size.x);
                    shape->size.y = std::max(0.0f, shape->size.y);



                    if (!shape_effectively_locked && shape->isLayoutContainer && shape->layoutManager != nullptr)
                    {
                        try {

                            shape->layoutManager->doLayout(*shape, shape->size);
                        }
                        catch (const std::exception& e) {
                            std::cerr << "      EXCEPTION during doLayout for " << shape->name << ": " << e.what() << std::endl;
                        }
                        catch (...) {
                            std::cerr << "      UNKNOWN EXCEPTION during doLayout for " << shape->name << std::endl;
                        }
                    }




                    if (!shape->children.empty()) {



                        for (ShapeItem* child : shape->children) {
                            if (child) {

                                processShapeRecursive(child, shape->size, shape);
                            }
                            else {
                                std::cerr << "      WARNING: Found null child pointer in children of ID: " << shape->id << std::endl;
                            }
                        }
                    }




                    if (parentShape != nullptr)
                    {
                        ImVec2 parentFinalPos = parentShape->position;
                        float parentFinalRot = parentShape->rotation;

                        ImVec2 localOffset = shape->originalPosition;
                        ImVec2 rotatedLocalOffset = RotateP(localOffset, ImVec2(0.0f, 0.0f), parentFinalRot);


                        shape->position = parentFinalPos + rotatedLocalOffset;

                        shape->rotation = parentFinalRot + shape->baseRotation;
                    }



                };


            for (ShapeItem* rootShape : rootShapes) {

                processShapeRecursive(rootShape, containerSizeForShapeKeys, nullptr);
            }
        }
    }


    inline ImVec2 ComputeChainOffset(const ShapeItem& shape) {
        const ChainAnimation& chain = shape.chainAnimation;
        ImVec2 offset(0, 0);
        if (!chain.isPlaying) {
            return offset;
        }
        if (!chain.reverseMode) {

            for (int i = 0; i <= chain.currentStep; i++) {
                const ButtonAnimation& anim = chain.steps[i].animation;
                float t = (i < chain.currentStep) ? 1.0f : anim.progress;
                offset.x += Lerp(0.0f, anim.animationTargetPosition.x - shape.basePosition.x, t);
                offset.y += Lerp(0.0f, anim.animationTargetPosition.y - shape.basePosition.y, t);
            }
        }
        else {

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

                    if (shape.parent != nullptr)
                    {
                        if (chain.isPlaying && !chain.steps.empty())
                        {
                            ButtonAnimation& anim = chain.steps[chain.currentStep].animation;
                            if (!chain.reverseMode) {
                                anim.progress += deltaTime * std::fabs(anim.speed);
                                if (anim.progress >= 1.0f) {
                                    anim.progress = 1.0f;
                                    if (chain.steps[chain.currentStep].onStepComplete) chain.steps[chain.currentStep].onStepComplete();
                                    if (chain.currentStep < (int)chain.steps.size() - 1) {
                                        chain.currentStep++;
                                        chain.steps[chain.currentStep].animation.progress = 0.0f;
                                    }
                                    else {
                                        chain.isPlaying = false;
                                        chain.toggled = true;
                                    }
                                }
                            }
                            else {
                                anim.progress -= deltaTime * std::fabs(anim.speed);
                                if (anim.progress <= 0.0f) {
                                    anim.progress = 0.0f;
                                    if (chain.currentStep > 0) {
                                        chain.currentStep--;
                                        chain.steps[chain.currentStep].animation.progress = 1.0f;
                                    }
                                    else {
                                        chain.isPlaying = false;
                                        chain.toggled = false;
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
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
    }

    static int GetUniqueShapeID() {
        static int nextShapeID = 1000;
        static std::unordered_set<int> usedIDs;

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

        const char* currentWindowName = ImGui::GetCurrentWindow()->Name;
        std::string windowNameStr = currentWindowName;


        if (g_windowsMap.empty())
            return;


        bool didHideGlass = false;
        if (shouldCaptureScene)
        {
            for (auto& [oWindowName, windowData] : g_windowsMap)
            {
                for (auto& layer : windowData.layers)
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
                continue;


            std::stable_sort(windowData.layers.begin(), windowData.layers.end(), CompareLayersByZOrder);


            for (auto& layer : windowData.layers)
            {

                if (!layer.visible)
                    continue;


                std::stable_sort(layer.shapes.begin(), layer.shapes.end(), CompareShapesByZOrder);


                for (auto& shape : layer.shapes)
                {






                    if (shape.ownerWindow == oWindowName && shape.visible)
                    {



                        DrawShape(dl, shape, ImGui::GetWindowPos());
                    }
                }
            }
        }


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

            else if (ch == '\r') {

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






    inline std::string GenerateButtonAnimationCode(const ButtonAnimation& anim)
    {
        std::stringstream ss;
        ss << "ShapeBuilder().createAnimation()\n";
        ss << "    .setName(\"" << anim.name << "\")\n";
        if (anim.duration != 0.5f) ss << "    .setDuration(" << anim.duration << "f)\n";
        if (anim.speed != 1.0f) ss << "    .setSpeed(" << anim.speed << "f)\n";
        if (anim.animationTargetPosition != ImVec2(0, 0)) ss << "    .setAnimationTargetPosition(ImVec2(" << anim.animationTargetPosition.x << "f, " << anim.animationTargetPosition.y << "f))\n";
        if (anim.animationTargetSize != ImVec2(0, 0)) ss << "    .setAnimationTargetSize(ImVec2(" << anim.animationTargetSize.x << "f, " << anim.animationTargetSize.y << "f))\n";
        if (std::fabs(anim.transformRotation) > 1e-4f) ss << "    .setTransformRotation(" << anim.transformRotation << "f)\n";
        if (anim.repeatCount != 1) ss << "    .setRepeatCount(" << anim.repeatCount << ")\n";
        if (anim.playbackOrder != PlaybackOrder::Sirali) {
            ss << "    .setPlaybackOrder(PlaybackOrder::HemenArkasina)\n";
        }
        if (anim.interpolationMethod != ButtonAnimation::InterpolationMethod::Linear) {
            ss << "    .setInterpolationMethod(ButtonAnimation::InterpolationMethod::EaseInOut)\n";
        }
        if (anim.triggerMode != ButtonAnimation::TriggerMode::OnClick) {
            ss << "    .setTriggerMode(ButtonAnimation::TriggerMode::OnHover)\n";
        }
        if (anim.behavior != ButtonAnimation::AnimationBehavior::PlayOnceAndStay) {
            ss << "    .setBehavior(ButtonAnimation::AnimationBehavior::";
            switch (anim.behavior) {
            case ButtonAnimation::AnimationBehavior::PlayOnceAndReverse: ss << "PlayOnceAndReverse"; break;
            case ButtonAnimation::AnimationBehavior::Toggle: ss << "Toggle"; break;
            case ButtonAnimation::AnimationBehavior::PlayWhileHoldingAndReverseOnRelease: ss << "PlayWhileHoldingAndReverseOnRelease"; break;
            case ButtonAnimation::AnimationBehavior::PlayWhileHoldingAndStay: ss << "PlayWhileHoldingAndStay"; break;
            default: break;
            }
            ss << ")\n";
        }
        ss << "    .build()";
        return ss.str();
    }


    inline std::string GenerateShapeKeyCode(const ShapeKey& key)
    {
        std::stringstream ss;
        ss << "ShapeBuilder::createShapeKey()\n";
        ss << "    .setName(\"" << key.name << "\")\n";
        if (key.type != ShapeKeyType::SizeX) {
            ss << "    .setType(ShapeKeyType::";
            switch (key.type) {
            case ShapeKeyType::SizeY: ss << "SizeY"; break;
            case ShapeKeyType::PositionX: ss << "PositionX"; break;
            case ShapeKeyType::PositionY: ss << "PositionY"; break;
            case ShapeKeyType::Rotation: ss << "Rotation"; break;
            default: break;
            }
            ss << ")\n";
        }
        ss << "    .setStartWindowSize(ImVec2(" << key.startWindowSize.x << "f, " << key.startWindowSize.y << "f))\n";
        ss << "    .setEndWindowSize(ImVec2(" << key.endWindowSize.x << "f, " << key.endWindowSize.y << "f))\n";

        if (key.type == ShapeKeyType::Rotation) {
            if (std::fabs(key.targetRotation) > 1e-4f)
                ss << "    .setTargetRotation(" << key.targetRotation << "f)\n";
            if (std::fabs(key.rotationOffset) > 1e-4f)
                ss << "    .setRotationOffset(" << key.rotationOffset << "f)\n";
        }
        else {
            if (key.targetValue != ImVec2(0, 0))
                ss << "    .setTargetValue(ImVec2(" << key.targetValue.x << "f, " << key.targetValue.y << "f))\n";
            if (key.offset != ImVec2(0, 0))
                ss << "    .setOffset(ImVec2(" << key.offset.x << "f, " << key.offset.y << "f))\n";
        }
        ss << "    .build()";
        return ss.str();
    }


    inline std::string GenerateSingleShapeCode(const DesignManager::ShapeItem& shape)
    {
        using namespace DesignManager;
        std::string shapeVar = SanitizeVariableName(shape.name);
        std::stringstream code;
        int uniqueID = shape.id;

        auto writeColorLine = [&](const std::string& methodName, const ImVec4& color) {
            code << "        ." << methodName << "(ImVec4("
                << color.x << "f, " << color.y << "f, " << color.z << "f, " << color.w << "f))\n";
            };

        code << "    auto " << shapeVar << " = ShapeBuilder()\n";
        code << "        .setId(" << uniqueID << ")\n";
        code << "        .setName(\"" << shape.name << "\")\n";
        if (!shape.ownerWindow.empty()) code << "        .setOwnerWindow(\"" << shape.ownerWindow << "\")\n";
        if (shape.groupId != 0) code << "        .setGroupId(" << shape.groupId << ")\n";


        code << "        .setBasePosition(ImVec2(" << shape.basePosition.x << "f, " << shape.basePosition.y << "f))\n";
        code << "        .setBaseSize(ImVec2(" << shape.baseSize.x << "f, " << shape.baseSize.y << "f))\n";
        if (std::fabs(shape.baseRotation) > 1e-4f) code << "        .setBaseRotation(" << shape.baseRotation << "f)\n";

        if (shape.position != shape.basePosition) code << "        .setPosition(ImVec2(" << shape.position.x << "f, " << shape.position.y << "f))\n";
        if (shape.size != shape.baseSize) code << "        .setSize(ImVec2(" << shape.size.x << "f, " << shape.size.y << "f))\n";
        if (std::fabs(shape.rotation) > 1e-4f && std::fabs(shape.rotation - shape.baseRotation) > 1e-4f) code << "        .setRotation(" << shape.rotation << "f)\n";

        if (shape.anchorMode != ShapeItem::AnchorMode::None) {
            code << "        .setAnchorMode(DesignManager::ShapeItem::AnchorMode::";
            switch (shape.anchorMode) {
            case ShapeItem::AnchorMode::TopLeft: code << "TopLeft"; break;
            case ShapeItem::AnchorMode::Top: code << "Top"; break;
            case ShapeItem::AnchorMode::TopRight: code << "TopRight"; break;
            case ShapeItem::AnchorMode::Left: code << "Left"; break;
            case ShapeItem::AnchorMode::Center: code << "Center"; break;
            case ShapeItem::AnchorMode::Right: code << "Right"; break;
            case ShapeItem::AnchorMode::BottomLeft: code << "BottomLeft"; break;
            case ShapeItem::AnchorMode::Bottom: code << "Bottom"; break;
            case ShapeItem::AnchorMode::BottomRight: code << "BottomRight"; break;
            default: code << "None"; break;
            }
            code << ")\n";

            if (shape.anchorMargin != ImVec2(0, 0)) code << "        .setAnchorMargin(ImVec2(" << shape.anchorMargin.x << "f, " << shape.anchorMargin.y << "f))\n";
        }
        if (shape.anchorMode == ShapeItem::AnchorMode::None && shape.usePercentagePos) {
            code << "        .setUsePercentagePos(true)\n";

            if (shape.percentagePos != ImVec2(0, 0)) code << "        .setPercentagePos(ImVec2(" << shape.percentagePos.x << "f, " << shape.percentagePos.y << "f))\n";
        }

        if (shape.usePercentageSize) {
            code << "        .setUsePercentageSize(true)\n";

            if (shape.percentageSize != ImVec2(10, 10)) code << "        .setPercentageSize(ImVec2(" << shape.percentageSize.x << "f, " << shape.percentageSize.y << "f))\n";
        }


        if (shape.minSize != ImVec2(0, 0)) code << "        .setMinSize(ImVec2(" << shape.minSize.x << "f, " << shape.minSize.y << "f))\n";
        if (shape.maxSize != ImVec2(99999, 99999)) code << "        .setMaxSize(ImVec2(" << shape.maxSize.x << "f, " << shape.maxSize.y << "f))\n";

        if (shape.cornerRadius != 10.0f) code << "        .setCornerRadius(" << shape.cornerRadius << "f)\n";
        if (shape.borderThickness != 2.0f) code << "        .setBorderThickness(" << shape.borderThickness << "f)\n";
        if (shape.fillColor != ImVec4(0.932f, 0.932f, 0.932f, 1)) writeColorLine("setFillColor", shape.fillColor);
        if (shape.borderColor != ImVec4(0, 0, 0, 0.8f)) writeColorLine("setBorderColor", shape.borderColor);
        if (shape.shadowColor != ImVec4(0, 0, 0, 0.2f)) writeColorLine("setShadowColor", shape.shadowColor);
        if (shape.shadowSpread != ImVec4(2, 2, 2, 2)) code << "        .setShadowSpread(ImVec4(" << shape.shadowSpread.x << "f, " << shape.shadowSpread.y << "f, " << shape.shadowSpread.z << "f, " << shape.shadowSpread.w << "f))\n";
        if (shape.shadowOffset != ImVec2(2, 2)) code << "        .setShadowOffset(ImVec2(" << shape.shadowOffset.x << "f, " << shape.shadowOffset.y << "f))\n";
        if (!shape.shadowUseCornerRadius) code << "        .setShadowUseCornerRadius(false)\n";
        if (std::fabs(shape.shadowRotation) > 1e-4f) code << "        .setShadowRotation(" << shape.shadowRotation << "f)\n";
        if (shape.blurAmount != 0.0f) code << "        .setBlurAmount(" << shape.blurAmount << "f)\n";
        if (!shape.visible) code << "        .setVisible(false)\n";
        if (shape.locked) code << "        .setLocked(true)\n";

        if (shape.useGradient) {
            code << "        .setUseGradient(true)\n";
            if (shape.gradientRotation != 0.0f) code << "        .setGradientRotation(" << shape.gradientRotation << "f)\n";
            if (shape.gradientInterpolation != ShapeItem::GradientInterpolation::Linear) {
                code << "        .setGradientInterpolation(DesignManager::ShapeItem::GradientInterpolation::";
                switch (shape.gradientInterpolation) {
                case ShapeItem::GradientInterpolation::Ease: code << "Ease"; break;
                case ShapeItem::GradientInterpolation::Constant: code << "Constant"; break;
                case ShapeItem::GradientInterpolation::Cardinal: code << "Cardinal"; break;
                case ShapeItem::GradientInterpolation::BSpline: code << "BSpline"; break;
                default: code << "Linear"; break;
                }
                code << ")\n";
            }
            if (!shape.colorRamp.empty() && !(shape.colorRamp.size() == 2 && shape.colorRamp[0].first == 0.0f && shape.colorRamp[1].first == 1.0f && shape.colorRamp[0].second == ImVec4(1.0f, 1.0f, 1.0f, 1.0f) && shape.colorRamp[1].second == ImVec4(0.5f, 0.5f, 0.5f, 1.0f))) {
                code << "        .setColorRamp({\n";
                for (const auto& ramp : shape.colorRamp) { code << "            {" << ramp.first << "f, ImVec4(" << ramp.second.x << "f, " << ramp.second.y << "f, " << ramp.second.z << "f, " << ramp.second.w << "f)},\n"; }
                code << "        })\n";
            }
        }

        if (shape.useGlass) {
            code << "        .setUseGlass(true)\n";
            if (shape.glassBlur != 10.0f) code << "        .setGlassBlur(" << shape.glassBlur << "f)\n";
            if (shape.glassAlpha != 0.7f) code << "        .setGlassAlpha(" << shape.glassAlpha << "f)\n";
            if (shape.glassColor != ImVec4(1, 1, 1, 0.3f)) writeColorLine("setGlassColor", shape.glassColor);
        }

        if (shape.zOrder != 0) code << "        .setZOrder(" << shape.zOrder << ")\n";

        if (shape.isChildWindow) {
            code << "        .setIsChildWindow(true)\n";
            if (shape.childWindowSync) code << "        .setChildWindowSync(true)\n";
            if (shape.toggleChildWindow) code << "        .setToggleChildWindow(true)\n";
            if (shape.childWindowGroupId != -1) code << "        .setChildWindowGroupId(" << shape.childWindowGroupId << ")\n";
            if (shape.targetShapeID != 0) code << "        .setTargetShapeID(" << shape.targetShapeID << ")\n";
        }
        else if (shape.isImGuiContainer) {
            code << "        .setIsImGuiContainer(true)\n";

            code << "        // .setRenderImGuiContent(...) // Set callback manually\n";
        }

        if (shape.isButton) {
            code << "        .setIsButton(true)\n";
            if (shape.buttonBehavior != ShapeItem::ButtonBehavior::SingleClick) {
                code << "        .setButtonBehavior(DesignManager::ShapeItem::ButtonBehavior::";
                switch (shape.buttonBehavior) {
                case ShapeItem::ButtonBehavior::Toggle: code << "Toggle"; break;
                case ShapeItem::ButtonBehavior::Hold:   code << "Hold"; break;
                default: break;
                }
                code << ")\n";
            }
            if (shape.useOnClick) { code << "        .setUseOnClick(true)\n"; }
            if (shape.hoverColor != ImVec4(0.8f, 0.8f, 0.8f, 1.0f)) writeColorLine("setHoverColor", shape.hoverColor);
            if (shape.clickedColor != ImVec4(0.6f, 0.6f, 0.6f, 1.0f)) writeColorLine("setClickedColor", shape.clickedColor);
        }

        if (shape.hasText && !shape.isImGuiContainer && !shape.isChildWindow) {
            code << "        .setHasText(true)\n";
            if (!shape.text.empty()) code << "        .setText(\"" << escapeNewlines(shape.text) << "\")\n";
            if (shape.textColor != ImVec4(0, 0, 0, 1)) writeColorLine("setTextColor", shape.textColor);
            if (shape.textSize != 16.0f) code << "        .setTextSize(" << shape.textSize << "f)\n";
            if (shape.textFont != 0) code << "        .setTextFont(" << shape.textFont << ")\n";
            if (shape.textPosition != ImVec2(0, 0)) code << "        .setTextPosition(ImVec2(" << shape.textPosition.x << "f, " << shape.textPosition.y << "f))\n";
            if (std::fabs(shape.textRotation) > 1e-4f) code << "        .setTextRotation(" << shape.textRotation << "f)\n";
            if (shape.textAlignment != 0) code << "        .setTextAlignment(" << shape.textAlignment << ")\n";
            if (shape.dynamicTextSize) code << "        .setDynamicTextSize(true)\n";
        }

        if (shape.updateAnimBaseOnResize) code << "        .setUpdateAnimBaseOnResize(true)\n";
        if (shape.hasEmbeddedImage) {
            code << "        .setHasEmbeddedImage(true)\n";
            if (shape.embeddedImageIndex >= 0) code << "        .setEmbeddedImageIndex(" << shape.embeddedImageIndex << ")\n";
        }
        if (shape.allowItemOverlap) code << "        .setAllowItemOverlap(true)\n";
        if (shape.forceOverlap) code << "        .setForceOverlap(true)\n";
        if (!shape.blockUnderlying) code << "        .setBlockUnderlying(false)\n";
        if (shape.type != ShapeType::Rectangle) code << "        .setType(ShapeType::Circle)\n";

        if (!shape.onClickAnimations.empty()) {
            code << "        // Add OnClick Animations manually using .addOnClickAnimation(AnimationBuilder()...build())\n";
        }
        if (!shape.shapeKeys.empty()) {
            code << "        // Add Shape Keys manually using .addShapeKey(ShapeBuilder::createShapeKey()...build())\n";
        }
        if (!shape.eventHandlers.empty()) {
            code << "        // Add Event Handlers manually using .addEventHandler(...)\n";
        }

        code << "        .build();\n";

        return code.str();
    }




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



    inline std::set<int> GetAllShapeIDs() {
        std::set<int> ids;

        if (!g_windowsMap.empty()) {
            for (const auto& pair : g_windowsMap) {
                const auto& winData = pair.second;
                for (const auto& layer : winData.layers) {
                    for (const auto& shape : layer.shapes) {
                        ids.insert(shape.id);
                    }
                }
            }
        }
        return ids;
    }

    inline ShapeItem* FindShapeByID_Internal(int id) {

        if (!g_windowsMap.empty()) {
            for (auto& pair : g_windowsMap) {
                auto& winData = pair.second;
                for (auto& layer : winData.layers) {
                    for (auto& shape : layer.shapes) {
                        if (shape.id == id) {
                            return &shape;
                        }

                    }
                }
            }
        }
        return nullptr;
    }



    inline std::string GenerateComponentDefinitionCode(const std::string& componentName, const ComponentDefinition& compDef)
    {
        using namespace DesignManager;
        std::string safeCompName = SanitizeVariableName(componentName);
        std::stringstream code;


        code << "inline std::vector<DesignManager::ShapeItem> Create" << safeCompName << "Instance(\n";
        code << "    int instanceRootId, // Placeholder ID for the root instance concept\n";
        code << "    const std::string& instanceRootName, // Base name for generated shapes\n";
        code << "    const ImVec2& positionOffset, // Where to place the component instance\n";
        code << "    const std::string& ownerWindow, // Window the instance belongs to\n";
        code << "    int& nextAvailableId) // Pass ID counter by reference to ensure uniqueness across calls\n";
        code << "{\n";


        code << "    std::vector<DesignManager::ShapeItem> createdShapes;\n";
        code << "    std::map<int, int> originalIdToNewId;\n";
        code << "    std::map<int, int> newIdToOriginalParentId;\n";

        code << "    std::vector<DesignManager::ShapeItem> tempStorage;\n";
        code << "    std::set<int> existingInstanceIDs; // Keep track of IDs used *within this instance*\n";
        code << "    tempStorage.reserve(" << compDef.shapeTemplates.size() << ");\n\n";

        code << "    // --- First Pass: Create Shapes (Assigning New Unique IDs) ---\n";
        for (const auto& shapeTemplate : compDef.shapeTemplates) {


            std::string shapeCode = GenerateSingleShapeCode(shapeTemplate.item);

            std::stringstream finalShapeCodeSS;
            std::stringstream generatedSS(shapeCode);
            std::string line;




            code << "    int newId = nextAvailableId;\n";



            code << "    nextAvailableId++;\n";


            code << "    originalIdToNewId[" << shapeTemplate.originalId << "] = newId;\n";
            code << "    newIdToOriginalParentId[newId] = " << shapeTemplate.originalParentId << ";\n\n";



            std::string shapeVarName = SanitizeVariableName(shapeTemplate.item.name) + "_" + std::to_string(shapeTemplate.originalId);
            finalShapeCodeSS << "ShapeBuilder()\n";

            while (std::getline(generatedSS, line)) {

                if (line.find(".setId(") == std::string::npos &&
                    line.find(".setName(") == std::string::npos &&
                    line.find(".setPosition(") == std::string::npos &&
                    line.find(".setBasePosition(") == std::string::npos &&
                    line.find(".setOwnerWindow(") == std::string::npos &&
                    line.find(".setComponentSource(") == std::string::npos &&
                    line.find("auto ") != 0 &&
                    line.find(".build();") == std::string::npos)
                {
                    finalShapeCodeSS << line << "\n";
                }
            }


            finalShapeCodeSS << "        .setId(newId)\n";
            finalShapeCodeSS << "        .setName(instanceRootName + \"_" << SanitizeVariableName(shapeTemplate.item.name) << "\") // Combine instance name and original name\n";
            finalShapeCodeSS << "        .setPosition(ImVec2(positionOffset.x + " << shapeTemplate.item.position.x << "f, positionOffset.y + " << shapeTemplate.item.position.y << "f)) // Apply offset to relative position\n";
            finalShapeCodeSS << "        .setBasePosition(ImVec2(positionOffset.x + " << shapeTemplate.item.basePosition.x << "f, positionOffset.y + " << shapeTemplate.item.basePosition.y << "f)) // Apply offset to relative base position\n";
            finalShapeCodeSS << "        .setOwnerWindow(ownerWindow)\n";
            finalShapeCodeSS << "        .setComponentSource(\"\") // Clear component source for the instance\n";
            finalShapeCodeSS << "        .build()";


            code << "    tempStorage.push_back(" << finalShapeCodeSS.str() << ");\n\n";
        }
        code << "\n";

        code << "    // --- Second Pass: Link Parents ---\n";
        code << "    std::map<int, DesignManager::ShapeItem*> newIdToShapePtrMap;\n";
        code << "    for(auto& shape : tempStorage) { newIdToShapePtrMap[shape.id] = &shape; }\n\n";
        code << "    for(auto& shape : tempStorage) {\n";
        code << "        int currentNewId = shape.id;\n";
        code << "        if (newIdToOriginalParentId.count(currentNewId)) {\n";
        code << "            int originalParentId = newIdToOriginalParentId.at(currentNewId);\n";
        code << "            if (originalParentId != -1 && originalIdToNewId.count(originalParentId)) { // Check if parent was part of the component\n";
        code << "                int newParentId = originalIdToNewId.at(originalParentId);\n";
        code << "                if (newIdToShapePtrMap.count(newParentId)) { // Check if the new parent pointer exists\n";
        code << "                    shape.parent = newIdToShapePtrMap.at(newParentId); // Assign parent pointer\n";
        code << "                    newIdToShapePtrMap.at(newParentId)->children.push_back(&shape); // Add to parent's children\n";
        code << "                }\n";
        code << "            }\n";
        code << "        }\n";
        code << "    }\n\n";

        code << "    createdShapes = std::move(tempStorage);\n";
        code << "    return createdShapes;\n";
        code << "}\n\n";

        return code.str();
    }



    inline std::string GenerateAllComponentDefinitionsCode() {



        const auto& currentComponentDefinitions = DesignManager::g_componentDefinitions;

        std::stringstream allCompCode;
        allCompCode << "// --- Generated Component Definitions ---\n\n";
        allCompCode << "#pragma once\n\n";
        allCompCode << "#include \"design_manager.h\" // Include the base header where ShapeItem etc. are defined\n";
        allCompCode << "#include <vector>\n";
        allCompCode << "#include <map>\n";
        allCompCode << "#include <set>\n\n";
        allCompCode << "namespace DesignManager {\n\n";


        for (const auto& pair : currentComponentDefinitions) {
            const std::string& name = pair.first;
            const ComponentDefinition& compDef = pair.second;

            allCompCode << GenerateComponentDefinitionCode(name, compDef);
        }

        allCompCode << "} // namespace DesignManager\n";
        allCompCode << "// --- End Generated Component Definitions ---\n";
        return allCompCode.str();
    }

    inline std::string GenerateCodeForWindow(const std::string& windowName)
    {
        using namespace DesignManager;


        if (g_windowsMap.find(windowName) == g_windowsMap.end()) {
            std::cerr << "Error: Window '" << windowName << "' not found for code generation." << std::endl;
            return "// Error: Window '" + windowName + "' not found.\n";
        }
        const auto& winData = g_windowsMap.at(windowName);

        std::string safeWindowName = SanitizeVariableName(windowName);
        std::stringstream code;



        code << "// --- Automatically generated code for window: " << windowName << " ---\n";
        code << "// --- Paste this code block into your initialization function --- \n\n";



        if (!g_componentDefinitions.empty())
        {
            code << "    // --- Define Components ---\n";


            for (const auto& [compName, compDef] : g_componentDefinitions)
            {
                std::string safeCompName = SanitizeVariableName(compName);
                code << "    {\n";
                code << "        DesignManager::ComponentDefinition " << safeCompName << "_def;\n";
                code << "        " << safeCompName << "_def.name = \"" << compName << "\";\n";
                if (!compDef.shapeTemplates.empty()) {
                    code << "        " << safeCompName << "_def.shapeTemplates.reserve(" << compDef.shapeTemplates.size() << ");\n";
                }
                code << "\n";

                int templateIdx = 0;
                for (const auto& shapeTemplate : compDef.shapeTemplates)
                {
                    std::string templateShapeVar = safeCompName + "_template_" + std::to_string(templateIdx);
                    std::string templateShapeCode = GenerateSingleShapeCode(shapeTemplate.item);
                    size_t assignPos = templateShapeCode.find("= ShapeBuilder()");
                    size_t buildPos = templateShapeCode.rfind(".build();");

                    if (assignPos != std::string::npos && buildPos != std::string::npos) {
                        templateShapeCode = templateShapeCode.substr(assignPos + 2, buildPos - (assignPos + 2));
                    }
                    else {
                        templateShapeCode = "ShapeBuilder() /* Error parsing template code */";
                    }

                    code << "        DesignManager::ComponentShapeTemplate " << templateShapeVar << "_template;\n";
                    code << "        " << templateShapeVar << "_template.item = " << templateShapeCode << ".build();\n";
                    code << "        " << templateShapeVar << "_template.originalId = " << shapeTemplate.originalId << ";\n";
                    code << "        " << templateShapeVar << "_template.originalParentId = " << shapeTemplate.originalParentId << ";\n";
                    code << "        " << safeCompName << "_def.shapeTemplates.push_back(" << templateShapeVar << "_template);\n\n";
                    templateIdx++;
                }
                code << "        DesignManager::g_componentDefinitions[\"" << compName << "\"] = " << safeCompName << "_def;\n";
                code << "    }\n";
            }
            code << "\n";
        }
        else {
            code << "    // No components defined in the editor.\n\n";
        }






        for (const auto& layer : winData.layers)
        {
            std::string layerVar = SanitizeVariableName(layer.name);
            code << "    DesignManager::Layer " << layerVar << "(\"" << layer.name << "\");\n";
            if (layer.zOrder != 0) code << "    " << layerVar << ".zOrder = " << layer.zOrder << ";\n";
            if (!layer.visible) code << "    " << layerVar << ".visible = false;\n";
            if (layer.locked) code << "    " << layerVar << ".locked = true;\n";
            code << "\n";

            for (const auto& shape : layer.shapes)
            {

                code << GenerateSingleShapeCode(shape);

                code << "    " << layerVar << ".shapes.push_back(" << SanitizeVariableName(shape.name) << ");\n\n";
            }


            code << "    DesignManager::g_windowsMap[\"" << windowName << "\"].layers.push_back(std::move(" << layerVar << "));\n\n";
        }








        code << "    // --- Child Window Mappings ---\n";
        code << GenerateChildWindowMappingsCode();
        code << "\n";


        code << "// --- End of generated code for window: " << windowName << " ---\n";

        return code.str();
    }




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











    inline void RefreshLayerIDs()
    {
        auto& windowData = g_windowsMap[DesignManager::selectedGuiWindow];
        for (int i = 0; i < static_cast<int>(windowData.layers.size()); i++)
        {
            windowData.layers[i].id = i;
        }
    }



    inline void EnsureMainWindowExists() {
        if (g_windowsMap.find("Main") == g_windowsMap.end()) {
            g_windowsMap["Main"] = WindowData{};
            g_windowsMap["Main"].isOpen = true;
            g_windowsMap["Main"].layers.emplace_back("Layer 1");
            if (selectedGuiWindow.empty() || g_windowsMap.find(selectedGuiWindow) == g_windowsMap.end()) {
                selectedGuiWindow = "Main";
            }
            selectedLayerIndex = 0;
            selectedShapes.clear();
            lastSelectedLayerIndex = -1;
            lastSelectedShapeIndex = -1;
        }
        else {

            if (g_windowsMap.find(selectedGuiWindow) == g_windowsMap.end()) {
                selectedGuiWindow = "Main";

                selectedLayerIndex = g_windowsMap["Main"].layers.empty() ? -1 : 0;
                selectedShapes.clear();
                lastSelectedLayerIndex = -1;
                lastSelectedShapeIndex = -1;
            }
        }


        WindowData& currentWindowData = g_windowsMap[selectedGuiWindow];
        if (!currentWindowData.layers.empty()) {
            if (selectedLayerIndex < 0 || selectedLayerIndex >= currentWindowData.layers.size()) {
                selectedLayerIndex = 0;
                selectedShapes.clear();
                lastSelectedLayerIndex = -1;
                lastSelectedShapeIndex = -1;
            }
        }
        else {
            selectedLayerIndex = -1;
            selectedShapes.clear();
            lastSelectedLayerIndex = -1;
            lastSelectedShapeIndex = -1;
        }



    }


    inline int GetUniqueLayerID() {
        static int layer_id_counter = 1000;
        return layer_id_counter++;
    }


    static ShapeItem* lastClickedShape = nullptr;



    static int lastClickedLayerIndex = -1;

    inline ShapeItem* FindShapeByIdRecursiveHelper(int shapeId, ShapeItem* currentShape) {
        if (!currentShape) {
            return nullptr;
        }
        if (currentShape->id == shapeId) {
            return currentShape;
        }
        for (ShapeItem* child : currentShape->children) {
            ShapeItem* found = FindShapeByIdRecursiveHelper(shapeId, child);
            if (found) {
                return found;
            }
        }
        return nullptr;
    }


    inline ShapeItem* FindShapeByIdRecursive(int shapeId) {
        if (g_windowsMap.count(selectedGuiWindow)) {
            WindowData& currentWindowData = g_windowsMap.at(selectedGuiWindow);
            for (Layer& layer : currentWindowData.layers) {
                for (ShapeItem& shape : layer.shapes) {

                    if (shape.parent == nullptr) {
                        ShapeItem* found = FindShapeByIdRecursiveHelper(shapeId, &shape);
                        if (found) {
                            return found;
                        }
                    }
                }





            }
        }
        return nullptr;
    }


    inline void ShowUI_HierarchyPanel(WindowData& windowData, int& selectedLayerIndex, std::vector<ShapeItem*>& selectedShapes) {
        ImGui::BeginChild("HierarchyPanel", ImVec2(0, -ImGui::GetFrameHeightWithSpacing() * 2.5f));


        ImGui::TextUnformatted("Window:"); ImGui::SameLine();
        ImGui::PushItemWidth(-FLT_MIN);
        if (ImGui::BeginCombo("##SelectedImGuiWindow", selectedGuiWindow.c_str())) {
            std::vector<std::string> windowNames;
            for (auto const& [winName, winData] : g_windowsMap) {
                windowNames.push_back(winName);
            }
            std::sort(windowNames.begin(), windowNames.end());

            for (const auto& winName : windowNames) {
                bool is_selected = (selectedGuiWindow == winName);
                if (ImGui::Selectable(winName.c_str(), is_selected)) {
                    if (selectedGuiWindow != winName) {
                        selectedGuiWindow = winName;
                        EnsureMainWindowExists();

                        windowData = g_windowsMap[selectedGuiWindow];
                        selectedLayerIndex = windowData.layers.empty() ? -1 : 0;
                        selectedShapes.clear();
                        lastSelectedLayerIndex = -1;
                        lastSelectedShapeIndex = -1;
                        MarkSceneUpdated();
                    }
                }
                if (is_selected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        ImGui::PopItemWidth();
        ImGui::Separator();

        ImGui::Text("Layers & Shapes");
        ImGui::Spacing();




        static bool needsLayerSort = false;
        static int layerToSort = -1;

        for (int i = 0; i < (int)windowData.layers.size(); i++) {
            ImGui::PushID(i);
            Layer& layer = windowData.layers[i];
            bool layer_is_selected_for_ops = (selectedLayerIndex == i);

            ImGuiTreeNodeFlags layerNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap;
            if (layer_is_selected_for_ops && selectedShapes.empty()) {
                layerNodeFlags |= ImGuiTreeNodeFlags_Selected;
            }


            ImGui::BeginGroup();
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, layer.visible ? 1.0f : 0.5f);
            if (ImGui::Checkbox(("##Visible" + std::to_string(i)).c_str(), &layer.visible)) { MarkSceneUpdated(); }
            if (ImGui::IsItemHovered()) ImGui::SetTooltip("Layer Visibility");
            ImGui::PopStyleVar();
            ImGui::SameLine(0, 2);

            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, layer.locked ? 1.0f : 0.6f);
            const char* lockIcon = layer.locked ? "[L]" : "[U]";
            if (ImGui::Checkbox((lockIcon + std::string("##Locked") + std::to_string(i)).c_str(), &layer.locked)) {
                if (layer.locked) {
                    selectedShapes.erase(std::remove_if(selectedShapes.begin(), selectedShapes.end(),
                        [&](ShapeItem* s_ptr) { return FindShapeLayerIndex(s_ptr->id) == i; }), selectedShapes.end());
                    if (selectedLayerIndex == i && selectedShapes.empty()) {

                    }
                }
                MarkSceneUpdated();
            }
            if (ImGui::IsItemHovered()) ImGui::SetTooltip("Lock Layer (prevents selection/modification)");
            ImGui::PopStyleVar();

            ImGui::SameLine(0, 5);
            ImGui::PushItemWidth(50);

            int currentZ = layer.zOrder;
            if (ImGui::DragInt(("Z##LayerZ" + std::to_string(i)).c_str(), &layer.zOrder, 0.1f)) {

                needsLayerSort = true;
                layerToSort = -2;
                MarkSceneUpdated();
            }

            if (ImGui::IsItemDeactivatedAfterEdit() && currentZ != layer.zOrder) {
                if (needsLayerSort && layerToSort == -2) {
                    std::stable_sort(windowData.layers.begin(), windowData.layers.end(), [](const Layer& a, const Layer& b) {
                        return a.zOrder < b.zOrder;
                        });

                    int oldSelectedLayerId = (selectedLayerIndex >= 0 && selectedLayerIndex < windowData.layers.size()) ? windowData.layers[selectedLayerIndex].id : -1;
                    int oldLastSelectedLayerId = (lastSelectedLayerIndex >= 0 && lastSelectedLayerIndex < windowData.layers.size()) ? windowData.layers[lastSelectedLayerIndex].id : -1;


                    selectedLayerIndex = -1;
                    lastSelectedLayerIndex = -1;
                    for (int newIdx = 0; newIdx < windowData.layers.size(); ++newIdx) {
                        if (windowData.layers[newIdx].id == oldSelectedLayerId) selectedLayerIndex = newIdx;
                        if (windowData.layers[newIdx].id == oldLastSelectedLayerId) lastSelectedLayerIndex = newIdx;
                    }
                    if (selectedLayerIndex == -1 && !windowData.layers.empty()) selectedLayerIndex = 0;


                    needsLayerSort = false;
                    layerToSort = -1;
                    MarkSceneUpdated();
                }
            }
            ImGui::PopItemWidth();
            ImGui::SameLine();
            ImGui::EndGroup();



            bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)layer.id, layerNodeFlags, "%s", layer.name.c_str());


            if (!layer.locked && ImGui::BeginDragDropTarget())
            {

                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SHAPE_ITEM"))
                {
                    IM_ASSERT(payload->DataSize == sizeof(ShapeItem*));
                    ShapeItem* dragged_shape_ptr = *(ShapeItem**)payload->Data;


                    int originalLayerIndex = -1;
                    Layer* originalLayerPtr = nullptr;
                    for (size_t layIdx = 0; layIdx < windowData.layers.size(); ++layIdx) {
                        auto& shapes = windowData.layers[layIdx].shapes;
                        auto it = std::find_if(shapes.begin(), shapes.end(), [id = dragged_shape_ptr->id](const ShapeItem& s) { return s.id == id; });
                        if (it != shapes.end()) {
                            originalLayerIndex = layIdx;
                            originalLayerPtr = &windowData.layers[layIdx];
                            break;
                        }
                    }


                    if (dragged_shape_ptr && !dragged_shape_ptr->locked && originalLayerPtr != nullptr && originalLayerIndex != i)
                    {

                        ShapeItem movedShape = *dragged_shape_ptr;



                        if (dragged_shape_ptr->parent) {

                            ShapeItem* original_parent_ptr = FindShapeByIdRecursive(dragged_shape_ptr->parent->id);
                            if (original_parent_ptr) {
                                auto& childrenVec = original_parent_ptr->children;


                                childrenVec.erase(std::remove(childrenVec.begin(), childrenVec.end(), dragged_shape_ptr), childrenVec.end());
                            }
                        }

                        std::vector<ShapeItem*> children_copy = dragged_shape_ptr->children;
                        for (auto* child_ptr : children_copy) {
                            if (child_ptr) {

                                RemoveParentKeepTransform(child_ptr);
                            }
                        }


                        movedShape.parent = nullptr;
                        movedShape.children.clear();
                        movedShape.ownerWindow = selectedGuiWindow;



                        originalLayerPtr->shapes.erase(std::remove_if(originalLayerPtr->shapes.begin(), originalLayerPtr->shapes.end(),
                            [id = movedShape.id](const ShapeItem& s) { return s.id == id; }),
                            originalLayerPtr->shapes.end());


                        layer.shapes.push_back(movedShape);
                        ShapeItem* newShapePtr = &layer.shapes.back();


                        selectedShapes.clear();
                        selectedShapes.push_back(newShapePtr);
                        selectedLayerIndex = i;
                        lastSelectedShapeIndex = layer.shapes.size() - 1;
                        lastSelectedLayerIndex = i;
                        lastClickedShape = newShapePtr;
                        lastClickedLayerIndex = i;


                        MarkSceneUpdated();
                    }
                }

                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DESIGNER_COMPONENT")) {
                    IM_ASSERT(payload->DataSize > 0);
                    const char* componentNamePayload = (const char*)payload->Data;
                    std::string componentNameToInstantiate = componentNamePayload;

                    if (g_componentDefinitions.count(componentNameToInstantiate)) {
                        const ComponentDefinition& compDef = g_componentDefinitions.at(componentNameToInstantiate);


                        ImVec2 dropPos = ImGui::GetMousePos();
                        ImVec2 targetWindowPos = ImVec2(0, 0);
                        ImVec2 targetWindowScroll = ImVec2(0, 0);
                        ImGuiWindow* targetImGuiWindow = ImGui::FindWindowByName(selectedGuiWindow.c_str());
                        if (targetImGuiWindow) {
                            targetWindowPos = targetImGuiWindow->Pos;
                            targetWindowScroll = targetImGuiWindow->Scroll;
                        }
                        ImVec2 instantiationPos = dropPos - targetWindowPos + targetWindowScroll;

                        std::vector<ShapeItem*> newInstanceShapesPtrs;
                        std::map<int, int> originalToNewIdMap;
                        std::map<int, ShapeItem*> newIdToPtrMap;
                        std::map<int, int> newIdToOriginalParentId;

                        std::set<std::string> existingNamesInWindow;
                        for (const auto& l : windowData.layers) {
                            for (const auto& shp : l.shapes) existingNamesInWindow.insert(shp.name);
                        }


                        for (const auto& shapeTemplate : compDef.shapeTemplates) {
                            ShapeItem newShape = shapeTemplate.item;
                            int newId = GetUniqueShapeID();
                            newShape.id = newId;

                            newShape.position = instantiationPos + shapeTemplate.item.position;
                            newShape.basePosition = newShape.position;
                            newShape.ownerWindow = selectedGuiWindow;
                            newShape.parent = nullptr;
                            newShape.children.clear();
                            newShape.visible = true;
                            newShape.locked = false;
                            newShape.isPressed = false;
                            newShape.isHeld = false;
                            newShape.isAnimating = false;
                            newShape.currentAnimation = nullptr;


                            std::string baseName = componentNameToInstantiate + "_" + shapeTemplate.item.name;
                            std::string finalName = baseName;
                            int suffix = 1;
                            while (existingNamesInWindow.count(finalName)) { finalName = baseName + "_" + std::to_string(suffix++); }
                            newShape.name = finalName;
                            existingNamesInWindow.insert(finalName);


                            layer.shapes.push_back(newShape);
                            ShapeItem* ptr = &layer.shapes.back();


                            newInstanceShapesPtrs.push_back(ptr);
                            originalToNewIdMap[shapeTemplate.originalId] = newId;
                            newIdToPtrMap[newId] = ptr;
                            newIdToOriginalParentId[newId] = shapeTemplate.originalParentId;
                        }


                        for (ShapeItem* instanceShapePtr : newInstanceShapesPtrs) {
                            int currentNewId = instanceShapePtr->id;
                            if (newIdToOriginalParentId.count(currentNewId)) {
                                int originalParentId = newIdToOriginalParentId[currentNewId];

                                if (originalParentId != -1 && originalToNewIdMap.count(originalParentId)) {
                                    int newParentId = originalToNewIdMap[originalParentId];

                                    if (newIdToPtrMap.count(newParentId)) {
                                        ShapeItem* newParentPtr = newIdToPtrMap[newParentId];


                                        SetParent(instanceShapePtr, newParentPtr);
                                    }
                                }
                            }
                        }


                        selectedShapes = newInstanceShapesPtrs;
                        selectedLayerIndex = i;
                        lastSelectedLayerIndex = i;
                        lastSelectedShapeIndex = -1;
                        lastClickedShape = nullptr;
                        lastClickedLayerIndex = i;

                        MarkSceneUpdated();
                    }
                }
                ImGui::EndDragDropTarget();
            }


            if (ImGui::IsItemClicked(0) && !ImGui::IsItemToggledOpen() && !ImGui::GetIO().KeyShift && !ImGui::IsMouseDragging(0)) {
                selectedLayerIndex = i;
                selectedShapes.clear();
                lastSelectedLayerIndex = i;
                lastSelectedShapeIndex = -1;
                lastClickedShape = nullptr;
                lastClickedLayerIndex = i;
            }




            if (ImGui::BeginPopupContextItem(("LayerContext##" + std::to_string(layer.id)).c_str())) {
                if (ImGui::MenuItem("Rename")) {

                    ImGui::OpenPopup("RenameLayerPopup");
                }
                if (ImGui::MenuItem("Delete Layer", nullptr, false, windowData.layers.size() > 1)) {
                    std::vector<int> shapesInDeletedLayerIds;
                    for (const auto& s : layer.shapes) shapesInDeletedLayerIds.push_back(s.id);


                    selectedShapes.erase(std::remove_if(selectedShapes.begin(), selectedShapes.end(),
                        [&](ShapeItem* s_ptr) {
                            return std::find(shapesInDeletedLayerIds.begin(), shapesInDeletedLayerIds.end(), s_ptr->id) != shapesInDeletedLayerIds.end();
                        }), selectedShapes.end());


                    int deletedLayerId = layer.id;


                    windowData.layers.erase(windowData.layers.begin() + i);



                    int oldSelectedLayerIdAfterPotentialDelete = -1;
                    if (selectedLayerIndex != -1) {
                        if (selectedLayerIndex == i) {
                            oldSelectedLayerIdAfterPotentialDelete = -1;
                        }
                        else if (selectedLayerIndex > i) {

                            oldSelectedLayerIdAfterPotentialDelete = windowData.layers[selectedLayerIndex - 1].id;
                        }
                        else {
                            oldSelectedLayerIdAfterPotentialDelete = windowData.layers[selectedLayerIndex].id;
                        }
                    }


                    selectedLayerIndex = -1;
                    for (int k = 0; k < windowData.layers.size(); ++k) {
                        if (windowData.layers[k].id == oldSelectedLayerIdAfterPotentialDelete) {
                            selectedLayerIndex = k;
                            break;
                        }
                    }

                    if (oldSelectedLayerIdAfterPotentialDelete == -1 && selectedLayerIndex == -1) {
                        selectedLayerIndex = windowData.layers.empty() ? -1 : std::max(0, i - 1);
                    }



                    lastSelectedLayerIndex = selectedLayerIndex;
                    lastClickedLayerIndex = selectedLayerIndex;
                    lastSelectedShapeIndex = -1;
                    if (selectedLayerIndex == -1) selectedShapes.clear();

                    MarkSceneUpdated();
                    ImGui::CloseCurrentPopup();
                    ImGui::EndPopup();
                    ImGui::PopID();
                    node_open = false;
                    i--;
                    continue;
                }

                if (ImGui::MenuItem("Move Up", nullptr, false, i > 0)) {
                    int currentSelectedId = (selectedLayerIndex != -1) ? windowData.layers[selectedLayerIndex].id : -1;
                    std::swap(windowData.layers[i], windowData.layers[i - 1]);

                    selectedLayerIndex = -1;
                    for (int k = 0; k < windowData.layers.size(); ++k) if (windowData.layers[k].id == currentSelectedId) selectedLayerIndex = k;
                    lastSelectedLayerIndex = selectedLayerIndex;
                    lastClickedLayerIndex = selectedLayerIndex;
                    MarkSceneUpdated();
                    ImGui::CloseCurrentPopup();
                }
                if (ImGui::MenuItem("Move Down", nullptr, false, i < (int)windowData.layers.size() - 1)) {
                    int currentSelectedId = (selectedLayerIndex != -1) ? windowData.layers[selectedLayerIndex].id : -1;
                    std::swap(windowData.layers[i], windowData.layers[i + 1]);

                    selectedLayerIndex = -1;
                    for (int k = 0; k < windowData.layers.size(); ++k) if (windowData.layers[k].id == currentSelectedId) selectedLayerIndex = k;
                    lastSelectedLayerIndex = selectedLayerIndex;
                    lastClickedLayerIndex = selectedLayerIndex;
                    MarkSceneUpdated();
                    ImGui::CloseCurrentPopup();
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Add New Shape Here")) {
                    ShapeItem s;
                    std::string baseName = "New Shape";
                    std::string finalName = baseName;
                    int suffix = 0;
                    std::set<std::string> existingNames;
                    for (const auto& l : windowData.layers) for (const auto& shp : l.shapes) existingNames.insert(shp.name);
                    while (existingNames.count(finalName)) { finalName = baseName + "_" + std::to_string(suffix++); }
                    s.name = finalName;
                    s.id = GetUniqueShapeID();
                    s.ownerWindow = selectedGuiWindow;
                    s.zOrder = layer.shapes.empty() ? 0 : layer.shapes.back().zOrder + 1;
                    s.position = ImVec2(150, 150);
                    s.size = ImVec2(150, 150);
                    s.basePosition = s.position;
                    s.baseSize = s.size;
                    s.parent = nullptr;
                    s.children.clear();

                    layer.shapes.push_back(s);
                    selectedLayerIndex = i;
                    selectedShapes.clear();
                    selectedShapes.push_back(&layer.shapes.back());
                    lastSelectedLayerIndex = i;
                    lastSelectedShapeIndex = layer.shapes.size() - 1;
                    lastClickedShape = selectedShapes.back();
                    lastClickedLayerIndex = i;
                    MarkSceneUpdated();
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }


            if (ImGui::BeginPopup("RenameLayerPopup")) {
                static char renameLayerBufferContext[128];

                if (ImGui::IsWindowAppearing()) {

                    if (selectedLayerIndex == i) {
                        strncpy(renameLayerBufferContext, layer.name.c_str(), 127);
                        renameLayerBufferContext[127] = '\0';
                    }
                    else {


                        renameLayerBufferContext[0] = '\0';
                    }
                    ImGui::SetKeyboardFocusHere();
                }

                ImGui::Text("Rename Layer:");
                ImGui::Separator();
                ImGui::PushItemWidth(200);
                if (ImGui::InputText("##NewLayerNameInput", renameLayerBufferContext, IM_ARRAYSIZE(renameLayerBufferContext), ImGuiInputTextFlags_EnterReturnsTrue)) {
                    if (strlen(renameLayerBufferContext) > 0 && layer.name != renameLayerBufferContext) {
                        layer.name = renameLayerBufferContext;
                        MarkSceneUpdated();
                    }
                    ImGui::CloseCurrentPopup();
                }
                ImGui::PopItemWidth();
                ImGui::Spacing();
                if (ImGui::Button("OK##RenameLayer")) {
                    if (strlen(renameLayerBufferContext) > 0 && layer.name != renameLayerBufferContext) {
                        layer.name = renameLayerBufferContext;
                        MarkSceneUpdated();
                    }
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (ImGui::Button("Cancel##RenameLayer") || ImGui::IsKeyPressed(ImGuiKey_Escape)) {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }


            if (node_open) {


                if (ImGui::IsMouseReleased(0) && needsLayerSort && layerToSort == i) {
                    std::stable_sort(layer.shapes.begin(), layer.shapes.end(), [](const ShapeItem& a, const ShapeItem& b) {
                        return a.zOrder < b.zOrder;
                        });

                    if (!selectedShapes.empty() && lastSelectedLayerIndex == i && lastSelectedShapeIndex != -1) {
                        int currentlySelectedShapeId = selectedShapes.back()->id;
                        lastSelectedShapeIndex = -1;
                        for (int newIdx = 0; newIdx < layer.shapes.size(); ++newIdx) {
                            if (layer.shapes[newIdx].id == currentlySelectedShapeId) {
                                lastSelectedShapeIndex = newIdx;
                                break;
                            }
                        }
                    }
                    needsLayerSort = false;
                    layerToSort = -1;
                    MarkSceneUpdated();
                }



                for (int j = 0; j < (int)layer.shapes.size(); ++j) {




                    ShapeItem& current_shape = layer.shapes[j];

                    if (current_shape.parent == nullptr) {

                        DrawShapeTreeNode(&current_shape, layer, i, j, selectedLayerIndex, selectedShapes, lastClickedShape, lastClickedLayerIndex, layerToSort, needsLayerSort);
                    }
                }
                ImGui::TreePop();
            }
            ImGui::PopID();
        }


        ImGui::EndChild();

        if (ImGui::Button("Add Layer")) {
            std::string newLayerName = "Layer " + std::to_string(windowData.layers.size() + 1);

            int suffix = 1;
            std::set<std::string> existingNames;
            for (const auto& lyr : windowData.layers) existingNames.insert(lyr.name);
            while (existingNames.count(newLayerName)) { newLayerName = "Layer " + std::to_string(windowData.layers.size() + 1) + "_" + std::to_string(suffix++); }

            windowData.layers.emplace_back(newLayerName);

            int maxZ = -1;
            if (windowData.layers.size() > 1) {
                maxZ = windowData.layers[windowData.layers.size() - 2].zOrder;
            }
            windowData.layers.back().zOrder = maxZ + 1;
            windowData.layers.back().id = GetUniqueLayerID();

            selectedLayerIndex = (int)windowData.layers.size() - 1;
            selectedShapes.clear();
            lastSelectedLayerIndex = selectedLayerIndex;
            lastSelectedShapeIndex = -1;
            lastClickedLayerIndex = selectedLayerIndex;
            lastClickedShape = nullptr;
            MarkSceneUpdated();
        }

        ImGui::SameLine();

        ImGui::BeginDisabled(selectedShapes.size() < 2);
        if (ImGui::Button("Set Parent (Manual)")) {
            if (selectedShapes.size() >= 2) {
                ShapeItem* parent = selectedShapes.back();
                for (size_t k = 0; k < selectedShapes.size() - 1; ++k) {
                    ShapeItem* child = selectedShapes[k];
                    if (child && parent && child != parent && !child->locked && !parent->locked) {

                        int parentLayer = FindShapeLayerIndex(parent->id);
                        int childLayer = FindShapeLayerIndex(child->id);
                        if (parentLayer != -1 && parentLayer == childLayer) {
                            if (!IsAncestor(child, parent)) {
                                SetParent(child, parent);
                            }
                        }
                        else {

                        }
                    }
                }
                MarkSceneUpdated();

            }
        }
        ImGui::EndDisabled();


        ImGui::SameLine();
        ImGui::BeginDisabled(selectedShapes.empty() || std::all_of(selectedShapes.begin(), selectedShapes.end(), [](ShapeItem* s) { return !s || s->parent == nullptr; }));
        if (ImGui::Button("Unparent")) {
            std::vector<ShapeItem*> shapesToUnparent = selectedShapes;
            for (ShapeItem* child : shapesToUnparent) {
                if (child && !child->locked && child->parent) {
                    RemoveParent(child);
                }
            }

            MarkSceneUpdated();
        }
        ImGui::SameLine();
        if (ImGui::Button("Unparent (Keep Transform)")) {
            std::vector<ShapeItem*> shapesToUnparent = selectedShapes;
            for (ShapeItem* child : shapesToUnparent) {
                if (child && !child->locked && child->parent) {
                    RemoveParentKeepTransform(child);
                }
            }

            MarkSceneUpdated();
        }
        ImGui::EndDisabled();
    }



    inline void DrawShapeTreeNode(ShapeItem* shape, Layer& layer, int layerIndex, int shapeIndexInLayer, int& selectedLayerIndex, std::vector<ShapeItem*>& selectedShapes, ShapeItem*& lastClickedShape, int& lastClickedLayerIndex, int& layerToSort, bool& needsLayerSort) {
        if (!shape) return;
        ImGui::PushID(shape->id);


        ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap;


        bool is_selected = std::find(selectedShapes.begin(), selectedShapes.end(), shape) != selectedShapes.end();
        if (is_selected) {
            node_flags |= ImGuiTreeNodeFlags_Selected;
        }


        bool is_leaf = true;
        for (const auto& child : shape->children) {
            if (child != nullptr) {
                is_leaf = false;
                break;
            }
        }
        if (is_leaf) {
            node_flags |= ImGuiTreeNodeFlags_Leaf;
        }


        bool layer_is_locked = layer.locked;
        bool shape_effectively_locked = shape->locked || layer_is_locked;
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, shape_effectively_locked ? 0.5f : 1.0f);


        bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)shape->id, node_flags, "%s", shape->name.c_str());


        if (!shape_effectively_locked && ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {

            ImGui::SetDragDropPayload("SHAPE_ITEM", &shape, sizeof(ShapeItem*));

            ImGui::Text("Move/Parent: %s", shape->name.c_str());
            ImGui::EndDragDropSource();
        }


        if (!shape_effectively_locked && ImGui::BeginDragDropTarget()) {

            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SHAPE_ITEM")) {
                IM_ASSERT(payload->DataSize == sizeof(ShapeItem*));
                ShapeItem* dragged_shape = *(ShapeItem**)payload->Data;





                if (dragged_shape && shape && dragged_shape != shape && !IsAncestor(shape, dragged_shape) && !shape->locked && !dragged_shape->locked) {

                    int draggedShapeLayerIndex = FindShapeLayerIndex(dragged_shape->id);
                    if (draggedShapeLayerIndex != -1) {


                        SetParent(dragged_shape, shape);
                        MarkSceneUpdated();




                    }
                    else {
                        std::cerr << "Warning: Dragged shape layer not found for parenting." << std::endl;
                    }
                }
            }


            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DESIGNER_COMPONENT")) {
                IM_ASSERT(payload->DataSize > 0);
                const char* componentNamePayload = (const char*)payload->Data;
                std::string componentNameToInstantiate = componentNamePayload;
                std::cerr << "Warning: Dropping component onto shape not fully implemented yet." << std::endl;




            }
            ImGui::EndDragDropTarget();
        }




        if (!shape_effectively_locked && ImGui::IsItemClicked(0) && !ImGui::IsMouseDragging(0)) {
            bool ctrl_pressed = ImGui::GetIO().KeyCtrl;
            bool shift_pressed = ImGui::GetIO().KeyShift;

            if (!shift_pressed) {
                if (!ctrl_pressed) {
                    selectedShapes.clear();
                    selectedShapes.push_back(shape);
                    lastClickedShape = shape;
                    lastClickedLayerIndex = layerIndex;
                    selectedLayerIndex = layerIndex;
                    lastSelectedLayerIndex = layerIndex;


                    lastSelectedShapeIndex = shapeIndexInLayer;
                }
                else {
                    auto it = std::find(selectedShapes.begin(), selectedShapes.end(), shape);
                    if (it == selectedShapes.end()) {
                        selectedShapes.push_back(shape);

                        lastClickedShape = shape;
                        lastClickedLayerIndex = layerIndex;

                        lastSelectedLayerIndex = layerIndex;
                        lastSelectedShapeIndex = shapeIndexInLayer;
                    }
                    else {
                        selectedShapes.erase(it);

                        if (lastClickedShape == shape) {
                            lastClickedShape = nullptr;
                            lastClickedLayerIndex = -1;

                            lastSelectedShapeIndex = -1;
                        }

                        if (selectedShapes.empty()) {
                            lastSelectedShapeIndex = -1;


                        }
                    }

                    selectedLayerIndex = layerIndex;
                }
            }
            else {

                if (lastClickedShape != nullptr && lastClickedLayerIndex == layerIndex) {
                    selectedShapes.clear();








                    int currentShapeIndexInLayerVec = -1;
                    int lastShapeIndexInLayerVec = -1;
                    for (int k = 0; k < layer.shapes.size(); ++k) {
                        if (layer.shapes[k].id == shape->id) currentShapeIndexInLayerVec = k;
                        if (layer.shapes[k].id == lastClickedShape->id) lastShapeIndexInLayerVec = k;
                    }

                    if (currentShapeIndexInLayerVec != -1 && lastShapeIndexInLayerVec != -1) {
                        int startIdx = std::min(currentShapeIndexInLayerVec, lastShapeIndexInLayerVec);
                        int endIdx = std::max(currentShapeIndexInLayerVec, lastShapeIndexInLayerVec);

                        for (int k = startIdx; k <= endIdx; ++k) {

                            if (k < layer.shapes.size()) {
                                ShapeItem& potentialSelection = layer.shapes[k];
                                bool potentialLocked = potentialSelection.locked || layer.locked;
                                if (!potentialLocked) {

                                    if (std::find(selectedShapes.begin(), selectedShapes.end(), &potentialSelection) == selectedShapes.end()) {
                                        selectedShapes.push_back(&potentialSelection);
                                    }
                                }
                            }
                        }
                    }
                    else {

                        if (!shape->locked) selectedShapes.push_back(shape);
                    }


                    selectedLayerIndex = layerIndex;
                    lastSelectedLayerIndex = layerIndex;

                    lastSelectedShapeIndex = currentShapeIndexInLayerVec;

                }
                else {
                    selectedShapes.clear();
                    if (!shape->locked) selectedShapes.push_back(shape);
                    lastClickedShape = shape;
                    lastClickedLayerIndex = layerIndex;
                    selectedLayerIndex = layerIndex;
                    lastSelectedLayerIndex = layerIndex;
                    lastSelectedShapeIndex = shapeIndexInLayer;
                }
            }
        }


        if (ImGui::BeginPopupContextItem(("ShapeContext##" + std::to_string(shape->id)).c_str())) {

            if (ImGui::MenuItem("Rename##ShapeCtx", nullptr, false, !shape_effectively_locked)) {


                ImGui::OpenPopup("RenameShapePopupProp");


            }

            if (ImGui::MenuItem("Duplicate##ShapeCtx", nullptr, false, !shape_effectively_locked)) {
                ShapeItem duplicatedShape = *shape;
                duplicatedShape.id = GetUniqueShapeID();


                std::string baseName = shape->name + "_Copy";
                std::string finalName = baseName;
                int suffix = 1;
                std::set<std::string> existingNames;
                WindowData& currentWindowData = g_windowsMap.at(selectedGuiWindow);
                for (const auto& l : currentWindowData.layers) {
                    for (const auto& shp : l.shapes) existingNames.insert(shp.name);
                }
                while (existingNames.count(finalName)) {
                    finalName = baseName + "_" + std::to_string(suffix++);
                }
                duplicatedShape.name = finalName;


                duplicatedShape.parent = nullptr;
                duplicatedShape.children.clear();


                duplicatedShape.isPressed = false;
                duplicatedShape.isHeld = false;
                duplicatedShape.isAnimating = false;
                duplicatedShape.currentAnimation = nullptr;


                duplicatedShape.position += ImVec2(10, 10);
                duplicatedShape.basePosition = duplicatedShape.position;


                layer.shapes.push_back(duplicatedShape);


                selectedShapes.clear();
                selectedShapes.push_back(&layer.shapes.back());
                lastSelectedShapeIndex = layer.shapes.size() - 1;
                lastSelectedLayerIndex = layerIndex;
                lastClickedShape = selectedShapes.back();
                lastClickedLayerIndex = layerIndex;

                MarkSceneUpdated();
                ImGui::CloseCurrentPopup();
            }

            if (ImGui::MenuItem("Delete##ShapeCtx", nullptr, false, !shape_effectively_locked)) {

                int deletedShapeId = shape->id;


                std::vector<ShapeItem*> childrenCopy = shape->children;
                for (auto* child : childrenCopy) {
                    if (child) RemoveParentKeepTransform(child);
                }




                if (shape->parent) {
                    auto& siblings = shape->parent->children;

                    siblings.erase(std::remove_if(siblings.begin(), siblings.end(),
                        [deletedShapeId](const ShapeItem* s) { return s && s->id == deletedShapeId; }),
                        siblings.end());
                    shape->parent = nullptr;
                }


                layer.shapes.erase(std::remove_if(layer.shapes.begin(), layer.shapes.end(),
                    [deletedShapeId](const ShapeItem& s) { return s.id == deletedShapeId; }),
                    layer.shapes.end());


                selectedShapes.erase(std::remove_if(selectedShapes.begin(), selectedShapes.end(),
                    [deletedShapeId](ShapeItem* s_ptr) { return s_ptr && s_ptr->id == deletedShapeId; }),
                    selectedShapes.end());


                if (lastClickedShape && lastClickedShape->id == deletedShapeId) {
                    lastClickedShape = nullptr;
                    lastClickedLayerIndex = -1;
                }

                if (lastSelectedLayerIndex == layerIndex) {
                    lastSelectedShapeIndex = -1;
                }

                if (selectedShapes.empty()) {
                    lastSelectedLayerIndex = layerIndex;
                    lastSelectedShapeIndex = -1;
                }

                MarkSceneUpdated();
                ImGui::CloseCurrentPopup();
                ImGui::EndPopup();
                ImGui::PopStyleVar();
                ImGui::PopID();

                return;
            }
            ImGui::Separator();

            if (ImGui::MenuItem("Clear Parent", nullptr, false, shape->parent != nullptr && !shape_effectively_locked)) {
                RemoveParent(shape);
                MarkSceneUpdated();
                ImGui::CloseCurrentPopup();
            }

            if (ImGui::MenuItem("Clear Parent and Keep Transform", nullptr, false, shape->parent != nullptr && !shape_effectively_locked)) {
                RemoveParentKeepTransform(shape);
                MarkSceneUpdated();
                ImGui::CloseCurrentPopup();
            }
            ImGui::Separator();

            int currentShapeZ = shape->zOrder;
            ImGui::PushItemWidth(80);
            if (ImGui::DragInt("Z-Order##ShapeCtx", &shape->zOrder, 0.1f)) {

                if (shape->zOrder != currentShapeZ) {
                    needsLayerSort = true;
                    layerToSort = layerIndex;
                    MarkSceneUpdated();
                }

            }


            if (ImGui::IsItemDeactivatedAfterEdit()) {

            }
            ImGui::PopItemWidth();

            ImGui::EndPopup();
        }




        if (node_open) {
            if (!is_leaf) {


                std::vector<ShapeItem*> childrenToDraw = shape->children;
                for (ShapeItem* child : childrenToDraw) {
                    if (!child) continue;





                    int childShapeIndexInLayer = -1;






                    DrawShapeTreeNode(child, layer, layerIndex, childShapeIndexInLayer, selectedLayerIndex, selectedShapes, lastClickedShape, lastClickedLayerIndex, layerToSort, needsLayerSort);

                }
            }
            ImGui::TreePop();
        }

        ImGui::PopStyleVar();
        ImGui::PopID();
    }

    inline void ShowUI_PropertiesPanel(WindowData& windowData, int& selectedLayerIndex, std::vector<ShapeItem*>& selectedShapes) {





        ImGui::BeginChild("PropertiesPanel");

        if (selectedShapes.empty() && selectedLayerIndex >= 0 && selectedLayerIndex < windowData.layers.size())
        {

            ImGui::Text("Layer Properties: %s", windowData.layers[selectedLayerIndex].name.c_str());
            ImGui::Separator();
            Layer& layer = windowData.layers[selectedLayerIndex];
            bool layerWasLocked = layer.locked;
            if (ImGui::Checkbox("Visible##LayerProp", &layer.visible)) MarkSceneUpdated(); ImGui::SameLine();
            if (ImGui::Checkbox("Locked##LayerProp", &layer.locked)) {
                if (layer.locked && !layerWasLocked) {
                    selectedShapes.erase(std::remove_if(selectedShapes.begin(), selectedShapes.end(),
                        [&](ShapeItem* s_ptr) { return s_ptr && FindShapeLayerIndex(s_ptr->id) == selectedLayerIndex; }), selectedShapes.end());
                }
                MarkSceneUpdated();
            }
            if (ImGui::DragInt("Z-Order##LayerProp", &layer.zOrder, 0.1f)) {




                MarkSceneUpdated();

            }

        }
        else if (selectedShapes.size() == 1)
        {

            ShapeItem& s = *selectedShapes[0];
            bool layer_is_locked = false;
            int current_shape_layer_idx = FindShapeLayerIndex(s.id);
            if (current_shape_layer_idx != -1 && current_shape_layer_idx < windowData.layers.size()) {
                layer_is_locked = windowData.layers[current_shape_layer_idx].locked;
            }
            bool shape_effectively_locked = s.locked || layer_is_locked;


            ImGui::Text("Shape Properties: %s (ID: %d)", s.name.c_str(), s.id);
            if (shape_effectively_locked) {
                ImGui::SameLine(); ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "[LOCKED]");
            }
            ImGui::Separator();


            ImGui::BeginDisabled(shape_effectively_locked);
            if (ImGui::Button("[X] Delete##Shape")) {
                int layerIdx = FindShapeLayerIndex(s.id);
                if (layerIdx != -1) {
                    Layer& layer = windowData.layers[layerIdx];
                    std::vector<ShapeItem*> childrenCopy = s.children;
                    for (auto* child : childrenCopy) if (child) RemoveParentKeepTransform(child);
                    if (s.parent) {
                        auto& siblings = s.parent->children;
                        siblings.erase(std::remove(siblings.begin(), siblings.end(), &s), siblings.end());
                    }
                    int deletedShapeId = s.id;
                    layer.shapes.erase(std::remove_if(layer.shapes.begin(), layer.shapes.end(),
                        [deletedShapeId](const ShapeItem& shp) { return shp.id == deletedShapeId; }),
                        layer.shapes.end());

                    selectedShapes.clear();

                    if (lastClickedShape && lastClickedShape->id == deletedShapeId) {
                        lastClickedShape = nullptr;
                        lastClickedLayerIndex = -1;
                    }
                    lastSelectedLayerIndex = layerIdx;
                    lastSelectedShapeIndex = -1;

                    MarkSceneUpdated();
                    ImGui::EndChild();
                    ImGui::EndDisabled();
                    return;
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("[Dup] Duplicate##Shape")) {
                int layerIdx = FindShapeLayerIndex(s.id);
                if (layerIdx != -1) {
                    Layer& layer = windowData.layers[layerIdx];
                    ShapeItem duplicatedShape = s;
                    duplicatedShape.id = GetUniqueShapeID();
                    std::string baseName = s.name + "_Copy";
                    std::string finalName = baseName;
                    int suffix = 1;
                    std::set<std::string> existingNames;
                    for (const auto& l : windowData.layers) for (const auto& shp : l.shapes) existingNames.insert(shp.name);
                    while (existingNames.count(finalName)) { finalName = baseName + "_" + std::to_string(suffix++); }
                    duplicatedShape.name = finalName;

                    duplicatedShape.parent = nullptr;
                    duplicatedShape.children.clear();
                    duplicatedShape.isPressed = false;
                    duplicatedShape.isHeld = false;
                    duplicatedShape.isAnimating = false;
                    duplicatedShape.currentAnimation = nullptr;
                    duplicatedShape.position += ImVec2(10, 10);
                    duplicatedShape.basePosition = duplicatedShape.position;

                    layer.shapes.push_back(duplicatedShape);
                    selectedShapes.clear();
                    selectedShapes.push_back(&layer.shapes.back());
                    lastSelectedShapeIndex = layer.shapes.size() - 1;
                    lastSelectedLayerIndex = layerIdx;
                    if (lastClickedShape) {
                        lastClickedShape = selectedShapes.back();
                        lastClickedLayerIndex = layerIdx;
                    }
                    MarkSceneUpdated();
                }
            }
            ImGui::SameLine();
            static char renameBufferPopup[128];
            if (ImGui::Button("[Ren] Rename##Shape")) {
                strncpy(renameBufferPopup, s.name.c_str(), 127);
                renameBufferPopup[127] = '\0';
                ImGui::OpenPopup("RenameShapePopupProp");
            }
            if (ImGui::BeginPopup("RenameShapePopupProp")) {
                ImGui::Text("Rename Shape:");
                if (ImGui::IsWindowAppearing()) ImGui::SetKeyboardFocusHere();
                if (ImGui::InputText("##NewShapeNameInput", renameBufferPopup, 128, ImGuiInputTextFlags_EnterReturnsTrue)) {
                    if (strlen(renameBufferPopup) > 0 && s.name != renameBufferPopup) {
                        s.name = renameBufferPopup;
                        MarkSceneUpdated();
                    }
                    ImGui::CloseCurrentPopup();
                }
                if (ImGui::Button("OK##RenameShape")) {
                    if (strlen(renameBufferPopup) > 0 && s.name != renameBufferPopup) {
                        s.name = renameBufferPopup;
                        MarkSceneUpdated();
                    }
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (ImGui::Button("Cancel##RenameShape") || ImGui::IsKeyPressed(ImGuiKey_Escape)) {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
            ImGui::EndDisabled();
            ImGui::Separator();


            ImGui::BeginDisabled(shape_effectively_locked);

            if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::BeginDisabled(shape_effectively_locked);


                ImGui::Text("Abs:");
                ImVec2 tempAbsPos = s.position;

                if (ImGui::DragFloat2("Position##Abs", (float*)&tempAbsPos, 0.5f))
                {
                    if (s.parent == nullptr) {

                        ImVec2 delta = tempAbsPos - s.position;
                        s.basePosition = s.basePosition + delta;
                    }
                    else {

                        ImVec2 parentPos = s.parent->position;
                        float parentRot = s.parent->rotation;


                        ImVec2 newWorldOffset = tempAbsPos - parentPos;


                        ImVec2 newLocalOffset = RotateP(newWorldOffset, ImVec2(0.0f, 0.0f), -parentRot);


                        s.originalPosition = newLocalOffset;
                    }
                    MarkSceneUpdated();
                }
                ImGui::Separator();
                ImGui::Text("Local (Parented):");
                ImGui::BeginDisabled(s.parent == nullptr);
                if (ImGui::DragFloat2("Offset##Local", (float*)&s.originalPosition, 0.5f)) {
                    MarkSceneUpdated();
                }
                if (s.parent == nullptr && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                    ImGui::SetTooltip("Only available when parented.");
                }
                ImGui::EndDisabled();
                ImGui::Separator();


                ImVec2 currentSize = s.size;
                ImVec2 tempAbsSize = s.size;
                if (ImGui::DragFloat2("Size##Abs", (float*)&tempAbsSize, 0.5f)) {

                    tempAbsSize.x = std::max(s.minSize.x, std::min(tempAbsSize.x, s.maxSize.x));
                    tempAbsSize.y = std::max(s.minSize.y, std::min(tempAbsSize.y, s.maxSize.y));

                    if (memcmp(&tempAbsSize, &currentSize, sizeof(ImVec2)) != 0) {
                        ImVec2 deltaSize = tempAbsSize - s.size;
                        s.baseSize = s.baseSize + deltaSize;

                        s.baseSize.x = std::max(s.minSize.x, std::min(s.baseSize.x, s.maxSize.x));
                        s.baseSize.y = std::max(s.minSize.y, std::min(s.baseSize.y, s.maxSize.y));
                        MarkSceneUpdated();
                    }
                }
                float currentWorldRotDeg = s.rotation * (180.0f / IM_PI);
                float tempAbsRotDeg = currentWorldRotDeg;
                if (ImGui::DragFloat("Rot##Abs", &tempAbsRotDeg, 1.0f, -720, 720, "%.1f deg")) {
                    if (fabs(tempAbsRotDeg - currentWorldRotDeg) > 1e-4) {
                        float newWorldRotRad = tempAbsRotDeg * (IM_PI / 180.0f);
                        if (s.parent != nullptr) {


                            s.baseRotation = newWorldRotRad - s.parent->rotation;
                        }
                        else {

                            s.baseRotation = newWorldRotRad;
                            s.rotation = newWorldRotRad;
                        }
                        MarkSceneUpdated();
                    }
                }

                ImGui::Text("Base:");
                if (ImGui::DragFloat2("Position##Base", (float*)&s.basePosition, 0.5f)) MarkSceneUpdated();
                ImVec2 currentBaseSize = s.baseSize;
                if (ImGui::DragFloat2("Size##Base", (float*)&s.baseSize, 0.5f)) {
                    s.baseSize.x = std::max(s.minSize.x, std::min(s.baseSize.x, s.maxSize.x));
                    s.baseSize.y = std::max(s.minSize.y, std::min(s.baseSize.y, s.maxSize.y));
                    if (memcmp(&s.baseSize, &currentBaseSize, sizeof(ImVec2)) != 0) MarkSceneUpdated();
                }
                float baseRotDeg = s.baseRotation * (180.0f / IM_PI);
                if (ImGui::DragFloat("Rot##Base", &baseRotDeg, 1.0f, -720, 720, "%.1f deg")) { s.baseRotation = baseRotDeg * (IM_PI / 180.0f); MarkSceneUpdated(); }

                if (ImGui::Button("Apply Current to Base")) {
                    s.basePosition = s.position;
                    s.baseSize = s.size;
                    s.baseRotation = s.rotation;
                    MarkSceneUpdated();
                }
                if (ImGui::Button("Reset Transform to Base")) {
                    s.position = s.basePosition;
                    s.size = s.baseSize;
                    s.rotation = s.baseRotation;

                    for (auto& anim : s.onClickAnimations) { anim.progress = 0.0; anim.isPlaying = false; }
                    s.currentAnimation = nullptr;
                    s.baseKeyOffset = ImVec2(0, 0); s.baseKeySizeOffset = ImVec2(0, 0); s.baseKeyRotationOffset = 0.0f;
                    MarkSceneUpdated();
                }
                ImGui::EndDisabled();
            }


            if (ImGui::CollapsingHeader("Layout & Constraints", ImGuiTreeNodeFlags_DefaultOpen)) {

                ImGui::SeparatorText("Sizing");
                if (ImGui::Checkbox("Percentage Size", &s.usePercentageSize)) MarkSceneUpdated();
                ImGui::BeginDisabled(!s.usePercentageSize);
                if (ImGui::DragFloat2("Size (%)##PercSize", (float*)&s.percentageSize, 0.5f, 0.0f, 1000.0f, "%.1f")) {
                    s.percentageSize.x = std::max(0.0f, s.percentageSize.x);
                    s.percentageSize.y = std::max(0.0f, s.percentageSize.y);
                    MarkSceneUpdated();
                }
                ImGui::EndDisabled();


                ImGui::SeparatorText("Positioning");

                const char* anchorModes[] = { "None", "TopLeft", "Top", "TopRight", "Left", "Center", "Right", "BottomLeft", "Bottom", "BottomRight" };
                int currentAnchor = static_cast<int>(s.anchorMode);
                if (ImGui::Combo("Anchor", &currentAnchor, anchorModes, IM_ARRAYSIZE(anchorModes))) {
                    s.anchorMode = static_cast<ShapeItem::AnchorMode>(currentAnchor);

                    if (s.anchorMode != ShapeItem::AnchorMode::None) {
                        s.usePercentagePos = false;
                    }
                    MarkSceneUpdated();
                }


                ImGui::BeginDisabled(s.anchorMode == ShapeItem::AnchorMode::None);
                if (ImGui::DragFloat2("Margin##Anchor", (float*)&s.anchorMargin, 0.5f)) MarkSceneUpdated();
                ImGui::EndDisabled();


                ImGui::BeginDisabled(s.anchorMode != ShapeItem::AnchorMode::None);
                if (ImGui::Checkbox("Percentage Position", &s.usePercentagePos)) {

                    if (s.usePercentagePos) {
                        s.anchorMode = ShapeItem::AnchorMode::None;
                    }
                    MarkSceneUpdated();
                }
                if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) ImGui::SetTooltip("Anchoring must be 'None' to enable percentage position.");
                ImGui::EndDisabled();


                ImGui::BeginDisabled(!s.usePercentagePos || s.anchorMode != ShapeItem::AnchorMode::None);
                if (ImGui::DragFloat2("Position (%)##PercPos", (float*)&s.percentagePos, 0.5f, -1000.0f, 1000.0f, "%.1f")) MarkSceneUpdated();
                ImGui::EndDisabled();


                ImGui::SeparatorText("Constraints");
                ImVec2 currentMin = s.minSize;
                ImVec2 currentMax = s.maxSize;
                bool minChanged = false;
                bool maxChanged = false;


                if (ImGui::DragFloat2("Min Size##Const", (float*)&s.minSize, 1.0f, 0.0f, 99999.0f, "%.0f")) {
                    s.minSize.x = std::max(0.f, s.minSize.x);
                    s.minSize.y = std::max(0.f, s.minSize.y);

                    s.maxSize.x = std::max(s.minSize.x, s.maxSize.x);
                    s.maxSize.y = std::max(s.minSize.y, s.maxSize.y);
                    minChanged = true;
                    MarkSceneUpdated();
                }


                if (ImGui::DragFloat2("Max Size##Const", (float*)&s.maxSize, 1.0f, 0.0f, 99999.0f, "%.0f")) {
                    s.maxSize.x = std::max(0.f, s.maxSize.x);
                    s.maxSize.y = std::max(0.f, s.maxSize.y);

                    s.maxSize.x = std::max(s.minSize.x, s.maxSize.x);
                    s.maxSize.y = std::max(s.minSize.y, s.maxSize.y);
                    maxChanged = true;
                    MarkSceneUpdated();
                }


                if (minChanged || maxChanged) {
                    s.size.x = std::max(s.minSize.x, std::min(s.size.x, s.maxSize.x));
                    s.size.y = std::max(s.minSize.y, std::min(s.size.y, s.maxSize.y));
                    s.baseSize.x = std::max(s.minSize.x, std::min(s.baseSize.x, s.maxSize.x));
                    s.baseSize.y = std::max(s.minSize.y, std::min(s.baseSize.y, s.maxSize.y));
                    MarkSceneUpdated();
                }
            }


            if (ImGui::CollapsingHeader("Appearance", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::SeparatorText("Shape");
                int st = (int)s.type; if (ImGui::Combo("Type", &st, "Rectangle\0Circle\0")) { s.type = (ShapeType)st; MarkSceneUpdated(); }
                if (ImGui::DragFloat("Corner Radius", &s.cornerRadius, 0.5f, 0.0f, 200.0f)) MarkSceneUpdated();
                if (ImGui::DragFloat("Border", &s.borderThickness, 0.1f, 0.0f, 20.0f)) MarkSceneUpdated();

                ImGui::SeparatorText("Fill & Border");
                if (ImGui::ColorEdit4("Fill##Color", (float*)&s.fillColor, ImGuiColorEditFlags_AlphaBar)) MarkSceneUpdated();
                if (ImGui::ColorEdit4("Border##Color", (float*)&s.borderColor, ImGuiColorEditFlags_AlphaBar)) MarkSceneUpdated();

                if (ImGui::Checkbox("Use Gradient Fill", &s.useGradient)) { MarkSceneUpdated(); ClearGradientTextureCache(); }
                ImGui::BeginDisabled(!s.useGradient);
                if (s.useGradient) {
                    ImGui::Indent();
                    float gradr = s.gradientRotation; if (ImGui::DragFloat("Rotation##Grad", &gradr, 1.0f, 0.0f, 360.0f, "%.1f deg")) { s.gradientRotation = gradr; MarkSceneUpdated(); ClearGradientTextureCache(); }
                    int interp_type = (int)s.gradientInterpolation; const char* interpItems[] = { "Linear", "Ease", "Constant", "Cardinal", "BSpline" }; if (ImGui::Combo("Interpolation##Grad", &interp_type, interpItems, IM_ARRAYSIZE(interpItems))) { s.gradientInterpolation = (ShapeItem::GradientInterpolation)interp_type; MarkSceneUpdated(); ClearGradientTextureCache(); }
                    ImGui::Text("Color Ramp:");
                    int ramp_color_to_delete = -1;
                    for (int ci = 0; ci < (int)s.colorRamp.size(); ci++) {
                        ImGui::PushID(ci + 3000);
                        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
                        if (ImGui::SliderFloat("Pos##Grad", &s.colorRamp[ci].first, 0.0f, 1.0f, "%.3f")) { MarkSceneUpdated(); ClearGradientTextureCache(); }
                        ImGui::PopItemWidth(); ImGui::SameLine();
                        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - 30);
                        if (ImGui::ColorEdit4("Color##Grad", (float*)&s.colorRamp[ci].second, ImGuiColorEditFlags_AlphaBar)) { MarkSceneUpdated(); ClearGradientTextureCache(); }
                        ImGui::PopItemWidth(); ImGui::SameLine();
                        if (ImGui::Button("[X]##GradDel", ImVec2(25, 0)) && s.colorRamp.size() > 1) {
                            ramp_color_to_delete = ci;
                        }
                        ImGui::PopID();
                    }
                    if (ramp_color_to_delete != -1) {
                        s.colorRamp.erase(s.colorRamp.begin() + ramp_color_to_delete); MarkSceneUpdated(); ClearGradientTextureCache();
                    }
                    if (ImGui::Button("[+] Add Color##Grad")) {
                        s.colorRamp.emplace_back(0.5f, ImVec4(1, 1, 1, 1));
                        std::sort(s.colorRamp.begin(), s.colorRamp.end(), [](const auto& a, const auto& b) {return a.first < b.first; });
                        MarkSceneUpdated(); ClearGradientTextureCache();
                    }
                    ImGui::Unindent();
                }
                ImGui::EndDisabled();

                ImGui::SeparatorText("Effects");
                if (ImGui::TreeNodeEx("Shadow##Effects", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_AllowItemOverlap)) {
                    if (ImGui::ColorEdit4("Color##Shadow", (float*)&s.shadowColor, ImGuiColorEditFlags_AlphaBar)) MarkSceneUpdated();
                    if (ImGui::DragFloat4("Spread##Shadow", (float*)&s.shadowSpread, 0.1f, 0.0f, 100.0f)) MarkSceneUpdated();
                    if (ImGui::DragFloat2("Offset##Shadow", (float*)&s.shadowOffset, 0.5f)) MarkSceneUpdated();
                    float shadow_rd = s.shadowRotation * (180.0f / IM_PI); if (ImGui::DragFloat("Rotation##Shadow", &shadow_rd, 1.0f, 0.0f, 360.0f, "%.1f deg")) { s.shadowRotation = shadow_rd * (IM_PI / 180.0f); MarkSceneUpdated(); }
                    if (ImGui::Checkbox("Use Corner Radius##Shadow", &s.shadowUseCornerRadius)) MarkSceneUpdated();
                    ImGui::TreePop();
                }
                if (ImGui::TreeNodeEx("Blur##Effects", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_AllowItemOverlap)) {
                    if (ImGui::DragFloat("Amount##Blur", &s.blurAmount, 0.1f, 0.0f, 20.0f)) MarkSceneUpdated();
                    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Basic simulated blur effect (visual only).");
                    ImGui::TreePop();
                }
                if (ImGui::TreeNodeEx("Glass##Effects", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_AllowItemOverlap)) {
                    if (ImGui::Checkbox("Enable##Glass", &s.useGlass)) MarkSceneUpdated();
                    ImGui::BeginDisabled(!s.useGlass);
                    if (ImGui::SliderFloat("Blur##Glass", &s.glassBlur, 1.0f, 100.0f)) MarkSceneUpdated();
                    if (ImGui::SliderFloat("Alpha##Glass", &s.glassAlpha, 0.0f, 1.0f)) MarkSceneUpdated();
                    if (ImGui::ColorEdit4("Tint##Glass", (float*)&s.glassColor, ImGuiColorEditFlags_AlphaBar)) MarkSceneUpdated();
                    ImGui::EndDisabled();
                    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) ImGui::SetTooltip("Glass effect likely requires custom shader implementation.");
                    ImGui::TreePop();
                }

                ImGui::SeparatorText("Visibility & Interaction");
                if (ImGui::Checkbox("Visible##Vis", &s.visible)) MarkSceneUpdated(); ImGui::SameLine();
                if (ImGui::Checkbox("Locked##Vis", &s.locked)) MarkSceneUpdated(); ImGui::SameLine();
                ImGui::PushItemWidth(60);
                int currentShapeZ = s.zOrder;
                if (ImGui::DragInt("Z-Order##Vis", &s.zOrder, 0.1f)) {
                    if (s.zOrder != currentShapeZ) {
                        int layerIdx = FindShapeLayerIndex(s.id);
                        if (layerIdx != -1) {


                            MarkSceneUpdated();
                        }
                    }
                }
                if (ImGui::IsItemDeactivatedAfterEdit() && s.zOrder != currentShapeZ) {}
                ImGui::PopItemWidth();
                if (ImGui::Checkbox("Allow Overlap##Vis", &s.allowItemOverlap)) MarkSceneUpdated(); ImGui::SameLine();
                if (ImGui::Checkbox("Force Render Last##Vis", &s.forceOverlap)) MarkSceneUpdated(); ImGui::SameLine();
                if (ImGui::Checkbox("Block Underneath##Vis", &s.blockUnderlying)) MarkSceneUpdated();
            }


            bool isContainer = s.isChildWindow || s.isImGuiContainer;
            ImGui::BeginDisabled(isContainer);
            if (ImGui::CollapsingHeader("Text", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (isContainer && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) ImGui::SetTooltip("Text properties disabled for containers.");
                if (ImGui::Checkbox("Enable Text", &s.hasText)) MarkSceneUpdated();
                ImGui::BeginDisabled(!s.hasText);
                if (s.hasText) {
                    static char textBufferProp[1024];
                    static int lastEditedShapeId = -1;
                    if (s.id != lastEditedShapeId) {
                        strncpy(textBufferProp, s.text.c_str(), 1023);
                        textBufferProp[1023] = '\0';
                        lastEditedShapeId = s.id;
                    }
                    if (ImGui::InputTextMultiline("Content##Text", textBufferProp, 1024, ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 4))) { s.text = textBufferProp; MarkSceneUpdated(); }
                    if (ImGui::ColorEdit4("Color##Text", (float*)&s.textColor, ImGuiColorEditFlags_AlphaBar)) MarkSceneUpdated();
                    if (ImGui::DragFloat("Size##Text", &s.textSize, 0.1f, 1.0f, 128.0f)) MarkSceneUpdated();
                    if (ImGui::DragFloat2("Position Offset##Text", (float*)&s.textPosition, 0.5f)) MarkSceneUpdated();
                    float textRotDeg = s.textRotation; if (ImGui::DragFloat("Rotation##Text", &textRotDeg, 1.0f, -360, 360, "%.1f deg")) { s.textRotation = textRotDeg; MarkSceneUpdated(); }
                    const char* alignItems[] = { "Left", "Center", "Right" }; int textAlign = s.textAlignment; if (ImGui::Combo("Align##Text", &textAlign, alignItems, 3)) { s.textAlignment = textAlign; MarkSceneUpdated(); }
                    if (ImGui::Checkbox("Dynamic Size##Text", &s.dynamicTextSize)) MarkSceneUpdated();
                }
                ImGui::EndDisabled();
            }
            else {
                if (isContainer && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) ImGui::SetTooltip("Text properties disabled for containers.");
            }
            ImGui::EndDisabled();


            if (ImGui::CollapsingHeader("Image", ImGuiTreeNodeFlags_DefaultOpen)) {
                bool prevHasImage = s.hasEmbeddedImage;
                if (ImGui::Checkbox("Use Embedded Image", &s.hasEmbeddedImage)) {
                    if (prevHasImage && !s.hasEmbeddedImage) s.imageDirty = true;
                    MarkSceneUpdated();
                }
                ImGui::BeginDisabled(!s.hasEmbeddedImage);
                if (s.hasEmbeddedImage) {
                    if (g_embeddedImageFunctionsCount > 0) {
                        if (ImGui::Combo("Source##Image", &s.embeddedImageIndex, g_embeddedImageFunctions, g_embeddedImageFunctionsCount)) {
                            s.imageDirty = true;
                            MarkSceneUpdated();
                        }
                        if (s.embeddedImageIndex >= 0 && s.embeddedImageTexture) {
                            ImGui::Text("Preview:");
                            float aspect = (s.embeddedImageHeight > 0 && s.embeddedImageWidth > 0) ? ((float)s.embeddedImageHeight / (float)s.embeddedImageWidth) : 1.0f;
                            ImVec2 previewSize = ImVec2(100, 100.0f * aspect);
                            ImGui::Image(s.embeddedImageTexture, previewSize);
                        }
                        else if (s.embeddedImageIndex >= 0) { ImGui::TextDisabled("Loading..."); }
                    }
                    else { ImGui::TextDisabled("No embedded image functions available."); }
                }
                ImGui::EndDisabled();
            }


            if (ImGui::CollapsingHeader("Button Behavior", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (ImGui::Checkbox("Is Button", &s.isButton)) MarkSceneUpdated();
                ImGui::BeginDisabled(!s.isButton);
                if (s.isButton) {
                    if (ImGui::ColorEdit4("Hover Color##Button", (float*)&s.hoverColor, ImGuiColorEditFlags_AlphaBar)) MarkSceneUpdated();
                    if (ImGui::ColorEdit4("Click Color##Button", (float*)&s.clickedColor, ImGuiColorEditFlags_AlphaBar)) MarkSceneUpdated();
                    const char* behaviors[] = { "Single Click", "Toggle", "Hold" }; int currentBehavior = (int)s.buttonBehavior; if (ImGui::Combo("Behavior##Button", &currentBehavior, behaviors, 3)) { s.buttonBehavior = (ShapeItem::ButtonBehavior)currentBehavior; MarkSceneUpdated(); }
                    if (s.buttonBehavior == ShapeItem::ButtonBehavior::Toggle) {
                        if (ImGui::InputInt("Toggle Group ID##Button", &s.groupId)) MarkSceneUpdated();
                        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Buttons in the same group (>0) will toggle exclusively.");
                    }
                    bool usesAction = s.useOnClick || !s.eventHandlers.empty();
                    if (ImGui::Checkbox("Has Click Action##Button", &usesAction)) {
                        if (usesAction && !s.useOnClick && s.eventHandlers.empty()) {
                            s.eventHandlers.push_back({ "onClick", "defaultOnClickAction", nullptr });
                        }
                        else if (!usesAction) {
                            s.eventHandlers.clear();
                            s.useOnClick = false;
                            s.onClick = nullptr;
                        }
                        MarkSceneUpdated();
                    }
                    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Indicates if a click action (legacy onClick or EventHandler) is assigned.\nActual function must be assigned in code.");
                }
                ImGui::EndDisabled();
            }


            if (ImGui::CollapsingHeader("Container Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
                bool is_child_win = s.isChildWindow;
                ImGui::BeginDisabled(s.isImGuiContainer);
                if (ImGui::Checkbox("Registered Child Window Container", &is_child_win)) {
                    if (is_child_win) { s.isChildWindow = true; s.isImGuiContainer = false; }
                    else { s.isChildWindow = false; } MarkSceneUpdated();
                }
                if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) ImGui::SetTooltip("Mutually exclusive with ImGui Content Container.");
                ImGui::EndDisabled();

                ImGui::BeginDisabled(!s.isChildWindow);
                if (s.isChildWindow) {
                    ImGui::Indent();
                    if (ImGui::Checkbox("Sync Size/Pos", &s.childWindowSync)) MarkSceneUpdated();
                    if (ImGui::InputInt("Window Group ID##Child", &s.childWindowGroupId)) MarkSceneUpdated();
                    ImGui::Text("Note: Window mapping/logic is set globally.");
                    ImGui::Unindent();
                }
                ImGui::EndDisabled();

                bool is_imgui_cont = s.isImGuiContainer;
                ImGui::BeginDisabled(s.isChildWindow);
                if (ImGui::Checkbox("Direct ImGui Content Container", &is_imgui_cont)) {
                    if (is_imgui_cont) { s.isImGuiContainer = true; s.isChildWindow = false; }
                    else { s.isImGuiContainer = false; } MarkSceneUpdated();
                }
                if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) ImGui::SetTooltip("Mutually exclusive with Child Window Container.");
                ImGui::EndDisabled();

                ImGui::BeginDisabled(!s.isImGuiContainer);
                if (s.isImGuiContainer) {
                    ImGui::Indent();
                    ImGui::TextDisabled("Requires setting 'renderImGuiContent' callback in code.");
                    if (fabs(s.rotation) > 0.01) ImGui::TextColored(ImVec4(1, 1, 0, 1), "Warning: Rotation may clip ImGui content.");
                    ImGui::Unindent();
                }
                ImGui::EndDisabled();
            }

            if (ImGui::CollapsingHeader("Layout Konteyner Ayarları", ImGuiTreeNodeFlags_DefaultOpen))
            {
                bool isContainer = s.isLayoutContainer;
                if (ImGui::Checkbox("Layout Konteyneri Olarak Kullan##Layout", &isContainer)) {
                    if (s.isLayoutContainer != isContainer) {
                        s.isLayoutContainer = isContainer;

                        if (!isContainer && s.layoutManager != nullptr) {
                            delete s.layoutManager;
                            s.layoutManager = nullptr;
                        }


                        MarkSceneUpdated();
                    }
                }

                ImGui::BeginDisabled(!s.isLayoutContainer);

                const char* layoutTypes[] = { "Yok (None)", "Dikey (Vertical)", "Yatay (Horizontal)" };
                int currentLayoutIndex = 0;
                if (s.layoutManager != nullptr) {
                    const char* typeName = s.layoutManager->getTypeName();
                    if (strcmp(typeName, "Vertical") == 0) {
                        currentLayoutIndex = 1;
                    }
                    else if (strcmp(typeName, "Horizontal") == 0) {
                        currentLayoutIndex = 2;
                    }
                }

                ImGui::PushItemWidth(-FLT_MIN * 0.5f);
                if (ImGui::Combo("Layout Türü##Layout", &currentLayoutIndex, layoutTypes, IM_ARRAYSIZE(layoutTypes)))
                {

                    if (s.layoutManager != nullptr) {
                        delete s.layoutManager;
                        s.layoutManager = nullptr;
                    }
                    if (currentLayoutIndex == 1) {
                        s.layoutManager = new VerticalLayout();
                    }
                    else if (currentLayoutIndex == 2) {
                        s.layoutManager = new HorizontalLayout();
                    }

                    MarkSceneUpdated();
                }
                ImGui::PopItemWidth();


                if (s.layoutManager != nullptr) {
                    ImGui::Indent();
                    if (ImGui::DragFloat("Spacing##Layout", &s.layoutManager->spacing, 0.5f, 0.0f, 10000.0f)) MarkSceneUpdated();
                    if (ImGui::DragFloat("Padding Top##Layout", &s.layoutManager->paddingTop, 0.5f, 0.0f, 10000.0f)) MarkSceneUpdated();
                    if (ImGui::DragFloat("Padding Bottom##Layout", &s.layoutManager->paddingBottom, 0.5f, 0.0f, 10000.0f)) MarkSceneUpdated();
                    if (ImGui::DragFloat("Padding Left##Layout", &s.layoutManager->paddingLeft, 0.5f, 0.0f, 10000.0f)) MarkSceneUpdated();
                    if (ImGui::DragFloat("Padding Right##Layout", &s.layoutManager->paddingRight, 0.5f, 0.0f, 10000.0f)) MarkSceneUpdated();
                    ImGui::Unindent();
                }

                ImGui::EndDisabled();
            }




            bool showChildSettings = (s.parent != nullptr && s.parent->isLayoutContainer && s.parent->layoutManager != nullptr);
            if (showChildSettings)
            {
                if (ImGui::CollapsingHeader("Layout Çocuk Ayarları", ImGuiTreeNodeFlags_DefaultOpen))
                {

                    if (ImGui::DragFloat("Esneme Faktörü (Stretch)##LayoutChild", &s.stretchFactor, 0.1f, 0.0f, 10000.0f)) {
                        s.stretchFactor = std::max(0.0f, s.stretchFactor);
                        MarkSceneUpdated();
                    }
                    if (ImGui::IsItemHovered()) ImGui::SetTooltip("0: Esneme yok.\n>0: Diğer esneyen elemanlarla orantılı olarak boş alanı doldurur.");


                    const char* hAlignItems[] = { "Doldur (Fill)", "Sol (Left)", "Orta (Center)", "Sağ (Right)" };
                    int currentHAlign = static_cast<int>(s.horizontalAlignment);
                    if (ImGui::Combo("Yatay Hizalama##LayoutChild", &currentHAlign, hAlignItems, IM_ARRAYSIZE(hAlignItems))) {
                        s.horizontalAlignment = static_cast<HAlignment>(currentHAlign);
                        MarkSceneUpdated();
                    }


                    const char* vAlignItems[] = { "Doldur (Fill)", "Üst (Top)", "Orta (Center)", "Alt (Bottom)" };
                    int currentVAlign = static_cast<int>(s.verticalAlignment);
                    if (ImGui::Combo("Dikey Hizalama##LayoutChild", &currentVAlign, vAlignItems, IM_ARRAYSIZE(vAlignItems))) {
                        s.verticalAlignment = static_cast<VAlignment>(currentVAlign);
                        MarkSceneUpdated();
                    }
                }
            }


            if (ImGui::CollapsingHeader("Animations (Button)", ImGuiTreeNodeFlags_DefaultOpen)) {
                int anim_to_delete = -1;
                for (int aIdx = 0; aIdx < (int)s.onClickAnimations.size(); aIdx++) {
                    ImGui::PushID(aIdx + 5000);
                    ButtonAnimation& anim = s.onClickAnimations[aIdx];
                    const char* animLabel = (anim.name.empty()) ? "[Unnamed Animation]" : anim.name.c_str();
                    if (ImGui::TreeNode(animLabel)) {
                        char animNameBuf[128]; strncpy(animNameBuf, anim.name.c_str(), 127); animNameBuf[127] = '\0'; if (ImGui::InputText("Name##Anim", animNameBuf, 128)) { anim.name = animNameBuf; MarkSceneUpdated(); }
                        if (ImGui::DragFloat("Duration##Anim", &anim.duration, 0.05f, 0.01f, 30.0f, "%.2f s")) MarkSceneUpdated();
                        if (ImGui::DragFloat("Speed##Anim", &anim.speed, 0.1f, 0.1f, 10.0f)) MarkSceneUpdated();
                        if (ImGui::DragFloat2("Target Pos Offset##Anim", (float*)&anim.animationTargetPosition, 0.5f)) MarkSceneUpdated();
                        if (ImGui::DragFloat2("Target Size Offset##Anim", (float*)&anim.animationTargetSize, 0.5f)) MarkSceneUpdated();
                        float targetRotDeg = anim.transformRotation; if (ImGui::DragFloat("Target Rot Offset##Anim", &targetRotDeg, 1.0f, -720, 720, "%.1f deg")) { anim.transformRotation = targetRotDeg; MarkSceneUpdated(); }
                        const char* interpItems[] = { "Linear", "EaseInOut" }; int interpIdx = (int)anim.interpolationMethod; if (ImGui::Combo("Interpolation##Anim", &interpIdx, interpItems, 2)) { anim.interpolationMethod = (ButtonAnimation::InterpolationMethod)interpIdx; MarkSceneUpdated(); }
                        const char* triggerItems[] = { "OnClick", "OnHover" }; int trigIdx = (int)anim.triggerMode; if (ImGui::Combo("Trigger##Anim", &trigIdx, triggerItems, 2)) { anim.triggerMode = (ButtonAnimation::TriggerMode)trigIdx; MarkSceneUpdated(); }
                        const char* behaviorItems[] = { "Play Once & Stay", "Play Once & Reverse", "Toggle", "Hold & Reverse", "Hold & Stay" }; int behIdx = (int)anim.behavior; if (ImGui::Combo("Behavior##Anim", &behIdx, behaviorItems, 5)) { anim.behavior = (ButtonAnimation::AnimationBehavior)behIdx; MarkSceneUpdated(); }
                        if (ImGui::InputInt("Repeat (0=inf)##Anim", &anim.repeatCount)) MarkSceneUpdated();
                        ImGui::ProgressBar(anim.progress, ImVec2(-FLT_MIN, 0));
                        if (ImGui::Button("[X] Remove Animation##Anim")) { anim_to_delete = aIdx; }
                        ImGui::TreePop();
                    }
                    ImGui::PopID();
                }
                if (anim_to_delete != -1) { s.onClickAnimations.erase(s.onClickAnimations.begin() + anim_to_delete); MarkSceneUpdated(); }
                if (ImGui::Button("[+] Add Animation##Anim")) {
                    s.onClickAnimations.push_back({});
                    s.onClickAnimations.back().name = "New Animation";
                    MarkSceneUpdated();
                }
            }


            if (ImGui::CollapsingHeader("Shape Keys (Responsive)", ImGuiTreeNodeFlags_DefaultOpen)) {
                int key_to_delete = -1;
                for (int iKey = 0; iKey < (int)s.shapeKeys.size(); iKey++) {
                    ImGui::PushID(iKey + 6000);
                    auto& sk = s.shapeKeys[iKey];
                    const char* keyLabel = (sk.name.empty()) ? "[Unnamed Key]" : sk.name.c_str();
                    if (ImGui::TreeNode(keyLabel)) {
                        char nameBuf[128]; strncpy(nameBuf, sk.name.c_str(), 127); nameBuf[127] = '\0'; if (ImGui::InputText("Name##SK", nameBuf, 128)) { sk.name = nameBuf; MarkSceneUpdated(); }
                        const char* types[] = { "SizeX", "SizeY", "PositionX", "PositionY", "Rotation" }; int currentType = (int)sk.type; if (ImGui::Combo("Type##SK", &currentType, types, 5)) { sk.type = (ShapeKeyType)currentType; MarkSceneUpdated(); }
                        if (ImGui::DragFloat2("Start Window Size##SK", (float*)&sk.startWindowSize, 5.0f, 0.0f, 8000.0f, "%.0f")) MarkSceneUpdated();
                        if (ImGui::DragFloat2("End Window Size##SK", (float*)&sk.endWindowSize, 5.0f, 0.0f, 8000.0f, "%.0f")) MarkSceneUpdated();

                        if (sk.type == ShapeKeyType::Rotation) {
                            float targetRotDeg = sk.targetRotation; if (ImGui::DragFloat("Target Rot##SK", &targetRotDeg, 1.0f, -720, 720, "%.1f deg")) { sk.targetRotation = targetRotDeg; MarkSceneUpdated(); }
                            float rotOffDeg = sk.rotationOffset; if (ImGui::DragFloat("Rot Offset##SK", &rotOffDeg, 1.0f, -720, 720, "%.1f deg")) { sk.rotationOffset = rotOffDeg; MarkSceneUpdated(); }
                        }
                        else {
                            if (ImGui::DragFloat2("Target Value##SK", (float*)&sk.targetValue, 0.5f)) MarkSceneUpdated();
                            if (ImGui::DragFloat2("Offset##SK", (float*)&sk.offset, 0.5f)) MarkSceneUpdated();
                        }
                        char progressText[32];
                        snprintf(progressText, sizeof(progressText), "%.1f%%", sk.value * 100.0f);
                        ImGui::ProgressBar(sk.value, ImVec2(-FLT_MIN, 0), progressText);
                        if (ImGui::Button("[X] Remove Key##SK")) { key_to_delete = iKey; }
                        ImGui::TreePop();
                    }
                    ImGui::PopID();
                }
                if (key_to_delete != -1) { s.shapeKeys.erase(s.shapeKeys.begin() + key_to_delete); MarkSceneUpdated(); }
                if (ImGui::Button("[+] Add Shape Key##SK")) {
                    s.shapeKeys.push_back({});
                    s.shapeKeys.back().name = "New Key";
                    MarkSceneUpdated();
                }
                ImGui::Separator();
                ImGui::Text("Base Offsets for Keys:");
                if (ImGui::DragFloat2("Pos##SKBase", (float*)&s.baseKeyOffset, 0.5f)) MarkSceneUpdated();
                if (ImGui::DragFloat2("Size##SKBase", (float*)&s.baseKeySizeOffset, 0.5f)) MarkSceneUpdated();
                float baseOffRotDeg = s.baseKeyRotationOffset; if (ImGui::DragFloat("Rot##SKBase", &baseOffRotDeg, 1.0f, -720, 720, "%.1f deg")) { s.baseKeyRotationOffset = baseOffRotDeg; MarkSceneUpdated(); }
                if (ImGui::Button("Reset Base Offsets##SK")) { s.baseKeyOffset = ImVec2(0, 0); s.baseKeySizeOffset = ImVec2(0, 0); s.baseKeyRotationOffset = 0; MarkSceneUpdated(); }
            }


            if (ImGui::CollapsingHeader("Event Handlers", ImGuiTreeNodeFlags_DefaultOpen)) {
                int handler_to_delete = -1;
                for (int i = 0; i < s.eventHandlers.size(); i++) {
                    ImGui::PushID(i + 7000);
                    auto& handler = s.eventHandlers[i];
                    ImGui::BulletText("'%s' -> %s", handler.eventType.c_str(), handler.name.c_str()); ImGui::SameLine();
                    if (ImGui::SmallButton("[X]##DelHandler")) { handler_to_delete = i; }
                    ImGui::PopID();
                }
                if (handler_to_delete != -1) { s.eventHandlers.erase(s.eventHandlers.begin() + handler_to_delete); MarkSceneUpdated(); }

                static char eventTypeBuffer[64] = "onClick";
                static char handlerNameBuffer[64] = "myHandler";
                ImGui::InputText("Event Type##Add", eventTypeBuffer, 64); ImGui::SameLine();
                ImGui::InputText("Handler Name##Add", handlerNameBuffer, 64); ImGui::SameLine();
                if (ImGui::Button("[+]##AddHandler")) {
                    if (strlen(eventTypeBuffer) > 0 && strlen(handlerNameBuffer) > 0) {
                        s.eventHandlers.push_back({ eventTypeBuffer, handlerNameBuffer, nullptr });
                        MarkSceneUpdated();
                    }
                }
                ImGui::TextDisabled("Note: Handler function must be assigned in code via name lookup.");
            }

            ImGui::EndDisabled();

        }
        else if (selectedShapes.size() > 1)
        {

            ImGui::Text("%zu Shapes Selected", selectedShapes.size());
            ImGui::Separator();


            bool anyLocked = std::any_of(selectedShapes.begin(), selectedShapes.end(), [](ShapeItem* s) {
                if (!s) return false;
                int layerIdx = FindShapeLayerIndex(s->id);
                bool layerLocked = false;
                if (g_windowsMap.count(selectedGuiWindow) && layerIdx != -1) {
                    WindowData& currentWindowData = g_windowsMap.at(selectedGuiWindow);
                    if (layerIdx >= 0 && layerIdx < currentWindowData.layers.size()) layerLocked = currentWindowData.layers[layerIdx].locked;
                }
                return s->locked || layerLocked;
                });

            if (anyLocked) ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "Multi-edit disabled (some shapes/layers locked)");



            static bool needsRotationSample = true;
            static bool needsColorSample = true;
            static bool needsMinMaxSizeSample = true;


            static bool multiRotOffsetMode = false;
            static bool multiColOffsetMode = false;


            static float multiRotation = 0.0f;
            static ImVec4 multiFillColor = ImVec4(1, 1, 1, 1);
            static ImVec2 multiMinSize = ImVec2(0, 0);
            static ImVec2 multiMaxSize = ImVec2(99999, 99999);


            static float multiDeltaPos[2] = { 0.0f, 0.0f };
            static float multiDeltaSize[2] = { 0.0f, 0.0f };
            static float multiDeltaRot = 0.0f;
            static float multiDeltaColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };


            static bool isDraggingRot = false;
            static bool isDraggingCol = false;


            static std::vector<float> initialBaseRotations;
            static std::vector<ImVec4> initialColors;


            static float multiDeltaRotAccum = 0.0f;
            static float multiDeltaColAccum[4] = { 0,0,0,0 };


            ImGui::BeginDisabled(anyLocked);


            static std::vector<int> lastSelectedShapeIds;
            std::vector<int> currentSelectedShapeIds;
            for (const auto* s : selectedShapes) { if (s) currentSelectedShapeIds.push_back(s->id); }
            std::sort(currentSelectedShapeIds.begin(), currentSelectedShapeIds.end());

            bool selectionChanged = (currentSelectedShapeIds != lastSelectedShapeIds);
            if (selectionChanged) {
                lastSelectedShapeIds = currentSelectedShapeIds;
                needsRotationSample = true;
                needsColorSample = true;
                needsMinMaxSizeSample = true;

                isDraggingRot = isDraggingCol = false;
                multiDeltaRotAccum = 0.0f;
                multiDeltaColAccum[0] = multiDeltaColAccum[1] = multiDeltaColAccum[2] = multiDeltaColAccum[3] = 0.0f;
            }


            ImGui::PushID("MultiPos");

            if (ImGui::DragFloat2("Delta Position", multiDeltaPos, 0.5f))
            {

                if (fabs(multiDeltaPos[0]) > 1e-6f || fabs(multiDeltaPos[1]) > 1e-6f)
                {
                    for (ShapeItem* shape : selectedShapes)
                    {
                        if (!shape || shape->locked || anyLocked) continue;

                        shape->basePosition.x += multiDeltaPos[0];
                        shape->basePosition.y += multiDeltaPos[1];

                    }
                    MarkSceneUpdated();

                    multiDeltaPos[0] = multiDeltaPos[1] = 0.0f;
                }

                else if (multiDeltaPos[0] == 0.0f && multiDeltaPos[1] == 0.0f) {

                }
            }
            ImGui::PopID();



            ImGui::PushID("MultiSize");
            if (ImGui::DragFloat2("Delta Size", multiDeltaSize, 0.5f))
            {

                if (fabs(multiDeltaSize[0]) > 1e-6f || fabs(multiDeltaSize[1]) > 1e-6f)
                {
                    for (ShapeItem* shape : selectedShapes)
                    {
                        if (!shape || shape->locked || anyLocked) continue;

                        ImVec2 targetBaseSize = shape->baseSize;
                        targetBaseSize.x += multiDeltaSize[0];
                        targetBaseSize.y += multiDeltaSize[1];


                        targetBaseSize.x = std::max(shape->minSize.x, std::min(targetBaseSize.x, shape->maxSize.x));
                        targetBaseSize.y = std::max(shape->minSize.y, std::min(targetBaseSize.y, shape->maxSize.y));


                        shape->baseSize = targetBaseSize;

                    }
                    MarkSceneUpdated();

                    multiDeltaSize[0] = multiDeltaSize[1] = 0.0f;
                }
                else if (multiDeltaSize[0] == 0.0f && multiDeltaSize[1] == 0.0f) {

                }
            }
            ImGui::PopID();



            static bool lastMultiRotOffsetMode = multiRotOffsetMode;
            if (ImGui::Checkbox("Offset Mode (Rotation)", &multiRotOffsetMode)) {
                if (!multiRotOffsetMode && lastMultiRotOffsetMode) { needsRotationSample = true; }
                lastMultiRotOffsetMode = multiRotOffsetMode;
            }

            if (!multiRotOffsetMode) {
                if (needsRotationSample && !selectedShapes.empty()) {
                    ShapeItem* firstValidShape = nullptr;
                    for (auto* s : selectedShapes) { if (s && !s->locked && !anyLocked) { firstValidShape = s; break; } }
                    if (firstValidShape) { multiRotation = firstValidShape->baseRotation * (180.0f / IM_PI); }
                    else { multiRotation = 0.0f; }
                    needsRotationSample = false;
                }
                ImGui::PushID("MultiRotAbs");
                if (ImGui::DragFloat("Set Rotation", &multiRotation, 1.0f, -720, 720, "%.1f deg")) {
                    float rotationInRadians = multiRotation * (IM_PI / 180.0f);
                    for (auto* shape : selectedShapes) { if (shape && !shape->locked && !anyLocked) { shape->baseRotation = rotationInRadians; shape->rotation = rotationInRadians; } }
                    MarkSceneUpdated();
                    needsRotationSample = false;
                }
                ImGui::PopID();
            }
            else {
                ImGui::PushID("MultiRotDelta");
                if (ImGui::DragFloat("Delta Rotation", &multiDeltaRot, 1.0f, -360.0f, 360.0f, "%+.1f deg")) {
                    if (ImGui::IsItemActivated()) {
                        isDraggingRot = true; multiDeltaRotAccum = 0.0f; initialBaseRotations.clear();
                        for (auto* shape : selectedShapes) { initialBaseRotations.push_back((shape && !shape->locked && !anyLocked) ? shape->baseRotation : NAN); }
                    }
                    if (isDraggingRot && fabs(multiDeltaRot) > 1e-6) {
                        multiDeltaRotAccum += multiDeltaRot * (IM_PI / 180.0f);
                        for (size_t i = 0; i < selectedShapes.size(); ++i) { if (i < initialBaseRotations.size() && selectedShapes[i] && !isnan(initialBaseRotations[i])) { selectedShapes[i]->rotation = initialBaseRotations[i] + multiDeltaRotAccum; } }
                        MarkSceneUpdated();
                    }
                    multiDeltaRot = 0.0f;
                }
                else if (isDraggingRot && !ImGui::IsItemActive()) {
                    isDraggingRot = false;
                    for (size_t i = 0; i < selectedShapes.size(); ++i) { if (i < initialBaseRotations.size() && selectedShapes[i] && !isnan(initialBaseRotations[i])) { selectedShapes[i]->baseRotation = initialBaseRotations[i] + multiDeltaRotAccum; selectedShapes[i]->rotation = selectedShapes[i]->baseRotation; } }
                    multiDeltaRotAccum = 0.0f; multiDeltaRot = 0.0f; initialBaseRotations.clear(); MarkSceneUpdated();
                }
                ImGui::PopID();
            }



            static bool lastMultiColOffsetMode = multiColOffsetMode;
            if (ImGui::Checkbox("Offset Mode (Fill Color)", &multiColOffsetMode)) {
                if (!multiColOffsetMode && lastMultiColOffsetMode) { needsColorSample = true; }
                lastMultiColOffsetMode = multiColOffsetMode;
            }

            if (!multiColOffsetMode) {
                if (needsColorSample && !selectedShapes.empty()) {
                    ShapeItem* firstValidShape = nullptr;
                    for (auto* s : selectedShapes) { if (s && !s->locked && !anyLocked) { firstValidShape = s; break; } }
                    if (firstValidShape) { multiFillColor = firstValidShape->fillColor; }
                    else { multiFillColor = ImVec4(1, 1, 1, 1); }
                    needsColorSample = false;
                }
                ImGui::PushID("MultiColorAbs");
                if (ImGui::ColorEdit4("Set Fill Color", (float*)&multiFillColor, ImGuiColorEditFlags_AlphaBar)) {
                    for (auto* shape : selectedShapes) { if (shape && !shape->locked && !anyLocked) { shape->fillColor = multiFillColor; } }
                    MarkSceneUpdated();
                    needsColorSample = false;
                }
                ImGui::PopID();
            }
            else {
                ImGui::PushID("MultiColorDelta");
                if (ImGui::DragFloat4("Delta Fill Color", multiDeltaColor, 0.01f, -1.0f, 1.0f)) {
                    if (ImGui::IsItemActivated()) {
                        isDraggingCol = true; for (int k = 0; k < 4; ++k) multiDeltaColAccum[k] = 0.0f; initialColors.clear();
                        for (auto* shape : selectedShapes) { initialColors.push_back((shape && !shape->locked && !anyLocked) ? shape->fillColor : ImVec4(NAN, NAN, NAN, NAN)); }
                    }
                    bool changed = false; for (int i = 0; i < 4; ++i) { if (fabs(multiDeltaColor[i]) > 1e-6) { changed = true; break; } }
                    if (isDraggingCol && changed) {
                        for (int k = 0; k < 4; ++k) multiDeltaColAccum[k] += multiDeltaColor[k];
                        for (size_t i = 0; i < selectedShapes.size(); ++i) {
                            if (i < initialColors.size() && selectedShapes[i] && !isnan(initialColors[i].x)) {
                                selectedShapes[i]->fillColor.x = std::max(0.0f, std::min(1.0f, initialColors[i].x + multiDeltaColAccum[0]));
                                selectedShapes[i]->fillColor.y = std::max(0.0f, std::min(1.0f, initialColors[i].y + multiDeltaColAccum[1]));
                                selectedShapes[i]->fillColor.z = std::max(0.0f, std::min(1.0f, initialColors[i].z + multiDeltaColAccum[2]));
                                selectedShapes[i]->fillColor.w = std::max(0.0f, std::min(1.0f, initialColors[i].w + multiDeltaColAccum[3]));
                            }
                        }
                        MarkSceneUpdated();
                    }
                    multiDeltaColor[0] = multiDeltaColor[1] = multiDeltaColor[2] = multiDeltaColor[3] = 0.0f;
                }
                else if (isDraggingCol && !ImGui::IsItemActive()) {
                    isDraggingCol = false; multiDeltaColAccum[0] = multiDeltaColAccum[1] = multiDeltaColAccum[2] = multiDeltaColAccum[3] = 0.0f;
                    multiDeltaColor[0] = multiDeltaColor[1] = multiDeltaColor[2] = multiDeltaColor[3] = 0.0f; initialColors.clear();
                }
                ImGui::PopID();
            }



            ImGui::SeparatorText("Layout & Constraints (Multi-Edit)");


            ShapeItem::AnchorMode firstAnchor = ShapeItem::AnchorMode::None; bool foundFirst = false;
            bool mixedAnchor = false;
            const char* anchorModes[] = { "None", "TopLeft", "Top", "TopRight", "Left", "Center", "Right", "BottomLeft", "Bottom", "BottomRight" };
            int currentAnchorIndex = mixedAnchor ? -1 : static_cast<int>(firstAnchor);
            const char* previewText = mixedAnchor ? "[Mixed]" : ((currentAnchorIndex >= 0 && currentAnchorIndex < IM_ARRAYSIZE(anchorModes)) ? anchorModes[currentAnchorIndex] : "[Error]");
            ImGui::PushItemWidth(-FLT_MIN * 0.6f);
            if (ImGui::BeginCombo("Set Anchor", previewText, ImGuiComboFlags_None)) {
                for (int n = 0; n < IM_ARRAYSIZE(anchorModes); n++) {
                    bool is_selected = (!mixedAnchor && n == currentAnchorIndex); if (ImGui::Selectable(anchorModes[n], is_selected)) {
                        ShapeItem::AnchorMode newAnchor = static_cast<ShapeItem::AnchorMode>(n);
                        for (auto* shape : selectedShapes) { if (shape && !shape->locked && !anyLocked) { shape->anchorMode = newAnchor; if (newAnchor != ShapeItem::AnchorMode::None) shape->usePercentagePos = false; } }
                        MarkSceneUpdated();
                    } if (is_selected) ImGui::SetItemDefaultFocus();
                } ImGui::EndCombo();
            } ImGui::PopItemWidth();



            if (needsMinMaxSizeSample && !selectedShapes.empty()) {
                ShapeItem* firstValidShape = nullptr; for (auto* s : selectedShapes) { if (s && !s->locked && !anyLocked) { firstValidShape = s; break; } }
                if (firstValidShape) { multiMinSize = firstValidShape->minSize; multiMaxSize = firstValidShape->maxSize; }
                else { multiMinSize = ImVec2(0, 0); multiMaxSize = ImVec2(99999, 99999); }
                needsMinMaxSizeSample = false;
            }
            bool minChanged = false; bool maxChanged = false;
            ImGui::PushItemWidth(-FLT_MIN * 0.6f);
            if (ImGui::DragFloat2("Set Min Size", (float*)&multiMinSize, 1.0f, 0.0f, 99999.0f, "%.0f")) {
                multiMinSize.x = std::max(0.f, multiMinSize.x); multiMinSize.y = std::max(0.f, multiMinSize.y); multiMaxSize.x = std::max(multiMinSize.x, multiMaxSize.x); multiMaxSize.y = std::max(multiMinSize.y, multiMaxSize.y);
                minChanged = true; needsMinMaxSizeSample = false;
            }
            if (ImGui::DragFloat2("Set Max Size", (float*)&multiMaxSize, 1.0f, 0.0f, 99999.0f, "%.0f")) {
                multiMaxSize.x = std::max(0.f, multiMaxSize.x); multiMaxSize.y = std::max(0.f, multiMaxSize.y); multiMaxSize.x = std::max(multiMinSize.x, multiMaxSize.x); multiMaxSize.y = std::max(multiMinSize.y, multiMaxSize.y);
                maxChanged = true; needsMinMaxSizeSample = false;
            }
            ImGui::PopItemWidth();
            if (minChanged || maxChanged) {
                for (auto* shape : selectedShapes) { if (!shape || shape->locked || anyLocked) continue; if (minChanged) shape->minSize = multiMinSize; if (maxChanged) shape->maxSize = multiMaxSize; shape->maxSize.x = std::max(shape->minSize.x, shape->maxSize.x); shape->maxSize.y = std::max(shape->minSize.y, shape->maxSize.y); shape->size.x = std::max(shape->minSize.x, std::min(shape->size.x, shape->maxSize.x)); shape->size.y = std::max(shape->minSize.y, std::min(shape->size.y, shape->maxSize.y)); shape->baseSize.x = std::max(shape->minSize.x, std::min(shape->baseSize.x, shape->maxSize.x)); shape->baseSize.y = std::max(shape->minSize.y, std::min(shape->baseSize.y, shape->maxSize.y)); }
                MarkSceneUpdated();
            }

            ImGui::EndDisabled();

        }
        else
        {
            ImGui::TextUnformatted("Select a layer or shape to see properties.");
        }

        ImGui::EndChild();
    }


    inline void ShowUI_CodeGenerationPanel() {
        ImGui::BeginChild("CodeGenerationPanel");

        ImGui::Text("Code Generation");
        ImGui::Separator();

        if (ImGui::Button(("Generate Code for Window: " + selectedGuiWindow).c_str())) {
            generatedCodeForWindow = GenerateCodeForWindow(selectedGuiWindow);
        }
        if (!generatedCodeForWindow.empty()) {
            ImGui::InputTextMultiline("##WindowCode", &generatedCodeForWindow[0], generatedCodeForWindow.size() + 1,
                ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 10), ImGuiInputTextFlags_ReadOnly);
            if (ImGui::Button("Copy Window Code")) {
                CopyToClipboard(generatedCodeForWindow);
            }
        }

        ImGui::Separator();

        ImGui::BeginDisabled(selectedShapes.size() != 1);
        if (ImGui::Button("Generate Code for Selected Shape")) {
            if (selectedShapes.size() == 1) {
                generatedCodeForSingleShape = GenerateSingleShapeCode(*selectedShapes[0]);
            }
        }
        ImGui::EndDisabled();
        if (!generatedCodeForSingleShape.empty()) {
            ImGui::InputTextMultiline("##ShapeCode", &generatedCodeForSingleShape[0], generatedCodeForSingleShape.size() + 1,
                ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 8), ImGuiInputTextFlags_ReadOnly);
            if (ImGui::Button("Copy Shape Code")) {
                CopyToClipboard(generatedCodeForSingleShape);
            }
        }

        ImGui::Separator();

        ImGui::BeginDisabled(!(selectedShapes.size() == 1 && selectedShapes[0]->isButton));
        if (ImGui::Button("Generate Code for Selected Button (.h/.cpp)")) {
            if (selectedShapes.size() == 1 && selectedShapes[0]->isButton) {
                generatedCodeForButton = GenerateCodeForSingleButton(*selectedShapes[0]);
            }
        }
        ImGui::EndDisabled();
        if (!generatedCodeForButton.empty()) {
            ImGui::InputTextMultiline("##ButtonCode", &generatedCodeForButton[0], generatedCodeForButton.size() + 1,
                ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 10), ImGuiInputTextFlags_ReadOnly);
            if (ImGui::Button("Copy Button Code")) {
                CopyToClipboard(generatedCodeForButton);
            }
        }


        ImGui::EndChild();
    }



    inline void ShowUI_ComponentPanel() {
        ImGui::BeginChild("ComponentPanel");
        ImGui::Text("Component Management");
        ImGui::Separator();

        static char newComponentNameBuffer[128] = "";
        ImGui::InputText("Component Name##New", newComponentNameBuffer, sizeof(newComponentNameBuffer));
        ImGui::SameLine();
        ImGui::BeginDisabled(selectedShapes.empty() || strlen(newComponentNameBuffer) == 0);
        if (ImGui::Button("Save Selection as Component")) {
            if (g_componentDefinitions.find(newComponentNameBuffer) == g_componentDefinitions.end()) {
                ComponentDefinition newComp;
                newComp.name = newComponentNameBuffer;
                ImVec2 minPos = ImVec2(FLT_MAX, FLT_MAX);
                std::set<int> selectedShapeIds;
                for (const auto* shapePtr : selectedShapes) {
                    minPos.x = std::min(minPos.x, shapePtr->position.x);
                    minPos.y = std::min(minPos.y, shapePtr->position.y);
                    selectedShapeIds.insert(shapePtr->id);
                }
                for (const auto* shapePtr : selectedShapes) {
                    ComponentShapeTemplate shapeTemplate;
                    shapeTemplate.item = *shapePtr;
                    shapeTemplate.originalId = shapePtr->id;
                    shapeTemplate.item.position = shapePtr->position - minPos;
                    shapeTemplate.item.basePosition = shapeTemplate.item.position;
                    if (shapePtr->parent != nullptr && selectedShapeIds.count(shapePtr->parent->id)) {
                        shapeTemplate.originalParentId = shapePtr->parent->id;
                    }
                    else {
                        shapeTemplate.originalParentId = -1;
                    }
                    shapeTemplate.item.parent = nullptr;
                    shapeTemplate.item.children.clear();
                    shapeTemplate.item.isPressed = false;
                    shapeTemplate.item.isHeld = false;
                    shapeTemplate.item.isAnimating = false;
                    shapeTemplate.item.currentAnimation = nullptr;
                    shapeTemplate.item.id = -1;


                    newComp.shapeTemplates.push_back(shapeTemplate);
                }
                g_componentDefinitions[newComp.name] = newComp;
                strncpy(newComponentNameBuffer, "", sizeof(newComponentNameBuffer));
            }
            else {
                ImGui::OpenPopup("Component Name Exists");
            }
        }
        ImGui::EndDisabled();

        if (ImGui::BeginPopupModal("Component Name Exists", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("A component with this name already exists.\nPlease choose a different name.");
            if (ImGui::Button("OK")) { ImGui::CloseCurrentPopup(); }
            ImGui::EndPopup();
        }

        ImGui::Separator();
        ImGui::Text("Defined Components (Drag to Hierarchy):");
        if (g_componentDefinitions.empty()) {
            ImGui::TextDisabled("No components defined yet.");
        }
        else {
            float listHeight = ImGui::GetTextLineHeightWithSpacing() * std::min<float>((float)g_componentDefinitions.size() + 1.0f, 8.0f);
            if (ImGui::BeginListBox("##ComponentList", ImVec2(-FLT_MIN, listHeight))) {
                std::string componentToDelete = "";
                int comp_idx = 0;
                for (auto it = g_componentDefinitions.begin(); it != g_componentDefinitions.end(); ) {
                    const auto& name = it->first;
                    ImGui::PushID(comp_idx);
                    ImGui::Selectable(name.c_str(), false, ImGuiSelectableFlags_AllowItemOverlap);

                    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                        const char* componentNameCStr = name.c_str();
                        ImGui::SetDragDropPayload("DESIGNER_COMPONENT", componentNameCStr, strlen(componentNameCStr) + 1);
                        ImGui::Text("Component: %s", name.c_str());
                        ImGui::EndDragDropSource();
                    }

                    float deleteButtonWidth = ImGui::GetFrameHeight();
                    ImGui::SameLine(ImGui::GetContentRegionAvail().x - deleteButtonWidth);
                    if (ImGui::Button(("[X]##DelComp" + std::to_string(comp_idx)).c_str(), ImVec2(deleteButtonWidth, deleteButtonWidth))) {
                        componentToDelete = name;
                    }

                    ImGui::PopID();
                    comp_idx++;
                    if (componentToDelete != name) {
                        ++it;
                    }
                    else {
                        it = g_componentDefinitions.erase(it);
                        componentToDelete = "";
                    }
                }
                ImGui::EndListBox();
            }
        }
        ImGui::EndChild();
    }
    inline void ShowUI_LayerShapeManager_ChildWindowMappings()
    {
        if (ImGui::CollapsingHeader("Child Window Mappings", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::SeparatorText("Define how buttons control child window visibility");
            int mapping_to_delete = -1;
            int mappingIndex = 0;

            for (auto& mapping : g_combinedChildWindowMappings)
            {
                ImGui::PushID(mappingIndex);
                ImGui::Separator();

                std::vector<ShapeItem*> allShapes = GetAllShapes();
                int currentShapeIndex = -1;
                for (int i = 0; i < (int)allShapes.size(); i++) {
                    if (allShapes[i]->id == mapping.shapeId) {
                        currentShapeIndex = i;
                        break;
                    }
                }

                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.8f);
                if (ImGui::Combo("Container Shape", &currentShapeIndex, [](void* data, int idx, const char** out_text) -> bool {
                    auto* vec = static_cast<std::vector<ShapeItem*>*>(data);
                    if (idx >= 0 && idx < (int)vec->size()) {
                        *out_text = (*vec)[idx]->name.c_str();
                        return true;
                    }
                    *out_text = "[Invalid Index]";
                    return false;
                    }, static_cast<void*>(&allShapes), (int)allShapes.size()))
                {
                    if (currentShapeIndex >= 0) {
                        mapping.shapeId = allShapes[currentShapeIndex]->id;
                        allShapes[currentShapeIndex]->isChildWindow = true;
                        MarkSceneUpdated();
                    }
                }
                ImGui::SameLine();
                if (ImGui::Button(("[X]##DelMap" + std::to_string(mappingIndex)).c_str())) {
                    mapping_to_delete = mappingIndex;
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
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.8f);
                if (ImGui::Combo("Logic Operator", &currentOpIndex, opOptions, opCount)) {
                    mapping.logicOp = opOptions[currentOpIndex];
                    MarkSceneUpdated();
                }

                ImGui::Text("Button -> Window Pairs:");
                ImGui::Indent();
                int pair_to_delete = -1;
                for (size_t j = 0; j < mapping.buttonIds.size(); j++)
                {
                    ImGui::PushID((int)j);
                    std::vector<ShapeItem*> availableButtons = GetAllButtonShapes();
                    availableButtons.insert(availableButtons.begin(), nullptr);

                    int currentButtonIndex = 0;
                    if (mapping.buttonIds[j] != -1) {
                        for (int i = 1; i < (int)availableButtons.size(); i++) {
                            if (availableButtons[i] && availableButtons[i]->id == mapping.buttonIds[j]) {
                                currentButtonIndex = i;
                                break;
                            }
                        }
                    }

                    float pairComboWidth = (ImGui::GetContentRegionAvail().x - ImGui::GetFrameHeight() - ImGui::GetStyle().ItemSpacing.x * 2) * 0.5f - 5;

                    ImGui::SetNextItemWidth(pairComboWidth);
                    if (ImGui::Combo("Button##Pair", &currentButtonIndex, [](void* data, int idx, const char** out_text) -> bool {
                        auto* vec = static_cast<std::vector<ShapeItem*>*>(data);
                        if (idx == 0) {
                            *out_text = "None (Always Active)"; return true;
                        }
                        if (idx > 0 && idx < (int)vec->size() && (*vec)[idx]) {
                            *out_text = (*vec)[idx]->name.c_str(); return true;
                        }
                        *out_text = "[Invalid Button]"; return false;
                        }, static_cast<void*>(&availableButtons), (int)availableButtons.size()))
                    {
                        mapping.buttonIds[j] = (currentButtonIndex == 0) ? -1 : availableButtons[currentButtonIndex]->id;
                        if (mapping.buttonIds[j] == -1) {
                            if (j < mapping.childWindowKeys.size()) {
                                SetWindowOpen(mapping.childWindowKeys[j], true);
                            }
                        }
                        MarkSceneUpdated();
                    }
                    ImGui::SameLine();

                    std::vector<std::string> availableChildWindows;
                    for (auto& [key, winData] : g_windowsMap) availableChildWindows.push_back(key);
                    for (auto sh : GetAllShapes()) {
                        if (std::find(availableChildWindows.begin(), availableChildWindows.end(), sh->name) == availableChildWindows.end()) {
                        }
                    }
                    std::sort(availableChildWindows.begin(), availableChildWindows.end());


                    int currentChildIndex = -1;
                    if (j < mapping.childWindowKeys.size()) {
                        for (int i = 0; i < (int)availableChildWindows.size(); i++) {
                            if (availableChildWindows[i] == mapping.childWindowKeys[j]) {
                                currentChildIndex = i;
                                break;
                            }
                        }
                    }

                    ImGui::SetNextItemWidth(pairComboWidth);
                    if (ImGui::Combo("Window##Pair", &currentChildIndex, [](void* data, int idx, const char** out_text) -> bool {
                        auto* vec = static_cast<std::vector<std::string>*>(data);
                        if (idx >= 0 && idx < (int)vec->size()) {
                            *out_text = (*vec)[idx].c_str();
                            return true;
                        }
                        *out_text = "[Invalid Window]";
                        return false;
                        }, static_cast<void*>(&availableChildWindows), (int)availableChildWindows.size()))
                    {
                        if (currentChildIndex >= 0 && j < mapping.childWindowKeys.size()) {
                            mapping.childWindowKeys[j] = availableChildWindows[currentChildIndex];
                            MarkSceneUpdated();
                        }
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("X##DelPair"))
                    {
                        pair_to_delete = j;
                    }
                    ImGui::PopID();
                }

                if (pair_to_delete != -1) {
                    if (pair_to_delete < mapping.buttonIds.size()) mapping.buttonIds.erase(mapping.buttonIds.begin() + pair_to_delete);
                    if (pair_to_delete < mapping.childWindowKeys.size()) mapping.childWindowKeys.erase(mapping.childWindowKeys.begin() + pair_to_delete);
                    MarkSceneUpdated();
                }

                if (ImGui::Button("+ Add Button/Window Pair##Map"))
                {
                    std::vector<ShapeItem*> availableButtons = GetAllButtonShapes();
                    int defaultButtonId = -1;
                    std::vector<std::string> availableChildWindows;
                    for (auto& [key, winData] : g_windowsMap) availableChildWindows.push_back(key);
                    std::string defaultChild = (!availableChildWindows.empty()) ? availableChildWindows[0] : "";

                    mapping.buttonIds.push_back(defaultButtonId);
                    mapping.childWindowKeys.push_back(defaultChild);
                    MarkSceneUpdated();
                }
                ImGui::Unindent();

                ImGui::PopID();
            }

            if (mapping_to_delete != -1) {
                g_combinedChildWindowMappings.erase(g_combinedChildWindowMappings.begin() + mapping_to_delete);
                MarkSceneUpdated();
            }

            if (ImGui::Button("+ Add New Mapping Rule"))
            {
                std::vector<ShapeItem*> allShapes = GetAllShapes();
                int defaultShapeId = (!allShapes.empty()) ? allShapes[0]->id : 0;
                if (!allShapes.empty()) allShapes[0]->isChildWindow = true;

                std::vector<ShapeItem*> availableButtons = GetAllButtonShapes();
                int defaultButtonId = -1;

                std::vector<std::string> availableChildWindows;
                for (auto& [key, winData] : g_windowsMap) availableChildWindows.push_back(key);
                std::string defaultChild = (!availableChildWindows.empty()) ? availableChildWindows[0] : "";

                CombinedMapping newMapping;
                newMapping.shapeId = defaultShapeId;
                newMapping.logicOp = "None";
                newMapping.buttonIds.push_back(defaultButtonId);
                newMapping.childWindowKeys.push_back(defaultChild);

                g_combinedChildWindowMappings.push_back(newMapping);
                MarkSceneUpdated();
            }
        }
    }

    static bool VerticalSplitter(const char* str_id, float thickness, float height, float* size_left, float* size_right, float min_left, float min_right)
    {
        using namespace ImGui;
        ImGuiWindow* window = GetCurrentWindow();
        ImGuiID id = window->GetID(str_id);
        ImRect bb;
        bb.Min = window->DC.CursorPos;


        ImVec2 splitter_size = ImVec2(thickness, height);
        bb.Max = bb.Min + splitter_size;


        InvisibleButton(str_id, splitter_size);

        bool changed = false;
        if (IsItemActive() && IsMouseDragging(ImGuiMouseButton_Left))
        {
            float mouse_delta_x = GetMouseDragDelta(ImGuiMouseButton_Left).x;


            if (std::abs(mouse_delta_x) > 0.1f)
            {

                float new_size_left = *size_left + mouse_delta_x;
                float new_size_right = *size_right - mouse_delta_x;


                float actual_delta = mouse_delta_x;


                if (new_size_left < min_left) {
                    actual_delta = min_left - *size_left;
                }

                if (new_size_right < min_right) {

                    if (mouse_delta_x < 0) {
                        actual_delta = *size_right - min_right;
                    }

                    else {
                        actual_delta = *size_right - min_right;
                    }
                }





                if (std::abs(actual_delta) > 0.1f) {
                    *size_left += actual_delta;
                    *size_right -= actual_delta;
                    changed = true;


                    ResetMouseDragDelta(ImGuiMouseButton_Left);
                }
                else {


                }

            }
        }


        ImDrawList* draw_list = GetWindowDrawList();
        ImU32 col = GetColorU32(IsItemActive() ? ImGuiCol_ButtonActive : IsItemHovered() ? ImGuiCol_ButtonHovered : ImGuiCol_Separator);
        draw_list->AddRectFilled(bb.Min, bb.Max, col, 0.0f);


        if (IsItemHovered()) {
            SetMouseCursor(ImGuiMouseCursor_ResizeEW);
        }

        return changed;
    }





    inline void ShowUI(GLFWwindow* window) {
        EnsureMainWindowExists();
        WindowData& currentWindowData = g_windowsMap.at(selectedGuiWindow);

        if (currentWindowData.layers.empty()) { selectedLayerIndex = -1; }
        else if (selectedLayerIndex < 0 || selectedLayerIndex >= currentWindowData.layers.size()) { selectedLayerIndex = 0; }

        ImGui::SetNextWindowSize(ImVec2(1000, 700), ImGuiCond_FirstUseEver);
        ImGui::Begin("Design Editor");

        float minPaneWidth = 100.0f;
        float splitterWidth = 6.0f;


        static float leftPaneWidth = 250.0f;
        static float middlePaneWidth = 350.0f;


        ImVec2 availableSize = ImGui::GetContentRegionAvail();
        float availableWidth = availableSize.x;
        float availableHeight = availableSize.y;



        float totalMinWidth = minPaneWidth * 3 + splitterWidth * 2;


        leftPaneWidth = std::max(minPaneWidth, leftPaneWidth);
        middlePaneWidth = std::max(minPaneWidth, middlePaneWidth);


        float maxLeftMiddleSum = availableWidth - minPaneWidth - splitterWidth * 2;
        if (leftPaneWidth + middlePaneWidth > maxLeftMiddleSum) {


            float excess = (leftPaneWidth + middlePaneWidth) - maxLeftMiddleSum;
            float middleReduction = std::min(excess, std::max(0.0f, middlePaneWidth - minPaneWidth));
            middlePaneWidth -= middleReduction;
            excess -= middleReduction;
            if (excess > 0) {
                float leftReduction = std::min(excess, std::max(0.0f, leftPaneWidth - minPaneWidth));
                leftPaneWidth -= leftReduction;
            }

            leftPaneWidth = std::max(minPaneWidth, leftPaneWidth);
            middlePaneWidth = std::max(minPaneWidth, middlePaneWidth);
        }




        float rightPaneWidth = availableWidth - leftPaneWidth - middlePaneWidth - splitterWidth * 2;
        rightPaneWidth = std::max(minPaneWidth, rightPaneWidth);



        float currentTotal = leftPaneWidth + middlePaneWidth + rightPaneWidth + splitterWidth * 2;
        if (currentTotal > availableWidth) {
            float excess = currentTotal - availableWidth;

            float middleReduction = std::min(excess, std::max(0.0f, middlePaneWidth - minPaneWidth));
            middlePaneWidth -= middleReduction;
            excess -= middleReduction;
            if (excess > 0) {
                float leftReduction = std::min(excess, std::max(0.0f, leftPaneWidth - minPaneWidth));
                leftPaneWidth -= leftReduction;
            }

            rightPaneWidth = availableWidth - leftPaneWidth - middlePaneWidth - splitterWidth * 2;
        }





        ImGui::BeginChild("LeftPane", ImVec2(leftPaneWidth, availableHeight), true);
        ShowUI_HierarchyPanel(currentWindowData, selectedLayerIndex, selectedShapes);
        ImGui::EndChild();


        ImGui::SameLine(0.0f, 0.0f);

        if (VerticalSplitter("##Splitter1", splitterWidth, availableHeight, &leftPaneWidth, &middlePaneWidth, minPaneWidth, minPaneWidth))
        {


            rightPaneWidth = availableWidth - leftPaneWidth - middlePaneWidth - splitterWidth * 2;


        }
        ImGui::SameLine(0.0f, 0.0f);



        ImGui::BeginChild("MiddlePane", ImVec2(middlePaneWidth, availableHeight), true);
        ShowUI_PropertiesPanel(currentWindowData, selectedLayerIndex, selectedShapes);
        ImGui::EndChild();


        ImGui::SameLine(0.0f, 0.0f);

        if (VerticalSplitter("##Splitter2", splitterWidth, availableHeight, &middlePaneWidth, &rightPaneWidth, minPaneWidth, minPaneWidth))
        {




        }
        ImGui::SameLine(0.0f, 0.0f);





        ImGui::BeginChild("RightPane", ImVec2(std::max(0.0f, rightPaneWidth), availableHeight), true);
        if (ImGui::BeginTabBar("ExtraTabs")) {
            if (ImGui::BeginTabItem("Code")) { ShowUI_CodeGenerationPanel(); ImGui::EndTabItem(); }
            if (ImGui::BeginTabItem("Components")) { ShowUI_ComponentPanel(); ImGui::EndTabItem(); }
            if (ImGui::BeginTabItem("Mappings")) { ShowUI_LayerShapeManager_ChildWindowMappings(); ImGui::EndTabItem(); }
            ImGui::EndTabBar();
        }
        ImGui::EndChild();




        ImGui::End();

        RenderTemporaryWindows();
        RenderAllRegisteredWindows();
    }

    inline std::string SaveConfiguration() {
        return GenerateCodeForWindow(DesignManager::selectedGuiWindow);
    }



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

        }

        static float sliderValue = 0.5f;
        ImGui::SliderFloat("Example Slider", &sliderValue, 0.0f, 1.0f);
    }

    inline void Init(int width, int height, GLFWwindow* window) {

        selectedLayerIndex = 0;
        selectedShapeIndex = -1;

        if (black_texture_id) {
            glDeleteTextures(1, (GLuint*)&black_texture_id);
            black_texture_id = 0;
        }
        ClearGradientTextureCache();


        if (g_windowsMap.find("Main") == g_windowsMap.end()) {
            g_windowsMap["Main"] = {};


        }


        if (DesignManager::selectedGuiWindow.empty() || g_windowsMap.find(DesignManager::selectedGuiWindow) == g_windowsMap.end()) {
            DesignManager::selectedGuiWindow = "Main";
        }


        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->AddFontDefault();



        RegisterWindow("ChildWindow_1", RenderChildWindowForShape1);








        auto it = g_windowsMap.find(DesignManager::selectedGuiWindow);
        if (it != g_windowsMap.end()) {
            if (it->second.layers.empty()) {


                selectedLayerIndex = -1;
            }
            else {

                selectedLayerIndex = std::max(0, std::min(selectedLayerIndex, (int)it->second.layers.size() - 1));
            }
        }
        else {

            selectedLayerIndex = -1;
        }
        selectedShapeIndex = -1;






        MarkSceneUpdated();
    }





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



    class ShapeBuilder {
    public:
        ShapeItem shape;


        DEFINE_SETTER(Id, id)
            DEFINE_SETTER(Name, name)
            DEFINE_SETTER(OwnerWindow, ownerWindow)
            DEFINE_SETTER(GroupId, groupId)
            DEFINE_SETTER(Position, position)
            DEFINE_SETTER(Size, size)
            DEFINE_SETTER(Rotation, rotation)


            DEFINE_SETTER(BasePosition, basePosition)
            DEFINE_SETTER(BaseSize, baseSize)
            DEFINE_SETTER(BaseRotation, baseRotation)


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

            ShapeBuilder& addColorRampEntry(float pos, const ImVec4& color) {
            shape.colorRamp.emplace_back(pos, color);
            return *this;
        }


        DEFINE_SETTER(UseGlass, useGlass)
            DEFINE_SETTER(GlassBlur, glassBlur)
            DEFINE_SETTER(GlassAlpha, glassAlpha)
            DEFINE_SETTER(GlassColor, glassColor)


            DEFINE_SETTER(IsButton, isButton)
            DEFINE_SETTER(ButtonBehavior, buttonBehavior)
            DEFINE_SETTER(HoverColor, hoverColor)
            DEFINE_SETTER(ClickedColor, clickedColor)
            DEFINE_SETTER(UseOnClick, useOnClick)

            ShapeBuilder& addOnClickAnimation(const ButtonAnimation& anim) {
            shape.onClickAnimations.push_back(anim);
            return *this;
        }


        DEFINE_SETTER(HasText, hasText)
            DEFINE_SETTER(Text, text)
            DEFINE_SETTER(TextColor, textColor)
            DEFINE_SETTER(TextSize, textSize)
            DEFINE_SETTER(TextFont, textFont)
            DEFINE_SETTER(TextPosition, textPosition)
            DEFINE_SETTER(TextRotation, textRotation)
            DEFINE_SETTER(TextAlignment, textAlignment)
            DEFINE_SETTER(DynamicTextSize, dynamicTextSize)


            DEFINE_SETTER(ZOrder, zOrder)
            DEFINE_SETTER(UseGradient, useGradient)
            DEFINE_SETTER(GradientRotation, gradientRotation)
            DEFINE_SETTER(GradientInterpolation, gradientInterpolation)


            ShapeBuilder& addShapeKey(const ShapeKey& key) {
            shape.shapeKeys.push_back(key);
            return *this;
        }


        DEFINE_SETTER(IsChildWindow, isChildWindow)
            DEFINE_SETTER(ChildWindowSync, childWindowSync)
            DEFINE_SETTER(ChildWindowGroupId, childWindowGroupId)
            DEFINE_SETTER(ToggleChildWindow, toggleChildWindow)
            DEFINE_SETTER(TargetShapeID, targetShapeID)


            DEFINE_SETTER(IsImGuiContainer, isImGuiContainer)

            ShapeBuilder& setRenderImGuiContent(const std::function<void()>& func) {
            shape.renderImGuiContent = func;
            return *this;
        }


        DEFINE_SETTER(AnchorMode, anchorMode)
            DEFINE_SETTER(AnchorMargin, anchorMargin)
            DEFINE_SETTER(UsePercentagePos, usePercentagePos)
            DEFINE_SETTER(PercentagePos, percentagePos)
            DEFINE_SETTER(UsePercentageSize, usePercentageSize)
            DEFINE_SETTER(PercentageSize, percentageSize)
            DEFINE_SETTER(MinSize, minSize)
            DEFINE_SETTER(MaxSize, maxSize)


            DEFINE_SETTER(UpdateAnimBaseOnResize, updateAnimBaseOnResize)
            DEFINE_SETTER(HasEmbeddedImage, hasEmbeddedImage)
            DEFINE_SETTER(EmbeddedImageIndex, embeddedImageIndex)
            DEFINE_SETTER(AllowItemOverlap, allowItemOverlap)
            DEFINE_SETTER(ForceOverlap, forceOverlap)
            DEFINE_SETTER(BlockUnderlying, blockUnderlying)
            DEFINE_SETTER(ShadowRotation, shadowRotation)
            DEFINE_SETTER(BlurAmount, blurAmount)
            DEFINE_SETTER(Type, type)




            AnimationBuilder createAnimation() {
            return AnimationBuilder();
        }


        static ShapeKeyBuilder createShapeKey() {
            return ShapeKeyBuilder();
        }


        ShapeBuilder& addEventHandler(const std::string& eventType, const std::string& name, const std::function<void(ShapeItem&)>& handler) {
            shape.eventHandlers.push_back({ eventType, name, handler });
            return *this;
        }

        ShapeBuilder& setIsLayoutContainer(bool value) {
            shape.isLayoutContainer = value;





            return *this;
        }


        ShapeBuilder& setLayoutManager(LayoutManager* manager) {


            shape.layoutManager = manager;


            return *this;
        }




        ShapeBuilder& setStretchFactor(float factor) {
            shape.stretchFactor = std::max(0.0f, factor);
            return *this;
        }

        ShapeBuilder& setHorizontalAlignment(HAlignment alignment) {
            shape.horizontalAlignment = alignment;
            return *this;
        }

        ShapeBuilder& setVerticalAlignment(VAlignment alignment) {
            shape.verticalAlignment = alignment;
            return *this;
        }





        ShapeItem build() {


            if (shape.basePosition == ImVec2(0, 0) && shape.position != ImVec2(0, 0)) shape.basePosition = shape.position;
            if (shape.baseSize == ImVec2(0, 0) && shape.size != ImVec2(0, 0)) shape.baseSize = shape.size;
            if (shape.baseRotation == 0.0f && shape.rotation != 0.0f) shape.baseRotation = shape.rotation;


            shape.maxSize.x = std::max(shape.minSize.x, shape.maxSize.x);
            shape.maxSize.y = std::max(shape.minSize.y, shape.maxSize.y);

            shape.size.x = std::max(shape.minSize.x, std::min(shape.size.x, shape.maxSize.x));
            shape.size.y = std::max(shape.minSize.y, std::min(shape.size.y, shape.maxSize.y));

            shape.baseSize.x = std::max(shape.minSize.x, std::min(shape.baseSize.x, shape.maxSize.x));
            shape.baseSize.y = std::max(shape.minSize.y, std::min(shape.baseSize.y, shape.maxSize.y));



            if (shape.anchorMode != ShapeItem::AnchorMode::None) {
                shape.usePercentagePos = false;
            }

            if (shape.isImGuiContainer) {
                shape.isChildWindow = false;
            }
            else if (shape.isChildWindow) {
                shape.isImGuiContainer = false;
            }

            if (!shape.isChildWindow && !shape.isImGuiContainer) {
                shape.isChildWindow = false;
                shape.isImGuiContainer = false;
            }



            if (!shape.isLayoutContainer && shape.layoutManager != nullptr) {

                shape.layoutManager = nullptr;
            }






            return shape;
        }
    };


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

