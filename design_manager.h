// design_manager.h
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
#include <sstream>
#include <set>
#include <limits>
#include <numeric>
#include <iomanip>
#include <memory> 
#include <type_traits>

#include "Images_BINARY.h"
#include "GeneratedButtons.h"

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

namespace DesignManager {
    extern const char* g_embeddedImageFunctions[];
    extern const EmbeddedDataFunc g_embeddedImageFuncs[];
    extern const int g_embeddedImageFunctionsCount;
    extern const int g_embeddedImageFuncsCount;

    extern int oldWindowWidth;
    extern int oldWindowHeight;
    extern bool shouldCaptureScene;
    extern int newLayerCount;
}


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

    extern bool sceneUpdated;
    extern int  nextLayerID, nextShapeID;
    extern int  selectedLayerID, selectedShapeID;
    extern bool snapEnabled;
    extern float snapGridSize;
    extern int layerCount;

    extern ImTextureID black_texture_id;
    extern std::map<int, ImTextureID> gradientTextureCache;
    void ClearGradientTextureCache();

    extern std::string generatedCodeForSingleShape;
    extern std::string generatedCodeForWindow;
    extern std::string generatedCodeForButton;

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
    struct Layer;
    struct WindowData;
    class LayoutManager; 

    enum class HAlignment { Fill, Left, Center, Right };
    enum class VAlignment { Fill, Top, Center, Bottom };

    class LayoutManager {
    public:
        float spacing = 5.0f;
        virtual ~LayoutManager() = default;
        virtual void doLayout(ShapeItem& container, const ImVec2& availableSize) = 0;
        virtual const char* getTypeName() const = 0;
        virtual std::unique_ptr<LayoutManager> Clone() const = 0; 
    };

    class VerticalLayout : public LayoutManager {
    public:
        virtual ~VerticalLayout() override = default;
        virtual void doLayout(ShapeItem& container, const ImVec2& availableSize) override;
        virtual const char* getTypeName() const override;
        virtual std::unique_ptr<LayoutManager> Clone() const override; 
    };

    class HorizontalLayout : public LayoutManager {
    public:
        virtual ~HorizontalLayout() override = default;
        virtual void doLayout(ShapeItem& container, const ImVec2& availableSize) override;
        virtual const char* getTypeName() const override;
        virtual std::unique_ptr<LayoutManager> Clone() const override; 
    };

    enum class PositioningMode { Relative, Absolute };

    enum class FlexDirection { Row, RowReverse, Column, ColumnReverse };
    enum class FlexWrap { NoWrap, Wrap, WrapReverse };
    enum class JustifyContent { FlexStart, FlexEnd, Center, SpaceBetween, SpaceAround, SpaceEvenly };
    enum class AlignItems { Stretch, FlexStart, FlexEnd, Center, Baseline };
    enum class AlignContent { Stretch, FlexStart, FlexEnd, Center, SpaceBetween, SpaceAround, SpaceEvenly };
    enum class AlignSelf { Auto, Stretch, FlexStart, FlexEnd, Center, Baseline };

    enum class GridAutoFlow { Row, Column, RowDense, ColumnDense };
    enum class GridAxisAlignment { Start, End, Center, Stretch };

    struct LengthUnit {
        enum class Unit { Px, Percent };
        float value = 0.0f;
        Unit unit = Unit::Px;

        float getPixels(float baseSize) const {
            if (unit == Unit::Percent) {
                return baseSize * (value / 100.0f);
            }
            return value;
        }
    };

    struct GridTrackSize {
        struct TrackSizeValue {
            enum class Unit { Auto, Px, Percent, Fr };
            Unit unit = Unit::Auto;
            float value = 0.0f;

            float getPixels(float baseSize, float autoValue = 0.0f) const {
                switch (unit) {
                case Unit::Px:      return std::max(0.0f, value);
                case Unit::Percent: return std::max(0.0f, baseSize * (value / 100.0f));
                case Unit::Auto:    return std::max(0.0f, autoValue);
                case Unit::Fr:      return 0.0f;
                default:            return 0.0f;
                }
            }
            bool isFr() const { return unit == Unit::Fr; }
            bool isAuto() const { return unit == Unit::Auto; }
        };

        enum class Mode { Auto, Fixed, Fraction, Percentage, MinMax };
        Mode mode = Mode::Auto;
        float value = 0.0f;
        TrackSizeValue minVal;
        TrackSizeValue maxVal;

        GridTrackSize() {
            mode = Mode::Auto;
            value = 0.0f;
            minVal = { TrackSizeValue::Unit::Auto, 0.0f };
            maxVal = { TrackSizeValue::Unit::Auto, 0.0f };
        }
    };

    enum class ConstraintType {
        LeftDistance, RightDistance, TopDistance, BottomDistance,
        CenterXAlignment, CenterYAlignment,
        WidthFixed, HeightFixed,
        WidthPercentage, HeightPercentage,
        AspectRatio
    };

    struct Constraint {
        ConstraintType type;
        float value = 0.0f;
    };

    class FlexLayout : public LayoutManager {
    public:
        FlexDirection direction = FlexDirection::Row;
        FlexWrap wrap = FlexWrap::NoWrap;
        JustifyContent justifyContent = JustifyContent::FlexStart;
        AlignItems alignItems = AlignItems::Stretch;
        AlignContent alignContent = AlignContent::Stretch;
        float gap = 0.0f;
        virtual ~FlexLayout() override = default;
        virtual void doLayout(ShapeItem& container, const ImVec2& availableSize) override;
        virtual const char* getTypeName() const override { return "FlexLayout"; }
        virtual std::unique_ptr<LayoutManager> Clone() const override; 
    };

    class GridLayout : public LayoutManager {
    public:
        std::vector<GridTrackSize> templateColumns;
        std::vector<GridTrackSize> templateRows;
        LengthUnit rowGap;
        LengthUnit columnGap;
        GridAutoFlow autoFlow = GridAutoFlow::Row;
        GridAxisAlignment defaultCellContentJustify = GridAxisAlignment::Stretch;
        GridAxisAlignment defaultCellContentAlign = GridAxisAlignment::Stretch;
        LengthUnit implicitTrackRowSize = { 50.0f, LengthUnit::Unit::Px };
        LengthUnit implicitTrackColSize = { 50.0f, LengthUnit::Unit::Px };
        JustifyContent justifyContent = JustifyContent::FlexStart;
        AlignContent alignContent = AlignContent::FlexStart;

        virtual ~GridLayout() override = default;
        virtual void doLayout(ShapeItem& container, const ImVec2& availableSize) override;
        virtual const char* getTypeName() const override { return "GridLayout"; }
        virtual std::unique_ptr<LayoutManager> Clone() const override; 
        GridLayout();
    };

    struct ShapeItem
    {
        int id;
        ShapeType type;
        bool isPolygon = false; 
        std::vector<ImVec2> polygonVertices; 
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
        bool usePerSideBorderThicknesses = false; 
        float borderSideThicknesses[4];
    
        ImVec4 fillColor, borderColor, shadowColor, shadowSpread;
        
        bool usePerSideBorderColors = false;
        ImVec4 borderSideColors[4];
    
        ImVec2 shadowOffset;
        bool shadowUseCornerRadius;
        bool shadowInset = false;
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
        std::unique_ptr<LayoutManager> layoutManager; 
        float stretchFactor = 0.0f;
        HAlignment horizontalAlignment = HAlignment::Fill;
        VAlignment verticalAlignment = VAlignment::Fill;
        PositioningMode positioningMode = PositioningMode::Relative;
        std::vector<Constraint> constraints;
        float flexGrow = 0.0f;
        float flexShrink = 1.0f;
        enum class FlexBasisMode { Auto = -1, Pixels = 0, Content = -2, Percentage = -3 };
        FlexBasisMode flexBasisMode = FlexBasisMode::Auto;
        float flexBasisPixels = 0.0f;
        AlignSelf alignSelf = AlignSelf::Auto;
        int order = 0;
        int gridColumnStart = -1;
        int gridColumnEnd = -1;
        int gridRowStart = -1;
        int gridRowEnd = -1;
        GridAxisAlignment justifySelf = GridAxisAlignment::Stretch;
        GridAxisAlignment alignSelfGrid = GridAxisAlignment::Stretch;
        ImVec4 padding = ImVec4(0, 0, 0, 0);
        ImVec4 margin = ImVec4(0, 0, 0, 0);
    
        enum class BoxSizing {
            BorderBox,  
            ContentBox, 
            StrokeBox   
        };
        BoxSizing boxSizing = BoxSizing::StrokeBox; 
    
        ShapeItem();
        ShapeItem(const ShapeItem& other);
        ShapeItem& operator=(const ShapeItem& other);
        ShapeItem(ShapeItem&& other) noexcept;
        ShapeItem& operator=(ShapeItem&& other) noexcept;

    private:
        void swap(ShapeItem& other) noexcept;
        void copy_from(const ShapeItem& other);
    };

    inline bool g_IsInEditMode = false;

    enum class InteractionType {
        None,
        Selecting,
        Dragging,
        Resizing,
        Rotating
    };

    enum class InteractionHandle {
        None, Body,
        TopLeft, Top, TopRight,
        Left, Right,
        BottomLeft, Bottom, BottomRight,
        Rotate
    };

    struct InteractionState {
        InteractionType type = InteractionType::None;
        InteractionHandle activeHandle = InteractionHandle::None;
        ImVec2 dragStartMousePos = ImVec2(0, 0);
        std::vector<std::pair<ShapeItem*, ImVec2>> dragStartShapePositions;
        std::vector<std::pair<ShapeItem*, ImVec2>> dragStartShapeSizes;
        std::vector<std::pair<ShapeItem*, float>>  dragStartShapeRotations;
        ImVec2 interactionStartShapeCenter = ImVec2(0, 0);
        bool undoStateRecorded = false;
    };

    inline InteractionState g_InteractionState;
    inline bool g_ShowLayoutDebugLines = false;

    void DrawGridLayoutDebug(ShapeItem& container, ImDrawList* dl);
    void DrawFlexLayoutDebug(ShapeItem& container, ImDrawList* dl);
    void DrawLayoutItemBoundsDebug(ShapeItem& container, ImDrawList* dl);

    inline std::vector<ShapeItem*> selectedShapes;
    std::string GenerateSingleShapeCode(const ShapeItem& shape);
    std::string GenerateCodeForSingleButton(const ShapeItem& buttonShape);
    inline std::map<int, bool> temporaryWindowsOpen;
    inline bool exclusiveChildWindowMode = true;

    struct Layer
    {
        int id;
        std::string name;
        std::vector<std::unique_ptr<ShapeItem>> shapes; 
        bool visible, locked;
        int zOrder;

        Layer(const std::string& n);
        
        Layer(const Layer& other);
        Layer& operator=(const Layer& other);
        Layer(Layer&& other) noexcept;
        Layer& operator=(Layer&& other) noexcept;
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

    bool CompareShapesByZOrder(const ShapeItem& a, const ShapeItem& b);
    bool CompareLayersByZOrder(const Layer& a, const Layer& b);

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




    
    struct ButtonState {
        float lastMoveTime = 0.0f;
        int targetPositionIndex = 0;
        std::vector<ImVec2> waypoints;
        ImVec2 currentAnimatedPos;
        float moveProgress = 1.0f;
        int moveCycleCount = 0;
        bool layerChanged = false;
        bool layoutChanged = false;
        float lastRelativeMoveTime = 0.0f;
        bool nextIsPercentageMove = false;
    };

    struct PanelState {
        float lastTextChangeTime = 0.0f;
        int textVariant = 0;
        float lastBorderAnimTime = 0.0f;
        int animationCompletionCount = 0;
        bool isFadingOut = false;
        float fadeProgress = 0.0f;
    };

    
    extern std::map<int, ButtonState> g_buttonStates;
    extern std::map<int, PanelState> g_panelStates;

    
    template <typename T>
    inline T& GetOrCreatePerItemState(int shapeId) {
        if constexpr (std::is_same_v<T, ButtonState>) {
            auto it = g_buttonStates.find(shapeId);
            if (it == g_buttonStates.end()) {
                it = g_buttonStates.emplace(shapeId, ButtonState{}).first;
            }
            return it->second;
        }
        else if constexpr (std::is_same_v<T, PanelState>) {
            auto it = g_panelStates.find(shapeId);
            if (it == g_panelStates.end()) {
                it = g_panelStates.emplace(shapeId, PanelState{}).first;
            }
            return it->second;
        }
        static_assert(std::is_same_v<T, ButtonState> || std::is_same_v<T, PanelState>, "GetOrCreatePerItemState called with an unsupported type.");
        
        
        
        
        
        
        
        static T dummy{};
        return dummy;
    }

    
    WindowData& GetOrCreateWindow(const std::string& name, bool isOpen = true);
    Layer* GetOrCreateLayer(WindowData& window, const std::string& layerName, int zOrder);
    ShapeItem* GetOrCreateShapeInLayer(Layer& layer, const ShapeItem& templateShape);
    std::unique_ptr<ShapeItem> RemoveShapeFromLayer(Layer& layer, int shapeId);
    ShapeItem* AddShapeToLayer(Layer& layer, std::unique_ptr<ShapeItem> shape_uptr);

    
    bool IsItemClicked(int shapeId, float deltaTime); 
    namespace Scheduler { 
        void ProcessTasks(float totalTime);
    }

    
    void SetupProgrammaticUI_UltimateFreedom(float deltaTime, float totalTime);






    std::vector<ShapeItem*> GetAllShapes();
    std::vector<ShapeItem*> GetAllButtonShapes();

    void RegisterWindow(std::string name, std::function<void()> renderFunc);
    void SetWindowOpen(const std::string& name, bool open);
    bool IsWindowOpen(const std::string& name);

    inline float globalScaleFactor = 1.0f;

    ShapeItem* FindShapeByID(int shapeID);
    void UpdateGlobalScaleFactor(int currentW, int currentH);
    void RemoveParent(ShapeItem* child);
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
    void SetParent(ShapeItem* child, ShapeItem* parent);
    void RemoveParentKeepTransform(ShapeItem* child);

    inline std::string selectedGuiWindow = "Main";

    void DrawShape_RenderImGuiContent(ImDrawList* dl, ShapeItem& s, ImVec2 actualPos_World, ImVec2 actualSizePx, float scaleFactor);
    void BuildRectPoly(std::vector<ImVec2>& poly, ImVec2 pos, ImVec2 size, float r, ImVec2 c, float rot);
    void BuildCirclePoly(std::vector<ImVec2>& poly, ImVec2 c, float rx, float ry, float rot);
    ImTextureID CreateWhiteMaskTexture(int width, int height);
    ImTextureID CreateGradientTexture(const ImVec2& size, float gradRotation, const std::vector<std::pair<float, ImVec4>>& colorRamp, DesignManager::ShapeItem::GradientInterpolation interpolationType);
    void DrawGradient(ImDrawList* dl, const std::vector<ImVec2>& poly, float gradRotation, const std::vector<std::pair<float, ImVec4>>& colorRamp, const ShapeItem& s);

    extern std::vector<Layer> layers;

    void RenderTemporaryWindows();
    void DrawShapeTreeNode(ShapeItem* shape, Layer& layer, int layerIndex, int shapeIndexInLayer, int& selectedLayerIndex, std::vector<ShapeItem*>& selectedShapes, ShapeItem*& lastClickedShape, int& lastClickedLayerIndex, int& layerToSort, bool& needsLayerSort);
    void AddTextRotated(ImDrawList* draw_list, ImFont* font, float font_size, const ImVec2& pos, ImU32 col, const char* text, float angle_radians, const ImVec2& pivot_norm);
    void DrawShape(ImDrawList* dl, ShapeItem& s, ImVec2 wp);
    void DrawShape_RenderChildWindow(ShapeItem& s, ImVec2 contentActualPos_World, ImVec2 contentActualSizePx);
    void DrawShape_Shadow(ImDrawList* dlEffective, ShapeItem& s, ImVec2 wp, float scaleFactor, ImVec2 c, float totalrot);
    void DrawShape_Blur(ImDrawList* dlEffective, ShapeItem& s, ImVec2 wp, float scaleFactor, ImVec2 c);
    void BuildMainShapePoly(ShapeItem& s, ImVec2 wp, float scaleFactor, ImVec2 c, std::vector<ImVec2>& poly);
    void DrawShape_LoadEmbeddedImageIfNeeded(ShapeItem& item);
    void DrawShape_DrawEmbeddedImageIfAny(ImDrawList* dlEffective, ShapeItem& s, float scaleFactor, ImVec2 cImage, std::vector<ImVec2>& poly);
    void DispatchEvent(ShapeItem& shape, const std::string& eventType);
    void DrawShape_ProcessButtonLogic(ImDrawList* dlEffective, ShapeItem& s, float scaleFactor, ImVec2 wp, ImVec4& drawColor);
    void DrawShape_FillWithGradient(ImDrawList* dlEffective, const std::vector<ImVec2>& poly,
        const ShapeItem& shape, 
        const ImVec2& fillActualPosPx_World, 
        const ImVec2& fillActualSizePx,      
        const ImVec2& fillActualCenterPx_World 
    );
    void DrawShape_Fill(ImDrawList* dlEffective, ShapeItem& s, const std::vector<ImVec2>& poly,
        const ImVec2& fillActualPosPx_World, const ImVec2& fillActualSizePx,
        const ImVec2& fillActualCenterPx_World,
        ImVec4 drawColor);
    void DrawShape_DrawBorder(ImDrawList* dlEffective, ShapeItem& s, float scaleFactor, ImVec2 c, ImVec2 wp);
    void DrawShape_DrawText(ImDrawList* dlEffective, ShapeItem& s, ImVec2 contentActualPos_World, ImVec2 contentActualSizePx, float scaleFactor);
    void DrawShape_FinalOnClick(ShapeItem& s);

    extern int lastSelectedLayerIndex;
    extern int lastSelectedShapeIndex;
    extern int selectedLayerIndex, selectedShapeIndex;

    void RenderAllRegisteredWindows();

    inline void MarkSceneUpdated() {
        DesignManager::sceneUpdated = false;
        DesignManager::shouldCaptureScene = true;
        glFinish();
    }

    inline float NormalizeProgress(float progress) {
        return progress / 100.0f;
    }

    ImVec2 CalculateIntrinsicSize(const ShapeItem& item);
    float GetBaselineOffset(const ShapeItem& item, const ImVec2& calculatedSize);
    int FindShapeLayerIndex(int shapeId);

    GridTrackSize::TrackSizeValue ParseTrackSizeValueString(const std::string& segment);
    std::vector<GridTrackSize> ParseGridTemplate(const std::string& templateString);

    inline AlignSelf ResolveAlignSelf(AlignSelf itemAlignSelfValue, AlignItems containerAlignItemsValue) {
        if (itemAlignSelfValue != AlignSelf::Auto) {
            return itemAlignSelfValue;
        }
        switch (containerAlignItemsValue) {
        case AlignItems::FlexStart: return AlignSelf::FlexStart;
        case AlignItems::FlexEnd:   return AlignSelf::FlexEnd;
        case AlignItems::Center:    return AlignSelf::Center;
        case AlignItems::Stretch:   return AlignSelf::Stretch;
        case AlignItems::Baseline:  return AlignSelf::Baseline;
        default:                    return AlignSelf::Stretch;
        }
    }
    void ApplyConstraints(ShapeItem* shape, const ImVec2& parentFinalSize, ImVec2* outPosition, ImVec2* outSize);
    void UpdateShapeTransforms_Unified(GLFWwindow* window, float deltaTime);
    ImVec2 ComputeChainOffset(const ShapeItem& shape);
    void UpdateChainAnimations(float deltaTime);
    int GetUniqueShapeID();
    void ShowChainAnimationGUI();
    void DrawAll(ImDrawList* dl);
    std::string SanitizeVariableName(const std::string& name);
    std::string escapeNewlines(const std::string& input);
    void CopyToClipboard(const std::string& text);
    std::string GenerateButtonAnimationCode(const ButtonAnimation& anim);
    std::string GenerateShapeKeyCode(const ShapeKey& key);
    std::string GenerateChildWindowMappingsCode();
    std::set<int> GetAllShapeIDs();
    ShapeItem* FindShapeByID_Internal(int id);
    std::string GenerateComponentDefinitionCode(const std::string& componentName, const ComponentDefinition& compDef);
    std::string GenerateAllComponentDefinitionsCode();
    std::string GenerateLengthUnitCode(const LengthUnit& unit);
    std::string GenerateGridTrackSizeCode(const GridTrackSize& track);
    std::string GenerateCodeForWindow(const std::string& windowName);

    void RefreshLayerIDs();
    void EnsureMainWindowExists();
    int GetUniqueLayerID();

    extern ShapeItem* lastClickedShape;
    extern int lastClickedLayerIndex;

    ShapeItem* FindShapeByIdRecursiveHelper(int shapeId, ShapeItem* currentShape);
    ShapeItem* FindShapeByIdRecursive(int shapeId);
    ImRect GetShapeBoundingBox(const ShapeItem& s);
    bool IsMouseOverShape(const ImVec2& mouseCanvasPos, const ShapeItem& s);
    void ProcessCanvasInteractions();
    void DrawInteractionGizmos(ImDrawList* fgDrawList);
    void ShowUI_HierarchyPanel(WindowData& windowData, int& selectedLayerIndex, std::vector<ShapeItem*>& selectedShapes);
    void ShowUI_PropertiesPanel(WindowData& windowData, int& selectedLayerIndex, std::vector<ShapeItem*>& selectedShapes);
    void ShowUI_CodeGenerationPanel();
    void ShowUI_ComponentPanel();
    void ShowUI_LayerShapeManager_ChildWindowMappings();
    bool VerticalSplitter(const char* str_id, float thickness, float height, float* size_left, float* size_right, float min_left, float min_right);
    void ShowUI(GLFWwindow* window);
    std::string SaveConfiguration();
    void DrawAllForWindow(const std::string& windowName, const Layer& layer);


    void RenderChildWindowForShape1();




    void Init(int width, int height, GLFWwindow* window);

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

            DEFINE_SETTER(UsePerSideBorderThicknesses, usePerSideBorderThicknesses) 

            DEFINE_SETTER(FillColor, fillColor)
            DEFINE_SETTER(BorderColor, borderColor)
            DEFINE_SETTER(UsePerSideBorderColors, usePerSideBorderColors)
            DEFINE_SETTER(Visible, visible)
            DEFINE_SETTER(Locked, locked)
            DEFINE_SETTER(ShadowColor, shadowColor)
            DEFINE_SETTER(ShadowSpread, shadowSpread)
            DEFINE_SETTER(ShadowOffset, shadowOffset)
            DEFINE_SETTER(ShadowUseCornerRadius, shadowUseCornerRadius)
            DEFINE_SETTER(ShadowRotation, shadowRotation)
            DEFINE_SETTER(ShadowInset, shadowInset)
            DEFINE_SETTER(BlurAmount, blurAmount)
            DEFINE_SETTER(UseGradient, useGradient)
            DEFINE_SETTER(GradientRotation, gradientRotation)
            DEFINE_SETTER(GradientInterpolation, gradientInterpolation)
            DEFINE_SETTER(ColorRamp, colorRamp)
            DEFINE_SETTER(UseGlass, useGlass)
            DEFINE_SETTER(GlassBlur, glassBlur)
            DEFINE_SETTER(GlassAlpha, glassAlpha)
            DEFINE_SETTER(GlassColor, glassColor)
            DEFINE_SETTER(IsChildWindow, isChildWindow)
            DEFINE_SETTER(ChildWindowSync, childWindowSync)
            DEFINE_SETTER(ToggleChildWindow, toggleChildWindow)
            DEFINE_SETTER(ToggleBehavior, toggleBehavior)
            DEFINE_SETTER(ChildWindowGroupId, childWindowGroupId)
            DEFINE_SETTER(TargetShapeID, targetShapeID)
            DEFINE_SETTER(TriggerGroupID, triggerGroupID)
            DEFINE_SETTER(IsImGuiContainer, isImGuiContainer)
            DEFINE_SETTER(IsButton, isButton)
            DEFINE_SETTER(ButtonBehavior, buttonBehavior)
            DEFINE_SETTER(UseOnClick, useOnClick)
            DEFINE_SETTER(HoverColor, hoverColor)
            DEFINE_SETTER(ClickedColor, clickedColor)
            DEFINE_SETTER(ToggledStatePositionOffset, toggledStatePositionOffset)
            DEFINE_SETTER(ToggledStateSizeOffset, toggledStateSizeOffset)
            DEFINE_SETTER(ToggledStateRotationOffset, toggledStateRotationOffset)
            DEFINE_SETTER(HasText, hasText)
            DEFINE_SETTER(Text, text)
            DEFINE_SETTER(TextColor, textColor)
            DEFINE_SETTER(TextSize, textSize)
            DEFINE_SETTER(TextFont, textFont)
            DEFINE_SETTER(TextPosition, textPosition)
            DEFINE_SETTER(TextRotation, textRotation)
            DEFINE_SETTER(TextAlignment, textAlignment)
            DEFINE_SETTER(DynamicTextSize, dynamicTextSize)
            DEFINE_SETTER(BaseTextSize, baseTextSize)
            DEFINE_SETTER(MinTextSize, minTextSize)
            DEFINE_SETTER(MaxTextSize, maxTextSize)
            DEFINE_SETTER(AnchorMode, anchorMode)
            DEFINE_SETTER(AnchorMargin, anchorMargin)
            DEFINE_SETTER(UsePercentagePos, usePercentagePos)
            DEFINE_SETTER(PercentagePos, percentagePos)
            DEFINE_SETTER(UsePercentageSize, usePercentageSize)
            DEFINE_SETTER(PercentageSize, percentageSize)
            DEFINE_SETTER(MinSize, minSize)
            DEFINE_SETTER(MaxSize, maxSize)
            DEFINE_SETTER(HasEmbeddedImage, hasEmbeddedImage)
            DEFINE_SETTER(EmbeddedImageIndex, embeddedImageIndex)
            DEFINE_SETTER(ZOrder, zOrder)
            DEFINE_SETTER(AllowItemOverlap, allowItemOverlap)
            DEFINE_SETTER(ForceOverlap, forceOverlap)
            DEFINE_SETTER(BlockUnderlying, blockUnderlying)
            DEFINE_SETTER(Type, type)
            DEFINE_SETTER(UpdateAnimBaseOnResize, updateAnimBaseOnResize)
            DEFINE_SETTER(PositioningMode, positioningMode)
            DEFINE_SETTER(FlexGrow, flexGrow)
            DEFINE_SETTER(FlexShrink, flexShrink)
            DEFINE_SETTER(FlexBasisMode, flexBasisMode)
            DEFINE_SETTER(FlexBasisPixels, flexBasisPixels)
            DEFINE_SETTER(AlignSelf, alignSelf)
            DEFINE_SETTER(Order, order)
            DEFINE_SETTER(GridColumnStart, gridColumnStart)
            DEFINE_SETTER(GridColumnEnd, gridColumnEnd)
            DEFINE_SETTER(GridRowStart, gridRowStart)
            DEFINE_SETTER(GridRowEnd, gridRowEnd)
            DEFINE_SETTER(JustifySelf, justifySelf)
            DEFINE_SETTER(AlignSelfGrid, alignSelfGrid)
            DEFINE_SETTER(IsLayoutContainer, isLayoutContainer)
            DEFINE_SETTER(StretchFactor, stretchFactor)
            DEFINE_SETTER(HorizontalAlignment, horizontalAlignment)
            DEFINE_SETTER(VerticalAlignment, verticalAlignment)
            DEFINE_SETTER(BoxSizing, boxSizing) 
            DEFINE_SETTER(IsPolygon, isPolygon) 
            DEFINE_SETTER(PolygonVertices, polygonVertices) 

        ShapeBuilder& addColorRampEntry(float pos, const ImVec4& color) {
            shape.colorRamp.emplace_back(pos, color);
            return *this;
        }
        ShapeBuilder& addOnClickAnimation(const ButtonAnimation& anim) {
            shape.onClickAnimations.push_back(anim);
            return *this;
        }
        ShapeBuilder& addShapeKey(const ShapeKey& key) {
            shape.shapeKeys.push_back(key);
            return *this;
        }
        ShapeBuilder& setRenderImGuiContent(const std::function<void()>& func) {
            shape.renderImGuiContent = func;
            return *this;
        }
        ShapeBuilder& addEventHandler(const std::string& eventType, const std::string& name, const std::function<void(ShapeItem&)>& handler) {
            shape.eventHandlers.push_back({ eventType, name, handler });
            return *this;
        }
        ShapeBuilder& addConstraint(const Constraint& constraint) {
            shape.constraints.push_back(constraint);
            return *this;
        }
        ShapeBuilder& setLayoutManager(std::unique_ptr<LayoutManager> manager) { 
            shape.layoutManager = std::move(manager);
            return *this;
        }
        ShapeBuilder& setPadding(const ImVec4& value) {
            shape.padding = value;
            return *this;
        }
        ShapeBuilder& setMargin(const ImVec4& value) {
            shape.margin = value;
            return *this;
        }
        ShapeBuilder& setBorderSideColor(int side, const ImVec4& color) { 
            if (side >= 0 && side < 4) {
                shape.borderSideColors[side] = color;
                shape.usePerSideBorderColors = true; 
            }
            return *this;
        }
        ShapeBuilder& setBorderSideThickness(int side, float thickness) { 
            if (side >= 0 && side < 4) {
                shape.borderSideThicknesses[side] = std::max(0.0f, thickness);
                shape.usePerSideBorderThicknesses = true; 
            }
            return *this;
        }

        ShapeBuilder& setBorderSidesThickness(float top, float right, float bottom, float left) { 
            shape.borderSideThicknesses[0] = std::max(0.0f, top);
            shape.borderSideThicknesses[1] = std::max(0.0f, right);
            shape.borderSideThicknesses[2] = std::max(0.0f, bottom);
            shape.borderSideThicknesses[3] = std::max(0.0f, left);
            shape.usePerSideBorderThicknesses = true; 
            return *this;
        }
        ShapeBuilder& setBorderSidesColor(const ImVec4& top, const ImVec4& right, const ImVec4& bottom, const ImVec4& left) { 
            shape.borderSideColors[0] = top;
            shape.borderSideColors[1] = right;
            shape.borderSideColors[2] = bottom;
            shape.borderSideColors[3] = left;
            shape.usePerSideBorderColors = true; 
            return *this;
        }
        static AnimationBuilder createAnimation() {
            return AnimationBuilder();
        }
        static ShapeKeyBuilder createShapeKey() {
            return ShapeKeyBuilder();
        }

        ShapeItem build();
    };

#undef DEFINE_ANIM_SETTER
#undef DEFINE_SHAPEKEY_SETTER
#undef DEFINE_SETTER

    void GeneratedCode();

}
