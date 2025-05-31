
#include "design_manager.h"

namespace DesignManager {

    std::unique_ptr<LayoutManager> VerticalLayout::Clone() const {
        return std::make_unique<VerticalLayout>(*this);
    }

    std::unique_ptr<LayoutManager> HorizontalLayout::Clone() const {
        return std::make_unique<HorizontalLayout>(*this);
    }

    std::unique_ptr<LayoutManager> FlexLayout::Clone() const {
        return std::make_unique<FlexLayout>(*this);
    }

    std::unique_ptr<LayoutManager> GridLayout::Clone() const {
        return std::make_unique<GridLayout>(*this);
    }








    std::map<int, ButtonState> g_buttonStates;
    std::map<int, PanelState> g_panelStates;

    
    WindowData& GetOrCreateWindow(const std::string& name, bool isOpen) {
        auto it = g_windowsMap.find(name);
        if (it == g_windowsMap.end()) {
            WindowData newWindow;
            newWindow.isOpen = isOpen;
            
            if (newWindow.layers.empty()) {
                Layer defaultLayer("Default Layer"); 
                defaultLayer.id = GetUniqueLayerID(); 
                defaultLayer.zOrder = 0;
                newWindow.layers.push_back(std::move(defaultLayer));
            }
            it = g_windowsMap.emplace(name, std::move(newWindow)).first;
        }
        else {
            it->second.isOpen = isOpen; 
        }
        return it->second;
    }

    Layer* GetOrCreateLayer(WindowData& window, const std::string& layerName, int zOrder) {
        auto it = std::find_if(window.layers.begin(), window.layers.end(),
            [&](const Layer& layer) { return layer.name == layerName; });
        if (it == window.layers.end()) {
            Layer newLayer(layerName); 
            newLayer.id = GetUniqueLayerID(); 
            newLayer.zOrder = zOrder;
            newLayer.visible = true;
            newLayer.locked = false;
            window.layers.push_back(std::move(newLayer));

            std::stable_sort(window.layers.begin(), window.layers.end(), CompareLayersByZOrder); 

            
            
            auto sorted_it = std::find_if(window.layers.begin(), window.layers.end(),
                [&](const Layer& layer) { return layer.name == layerName; });
            if (sorted_it != window.layers.end()) {
                return &(*sorted_it);
            }
            return nullptr; 
        }
        else {
            bool resort = false;
            if (it->zOrder != zOrder) {
                it->zOrder = zOrder;
                resort = true;
            }
            
            
            if (resort) {
                std::stable_sort(window.layers.begin(), window.layers.end(), CompareLayersByZOrder);
                
                auto sorted_it = std::find_if(window.layers.begin(), window.layers.end(),
                    [&](const Layer& layer) { return layer.name == layerName; });
                if (sorted_it != window.layers.end()) {
                    return &(*sorted_it);
                }
                return nullptr; 
            }
            return &(*it);
        }
    }

    ShapeItem* GetOrCreateShapeInLayer(Layer& layer, const ShapeItem& templateShape) {
        auto it = std::find_if(layer.shapes.begin(), layer.shapes.end(),
            [&](const std::unique_ptr<ShapeItem>& s_ptr) {
                return s_ptr && s_ptr->id == templateShape.id;
            });
        if (it != layer.shapes.end()) {
            
            
            
            
            return it->get();
        }
        else {
            
            
            
            auto newShape = std::make_unique<ShapeItem>(templateShape);
            ShapeItem* raw_ptr = newShape.get(); 
            layer.shapes.push_back(std::move(newShape));

            
            std::stable_sort(layer.shapes.begin(), layer.shapes.end(),
                [](const std::unique_ptr<ShapeItem>& a, const std::unique_ptr<ShapeItem>& b) {
                    if (!a || !b) return false; 
                    return a->zOrder < b->zOrder; 
                });
            return raw_ptr; 
        }
    }

    std::unique_ptr<ShapeItem> RemoveShapeFromLayer(Layer& layer, int shapeId) {
        auto it = std::find_if(layer.shapes.begin(), layer.shapes.end(),
            [&](const std::unique_ptr<ShapeItem>& s_ptr) {
                return s_ptr && s_ptr->id == shapeId;
            });
        if (it != layer.shapes.end()) {
            std::unique_ptr<ShapeItem> shape_uptr = std::move(*it);
            layer.shapes.erase(it);
            if (shape_uptr) {
                shape_uptr->parent = nullptr; 
            }
            return shape_uptr;
        }
        return nullptr; 
    }

    ShapeItem* AddShapeToLayer(Layer& layer, std::unique_ptr<ShapeItem> shape_uptr) {
        if (!shape_uptr) return nullptr;
        ShapeItem* raw_ptr = shape_uptr.get(); 
        layer.shapes.push_back(std::move(shape_uptr));

        
        std::stable_sort(layer.shapes.begin(), layer.shapes.end(),
            [](const std::unique_ptr<ShapeItem>& a, const std::unique_ptr<ShapeItem>& b) {
                if (!a || !b) return false;
                return a->zOrder < b->zOrder;
            });
        return raw_ptr; 
    }

    
    
    
    
    
    
    bool IsItemClicked(int shapeId, float deltaTime) {
        
        
        
        
        
        
        
        
        
        ShapeItem* shape = FindShapeByID(shapeId); 
        if (shape) {
            
            
            
            
            
            if (shape->isClicked) { 
                
                
            }
        }
        return false;
    }

    namespace Scheduler {
        void ProcessTasks(float totalTime) {
            
            
        }
    } 






    

    const char* g_embeddedImageFunctions[] = {
        "GetShape0_embeddedData",
        "GetShape1_embeddedData",
    };
    const EmbeddedDataFunc g_embeddedImageFuncs[] = {
        &GetShape0_embeddedData,
        &GetShape1_embeddedData,
    };
    const int g_embeddedImageFunctionsCount = sizeof(g_embeddedImageFunctions) / sizeof(g_embeddedImageFunctions[0]);
    const int g_embeddedImageFuncsCount = sizeof(g_embeddedImageFuncs) / sizeof(g_embeddedImageFuncs[0]);

    int oldWindowWidth = 0;
    int oldWindowHeight = 0;
    bool shouldCaptureScene = false;
    int newLayerCount = 0;
    bool sceneUpdated = false;
    int  nextLayerID = 1, nextShapeID = 1;
    int  selectedLayerID = -1, selectedShapeID = -1;
    bool snapEnabled = true;
    float snapGridSize = 10.0f;
    int layerCount = -1;
    ImTextureID black_texture_id = 0;
    std::map<int, ImTextureID> gradientTextureCache;
    std::string generatedCodeForSingleShape;
    std::string generatedCodeForWindow;
    std::string generatedCodeForButton;
    std::vector<Layer> layers;
    int lastSelectedLayerIndex = -1;
    int lastSelectedShapeIndex = -1;
    int selectedLayerIndex = 0, selectedShapeIndex = -1;
    ShapeItem* lastClickedShape = nullptr;
    int lastClickedLayerIndex = -1;

    ShapeItem::ShapeItem()
        : id(GetUniqueShapeID()),
        type(ShapeType::Rectangle),
        name("Shape"),
        position(ImVec2(100, 100)),
        size(ImVec2(200, 100)),
        minSize(ImVec2(0, 0)),
        maxSize(ImVec2(99999, 99999)),
        cornerRadius(10),
        borderThickness(2),
        usePerSideBorderThicknesses(false), 
        fillColor(ImVec4(0.932f, 0.932f, 0.932f, 1)),
        borderColor(ImVec4(0, 0, 0, 0.8f)),
        usePerSideBorderColors(false),
        shadowColor(ImVec4(0, 0, 0, 0.2f)),
        shadowSpread(ImVec4(2, 2, 2, 2)),
        shadowOffset(ImVec2(2, 2)),
        shadowUseCornerRadius(true),
        shadowInset(false), 
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
        verticalAlignment(VAlignment::Fill),
        positioningMode(PositioningMode::Relative),
        constraints(),
        flexGrow(0.0f),
        flexShrink(1.0f),
        alignSelf(AlignSelf::Auto),
        order(0),
        gridColumnStart(-1),
        gridColumnEnd(-1),
        gridRowStart(-1),
        gridRowEnd(-1),
        justifySelf(GridAxisAlignment::Stretch),
        alignSelfGrid(GridAxisAlignment::Stretch),
        padding(ImVec4(0.0f, 0.0f, 0.0f, 0.0f)),
        margin(ImVec4(0.0f, 0.0f, 0.0f, 0.0f)),
        boxSizing(BoxSizing::StrokeBox)


    {
        for (int i = 0; i < 4; ++i) { 
            borderSideColors[i] = borderColor;
        }
        for (int i = 0; i < 4; ++i) { 
            borderSideThicknesses[i] = borderThickness; 
        }
        colorRamp.emplace_back(0.0f, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        colorRamp.emplace_back(1.0f, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
        basePosition = position;
        baseSize = size;
        baseRotation = rotation;
    }

    ShapeItem::ShapeItem(const ShapeItem& other)
        : id(other.id), 
        type(other.type),
        name(other.name),
        position(other.position),
        size(other.size),
        basePosition(other.basePosition),
        baseSize(other.baseSize),
        rotation(other.rotation),
        baseRotation(other.baseRotation),
        minSize(other.minSize),
        maxSize(other.maxSize),
        isChildWindow(other.isChildWindow),
        childWindowSync(other.childWindowSync),
        toggleChildWindow(other.toggleChildWindow),
        toggleBehavior(other.toggleBehavior),
        childWindowGroupId(other.childWindowGroupId),
        targetShapeID(other.targetShapeID),
        triggerGroupID(other.triggerGroupID),
        isImGuiContainer(other.isImGuiContainer),
        renderImGuiContent(other.renderImGuiContent),
        allowItemOverlap(other.allowItemOverlap),
        forceOverlap(other.forceOverlap),
        blockUnderlying(other.blockUnderlying),
        hasText(other.hasText),
        text(other.text),
        textColor(other.textColor),
        textSize(other.textSize),
        textFont(other.textFont),
        textPosition(other.textPosition),
        textRotation(other.textRotation),
        textAlignment(other.textAlignment),
        dynamicTextSize(other.dynamicTextSize),
        baseTextSize(other.baseTextSize),
        minTextSize(other.minTextSize),
        maxTextSize(other.maxTextSize),
        shapeKeys(other.shapeKeys),
        shapeKeyAnimations(other.shapeKeyAnimations),
        shapeKeyValue(other.shapeKeyValue),
        onClickAnimations(other.onClickAnimations),
        chainAnimation(other.chainAnimation),
        isPressed(false), 
        groupId(other.groupId),
        currentAnimation(nullptr), 
        updateAnimBaseOnResize(other.updateAnimBaseOnResize),
        isHeld(false), 
        isAnimating(false), 
        animationProgress(0.0f), 
        baseKeyOffset(other.baseKeyOffset),
        baseKeySizeOffset(other.baseKeySizeOffset),
        baseKeyRotationOffset(other.baseKeyRotationOffset),
        cornerRadius(other.cornerRadius),
        borderThickness(other.borderThickness),
        usePerSideBorderThicknesses(other.usePerSideBorderThicknesses), 
        fillColor(other.fillColor),
        borderColor(other.borderColor),
        usePerSideBorderColors(other.usePerSideBorderColors),
        shadowColor(other.shadowColor),
        shadowSpread(other.shadowSpread),
        shadowOffset(other.shadowOffset),
        shadowUseCornerRadius(other.shadowUseCornerRadius),
        shadowInset(other.shadowInset),
        blurAmount(other.blurAmount),
        visible(other.visible),
        locked(other.locked),
        useGradient(other.useGradient),
        gradientRotation(other.gradientRotation),
        gradientInterpolation(other.gradientInterpolation),
        colorRamp(other.colorRamp),
        shadowRotation(other.shadowRotation),
        useGlass(other.useGlass),
        glassBlur(other.glassBlur),
        glassAlpha(other.glassAlpha),
        glassColor(other.glassColor),
        zOrder(other.zOrder),
        isButton(other.isButton),
        assignedChildWindows(other.assignedChildWindows),
        selectedChildWindowIndex(other.selectedChildWindowIndex),
        logicExpression(other.logicExpression),
        logicAction(other.logicAction),
        clickCounter(0), 
        buttonBehavior(other.buttonBehavior),
        triggerEvent(false), 
        buttonState(false), 
        shouldCallOnClick(false), 
        isClicked(false), 
        hoverColor(other.hoverColor),
        clickedColor(other.clickedColor),
        useOnClick(other.useOnClick),
        onClick(other.onClick),
        storedOnClick(other.storedOnClick),
        parent(nullptr), 
        
        originalPosition(other.originalPosition),
        originalRotation(other.originalRotation),
        originalSize(other.originalSize),
        ownerWindow(other.ownerWindow),
        toggledStatePositionOffset(other.toggledStatePositionOffset),
        toggledStateSizeOffset(other.toggledStateSizeOffset),
        toggledStateRotationOffset(other.toggledStateRotationOffset),
        sizeThresholds(other.sizeThresholds),
        hasEmbeddedImage(other.hasEmbeddedImage),
        embeddedImageData(other.embeddedImageData),
        embeddedImageWidth(other.embeddedImageWidth),
        embeddedImageHeight(other.embeddedImageHeight),
        embeddedImageChannels(other.embeddedImageChannels),
        embeddedImageIndex(other.embeddedImageIndex),
        embeddedImageTexture(0), 
        imageDirty(true),        
        eventHandlers(other.eventHandlers),
        anchorMode(other.anchorMode),
        anchorMargin(other.anchorMargin),
        usePercentagePos(other.usePercentagePos),
        percentagePos(other.percentagePos),
        usePercentageSize(other.usePercentageSize),
        percentageSize(other.percentageSize),
        isLayoutContainer(other.isLayoutContainer),
        layoutManager(nullptr), 
        stretchFactor(other.stretchFactor),
        horizontalAlignment(other.horizontalAlignment),
        verticalAlignment(other.verticalAlignment),
        positioningMode(other.positioningMode),
        constraints(other.constraints),
        flexGrow(other.flexGrow),
        flexShrink(other.flexShrink),
        flexBasisMode(other.flexBasisMode),
        flexBasisPixels(other.flexBasisPixels),
        alignSelf(other.alignSelf),
        order(other.order),
        gridColumnStart(other.gridColumnStart),
        gridColumnEnd(other.gridColumnEnd),
        gridRowStart(other.gridRowStart),
        gridRowEnd(other.gridRowEnd),
        justifySelf(other.justifySelf),
        alignSelfGrid(other.alignSelfGrid),
        padding(other.padding),
        margin(other.margin),
        boxSizing(other.boxSizing),
        isPolygon(other.isPolygon), 
        polygonVertices(other.polygonVertices) 

    {
        for (int i = 0; i < 4; ++i) { 
            borderSideColors[i] = other.borderSideColors[i];
        }
        for (int i = 0; i < 4; ++i) { 
            borderSideThicknesses[i] = other.borderSideThicknesses[i];
        }

        if (other.layoutManager) {
            layoutManager = other.layoutManager->Clone();
        }
        
        for (int i = 0; i < 2; ++i) {
            glassBlurFBO[i] = 0;
            glassBlurTex[i] = 0;
        }
    }

    
    ShapeItem& ShapeItem::operator=(const ShapeItem& other) {
        if (this == &other) return *this;

        
        
        id = other.id;
        type = other.type;
        name = other.name;
        position = other.position;
        size = other.size;
        basePosition = other.basePosition;
        baseSize = other.baseSize;
        rotation = other.rotation;
        baseRotation = other.baseRotation;
        minSize = other.minSize;
        maxSize = other.maxSize;
        isChildWindow = other.isChildWindow;
        childWindowSync = other.childWindowSync;
        toggleChildWindow = other.toggleChildWindow;
        toggleBehavior = other.toggleBehavior;
        childWindowGroupId = other.childWindowGroupId;
        targetShapeID = other.targetShapeID;
        triggerGroupID = other.triggerGroupID;
        isImGuiContainer = other.isImGuiContainer;
        renderImGuiContent = other.renderImGuiContent;
        allowItemOverlap = other.allowItemOverlap;
        forceOverlap = other.forceOverlap;
        blockUnderlying = other.blockUnderlying;
        hasText = other.hasText;
        text = other.text;
        textColor = other.textColor;
        textSize = other.textSize;
        textFont = other.textFont;
        textPosition = other.textPosition;
        textRotation = other.textRotation;
        textAlignment = other.textAlignment;
        dynamicTextSize = other.dynamicTextSize;
        baseTextSize = other.baseTextSize;
        minTextSize = other.minTextSize;
        maxTextSize = other.maxTextSize;
        shapeKeys = other.shapeKeys;
        shapeKeyAnimations = other.shapeKeyAnimations;
        shapeKeyValue = other.shapeKeyValue;
        onClickAnimations = other.onClickAnimations;
        chainAnimation = other.chainAnimation;
        isPressed = false;
        groupId = other.groupId;
        currentAnimation = nullptr;
        updateAnimBaseOnResize = other.updateAnimBaseOnResize;
        isHeld = false;
        isAnimating = false;
        animationProgress = 0.0f;
        baseKeyOffset = other.baseKeyOffset;
        baseKeySizeOffset = other.baseKeySizeOffset;
        baseKeyRotationOffset = other.baseKeyRotationOffset;
        cornerRadius = other.cornerRadius;
        borderThickness = other.borderThickness;
        usePerSideBorderThicknesses = other.usePerSideBorderThicknesses; 
        for (int i = 0; i < 4; ++i) { 
            borderSideThicknesses[i] = other.borderSideThicknesses[i];
        }
        fillColor = other.fillColor;
        borderColor = other.borderColor;
        usePerSideBorderColors = other.usePerSideBorderColors;
        for (int i = 0; i < 4; ++i) { 
            borderSideColors[i] = other.borderSideColors[i];
        }
        shadowColor = other.shadowColor;
        shadowSpread = other.shadowSpread;
        shadowOffset = other.shadowOffset;
        shadowUseCornerRadius = other.shadowUseCornerRadius;
        shadowInset = other.shadowInset;
        blurAmount = other.blurAmount;
        visible = other.visible;
        locked = other.locked;
        useGradient = other.useGradient;
        gradientRotation = other.gradientRotation;
        gradientInterpolation = other.gradientInterpolation;
        colorRamp = other.colorRamp;
        shadowRotation = other.shadowRotation;
        useGlass = other.useGlass;
        glassBlur = other.glassBlur;
        glassAlpha = other.glassAlpha;
        glassColor = other.glassColor;
        zOrder = other.zOrder;
        isButton = other.isButton;
        assignedChildWindows = other.assignedChildWindows;
        selectedChildWindowIndex = other.selectedChildWindowIndex;
        logicExpression = other.logicExpression;
        logicAction = other.logicAction;
        clickCounter = 0;
        buttonBehavior = other.buttonBehavior;
        triggerEvent = false;
        buttonState = false;
        shouldCallOnClick = false;
        isClicked = false;
        hoverColor = other.hoverColor;
        clickedColor = other.clickedColor;
        useOnClick = other.useOnClick;
        onClick = other.onClick;
        storedOnClick = other.storedOnClick;
        parent = nullptr;
        children.clear();
        originalPosition = other.originalPosition;
        originalRotation = other.originalRotation;
        originalSize = other.originalSize;
        ownerWindow = other.ownerWindow;
        toggledStatePositionOffset = other.toggledStatePositionOffset;
        toggledStateSizeOffset = other.toggledStateSizeOffset;
        toggledStateRotationOffset = other.toggledStateRotationOffset;
        sizeThresholds = other.sizeThresholds;
        hasEmbeddedImage = other.hasEmbeddedImage;
        embeddedImageData = other.embeddedImageData;
        embeddedImageWidth = other.embeddedImageWidth;
        embeddedImageHeight = other.embeddedImageHeight;
        embeddedImageChannels = other.embeddedImageChannels;
        embeddedImageIndex = other.embeddedImageIndex;
        embeddedImageTexture = 0;
        imageDirty = true;
        eventHandlers = other.eventHandlers;
        anchorMode = other.anchorMode;
        anchorMargin = other.anchorMargin;
        usePercentagePos = other.usePercentagePos;
        percentagePos = other.percentagePos;
        usePercentageSize = other.usePercentageSize;
        percentageSize = other.percentageSize;
        isLayoutContainer = other.isLayoutContainer;
        stretchFactor = other.stretchFactor;
        horizontalAlignment = other.horizontalAlignment;
        verticalAlignment = other.verticalAlignment;
        positioningMode = other.positioningMode;
        constraints = other.constraints;
        flexGrow = other.flexGrow;
        flexShrink = other.flexShrink;
        flexBasisMode = other.flexBasisMode;
        flexBasisPixels = other.flexBasisPixels;
        alignSelf = other.alignSelf;
        order = other.order;
        gridColumnStart = other.gridColumnStart;
        gridColumnEnd = other.gridColumnEnd;
        gridRowStart = other.gridRowStart;
        gridRowEnd = other.gridRowEnd;
        justifySelf = other.justifySelf;
        alignSelfGrid = other.alignSelfGrid;
        padding = other.padding;
        margin = other.margin;
        boxSizing = other.boxSizing;
        isPolygon = other.isPolygon; 
        polygonVertices = other.polygonVertices; 

        layoutManager.reset();
        if (other.layoutManager) {
            layoutManager = other.layoutManager->Clone();
        }
        for (int i = 0; i < 2; ++i) {
            glassBlurFBO[i] = 0;
            glassBlurTex[i] = 0;
        }
        return *this;
    }

    
    ShapeItem::ShapeItem(ShapeItem&& other) noexcept
        : id(other.id),
        type(other.type),
        name(std::move(other.name)),
        position(other.position),
        size(other.size),
        basePosition(other.basePosition),
        baseSize(other.baseSize),
        rotation(other.rotation),
        baseRotation(other.baseRotation),
        minSize(other.minSize),
        maxSize(other.maxSize),
        isChildWindow(other.isChildWindow),
        childWindowSync(other.childWindowSync),
        toggleChildWindow(other.toggleChildWindow),
        toggleBehavior(other.toggleBehavior),
        childWindowGroupId(other.childWindowGroupId),
        targetShapeID(other.targetShapeID),
        triggerGroupID(other.triggerGroupID),
        isImGuiContainer(other.isImGuiContainer),
        renderImGuiContent(std::move(other.renderImGuiContent)),
        allowItemOverlap(other.allowItemOverlap),
        forceOverlap(other.forceOverlap),
        blockUnderlying(other.blockUnderlying),
        hasText(other.hasText),
        text(std::move(other.text)),
        textColor(other.textColor),
        textSize(other.textSize),
        textFont(other.textFont),
        textPosition(other.textPosition),
        textRotation(other.textRotation),
        textAlignment(other.textAlignment),
        dynamicTextSize(other.dynamicTextSize),
        baseTextSize(other.baseTextSize),
        minTextSize(other.minTextSize),
        maxTextSize(other.maxTextSize),
        shapeKeys(std::move(other.shapeKeys)),
        shapeKeyAnimations(std::move(other.shapeKeyAnimations)),
        shapeKeyValue(other.shapeKeyValue),
        onClickAnimations(std::move(other.onClickAnimations)),
        chainAnimation(std::move(other.chainAnimation)),
        isPressed(other.isPressed),
        groupId(other.groupId),
        currentAnimation(other.currentAnimation),
        updateAnimBaseOnResize(other.updateAnimBaseOnResize),
        isHeld(other.isHeld),
        isAnimating(other.isAnimating),
        animationProgress(other.animationProgress),
        baseKeyOffset(other.baseKeyOffset),
        baseKeySizeOffset(other.baseKeySizeOffset),
        baseKeyRotationOffset(other.baseKeyRotationOffset),
        cornerRadius(other.cornerRadius),
        borderThickness(other.borderThickness),
        usePerSideBorderThicknesses(other.usePerSideBorderThicknesses), 
        fillColor(other.fillColor),
        borderColor(other.borderColor),
        usePerSideBorderColors(other.usePerSideBorderColors),
        shadowColor(other.shadowColor),
        shadowSpread(other.shadowSpread),
        shadowOffset(other.shadowOffset),
        shadowUseCornerRadius(other.shadowUseCornerRadius),
        shadowInset(other.shadowInset),
        blurAmount(other.blurAmount),
        visible(other.visible),
        locked(other.locked),
        useGradient(other.useGradient),
        gradientRotation(other.gradientRotation),
        gradientInterpolation(other.gradientInterpolation),
        colorRamp(std::move(other.colorRamp)),
        shadowRotation(other.shadowRotation),
        useGlass(other.useGlass),
        glassBlur(other.glassBlur),
        glassAlpha(other.glassAlpha),
        glassColor(other.glassColor),
        zOrder(other.zOrder),
        isButton(other.isButton),
        assignedChildWindows(std::move(other.assignedChildWindows)),
        selectedChildWindowIndex(other.selectedChildWindowIndex),
        logicExpression(std::move(other.logicExpression)),
        logicAction(std::move(other.logicAction)),
        clickCounter(other.clickCounter),
        buttonBehavior(other.buttonBehavior),
        triggerEvent(other.triggerEvent),
        buttonState(other.buttonState),
        shouldCallOnClick(other.shouldCallOnClick),
        isClicked(other.isClicked),
        hoverColor(other.hoverColor),
        clickedColor(other.clickedColor),
        useOnClick(other.useOnClick),
        onClick(std::move(other.onClick)),
        storedOnClick(std::move(other.storedOnClick)),
        parent(other.parent),
        children(std::move(other.children)),
        originalPosition(other.originalPosition),
        originalRotation(other.originalRotation),
        originalSize(other.originalSize),
        ownerWindow(std::move(other.ownerWindow)),
        toggledStatePositionOffset(other.toggledStatePositionOffset),
        toggledStateSizeOffset(other.toggledStateSizeOffset),
        toggledStateRotationOffset(other.toggledStateRotationOffset),
        sizeThresholds(std::move(other.sizeThresholds)),
        hasEmbeddedImage(other.hasEmbeddedImage),
        embeddedImageData(std::move(other.embeddedImageData)),
        embeddedImageWidth(other.embeddedImageWidth),
        embeddedImageHeight(other.embeddedImageHeight),
        embeddedImageChannels(other.embeddedImageChannels),
        embeddedImageIndex(other.embeddedImageIndex),
        embeddedImageTexture(other.embeddedImageTexture),
        imageDirty(other.imageDirty),
        eventHandlers(std::move(other.eventHandlers)),
        anchorMode(other.anchorMode),
        anchorMargin(other.anchorMargin),
        usePercentagePos(other.usePercentagePos),
        percentagePos(other.percentagePos),
        usePercentageSize(other.usePercentageSize),
        percentageSize(other.percentageSize),
        isLayoutContainer(other.isLayoutContainer),
        layoutManager(std::move(other.layoutManager)),
        stretchFactor(other.stretchFactor),
        horizontalAlignment(other.horizontalAlignment),
        verticalAlignment(other.verticalAlignment),
        positioningMode(other.positioningMode),
        constraints(std::move(other.constraints)),
        flexGrow(other.flexGrow),
        flexShrink(other.flexShrink),
        flexBasisMode(other.flexBasisMode),
        flexBasisPixels(other.flexBasisPixels),
        alignSelf(other.alignSelf),
        order(other.order),
        gridColumnStart(other.gridColumnStart),
        gridColumnEnd(other.gridColumnEnd),
        gridRowStart(other.gridRowStart),
        gridRowEnd(other.gridRowEnd),
        justifySelf(other.justifySelf),
        alignSelfGrid(other.alignSelfGrid),
        padding(other.padding),
        margin(other.margin),
        boxSizing(other.boxSizing),
        isPolygon(other.isPolygon), 
        polygonVertices(other.polygonVertices) 
    {
        for (int i = 0; i < 4; ++i) { 
            borderSideColors[i] = other.borderSideColors[i]; 
        }
        for (int i = 0; i < 4; ++i) { 
            borderSideThicknesses[i] = other.borderSideThicknesses[i]; 
        }
        for (int i = 0; i < 2; ++i) {
            glassBlurFBO[i] = other.glassBlurFBO[i];
            glassBlurTex[i] = other.glassBlurTex[i];
            other.glassBlurFBO[i] = 0; 
            other.glassBlurTex[i] = 0;
        }

        other.parent = nullptr;
        other.currentAnimation = nullptr;
        other.embeddedImageTexture = 0;
        other.boxSizing = BoxSizing::StrokeBox;
    }

    
    ShapeItem& ShapeItem::operator=(ShapeItem&& other) noexcept {
        if (this == &other) return *this;

        id = other.id;
        type = other.type;
        name = std::move(other.name);
        position = other.position;
        size = other.size;
        basePosition = other.basePosition;
        baseSize = other.baseSize;
        rotation = other.rotation;
        baseRotation = other.baseRotation;
        minSize = other.minSize;
        maxSize = other.maxSize;
        isChildWindow = other.isChildWindow;
        childWindowSync = other.childWindowSync;
        toggleChildWindow = other.toggleChildWindow;
        toggleBehavior = other.toggleBehavior;
        childWindowGroupId = other.childWindowGroupId;
        targetShapeID = other.targetShapeID;
        triggerGroupID = other.triggerGroupID;
        isImGuiContainer = other.isImGuiContainer;
        renderImGuiContent = std::move(other.renderImGuiContent);
        allowItemOverlap = other.allowItemOverlap;
        forceOverlap = other.forceOverlap;
        blockUnderlying = other.blockUnderlying;
        hasText = other.hasText;
        text = std::move(other.text);
        textColor = other.textColor;
        textSize = other.textSize;
        textFont = other.textFont;
        textPosition = other.textPosition;
        textRotation = other.textRotation;
        textAlignment = other.textAlignment;
        dynamicTextSize = other.dynamicTextSize;
        baseTextSize = other.baseTextSize;
        minTextSize = other.minTextSize;
        maxTextSize = other.maxTextSize;
        shapeKeys = std::move(other.shapeKeys);
        shapeKeyAnimations = std::move(other.shapeKeyAnimations);
        shapeKeyValue = other.shapeKeyValue;
        onClickAnimations = std::move(other.onClickAnimations);
        chainAnimation = std::move(other.chainAnimation);
        isPressed = other.isPressed;
        groupId = other.groupId;
        currentAnimation = other.currentAnimation;
        updateAnimBaseOnResize = other.updateAnimBaseOnResize;
        isHeld = other.isHeld;
        isAnimating = other.isAnimating;
        animationProgress = other.animationProgress;
        baseKeyOffset = other.baseKeyOffset;
        baseKeySizeOffset = other.baseKeySizeOffset;
        baseKeyRotationOffset = other.baseKeyRotationOffset;
        cornerRadius = other.cornerRadius;
        borderThickness = other.borderThickness;
        usePerSideBorderThicknesses = other.usePerSideBorderThicknesses; 
        for (int i = 0; i < 4; ++i) { 
            borderSideThicknesses[i] = other.borderSideThicknesses[i];
        }
        fillColor = other.fillColor;
        borderColor = other.borderColor;
        usePerSideBorderColors = other.usePerSideBorderColors;
        for (int i = 0; i < 4; ++i) { 
            borderSideColors[i] = other.borderSideColors[i];
        }
        shadowColor = other.shadowColor;
        shadowSpread = other.shadowSpread;
        shadowOffset = other.shadowOffset;
        shadowUseCornerRadius = other.shadowUseCornerRadius;
        shadowInset = other.shadowInset;
        blurAmount = other.blurAmount;
        visible = other.visible;
        locked = other.locked;
        useGradient = other.useGradient;
        gradientRotation = other.gradientRotation;
        gradientInterpolation = other.gradientInterpolation;
        colorRamp = std::move(other.colorRamp);
        shadowRotation = other.shadowRotation;
        useGlass = other.useGlass;
        glassBlur = other.glassBlur;
        glassAlpha = other.glassAlpha;
        glassColor = other.glassColor;
        zOrder = other.zOrder;
        isButton = other.isButton;
        assignedChildWindows = std::move(other.assignedChildWindows);
        selectedChildWindowIndex = other.selectedChildWindowIndex;
        logicExpression = std::move(other.logicExpression);
        logicAction = std::move(other.logicAction);
        clickCounter = other.clickCounter;
        buttonBehavior = other.buttonBehavior;
        triggerEvent = other.triggerEvent;
        buttonState = other.buttonState;
        shouldCallOnClick = other.shouldCallOnClick;
        isClicked = other.isClicked;
        hoverColor = other.hoverColor;
        clickedColor = other.clickedColor;
        useOnClick = other.useOnClick;
        onClick = std::move(other.onClick);
        storedOnClick = std::move(other.storedOnClick);
        parent = other.parent;
        children = std::move(other.children);
        originalPosition = other.originalPosition;
        originalRotation = other.originalRotation;
        originalSize = other.originalSize;
        ownerWindow = std::move(other.ownerWindow);
        toggledStatePositionOffset = other.toggledStatePositionOffset;
        toggledStateSizeOffset = other.toggledStateSizeOffset;
        toggledStateRotationOffset = other.toggledStateRotationOffset;
        sizeThresholds = std::move(other.sizeThresholds);
        hasEmbeddedImage = other.hasEmbeddedImage;
        embeddedImageData = std::move(other.embeddedImageData);
        embeddedImageWidth = other.embeddedImageWidth;
        embeddedImageHeight = other.embeddedImageHeight;
        embeddedImageChannels = other.embeddedImageChannels;
        embeddedImageIndex = other.embeddedImageIndex;
        embeddedImageTexture = other.embeddedImageTexture;
        imageDirty = other.imageDirty;
        eventHandlers = std::move(other.eventHandlers);
        anchorMode = other.anchorMode;
        anchorMargin = other.anchorMargin;
        usePercentagePos = other.usePercentagePos;
        percentagePos = other.percentagePos;
        usePercentageSize = other.usePercentageSize;
        percentageSize = other.percentageSize;
        isLayoutContainer = other.isLayoutContainer;
        layoutManager = std::move(other.layoutManager);
        stretchFactor = other.stretchFactor;
        horizontalAlignment = other.horizontalAlignment;
        verticalAlignment = other.verticalAlignment;
        positioningMode = other.positioningMode;
        constraints = std::move(other.constraints);
        flexGrow = other.flexGrow;
        flexShrink = other.flexShrink;
        flexBasisMode = other.flexBasisMode;
        flexBasisPixels = other.flexBasisPixels;
        alignSelf = other.alignSelf;
        order = other.order;
        gridColumnStart = other.gridColumnStart;
        gridColumnEnd = other.gridColumnEnd;
        gridRowStart = other.gridRowStart;
        gridRowEnd = other.gridRowEnd;
        justifySelf = other.justifySelf;
        alignSelfGrid = other.alignSelfGrid;
        padding = other.padding;
        margin = other.margin;
        boxSizing = other.boxSizing;
        isPolygon = other.isPolygon; 
        polygonVertices = other.polygonVertices; 

        for (int i = 0; i < 2; ++i) {
            glassBlurFBO[i] = other.glassBlurFBO[i];
            glassBlurTex[i] = other.glassBlurTex[i];
            other.glassBlurFBO[i] = 0;
            other.glassBlurTex[i] = 0;
        }
        other.parent = nullptr;
        other.currentAnimation = nullptr;
        other.embeddedImageTexture = 0;
        other.boxSizing = BoxSizing::StrokeBox;
        return *this;
    }

    Layer::Layer(const std::string& n)
        : id(0), name(n), visible(true), locked(false), zOrder(0)
    {
    }

    
    Layer::Layer(const Layer& other)
        : id(other.id),
        name(other.name),
        visible(other.visible),
        locked(other.locked),
        zOrder(other.zOrder)
    {
        shapes.reserve(other.shapes.size());
        for (const auto& shape_uptr : other.shapes) {
            if (shape_uptr) {
                shapes.push_back(std::make_unique<ShapeItem>(*shape_uptr)); 
            }
            else {
                shapes.push_back(nullptr);
            }
        }
    }

    
    Layer& Layer::operator=(const Layer& other) {
        if (this == &other) return *this;

        id = other.id;
        name = other.name;
        visible = other.visible;
        locked = other.locked;
        zOrder = other.zOrder;

        shapes.clear();
        shapes.reserve(other.shapes.size());
        for (const auto& shape_uptr : other.shapes) {
            if (shape_uptr) {
                shapes.push_back(std::make_unique<ShapeItem>(*shape_uptr)); 
            }
            else {
                shapes.push_back(nullptr);
            }
        }
        return *this;
    }

    
    Layer::Layer(Layer&& other) noexcept
        : id(other.id),
        name(std::move(other.name)),
        shapes(std::move(other.shapes)),
        visible(other.visible),
        locked(other.locked),
        zOrder(other.zOrder)
    {
        other.id = 0;
        other.visible = true;
        other.locked = false;
        other.zOrder = 0;
    }

    
    Layer& Layer::operator=(Layer&& other) noexcept {
        if (this == &other) return *this;

        id = other.id;
        name = std::move(other.name);
        shapes = std::move(other.shapes);
        visible = other.visible;
        locked = other.locked;
        zOrder = other.zOrder;

        other.id = 0;
        other.visible = true;
        other.locked = false;
        other.zOrder = 0;
        return *this;
    }

    GridLayout::GridLayout() {
        GridTrackSize colTrack1;
        colTrack1.mode = GridTrackSize::Mode::Fraction;
        colTrack1.value = 1.0f;
        templateColumns.push_back(colTrack1);

        GridTrackSize colTrack2;
        colTrack2.mode = GridTrackSize::Mode::Fraction;
        colTrack2.value = 1.0f;
        templateColumns.push_back(colTrack2);

        GridTrackSize rowTrack1;
        templateRows.push_back(rowTrack1);

        rowGap = { 5.0f, LengthUnit::Unit::Px };
        columnGap = { 5.0f, LengthUnit::Unit::Px };
    }


    void ClearGradientTextureCache()
    {
        for (auto& pair : gradientTextureCache)
        {
            glDeleteTextures(1, (GLuint*)&pair.second);
        }
        gradientTextureCache.clear();
    }

    const char* VerticalLayout::getTypeName() const { return "Vertical"; }
    void VerticalLayout::doLayout(ShapeItem& container, const ImVec2& availableSize)
    {
        const float paddingLeft = container.padding.x;
        const float paddingRight = container.padding.z;
        const float paddingTop = container.padding.y;
        const float paddingBottom = container.padding.w;

        float availableInnerWidth = std::max(0.0f, availableSize.x - paddingLeft - paddingRight);
        float availableInnerHeight = std::max(0.0f, availableSize.y - paddingTop - paddingBottom);

        float totalStretch = 0.0f;
        float totalFixedY = 0.0f;
        int visibleChildrenCount = 0;

        if (container.children.empty()) {
            return;
        }

        for (ShapeItem* child : container.children) {
            if (!child || !child->visible) continue;
            visibleChildrenCount++;
            const float marginTop = child->margin.y;
            const float marginBottom = child->margin.w;
            const float totalVerticalMargin = marginTop + marginBottom;

            if (child->verticalAlignment != VAlignment::Fill && child->stretchFactor <= 0.0f) {
                totalFixedY += child->size.y + totalVerticalMargin;
            }
            else {
                totalStretch += std::max(0.0f, child->stretchFactor);
                totalFixedY += totalVerticalMargin;
            }
        }

        float totalSpacing = (visibleChildrenCount > 1) ? (spacing * (visibleChildrenCount - 1)) : 0.0f;
        totalFixedY += totalSpacing;
        float availableStretchY = std::max(0.0f, availableInnerHeight - totalFixedY);

        float currentY = paddingTop;

        for (ShapeItem* child : container.children) {
            if (!child || !child->visible) continue;

            const float marginLeft = child->margin.x;
            const float marginRight = child->margin.z;
            const float marginTop = child->margin.y;
            const float marginBottom = child->margin.w;
            const float totalVerticalMargin = marginTop + marginBottom;
            const float totalHorizontalMargin = marginLeft + marginRight;

            float childAvailableOuterWidth = availableInnerWidth;
            float childAvailableInnerWidth = std::max(0.0f, childAvailableOuterWidth - totalHorizontalMargin);

            float childHeight = child->size.y;
            if (child->verticalAlignment == VAlignment::Fill || child->stretchFactor > 0.0f) {
                float availableHeightForChild = 0;
                if (totalStretch > 1e-6f && child->stretchFactor > 0.0f) {
                    availableHeightForChild = (child->stretchFactor / totalStretch) * availableStretchY;
                }
                else if (child->verticalAlignment == VAlignment::Fill && totalStretch <= 1e-6f) {
                    int fillCount = std::max(1, (int)std::count_if(container.children.begin(), container.children.end(),
                        [](ShapeItem* c) { return c && c->visible && (c->verticalAlignment == VAlignment::Fill || c->stretchFactor > 0.0f); }));
                    availableHeightForChild = availableStretchY / fillCount;
                }
                childHeight = availableHeightForChild;
            }
            childHeight = std::max(child->minSize.y, std::min(childHeight, child->maxSize.y));
            childHeight = std::max(0.0f, childHeight);

            float childWidth = child->size.x;
            if (child->horizontalAlignment == HAlignment::Fill) {
                childWidth = childAvailableInnerWidth;
            }
            childWidth = std::max(child->minSize.x, std::min(childWidth, child->maxSize.x));
            childWidth = std::max(0.0f, childWidth);

            float borderBoxY = currentY + marginTop;
            float borderBoxX = paddingLeft + marginLeft;

            float outerWidth = childWidth + totalHorizontalMargin;
            float horizontalRemainingSpace = childAvailableOuterWidth - outerWidth;
            if (child->horizontalAlignment == HAlignment::Center) {
                borderBoxX += horizontalRemainingSpace * 0.5f;
            }
            else if (child->horizontalAlignment == HAlignment::Right) {
                borderBoxX += horizontalRemainingSpace;
            }
            else if (child->horizontalAlignment == HAlignment::Fill) {
                childWidth = childAvailableInnerWidth;
                childWidth = std::max(child->minSize.x, std::min(childWidth, child->maxSize.x));
                childWidth = std::max(0.0f, childWidth);
            }

            child->position = ImVec2(borderBoxX, borderBoxY);
            child->size = ImVec2(childWidth, childHeight);

            currentY += childHeight + totalVerticalMargin + spacing;
        }
    }
    const char* HorizontalLayout::getTypeName() const { return "Horizontal"; }
    void HorizontalLayout::doLayout(ShapeItem& container, const ImVec2& availableSize)
    {
        const float paddingLeft = container.padding.x;
        const float paddingRight = container.padding.z;
        const float paddingTop = container.padding.y;
        const float paddingBottom = container.padding.w;

        float availableInnerWidth = std::max(0.0f, availableSize.x - paddingLeft - paddingRight);
        float availableInnerHeight = std::max(0.0f, availableSize.y - paddingTop - paddingBottom);

        float totalStretch = 0.0f;
        float totalFixedX = 0.0f;
        int visibleChildrenCount = 0;

        if (container.children.empty()) {
            return;
        }

        for (ShapeItem* child : container.children) {
            if (!child || !child->visible) continue;
            visibleChildrenCount++;
            const float marginLeft = child->margin.x;
            const float marginRight = child->margin.z;
            const float totalHorizontalMargin = marginLeft + marginRight;

            if (child->horizontalAlignment != HAlignment::Fill && child->stretchFactor <= 0.0f) {
                totalFixedX += child->size.x + totalHorizontalMargin;
            }
            else {
                totalStretch += std::max(0.0f, child->stretchFactor);
                totalFixedX += totalHorizontalMargin;
            }
        }

        float totalSpacing = (visibleChildrenCount > 1) ? (spacing * (visibleChildrenCount - 1)) : 0.0f;
        totalFixedX += totalSpacing;
        float availableStretchX = std::max(0.0f, availableInnerWidth - totalFixedX);

        float currentX = paddingLeft;

        for (ShapeItem* child : container.children) {
            if (!child || !child->visible) continue;

            const float marginLeft = child->margin.x;
            const float marginRight = child->margin.z;
            const float marginTop = child->margin.y;
            const float marginBottom = child->margin.w;
            const float totalHorizontalMargin = marginLeft + marginRight;
            const float totalVerticalMargin = marginTop + marginBottom;

            float childAvailableOuterHeight = availableInnerHeight;
            float childAvailableInnerHeight = std::max(0.0f, childAvailableOuterHeight - totalVerticalMargin);

            float childWidth = child->size.x;
            if (child->horizontalAlignment == HAlignment::Fill || child->stretchFactor > 0.0f) {
                float availableWidthForChild = 0;
                if (totalStretch > 1e-6f && child->stretchFactor > 0.0f) {
                    availableWidthForChild = (child->stretchFactor / totalStretch) * availableStretchX;
                }
                else if (child->horizontalAlignment == HAlignment::Fill && totalStretch <= 1e-6f) {
                    int fillCount = std::max(1, (int)std::count_if(container.children.begin(), container.children.end(),
                        [](ShapeItem* c) { return c && c->visible && (c->horizontalAlignment == HAlignment::Fill || c->stretchFactor > 0.0f); }));
                    availableWidthForChild = availableStretchX / fillCount;
                }
                childWidth = availableWidthForChild;
            }
            childWidth = std::max(child->minSize.x, std::min(childWidth, child->maxSize.x));
            childWidth = std::max(0.0f, childWidth);

            float childHeight = child->size.y;
            if (child->verticalAlignment == VAlignment::Fill) {
                childHeight = childAvailableInnerHeight;
            }
            childHeight = std::max(child->minSize.y, std::min(childHeight, child->maxSize.y));
            childHeight = std::max(0.0f, childHeight);

            float borderBoxX = currentX + marginLeft;
            float borderBoxY = paddingTop + marginTop;

            float outerHeight = childHeight + totalVerticalMargin;
            float verticalRemainingSpace = childAvailableOuterHeight - outerHeight;
            if (child->verticalAlignment == VAlignment::Center) {
                borderBoxY += verticalRemainingSpace * 0.5f;
            }
            else if (child->verticalAlignment == VAlignment::Bottom) {
                borderBoxY += verticalRemainingSpace;
            }
            else if (child->verticalAlignment == VAlignment::Fill) {
                childHeight = childAvailableInnerHeight;
                childHeight = std::max(child->minSize.y, std::min(childHeight, child->maxSize.y));
                childHeight = std::max(0.0f, childHeight);
            }

            child->position = ImVec2(borderBoxX, borderBoxY);
            child->size = ImVec2(childWidth, childHeight);

            currentX += childWidth + totalHorizontalMargin + spacing;
        }
    }

    bool CompareShapesByZOrder(const ShapeItem& a, const ShapeItem& b) { return a.zOrder < b.zOrder; }
    bool CompareLayersByZOrder(const Layer& a, const Layer& b) { return a.zOrder < b.zOrder; }

    std::vector<ShapeItem*> GetAllShapes() {
        std::vector<ShapeItem*> shapes_ptrs;
        for (auto& [winName, windowData] : g_windowsMap) {
            for (auto& layer : windowData.layers) {
                for (auto& shape_uptr : layer.shapes) {
                    if (shape_uptr) shapes_ptrs.push_back(shape_uptr.get());
                }
            }
        }
        return shapes_ptrs;
    }
    std::vector<ShapeItem*> GetAllButtonShapes() {
        std::vector<ShapeItem*> buttonShapes;
        for (auto& [winName, winData] : g_windowsMap) {
            for (auto& layer : winData.layers) {
                for (auto& shape_uptr : layer.shapes) {
                    if (shape_uptr && shape_uptr->isButton)
                        buttonShapes.push_back(shape_uptr.get());
                }
            }
        }
        return buttonShapes;
    }

    void RegisterWindow(std::string name, std::function<void()> renderFunc)
    {
        if (name.empty())
        {
            static int childWindowCounter = 0;
            name = "ChildWindow_" + std::to_string(++childWindowCounter);
        }
        g_windowsMap[name].renderFunc = renderFunc;
        ImGui::SetNextItemAllowOverlap();
    }

    void SetWindowOpen(const std::string& name, bool open)
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
            for (auto shape_ptr : GetAllShapes())
            {
                if (shape_ptr && shape_ptr->name == name)
                {
                    shape_ptr->isChildWindow = open;
                    break;
                }
            }
        }
    }

    bool IsWindowOpen(const std::string& name)
    {
        auto it = g_windowsMap.find(name);
        if (it != g_windowsMap.end())
            return it->second.isOpen;
        return false;
    }

    ShapeItem* FindShapeByID(int shapeID) {
        for (auto& [winName, windowData] : DesignManager::g_windowsMap) {
            for (auto& layer : windowData.layers) {
                for (auto& shape_uptr : layer.shapes) {
                    if (shape_uptr && shape_uptr->id == shapeID) {
                        return shape_uptr.get();
                    }
                }
            }
        }
        return nullptr;
    }

    void UpdateGlobalScaleFactor(int currentW, int currentH)
    {
        float scaleX = currentW / minWindowSize.x;
        float scaleY = currentH / minWindowSize.y;
        globalScaleFactor = std::min(scaleX, scaleY);
    }

    void RemoveParent(ShapeItem* child)
    {
        if (child->parent == nullptr) return;
        auto& children = child->parent->children;
        children.erase(std::remove(children.begin(), children.end(), child), children.end());
        child->parent = nullptr;
    }
    void SetParent(ShapeItem* child, ShapeItem* parent)
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

    void RemoveParentKeepTransform(ShapeItem* child)
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
    void DrawGridLayoutDebug(ShapeItem& container, ImDrawList* dl)
    {
        if (!container.isLayoutContainer || !dl || !container.layoutManager) return;
        GridLayout* gridLayout = dynamic_cast<GridLayout*>(container.layoutManager.get());
        if (!gridLayout) return;

        ImVec2 containerWorldPos = AddV(container.position, ImGui::GetWindowPos());
        ImVec2 containerSize = container.size;

        float padLeftPx = container.padding.x;
        float padTopPx = container.padding.y;
        float padRightPx = container.padding.z;
        float padBottomPx = container.padding.w;
        float colGapPx = gridLayout->columnGap.getPixels(containerSize.x);
        float rowGapPx = gridLayout->rowGap.getPixels(containerSize.y);

        const float availableInnerWidth = std::max(0.0f, containerSize.x - padLeftPx - padRightPx);
        const float availableInnerHeight = std::max(0.0f, containerSize.y - padTopPx - padBottomPx);

        auto compute_track_sizes_debug = [&](const std::vector<GridTrackSize>& templates, float available_space, float gap_px) -> std::vector<float> {
            size_t track_count = templates.size();
            std::vector<float> sizes;
            const float default_auto_size = 50.0f;

            if (track_count == 0) {
                track_count = 1;
                sizes.resize(track_count, default_auto_size);
            }
            else {
                sizes.resize(track_count, 0.0f);
            }
            float total_fixed_percent_auto_size = 0; float total_fr = 0;
            std::vector<size_t> fr_indices; std::vector<size_t> auto_indices;
            if (templates.empty()) {
                total_fixed_percent_auto_size = sizes[0];
                auto_indices.push_back(0);
            }
            else {
                for (size_t i = 0; i < templates.size(); ++i) {
                    const auto& track = templates[i];
                    if (track.mode == GridTrackSize::Mode::Fixed) { sizes[i] = std::max(0.0f, track.value); total_fixed_percent_auto_size += sizes[i]; }
                    else if (track.mode == GridTrackSize::Mode::Percentage) { sizes[i] = std::max(0.0f, available_space * (track.value / 100.0f)); total_fixed_percent_auto_size += sizes[i]; }
                    else if (track.mode == GridTrackSize::Mode::Fraction) { total_fr += std::max(0.0f, track.value); fr_indices.push_back(i); sizes[i] = 0; }
                    else { sizes[i] = default_auto_size; auto_indices.push_back(i); total_fixed_percent_auto_size += sizes[i]; }
                }
            }
            float total_gap = (track_count > 1) ? (gap_px * (track_count - 1)) : 0.0f;
            float remaining_space_for_fr = available_space - total_fixed_percent_auto_size - total_gap;
            if (total_fr > 1e-6f) {
                float fr_unit_size = (remaining_space_for_fr > 1e-6f) ? (remaining_space_for_fr / total_fr) : 0.0f;
                for (size_t i : fr_indices) { if (i < sizes.size()) sizes[i] = std::max(0.0f, templates[i].value * fr_unit_size); }
            }
            else {
                for (size_t i : fr_indices) { if (i < sizes.size()) sizes[i] = 0.0f; }
            }
            for (float& s : sizes) { s = std::max(0.0f, s); }
            return sizes;
            };
        std::vector<float> colSizesDebug = compute_track_sizes_debug(gridLayout->templateColumns, availableInnerWidth, colGapPx);
        std::vector<float> rowSizesDebug = compute_track_sizes_debug(gridLayout->templateRows, availableInnerHeight, rowGapPx);

        auto compute_positions_debug = [&](const std::vector<float>& sizes, float start_offset_px, float gap_px) -> std::vector<float> {
            std::vector<float> positions; float current_pos = start_offset_px; positions.push_back(current_pos);
            const float default_implicit_size = 50.0f;
            if (sizes.empty()) {
                positions.push_back(start_offset_px + default_implicit_size);
            }
            else {
                for (size_t i = 0; i < sizes.size(); ++i) {
                    current_pos += sizes[i];
                    if (i < sizes.size() - 1) {
                        current_pos += gap_px;
                    }
                    positions.push_back(current_pos);
                }
            }
            return positions;
            };
        std::vector<float> colPositionsDebug = compute_positions_debug(colSizesDebug, containerWorldPos.x + padLeftPx, colGapPx);
        std::vector<float> rowPositionsDebug = compute_positions_debug(rowSizesDebug, containerWorldPos.y + padTopPx, rowGapPx);

        ImU32 gridLineColor = IM_COL32(255, 0, 0, 100);

        float gridTop = containerWorldPos.y + padTopPx;
        float gridBottom = rowPositionsDebug.empty() ? (containerWorldPos.y + containerSize.y - padBottomPx) : rowPositionsDebug.back();

        for (float xPos : colPositionsDebug) {
            dl->AddLine(ImVec2(xPos, gridTop), ImVec2(xPos, gridBottom), gridLineColor, 1.0f);
        }

        float gridLeft = containerWorldPos.x + padLeftPx;
        float gridRight = colPositionsDebug.empty() ? (containerWorldPos.x + containerSize.x - padRightPx) : colPositionsDebug.back();

        for (float yPos : rowPositionsDebug) {
            dl->AddLine(ImVec2(gridLeft, yPos), ImVec2(gridRight, yPos), gridLineColor, 1.0f);
        }

        ImU32 paddingBoxColor = IM_COL32(0, 0, 255, 80);
        dl->AddRect(ImVec2(containerWorldPos.x + padLeftPx, containerWorldPos.y + padTopPx),
            ImVec2(containerWorldPos.x + containerSize.x - padRightPx, containerWorldPos.y + containerSize.y - padBottomPx),
            paddingBoxColor);
    }
    void DrawFlexLayoutDebug(ShapeItem& container, ImDrawList* dl)
    {
        if (!container.isLayoutContainer || !dl || !container.layoutManager) return;
        FlexLayout* flexLayout = dynamic_cast<FlexLayout*>(container.layoutManager.get());
        if (!flexLayout) return;

        ImVec2 containerPos = AddV(container.position, ImGui::GetWindowPos());
        ImVec2 containerSize = container.size;
        const float paddingLeft = container.padding.x;
        const float paddingRight = container.padding.z;
        const float paddingTop = container.padding.y;
        const float paddingBottom = container.padding.w;

        ImU32 paddingBoxColor = IM_COL32(0, 255, 0, 80);
        dl->AddRect(ImVec2(containerPos.x + paddingLeft, containerPos.y + paddingTop),
            ImVec2(containerPos.x + containerSize.x - paddingRight, containerPos.y + containerSize.y - paddingBottom),
            paddingBoxColor);
    }
    void DrawLayoutItemBoundsDebug(ShapeItem& container, ImDrawList* dl)
    {
        if (!g_ShowLayoutDebugLines || !dl || container.children.empty()) {
            return;
        }

        ImVec2 containerWorldPos = AddV(container.position, ImGui::GetWindowPos());
        ImU32 itemBoundsColor = IM_COL32(0, 0, 255, 120);

        for (ShapeItem* child : container.children)
        {
            if (child && child->visible && child->positioningMode == PositioningMode::Relative)
            {
                ImVec2 childLocalPos = child->position;
                ImVec2 childSize = child->size;
                ImVec2 childTopLeftWorld = AddV(containerWorldPos, childLocalPos);
                ImVec2 childBottomRightWorld = AddV(childTopLeftWorld, childSize);
                dl->AddRect(childTopLeftWorld, childBottomRightWorld, itemBoundsColor, 0.0f, 0, 1.0f);
            }
        }
    }

    void DrawShape_RenderImGuiContent(ImDrawList* dl, ShapeItem& s, ImVec2 actualPos_World, ImVec2 actualSizePx, float scaleFactor) {
        if (!s.isImGuiContainer) return;

        if (std::fabs(s.rotation) > 1e-4f) {
            return;
        }

        std::string childId = "ImGuiContainer_" + std::to_string(s.id);

        ImVec2 childPos = actualPos_World; 
        ImVec2 childSize = actualSizePx;   

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

    void BuildRectPoly(std::vector<ImVec2>& poly, ImVec2 pos, ImVec2 size, float r, ImVec2 c, float rot)
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

    void BuildCirclePoly(std::vector<ImVec2>& poly, ImVec2 c, float rx, float ry, float rot)
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

    ImTextureID CreateWhiteMaskTexture(int width, int height)
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

    ImTextureID CreateGradientTexture(const ImVec2& size, float gradRotation, const std::vector<std::pair<float, ImVec4>>& colorRamp, DesignManager::ShapeItem::GradientInterpolation interpolationType)
    {
        int width = std::max(1, (int)size.x); 
        int height = std::max(1, (int)size.y);
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
                                float localT_ = (t - sortedRamp[i].first) / (sortedRamp[i + 1].first - sortedRamp[i].first);
                                localT_ = ImClamp(localT_, 0.0f, 1.0f);
                                ImVec4 p0 = sortedRamp[p0_index].second;
                                ImVec4 p1 = sortedRamp[i].second;
                                ImVec4 p2 = sortedRamp[p2_index].second;
                                ImVec4 p3 = sortedRamp[p3_index].second;
                                color = CardinalSpline(p0, p1, p2, p3, localT_, 0.5f);
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

    void DrawGradient(ImDrawList* dl, const std::vector<ImVec2>& poly, float gradRotation, const std::vector<std::pair<float, ImVec4>>& colorRamp, const ShapeItem& s)
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

    void RenderTemporaryWindows()
    {
        for (auto& [wName, WindowData] : g_windowsMap)
        {
            for (auto& layer : WindowData.layers)
            {
                for (auto& shape_uptr : layer.shapes)
                {
                    if (shape_uptr && shape_uptr->childWindowSync)
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
                    for (auto& shape_uptr : layer.shapes)
                    {
                        if (shape_uptr && shape_uptr->id == shapeId)
                        {
                            theShape = shape_uptr.get();
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
            ImGui::Text("Bu pencere, onClick tanÃ„Â±mlanmamÃ„Â±Ã…Å¸ bir butona tÃ„Â±klama sonucu aÃƒÂ§Ã„Â±ldÃ„Â±.");
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
            if (ImGui::Button("Bu geÃƒÂ§ici pencereyi kapat"))
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

    void AddTextRotated(
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

    void DrawShape_Shadow(ImDrawList* dlEffective, ShapeItem& s, ImVec2 wp, float scaleFactor, ImVec2 c, float totalrot) {

        
        ImVec2 hostShapeScaledPos = s.position * scaleFactor;
        ImVec2 hostShapeScaledSize = s.size * scaleFactor;
        float hostShapeScaledCornerRadius = s.cornerRadius * scaleFactor;

        
        ImVec2 hostShapeScreenPos_TopLeft = wp + hostShapeScaledPos;
        ImVec2 hostShapeScreenCenter = wp + hostShapeScaledPos + hostShapeScaledSize * 0.5f;

        if (s.shadowInset) {
            
            
            
            
            
            
            

            ImVec2 shadowCasterPos = hostShapeScreenPos_TopLeft;
            ImVec2 shadowCasterSize = hostShapeScaledSize;
            float shadowCasterCornerRadius = hostShapeScaledCornerRadius;

            
            
            
            
            shadowCasterPos.x -= s.shadowSpread.x * scaleFactor;
            shadowCasterPos.y -= s.shadowSpread.y * scaleFactor;
            shadowCasterSize.x += (s.shadowSpread.x + s.shadowSpread.z) * scaleFactor;
            shadowCasterSize.y += (s.shadowSpread.y + s.shadowSpread.w) * scaleFactor;

            
            
            
            float spreadEffectOnRadius = std::max({ s.shadowSpread.x, s.shadowSpread.y, s.shadowSpread.z, s.shadowSpread.w }) * scaleFactor;
            shadowCasterCornerRadius = std::max(0.0f, hostShapeScaledCornerRadius + spreadEffectOnRadius);
            shadowCasterCornerRadius = std::min({ shadowCasterCornerRadius, shadowCasterSize.x / 2.0f, shadowCasterSize.y / 2.0f });


            
            shadowCasterPos.x += s.shadowOffset.x * scaleFactor;
            shadowCasterPos.y += s.shadowOffset.y * scaleFactor;

            
            ImVec2 shadowCasterCenter = shadowCasterPos + shadowCasterSize * 0.5f;

            std::vector<ImVec2> shpoly_caster;
            if (s.type == ShapeType::Rectangle) {
                BuildRectPoly(shpoly_caster, shadowCasterPos, shadowCasterSize, shadowCasterCornerRadius, shadowCasterCenter, totalrot);
            }
            else { 
                float rx = shadowCasterSize.x * 0.5f;
                float ry = shadowCasterSize.y * 0.5f;
                BuildCirclePoly(shpoly_caster, shadowCasterCenter, rx, ry, totalrot);
            }

            
            
            

            ImRect hostClipRect;
            if (s.type == ShapeType::Rectangle) {
                std::vector<ImVec2> hostPolyVerts;
                BuildRectPoly(hostPolyVerts, hostShapeScreenPos_TopLeft, hostShapeScaledSize, hostShapeScaledCornerRadius, hostShapeScreenCenter, s.rotation); 
                if (!hostPolyVerts.empty()) {
                    hostClipRect.Min = hostPolyVerts[0]; hostClipRect.Max = hostPolyVerts[0];
                    for (size_t k = 1; k < hostPolyVerts.size(); ++k) hostClipRect.Add(hostPolyVerts[k]);
                }
                else { 
                    hostClipRect = ImRect(hostShapeScreenPos_TopLeft, hostShapeScreenPos_TopLeft + hostShapeScaledSize);
                }
            }
            else { 
                ImVec2 circleCenter = hostShapeScreenCenter;
                float rMax = std::max(hostShapeScaledSize.x, hostShapeScaledSize.y) * 0.5f;
                hostClipRect = ImRect(circleCenter - ImVec2(rMax, rMax), circleCenter + ImVec2(rMax, rMax));
            }


            
            ImVec4 oldClipRect = dlEffective->CmdBuffer.back().ClipRect; 

            
            ImVec2 newClipMin = ImVec2(std::max(oldClipRect.x, hostClipRect.Min.x), std::max(oldClipRect.y, hostClipRect.Min.y));
            ImVec2 newClipMax = ImVec2(std::min(oldClipRect.z, hostClipRect.Max.x), std::min(oldClipRect.w, hostClipRect.Max.y));

            if (newClipMax.x > newClipMin.x && newClipMax.y > newClipMin.y) {
                dlEffective->PushClipRect(newClipMin, newClipMax, true); 
                if (!shpoly_caster.empty()) {
                    dlEffective->AddConvexPolyFilled(shpoly_caster.data(), (int)shpoly_caster.size(), ColU32(s.shadowColor));
                }
                dlEffective->PopClipRect();
            }

        }
        else {
            
            float cr = s.shadowUseCornerRadius ? s.cornerRadius * scaleFactor : 0.0f;
            
            ImVec2 spos = ImVec2(
                s.position.x * scaleFactor - s.shadowSpread.x * scaleFactor + s.shadowOffset.x * scaleFactor,
                s.position.y * scaleFactor - s.shadowSpread.y * scaleFactor + s.shadowOffset.y * scaleFactor
            );
            ImVec2 ssize = ImVec2(
                s.size.x * scaleFactor + (s.shadowSpread.x + s.shadowSpread.z) * scaleFactor,
                s.size.y * scaleFactor + (s.shadowSpread.y + s.shadowSpread.w) * scaleFactor
            );

            std::vector<ImVec2> shpoly;
            
            
            
            
            ImVec2 shadowGeomCenter_Screen = wp + spos + ssize * 0.5f;
            
            ImVec2 shadowRotationCenter_Screen = c + s.shadowOffset * scaleFactor; 

            if (s.type == ShapeType::Rectangle) {
                BuildRectPoly(shpoly, wp + spos, ssize, cr, shadowRotationCenter_Screen, totalrot);
            }
            else { 
                float rx = ssize.x * 0.5f;
                float ry = ssize.y * 0.5f;
                BuildCirclePoly(shpoly, shadowGeomCenter_Screen, rx, ry, totalrot); 
            }
            if (!shpoly.empty()) {
                dlEffective->AddConvexPolyFilled(shpoly.data(), (int)shpoly.size(), ColU32(s.shadowColor));
            }
        }
        
    }
    void DrawShape_Blur(ImDrawList* dlEffective, ShapeItem& s, ImVec2 wp, float scaleFactor, ImVec2 c) {
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

    void BuildMainShapePoly(ShapeItem& s, ImVec2 wp, float scaleFactor, ImVec2 c, std::vector<ImVec2>& poly) {
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

    void DrawShape_LoadEmbeddedImageIfNeeded(ShapeItem& item) {
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
            std::cerr << "Resim yÃƒÂ¼klenemedi: " << item.name << std::endl;
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

    void DrawShape_DrawEmbeddedImageIfAny(ImDrawList* dlEffective, ShapeItem& s, float scaleFactor, ImVec2 cImage, std::vector<ImVec2>& poly) {
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

    void DispatchEvent(ShapeItem& shape, const std::string& eventType) {
        for (auto& handler : shape.eventHandlers) {
            if (handler.eventType == eventType) {
                handler.handler(shape);
            }
        }
    }

    void DrawShape_ProcessButtonLogic(ImDrawList* dlEffective, ShapeItem& s, float scaleFactor, ImVec2 wp, ImVec4& drawColor) {
        if (DesignManager::g_IsInEditMode) {
            drawColor = s.fillColor;
            return;
        }
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

    void DrawShape_FillWithGradient(ImDrawList* dlEffective, const std::vector<ImVec2>& poly,
        const ShapeItem& shape,
        const ImVec2& fillActualPosPx_World,
        const ImVec2& fillActualSizePx,
        const ImVec2& fillActualCenterPx_World
    )
    {
        size_t n = poly.size();
        if (n < 3) {
            dlEffective->AddConvexPolyFilled(poly.data(), (int)n, ColU32(shape.fillColor));
            return;
        }

        int hashKeyBase = (int)(fillActualSizePx.x * 100.0f) ^ (int)(fillActualSizePx.y * 100.0f) ^ (int)(shape.gradientRotation * 100.0f);
        hashKeyBase = (hashKeyBase * 31 + static_cast<int>(shape.gradientInterpolation));
        for (const auto& pair_ : shape.colorRamp) {
            hashKeyBase = (hashKeyBase * 31 + static_cast<int>(pair_.first * 1000.0f)) * 31 + ColU32(pair_.second);
        }

        ImTextureID gradient_texture_id;
        auto it = gradientTextureCache.find(hashKeyBase);
        if (it == gradientTextureCache.end()) {
            gradient_texture_id = CreateGradientTexture(
                fillActualSizePx,
                shape.gradientRotation,
                shape.colorRamp,
                shape.gradientInterpolation
            );
            if (gradient_texture_id) gradientTextureCache[hashKeyBase] = gradient_texture_id;
        }
        else {
            gradient_texture_id = it->second;
        }

        if (!gradient_texture_id) {
            dlEffective->AddConvexPolyFilled(poly.data(), (int)poly.size(), ColU32(shape.fillColor));
            return;
        }

        dlEffective->PushTextureID(gradient_texture_id);
        std::vector<ImVec2> uvCoords(n);
        for (size_t i = 0; i < n; i++) {
            uvCoords[i] = UV(
                poly[i],
                fillActualSizePx,
                fillActualCenterPx_World,
                fillActualPosPx_World,
                shape.rotation
            );
        }
        for (size_t i = 1; i < n - 1; i++) {
            dlEffective->PrimReserve(3, 3);
            dlEffective->PrimVtx(poly[0], uvCoords[0], IM_COL32_WHITE);
            dlEffective->PrimVtx(poly[i], uvCoords[i], IM_COL32_WHITE);
            dlEffective->PrimVtx(poly[i + 1], uvCoords[i + 1], IM_COL32_WHITE);
        }
        dlEffective->PopTextureID();
    }

    void DrawShape_Fill(ImDrawList* dlEffective, ShapeItem& s, const std::vector<ImVec2>& poly,
        const ImVec2& fillActualPosPx_World, const ImVec2& fillActualSizePx,
        const ImVec2& fillActualCenterPx_World,
        ImVec4 drawColor)
    {
        if (s.useGradient && !poly.empty()) { 
            DrawShape_FillWithGradient(dlEffective, poly, s, fillActualPosPx_World, fillActualSizePx, fillActualCenterPx_World);
        }
        else if (!poly.empty()) { 
            dlEffective->AddConvexPolyFilled(poly.data(), (int)poly.size(), ColU32(drawColor));
        }
    }

    void DrawShape_RenderChildWindow(ShapeItem& s, ImVec2 contentActualPos_World, ImVec2 contentActualSizePx)
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
                            for (auto& sh_uptr : layer.shapes)
                            {
                                if (sh_uptr && sh_uptr->isButton && sh_uptr->id == mapping.buttonIds[i])
                                {
                                    btnState = sh_uptr->buttonState;
                                    goto found_button_state_none; 
                                }
                            }
                        }
                    }
                found_button_state_none:;
                    const std::string& childKey = mapping.childWindowKeys[i];
                    if (g_windowsMap.count(childKey)) { 
                        g_windowsMap[childKey].groupId = s.childWindowGroupId;
                        SetWindowOpen(childKey, btnState);
                    }
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
                            for (auto& sh_uptr : layer.shapes)
                            {
                                if (sh_uptr && sh_uptr->isButton && sh_uptr->id == btnId && sh_uptr->buttonState)
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
                                    for (auto& sh_uptr : layer.shapes) {
                                        if (sh_uptr && sh_uptr->isButton && sh_uptr->id == mapping.buttonIds[0])
                                            first = sh_uptr->buttonState;
                                        if (sh_uptr && sh_uptr->isButton && sh_uptr->id == mapping.buttonIds[1])
                                            second = sh_uptr->buttonState;
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
                                    for (auto& sh_uptr : layer.shapes) {
                                        if (sh_uptr && sh_uptr->isButton && sh_uptr->id == mapping.buttonIds[0]) {
                                            state = sh_uptr->buttonState;
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
                                    for (auto& sh_uptr : layer.shapes) {
                                        if (sh_uptr && sh_uptr->isButton && sh_uptr->id == mapping.buttonIds[0]) {
                                            firstState = sh_uptr->buttonState;
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
                                        for (auto& sh_uptr : layer.shapes) {
                                            if (sh_uptr && sh_uptr->isButton && sh_uptr->id == btnId) {
                                                state = sh_uptr->buttonState;
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

                for (const auto& childKey : mapping.childWindowKeys) 
                {
                    if (g_windowsMap.count(childKey)) { 
                        g_windowsMap[childKey].groupId = s.childWindowGroupId;
                        SetWindowOpen(childKey, conditionMet);
                    }
                }
            }

            for (const auto& childKey : mapping.childWindowKeys)
            {
                if (IsWindowOpen(childKey))
                {
                    if (s.childWindowSync)
                    {
                        
                        
                        ImGui::SetNextWindowPos(contentActualPos_World);
                        ImGui::SetNextWindowSize(contentActualSizePx);
                    }
                    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
                    
                    ImGui::SetCursorScreenPos(contentActualPos_World); 
                    ImGui::BeginChild(childKey.c_str(), contentActualSizePx, 
                        false, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground);

                    if (g_windowsMap.count(childKey))
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
                            std::stable_sort(layer.shapes.begin(), layer.shapes.end(), [](const auto& a, const auto& b) { return (a && b) ? CompareShapesByZOrder(*a, *b) : (a != nullptr); });
                            for (auto& childShape_uptr : layer.shapes)
                            {
                                
                                
                                if (childShape_uptr && childShape_uptr->ownerWindow == childKey && childShape_uptr->parent == nullptr)
                                    DrawShape(ImGui::GetWindowDrawList(), *childShape_uptr, ImGui::GetWindowPos());
                            }
                        }
                    }
                    else 
                    {
                        ShapeItem* targetShape = nullptr;
                        auto allShapes_ptrs = GetAllShapes();
                        for (auto sh_ptr : allShapes_ptrs)
                        {
                            if (sh_ptr && sh_ptr->name == childKey)
                            {
                                targetShape = sh_ptr;
                                break;
                            }
                        }
                        if (targetShape) 
                            DrawShape(ImGui::GetWindowDrawList(), *targetShape, ImGui::GetWindowPos());
                        else
                            ImGui::Text("Child window content not registered or shape not found.");
                    }
                    ImGui::EndChild();
                    ImGui::PopStyleVar();
                }
            }
        }
    }

    void DrawShape_DrawBorder(ImDrawList* dlEffective, ShapeItem& s, float scaleFactor, ImVec2 shapeCenter_World_from_caller, ImVec2 windowTopLeft_wp) {
        float globalBorderThicknessPx_Scaled = s.borderThickness * scaleFactor;

        float perSideBorderThicknessesPx_Scaled[4];
        if (s.usePerSideBorderThicknesses) {
            for (int i = 0; i < 4; ++i) {
                perSideBorderThicknessesPx_Scaled[i] = s.borderSideThicknesses[i] * scaleFactor;
            }
        }
        else {
            for (int i = 0; i < 4; ++i) {
                perSideBorderThicknessesPx_Scaled[i] = globalBorderThicknessPx_Scaled;
            }
        }

        float maxVisibleThicknessForPolyline = 0.0f;
        bool isAnyBorderVisible = false;

        if (s.usePerSideBorderThicknesses) {
            for (int i = 0; i < 4; ++i) {
                if (perSideBorderThicknessesPx_Scaled[i] > 0.001f) {
                    bool colorAllowsDrawing = false;
                    if (s.usePerSideBorderColors) {
                        if (s.borderSideColors[i].w > 0.0f) colorAllowsDrawing = true;
                    }
                    else {
                        if (s.borderColor.w > 0.0f) colorAllowsDrawing = true;
                    }
                    if (colorAllowsDrawing) {
                        isAnyBorderVisible = true;
                        maxVisibleThicknessForPolyline = std::max(maxVisibleThicknessForPolyline, perSideBorderThicknessesPx_Scaled[i]);
                    }
                }
            }
        }
        else {
            if (globalBorderThicknessPx_Scaled > 0.001f && s.borderColor.w > 0.0f) {
                isAnyBorderVisible = true;
                maxVisibleThicknessForPolyline = globalBorderThicknessPx_Scaled;
            }
        }

        if (!isAnyBorderVisible || maxVisibleThicknessForPolyline <= 0.001f) {
            return;
        }

        ImVec2 borderTopLeft_World = ImVec2(windowTopLeft_wp.x + s.position.x * scaleFactor,
            windowTopLeft_wp.y + s.position.y * scaleFactor);
        ImVec2 borderSize_Scaled = ImVec2(s.size.x * scaleFactor, s.size.y * scaleFactor);
        float cornerRadius_Scaled = s.cornerRadius * scaleFactor;

        
        
        
        

        if (borderSize_Scaled.x < 1e-3f || borderSize_Scaled.y < 1e-3f) {
            return;
        }

        if (s.type == ShapeType::Rectangle && cornerRadius_Scaled < 0.001f) {
            ImVec2 p[4];
            p[0] = borderTopLeft_World;
            p[1] = ImVec2(borderTopLeft_World.x + borderSize_Scaled.x, borderTopLeft_World.y);
            p[2] = borderTopLeft_World + borderSize_Scaled;
            p[3] = ImVec2(borderTopLeft_World.x, borderTopLeft_World.y + borderSize_Scaled.y);

            if (std::abs(s.rotation) > 1e-4f) {
                for (int i_pt = 0; i_pt < 4; ++i_pt) {
                    p[i_pt] = RotateP(p[i_pt], shapeCenter_World_from_caller, s.rotation);
                }
            }

            float current_thickness_final[4];
            ImU32 current_color_final[4];
            bool side_should_be_drawn[4];

            for (int i = 0; i < 4; ++i) {
                current_thickness_final[i] = s.usePerSideBorderThicknesses ? perSideBorderThicknessesPx_Scaled[i] : globalBorderThicknessPx_Scaled;
                ImVec4 side_color_v4 = s.usePerSideBorderColors ? s.borderSideColors[i] : s.borderColor;
                current_color_final[i] = ColU32(side_color_v4);
                side_should_be_drawn[i] = (current_thickness_final[i] > 0.001f && side_color_v4.w > 0.0f);
            }

            if (side_should_be_drawn[0])
                dlEffective->AddLine(p[0], p[1], current_color_final[0], current_thickness_final[0]);
            if (side_should_be_drawn[1])
                dlEffective->AddLine(p[1], p[2], current_color_final[1], current_thickness_final[1]);
            if (side_should_be_drawn[2])
                dlEffective->AddLine(p[2], p[3], current_color_final[2], current_thickness_final[2]);
            if (side_should_be_drawn[3])
                dlEffective->AddLine(p[3], p[0], current_color_final[3], current_thickness_final[3]);

        }
        else {
            ImVec2 polyPos_World = borderTopLeft_World;
            ImVec2 polySize_Scaled = borderSize_Scaled;
            float polyRadius_Scaled = cornerRadius_Scaled;
            float polylineThicknessToUse = maxVisibleThicknessForPolyline;

            
            
            
            polyPos_World = ImVec2(borderTopLeft_World.x + polylineThicknessToUse * 0.5f,
                borderTopLeft_World.y + polylineThicknessToUse * 0.5f);
            polySize_Scaled = ImVec2(std::max(0.0f, borderSize_Scaled.x - polylineThicknessToUse),
                std::max(0.0f, borderSize_Scaled.y - polylineThicknessToUse));
            polyRadius_Scaled = std::max(0.0f, cornerRadius_Scaled - polylineThicknessToUse * 0.5f);

            std::vector<ImVec2> borderPoly_World;
            
            
            
            ImVec2 centerForPolyBuild = polyPos_World + polySize_Scaled * 0.5f; 
            if (s.type == ShapeType::Rectangle) {
                BuildRectPoly(borderPoly_World, polyPos_World, polySize_Scaled, polyRadius_Scaled, shapeCenter_World_from_caller, s.rotation);
            }
            else if (s.type == ShapeType::Circle) {
                
                BuildCirclePoly(borderPoly_World, shapeCenter_World_from_caller, polySize_Scaled.x * 0.5f, polySize_Scaled.y * 0.5f, s.rotation);
            }

            if (!borderPoly_World.empty()) {
                ImU32 polyline_color_u32 = 0;
                bool polyline_color_is_valid = false;

                if (s.usePerSideBorderColors) {
                    for (int i = 0; i < 4; ++i) {
                        bool side_thick_enough_for_color = s.usePerSideBorderThicknesses ?
                            (perSideBorderThicknessesPx_Scaled[i] > 0.001f) :
                            (globalBorderThicknessPx_Scaled > 0.001f);
                        if (s.borderSideColors[i].w > 0.0f && side_thick_enough_for_color) {
                            polyline_color_u32 = ColU32(s.borderSideColors[i]);
                            polyline_color_is_valid = true;
                            break;
                        }
                    }
                }
                else {
                    if (s.borderColor.w > 0.0f) {
                        polyline_color_u32 = ColU32(s.borderColor);
                        polyline_color_is_valid = true;
                    }
                }

                if (polyline_color_is_valid && polylineThicknessToUse > 0.001f) {
                    dlEffective->AddPolyline(borderPoly_World.data(), (int)borderPoly_World.size(), polyline_color_u32, ImDrawFlags_Closed, polylineThicknessToUse);
                }
            }
        }
    }
    void DrawShape_DrawText(ImDrawList* dlEffective, ShapeItem& s, ImVec2 contentActualPos_World, ImVec2 contentActualSizePx, float scaleFactor) {
        if (s.hasText) {
            float computedTextSize = s.textSize * scaleFactor;
            if (s.dynamicTextSize && s.baseSize.x > 0.0f) {
                computedTextSize = s.textSize * scaleFactor * (s.size.x / s.baseSize.x);
            }
            ImVec2 textRenderPos = ImVec2(
                contentActualPos_World.x + s.textPosition.x * scaleFactor, 
                contentActualPos_World.y + s.textPosition.y * scaleFactor
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
                    textRenderPos.x = contentActualPos_World.x + (contentActualSizePx.x - textDimensions.x) * 0.5f + s.textPosition.x * scaleFactor;
                }
                else if (s.textAlignment == 2) { 
                    textRenderPos.x = contentActualPos_World.x + (contentActualSizePx.x - textDimensions.x) + s.textPosition.x * scaleFactor;
                }
                
            }
            float angleRadians = s.textRotation * (IM_PI / 180.0f); 
            ImU32 col = ColU32(s.textColor);
            if (font) ImGui::PushFont(font);
            if (fabs(angleRadians) < 1e-4f) {
                dlEffective->AddText(font ? font : ImGui::GetFont(), computedTextSize, textRenderPos, col, s.text.c_str());
            }
            else {
                AddTextRotated(dlEffective, font, computedTextSize, textRenderPos, col, s.text.c_str(), angleRadians, ImVec2(0.5f, 0.5f));
            }
            if (font) ImGui::PopFont();
        }
    }

    void DrawShape_FinalOnClick(ShapeItem& s) {
        if (s.shouldCallOnClick) {
            DispatchEvent(s, "onClick");
            s.shouldCallOnClick = false;
        }
    }
    inline ImVec2 NormalizeImVec2(const ImVec2& v) {
        float len = sqrtf(v.x * v.x + v.y * v.y);
        if (len > 1e-5f) return ImVec2(v.x / len, v.y / len);
        return ImVec2(0, 0);
    }

    
    void ExpandQuadVertices(ImVec2& v0, ImVec2& v1, ImVec2& v2, ImVec2& v3, float amount) {
        if (amount == 0.0f) return;
        ImVec2 centroid = (v0 + v1 + v2 + v3) * 0.25f; 
        v0 = v0 + NormalizeImVec2(v0 - centroid) * amount;
        v1 = v1 + NormalizeImVec2(v1 - centroid) * amount;
        v2 = v2 + NormalizeImVec2(v2 - centroid) * amount;
        v3 = v3 + NormalizeImVec2(v3 - centroid) * amount;
    }
    void DrawShape(ImDrawList* dl, ShapeItem& s, ImVec2 wp) {
        if (!s.visible) return;

        ImDrawList* dlEffective = (s.forceOverlap && s.allowItemOverlap) ? ImGui::GetForegroundDrawList() : dl;

        if (s.isPolygon && !s.polygonVertices.empty()) {
            std::vector<ImVec2> absolutePolyVertices;
            absolutePolyVertices.reserve(s.polygonVertices.size());
            for (const auto& v_window_relative : s.polygonVertices) {
                absolutePolyVertices.push_back(ImVec2(wp.x + v_window_relative.x, wp.y + v_window_relative.y));
            }

            if (!absolutePolyVertices.empty()) {
                ImVec4 fillColorToUse = s.fillColor;

                if (s.isButton && !DesignManager::g_IsInEditMode) {
                    ImRect polyBoundingBoxAbsScreen;
                    polyBoundingBoxAbsScreen.Min = absolutePolyVertices[0];
                    polyBoundingBoxAbsScreen.Max = absolutePolyVertices[0];
                    for (size_t k_vtx = 1; k_vtx < absolutePolyVertices.size(); ++k_vtx) {
                        polyBoundingBoxAbsScreen.Add(absolutePolyVertices[k_vtx]);
                    }

                    ImGui::SetCursorScreenPos(polyBoundingBoxAbsScreen.Min);
                    std::string button_id_poly = "##button_poly_" + std::to_string(s.id);
                    ImGui::InvisibleButton(button_id_poly.c_str(), polyBoundingBoxAbsScreen.GetSize());

                    if (ImGui::IsItemHovered()) fillColorToUse = s.hoverColor;
                    if (ImGui::IsItemActive()) fillColorToUse = s.clickedColor;
                    if (ImGui::IsItemClicked()) s.shouldCallOnClick = true;
                }

                if (fillColorToUse.w > 0.0f) {
                    dlEffective->AddConvexPolyFilled(absolutePolyVertices.data(), (int)absolutePolyVertices.size(), ColU32(fillColorToUse));
                }

                
                
                if (s.borderThickness > 0.0f && s.borderColor.w > 0.0f) {
                    dlEffective->AddPolyline(absolutePolyVertices.data(), (int)absolutePolyVertices.size(), ColU32(s.borderColor), ImDrawFlags_Closed, s.borderThickness * globalScaleFactor);
                }
            }

            if (s.shouldCallOnClick) {
                DispatchEvent(s, "onClick");
                if (s.onClick && s.useOnClick) {
                    s.onClick();
                }
                s.shouldCallOnClick = false;
            }
            return;
        }

        float scaleFactor = globalScaleFactor;

        ImVec2 itemDesignPosPx_Scaled = s.position * scaleFactor;
        ImVec2 itemDesignSizePx_Scaled = s.size * scaleFactor;
        float itemDesignCornerRadiusPx_Scaled = s.cornerRadius * scaleFactor;
        float globalBorderThicknessPx_Scaled = s.borderThickness * scaleFactor; 

        float paddingLeftPx_Scaled = s.padding.x * scaleFactor;
        float paddingTopPx_Scaled = s.padding.y * scaleFactor;
        float paddingRightPx_Scaled = s.padding.z * scaleFactor;
        float paddingBottomPx_Scaled = s.padding.w * scaleFactor;
        float totalHorizontalPaddingPx_Scaled = paddingLeftPx_Scaled + paddingRightPx_Scaled;
        float totalVerticalPaddingPx_Scaled = paddingTopPx_Scaled + paddingBottomPx_Scaled;

        ImVec2 fillArea_TopLeft_OffsetPx, fillArea_SizePx;
        float fillArea_CornerRadiusPx;

        ImVec2 borderArea_TopLeft_OffsetPx, borderArea_SizePx;
        float borderArea_CornerRadiusPx;

        ImVec2 contentArea_TopLeft_OffsetPx, contentArea_SizePx;
        float contentArea_CornerRadiusPx;

        ImVec2 interactionArea_TopLeft_OffsetPx, interactionArea_SizePx;

        
        
        
        
        
        

        if (s.boxSizing == ShapeItem::BoxSizing::ContentBox) {
            contentArea_SizePx = itemDesignSizePx_Scaled;
            contentArea_TopLeft_OffsetPx = ImVec2(0, 0);
            contentArea_CornerRadiusPx = itemDesignCornerRadiusPx_Scaled;
            fillArea_SizePx = contentArea_SizePx;
            fillArea_TopLeft_OffsetPx = contentArea_TopLeft_OffsetPx;
            fillArea_CornerRadiusPx = contentArea_CornerRadiusPx;
            ImVec2 paddingBox_SizePx = ImVec2(contentArea_SizePx.x + totalHorizontalPaddingPx_Scaled, contentArea_SizePx.y + totalVerticalPaddingPx_Scaled);
            ImVec2 paddingBox_TopLeft_OffsetPx = ImVec2(contentArea_TopLeft_OffsetPx.x - paddingLeftPx_Scaled, contentArea_TopLeft_OffsetPx.y - paddingTopPx_Scaled);
            float paddingBox_CornerRadiusPx = contentArea_CornerRadiusPx;
            if (paddingLeftPx_Scaled > 0 || paddingTopPx_Scaled > 0 || paddingRightPx_Scaled > 0 || paddingBottomPx_Scaled > 0) {
                paddingBox_CornerRadiusPx = std::max(0.0f, contentArea_CornerRadiusPx + std::max({ paddingLeftPx_Scaled, paddingTopPx_Scaled, paddingRightPx_Scaled, paddingBottomPx_Scaled }));
            }
            borderArea_SizePx = ImVec2(paddingBox_SizePx.x + 2.0f * globalBorderThicknessPx_Scaled, paddingBox_SizePx.y + 2.0f * globalBorderThicknessPx_Scaled);
            borderArea_TopLeft_OffsetPx = ImVec2(paddingBox_TopLeft_OffsetPx.x - globalBorderThicknessPx_Scaled, paddingBox_TopLeft_OffsetPx.y - globalBorderThicknessPx_Scaled);
            borderArea_CornerRadiusPx = paddingBox_CornerRadiusPx + globalBorderThicknessPx_Scaled;
            interactionArea_SizePx = borderArea_SizePx;
            interactionArea_TopLeft_OffsetPx = borderArea_TopLeft_OffsetPx;
        }
        else if (s.boxSizing == ShapeItem::BoxSizing::BorderBox) {
            borderArea_SizePx = itemDesignSizePx_Scaled;
            borderArea_TopLeft_OffsetPx = ImVec2(0, 0);
            borderArea_CornerRadiusPx = itemDesignCornerRadiusPx_Scaled;
            interactionArea_SizePx = borderArea_SizePx;
            interactionArea_TopLeft_OffsetPx = borderArea_TopLeft_OffsetPx;
            ImVec2 paddingBox_SizePx = ImVec2(std::max(0.0f, borderArea_SizePx.x - 2.0f * globalBorderThicknessPx_Scaled), std::max(0.0f, borderArea_SizePx.y - 2.0f * globalBorderThicknessPx_Scaled));
            ImVec2 paddingBox_TopLeft_OffsetPx = ImVec2(borderArea_TopLeft_OffsetPx.x + globalBorderThicknessPx_Scaled, borderArea_TopLeft_OffsetPx.y + globalBorderThicknessPx_Scaled);
            float paddingBox_CornerRadiusPx = std::max(0.0f, borderArea_CornerRadiusPx - globalBorderThicknessPx_Scaled);
            contentArea_SizePx = ImVec2(std::max(0.0f, paddingBox_SizePx.x - totalHorizontalPaddingPx_Scaled), std::max(0.0f, paddingBox_SizePx.y - totalVerticalPaddingPx_Scaled));
            contentArea_TopLeft_OffsetPx = ImVec2(paddingBox_TopLeft_OffsetPx.x + paddingLeftPx_Scaled, paddingBox_TopLeft_OffsetPx.y + paddingTopPx_Scaled);
            contentArea_CornerRadiusPx = std::max(0.0f, paddingBox_CornerRadiusPx - std::max({ paddingLeftPx_Scaled, paddingTopPx_Scaled, paddingRightPx_Scaled, paddingBottomPx_Scaled }));
            fillArea_SizePx = contentArea_SizePx;
            fillArea_TopLeft_OffsetPx = contentArea_TopLeft_OffsetPx;
            fillArea_CornerRadiusPx = contentArea_CornerRadiusPx;
        }
        else { 
            ImVec2 paddingBox_SizePx = itemDesignSizePx_Scaled;
            ImVec2 paddingBox_TopLeft_OffsetPx = ImVec2(0, 0);
            float paddingBox_CornerRadiusPx = itemDesignCornerRadiusPx_Scaled;
            contentArea_SizePx = ImVec2(std::max(0.0f, paddingBox_SizePx.x - totalHorizontalPaddingPx_Scaled), std::max(0.0f, paddingBox_SizePx.y - totalVerticalPaddingPx_Scaled));
            contentArea_TopLeft_OffsetPx = ImVec2(paddingBox_TopLeft_OffsetPx.x + paddingLeftPx_Scaled, paddingBox_TopLeft_OffsetPx.y + paddingTopPx_Scaled);
            contentArea_CornerRadiusPx = std::max(0.0f, paddingBox_CornerRadiusPx - std::max({ paddingLeftPx_Scaled, paddingTopPx_Scaled, paddingRightPx_Scaled, paddingBottomPx_Scaled }));
            fillArea_SizePx = contentArea_SizePx;
            fillArea_TopLeft_OffsetPx = contentArea_TopLeft_OffsetPx;
            fillArea_CornerRadiusPx = contentArea_CornerRadiusPx;
            borderArea_SizePx = ImVec2(paddingBox_SizePx.x + globalBorderThicknessPx_Scaled, paddingBox_SizePx.y + globalBorderThicknessPx_Scaled);
            borderArea_TopLeft_OffsetPx = ImVec2(paddingBox_TopLeft_OffsetPx.x - globalBorderThicknessPx_Scaled * 0.5f, paddingBox_TopLeft_OffsetPx.y - globalBorderThicknessPx_Scaled * 0.5f);
            borderArea_CornerRadiusPx = paddingBox_CornerRadiusPx + globalBorderThicknessPx_Scaled * 0.5f;
            interactionArea_SizePx = borderArea_SizePx;
            interactionArea_TopLeft_OffsetPx = borderArea_TopLeft_OffsetPx;
        }

        fillArea_SizePx.x = std::max(0.0f, fillArea_SizePx.x); fillArea_SizePx.y = std::max(0.0f, fillArea_SizePx.y);
        borderArea_SizePx.x = std::max(0.0f, borderArea_SizePx.x); borderArea_SizePx.y = std::max(0.0f, borderArea_SizePx.y);
        contentArea_SizePx.x = std::max(0.0f, contentArea_SizePx.x); contentArea_SizePx.y = std::max(0.0f, contentArea_SizePx.y);
        interactionArea_SizePx.x = std::max(0.0f, interactionArea_SizePx.x); interactionArea_SizePx.y = std::max(0.0f, interactionArea_SizePx.y);
        fillArea_CornerRadiusPx = std::max(0.0f, fillArea_CornerRadiusPx);
        borderArea_CornerRadiusPx = std::max(0.0f, borderArea_CornerRadiusPx);
        contentArea_CornerRadiusPx = std::max(0.0f, contentArea_CornerRadiusPx);

        ImVec2 itemBasePos_World = wp + itemDesignPosPx_Scaled;
        ImVec2 fillActualPos_World = itemBasePos_World + fillArea_TopLeft_OffsetPx;
        ImVec2 fillActualSizePx = fillArea_SizePx;
        ImVec2 fillActualCenter_World = fillActualPos_World + fillActualSizePx * 0.5f;
        ImVec2 borderActualPos_World = itemBasePos_World + borderArea_TopLeft_OffsetPx;
        ImVec2 borderActualSizePx = borderArea_SizePx;
        ImVec2 borderActualCenter_World = borderActualPos_World + borderActualSizePx * 0.5f;
        ImVec2 contentActualPos_World = itemBasePos_World + contentArea_TopLeft_OffsetPx;
        ImVec2 contentActualSizePx = contentArea_SizePx;
        ImVec2 interactionActualPos_World = itemBasePos_World + interactionArea_TopLeft_OffsetPx;
        ImVec2 interactionActualSizePx = interactionArea_SizePx;

        if (s.shadowColor.w > 0.0f && !s.shadowInset && borderActualSizePx.x > 0 && borderActualSizePx.y > 0) {
            ShapeItem shadowShapeCopy = s;
            shadowShapeCopy.position = borderActualPos_World - wp;
            shadowShapeCopy.size = borderActualSizePx;
            shadowShapeCopy.cornerRadius = borderArea_CornerRadiusPx;
            shadowShapeCopy.shadowInset = false;
            DrawShape_Shadow(dlEffective, shadowShapeCopy, wp, 1.0f, borderActualCenter_World, s.rotation + s.shadowRotation);
        }

        if (s.blurAmount > 0.0f && borderActualSizePx.x > 0 && borderActualSizePx.y > 0) {
            ShapeItem blurShapeCopy = s;
            blurShapeCopy.position = borderActualPos_World - wp;
            blurShapeCopy.size = borderActualSizePx;
            blurShapeCopy.cornerRadius = borderArea_CornerRadiusPx;
            DrawShape_Blur(dlEffective, blurShapeCopy, wp, 1.0f, borderActualCenter_World);
        }

        std::vector<ImVec2> fillPoly_World_Coords;
        if (fillActualSizePx.x > 1e-3f && fillActualSizePx.y > 1e-3f) {
            if (s.type == ShapeType::Rectangle) {
                BuildRectPoly(fillPoly_World_Coords, fillActualPos_World, fillActualSizePx, fillArea_CornerRadiusPx, fillActualCenter_World, s.rotation);
            }
            else {
                BuildCirclePoly(fillPoly_World_Coords, fillActualCenter_World, fillActualSizePx.x * 0.5f, fillActualSizePx.y * 0.5f, s.rotation);
            }
        }

        ImVec4 currentFillColor = s.fillColor;
        DrawShape_LoadEmbeddedImageIfNeeded(s);
        if (s.hasEmbeddedImage && s.embeddedImageTexture && !fillPoly_World_Coords.empty()) {
            dlEffective->PushTextureID(s.embeddedImageTexture);
            if (std::abs(s.rotation) < 1e-4) {
                dlEffective->AddImageRounded(s.embeddedImageTexture, fillActualPos_World, ImVec2(fillActualPos_World.x + fillActualSizePx.x, fillActualPos_World.y + fillActualSizePx.y), ImVec2(0, 0), ImVec2(1, 1), IM_COL32_WHITE, fillArea_CornerRadiusPx);
            }
            else {
                std::vector<ImVec2> uvCoords(fillPoly_World_Coords.size());
                for (size_t i = 0; i < fillPoly_World_Coords.size(); i++) {
                    uvCoords[i] = UV(fillPoly_World_Coords[i], fillActualSizePx, fillActualCenter_World, fillActualPos_World, s.rotation);
                }
                for (size_t i = 1; i < fillPoly_World_Coords.size() - 1; i++) {
                    dlEffective->PrimReserve(3, 3);
                    dlEffective->PrimVtx(fillPoly_World_Coords[0], uvCoords[0], IM_COL32_WHITE);
                    dlEffective->PrimVtx(fillPoly_World_Coords[i], uvCoords[i], IM_COL32_WHITE);
                    dlEffective->PrimVtx(fillPoly_World_Coords[i + 1], uvCoords[i + 1], IM_COL32_WHITE);
                }
            }
            dlEffective->PopTextureID();
        }

        if (s.isButton) {
            ImGui::SetCursorScreenPos(interactionActualPos_World);
            std::string button_id_fixed = "##button_box_" + std::to_string(s.id);
            ImGui::InvisibleButton(button_id_fixed.c_str(), interactionActualSizePx);
            bool is_hovered = ImGui::IsItemHovered();
            bool is_active = ImGui::IsItemActive();
            if (!s.allowItemOverlap && ImGui::GetHoveredID() != 0 && ImGui::GetHoveredID() != ImGui::GetID(button_id_fixed.c_str())) {
                is_hovered = false;
                is_active = false;
            }
            else if (s.allowItemOverlap) {
                ImVec2 mousePosScreen = ImGui::GetIO().MousePos;
                ImRect buttonRectScreen(interactionActualPos_World, interactionActualPos_World + interactionActualSizePx);
                if (buttonRectScreen.Contains(mousePosScreen)) {
                    is_hovered = true;
                    is_active = ImGui::IsMouseDown(0) && is_hovered;
                }
                else {
                    is_hovered = false;
                    is_active = false;
                }
            }
            if (s.forceOverlap && s.allowItemOverlap && s.blockUnderlying) {
                ImGui::SetNextWindowPos(interactionActualPos_World);
                ImGui::SetNextWindowSize(interactionActualSizePx);
                std::string overlay_id = "BlockOverlay_" + std::to_string(s.id);
                ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
                ImGui::BeginChild(overlay_id.c_str(), interactionActualSizePx, false, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs);
                ImGui::EndChild();
                ImGui::PopStyleColor();
            }
            currentFillColor = s.fillColor;
            if (is_hovered && !DesignManager::g_IsInEditMode) {
                currentFillColor = s.hoverColor;
            }
            for (auto& anim : s.onClickAnimations) {
                if (anim.triggerMode == ButtonAnimation::TriggerMode::OnHover) {}
            }
            if (!DesignManager::g_IsInEditMode) {
                if (s.buttonBehavior == ShapeItem::ButtonBehavior::SingleClick) {
                    if (ImGui::IsItemClicked()) s.shouldCallOnClick = true;
                    if (is_active) currentFillColor = s.clickedColor;
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
                    if (s.buttonState) currentFillColor = s.clickedColor;
                }
                else if (s.buttonBehavior == ShapeItem::ButtonBehavior::Hold) {
                    if (is_active) {
                        currentFillColor = s.clickedColor;
                        if (!s.isHeld) { s.shouldCallOnClick = true; }
                    }
                    else {
                        if (s.isHeld) { s.shouldCallOnClick = true; }
                    }
                }
            }
            s.isHeld = is_active;
            if (s.shouldCallOnClick && !s.onClickAnimations.empty()) {}
        }

        DrawShape_Fill(dlEffective, s, fillPoly_World_Coords, fillActualPos_World, fillActualSizePx, fillActualCenter_World, currentFillColor);

        if (s.shadowColor.w > 0.0f && s.shadowInset && fillActualSizePx.x > 0 && fillActualSizePx.y > 0) {
            ShapeItem shadowShapeCopy = s;
            shadowShapeCopy.position = fillActualPos_World - wp;
            shadowShapeCopy.size = fillActualSizePx;
            shadowShapeCopy.cornerRadius = fillArea_CornerRadiusPx;
            shadowShapeCopy.shadowInset = true;
            DrawShape_Shadow(dlEffective, shadowShapeCopy, wp, 1.0f, fillActualCenter_World, s.rotation + s.shadowRotation);
        }



        
        float per_side_thickness_scaled[4];
        if (s.usePerSideBorderThicknesses) {
            for (int i = 0; i < 4; ++i) {
                per_side_thickness_scaled[i] = s.borderSideThicknesses[i] * scaleFactor;
            }
        }
        else {
            for (int i = 0; i < 4; ++i) {
                per_side_thickness_scaled[i] = globalBorderThicknessPx_Scaled;
            }
        }

        float max_thickness_for_polyline = 0.0f;
        bool any_border_visible = false;

        for (int i = 0; i < 4; ++i) {
            float current_side_effective_thickness = s.usePerSideBorderThicknesses ? per_side_thickness_scaled[i] : globalBorderThicknessPx_Scaled;
            ImVec4 current_side_effective_color_v4 = s.usePerSideBorderColors ? s.borderSideColors[i] : s.borderColor;

            if (current_side_effective_thickness > 0.001f && current_side_effective_color_v4.w > 0.0f) {
                any_border_visible = true;
                max_thickness_for_polyline = std::max(max_thickness_for_polyline, current_side_effective_thickness);
            }
        }

        if (any_border_visible && borderActualSizePx.x > 1e-3f && borderActualSizePx.y > 1e-3f) {
            if (s.type == ShapeType::Rectangle && s.cornerRadius < 0.001f) {
                

                ImVec2 outer_tl_unrot = ImVec2(0.0f, 0.0f);
                ImVec2 outer_tr_unrot = ImVec2(borderActualSizePx.x, 0.0f);
                ImVec2 outer_br_unrot = ImVec2(borderActualSizePx.x, borderActualSizePx.y);
                ImVec2 outer_bl_unrot = ImVec2(0.0f, borderActualSizePx.y);

                float current_th[4]; 
                ImU32 current_col[4];
                bool current_visible[4];

                for (int i = 0; i < 4; ++i) {
                    current_th[i] = s.usePerSideBorderThicknesses ? per_side_thickness_scaled[i] : globalBorderThicknessPx_Scaled;
                    ImVec4 c_v4 = s.usePerSideBorderColors ? s.borderSideColors[i] : s.borderColor;
                    current_col[i] = ColU32(c_v4);
                    current_visible[i] = (current_th[i] > 0.001f && c_v4.w > 0.0f);
                }

                
                ImVec2 inner_tl_raw = ImVec2(current_th[3], current_th[0]);
                ImVec2 inner_tr_raw = ImVec2(borderActualSizePx.x - current_th[1], current_th[0]);
                ImVec2 inner_br_raw = ImVec2(borderActualSizePx.x - current_th[1], borderActualSizePx.y - current_th[2]);
                ImVec2 inner_bl_raw = ImVec2(current_th[3], borderActualSizePx.y - current_th[2]);

                ImVec2 center_for_rotation = borderActualSizePx * 0.5f;
                ImVec2 final_pos_offset = borderActualPos_World;

                
                
                if (current_visible[0]) { 
                    ImVec2 q[4] = { outer_tl_unrot, outer_tr_unrot, inner_tr_raw, inner_tl_raw };
                    if (std::abs(s.rotation) > 1e-4f) { for (int i = 0; i < 4; ++i) q[i] = RotateP(q[i], center_for_rotation, s.rotation); }
                    dlEffective->AddQuadFilled(final_pos_offset + q[0], final_pos_offset + q[1], final_pos_offset + q[2], final_pos_offset + q[3], current_col[0]);
                }
                if (current_visible[1]) { 
                    ImVec2 q[4] = { outer_tr_unrot, outer_br_unrot, inner_br_raw, inner_tr_raw };
                    if (std::abs(s.rotation) > 1e-4f) { for (int i = 0; i < 4; ++i) q[i] = RotateP(q[i], center_for_rotation, s.rotation); }
                    dlEffective->AddQuadFilled(final_pos_offset + q[0], final_pos_offset + q[1], final_pos_offset + q[2], final_pos_offset + q[3], current_col[1]);
                }
                if (current_visible[2]) { 
                    ImVec2 q_inner_br = inner_br_raw;
                    ImVec2 q_inner_bl = inner_bl_raw;
                    
                    
                    
                    
                    
                    
                    

                    ImVec2 q[4] = { outer_bl_unrot, outer_br_unrot, q_inner_br, q_inner_bl };
                    if (std::abs(s.rotation) > 1e-4f) { for (int i = 0; i < 4; ++i) q[i] = RotateP(q[i], center_for_rotation, s.rotation); }
                    dlEffective->AddQuadFilled(final_pos_offset + q[0], final_pos_offset + q[1], final_pos_offset + q[2], final_pos_offset + q[3], current_col[2]);
                }
                if (current_visible[3]) { 
                    ImVec2 q_inner_bl = inner_bl_raw;
                    ImVec2 q_inner_tl = inner_tl_raw;
                    
                    
                    
                    
                    
                    


                    ImVec2 q[4] = { outer_tl_unrot, outer_bl_unrot, q_inner_bl, q_inner_tl };
                    if (std::abs(s.rotation) > 1e-4f) { for (int i = 0; i < 4; ++i) q[i] = RotateP(q[i], center_for_rotation, s.rotation); }
                    dlEffective->AddQuadFilled(final_pos_offset + q[0], final_pos_offset + q[1], final_pos_offset + q[2], final_pos_offset + q[3], current_col[3]);
                }
            }
            else { 
                ImVec2 poly_pos = borderActualPos_World;
                ImVec2 poly_size = borderActualSizePx;
                float poly_radius = borderArea_CornerRadiusPx;
                float effective_polyline_thickness = max_thickness_for_polyline;

                if (s.boxSizing == ShapeItem::BoxSizing::BorderBox || s.boxSizing == ShapeItem::BoxSizing::ContentBox || s.boxSizing == ShapeItem::BoxSizing::StrokeBox) {
                    poly_pos = ImVec2(borderActualPos_World.x + effective_polyline_thickness * 0.5f, borderActualPos_World.y + effective_polyline_thickness * 0.5f);
                    poly_size = ImVec2(std::max(0.0f, borderActualSizePx.x - effective_polyline_thickness), std::max(0.0f, borderActualSizePx.y - effective_polyline_thickness));
                    poly_radius = std::max(0.0f, borderArea_CornerRadiusPx - effective_polyline_thickness * 0.5f);
                }

                std::vector<ImVec2> final_border_poly_points;
                ImVec2 center_for_poly = poly_pos + poly_size * 0.5f;

                if (s.type == ShapeType::Rectangle) {
                    BuildRectPoly(final_border_poly_points, poly_pos, poly_size, poly_radius, center_for_poly, s.rotation);
                }
                else {
                    BuildCirclePoly(final_border_poly_points, center_for_poly, poly_size.x * 0.5f, poly_size.y * 0.5f, s.rotation);
                }

                if (!final_border_poly_points.empty()) {
                    ImU32 polyline_color_u32 = 0;
                    bool polyline_color_chosen = false;

                    if (s.usePerSideBorderColors) {
                        for (int i = 0; i < 4; ++i) {
                            float side_thick = s.usePerSideBorderThicknesses ? per_side_thickness_scaled[i] : globalBorderThicknessPx_Scaled;
                            if (s.borderSideColors[i].w > 0.0f && side_thick > 0.001f) {
                                polyline_color_u32 = ColU32(s.borderSideColors[i]);
                                polyline_color_chosen = true;
                                break;
                            }
                        }
                    }

                    if (!polyline_color_chosen) {
                        if (s.borderColor.w > 0.0f && globalBorderThicknessPx_Scaled > 0.001f) {
                            polyline_color_u32 = ColU32(s.borderColor);
                            polyline_color_chosen = true;
                        }
                    }

                    if (polyline_color_chosen && effective_polyline_thickness > 0.001f) {
                        dlEffective->AddPolyline(final_border_poly_points.data(), (int)final_border_poly_points.size(), polyline_color_u32, ImDrawFlags_Closed, effective_polyline_thickness);
                    }
                }
            }
        }
        

        DrawShape_RenderImGuiContent(dl, s, contentActualPos_World, contentActualSizePx, scaleFactor);
        if (s.isChildWindow && !s.isImGuiContainer) {
            DrawShape_RenderChildWindow(s, contentActualPos_World, contentActualSizePx);
        }
        else if (!s.isImGuiContainer) {
            if (s.useGlass) {}
            DrawShape_DrawText(dlEffective, s, contentActualPos_World, contentActualSizePx, scaleFactor);
        }
        DrawShape_FinalOnClick(s);
    }

    void RenderAllRegisteredWindows()
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
                std::stable_sort(layer.shapes.begin(), layer.shapes.end(), [](const auto& a, const auto& b) { return CompareShapesByZOrder(*a, *b); });
                for (auto& shape_uptr : layer.shapes)
                {
                    if (shape_uptr && shape_uptr->ownerWindow == winName && shape_uptr->parent == nullptr)
                        DrawShape(ImGui::GetWindowDrawList(), *shape_uptr, ImGui::GetWindowPos());
                }
            }
            ImGui::End();
        }
    }

    ImVec2 CalculateIntrinsicSize(const ShapeItem& item) {
        ImVec2 textSize(0, 0);
        ImVec2 childrenBoundsMin(FLT_MAX, FLT_MAX);
        ImVec2 childrenBoundsMax(-FLT_MAX, -FLT_MAX);
        bool hasChildrenBounds = false;

        if (item.hasText && !item.text.empty()) {
            ImFont* font = ImGui::GetIO().Fonts->Fonts.empty() ? ImGui::GetFont() : ImGui::GetIO().Fonts->Fonts[std::max(0, std::min(item.textFont, (int)ImGui::GetIO().Fonts->Fonts.Size - 1))];
            float fontSize = item.textSize > 0 ? item.textSize : font->FontSize;
            textSize = font->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, item.text.c_str());
            textSize.x += item.textPosition.x;
            textSize.y += item.textPosition.y;
        }

        for (const ShapeItem* child : item.children) {
            if (child && child->visible) {
                ImVec2 childTopLeft = child->basePosition;
                ImVec2 childBottomRight = childTopLeft + child->baseSize;
                childrenBoundsMin.x = std::min(childrenBoundsMin.x, childTopLeft.x);
                childrenBoundsMin.y = std::min(childrenBoundsMin.y, childTopLeft.y);
                childrenBoundsMax.x = std::max(childrenBoundsMax.x, childBottomRight.x);
                childrenBoundsMax.y = std::max(childrenBoundsMax.y, childBottomRight.y);
                hasChildrenBounds = true;
            }
        }

        ImVec2 childrenSize(0, 0);
        if (hasChildrenBounds) {
            childrenSize.x = std::max(0.0f, childrenBoundsMax.x);
            childrenSize.y = std::max(0.0f, childrenBoundsMax.y);
        }

        ImVec2 intrinsicSize;
        intrinsicSize.x = std::max(textSize.x, childrenSize.x);
        intrinsicSize.y = std::max(textSize.y, childrenSize.y);

        if (intrinsicSize.x <= 1e-6f && intrinsicSize.y <= 1e-6f) {
            intrinsicSize = item.baseSize;
        }

        intrinsicSize.x = std::max(item.minSize.x, std::min(intrinsicSize.x, item.maxSize.x));
        intrinsicSize.y = std::max(item.minSize.y, std::min(intrinsicSize.y, item.maxSize.y));
        intrinsicSize.x = std::max(0.0f, intrinsicSize.x);
        intrinsicSize.y = std::max(0.0f, intrinsicSize.y);

        return intrinsicSize;
    }

    float GetBaselineOffset(const ShapeItem& item, const ImVec2& calculatedSize) {
        if (item.hasText && !item.text.empty() && item.textSize > 0) {
            ImFont* font = ImGui::GetIO().Fonts->Fonts.empty() ? ImGui::GetFont() : ImGui::GetIO().Fonts->Fonts[std::max(0, std::min(item.textFont, (int)ImGui::GetIO().Fonts->Fonts.Size - 1))];
            if (font->FontSize > 1e-6f) {
                float fontScale = item.textSize / font->FontSize;
                return item.textPosition.y + (font->Ascent * fontScale);
            }
            else {
                return item.textPosition.y + item.textSize;
            }
        }
        else {
            return calculatedSize.y;
        }
    }

    int FindShapeLayerIndex(int shapeId) {
        if (g_windowsMap.count(selectedGuiWindow)) {
            WindowData& currentWindowData = g_windowsMap.at(selectedGuiWindow);
            for (size_t i = 0; i < currentWindowData.layers.size(); ++i) {
                for (const auto& shape_uptr : currentWindowData.layers[i].shapes) {
                    if (shape_uptr && shape_uptr->id == shapeId) {
                        return static_cast<int>(i);
                    }
                }
            }
        }
        return -1;
    }

    GridTrackSize::TrackSizeValue ParseTrackSizeValueString(const std::string& segment) {
        GridTrackSize::TrackSizeValue result;
        if (segment.empty()) return result;

        try {
            if (segment == "auto") {
                result.unit = GridTrackSize::TrackSizeValue::Unit::Auto;
                result.value = 0;
            }
            else if (segment.length() > 2 && segment.substr(segment.length() - 2) == "fr") {
                result.unit = GridTrackSize::TrackSizeValue::Unit::Fr;
                result.value = std::stof(segment.substr(0, segment.length() - 2));
            }
            else if (segment.length() > 1 && segment.back() == '%') {
                result.unit = GridTrackSize::TrackSizeValue::Unit::Percent;
                result.value = std::stof(segment.substr(0, segment.length() - 1));
            }
            else if (segment.length() > 2 && segment.substr(segment.length() - 2) == "px") {
                result.unit = GridTrackSize::TrackSizeValue::Unit::Px;
                result.value = std::stof(segment.substr(0, segment.length() - 2));
            }
            else {
                result.unit = GridTrackSize::TrackSizeValue::Unit::Px;
                result.value = std::stof(segment);
            }
            if (result.value < 0 && result.unit != GridTrackSize::TrackSizeValue::Unit::Auto) result.value = 0;
        }
        catch (...) {
            result.unit = GridTrackSize::TrackSizeValue::Unit::Auto;
            result.value = 0;
        }
        return result;
    }

    std::vector<GridTrackSize> ParseGridTemplate(const std::string& templateString) {
        std::vector<GridTrackSize> tracks;
        std::stringstream ss(templateString);
        std::string segment;
        std::string accumulatedSegment;

        while (ss >> segment) {
            accumulatedSegment += segment;
            if (accumulatedSegment.find("minmax(") == 0) {
                size_t openParen = accumulatedSegment.find('(');
                size_t closeParen = accumulatedSegment.find(')');
                if (closeParen != std::string::npos && openParen != std::string::npos && closeParen > openParen) {
                    std::string content = accumulatedSegment.substr(openParen + 1, closeParen - openParen - 1);
                    size_t commaPos = content.find(',');
                    if (commaPos != std::string::npos) {
                        std::string minStr = content.substr(0, commaPos);
                        std::string maxStr = content.substr(commaPos + 1);
                        minStr.erase(0, minStr.find_first_not_of(" \t\n\r\f\v"));
                        minStr.erase(minStr.find_last_not_of(" \t\n\r\f\v") + 1);
                        maxStr.erase(0, maxStr.find_first_not_of(" \t\n\r\f\v"));
                        maxStr.erase(maxStr.find_last_not_of(" \t\n\r\f\v") + 1);

                        GridTrackSize track;
                        track.mode = GridTrackSize::Mode::MinMax;
                        track.minVal = ParseTrackSizeValueString(minStr);
                        track.maxVal = ParseTrackSizeValueString(maxStr);

                        if (track.minVal.isFr()) {
                            track.minVal.unit = GridTrackSize::TrackSizeValue::Unit::Px;
                            track.minVal.value = 0;
                        }
                        if (track.minVal.unit == GridTrackSize::TrackSizeValue::Unit::Percent && track.maxVal.unit == GridTrackSize::TrackSizeValue::Unit::Fr) {
                            track.maxVal.unit = GridTrackSize::TrackSizeValue::Unit::Px;
                            track.maxVal.value = 99999.0f;
                        }
                        tracks.push_back(track);
                    }
                    else {
                        tracks.push_back({});
                    }
                    accumulatedSegment = "";
                }
            }
            else {
                bool looksLikeUnit = (segment.length() > 2 && segment.substr(segment.length() - 2) == "fr") ||
                    (segment.length() > 1 && segment.back() == '%') ||
                    (segment.length() > 2 && segment.substr(segment.length() - 2) == "px") ||
                    (segment == "auto") ||
                    (std::all_of(segment.begin(), segment.end(), ::isdigit));

                if (looksLikeUnit) {
                    GridTrackSize track;
                    GridTrackSize::TrackSizeValue val = ParseTrackSizeValueString(accumulatedSegment);
                    track.value = val.value;
                    switch (val.unit) {
                    case GridTrackSize::TrackSizeValue::Unit::Px:      track.mode = GridTrackSize::Mode::Fixed; break;
                    case GridTrackSize::TrackSizeValue::Unit::Percent: track.mode = GridTrackSize::Mode::Percentage; break;
                    case GridTrackSize::TrackSizeValue::Unit::Fr:      track.mode = GridTrackSize::Mode::Fraction; break;
                    case GridTrackSize::TrackSizeValue::Unit::Auto:
                    default:                                         track.mode = GridTrackSize::Mode::Auto; break;
                    }
                    tracks.push_back(track);
                    accumulatedSegment = "";
                }
                else {
                    accumulatedSegment += " ";
                }
            }
        }

        if (tracks.empty() && !templateString.empty()) {
            tracks.push_back({});
        }
        if (tracks.empty()) {
            GridTrackSize defaultTrack;
            tracks.push_back(defaultTrack);
        }
        return tracks;
    }

    void ApplyConstraints(ShapeItem* shape, const ImVec2& parentFinalSize, ImVec2* outPosition, ImVec2* outSize)
    {
        if (!shape) return;

        ImVec2 resultingPosition = shape->basePosition;
        ImVec2 resultingSize = shape->baseSize;

        bool leftSet = false, rightSet = false, topSet = false, bottomSet = false;
        bool widthSet = false, heightSet = false;
        bool centerXSet = false, centerYSet = false;
        bool aspectRatioSet = false;

        float cLeft = 0.0f, cRight = 0.0f, cTop = 0.0f, cBottom = 0.0f;
        float cWidth = 0.0f, cHeight = 0.0f;
        float cCenterXOffset = 0.0f, cCenterYOffset = 0.0f;
        float cAspectRatio = 0.0f;

        for (const auto& constraint : shape->constraints)
        {
            switch (constraint.type)
            {
            case ConstraintType::LeftDistance:      cLeft = constraint.value; leftSet = true; break;
            case ConstraintType::RightDistance:     cRight = parentFinalSize.x - constraint.value; rightSet = true; break;
            case ConstraintType::TopDistance:       cTop = constraint.value; topSet = true; break;
            case ConstraintType::BottomDistance:    cBottom = parentFinalSize.y - constraint.value; bottomSet = true; break;
            case ConstraintType::WidthFixed:        cWidth = constraint.value; widthSet = true; break;
            case ConstraintType::HeightFixed:       cHeight = constraint.value; heightSet = true; break;
            case ConstraintType::WidthPercentage:   cWidth = parentFinalSize.x * (constraint.value / 100.0f); widthSet = true; break;
            case ConstraintType::HeightPercentage:  cHeight = parentFinalSize.y * (constraint.value / 100.0f); heightSet = true; break;
            case ConstraintType::CenterXAlignment:  cCenterXOffset = constraint.value; centerXSet = true; break;
            case ConstraintType::CenterYAlignment:  cCenterYOffset = constraint.value; centerYSet = true; break;
            case ConstraintType::AspectRatio:       if (constraint.value > 1e-6f) { cAspectRatio = constraint.value; aspectRatioSet = true; } break;
            }
        }

        float finalWidth = shape->baseSize.x;
        bool widthIsExplicit = false;
        if (widthSet) {
            finalWidth = cWidth;
            widthIsExplicit = true;
        }
        else if (leftSet && rightSet) {
            finalWidth = cRight - cLeft;
            widthIsExplicit = true;
        }
        finalWidth = std::max(0.0f, finalWidth);

        float finalX = shape->basePosition.x;
        if (leftSet) {
            finalX = cLeft;
        }
        else if (rightSet) {
            finalX = cRight - finalWidth;
        }
        else if (widthSet) {
        }

        if (centerXSet) {
            finalX = (parentFinalSize.x * 0.5f) + cCenterXOffset - (finalWidth * 0.5f);
        }

        float finalHeight = shape->baseSize.y;
        bool heightIsExplicit = false;
        if (heightSet) {
            finalHeight = cHeight;
            heightIsExplicit = true;
        }
        else if (topSet && bottomSet) {
            finalHeight = cBottom - cTop;
            heightIsExplicit = true;
        }
        finalHeight = std::max(0.0f, finalHeight);

        float finalY = shape->basePosition.y;
        if (topSet) {
            finalY = cTop;
        }
        else if (bottomSet) {
            finalY = cBottom - finalHeight;
        }
        else if (heightSet) {
        }

        if (centerYSet) {
            finalY = (parentFinalSize.y * 0.5f) + cCenterYOffset - (finalHeight * 0.5f);
        }

        if (aspectRatioSet) {
            if (widthIsExplicit && !heightIsExplicit) {
                finalHeight = finalWidth / cAspectRatio;
            }
            else if (!widthIsExplicit && heightIsExplicit) {
                finalWidth = finalHeight * cAspectRatio;
            }
            else {
                finalHeight = finalWidth / cAspectRatio;
            }
            finalWidth = std::max(0.0f, finalWidth);
            finalHeight = std::max(0.0f, finalHeight);
        }

        resultingPosition.x = finalX;
        resultingPosition.y = finalY;
        resultingSize.x = finalWidth;
        resultingSize.y = finalHeight;

        resultingSize.x = std::max(shape->minSize.x, std::min(resultingSize.x, shape->maxSize.x));
        resultingSize.y = std::max(shape->minSize.y, std::min(resultingSize.y, shape->maxSize.y));
        resultingSize.x = std::max(0.0f, resultingSize.x);
        resultingSize.y = std::max(0.0f, resultingSize.y);

        *outPosition = resultingPosition;
        *outSize = resultingSize;
    }

    void FlexLayout::doLayout(ShapeItem& container, const ImVec2& availableSize)
    {
        if (container.children.empty() || !container.layoutManager) {
            return;
        }

        FlexLayout* flexLayout = static_cast<FlexLayout*>(container.layoutManager.get());
        bool isRow = (flexLayout->direction == FlexDirection::Row || flexLayout->direction == FlexDirection::RowReverse);
        bool isMainReverse = (flexLayout->direction == FlexDirection::RowReverse || flexLayout->direction == FlexDirection::ColumnReverse);
        bool isWrapReverse = (flexLayout->wrap == FlexWrap::WrapReverse);

        const float padMainStart = isRow ? container.padding.x : container.padding.y;
        const float padMainEnd = isRow ? container.padding.z : container.padding.w;
        const float padCrossStart = isRow ? container.padding.y : container.padding.x;
        const float padCrossEnd = isRow ? container.padding.w : container.padding.z;

        const float availableInnerMainSize = std::max(0.0f, (isRow ? availableSize.x : availableSize.y) - padMainStart - padMainEnd);
        const float availableInnerCrossSize = std::max(0.0f, (isRow ? availableSize.y : availableSize.x) - padCrossStart - padCrossEnd);

        const float currentGap = flexLayout->gap;

        struct FlexItemInfo {
            ShapeItem* item = nullptr;
            float flexBasis = 0.0f;
            float hypotheticalMainSize = 0.0f;
            float targetMainSize = 0.0f;
            float crossSize = 0.0f;
            float targetCrossSize = 0.0f;
            float flexGrowFactor = 0.0f;
            float flexShrinkFactor = 1.0f;
            float scaledShrinkFactor = 0.0f;
            bool isVisible = false;
            float baselineOffset = 0.0f;
            float marginMainStart = 0.0f;
            float marginMainEnd = 0.0f;
            float marginCrossStart = 0.0f;
            float marginCrossEnd = 0.0f;
            float totalMainMargin = 0.0f;
            float totalCrossMargin = 0.0f;
        };

        struct FlexLine {
            std::vector<FlexItemInfo*> items;
            float totalHypotheticalMainSize = 0.0f;
            float totalGrow = 0.0f;
            float totalScaledShrink = 0.0f;
            float crossSize = 0.0f;
            float mainAxisPosition = 0.0f;
            float crossAxisPosition = 0.0f;
            int visibleItemCount = 0;
            float maxBaselineOffset = 0.0f;
        };

        std::vector<FlexLine> lines;
        std::vector<FlexItemInfo> allItemsInfo;
        allItemsInfo.reserve(container.children.size());

        std::vector<ShapeItem*> sortedChildren = container.children;
        std::sort(sortedChildren.begin(), sortedChildren.end(), [](const ShapeItem* a, const ShapeItem* b) {
            if (!a || !b) return false;
            return a->order < b->order;
            });

        for (ShapeItem* child : sortedChildren) {
            if (!child) continue;

            FlexItemInfo info;
            info.item = child;
            info.isVisible = child->visible;

            if (info.isVisible) {
                info.marginMainStart = isRow ? child->margin.x : child->margin.y;
                info.marginMainEnd = isRow ? child->margin.z : child->margin.w;
                info.marginCrossStart = isRow ? child->margin.y : child->margin.x;
                info.marginCrossEnd = isRow ? child->margin.w : child->margin.z;
                info.totalMainMargin = info.marginMainStart + info.marginMainEnd;
                info.totalCrossMargin = info.marginCrossStart + info.marginCrossEnd;

                switch (child->flexBasisMode) {
                case ShapeItem::FlexBasisMode::Pixels: info.flexBasis = child->flexBasisPixels; break;
                case ShapeItem::FlexBasisMode::Content: {
                    ImVec2 contentSize = CalculateIntrinsicSize(*child);
                    info.flexBasis = isRow ? contentSize.x : contentSize.y;
                } break;
                case ShapeItem::FlexBasisMode::Percentage: {
                    info.flexBasis = availableInnerMainSize * (child->flexBasisPixels / 100.0f);
                } break;
                case ShapeItem::FlexBasisMode::Auto: default: {
                    bool sizeSetByConstraint = false;
                    for (const auto& constraint : child->constraints) {
                        if (isRow && constraint.type == ConstraintType::WidthFixed && constraint.value >= 0) { info.flexBasis = constraint.value; sizeSetByConstraint = true; break; }
                        else if (!isRow && constraint.type == ConstraintType::HeightFixed && constraint.value >= 0) { info.flexBasis = constraint.value; sizeSetByConstraint = true; break; }
                    }
                    if (!sizeSetByConstraint) { info.flexBasis = isRow ? child->baseSize.x : child->baseSize.y; }
                } break;
                }
                info.flexBasis = std::max(0.0f, info.flexBasis);

                float minMain = isRow ? child->minSize.x : child->minSize.y;
                float maxMain = isRow ? child->maxSize.x : child->maxSize.y;
                float clampedBasis = std::max(minMain, std::min(info.flexBasis, maxMain));
                info.hypotheticalMainSize = clampedBasis + info.totalMainMargin;

                info.flexGrowFactor = std::max(0.0f, child->flexGrow);
                info.flexShrinkFactor = std::max(0.0f, child->flexShrink);
                info.scaledShrinkFactor = info.flexShrinkFactor * clampedBasis;

                float crossBasis = 0.0f;
                bool crossSizeSet = false;
                for (const auto& constraint : info.item->constraints) {
                    if (!isRow && constraint.type == ConstraintType::WidthFixed && constraint.value >= 0) { crossBasis = constraint.value; crossSizeSet = true; break; }
                    else if (isRow && constraint.type == ConstraintType::HeightFixed && constraint.value >= 0) { crossBasis = constraint.value; crossSizeSet = true; break; }
                }
                if (!crossSizeSet) {
                    if (child->flexBasisMode == ShapeItem::FlexBasisMode::Content) {
                        ImVec2 contentSize = CalculateIntrinsicSize(*child);
                        crossBasis = isRow ? contentSize.y : contentSize.x;
                    }
                    else {
                        crossBasis = isRow ? info.item->baseSize.y : info.item->baseSize.x;
                    }
                }
                crossBasis = std::max(0.0f, crossBasis);
                float minCross = isRow ? info.item->minSize.y : info.item->minSize.x;
                float maxCross = isRow ? info.item->maxSize.y : info.item->maxSize.x;
                float clampedCrossBasis = std::max(minCross, std::min(crossBasis, maxCross));
                info.crossSize = clampedCrossBasis + info.totalCrossMargin;
                info.targetCrossSize = clampedCrossBasis;
            }
            allItemsInfo.push_back(info);
        }

        bool hasVisibleItems = std::any_of(allItemsInfo.begin(), allItemsInfo.end(), [](const FlexItemInfo& info) { return info.isVisible; });
        if (flexLayout->wrap == FlexWrap::NoWrap || !hasVisibleItems) {
            lines.emplace_back();
            FlexLine& currentLine = lines.back();
            for (size_t i = 0; i < allItemsInfo.size(); ++i) {
                if (allItemsInfo[i].isVisible) {
                    FlexItemInfo& info = allItemsInfo[i];
                    currentLine.items.push_back(&info);
                    currentLine.totalHypotheticalMainSize += info.hypotheticalMainSize;
                    currentLine.totalGrow += info.flexGrowFactor;
                    currentLine.totalScaledShrink += info.scaledShrinkFactor;
                    currentLine.visibleItemCount++;
                }
            }
        }
        else {
            lines.emplace_back();
            FlexLine* currentLine = &lines.back();
            float currentLineMainSize = 0.0f;
            int itemsInCurrentLine = 0;
            for (size_t i = 0; i < allItemsInfo.size(); ++i) {
                if (!allItemsInfo[i].isVisible) continue;
                FlexItemInfo& info = allItemsInfo[i];
                float itemSizeWithGap = info.hypotheticalMainSize + (itemsInCurrentLine > 0 ? currentGap : 0.0f);
                if (currentLineMainSize + itemSizeWithGap <= availableInnerMainSize || itemsInCurrentLine == 0) {
                    currentLine->items.push_back(&info);
                    currentLine->totalHypotheticalMainSize += info.hypotheticalMainSize;
                    currentLine->totalGrow += info.flexGrowFactor;
                    currentLine->totalScaledShrink += info.scaledShrinkFactor;
                    currentLineMainSize += itemSizeWithGap;
                    itemsInCurrentLine++;
                    currentLine->visibleItemCount++;
                }
                else {
                    lines.emplace_back();
                    currentLine = &lines.back();
                    currentLine->items.push_back(&info);
                    currentLine->totalHypotheticalMainSize = info.hypotheticalMainSize;
                    currentLine->totalGrow = info.flexGrowFactor;
                    currentLine->totalScaledShrink = info.scaledShrinkFactor;
                    currentLineMainSize = info.hypotheticalMainSize;
                    itemsInCurrentLine = 1;
                    currentLine->visibleItemCount = 1;
                }
            }
        }

        float totalCrossSizeUsed = 0;
        for (auto& line : lines) {
            if (line.items.empty() || line.visibleItemCount == 0) continue;

            float lineGap = (line.visibleItemCount > 1) ? (currentGap * (line.visibleItemCount - 1)) : 0.0f;
            float freeSpace = availableInnerMainSize - line.totalHypotheticalMainSize - lineGap;

            for (auto* infoPtr : line.items) {
                infoPtr->targetMainSize = infoPtr->hypotheticalMainSize - infoPtr->totalMainMargin;
            }

            if (std::fabs(freeSpace) > 1e-4f) {
                if (freeSpace > 0 && line.totalGrow > 1e-6f) {
                    for (auto* infoPtr : line.items) {
                        FlexItemInfo& info = *infoPtr;
                        if (info.flexGrowFactor <= 0) continue;
                        float growShare = (info.flexGrowFactor / line.totalGrow) * freeSpace;
                        float maxMain = isRow ? info.item->maxSize.x : info.item->maxSize.y;
                        info.targetMainSize = std::min(maxMain, info.targetMainSize + growShare);
                    }
                }
                else if (freeSpace < 0 && line.totalScaledShrink > 1e-6f) {
                    for (auto* infoPtr : line.items) {
                        FlexItemInfo& info = *infoPtr;
                        if (info.scaledShrinkFactor <= 1e-6f) continue;
                        float shrinkShare = (info.scaledShrinkFactor / line.totalScaledShrink) * std::fabs(freeSpace);
                        float minMain = isRow ? info.item->minSize.x : info.item->minSize.y;
                        info.targetMainSize = std::max(minMain, info.targetMainSize - shrinkShare);
                    }
                }
            }

            line.crossSize = 0.0f;
            line.maxBaselineOffset = 0.0f;
            bool lineHasBaselineItem = false;

            for (auto* infoPtr : line.items) {
                FlexItemInfo& info = *infoPtr;
                info.targetMainSize = std::max(0.0f, info.targetMainSize);
                info.targetCrossSize = info.crossSize - info.totalCrossMargin;
                line.crossSize = std::max(line.crossSize, info.crossSize);
                AlignSelf itemAlignSelfResolved = ResolveAlignSelf(info.item->alignSelf, flexLayout->alignItems);
                if (flexLayout->alignItems == AlignItems::Baseline || itemAlignSelfResolved == AlignSelf::Baseline) {
                    ImVec2 currentItemSize = isRow ? ImVec2(info.targetMainSize, info.targetCrossSize) : ImVec2(info.targetCrossSize, info.targetMainSize);
                    info.baselineOffset = GetBaselineOffset(*(info.item), currentItemSize);
                    line.maxBaselineOffset = std::max(line.maxBaselineOffset, info.baselineOffset);
                    lineHasBaselineItem = true;
                }
                else {
                    info.baselineOffset = 0;
                }
            }

            if (lineHasBaselineItem) {
                float requiredCrossSizeForBaselineMarginIncluded = 0;
                for (auto* infoPtr : line.items) {
                    FlexItemInfo& info = *infoPtr;
                    AlignSelf itemAlignSelfResolved = ResolveAlignSelf(info.item->alignSelf, flexLayout->alignItems);
                    if (flexLayout->alignItems == AlignItems::Baseline || itemAlignSelfResolved == AlignSelf::Baseline) {
                        requiredCrossSizeForBaselineMarginIncluded = std::max(requiredCrossSizeForBaselineMarginIncluded,
                            info.targetCrossSize + info.marginCrossStart + (line.maxBaselineOffset - info.baselineOffset) + info.marginCrossEnd);
                    }
                }
                line.crossSize = std::max(line.crossSize, requiredCrossSizeForBaselineMarginIncluded);
            }

            float lineFinalMainSizeWithMargins = 0;
            for (const auto* infoPtr : line.items) { lineFinalMainSizeWithMargins += (infoPtr->targetMainSize + infoPtr->totalMainMargin); }
            lineFinalMainSizeWithMargins += lineGap;
            float lineRemainingSpace = availableInnerMainSize - lineFinalMainSizeWithMargins;
            line.mainAxisPosition = padMainStart;
            if (lineRemainingSpace > 1e-4f) {
                switch (flexLayout->justifyContent) {
                case JustifyContent::FlexEnd: line.mainAxisPosition += lineRemainingSpace; break;
                case JustifyContent::Center: line.mainAxisPosition += lineRemainingSpace * 0.5f; break;
                case JustifyContent::SpaceAround:
                case JustifyContent::SpaceBetween:
                case JustifyContent::SpaceEvenly:
                case JustifyContent::FlexStart:
                default: break;
                }
            }
            totalCrossSizeUsed += line.crossSize;
        }
        totalCrossSizeUsed += (lines.size() > 1) ? (currentGap * (lines.size() - 1)) : 0.0f;

        float currentCrossPos = padCrossStart;
        float crossFreeSpace = availableInnerCrossSize - totalCrossSizeUsed;
        float spacingBetweenLines = currentGap;

        if (crossFreeSpace > 1e-4f && lines.size() > 0) {
            switch (flexLayout->alignContent) {
            case AlignContent::FlexEnd: currentCrossPos += crossFreeSpace; break;
            case AlignContent::Center: currentCrossPos += crossFreeSpace * 0.5f; break;
            case AlignContent::SpaceBetween: if (lines.size() > 1) spacingBetweenLines += crossFreeSpace / (float)(lines.size() - 1); else currentCrossPos += crossFreeSpace * 0.5f; break;
            case AlignContent::SpaceAround: spacingBetweenLines += crossFreeSpace / (float)lines.size(); currentCrossPos += (spacingBetweenLines - currentGap) * 0.5f; break;
            case AlignContent::Stretch: spacingBetweenLines = currentGap; break;
            case AlignContent::SpaceEvenly: spacingBetweenLines += crossFreeSpace / (float)(lines.size() + 1); currentCrossPos += (spacingBetweenLines - currentGap); break;
            case AlignContent::FlexStart: default: break;
            }
        }

        float stretchAmountPerLine = (flexLayout->alignContent == AlignContent::Stretch && crossFreeSpace > 1e-4f && lines.size() > 0) ? crossFreeSpace / (float)lines.size() : 0.0f;

        size_t lineStartIndex = isWrapReverse ? lines.size() - 1 : 0;
        int lineDirection = isWrapReverse ? -1 : 1;

        for (int lineIdx = 0; lineIdx < lines.size(); ++lineIdx) {
            size_t actualLineIdx = lineStartIndex + lineIdx * lineDirection;
            FlexLine& line = lines[actualLineIdx];
            if (line.items.empty() || line.visibleItemCount == 0) continue;

            float lineCrossSizeFinal = line.crossSize + stretchAmountPerLine;
            line.crossAxisPosition = currentCrossPos;

            float currentMainPosInLine = line.mainAxisPosition;
            float lineFinalMainSizeWithMargins = 0;
            for (const auto* infoPtr : line.items) { lineFinalMainSizeWithMargins += (infoPtr->targetMainSize + infoPtr->totalMainMargin); }
            lineFinalMainSizeWithMargins += (line.visibleItemCount > 1) ? (currentGap * (line.visibleItemCount - 1)) : 0.0f;
            float lineRemainingSpace = availableInnerMainSize - lineFinalMainSizeWithMargins;
            float spacingBetweenItems = currentGap;
            float initialMainOffset = 0.0f;

            if (lineRemainingSpace > 1e-4f) {
                switch (flexLayout->justifyContent) {
                case JustifyContent::SpaceBetween: if (line.visibleItemCount > 1) spacingBetweenItems += lineRemainingSpace / (float)(line.visibleItemCount - 1); break;
                case JustifyContent::SpaceAround: spacingBetweenItems += lineRemainingSpace / (float)(line.visibleItemCount); initialMainOffset = (spacingBetweenItems - currentGap) * 0.5f; break;
                case JustifyContent::SpaceEvenly: spacingBetweenItems += lineRemainingSpace / (float)(line.visibleItemCount + 1); initialMainOffset = (spacingBetweenItems - currentGap); break;
                default: break;
                }
            }
            currentMainPosInLine += initialMainOffset;

            size_t itemStartIndex = isMainReverse ? line.items.size() - 1 : 0;
            int itemDirection = isMainReverse ? -1 : 1;

            for (int itemIdx = 0; itemIdx < line.items.size(); ++itemIdx) {
                size_t actualItemIdx = itemStartIndex + itemIdx * itemDirection;
                FlexItemInfo& info = *line.items[actualItemIdx];
                ShapeItem* child = info.item;
                AlignSelf itemAlignSelfResolved = ResolveAlignSelf(child->alignSelf, flexLayout->alignItems);
                float itemMainSize = info.targetMainSize;
                float itemCrossSize = info.targetCrossSize;

                if (itemAlignSelfResolved == AlignSelf::Stretch) {
                    float lineAvailableCrossInner = std::max(0.0f, lineCrossSizeFinal - info.totalCrossMargin);
                    itemCrossSize = lineAvailableCrossInner;
                    float minCross = isRow ? child->minSize.y : child->minSize.x;
                    float maxCross = isRow ? child->maxSize.y : child->maxSize.x;
                    itemCrossSize = std::max(minCross, std::min(itemCrossSize, maxCross));
                    itemCrossSize = std::max(0.0f, itemCrossSize);
                }

                float contentBoxMainPos = currentMainPosInLine + info.marginMainStart;
                float contentBoxCrossPos = line.crossAxisPosition + info.marginCrossStart;
                bool useBaseline = (flexLayout->alignItems == AlignItems::Baseline || itemAlignSelfResolved == AlignSelf::Baseline);
                float borderBoxMainPos = currentMainPosInLine + info.marginMainStart;
                float borderBoxCrossPos = line.crossAxisPosition + info.marginCrossStart;
                float itemOuterCrossSize = itemCrossSize + info.totalCrossMargin;
                float crossSpaceInLine = lineCrossSizeFinal - itemOuterCrossSize;

                if (useBaseline) {
                    borderBoxCrossPos += line.maxBaselineOffset - info.baselineOffset;
                    switch (itemAlignSelfResolved) {
                    case AlignSelf::FlexEnd:   borderBoxCrossPos += crossSpaceInLine; break;
                    case AlignSelf::Center:    borderBoxCrossPos += crossSpaceInLine * 0.5f; break;
                    default: break;
                    }
                }
                else {
                    switch (itemAlignSelfResolved) {
                    case AlignSelf::FlexEnd:   borderBoxCrossPos += crossSpaceInLine; break;
                    case AlignSelf::Center:    borderBoxCrossPos += crossSpaceInLine * 0.5f; break;
                    case AlignSelf::Stretch:
                    case AlignSelf::FlexStart:
                    default: break;
                    }
                }

                if (isRow) {
                    child->position = ImVec2(borderBoxMainPos, borderBoxCrossPos);
                    child->size = ImVec2(itemMainSize, itemCrossSize);
                }
                else {
                    child->position = ImVec2(borderBoxCrossPos, borderBoxMainPos);
                    child->size = ImVec2(itemCrossSize, itemMainSize);
                }
                child->rotation = child->baseRotation;
                currentMainPosInLine += itemMainSize + info.totalMainMargin + spacingBetweenItems;
            }
            currentCrossPos += lineCrossSizeFinal + spacingBetweenLines;
        }
    }

    void GridLayout::doLayout(ShapeItem& container, const ImVec2& availableSize)
    {
        if (container.children.empty() || !container.layoutManager) {
            return;
        }

        GridLayout* gridLayout = static_cast<GridLayout*>(container.layoutManager.get());
        if (!gridLayout) return;

        float padLeftPx = container.padding.x;
        float padTopPx = container.padding.y;
        float padRightPx = container.padding.z;
        float padBottomPx = container.padding.w;
        float colGapPx = gridLayout->columnGap.getPixels(availableSize.x);
        float rowGapPx = gridLayout->rowGap.getPixels(availableSize.y);
        const float availableInnerWidth = std::max(0.0f, availableSize.x - padLeftPx - padRightPx);
        const float availableInnerHeight = std::max(0.0f, availableSize.y - padTopPx - padBottomPx);
        float implicitColPx = gridLayout->implicitTrackColSize.getPixels(availableInnerWidth);
        float implicitRowPx = gridLayout->implicitTrackRowSize.getPixels(availableInnerHeight);

        std::vector<ShapeItem*> itemsToPlace;
        for (auto* child : container.children) {
            if (child && child->visible && child->positioningMode == PositioningMode::Relative) {
                itemsToPlace.push_back(child);
            }
        }
        std::sort(itemsToPlace.begin(), itemsToPlace.end(), [](ShapeItem* a, ShapeItem* b) {
            return a->order < b->order;
            });

        std::map<std::pair<int, int>, bool> gridOccupancy;
        std::vector<ShapeItem*> itemsForAutoPlacement;
        int explicitMaxCols = 0;
        int explicitMaxRows = 0;

        struct ItemPlacement {
            int r_start = -1, r_end = -1, c_start = -1, c_end = -1;
            int r_span = 1, c_span = 1;
            bool placedExplicitly = false;
        };
        std::map<int, ItemPlacement> itemPlacements;

        for (auto* child : itemsToPlace) {
            ItemPlacement placement;
            int r_start_1based = child->gridRowStart;
            int r_end_1based = child->gridRowEnd;
            int c_start_1based = child->gridColumnStart;
            int c_end_1based = child->gridColumnEnd;
            bool rowExplicitStart = r_start_1based > 0;
            bool colExplicitStart = c_start_1based > 0;
            bool rowExplicitEnd = r_end_1based > 0;
            bool colExplicitEnd = c_end_1based > 0;

            placement.r_start = rowExplicitStart ? std::max(0, r_start_1based - 1) : -1;
            placement.c_start = colExplicitStart ? std::max(0, c_start_1based - 1) : -1;

            if (rowExplicitEnd && r_end_1based > 0) {
                if (rowExplicitStart && r_end_1based > r_start_1based) { placement.r_span = std::max(1, r_end_1based - r_start_1based); }
                else if (!rowExplicitStart) { placement.r_span = std::max(1, r_end_1based); }
                else { placement.r_span = 1; }
            }
            else if (rowExplicitStart && r_end_1based < 0 && r_end_1based != -1) {
                placement.r_span = std::max(1, -r_end_1based);
            }
            else { placement.r_span = 1; }

            if (colExplicitEnd && c_end_1based > 0) {
                if (colExplicitStart && c_end_1based > c_start_1based) { placement.c_span = std::max(1, c_end_1based - c_start_1based); }
                else if (!colExplicitStart) { placement.c_span = std::max(1, c_end_1based); }
                else { placement.c_span = 1; }
            }
            else if (colExplicitStart && c_end_1based < 0 && c_end_1based != -1) {
                placement.c_span = std::max(1, -c_end_1based);
            }
            else { placement.c_span = 1; }

            if (placement.r_start != -1) placement.r_end = placement.r_start + placement.r_span; else placement.r_end = -1;
            if (placement.c_start != -1) placement.c_end = placement.c_start + placement.c_span; else placement.c_end = -1;

            placement.placedExplicitly = rowExplicitStart || colExplicitStart;
            itemPlacements[child->id] = placement;

            if (rowExplicitStart && colExplicitStart) {
                for (int r = placement.r_start; r < placement.r_end; ++r) {
                    explicitMaxRows = std::max(explicitMaxRows, r + 1);
                    for (int c = placement.c_start; c < placement.c_end; ++c) {
                        explicitMaxCols = std::max(explicitMaxCols, c + 1);
                        gridOccupancy[{r, c}] = true;
                    }
                }
            }
            else {
                itemsForAutoPlacement.push_back(child);
            }
        }

        int currentMaxCols = gridLayout->templateColumns.size() > 0 ? gridLayout->templateColumns.size() : explicitMaxCols;
        int currentMaxRows = gridLayout->templateRows.size() > 0 ? gridLayout->templateRows.size() : explicitMaxRows;
        currentMaxCols = std::max(1, currentMaxCols);
        currentMaxRows = std::max(1, currentMaxRows);
        int autoPlaceRow = 0;
        int autoPlaceCol = 0;
        bool isRowFlow = (gridLayout->autoFlow == GridAutoFlow::Row || gridLayout->autoFlow == GridAutoFlow::RowDense);
        bool isDense = (gridLayout->autoFlow == GridAutoFlow::RowDense || gridLayout->autoFlow == GridAutoFlow::ColumnDense);

        for (auto* child : itemsForAutoPlacement) {
            ItemPlacement& placement = itemPlacements[child->id];
            int r_start_hint = placement.r_start;
            int r_span = placement.r_span;
            int c_start_hint = placement.c_start;
            int c_span = placement.c_span;
            bool placed = false;
            int finalPlaceRow = -1;
            int finalPlaceCol = -1;
            const int searchLimit = 200;

            if (isDense) {
                int max_r_search = currentMaxRows + searchLimit;
                int max_c_search = currentMaxCols + searchLimit;

                if (isRowFlow) {
                    for (int r_search = 0; r_search < max_r_search && !placed; ++r_search) {
                        for (int c_search = 0; c_search < max_c_search && !placed; ++c_search) {
                            if (r_start_hint != -1 && r_search != r_start_hint) continue;
                            if (c_start_hint != -1 && c_search != c_start_hint) continue;

                            bool canPlace = true;
                            for (int r = r_search; r < r_search + r_span; ++r) {
                                for (int c = c_search; c < c_search + c_span; ++c) {
                                    if (gridOccupancy.count({ r, c }) && gridOccupancy.at({ r, c })) {
                                        canPlace = false; break;
                                    }
                                }
                                if (!canPlace) break;
                            }
                            if (canPlace) { finalPlaceRow = r_search; finalPlaceCol = c_search; placed = true; }
                            if (r_start_hint != -1 && c_start_hint == -1 && placed) break;
                            if (r_start_hint == -1 && c_start_hint != -1 && placed) break;
                            if (r_start_hint != -1 && c_start_hint != -1 && placed) break;
                        }
                        if (r_start_hint != -1 && placed) break;
                    }
                }
                else {
                    for (int c_search = 0; c_search < max_c_search && !placed; ++c_search) {
                        for (int r_search = 0; r_search < max_r_search && !placed; ++r_search) {
                            if (r_start_hint != -1 && r_search != r_start_hint) continue;
                            if (c_start_hint != -1 && c_search != c_start_hint) continue;

                            bool canPlace = true;
                            for (int r = r_search; r < r_search + r_span; ++r) {
                                for (int c = c_search; c < c_search + c_span; ++c) {
                                    if (gridOccupancy.count({ r, c }) && gridOccupancy.at({ r, c })) {
                                        canPlace = false; break;
                                    }
                                }
                                if (!canPlace) break;
                            }
                            if (canPlace) { finalPlaceRow = r_search; finalPlaceCol = c_search; placed = true; }
                            if (r_start_hint != -1 && c_start_hint == -1 && placed) break;
                            if (r_start_hint == -1 && c_start_hint != -1 && placed) break;
                            if (r_start_hint != -1 && c_start_hint != -1 && placed) break;
                        }
                        if (c_start_hint != -1 && placed) break;
                    }
                }
            }
            else {
                int r = (r_start_hint != -1) ? r_start_hint : autoPlaceRow;
                int c = (c_start_hint != -1) ? c_start_hint : autoPlaceCol;
                int sparseSafety = 0;

                while (!placed && sparseSafety < searchLimit * searchLimit) {
                    if (r < 0) r = 0;
                    if (c < 0) c = 0;

                    if (isRowFlow && c >= currentMaxCols) { c = 0; r++; continue; }
                    else if (!isRowFlow && r >= currentMaxRows) { r = 0; c++; continue; }

                    if (r_start_hint != -1 && r != r_start_hint) {
                        if (isRowFlow) { c = 0; r++; }
                        else { r = 0; c++; } sparseSafety++; continue;
                    }
                    if (c_start_hint != -1 && c != c_start_hint) {
                        if (isRowFlow) { c = 0; r++; }
                        else { r = 0; c++; } sparseSafety++; continue;
                    }

                    bool canPlace = true;
                    for (int rr = r; rr < r + r_span; ++rr) {
                        for (int cc = c; cc < c + c_span; ++cc) {
                            if (isRowFlow && cc >= currentMaxCols) { canPlace = false; break; }
                            if (!isRowFlow && rr >= currentMaxRows) { canPlace = false; break; }
                            if (gridOccupancy.count({ rr, cc }) && gridOccupancy.at({ rr, cc })) {
                                canPlace = false; break;
                            }
                        }
                        if (!canPlace) break;
                    }

                    if (canPlace) {
                        finalPlaceRow = r;
                        finalPlaceCol = c;
                        placed = true;
                    }
                    else {
                        if (r_start_hint != -1 || c_start_hint != -1) { break; }
                        if (isRowFlow) { c++; }
                        else { r++; }
                    }
                    sparseSafety++;
                }

                if (placed && r_start_hint == -1 && c_start_hint == -1) {
                    if (isRowFlow) {
                        autoPlaceCol = finalPlaceCol + c_span;
                        autoPlaceRow = finalPlaceRow;
                        if (autoPlaceCol >= currentMaxCols) {
                            autoPlaceCol = 0;
                            autoPlaceRow++;
                        }
                    }
                    else {
                        autoPlaceRow = finalPlaceRow + r_span;
                        autoPlaceCol = finalPlaceCol;
                        if (autoPlaceRow >= currentMaxRows) {
                            autoPlaceRow = 0;
                            autoPlaceCol++;
                        }
                    }
                }
            }

            if (placed) {
                placement.r_start = finalPlaceRow;
                placement.r_end = finalPlaceRow + r_span;
                placement.c_start = finalPlaceCol;
                placement.c_end = finalPlaceCol + c_span;

                for (int r = placement.r_start; r < placement.r_end; ++r) {
                    currentMaxRows = std::max(currentMaxRows, r + 1);
                    for (int c = placement.c_start; c < placement.c_end; ++c) {
                        currentMaxCols = std::max(currentMaxCols, c + 1);
                        gridOccupancy[{r, c}] = true;
                    }
                }
            }
            else {
                itemPlacements.erase(child->id);
                std::cerr << "Warning: Could not place grid item '" << child->name << "' (ID: " << child->id << ")" << std::endl;
            }
        }

        int numCols = std::max(gridLayout->templateColumns.size(), (size_t)currentMaxCols);
        int numRows = std::max(gridLayout->templateRows.size(), (size_t)currentMaxRows);
        numCols = std::max(1, numCols);
        numRows = std::max(1, numRows);
        std::vector<GridTrackSize> finalColTemplates = gridLayout->templateColumns;
        while (finalColTemplates.size() < numCols) {
            GridTrackSize implicitColTrack; implicitColTrack.mode = GridTrackSize::Mode::Auto; finalColTemplates.push_back(implicitColTrack);
        }
        std::vector<GridTrackSize> finalRowTemplates = gridLayout->templateRows;
        while (finalRowTemplates.size() < numRows) {
            GridTrackSize implicitRowTrack; implicitRowTrack.mode = GridTrackSize::Mode::Auto; finalRowTemplates.push_back(implicitRowTrack);
        }

        auto compute_track_sizes_final =
            [&](const std::vector<GridTrackSize>& templates,
                float available_space,
                float gap_px,
                bool is_column_axis) -> std::vector<float>
            {
                size_t track_count = templates.size();
                if (track_count == 0) return {};

                std::vector<float> final_sizes(track_count, 0.0f);
                std::vector<float> max_content_sizes(track_count, 0.0f);
                std::vector<float> min_track_sizes(track_count, 0.0f);
                std::vector<float> max_track_sizes(track_count, std::numeric_limits<float>::max());
                std::vector<size_t> fr_indices;
                std::vector<size_t> auto_indices;
                std::vector<size_t> stretch_indices;
                float total_fr_factor = 0.0f;
                float fixed_space_used = 0.0f;
                float implicitTrackSizePx = is_column_axis ? implicitColPx : implicitRowPx;

                for (const auto& pair : itemPlacements) {
                    const ShapeItem* child = FindShapeByID(pair.first);
                    if (!child) continue;
                    const ItemPlacement& p = pair.second;
                    if (p.r_start < 0 || p.c_start < 0) continue;

                    float marginStart = is_column_axis ? child->margin.x : child->margin.y;
                    float marginEnd = is_column_axis ? child->margin.z : child->margin.w;
                    float totalMargin = marginStart + marginEnd;

                    ImVec2 preferredSize = child->baseSize;
                    bool widthFixed = false, heightFixed = false;
                    for (const auto& constraint : child->constraints) {
                        if (constraint.type == ConstraintType::WidthFixed && constraint.value >= 0) { preferredSize.x = constraint.value; widthFixed = true; }
                        else if (constraint.type == ConstraintType::HeightFixed && constraint.value >= 0) { preferredSize.y = constraint.value; heightFixed = true; }
                    }
                    for (const auto& constraint : child->constraints) {
                        float parentConstraintSize = is_column_axis ? availableInnerWidth : availableInnerHeight;
                        if (constraint.type == ConstraintType::WidthPercentage && constraint.value >= 0 && !widthFixed && is_column_axis) { preferredSize.x = parentConstraintSize * (constraint.value / 100.0f); widthFixed = true; }
                        else if (constraint.type == ConstraintType::HeightPercentage && constraint.value >= 0 && !heightFixed && !is_column_axis) { preferredSize.y = parentConstraintSize * (constraint.value / 100.0f); heightFixed = true; }
                    }
                    float aspectRatio = -1.0f;
                    for (const auto& constraint : child->constraints) { if (constraint.type == ConstraintType::AspectRatio && constraint.value > 1e-6f) { aspectRatio = constraint.value; break; } }
                    if (aspectRatio > 0) {
                        if (widthFixed && !heightFixed) preferredSize.y = preferredSize.x / aspectRatio;
                        else if (!widthFixed && heightFixed) preferredSize.x = preferredSize.y * aspectRatio;
                    }

                    float content_measure = is_column_axis ? preferredSize.x : preferredSize.y;
                    content_measure = std::max(is_column_axis ? child->minSize.x : child->minSize.y, std::min(content_measure, is_column_axis ? child->maxSize.x : child->maxSize.y));
                    content_measure = std::max(0.0f, content_measure);
                    int start = is_column_axis ? p.c_start : p.r_start;
                    int end = is_column_axis ? p.c_end : p.r_end;
                    int span = is_column_axis ? p.c_span : p.r_span;

                    if (start >= 0 && end > start && start < static_cast<int>(track_count)) {
                        if (span == 1) {
                            max_content_sizes[start] = std::max(max_content_sizes[start], content_measure);
                        }
                        else {
                            for (int k = start; k < end && k < static_cast<int>(track_count); ++k) {
                                max_content_sizes[k] = std::max(max_content_sizes[k], content_measure / span);
                            }
                        }
                    }
                }
                float total_gap = (track_count > 1) ? (gap_px * (track_count - 1)) : 0.0f;
                float available_after_gap = std::max(0.0f, available_space - total_gap);

                for (size_t i = 0; i < track_count; ++i) {
                    const auto& track = templates[i];
                    float content_size = max_content_sizes[i];

                    if (track.mode == GridTrackSize::Mode::Fixed) {
                        final_sizes[i] = std::max(0.0f, track.value);
                        min_track_sizes[i] = final_sizes[i];
                        max_track_sizes[i] = final_sizes[i];
                        fixed_space_used += final_sizes[i];
                    }
                    else if (track.mode == GridTrackSize::Mode::Percentage) {
                        final_sizes[i] = std::max(0.0f, available_after_gap * (track.value / 100.0f));
                        min_track_sizes[i] = final_sizes[i];
                        max_track_sizes[i] = final_sizes[i];
                        fixed_space_used += final_sizes[i];
                    }
                    else if (track.mode == GridTrackSize::Mode::Auto) {
                        final_sizes[i] = std::max(0.0f, (content_size > 1e-6f) ? content_size : implicitTrackSizePx);
                        min_track_sizes[i] = final_sizes[i];
                        max_track_sizes[i] = final_sizes[i];
                        fixed_space_used += final_sizes[i];
                        auto_indices.push_back(i);
                    }
                    else if (track.mode == GridTrackSize::Mode::Fraction) {
                        final_sizes[i] = 0;
                        min_track_sizes[i] = 0;
                        max_track_sizes[i] = std::numeric_limits<float>::max();
                        fr_indices.push_back(i);
                        stretch_indices.push_back(i);
                        total_fr_factor += std::max(0.0f, track.value);
                    }
                    else if (track.mode == GridTrackSize::Mode::MinMax) {
                        min_track_sizes[i] = track.minVal.getPixels(available_after_gap, content_size > 1e-6f ? content_size : implicitTrackSizePx);
                        if (!track.maxVal.isFr()) {
                            max_track_sizes[i] = track.maxVal.getPixels(available_after_gap, content_size > 1e-6f ? content_size : implicitTrackSizePx);
                        }
                        else {
                            max_track_sizes[i] = std::numeric_limits<float>::max();
                            fr_indices.push_back(i);
                            stretch_indices.push_back(i);
                            total_fr_factor += std::max(0.0f, track.maxVal.value);
                        }
                        min_track_sizes[i] = std::max(0.0f, min_track_sizes[i]);
                        max_track_sizes[i] = std::max(min_track_sizes[i], max_track_sizes[i]);

                        final_sizes[i] = std::max(min_track_sizes[i], content_size);
                        final_sizes[i] = std::min(final_sizes[i], max_track_sizes[i]);
                        fixed_space_used += final_sizes[i];
                    }
                }

                float remaining_space_for_fr = available_after_gap - fixed_space_used;

                if (!stretch_indices.empty() && remaining_space_for_fr > 1e-6f && total_fr_factor > 1e-6f) {
                    float fr_unit_size = remaining_space_for_fr / total_fr_factor;
                    float space_distributed = 0.0f;

                    for (size_t i : stretch_indices) {
                        if (final_sizes[i] < min_track_sizes[i]) {
                            float needed = min_track_sizes[i] - final_sizes[i];
                            float can_distribute = std::min(needed, remaining_space_for_fr - space_distributed);
                            final_sizes[i] += can_distribute;
                            space_distributed += can_distribute;
                            if (space_distributed >= remaining_space_for_fr - 1e-6f) break;
                        }
                    }

                    float space_left_after_min = remaining_space_for_fr - space_distributed;
                    if (space_left_after_min > 1e-6f && total_fr_factor > 1e-6f)
                    {
                        float current_fr_unit = space_left_after_min / total_fr_factor;
                        for (size_t i : stretch_indices) {
                            float fr_val = 0.0f;
                            const auto& track = templates[i];
                            if (track.mode == GridTrackSize::Mode::Fraction) fr_val = track.value;
                            else if (track.mode == GridTrackSize::Mode::MinMax && track.maxVal.isFr()) fr_val = track.maxVal.value;

                            if (fr_val > 1e-6f) {
                                float potential_add = fr_val * current_fr_unit;
                                float space_available_until_max = max_track_sizes[i] - final_sizes[i];
                                float actual_add = std::max(0.0f, std::min(potential_add, space_available_until_max));
                                final_sizes[i] += actual_add;
                            }
                        }
                    }
                }
                for (float& size : final_sizes) {
                    if (!std::isfinite(size) || size < 0.0f) {
                        size = 0.0f;
                    }
                }
                return final_sizes;
            };

        std::vector<float> colSizes = compute_track_sizes_final(finalColTemplates, availableInnerWidth, colGapPx, true);
        std::vector<float> rowSizes = compute_track_sizes_final(finalRowTemplates, availableInnerHeight, rowGapPx, false);

        auto compute_positions = [&](const std::vector<float>& sizes, float start_offset_px, float gap_px) -> std::vector<float> {
            std::vector<float> positions; float current_pos = start_offset_px; positions.push_back(current_pos);
            if (!sizes.empty()) {
                for (size_t i = 0; i < sizes.size(); ++i) {
                    current_pos += sizes[i];
                    if (i < sizes.size() - 1) { current_pos += gap_px; }
                    positions.push_back(current_pos);
                }
            }
            else { positions.push_back(start_offset_px); }
            return positions;
            };
        std::vector<float> colPositions = compute_positions(colSizes, padLeftPx, colGapPx);
        std::vector<float> rowPositions = compute_positions(rowSizes, padTopPx, rowGapPx);

        float totalActualGridWidth = 0; if (!colSizes.empty()) { totalActualGridWidth = colPositions.back() - colPositions.front(); }
        float totalActualGridHeight = 0; if (!rowSizes.empty()) { totalActualGridHeight = rowPositions.back() - rowPositions.front(); }
        float gridOffsetX = 0.0f;
        float gridOffsetY = 0.0f;
        float freeSpaceX = availableInnerWidth - totalActualGridWidth;
        float freeSpaceY = availableInnerHeight - totalActualGridHeight;

        if (freeSpaceX > 1e-4f) {
            switch (gridLayout->justifyContent) {
            case JustifyContent::FlexEnd: gridOffsetX = freeSpaceX; break;
            case JustifyContent::Center: gridOffsetX = freeSpaceX * 0.5f; break;
            default: gridOffsetX = 0.0f; break;
            }
        }
        if (freeSpaceY > 1e-4f) {
            switch (gridLayout->alignContent) {
            case AlignContent::FlexEnd: gridOffsetY = freeSpaceY; break;
            case AlignContent::Center: gridOffsetY = freeSpaceY * 0.5f; break;
            default: gridOffsetY = 0.0f; break;
            }
        }

        for (auto* child : itemsToPlace) {
            if (!itemPlacements.count(child->id)) continue;
            const ItemPlacement& p = itemPlacements.at(child->id);
            if (p.r_start < 0 || p.c_start < 0 || p.r_end > numRows || p.c_end > numCols || p.r_end <= p.r_start || p.c_end <= p.c_start) {
                continue;
            }

            float cellX = (p.c_start < colPositions.size()) ? colPositions[p.c_start] + gridOffsetX : gridOffsetX + padLeftPx;
            float cellY = (p.r_start < rowPositions.size()) ? rowPositions[p.r_start] + gridOffsetY : gridOffsetY + padTopPx;
            float cellWidth = 0;
            for (int c = p.c_start; c < p.c_end && c < colSizes.size(); ++c) { cellWidth += colSizes[c]; if (c < p.c_end - 1) cellWidth += colGapPx; }
            float cellHeight = 0;
            for (int r = p.r_start; r < p.r_end && r < rowSizes.size(); ++r) { cellHeight += rowSizes[r]; if (r < p.r_end - 1) cellHeight += rowGapPx; }
            cellWidth = std::max(0.0f, cellWidth);
            cellHeight = std::max(0.0f, cellHeight);

            const float marginLeft = child->margin.x;
            const float marginRight = child->margin.z;
            const float marginTop = child->margin.y;
            const float marginBottom = child->margin.w;
            float availableContentWidth = std::max(0.0f, cellWidth - marginLeft - marginRight);
            float availableContentHeight = std::max(0.0f, cellHeight - marginTop - marginBottom);

            ImVec2 itemContentSize = child->baseSize;
            bool widthSetByConstraint = false;
            bool heightSetByConstraint = false;

            for (const auto& constraint : child->constraints) {
                if (constraint.type == ConstraintType::WidthFixed && constraint.value >= 0) { itemContentSize.x = constraint.value; widthSetByConstraint = true; }
                if (constraint.type == ConstraintType::HeightFixed && constraint.value >= 0) { itemContentSize.y = constraint.value; heightSetByConstraint = true; }
            }
            for (const auto& constraint : child->constraints) {
                if (constraint.type == ConstraintType::WidthPercentage && constraint.value >= 0 && !widthSetByConstraint) { itemContentSize.x = availableContentWidth * (constraint.value / 100.0f); widthSetByConstraint = true; }
                if (constraint.type == ConstraintType::HeightPercentage && constraint.value >= 0 && !heightSetByConstraint) { itemContentSize.y = availableContentHeight * (constraint.value / 100.0f); heightSetByConstraint = true; }
            }
            float aspectRatio = -1.0f;
            for (const auto& constraint : child->constraints) { if (constraint.type == ConstraintType::AspectRatio && constraint.value > 1e-6f) { aspectRatio = constraint.value; break; } }
            if (aspectRatio > 0) {
                if (widthSetByConstraint && !heightSetByConstraint) itemContentSize.y = itemContentSize.x / aspectRatio;
                else if (!widthSetByConstraint && heightSetByConstraint) itemContentSize.x = itemContentSize.y * aspectRatio;
                else if (!widthSetByConstraint && !heightSetByConstraint) {
                    if (availableContentWidth / aspectRatio <= availableContentHeight + 1e-4f) {
                        itemContentSize.x = availableContentWidth; itemContentSize.y = availableContentHeight / aspectRatio;
                    }
                    else {
                        itemContentSize.y = availableContentHeight; itemContentSize.x = availableContentHeight * aspectRatio;
                    }
                    widthSetByConstraint = true;
                    heightSetByConstraint = true;
                }
            }

            GridAxisAlignment itemJustifyResolved = child->justifySelf;
            if (itemJustifyResolved == GridAxisAlignment::Stretch && widthSetByConstraint) itemJustifyResolved = GridAxisAlignment::Start;
            if (itemJustifyResolved == GridAxisAlignment::Stretch) itemJustifyResolved = gridLayout->defaultCellContentJustify;
            GridAxisAlignment itemAlignResolved = child->alignSelfGrid;
            if (itemAlignResolved == GridAxisAlignment::Stretch && heightSetByConstraint) itemAlignResolved = GridAxisAlignment::Start;
            if (itemAlignResolved == GridAxisAlignment::Stretch) itemAlignResolved = gridLayout->defaultCellContentAlign;

            if (itemJustifyResolved == GridAxisAlignment::Stretch) itemContentSize.x = availableContentWidth;
            if (itemAlignResolved == GridAxisAlignment::Stretch) itemContentSize.y = availableContentHeight;

            itemContentSize.x = std::max(child->minSize.x, std::min(itemContentSize.x, child->maxSize.x));
            itemContentSize.y = std::max(child->minSize.y, std::min(itemContentSize.y, child->maxSize.y));
            itemContentSize.x = std::max(0.0f, itemContentSize.x);
            itemContentSize.y = std::max(0.0f, itemContentSize.y);

            float marginBoxWidth = itemContentSize.x + marginLeft + marginRight;
            float marginBoxHeight = itemContentSize.y + marginTop + marginBottom;
            float offsetX = 0.0f;
            float offsetY = 0.0f;
            float remainingWidth = cellWidth - marginBoxWidth;
            float remainingHeight = cellHeight - marginBoxHeight;

            if (itemJustifyResolved == GridAxisAlignment::End)       offsetX = remainingWidth;
            else if (itemJustifyResolved == GridAxisAlignment::Center) offsetX = remainingWidth * 0.5f;
            if (itemAlignResolved == GridAxisAlignment::End)         offsetY = remainingHeight;
            else if (itemAlignResolved == GridAxisAlignment::Center)  offsetY = remainingHeight * 0.5f;

            float borderBoxX = cellX + offsetX;
            float borderBoxY = cellY + offsetY;

            if (!std::isfinite(borderBoxX) || !std::isfinite(borderBoxY) || !std::isfinite(itemContentSize.x) || !std::isfinite(itemContentSize.y)) {
                borderBoxX = cellX;
                borderBoxY = cellY;
                itemContentSize.x = std::max(0.0f, std::min(10.0f, cellWidth));
                itemContentSize.y = std::max(0.0f, std::min(10.0f, cellHeight));
                std::cerr << "Warning: Calculated NaN/inf position/size for grid item '" << child->name << "' (ID: " << child->id << "). Resetting." << std::endl;
            }
            else {
                borderBoxX += marginLeft;
                borderBoxY += marginTop;
            }

            child->position = ImVec2(borderBoxX, borderBoxY);
            child->size = itemContentSize;
            child->rotation = child->baseRotation;
        }
    }

    void UpdateShapeTransforms_Unified(GLFWwindow* window, float deltaTime)
    {
        for (auto& [winName, windowData] : g_windowsMap)
        {
            ImGuiWindow* imguiWindow = ImGui::FindWindowByName(winName.c_str());
            ImVec2 baseWindowSize = GetWindowSize(window);
            ImVec2 rootContainerSize = (imguiWindow != nullptr && imguiWindow->Size.x > 0 && imguiWindow->Size.y > 0) ? imguiWindow->Size : baseWindowSize;
            rootContainerSize.x = std::max(1.0f, rootContainerSize.x);
            rootContainerSize.y = std::max(1.0f, rootContainerSize.y);

            std::vector<ShapeItem*> rootShapes;
            for (auto& layer : windowData.layers) {
                if (!layer.visible) continue;
                for (auto& shape_uptr : layer.shapes) {
                    if (shape_uptr && shape_uptr->visible && shape_uptr->parent == nullptr && shape_uptr->ownerWindow == winName) {
                        rootShapes.push_back(shape_uptr.get());
                    }
                }
            }

            std::function<void(ShapeItem*, const ImVec2&, ShapeItem*)> processShapeRecursive =
                [&](ShapeItem* shape, const ImVec2& parentSizeForConstraints, ShapeItem* parentShape)
                {
                    if (!shape || !shape->visible) { return; }

                    bool layer_is_locked = false;
                    int current_shape_layer_idx = FindShapeLayerIndex(shape->id);
                    if (current_shape_layer_idx != -1 && g_windowsMap.count(shape->ownerWindow) && current_shape_layer_idx < g_windowsMap[shape->ownerWindow].layers.size()) {
                        layer_is_locked = g_windowsMap[shape->ownerWindow].layers[current_shape_layer_idx].locked;
                    }
                    bool shape_effectively_locked = shape->locked || layer_is_locked;

                    ImVec2 calculatedLocalPosition = shape->basePosition;
                    ImVec2 calculatedLocalSize = shape->baseSize;
                    float calculatedLocalRotation = shape->baseRotation;
                    bool managedByLayout = parentShape && parentShape->isLayoutContainer && parentShape->layoutManager && shape->positioningMode == PositioningMode::Relative;

                    if (!shape_effectively_locked)
                    {
                        if (!managedByLayout) {
                            if (shape->positioningMode == PositioningMode::Absolute) {
                                ApplyConstraints(shape, parentSizeForConstraints, &calculatedLocalPosition, &calculatedLocalSize);
                            }
                            else if (shape->positioningMode == PositioningMode::Relative) {
                                ApplyConstraints(shape, parentSizeForConstraints, &calculatedLocalPosition, &calculatedLocalSize);
                            }
                        }

                        if (shape->currentAnimation && shape->currentAnimation->isPlaying)
                        {
                            ButtonAnimation* anim = shape->currentAnimation;
                            float t = anim->progress;
                            if (anim->interpolationMethod == ButtonAnimation::InterpolationMethod::EaseInOut) {
                                t = t * t * (3.0f - 2.0f * t);
                            }
                            calculatedLocalPosition = Lerp(calculatedLocalPosition, anim->animationTargetPosition, t);
                            calculatedLocalSize = Lerp(calculatedLocalSize, anim->animationTargetSize, t);
                            calculatedLocalRotation = Lerp(calculatedLocalRotation, anim->transformRotation, t);
                        }

                        calculatedLocalSize.x = std::max(shape->minSize.x, std::min(calculatedLocalSize.x, shape->maxSize.x));
                        calculatedLocalSize.y = std::max(shape->minSize.y, std::min(calculatedLocalSize.y, shape->maxSize.y));
                        calculatedLocalSize.x = std::max(0.0f, calculatedLocalSize.x);
                        calculatedLocalSize.y = std::max(0.0f, calculatedLocalSize.y);
                    }

                    ImVec2 finalPosition;
                    ImVec2 finalSize;
                    float finalRotation;

                    if (parentShape != nullptr) {
                        ImVec2 parentFinalPos = parentShape->position;
                        float parentFinalRot = parentShape->rotation;
                        ImVec2 localPositionToUse;
                        ImVec2 localSizeToUse;
                        float localRotationToUse = calculatedLocalRotation;

                        if (managedByLayout) {
                            localPositionToUse = shape->position;
                            localSizeToUse = shape->size;
                        }
                        else {
                            localPositionToUse = calculatedLocalPosition;
                            localSizeToUse = calculatedLocalSize;
                        }

                        ImVec2 rotatedLocalOffset = RotateP(localPositionToUse, ImVec2(0.0f, 0.0f), parentFinalRot);
                        finalPosition = parentFinalPos + rotatedLocalOffset;
                        finalSize = localSizeToUse;
                        finalRotation = parentFinalRot + localRotationToUse;
                    }
                    else {
                        finalPosition = calculatedLocalPosition;
                        finalSize = calculatedLocalSize;
                        finalRotation = calculatedLocalRotation;
                    }

                    if (!shape_effectively_locked) {
                        shape->position = finalPosition;
                        shape->size = finalSize;
                        shape->rotation = finalRotation;
                    }

                    if (!shape_effectively_locked && shape->isLayoutContainer && shape->layoutManager != nullptr) {
                        ImVec2 sizeForLayout = shape->size;
                        try {
                            shape->layoutManager->doLayout(*shape, sizeForLayout);
                        }
                        catch (const std::exception& e) {
                            std::cerr << "      EXCEPTION during doLayout for " << shape->name << ": " << e.what() << std::endl;
                        }
                        catch (...) {
                            std::cerr << "      UNKNOWN EXCEPTION during doLayout for " << shape->name << std::endl;
                        }
                    }

                    if (!shape->children.empty()) {
                        ImVec2 childConstraintContextSize = shape->size;
                        for (ShapeItem* child : shape->children) {
                            if (child == nullptr) {
                                continue;
                            }
                            processShapeRecursive(child, childConstraintContextSize, shape);
                        }
                    }
                };

            for (ShapeItem* rootShape : rootShapes) {
                processShapeRecursive(rootShape, rootContainerSize, nullptr);
            }
        }
    }

    ImVec2 ComputeChainOffset(const ShapeItem& shape) {
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

    void UpdateChainAnimations(float deltaTime)
    {
        for (auto& [winName, windowData] : DesignManager::g_windowsMap)
        {
            for (auto& layer : windowData.layers)
            {
                for (auto& shape_uptr : layer.shapes)
                {
                    if (!shape_uptr) continue;
                    ShapeItem& shape = *shape_uptr;
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

    int GetUniqueShapeID() {
        static int nextShapeIDCounter = 1000;
        static std::unordered_set<int> usedIDs;
        while (usedIDs.count(nextShapeIDCounter))
            ++nextShapeIDCounter;
        usedIDs.insert(nextShapeIDCounter);
        return nextShapeIDCounter++;
    }

    void ShowChainAnimationGUI() {
        ImGui::Begin("Animation Manager");

        std::vector<ShapeItem*> allShapes_ptrs = GetAllShapes();

        static ShapeItem* selectedShape = nullptr;
        ImGui::Text("Select Animation Owner Shape:");
        if (ImGui::BeginListBox("##ShapeList", ImVec2(-1, 150))) {
            for (int i = 0; i < allShapes_ptrs.size(); i++) {
                bool isSelected = (selectedShape == allShapes_ptrs[i]);
                if (ImGui::Selectable(allShapes_ptrs[i]->name.c_str(), isSelected)) {
                    selectedShape = allShapes_ptrs[i];
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

            std::vector<ShapeItem*> buttonShapes = GetAllButtonShapes();

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

    void DrawAll(ImDrawList* dl)
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
                    for (auto& shape_uptr : layer.shapes)
                    {
                        if (shape_uptr && shape_uptr->useGlass && shape_uptr->visible)
                        {
                            shape_uptr->visible = false;
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

                std::stable_sort(layer.shapes.begin(), layer.shapes.end(), [](const auto& a, const auto& b) { return CompareShapesByZOrder(*a, *b); });

                for (auto& shape_uptr : layer.shapes)
                {
                    if (shape_uptr && shape_uptr->ownerWindow == oWindowName && shape_uptr->visible)
                    {
                        DrawShape(dl, *shape_uptr, ImGui::GetWindowPos());
                        if (g_ShowLayoutDebugLines && shape_uptr->isLayoutContainer && shape_uptr->layoutManager)
                        {
                            ImDrawList* foregroundDl = ImGui::GetForegroundDrawList();
                            if (dynamic_cast<GridLayout*>(shape_uptr->layoutManager.get())) {
                                DrawGridLayoutDebug(*shape_uptr, foregroundDl);
                            }
                            else if (dynamic_cast<FlexLayout*>(shape_uptr->layoutManager.get())) {
                                DrawFlexLayoutDebug(*shape_uptr, foregroundDl);
                            }
                            DrawLayoutItemBoundsDebug(*shape_uptr, foregroundDl);
                        }
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
                        for (auto& shape_uptr : layer.shapes)
                        {
                            if (shape_uptr && shape_uptr->useGlass && !shape_uptr->visible)
                                shape_uptr->visible = true;
                        }
                    }
                }
            }
            shouldCaptureScene = false;
        }
    }

    std::string SanitizeVariableName(const std::string& name) {
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

    std::string escapeNewlines(const std::string& input) {
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

    void CopyToClipboard(const std::string& text) {
        ImGui::SetClipboardText(text.c_str());
    }

    std::string GenerateButtonAnimationCode(const ButtonAnimation& anim)
    {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(6);
        ss << "ShapeBuilder().createAnimation()\n";
        ss << "    .setName(\"" << anim.name << "\")\n";
        ss << "    .setDuration(" << anim.duration << "f)\n";
        ss << "    .setSpeed(" << anim.speed << "f)\n";
        ss << "    .setAnimationTargetPosition(ImVec2(" << anim.animationTargetPosition.x << "f, " << anim.animationTargetPosition.y << "f))\n";
        ss << "    .setAnimationTargetSize(ImVec2(" << anim.animationTargetSize.x << "f, " << anim.animationTargetSize.y << "f))\n";
        ss << "    .setTransformRotation(" << anim.transformRotation << "f)\n";
        ss << "    .setRepeatCount(" << anim.repeatCount << ")\n";
        ss << "    .setPlaybackOrder(PlaybackOrder::" << (anim.playbackOrder == PlaybackOrder::Sirali ? "Sirali" : "HemenArkasina") << ")\n";
        ss << "    .setInterpolationMethod(ButtonAnimation::InterpolationMethod::" << (anim.interpolationMethod == ButtonAnimation::InterpolationMethod::Linear ? "Linear" : "EaseInOut") << ")\n";
        ss << "    .setTriggerMode(ButtonAnimation::TriggerMode::" << (anim.triggerMode == ButtonAnimation::TriggerMode::OnClick ? "OnClick" : "OnHover") << ")\n";
        ss << "    .setBehavior(ButtonAnimation::AnimationBehavior::";
        switch (anim.behavior) {
        case ButtonAnimation::AnimationBehavior::PlayOnceAndStay: ss << "PlayOnceAndStay"; break;
        case ButtonAnimation::AnimationBehavior::PlayOnceAndReverse: ss << "PlayOnceAndReverse"; break;
        case ButtonAnimation::AnimationBehavior::Toggle: ss << "Toggle"; break;
        case ButtonAnimation::AnimationBehavior::PlayWhileHoldingAndReverseOnRelease: ss << "PlayWhileHoldingAndReverseOnRelease"; break;
        case ButtonAnimation::AnimationBehavior::PlayWhileHoldingAndStay: ss << "PlayWhileHoldingAndStay"; break;
        }
        ss << ")\n";
        ss << "    .build()";
        return ss.str();
    }

    std::string GenerateShapeKeyCode(const ShapeKey& key)
    {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(6);
        ss << "ShapeBuilder::createShapeKey()\n";
        ss << "    .setName(\"" << key.name << "\")\n";
        ss << "    .setType(ShapeKeyType::";
        switch (key.type) {
        case ShapeKeyType::SizeX: ss << "SizeX"; break;
        case ShapeKeyType::SizeY: ss << "SizeY"; break;
        case ShapeKeyType::PositionX: ss << "PositionX"; break;
        case ShapeKeyType::PositionY: ss << "PositionY"; break;
        case ShapeKeyType::Rotation: ss << "Rotation"; break;
        }
        ss << ")\n";
        ss << "    .setStartWindowSize(ImVec2(" << key.startWindowSize.x << "f, " << key.startWindowSize.y << "f))\n";
        ss << "    .setEndWindowSize(ImVec2(" << key.endWindowSize.x << "f, " << key.endWindowSize.y << "f))\n";

        if (key.type == ShapeKeyType::Rotation) {
            ss << "    .setTargetRotation(" << key.targetRotation << "f)\n";
            ss << "    .setRotationOffset(" << key.rotationOffset << "f)\n";
        }
        else {
            ss << "    .setTargetValue(ImVec2(" << key.targetValue.x << "f, " << key.targetValue.y << "f))\n";
            ss << "    .setOffset(ImVec2(" << key.offset.x << "f, " << key.offset.y << "f))\n";
        }
        ss << "    .setValue(" << key.value << "f)\n";
        ss << "    .build()";
        return ss.str();
    }

    std::string GenerateSingleShapeCode(const DesignManager::ShapeItem& shape)
    {
        using namespace DesignManager;
        std::stringstream code;
        code << std::fixed << std::setprecision(6);

        auto writeColorLine = [&](const std::string& methodName, const ImVec4& color) {
            code << "    ." << methodName << "(ImVec4("
                << color.x << "f, " << color.y << "f, " << color.z << "f, " << color.w << "f))\n";
            };

        auto writeVec2Line = [&](const std::string& methodName, const ImVec2& vec) {
            code << "    ." << methodName << "(ImVec2(" << vec.x << "f, " << vec.y << "f))\n";
            };

        auto writeFloatLine = [&](const std::string& methodName, float val) {
            code << "    ." << methodName << "(" << val << "f)\n";
            };

        auto writeBoolLine = [&](const std::string& methodName, bool val) {
            code << "    ." << methodName << "(" << (val ? "true" : "false") << ")\n";
            };

        auto writeIntLine = [&](const std::string& methodName, int val) {
            code << "    ." << methodName << "(" << val << ")\n";
            };

        code << "ShapeBuilder()\n";
        code << "    .setId(" << shape.id << ")\n";
        code << "    .setName(\"" << shape.name << "\")\n";
        code << "    .setOwnerWindow(\"" << shape.ownerWindow << "\")\n";
        writeIntLine("setGroupId", shape.groupId);
        writeVec2Line("setBasePosition", shape.basePosition);
        writeVec2Line("setBaseSize", shape.baseSize);
        writeFloatLine("setBaseRotation", shape.baseRotation);
        writeVec2Line("setPosition", shape.position);
        writeVec2Line("setSize", shape.size);
        writeFloatLine("setRotation", shape.rotation);
        code << "    .setAnchorMode(DesignManager::ShapeItem::AnchorMode::";
        switch (shape.anchorMode) {
        case ShapeItem::AnchorMode::None: code << "None"; break;
        case ShapeItem::AnchorMode::TopLeft: code << "TopLeft"; break;
        case ShapeItem::AnchorMode::Top: code << "Top"; break;
        case ShapeItem::AnchorMode::TopRight: code << "TopRight"; break;
        case ShapeItem::AnchorMode::Left: code << "Left"; break;
        case ShapeItem::AnchorMode::Center: code << "Center"; break;
        case ShapeItem::AnchorMode::Right: code << "Right"; break;
        case ShapeItem::AnchorMode::BottomLeft: code << "BottomLeft"; break;
        case ShapeItem::AnchorMode::Bottom: code << "Bottom"; break;
        case ShapeItem::AnchorMode::BottomRight: code << "BottomRight"; break;
        }
        code << ")\n";
        writeVec2Line("setAnchorMargin", shape.anchorMargin);
        writeBoolLine("setUsePercentagePos", shape.usePercentagePos);
        writeVec2Line("setPercentagePos", shape.percentagePos);
        writeBoolLine("setUsePercentageSize", shape.usePercentageSize);
        writeVec2Line("setPercentageSize", shape.percentageSize);
        writeVec2Line("setMinSize", shape.minSize);
        writeVec2Line("setMaxSize", shape.maxSize);
        writeFloatLine("setCornerRadius", shape.cornerRadius);
        writeFloatLine("setBorderThickness", shape.borderThickness);
        writeColorLine("setFillColor", shape.fillColor);
        writeColorLine("setBorderColor", shape.borderColor);
        writeBoolLine("setUsePerSideBorderColors", shape.usePerSideBorderColors); 
        if (shape.usePerSideBorderColors) { 
            code << "    .setBorderSidesColor(ImVec4(" << shape.borderSideColors[0].x << "f, " << shape.borderSideColors[0].y << "f, " << shape.borderSideColors[0].z << "f, " << shape.borderSideColors[0].w << "f), "
                << "ImVec4(" << shape.borderSideColors[1].x << "f, " << shape.borderSideColors[1].y << "f, " << shape.borderSideColors[1].z << "f, " << shape.borderSideColors[1].w << "f), "
                << "ImVec4(" << shape.borderSideColors[2].x << "f, " << shape.borderSideColors[2].y << "f, " << shape.borderSideColors[2].z << "f, " << shape.borderSideColors[2].w << "f), "
                << "ImVec4(" << shape.borderSideColors[3].x << "f, " << shape.borderSideColors[3].y << "f, " << shape.borderSideColors[3].z << "f, " << shape.borderSideColors[3].w << "f))\n";
        } 
        writeBoolLine("setUsePerSideBorderThicknesses", shape.usePerSideBorderThicknesses); 
        if (shape.usePerSideBorderThicknesses) { 
            code << "    .setBorderSidesThickness(" << shape.borderSideThicknesses[0] << "f, "
                << shape.borderSideThicknesses[1] << "f, "
                << shape.borderSideThicknesses[2] << "f, "
                << shape.borderSideThicknesses[3] << "f)\n";
        } 
        writeColorLine("setShadowColor", shape.shadowColor);
        code << "    .setShadowSpread(ImVec4(" << shape.shadowSpread.x << "f, " << shape.shadowSpread.y << "f, " << shape.shadowSpread.z << "f, " << shape.shadowSpread.w << "f))\n";
        writeVec2Line("setShadowOffset", shape.shadowOffset);
        writeBoolLine("setShadowUseCornerRadius", shape.shadowUseCornerRadius);
        writeBoolLine("setShadowInset", shape.shadowInset);
        writeFloatLine("setShadowRotation", shape.shadowRotation);
        writeFloatLine("setBlurAmount", shape.blurAmount);
        writeBoolLine("setVisible", shape.visible);
        writeBoolLine("setLocked", shape.locked);
        writeBoolLine("setUseGradient", shape.useGradient);
        writeFloatLine("setGradientRotation", shape.gradientRotation);
        code << "    .setGradientInterpolation(DesignManager::ShapeItem::GradientInterpolation::";
        switch (shape.gradientInterpolation) {
        case ShapeItem::GradientInterpolation::Linear: code << "Linear"; break;
        case ShapeItem::GradientInterpolation::Ease: code << "Ease"; break;
        case ShapeItem::GradientInterpolation::Constant: code << "Constant"; break;
        case ShapeItem::GradientInterpolation::Cardinal: code << "Cardinal"; break;
        case ShapeItem::GradientInterpolation::BSpline: code << "BSpline"; break;
        }
        code << ")\n";
        code << "    .setColorRamp({\n";
        std::vector<std::pair<float, ImVec4>> sortedRamp = shape.colorRamp;
        std::sort(sortedRamp.begin(), sortedRamp.end(), [](const auto& a, const auto& b) { return a.first < b.first; });
        for (const auto& ramp : sortedRamp) {
            code << "        {" << ramp.first << "f, ImVec4(" << ramp.second.x << "f, " << ramp.second.y << "f, " << ramp.second.z << "f, " << ramp.second.w << "f)},\n";
        }
        code << "    })\n";
        writeBoolLine("setUseGlass", shape.useGlass);
        writeFloatLine("setGlassBlur", shape.glassBlur);
        writeFloatLine("setGlassAlpha", shape.glassAlpha);
        writeColorLine("setGlassColor", shape.glassColor);
        writeIntLine("setZOrder", shape.zOrder);
        writeBoolLine("setIsChildWindow", shape.isChildWindow);
        writeBoolLine("setChildWindowSync", shape.childWindowSync);
        writeBoolLine("setToggleChildWindow", shape.toggleChildWindow);
        code << "    .setToggleBehavior(DesignManager::ChildWindowToggleBehavior::";
        switch (shape.toggleBehavior) {
        case ChildWindowToggleBehavior::WindowOnly: code << "WindowOnly"; break;
        case ChildWindowToggleBehavior::ShapeAndWindow: code << "ShapeAndWindow"; break;
        }
        code << ")\n";
        writeIntLine("setChildWindowGroupId", shape.childWindowGroupId);
        writeIntLine("setTargetShapeID", shape.targetShapeID);
        writeIntLine("setTriggerGroupID", shape.triggerGroupID);
        writeBoolLine("setIsImGuiContainer", shape.isImGuiContainer);
        writeBoolLine("setIsButton", shape.isButton);
        code << "    .setButtonBehavior(DesignManager::ShapeItem::ButtonBehavior::";
        switch (shape.buttonBehavior) {
        case ShapeItem::ButtonBehavior::SingleClick: code << "SingleClick"; break;
        case ShapeItem::ButtonBehavior::Toggle: code << "Toggle"; break;
        case ShapeItem::ButtonBehavior::Hold: code << "Hold"; break;
        }
        code << ")\n";
        writeBoolLine("setUseOnClick", shape.useOnClick);
        writeColorLine("setHoverColor", shape.hoverColor);
        writeColorLine("setClickedColor", shape.clickedColor);
        writeVec2Line("setToggledStatePositionOffset", shape.toggledStatePositionOffset);
        writeVec2Line("setToggledStateSizeOffset", shape.toggledStateSizeOffset);
        writeFloatLine("setToggledStateRotationOffset", shape.toggledStateRotationOffset);
        writeBoolLine("setHasText", shape.hasText);
        code << "    .setText(\"" << escapeNewlines(shape.text) << "\")\n";
        writeColorLine("setTextColor", shape.textColor);
        writeFloatLine("setTextSize", shape.textSize);
        writeIntLine("setTextFont", shape.textFont);
        writeVec2Line("setTextPosition", shape.textPosition);
        writeFloatLine("setTextRotation", shape.textRotation);
        writeIntLine("setTextAlignment", shape.textAlignment);
        writeBoolLine("setDynamicTextSize", shape.dynamicTextSize);
        writeFloatLine("setBaseTextSize", shape.baseTextSize);
        writeFloatLine("setMinTextSize", shape.minTextSize);
        writeFloatLine("setMaxTextSize", shape.maxTextSize);
        writeBoolLine("setUpdateAnimBaseOnResize", shape.updateAnimBaseOnResize);
        writeBoolLine("setHasEmbeddedImage", shape.hasEmbeddedImage);
        writeIntLine("setEmbeddedImageIndex", shape.embeddedImageIndex);
        writeBoolLine("setAllowItemOverlap", shape.allowItemOverlap);
        writeBoolLine("setForceOverlap", shape.forceOverlap);
        writeBoolLine("setBlockUnderlying", shape.blockUnderlying);
        code << "    .setType(ShapeType::" << (shape.type == ShapeType::Rectangle ? "Rectangle" : "Circle") << ")\n";
        for (const auto& anim : shape.onClickAnimations) {
            code << "    .addOnClickAnimation(" << GenerateButtonAnimationCode(anim) << ")\n";
        }
        for (const auto& key : shape.shapeKeys) {
            code << "    .addShapeKey(" << GenerateShapeKeyCode(key) << ")\n";
        }
        code << "    .setPositioningMode(PositioningMode::" << (shape.positioningMode == PositioningMode::Relative ? "Relative" : "Absolute") << ")\n";
        for (const auto& constraint : shape.constraints) {
            code << "    .addConstraint({ DesignManager::ConstraintType::";
            switch (constraint.type) {
            case ConstraintType::LeftDistance: code << "LeftDistance"; break;
            case ConstraintType::RightDistance: code << "RightDistance"; break;
            case ConstraintType::TopDistance: code << "TopDistance"; break;
            case ConstraintType::BottomDistance: code << "BottomDistance"; break;
            case ConstraintType::CenterXAlignment: code << "CenterXAlignment"; break;
            case ConstraintType::CenterYAlignment: code << "CenterYAlignment"; break;
            case ConstraintType::WidthFixed: code << "WidthFixed"; break;
            case ConstraintType::HeightFixed: code << "HeightFixed"; break;
            case ConstraintType::WidthPercentage: code << "WidthPercentage"; break;
            case ConstraintType::HeightPercentage: code << "HeightPercentage"; break;
            case ConstraintType::AspectRatio: code << "AspectRatio"; break;
            }
            code << ", " << constraint.value << "f })\n";
        }
        writeFloatLine("setFlexGrow", shape.flexGrow);
        writeFloatLine("setFlexShrink", shape.flexShrink);
        code << "    .setFlexBasisMode(DesignManager::ShapeItem::FlexBasisMode::";
        switch (shape.flexBasisMode) {
        case ShapeItem::FlexBasisMode::Auto: code << "Auto"; break;
        case ShapeItem::FlexBasisMode::Pixels: code << "Pixels"; break;
        case ShapeItem::FlexBasisMode::Content: code << "Content"; break;
        case ShapeItem::FlexBasisMode::Percentage: code << "Percentage"; break;
        }
        code << ")\n";
        writeFloatLine("setFlexBasisPixels", shape.flexBasisPixels);
        code << "    .setAlignSelf(DesignManager::AlignSelf::";
        switch (shape.alignSelf) {
        case AlignSelf::Auto: code << "Auto"; break;
        case AlignSelf::Stretch: code << "Stretch"; break;
        case AlignSelf::FlexStart: code << "FlexStart"; break;
        case AlignSelf::FlexEnd: code << "FlexEnd"; break;
        case AlignSelf::Center: code << "Center"; break;
        case AlignSelf::Baseline: code << "Baseline"; break;
        }
        code << ")\n";
        writeIntLine("setOrder", shape.order);
        writeIntLine("setGridColumnStart", shape.gridColumnStart);
        writeIntLine("setGridColumnEnd", shape.gridColumnEnd);
        writeIntLine("setGridRowStart", shape.gridRowStart);
        writeIntLine("setGridRowEnd", shape.gridRowEnd);
        code << "    .setJustifySelf(DesignManager::GridAxisAlignment::";
        switch (shape.justifySelf) {
        case GridAxisAlignment::Stretch: code << "Stretch"; break;
        case GridAxisAlignment::Start: code << "Start"; break;
        case GridAxisAlignment::End: code << "End"; break;
        case GridAxisAlignment::Center: code << "Center"; break;
        }
        code << ")\n";
        code << "    .setAlignSelfGrid(DesignManager::GridAxisAlignment::";
        switch (shape.alignSelfGrid) {
        case GridAxisAlignment::Stretch: code << "Stretch"; break;
        case GridAxisAlignment::Start: code << "Start"; break;
        case GridAxisAlignment::End: code << "End"; break;
        case GridAxisAlignment::Center: code << "Center"; break;
        }
        code << ")\n";
        writeBoolLine("setIsLayoutContainer", shape.isLayoutContainer);
        writeFloatLine("setStretchFactor", shape.stretchFactor);
        code << "    .setHorizontalAlignment(DesignManager::HAlignment::";
        switch (shape.horizontalAlignment) {
        case HAlignment::Fill: code << "Fill"; break;
        case HAlignment::Left: code << "Left"; break;
        case HAlignment::Center: code << "Center"; break;
        case HAlignment::Right: code << "Right"; break;
        }
        code << ")\n";
        code << "    .setVerticalAlignment(DesignManager::VAlignment::";
        switch (shape.verticalAlignment) {
        case VAlignment::Fill: code << "Fill"; break;
        case VAlignment::Top: code << "Top"; break;
        case VAlignment::Center: code << "Center"; break;
        case VAlignment::Bottom: code << "Bottom"; break;
        }
        code << ")\n";
        code << "    .setBoxSizing(DesignManager::ShapeItem::BoxSizing::"; 
        switch (shape.boxSizing) { 
        case ShapeItem::BoxSizing::BorderBox:  code << "BorderBox"; break; 
        case ShapeItem::BoxSizing::ContentBox: code << "ContentBox"; break; 
        case ShapeItem::BoxSizing::StrokeBox:  code << "StrokeBox"; break; 
        default: code << "StrokeBox"; break; 
        } 
        code << ")\n"; 
        code << "    .setPadding(ImVec4(" << shape.padding.x << "f, " << shape.padding.y << "f, " << shape.padding.z << "f, " << shape.padding.w << "f))\n";
        code << "    .setMargin(ImVec4(" << shape.margin.x << "f, " << shape.margin.y << "f, " << shape.margin.z << "f, " << shape.margin.w << "f))\n";
        writeBoolLine("setIsPolygon", shape.isPolygon); 
        if (shape.isPolygon && !shape.polygonVertices.empty()) { 
            code << "    .setPolygonVertices({\n"; 
            for (const auto& v : shape.polygonVertices) { 
                code << "        ImVec2(" << v.x << "f, " << v.y << "f),\n"; 
            } 
            code << "    })\n"; 
        } 
        code << "    .build()";
        return code.str();
    }

    std::string GenerateChildWindowMappingsCode() {
        std::string code;
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

    std::set<int> GetAllShapeIDs() {
        std::set<int> ids;
        if (!g_windowsMap.empty()) {
            for (const auto& pair : g_windowsMap) {
                const auto& winData = pair.second;
                for (const auto& layer : winData.layers) {
                    for (const auto& shape_uptr : layer.shapes) {
                        if (shape_uptr) ids.insert(shape_uptr->id);
                    }
                }
            }
        }
        return ids;
    }

    ShapeItem* FindShapeByID_Internal(int id) {
        if (!g_windowsMap.empty()) {
            for (auto& pair : g_windowsMap) {
                auto& winData = pair.second;
                for (auto& layer : winData.layers) {
                    for (auto& shape_uptr : layer.shapes) {
                        if (shape_uptr && shape_uptr->id == id) {
                            return shape_uptr.get();
                        }
                    }
                }
            }
        }
        return nullptr;
    }

    std::string GenerateComponentDefinitionCode(const std::string& componentName, const ComponentDefinition& compDef)
    {
        using namespace DesignManager;
        std::string safeCompName = SanitizeVariableName(componentName);
        std::stringstream code;

        code << "inline std::vector<DesignManager::ShapeItem> Create" << safeCompName << "Instance(\n";
        code << "    int instanceRootId, \n";
        code << "    const std::string& instanceRootName, \n";
        code << "    const ImVec2& positionOffset, \n";
        code << "    const std::string& ownerWindow, \n";
        code << "    int& nextAvailableId) \n";
        code << "{\n";
        code << "    std::vector<DesignManager::ShapeItem> createdShapes;\n";
        code << "    std::map<int, int> originalIdToNewId;\n";
        code << "    std::map<int, int> newIdToOriginalParentId;\n";
        code << "    std::vector<DesignManager::ShapeItem> tempStorage;\n";
        code << "    std::set<int> existingInstanceIDs; \n";
        code << "    tempStorage.reserve(" << compDef.shapeTemplates.size() << ");\n\n";

        code << "    \n";
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
            finalShapeCodeSS << "        .setName(instanceRootName + \"_" << SanitizeVariableName(shapeTemplate.item.name) << "\") \n";
            finalShapeCodeSS << "        .setPosition(ImVec2(positionOffset.x + " << shapeTemplate.item.position.x << "f, positionOffset.y + " << shapeTemplate.item.position.y << "f)) \n";
            finalShapeCodeSS << "        .setBasePosition(ImVec2(positionOffset.x + " << shapeTemplate.item.basePosition.x << "f, positionOffset.y + " << shapeTemplate.item.basePosition.y << "f)) \n";
            finalShapeCodeSS << "        .setOwnerWindow(ownerWindow)\n";
            finalShapeCodeSS << "        .setComponentSource(\"\") \n";
            finalShapeCodeSS << "        .build()";

            code << "    tempStorage.push_back(" << finalShapeCodeSS.str() << ");\n\n";
        }
        code << "\n";

        code << "    \n";
        code << "    std::map<int, DesignManager::ShapeItem*> newIdToShapePtrMap;\n";
        code << "    for(auto& shape : tempStorage) { newIdToShapePtrMap[shape.id] = &shape; }\n\n";
        code << "    for(auto& shape : tempStorage) {\n";
        code << "        int currentNewId = shape.id;\n";
        code << "        if (newIdToOriginalParentId.count(currentNewId)) {\n";
        code << "            int originalParentId = newIdToOriginalParentId.at(currentNewId);\n";
        code << "            if (originalParentId != -1 && originalIdToNewId.count(originalParentId)) { \n";
        code << "                int newParentId = originalIdToNewId.at(originalParentId);\n";
        code << "                if (newIdToShapePtrMap.count(newParentId)) { \n";
        code << "                    shape.parent = newIdToShapePtrMap.at(newParentId); \n";
        code << "                    newIdToShapePtrMap.at(newParentId)->children.push_back(&shape); \n";
        code << "                }\n";
        code << "            }\n";
        code << "        }\n";
        code << "    }\n\n";

        code << "    createdShapes = std::move(tempStorage);\n";
        code << "    return createdShapes;\n";
        code << "}\n\n";
        return code.str();
    }

    std::string GenerateAllComponentDefinitionsCode() {
        const auto& currentComponentDefinitions = DesignManager::g_componentDefinitions;
        std::stringstream allCompCode;
        allCompCode << "\n\n";
        allCompCode << "#pragma once\n\n";
        allCompCode << "#include \"design_manager.h\" \n";
        allCompCode << "#include <vector>\n";
        allCompCode << "#include <map>\n";
        allCompCode << "#include <set>\n\n";
        allCompCode << "namespace DesignManager {\n\n";

        for (const auto& pair : currentComponentDefinitions) {
            const std::string& name = pair.first;
            const ComponentDefinition& compDef = pair.second;
            allCompCode << GenerateComponentDefinitionCode(name, compDef);
        }

        allCompCode << "} \n";
        allCompCode << "\n";
        return allCompCode.str();
    }
    std::string GenerateLengthUnitCode(const LengthUnit& unit) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(6);
        ss << "DesignManager::LengthUnit{" << unit.value << "f, DesignManager::LengthUnit::Unit::" << (unit.unit == LengthUnit::Unit::Px ? "Px" : "Percent") << "}";
        return ss.str();
    }

    std::string GenerateGridTrackSizeCode(const GridTrackSize& track) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(6);
        ss << "[]() { DesignManager::GridTrackSize track;\n";
        ss << "    track.mode = DesignManager::GridTrackSize::Mode::";
        switch (track.mode) {
        case GridTrackSize::Mode::Fixed: ss << "Fixed"; break;
        case GridTrackSize::Mode::Percentage: ss << "Percentage"; break;
        case GridTrackSize::Mode::Fraction: ss << "Fraction"; break;
        case GridTrackSize::Mode::Auto: ss << "Auto"; break;
        case GridTrackSize::Mode::MinMax: ss << "MinMax"; break;
        }
        ss << ";\n";
        if (track.mode != GridTrackSize::Mode::Auto && track.mode != GridTrackSize::Mode::MinMax) {
            ss << "    track.value = " << track.value << "f;\n";
        }
        if (track.mode == GridTrackSize::Mode::MinMax) {
            ss << "    track.minVal = {DesignManager::GridTrackSize::TrackSizeValue::Unit::";
        switch (track.minVal.unit) { case GridTrackSize::TrackSizeValue::Unit::Px: ss << "Px"; break; case GridTrackSize::TrackSizeValue::Unit::Percent: ss << "Percent"; break; case GridTrackSize::TrackSizeValue::Unit::Auto: default: ss << "Auto"; break; }
                                                                                 ss << ", " << track.minVal.value << "f};\n";
                                                                                 ss << "    track.maxVal = {DesignManager::GridTrackSize::TrackSizeValue::Unit::";
        switch (track.maxVal.unit) { case GridTrackSize::TrackSizeValue::Unit::Px: ss << "Px"; break; case GridTrackSize::TrackSizeValue::Unit::Percent: ss << "Percent"; break; case GridTrackSize::TrackSizeValue::Unit::Fr: ss << "Fr"; break; case GridTrackSize::TrackSizeValue::Unit::Auto: default: ss << "Auto"; break; }
                                                                                 ss << ", " << track.maxVal.value << "f};\n";
        }
        ss << "    return track; }()";
        return ss.str();
    }

    std::string GenerateCodeForWindow(const std::string& windowName)
    {
        using namespace DesignManager;

        if (g_windowsMap.find(windowName) == g_windowsMap.end()) {
            return "\n";
        }

        const auto& winData = g_windowsMap.at(windowName);
        std::string safeWindowName = SanitizeVariableName(windowName);
        std::stringstream code;
        code << std::fixed << std::setprecision(6);

        std::map<int, std::string> shapeIdToVarName;
        std::vector<std::pair<int, int>> parentingLinks;
        std::map<std::string, std::vector<int>> layerVarToShapeIds;

        code << "    \n"; 
        code << "    if (DesignManager::g_windowsMap.count(\"" << windowName << "\")) {\n";
        code << "        auto& targetWinData = DesignManager::g_windowsMap.at(\"" << windowName << "\");\n";
        code << "        for (auto& layer : targetWinData.layers) {\n";
        code << "            for (auto& shape_uptr : layer.shapes) {\n";
        code << "                 if (shape_uptr && shape_uptr->layoutManager) {\n";
        code << "                     shape_uptr->layoutManager.reset();\n";
        code << "                 }\n";
        code << "            }\n";
        code << "        }\n";
        code << "        targetWinData.layers.clear();\n";
        code << "    }\n\n";

        code << "    \n"; 
        for (const auto& layer : winData.layers) {
            std::string layerVar = SanitizeVariableName(layer.name);
            if (layerVarToShapeIds.find(layerVar) == layerVarToShapeIds.end()) {
                layerVarToShapeIds[layerVar] = {};
            }

            for (const auto& shape_uptr : layer.shapes) {
                if (!shape_uptr) continue;
                const ShapeItem& shape = *shape_uptr;

                if (shapeIdToVarName.find(shape.id) == shapeIdToVarName.end()) {
                    std::string shapeVar = SanitizeVariableName(shape.name);
                    shapeVar += "_" + std::to_string(shape.id);
                    shapeIdToVarName[shape.id] = shapeVar;
                    layerVarToShapeIds[layerVar].push_back(shape.id);

                    std::string builderCode = GenerateSingleShapeCode(shape);
                    code << "    auto " << shapeVar << "_shape_item_temp = " << builderCode << ";\n";
                    code << "    std::unique_ptr<DesignManager::ShapeItem> " << shapeVar << " = std::make_unique<DesignManager::ShapeItem>(" << shapeVar << "_shape_item_temp);\n";


                    if (shape.isLayoutContainer && shape.layoutManager) {
                        code << "    {\n"; 
                        if (auto* flex = dynamic_cast<FlexLayout*>(shape.layoutManager.get())) {
                            code << "        auto layoutMgr = std::make_unique<DesignManager::FlexLayout>();\n";
                            code << "        layoutMgr->direction = DesignManager::FlexDirection::";
                            switch (flex->direction) {
                            case FlexDirection::Row: code << "Row"; break;
                            case FlexDirection::RowReverse: code << "RowReverse"; break;
                            case FlexDirection::Column: code << "Column"; break;
                            case FlexDirection::ColumnReverse: code << "ColumnReverse"; break;
                            default: code << "Row"; break;
                            }
                            code << ";\n";
                            code << "        layoutMgr->wrap = DesignManager::FlexWrap::";
                            switch (flex->wrap) {
                            case FlexWrap::NoWrap: code << "NoWrap"; break;
                            case FlexWrap::Wrap: code << "Wrap"; break;
                            case FlexWrap::WrapReverse: code << "WrapReverse"; break;
                            default: code << "NoWrap"; break;
                            }
                            code << ";\n";
                            code << "        layoutMgr->justifyContent = DesignManager::JustifyContent::";
                            switch (flex->justifyContent) {
                            case JustifyContent::FlexStart: code << "FlexStart"; break;
                            case JustifyContent::FlexEnd: code << "FlexEnd"; break;
                            case JustifyContent::Center: code << "Center"; break;
                            case JustifyContent::SpaceBetween: code << "SpaceBetween"; break;
                            case JustifyContent::SpaceAround: code << "SpaceAround"; break;
                            case JustifyContent::SpaceEvenly: code << "SpaceEvenly"; break;
                            default: code << "FlexStart"; break;
                            }
                            code << ";\n";
                            code << "        layoutMgr->alignItems = DesignManager::AlignItems::";
                            switch (flex->alignItems) {
                            case AlignItems::Stretch: code << "Stretch"; break;
                            case AlignItems::FlexStart: code << "FlexStart"; break;
                            case AlignItems::FlexEnd: code << "FlexEnd"; break;
                            case AlignItems::Center: code << "Center"; break;
                            case AlignItems::Baseline: code << "Baseline"; break;
                            default: code << "Stretch"; break;
                            }
                            code << ";\n";
                            code << "        layoutMgr->alignContent = DesignManager::AlignContent::";
                            switch (flex->alignContent) {
                            case AlignContent::Stretch: code << "Stretch"; break;
                            case AlignContent::FlexStart: code << "FlexStart"; break;
                            case AlignContent::FlexEnd: code << "FlexEnd"; break;
                            case AlignContent::Center: code << "Center"; break;
                            case AlignContent::SpaceBetween: code << "SpaceBetween"; break;
                            case AlignContent::SpaceAround: code << "SpaceAround"; break;
                            case AlignContent::SpaceEvenly: code << "SpaceEvenly"; break;
                            default: code << "Stretch"; break;
                            }
                            code << ";\n";
                            code << "        layoutMgr->gap = " << flex->gap << "f;\n";
                            code << "        " << shapeVar << "->layoutManager = std::move(layoutMgr);\n";
                        }
                        else if (auto* grid = dynamic_cast<GridLayout*>(shape.layoutManager.get())) {
                            code << "        auto layoutMgr = std::make_unique<DesignManager::GridLayout>();\n";
                            code << "        layoutMgr->templateColumns = {\n";
                            for (size_t i = 0; i < grid->templateColumns.size(); ++i) {
                                code << "            " << GenerateGridTrackSizeCode(grid->templateColumns[i]) << (i == grid->templateColumns.size() - 1 ? "\n" : ",\n");
                            }
                            code << "        };\n";
                            code << "        layoutMgr->templateRows = {\n";
                            for (size_t i = 0; i < grid->templateRows.size(); ++i) {
                                code << "            " << GenerateGridTrackSizeCode(grid->templateRows[i]) << (i == grid->templateRows.size() - 1 ? "\n" : ",\n");
                            }
                            code << "        };\n";
                            code << "        layoutMgr->rowGap = " << GenerateLengthUnitCode(grid->rowGap) << ";\n";
                            code << "        layoutMgr->columnGap = " << GenerateLengthUnitCode(grid->columnGap) << ";\n";
                            code << "        layoutMgr->autoFlow = DesignManager::GridAutoFlow::";
                            switch (grid->autoFlow) {
                            case GridAutoFlow::Row: code << "Row"; break;
                            case GridAutoFlow::Column: code << "Column"; break;
                            case GridAutoFlow::RowDense: code << "RowDense"; break;
                            case GridAutoFlow::ColumnDense: code << "ColumnDense"; break;
                            default: code << "Row"; break;
                            }
                            code << ";\n";
                            code << "        layoutMgr->defaultCellContentJustify = DesignManager::GridAxisAlignment::";
                            switch (grid->defaultCellContentJustify) {
                            case GridAxisAlignment::Start: code << "Start"; break;
                            case GridAxisAlignment::End: code << "End"; break;
                            case GridAxisAlignment::Center: code << "Center"; break;
                            case GridAxisAlignment::Stretch: default: code << "Stretch"; break;
                            }
                            code << ";\n";
                            code << "        layoutMgr->defaultCellContentAlign = DesignManager::GridAxisAlignment::";
                            switch (grid->defaultCellContentAlign) {
                            case GridAxisAlignment::Start: code << "Start"; break;
                            case GridAxisAlignment::End: code << "End"; break;
                            case GridAxisAlignment::Center: code << "Center"; break;
                            case GridAxisAlignment::Stretch: default: code << "Stretch"; break;
                            }
                            code << ";\n";
                            code << "        layoutMgr->implicitTrackColSize = " << GenerateLengthUnitCode(grid->implicitTrackColSize) << ";\n";
                            code << "        layoutMgr->implicitTrackRowSize = " << GenerateLengthUnitCode(grid->implicitTrackRowSize) << ";\n";
                            code << "        layoutMgr->justifyContent = DesignManager::JustifyContent::";
                            switch (grid->justifyContent) {
                            case JustifyContent::FlexStart: code << "FlexStart"; break;
                            case JustifyContent::FlexEnd: code << "FlexEnd"; break;
                            case JustifyContent::Center: code << "Center"; break;
                            case JustifyContent::SpaceBetween: code << "SpaceBetween"; break;
                            case JustifyContent::SpaceAround: code << "SpaceAround"; break;
                            case JustifyContent::SpaceEvenly: code << "SpaceEvenly"; break;
                            default: code << "FlexStart"; break;
                            }
                            code << ";\n";
                            code << "        layoutMgr->alignContent = DesignManager::AlignContent::";
                            switch (grid->alignContent) {
                            case AlignContent::FlexStart: code << "FlexStart"; break;
                            case AlignContent::FlexEnd: code << "FlexEnd"; break;
                            case AlignContent::Center: code << "Center"; break;
                            case AlignContent::Stretch: code << "Stretch"; break;
                            case AlignContent::SpaceBetween: code << "SpaceBetween"; break;
                            case AlignContent::SpaceAround: code << "SpaceAround"; break;
                            case AlignContent::SpaceEvenly: code << "SpaceEvenly"; break;
                            default: code << "FlexStart"; break;
                            }
                            code << ";\n";
                            code << "        " << shapeVar << "->layoutManager = std::move(layoutMgr);\n";
                        }
                        else if (auto* vert = dynamic_cast<VerticalLayout*>(shape.layoutManager.get())) {
                            code << "        auto layoutMgr = std::make_unique<DesignManager::VerticalLayout>();\n";
                            code << "        layoutMgr->spacing = " << vert->spacing << "f;\n";
                            code << "        " << shapeVar << "->layoutManager = std::move(layoutMgr);\n";
                        }
                        else if (auto* horz = dynamic_cast<HorizontalLayout*>(shape.layoutManager.get())) {
                            code << "        auto layoutMgr = std::make_unique<DesignManager::HorizontalLayout>();\n";
                            code << "        layoutMgr->spacing = " << horz->spacing << "f;\n";
                            code << "        " << shapeVar << "->layoutManager = std::move(layoutMgr);\n";
                        }
                        code << "    }\n"; 
                    }

                    if (shape.parent != nullptr) {
                        parentingLinks.push_back({ shape.id, shape.parent->id });
                    }
                }
                else {
                    layerVarToShapeIds[layerVar].push_back(shape.id);
                    if (shape.parent != nullptr) {
                        bool linkExists = false;
                        for (const auto& link : parentingLinks) {
                            if (link.first == shape.id && link.second == shape.parent->id) {
                                linkExists = true;
                                break;
                            }
                        }
                        if (!linkExists) {
                            parentingLinks.push_back({ shape.id, shape.parent->id });
                        }
                    }
                }
            }
        }
        code << "\n";

        code << "    \n"; 
        for (const auto& layer : winData.layers) {
            std::string layerVar = SanitizeVariableName(layer.name);
            code << "    DesignManager::Layer " << layerVar << "_layer_temp(\"" << layer.name << "\");\n";
            if (layer.zOrder != 0) code << "    " << layerVar << "_layer_temp.zOrder = " << layer.zOrder << ";\n";
            if (!layer.visible) code << "    " << layerVar << "_layer_temp.visible = false;\n";
            if (layer.locked) code << "    " << layerVar << "_layer_temp.locked = true;\n";

            if (layerVarToShapeIds.count(layerVar)) {
                code << "    " << layerVar << "_layer_temp.shapes.reserve(" << layerVarToShapeIds.at(layerVar).size() << ");\n";
                for (int shapeId : layerVarToShapeIds.at(layerVar)) {
                    if (shapeIdToVarName.count(shapeId)) {
                        code << "    " << layerVar << "_layer_temp.shapes.push_back(std::move(" << shapeIdToVarName.at(shapeId) << "));\n";
                    }
                }
            }
            code << "    DesignManager::g_windowsMap[\"" << windowName << "\"].layers.push_back(std::move(" << layerVar << "_layer_temp));\n\n";
        }

        if (!parentingLinks.empty()) {
            code << "    \n"; 
            code << "    {\n"; 
            code << "        std::map<int, ShapeItem*> finalShapePtrs_Generated_" << safeWindowName << ";\n";
            code << "        if (DesignManager::g_windowsMap.count(\"" << windowName << "\")) {\n";
            code << "            auto& layers_vec_Generated_" << safeWindowName << " = DesignManager::g_windowsMap.at(\"" << windowName << "\").layers;\n";
            code << "            for (auto& layer_ref_Generated_" << safeWindowName << " : layers_vec_Generated_" << safeWindowName << ") {\n";
            code << "                for (auto& shape_uptr_Generated_" << safeWindowName << " : layer_ref_Generated_" << safeWindowName << ".shapes) {\n";
            code << "                    if(shape_uptr_Generated_" << safeWindowName << ") finalShapePtrs_Generated_" << safeWindowName << "[shape_uptr_Generated_" << safeWindowName << "->id] = shape_uptr_Generated_" << safeWindowName << ".get();\n";
            code << "                }\n";
            code << "            }\n";
            code << "        }\n\n";

            for (const auto& link : parentingLinks) {
                int childId = link.first;
                int parentId = link.second;
                if (parentId != -1) {
                    code << "        if (finalShapePtrs_Generated_" << safeWindowName << ".count(" << childId << ") && finalShapePtrs_Generated_" << safeWindowName << ".count(" << parentId << ")) {\n";
                    code << "            ShapeItem* childPtr = finalShapePtrs_Generated_" << safeWindowName << "[" << childId << "];\n";
                    code << "            ShapeItem* parentPtr = finalShapePtrs_Generated_" << safeWindowName << "[" << parentId << "];\n";
                    code << "            if (childPtr && parentPtr && childPtr != parentPtr) {\n";
                    code << "                 DesignManager::SetParent(childPtr, parentPtr);\n";
                    code << "            }\n";
                    code << "        }\n";
                }
            }
            code << "    }\n"; 
            code << "\n";
        }

        if (!g_combinedChildWindowMappings.empty()) {
            code << "    \n"; 
            code << "    g_combinedChildWindowMappings.clear();\n";
            for (const auto& mapping : g_combinedChildWindowMappings) {
                code << "    g_combinedChildWindowMappings.push_back(CombinedMapping{\n";
                code << "        " << mapping.shapeId << ",\n";
                code << "        \"" << mapping.logicOp << "\",\n";
                code << "        {";
                for (size_t i = 0; i < mapping.buttonIds.size(); i++) {
                    code << std::to_string(mapping.buttonIds[i]);
                    if (i < mapping.buttonIds.size() - 1) code << ", ";
                }
                code << "},\n";
                code << "        {";
                for (size_t i = 0; i < mapping.childWindowKeys.size(); i++) {
                    code << "\"" << mapping.childWindowKeys[i] << "\"";
                    if (i < mapping.childWindowKeys.size() - 1) code << ", ";
                }
                code << "}\n";
                code << "    });\n";
            }
            code << "\n";
        }

        return code.str();
    }

    std::string GenerateCodeForSingleButton(const DesignManager::ShapeItem& buttonShape)
    {
        std::string safeName = SanitizeVariableName(buttonShape.name);
        std::string onClickFunctionName = safeName + "_OnClick";
        std::string hCode;
        hCode += "\n\n";
        hCode += "#pragma once\n\n";
        hCode += "void " + onClickFunctionName + "();\n";
        hCode += "void generated" + safeName + "();\n";
        hCode += "void Init" + safeName + "LayersOnce();\n";
        hCode += "void Render" + safeName + "Window();\n";

        std::string cppCode;
        cppCode += "\n\n";
        cppCode += "#include \"" + safeName + ".h\"\n";
        cppCode += "#include \"design_manager.h\"\n";
        cppCode += "#include <iostream>\n\n";
        cppCode += "bool show" + safeName + "Window = false;\n\n";
        cppCode += "void " + onClickFunctionName + "() {\n";
        cppCode += "    show" + safeName + "Window = !show" + safeName + "Window;\n";
        cppCode += "    std::cout << \"[" + onClickFunctionName + "] show" + safeName + "Window = \" << show" + safeName + "Window << \"\\n\";\n";
        cppCode += "}\n\n";
        cppCode += "void generated" + safeName + "() {\n";
        cppCode += "    \n";
        cppCode += "}\n\n";
        cppCode += "static bool g_" + safeName + "LayersAdded = false;\n\n";
        cppCode += "void Init" + safeName + "LayersOnce() {\n";
        cppCode += "    if (g_" + safeName + "LayersAdded)\n        return;\n\n";
        cppCode += "    g_" + safeName + "LayersAdded = true;\n\n";
        cppCode += "    if (DesignManager::g_windowsMap.find(\"" + safeName + " Window\") == DesignManager::g_windowsMap.end())\n";
        cppCode += "        DesignManager::g_windowsMap[\"" + safeName + " Window\"] = {};\n";
        cppCode += "    generated" + safeName + "();\n";
        cppCode += "}\n\n";
        cppCode += "void Render" + safeName + "Window() {\n";
        cppCode += "    if (!show" + safeName + "Window) return;\n";
        cppCode += "    ImGui::Begin(\"" + safeName + " Window\", &show" + safeName + "Window);\n";
        cppCode += "    Init" + safeName + "LayersOnce();\n";
        cppCode += "    \n";
        cppCode += "    ImGui::End();\n";
        cppCode += "}\n";

        std::string fullCode =
            "\n\n" +
            hCode + "\n\n" +
            cppCode + "\n\n" +
            "\n";
        return fullCode;
    }

    void RefreshLayerIDs()
    {
        auto& windowData = g_windowsMap[DesignManager::selectedGuiWindow];
        for (int i = 0; i < static_cast<int>(windowData.layers.size()); i++)
        {
            windowData.layers[i].id = i;
        }
    }

    void EnsureMainWindowExists() {
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

    int GetUniqueLayerID() {
        static int layer_id_counter = 1000;
        return layer_id_counter++;
    }

    ShapeItem* FindShapeByIdRecursiveHelper(int shapeId, ShapeItem* currentShape) {
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

    ShapeItem* FindShapeByIdRecursive(int shapeId) {
        if (g_windowsMap.count(selectedGuiWindow)) {
            WindowData& currentWindowData = g_windowsMap.at(selectedGuiWindow);
            for (Layer& layer : currentWindowData.layers) {
                for (auto& shape_uptr : layer.shapes) {
                    if (shape_uptr && shape_uptr->parent == nullptr) {
                        ShapeItem* found = FindShapeByIdRecursiveHelper(shapeId, shape_uptr.get());
                        if (found) {
                            return found;
                        }
                    }
                }
            }
        }
        return nullptr;
    }

    ImRect GetShapeBoundingBox(const ShapeItem& s) {
        ImVec2 pos = s.position;
        ImVec2 size = s.size;
        float rot = s.rotation;
        ImVec2 center = pos + size * 0.5f;
        ImVec2 corners[4] = {
            pos,
            ImVec2(pos.x + size.x, pos.y),
            pos + size,
            ImVec2(pos.x, pos.y + size.y)
        };
        ImVec2 minBound(FLT_MAX, FLT_MAX);
        ImVec2 maxBound(-FLT_MAX, -FLT_MAX);
        for (int i = 0; i < 4; ++i) {
            ImVec2 rotatedCorner = RotateP(corners[i], center, rot);
            minBound.x = std::min(minBound.x, rotatedCorner.x);
            minBound.y = std::min(minBound.y, rotatedCorner.y);
            maxBound.x = std::max(maxBound.x, rotatedCorner.x);
            maxBound.y = std::max(maxBound.y, rotatedCorner.y);
        }
        return ImRect(minBound, maxBound);
    }

    bool IsMouseOverShape(const ImVec2& mouseCanvasPos, const ShapeItem& s) {
        ImVec2 center = s.position + s.size * 0.5f;
        ImVec2 localMousePos = RotateP(mouseCanvasPos, center, -s.rotation);
        ImVec2 shapeTopLeftLocal = s.position;
        ImVec2 shapeBottomRightLocal = s.position + s.size;
        return localMousePos.x >= shapeTopLeftLocal.x && localMousePos.x <= shapeBottomRightLocal.x &&
            localMousePos.y >= shapeTopLeftLocal.y && localMousePos.y <= shapeBottomRightLocal.y;
    }

    void ProcessCanvasInteractions() {
        if (!g_IsInEditMode) {
            if (g_InteractionState.type != InteractionType::None) {
                g_InteractionState = {};
            }
            return;
        }

        ImGuiIO& io = ImGui::GetIO();
        if (g_InteractionState.type == InteractionType::Dragging ||
            g_InteractionState.type == InteractionType::Resizing ||
            g_InteractionState.type == InteractionType::Rotating)
        {
            io.WantCaptureMouse = true;
        }
        ImVec2 mouseScreenPos = io.MousePos;
        if (g_InteractionState.type != InteractionType::None && !ImGui::IsMouseDown(0) && !ImGui::IsMouseReleased(0)) {
        }

        ShapeItem* hoveredShape = nullptr;
        InteractionHandle hoveredHandle = InteractionHandle::None;
        float minHoverDistSq = 10.0f * 10.0f;

        if (!selectedShapes.empty() && g_InteractionState.type == InteractionType::None) {
            if (selectedShapes.size() == 1) {
                ShapeItem& s = *selectedShapes[0];
                ImVec2 posScreen = s.position;
                ImVec2 size = s.size;
                float rotation = s.rotation;
                ImVec2 centerScreen = posScreen + size * 0.5f;
                ImVec2 cornersScreen[4];
                ImVec2 midPointsScreen[4];
                ImVec2 handlesScreen[8];
                InteractionHandle handleTypes[8] = {
                    InteractionHandle::TopLeft, InteractionHandle::Top, InteractionHandle::TopRight, InteractionHandle::Right,
                    InteractionHandle::BottomRight, InteractionHandle::Bottom, InteractionHandle::BottomLeft, InteractionHandle::Left
                };
                cornersScreen[0] = RotateP(posScreen, centerScreen, rotation);
                cornersScreen[1] = RotateP(posScreen + ImVec2(size.x, 0), centerScreen, rotation);
                cornersScreen[2] = RotateP(posScreen + size, centerScreen, rotation);
                cornersScreen[3] = RotateP(posScreen + ImVec2(0, size.y), centerScreen, rotation);
                midPointsScreen[0] = (cornersScreen[0] + cornersScreen[1]) * 0.5f;
                midPointsScreen[1] = (cornersScreen[1] + cornersScreen[2]) * 0.5f;
                midPointsScreen[2] = (cornersScreen[2] + cornersScreen[3]) * 0.5f;
                midPointsScreen[3] = (cornersScreen[3] + cornersScreen[0]) * 0.5f;
                handlesScreen[0] = cornersScreen[0]; handlesScreen[1] = midPointsScreen[0]; handlesScreen[2] = cornersScreen[1]; handlesScreen[3] = midPointsScreen[1];
                handlesScreen[4] = cornersScreen[2]; handlesScreen[5] = midPointsScreen[2]; handlesScreen[6] = cornersScreen[3]; handlesScreen[7] = midPointsScreen[3];
                for (int i = 0; i < 8; ++i) {
                    ImVec2 handleCenterScreen = handlesScreen[i];
                    float dx = mouseScreenPos.x - handleCenterScreen.x;
                    float dy = mouseScreenPos.y - handleCenterScreen.y;
                    float distSq = dx * dx + dy * dy;
                    if (distSq < minHoverDistSq) {
                        hoveredHandle = handleTypes[i];
                        hoveredShape = &s;
                        goto found_hover_target_overlay;
                    }
                }
            }
        }

        if (hoveredHandle == InteractionHandle::None && g_InteractionState.type == InteractionType::None) {
            if (g_windowsMap.count(selectedGuiWindow)) {
                WindowData& currentWindowData = g_windowsMap.at(selectedGuiWindow);
                for (int i = (int)currentWindowData.layers.size() - 1; i >= 0; --i) {
                    Layer& layer = currentWindowData.layers[i];
                    if (!layer.visible || layer.locked) continue;
                    std::vector<ShapeItem*> sortedLayerShapes;
                    for (auto& shape_uptr : layer.shapes) if (shape_uptr) sortedLayerShapes.push_back(shape_uptr.get());
                    std::sort(sortedLayerShapes.begin(), sortedLayerShapes.end(), [](const ShapeItem* a, const ShapeItem* b) { return a->zOrder > b->zOrder; });
                    for (ShapeItem* shapePtr : sortedLayerShapes) {
                        if (shapePtr && shapePtr->visible && !shapePtr->locked) {
                            if (IsMouseOverShape(mouseScreenPos, *shapePtr)) {
                                hoveredShape = shapePtr;
                                hoveredHandle = InteractionHandle::Body;
                                goto found_hover_target_overlay;
                            }
                        }
                    }
                }
            }
        }
    found_hover_target_overlay:;

        bool isDragging = ImGui::IsMouseDragging(0, 1.0f);
        bool isClicked = ImGui::IsMouseClicked(0);
        bool isReleased = ImGui::IsMouseReleased(0);
        bool isShiftDown = io.KeyShift;
        bool isCtrlDown = io.KeyCtrl || io.KeySuper;

        if (isClicked) {
            if (hoveredHandle != InteractionHandle::None && hoveredHandle != InteractionHandle::Body) {
                if (selectedShapes.size() == 1 && selectedShapes[0] == hoveredShape) {
                    g_InteractionState.type = (hoveredHandle == InteractionHandle::Rotate) ? InteractionType::Rotating : InteractionType::Resizing;
                    g_InteractionState.activeHandle = hoveredHandle;
                    g_InteractionState.dragStartMousePos = mouseScreenPos;
                    g_InteractionState.dragStartShapeSizes.clear();
                    g_InteractionState.dragStartShapeRotations.clear();
                    g_InteractionState.dragStartShapePositions.clear();
                    g_InteractionState.dragStartShapeSizes.push_back({ hoveredShape, hoveredShape->size });
                    g_InteractionState.dragStartShapeRotations.push_back({ hoveredShape, hoveredShape->rotation });
                    g_InteractionState.dragStartShapePositions.push_back({ hoveredShape, hoveredShape->position });
                    g_InteractionState.interactionStartShapeCenter = hoveredShape->position + hoveredShape->size * 0.5f;
                    g_InteractionState.undoStateRecorded = false;
                }
                else {
                    g_InteractionState = {};
                }
            }
            else if (hoveredShape) {
                bool alreadySelected = std::find(selectedShapes.begin(), selectedShapes.end(), hoveredShape) != selectedShapes.end();
                if (!isCtrlDown && !isShiftDown) {
                    if (!alreadySelected) {
                        selectedShapes.clear();
                        selectedShapes.push_back(hoveredShape);
                    }
                }
                else if (isCtrlDown) {
                    if (alreadySelected) {
                        selectedShapes.erase(std::remove(selectedShapes.begin(), selectedShapes.end(), hoveredShape), selectedShapes.end());
                    }
                    else {
                        selectedShapes.push_back(hoveredShape);
                    }
                }
                else if (isShiftDown) {
                    if (!alreadySelected) {
                        selectedShapes.push_back(hoveredShape);
                    }
                }
                if (hoveredHandle == InteractionHandle::Body && !selectedShapes.empty()) {
                    g_InteractionState.type = InteractionType::Selecting;
                    g_InteractionState.activeHandle = InteractionHandle::Body;
                    g_InteractionState.dragStartMousePos = mouseScreenPos;
                    g_InteractionState.dragStartShapePositions.clear();
                    g_InteractionState.dragStartShapeSizes.clear();
                    g_InteractionState.dragStartShapeRotations.clear();
                    for (ShapeItem* s : selectedShapes) {
                        g_InteractionState.dragStartShapePositions.push_back({ s, s->position });
                        g_InteractionState.dragStartShapeSizes.push_back({ s, s->size });
                        g_InteractionState.dragStartShapeRotations.push_back({ s, s->rotation });
                    }
                    g_InteractionState.undoStateRecorded = false;
                }
            }
            else {
                if (!isShiftDown && !isCtrlDown) {
                    selectedShapes.clear();
                }
                g_InteractionState = {};
            }
            MarkSceneUpdated();
        }
        else if (isDragging && g_InteractionState.type != InteractionType::None) {
            ImVec2 mouseDelta = mouseScreenPos - g_InteractionState.dragStartMousePos;
            if (g_InteractionState.type == InteractionType::Selecting && LengthV(mouseDelta) > 3.0f) {
                if (g_InteractionState.activeHandle == InteractionHandle::Body) {
                    g_InteractionState.type = InteractionType::Dragging;
                }
            }
            if (g_InteractionState.type == InteractionType::Dragging) {
                for (size_t i = 0; i < g_InteractionState.dragStartShapePositions.size(); ++i) {
                    ShapeItem* shape = g_InteractionState.dragStartShapePositions[i].first;
                    if (shape && std::find(selectedShapes.begin(), selectedShapes.end(), shape) != selectedShapes.end()) {
                        ImVec2 originalPos = g_InteractionState.dragStartShapePositions[i].second;
                        shape->position = originalPos + mouseDelta;
                    }
                }
                MarkSceneUpdated();
            }
            else if (g_InteractionState.type == InteractionType::Resizing) {
                if (!g_InteractionState.dragStartShapeSizes.empty()) {
                    ShapeItem* shape = g_InteractionState.dragStartShapeSizes[0].first;
                    ImVec2 originalSize = g_InteractionState.dragStartShapeSizes[0].second;
                    ImVec2 originalPos = g_InteractionState.dragStartShapePositions[0].second;
                    float originalRot = g_InteractionState.dragStartShapeRotations[0].second;
                    ImVec2 center = g_InteractionState.interactionStartShapeCenter;
                    ImVec2 localStartMouse = RotateP(g_InteractionState.dragStartMousePos, center, -originalRot);
                    ImVec2 localCurrentMouse = RotateP(mouseScreenPos, center, -originalRot);
                    ImVec2 localDelta = localCurrentMouse - localStartMouse;
                    ImVec2 newSize = originalSize;
                    ImVec2 posOffset(0, 0);
                    switch (g_InteractionState.activeHandle) {
                    case InteractionHandle::BottomRight: newSize.x += localDelta.x; newSize.y += localDelta.y; break;
                    case InteractionHandle::BottomLeft: newSize.x -= localDelta.x; newSize.y += localDelta.y; posOffset.x += localDelta.x; break;
                    case InteractionHandle::TopRight: newSize.x += localDelta.x; newSize.y -= localDelta.y; posOffset.y += localDelta.y; break;
                    case InteractionHandle::TopLeft: newSize.x -= localDelta.x; newSize.y -= localDelta.y; posOffset = localDelta; break;
                    case InteractionHandle::Top: newSize.y -= localDelta.y; posOffset.y += localDelta.y; break;
                    case InteractionHandle::Bottom: newSize.y += localDelta.y; break;
                    case InteractionHandle::Left: newSize.x -= localDelta.x; posOffset.x += localDelta.x; break;
                    case InteractionHandle::Right: newSize.x += localDelta.x; break;
                    default: break;
                    }
                    newSize.x = std::max(shape->minSize.x, std::min(newSize.x, shape->maxSize.x));
                    newSize.y = std::max(shape->minSize.y, std::min(newSize.y, shape->maxSize.y));
                    newSize.x = std::max(1.0f, newSize.x);
                    newSize.y = std::max(1.0f, newSize.y);
                    ImVec2 rotatedPosOffset = RotateP(posOffset, ImVec2(0, 0), originalRot);
                    shape->size = newSize;
                    shape->position = originalPos + rotatedPosOffset;
                    MarkSceneUpdated();
                }
            }
            else if (g_InteractionState.type == InteractionType::Rotating) {
                if (!g_InteractionState.dragStartShapeRotations.empty()) {
                    ShapeItem* shape = g_InteractionState.dragStartShapeRotations[0].first;
                    float originalRot = g_InteractionState.dragStartShapeRotations[0].second;
                    ImVec2 center = g_InteractionState.interactionStartShapeCenter;
                    ImVec2 startVec = g_InteractionState.dragStartMousePos - center;
                    ImVec2 currentVec = mouseScreenPos - center;
                    float angleDelta = atan2f(currentVec.y, currentVec.x) - atan2f(startVec.y, startVec.x);
                    shape->rotation = originalRot + angleDelta;
                    MarkSceneUpdated();
                }
            }
        }
        else if (isReleased) {
            if (g_InteractionState.type != InteractionType::None && !g_InteractionState.undoStateRecorded) {
                ImVec2 finalMouseDelta = mouseScreenPos - g_InteractionState.dragStartMousePos;
                if (g_InteractionState.type == InteractionType::Dragging) {
                    for (size_t i = 0; i < g_InteractionState.dragStartShapePositions.size(); ++i) {
                        ShapeItem* shape = g_InteractionState.dragStartShapePositions[i].first;
                        if (shape && std::find(selectedShapes.begin(), selectedShapes.end(), shape) != selectedShapes.end()) {
                            ImVec2 finalScreenPos = g_InteractionState.dragStartShapePositions[i].second + finalMouseDelta;
                            if (shape->parent == nullptr) {
                                shape->basePosition = finalScreenPos;
                            }
                            else {
                                ImVec2 parentScreenPos = shape->parent->position;
                                float parentScreenRot = shape->parent->rotation;
                                ImVec2 worldOffset = finalScreenPos - parentScreenPos;
                                ImVec2 localOffset = RotateP(worldOffset, ImVec2(0, 0), -parentScreenRot);
                                shape->originalPosition = localOffset;
                                shape->basePosition = localOffset;
                            }
                            shape->position = finalScreenPos;
                        }
                    }
                }
                else if (g_InteractionState.type == InteractionType::Resizing) {
                    for (size_t i = 0; i < g_InteractionState.dragStartShapeSizes.size(); ++i) {
                        ShapeItem* shape = g_InteractionState.dragStartShapeSizes[i].first;
                        if (shape && std::find(selectedShapes.begin(), selectedShapes.end(), shape) != selectedShapes.end()) {
                            shape->baseSize = shape->size;
                            if (i < g_InteractionState.dragStartShapePositions.size()) {
                                ImVec2 originalScreenPos = g_InteractionState.dragStartShapePositions[i].second;
                                float originalScreenRot = g_InteractionState.dragStartShapeRotations[i].second;
                                ImVec2 centerAtStart = originalScreenPos + g_InteractionState.dragStartShapeSizes[i].second * 0.5f;
                                ImVec2 localStartMouse = RotateP(g_InteractionState.dragStartMousePos, centerAtStart, -originalScreenRot);
                                ImVec2 localCurrentMouse = RotateP(mouseScreenPos, centerAtStart, -originalScreenRot);
                                ImVec2 localDelta = localCurrentMouse - localStartMouse;
                                ImVec2 posOffset(0, 0);
                                switch (g_InteractionState.activeHandle) {
                                case InteractionHandle::BottomLeft: posOffset.x += localDelta.x; break;
                                case InteractionHandle::TopRight:   posOffset.y += localDelta.y; break;
                                case InteractionHandle::TopLeft:    posOffset = localDelta; break;
                                case InteractionHandle::Top:        posOffset.y += localDelta.y; break;
                                case InteractionHandle::Left:       posOffset.x += localDelta.x; break;
                                default: break;
                                }
                                ImVec2 rotatedPosOffset = RotateP(posOffset, ImVec2(0, 0), originalScreenRot);
                                ImVec2 finalScreenPos = originalScreenPos + rotatedPosOffset;
                                if (shape->parent == nullptr) {
                                    shape->basePosition = finalScreenPos;
                                }
                                else {
                                    ImVec2 parentScreenPos = shape->parent->position;
                                    float parentScreenRot = shape->parent->rotation;
                                    ImVec2 worldOffset = finalScreenPos - parentScreenPos;
                                    ImVec2 localOffset = RotateP(worldOffset, ImVec2(0, 0), -parentScreenRot);
                                    shape->originalPosition = localOffset;
                                    shape->basePosition = localOffset;
                                }
                                shape->position = finalScreenPos;
                            }
                        }
                    }
                }
                else if (g_InteractionState.type == InteractionType::Rotating) {
                    for (size_t i = 0; i < g_InteractionState.dragStartShapeRotations.size(); ++i) {
                        ShapeItem* shape = g_InteractionState.dragStartShapeRotations[i].first;
                        if (shape && std::find(selectedShapes.begin(), selectedShapes.end(), shape) != selectedShapes.end()) {
                            if (shape->parent == nullptr) {
                                shape->baseRotation = shape->rotation;
                            }
                            else {
                                shape->baseRotation = shape->rotation - shape->parent->rotation;
                            }
                        }
                    }
                }
                g_InteractionState.undoStateRecorded = true;
                MarkSceneUpdated();
            }
            g_InteractionState = {};
        }

        if (hoveredHandle != InteractionHandle::None && hoveredHandle != InteractionHandle::Body && g_InteractionState.type == InteractionType::None) {
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
        }
        else if (g_InteractionState.type == InteractionType::Dragging || hoveredHandle == InteractionHandle::Body || g_InteractionState.type == InteractionType::Selecting) {
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        }
        else if (g_InteractionState.type == InteractionType::Resizing) {
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
        }
        else if (g_InteractionState.type == InteractionType::Rotating) {
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        }
        else {
            ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
        }
    }

    void DrawInteractionGizmos(ImDrawList* fgDrawList) {
        if (!g_IsInEditMode || selectedShapes.empty()) return;

        ImU32 gizmoColor = IM_COL32(0, 150, 255, 220);
        ImU32 gizmoHandleFillColor = IM_COL32(255, 255, 255, 255);
        float handleSize = 8.0f;
        ImVec2 handleHalfSizeVec = ImVec2(handleSize * 0.5f, handleSize * 0.5f);
        float lineThickness = 1.5f;

        if (selectedShapes.size() == 1) {
            ShapeItem& s = *selectedShapes[0];
            ImVec2 posScreen = s.position;
            ImVec2 size = s.size;
            float rotation = s.rotation;
            ImVec2 centerScreen = posScreen + size * 0.5f;
            ImVec2 cornersScreen[4];
            cornersScreen[0] = RotateP(posScreen, centerScreen, rotation);
            cornersScreen[1] = RotateP(ImVec2(posScreen.x + size.x, posScreen.y), centerScreen, rotation);
            cornersScreen[2] = RotateP(posScreen + size, centerScreen, rotation);
            cornersScreen[3] = RotateP(ImVec2(posScreen.x, posScreen.y + size.y), centerScreen, rotation);
            fgDrawList->AddPolyline(cornersScreen, 4, gizmoColor, ImDrawFlags_Closed, lineThickness);
            ImVec2 handlesScreen[8];
            for (int i = 0; i < 8; ++i) {
                ImVec2 handleCenterScreen;
                if (i % 2 == 0) handleCenterScreen = cornersScreen[i / 2];
                else handleCenterScreen = (cornersScreen[i / 2] + cornersScreen[(i / 2 + 1) % 4]) * 0.5f;
                handlesScreen[i] = handleCenterScreen;
                fgDrawList->AddRectFilled(handleCenterScreen - handleHalfSizeVec, handleCenterScreen + handleHalfSizeVec, gizmoHandleFillColor, 2.0f);
                fgDrawList->AddRect(handleCenterScreen - handleHalfSizeVec, handleCenterScreen + handleHalfSizeVec, gizmoColor, 2.0f, 0, lineThickness);
            }
        }
        else {
            ImRect selectionBounds;
            bool first = true;
            for (const auto* shape : selectedShapes) {
                if (shape) {
                    ImRect shapeBox = GetShapeBoundingBox(*shape);
                    if (first) {
                        selectionBounds = shapeBox;
                        first = false;
                    }
                    else {
                        selectionBounds.Add(shapeBox);
                    }
                }
            }
            if (!first) {
                fgDrawList->AddRect(selectionBounds.Min, selectionBounds.Max, gizmoColor, 0.0f, 0, lineThickness);
            }
        }
    }

    void ShowUI_HierarchyPanel(WindowData& windowData, int& selectedLayerIndex, std::vector<ShapeItem*>& selectedShapes) {
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
                        auto it = std::find_if(shapes.begin(), shapes.end(), [id_to_find = dragged_shape_ptr->id](const std::unique_ptr<ShapeItem>& s_uptr) { return s_uptr && s_uptr->id == id_to_find; });
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
                            [id_to_remove = movedShape.id](const std::unique_ptr<ShapeItem>& s_uptr) { return s_uptr && s_uptr->id == id_to_remove; }),
                            originalLayerPtr->shapes.end());

                        layer.shapes.push_back(std::make_unique<ShapeItem>(movedShape));
                        ShapeItem* newShapePtr = layer.shapes.back().get();

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
                            for (const auto& shp_uptr : l.shapes) if (shp_uptr) existingNamesInWindow.insert(shp_uptr->name);
                        }

                        for (const auto& shapeTemplate : compDef.shapeTemplates) {
                            ShapeItem newShapeItem = shapeTemplate.item;
                            int newId = GetUniqueShapeID();
                            newShapeItem.id = newId;
                            newShapeItem.position = instantiationPos + shapeTemplate.item.position;
                            newShapeItem.basePosition = newShapeItem.position;
                            newShapeItem.ownerWindow = selectedGuiWindow;
                            newShapeItem.parent = nullptr;
                            newShapeItem.children.clear();
                            newShapeItem.visible = true;
                            newShapeItem.locked = false;
                            newShapeItem.isPressed = false;
                            newShapeItem.isHeld = false;
                            newShapeItem.isAnimating = false;
                            newShapeItem.currentAnimation = nullptr;

                            std::string baseName = componentNameToInstantiate + "_" + shapeTemplate.item.name;
                            std::string finalName = baseName;
                            int suffix = 1;
                            while (existingNamesInWindow.count(finalName)) { finalName = baseName + "_" + std::to_string(suffix++); }
                            newShapeItem.name = finalName;
                            existingNamesInWindow.insert(finalName);

                            layer.shapes.push_back(std::make_unique<ShapeItem>(newShapeItem));
                            ShapeItem* ptr = layer.shapes.back().get();

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
                    for (const auto& s_uptr : layer.shapes) if (s_uptr) shapesInDeletedLayerIds.push_back(s_uptr->id);

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
                    ShapeItem s_item;
                    std::string baseName = "New Shape";
                    std::string finalName = baseName;
                    int suffix = 0;
                    std::set<std::string> existingNames;
                    for (const auto& l : windowData.layers) for (const auto& shp_uptr : l.shapes) if (shp_uptr) existingNames.insert(shp_uptr->name);
                    while (existingNames.count(finalName)) { finalName = baseName + "_" + std::to_string(suffix++); }
                    s_item.name = finalName;
                    s_item.id = GetUniqueShapeID();
                    s_item.ownerWindow = selectedGuiWindow;
                    s_item.zOrder = layer.shapes.empty() ? 0 : (layer.shapes.back() ? layer.shapes.back()->zOrder + 1 : 0);
                    s_item.position = ImVec2(150, 150);
                    s_item.size = ImVec2(150, 150);
                    s_item.basePosition = s_item.position;
                    s_item.baseSize = s_item.size;
                    s_item.parent = nullptr;
                    s_item.children.clear();

                    layer.shapes.push_back(std::make_unique<ShapeItem>(s_item));
                    selectedLayerIndex = i;
                    selectedShapes.clear();
                    selectedShapes.push_back(layer.shapes.back().get());
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
                    std::stable_sort(layer.shapes.begin(), layer.shapes.end(), [](const auto& a, const auto& b) {
                        return (a && b) ? (a->zOrder < b->zOrder) : (a != nullptr);
                        });

                    if (!selectedShapes.empty() && lastSelectedLayerIndex == i && lastSelectedShapeIndex != -1) {
                        int currentlySelectedShapeId = selectedShapes.back()->id;
                        lastSelectedShapeIndex = -1;
                        for (int newIdx = 0; newIdx < layer.shapes.size(); ++newIdx) {
                            if (layer.shapes[newIdx] && layer.shapes[newIdx]->id == currentlySelectedShapeId) {
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
                    ShapeItem* current_shape = layer.shapes[j].get();
                    if (current_shape && current_shape->parent == nullptr) {
                        DrawShapeTreeNode(current_shape, layer, i, j, selectedLayerIndex, selectedShapes, lastClickedShape, lastClickedLayerIndex, layerToSort, needsLayerSort);
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

    void DrawShapeTreeNode(ShapeItem* shape, Layer& layer, int layerIndex, int shapeIndexInLayer, int& selectedLayerIndex, std::vector<ShapeItem*>& selectedShapes, ShapeItem*& lastClickedShape, int& lastClickedLayerIndex, int& layerToSort, bool& needsLayerSort) {
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
                        if (layer.shapes[k] && layer.shapes[k]->id == shape->id) currentShapeIndexInLayerVec = k;
                        if (layer.shapes[k] && layer.shapes[k]->id == lastClickedShape->id) lastShapeIndexInLayerVec = k;
                    }
                    if (currentShapeIndexInLayerVec != -1 && lastShapeIndexInLayerVec != -1) {
                        int startIdx = std::min(currentShapeIndexInLayerVec, lastShapeIndexInLayerVec);
                        int endIdx = std::max(currentShapeIndexInLayerVec, lastShapeIndexInLayerVec);
                        for (int k = startIdx; k <= endIdx; ++k) {
                            if (k < layer.shapes.size() && layer.shapes[k]) {
                                ShapeItem& potentialSelection = *layer.shapes[k];
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
                ShapeItem duplicatedShapeItem = *shape;
                duplicatedShapeItem.id = GetUniqueShapeID();
                std::string baseName = shape->name + "_Copy";
                std::string finalName = baseName;
                int suffix = 1;
                std::set<std::string> existingNames;
                WindowData& currentWindowData = g_windowsMap.at(selectedGuiWindow);
                for (const auto& l : currentWindowData.layers) {
                    for (const auto& shp_uptr : l.shapes) if (shp_uptr) existingNames.insert(shp_uptr->name);
                }
                while (existingNames.count(finalName)) {
                    finalName = baseName + "_" + std::to_string(suffix++);
                }
                duplicatedShapeItem.name = finalName;
                duplicatedShapeItem.parent = nullptr;
                duplicatedShapeItem.children.clear();
                duplicatedShapeItem.isPressed = false;
                duplicatedShapeItem.isHeld = false;
                duplicatedShapeItem.isAnimating = false;
                duplicatedShapeItem.currentAnimation = nullptr;
                duplicatedShapeItem.position += ImVec2(10, 10);
                duplicatedShapeItem.basePosition = duplicatedShapeItem.position;
                layer.shapes.push_back(std::make_unique<ShapeItem>(duplicatedShapeItem));
                selectedShapes.clear();
                selectedShapes.push_back(layer.shapes.back().get());
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
                    [deletedShapeId](const std::unique_ptr<ShapeItem>& s_uptr) { return s_uptr && s_uptr->id == deletedShapeId; }),
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

    void ShowUI_PropertiesPanel(WindowData& windowData, int& selectedLayerIndex, std::vector<ShapeItem*>& selectedShapes) {
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
                        [deletedShapeId](const std::unique_ptr<ShapeItem>& shp_uptr) { return shp_uptr && shp_uptr->id == deletedShapeId; }),
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
                    ShapeItem duplicatedShapeItem = s;
                    duplicatedShapeItem.id = GetUniqueShapeID();
                    std::string baseName = s.name + "_Copy";
                    std::string finalName = baseName;
                    int suffix = 1;
                    std::set<std::string> existingNames;
                    for (const auto& l : windowData.layers) for (const auto& shp_uptr : l.shapes) if (shp_uptr) existingNames.insert(shp_uptr->name);
                    while (existingNames.count(finalName)) { finalName = baseName + "_" + std::to_string(suffix++); }
                    duplicatedShapeItem.name = finalName;
                    duplicatedShapeItem.parent = nullptr;
                    duplicatedShapeItem.children.clear();
                    duplicatedShapeItem.isPressed = false;
                    duplicatedShapeItem.isHeld = false;
                    duplicatedShapeItem.isAnimating = false;
                    duplicatedShapeItem.currentAnimation = nullptr;
                    duplicatedShapeItem.position += ImVec2(10, 10);
                    duplicatedShapeItem.basePosition = duplicatedShapeItem.position;
                    layer.shapes.push_back(std::make_unique<ShapeItem>(duplicatedShapeItem));
                    selectedShapes.clear();
                    selectedShapes.push_back(layer.shapes.back().get());
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
                    s.position = tempAbsPos; 
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
                        s.size = tempAbsSize; 
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
                        }
                        s.rotation = newWorldRotRad; 
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
                if (ImGui::DragFloat("Corner Radius", &s.cornerRadius, 0.1f, 0.0f, 200.0f)) MarkSceneUpdated();
                if (ImGui::DragFloat("Border", &s.borderThickness, 0.1f, 0.0f, 2000.0f)) MarkSceneUpdated();
                ImGui::SeparatorText("Fill & Border");
                if (ImGui::ColorEdit4("Fill##Color", (float*)&s.fillColor, ImGuiColorEditFlags_AlphaBar)) MarkSceneUpdated();
                if (ImGui::ColorEdit4("Border##Color", (float*)&s.borderColor, ImGuiColorEditFlags_AlphaBar)) MarkSceneUpdated();
                if (ImGui::Checkbox("Use Per-Side Border Colors##Appearance", &s.usePerSideBorderColors)) MarkSceneUpdated();
                if (s.usePerSideBorderColors) {
                    ImGui::Indent();
                    if (ImGui::ColorEdit4("Top##BorderColorSides", (float*)&s.borderSideColors[0], ImGuiColorEditFlags_AlphaBar)) MarkSceneUpdated();
                    if (ImGui::ColorEdit4("Right##BorderColorSides", (float*)&s.borderSideColors[1], ImGuiColorEditFlags_AlphaBar)) MarkSceneUpdated();
                    if (ImGui::ColorEdit4("Bottom##BorderColorSides", (float*)&s.borderSideColors[2], ImGuiColorEditFlags_AlphaBar)) MarkSceneUpdated();
                    if (ImGui::ColorEdit4("Left##BorderColorSides", (float*)&s.borderSideColors[3], ImGuiColorEditFlags_AlphaBar)) MarkSceneUpdated();
                    if (ImGui::Button("Set All Sides to Main Border Color##Appearance")) {
                        for (int i = 0; i < 4; ++i) s.borderSideColors[i] = s.borderColor;
                        MarkSceneUpdated();
                    }
                    ImGui::Unindent();
                }
                if (ImGui::Checkbox("Use Per-Side Border Thicknesses##Appearance", &s.usePerSideBorderThicknesses)) MarkSceneUpdated();
                if (s.usePerSideBorderThicknesses) {
                    ImGui::Indent();
                    float bst0 = s.borderSideThicknesses[0]; float bst1 = s.borderSideThicknesses[1];
                    float bst2 = s.borderSideThicknesses[2]; float bst3 = s.borderSideThicknesses[3];
                    bool changed = false;
                    if (ImGui::DragFloat("Top##BorderThickSides", &bst0, 0.1f, 0.0f, 200.0f)) { s.borderSideThicknesses[0] = bst0; changed = true; }
                    if (ImGui::DragFloat("Right##BorderThickSides", &bst1, 0.1f, 0.0f, 200.0f)) { s.borderSideThicknesses[1] = bst1; changed = true; }
                    if (ImGui::DragFloat("Bottom##BorderThickSides", &bst2, 0.1f, 0.0f, 200.0f)) { s.borderSideThicknesses[2] = bst2; changed = true; }
                    if (ImGui::DragFloat("Left##BorderThickSides", &bst3, 0.1f, 0.0f, 200.0f)) { s.borderSideThicknesses[3] = bst3; changed = true; }
                    if (changed) MarkSceneUpdated();

                    if (ImGui::Button("Set All Sides to Main Border Thickness##Appearance")) {
                        for (int i = 0; i < 4; ++i) s.borderSideThicknesses[i] = s.borderThickness;
                        MarkSceneUpdated();
                    }
                    ImGui::Unindent();
                }
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
                
                ImGui::SeparatorText("Sizing Model");
                const char* boxSizingModes[] = { "Border Box (KenarlÄ±k Ä°Ã§eride)", "Content Box (KenarlÄ±k DÄ±ÅŸarÄ±da)", "Stroke Box (KenarlÄ±k Ortada)" };
                int currentBoxSizing = static_cast<int>(s.boxSizing);
                if (ImGui::Combo("Box Sizing##Appearance", &currentBoxSizing, boxSizingModes, IM_ARRAYSIZE(boxSizingModes))) {
                    s.boxSizing = static_cast<ShapeItem::BoxSizing>(currentBoxSizing);
                    MarkSceneUpdated();
                }
                if (ImGui::IsItemHovered()) {
                    ImGui::BeginTooltip();
                    ImGui::TextUnformatted("Åeklin boyutunu kenarlÄ±k ve dolgunun nasÄ±l etkileyeceÄŸini belirler:\n"
                        "- Border Box: Boyut, kenarlÄ±ÄŸÄ± ve dolguyu iÃ§erir. Ä°Ã§erik kÃ¼Ã§Ã¼lÃ¼r.\n"
                        "  (CSS border-box benzeri)\n"
                        "- Content Box: Boyut sadece iÃ§eriÄŸi temsil eder. KenarlÄ±k/dolgu dÄ±ÅŸa doÄŸru geniÅŸler.\n"
                        "  (CSS content-box benzeri, kenarlÄ±k yerleÅŸimi iÃ§in)\n"
                        "- Stroke Box: Boyut iÃ§eriÄŸi temsil eder. KenarlÄ±k kenar Ã¼zerine ortalanÄ±r.\n"
                        "  (Mevcut varsayÄ±lan davranÄ±ÅŸ)");
                    ImGui::EndTooltip();
                }
                if (ImGui::CollapsingHeader("Padding & Margin##PropsPanel", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    
                    
                    
                    

                    float padding_arr[4] = { s.padding.x, s.padding.y, s.padding.z, s.padding.w };
                    if (ImGui::DragFloat4("Padding (L,T,R,B)##Shape", padding_arr, 0.5f, 0.0f, 9999.0f, "%.1f")) {
                        s.padding = ImVec4(std::max(0.0f, padding_arr[0]),
                            std::max(0.0f, padding_arr[1]),
                            std::max(0.0f, padding_arr[2]),
                            std::max(0.0f, padding_arr[3]));
                        MarkSceneUpdated();
                    }

                    float margin_arr[4] = { s.margin.x, s.margin.y, s.margin.z, s.margin.w };
                    if (ImGui::DragFloat4("Margin (L,T,R,B)##Shape", margin_arr, 0.5f, -9999.0f, 9999.0f, "%.1f")) {
                        s.margin = ImVec4(margin_arr[0], margin_arr[1], margin_arr[2], margin_arr[3]);
                        
                        MarkSceneUpdated();
                    }
                    
                }
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
                    ImGuiIO& io = ImGui::GetIO();
                    if (io.Fonts->Fonts.Size > 0) {
                        static auto font_getter = [](void* data, int idx, const char** out_text) -> bool {
                            ImGuiIO& io_local = ImGui::GetIO();
                            if (idx < 0 || idx >= io_local.Fonts->Fonts.Size) return false;
                            static std::vector<std::string> cached_names_local;
                            if (cached_names_local.size() != io_local.Fonts->Fonts.Size) {
                                cached_names_local.clear();
                                for (int i_font = 0; i_font < io_local.Fonts->Fonts.Size; ++i_font) {
                                    cached_names_local.push_back("Font " + std::to_string(i_font));
                                }
                            }
                            if (idx < cached_names_local.size()) {
                                *out_text = cached_names_local[idx].c_str();
                                return true;
                            }
                            return false;
                            };
                        int currentFontIndex = s.textFont;
                        if (currentFontIndex < 0 || currentFontIndex >= io.Fonts->Fonts.Size) {
                            currentFontIndex = 0;
                            s.textFont = 0;
                        }
                        if (ImGui::Combo("Font##Text", &currentFontIndex, font_getter, nullptr, io.Fonts->Fonts.Size)) {
                            if (currentFontIndex >= 0 && currentFontIndex < io.Fonts->Fonts.Size) {
                                s.textFont = currentFontIndex;
                                MarkSceneUpdated();
                            }
                        }
                    }
                    else {
                        ImGui::TextDisabled("No fonts loaded.");
                    }
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

            bool showChildSettings = (s.parent != nullptr && s.parent->isLayoutContainer && s.parent->layoutManager != nullptr);
            if (showChildSettings)
            {
                if (ImGui::CollapsingHeader("Layout Ãƒâ€¡ocuk AyarlarÃ„Â±", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    if (ImGui::DragFloat("Esneme FaktÃƒÂ¶rÃƒÂ¼ (Stretch)##LayoutChild", &s.stretchFactor, 0.1f, 0.0f, 10000.0f)) {
                        s.stretchFactor = std::max(0.0f, s.stretchFactor);
                        MarkSceneUpdated();
                    }
                    if (ImGui::IsItemHovered()) ImGui::SetTooltip("0: Esneme yok.\n>0: DiÃ„Å¸er esneyen elemanlarla orantÃ„Â±lÃ„Â± olarak boÃ…Å¸ alanÃ„Â± doldurur.");
                    const char* hAlignItems[] = { "Doldur (Fill)", "Sol (Left)", "Orta (Center)", "SaÃ„Å¸ (Right)" };
                    int currentHAlign = static_cast<int>(s.horizontalAlignment);
                    if (ImGui::Combo("Yatay Hizalama##LayoutChild", &currentHAlign, hAlignItems, IM_ARRAYSIZE(hAlignItems))) {
                        s.horizontalAlignment = static_cast<HAlignment>(currentHAlign);
                        MarkSceneUpdated();
                    }
                    const char* vAlignItems[] = { "Doldur (Fill)", "ÃƒÅ“st (Top)", "Orta (Center)", "Alt (Bottom)" };
                    int currentVAlign = static_cast<int>(s.verticalAlignment);
                    if (ImGui::Combo("Dikey Hizalama##LayoutChild", &currentVAlign, vAlignItems, IM_ARRAYSIZE(vAlignItems))) {
                        s.verticalAlignment = static_cast<VAlignment>(currentVAlign);
                        MarkSceneUpdated();
                    }
                }
            }

            ImGui::Checkbox("Show Layout Debug Lines", &g_ShowLayoutDebugLines);
            ImGui::Separator();

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

            if (ImGui::CollapsingHeader("Positioning & Constraints", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::BeginDisabled(shape_effectively_locked);
                const char* positioningModes[] = { "Relative", "Absolute" };
                int currentMode = static_cast<int>(s.positioningMode);
                ImGui::SetNextItemWidth(-FLT_MIN * 0.5f);
                if (ImGui::Combo("Positioning", &currentMode, positioningModes, IM_ARRAYSIZE(positioningModes))) {
                    s.positioningMode = static_cast<PositioningMode>(currentMode);
                    MarkSceneUpdated();
                }
                ImGui::SeparatorText("Constraints");
                bool constraintsDisabled = (s.positioningMode == PositioningMode::Relative && s.parent && s.parent->isLayoutContainer && s.parent->layoutManager);
                ImGui::BeginDisabled(constraintsDisabled);
                if (constraintsDisabled) { ImGui::TextDisabled("Constraints ignored for Relative items inside Layout."); }
                int constraint_to_delete = -1;
                for (int i = 0; i < s.constraints.size(); ++i) {
                    ImGui::PushID(i + 8000);
                    auto& constraint = s.constraints[i];
                    const char* constraintTypes[] = { "LeftDist", "RightDist", "TopDist", "BottomDist", "CenterX", "CenterY", "WidthFix", "HeightFix", "Width%", "Height%", "Aspect" };
                    int currentConstraintType = static_cast<int>(constraint.type);
                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.5f - 35);
                    if (ImGui::Combo("##Type", &currentConstraintType, constraintTypes, IM_ARRAYSIZE(constraintTypes))) { constraint.type = static_cast<ConstraintType>(currentConstraintType); MarkSceneUpdated(); }
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
                    const char* format = (constraint.type == ConstraintType::AspectRatio) ? "%.3f" : "%.1f";
                    if (ImGui::DragFloat("Value##Val", &constraint.value, 0.5f, 0.0f, 0.0f, format)) { MarkSceneUpdated(); }
                    if (constraint.type == ConstraintType::AspectRatio && ImGui::IsItemHovered()) { ImGui::SetTooltip("Value = Width / Height"); }
                    ImGui::SameLine();
                    if (ImGui::Button("X##DelC")) { constraint_to_delete = i; }
                    ImGui::PopID();
                }
                if (constraint_to_delete != -1) { s.constraints.erase(s.constraints.begin() + constraint_to_delete); MarkSceneUpdated(); }
                if (ImGui::Button("+ Add Constraint")) { s.constraints.push_back({}); MarkSceneUpdated(); }
                ImGui::EndDisabled();
                ImGui::SeparatorText("Size Limits");
                if (ImGui::DragFloat2("Min Size##Const", (float*)&s.minSize, 1.0f, 0.0f, 99999.0f, "%.0f")) { s.minSize.x = std::max(0.f, s.minSize.x); s.minSize.y = std::max(0.f, s.minSize.y); s.maxSize.x = std::max(s.minSize.x, s.maxSize.x); s.maxSize.y = std::max(s.minSize.y, s.maxSize.y); s.size.x = std::max(s.minSize.x, std::min(s.size.x, s.maxSize.x)); s.size.y = std::max(s.minSize.y, std::min(s.size.y, s.maxSize.y)); s.baseSize.x = std::max(s.minSize.x, std::min(s.baseSize.x, s.maxSize.x)); s.baseSize.y = std::max(s.minSize.y, std::min(s.baseSize.y, s.maxSize.y)); MarkSceneUpdated(); }
                if (ImGui::DragFloat2("Max Size##Const", (float*)&s.maxSize, 1.0f, 0.0f, 99999.0f, "%.0f")) { s.maxSize.x = std::max(0.f, s.maxSize.x); s.maxSize.y = std::max(0.f, s.maxSize.y); s.maxSize.x = std::max(s.minSize.x, s.maxSize.x); s.maxSize.y = std::max(s.minSize.y, s.maxSize.y); s.size.x = std::max(s.minSize.x, std::min(s.size.x, s.maxSize.x)); s.size.y = std::max(s.minSize.y, std::min(s.size.y, s.maxSize.y)); s.baseSize.x = std::max(s.minSize.x, std::min(s.baseSize.x, s.maxSize.x)); s.baseSize.y = std::max(s.minSize.y, std::min(s.baseSize.y, s.maxSize.y)); MarkSceneUpdated(); }
                ImGui::EndDisabled();
            }

            if (ImGui::CollapsingHeader("Layout Container Settings", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::BeginDisabled(shape_effectively_locked);
                bool isContainer = s.isLayoutContainer;
                if (ImGui::Checkbox("Is Layout Container", &isContainer)) {
                    if (s.isLayoutContainer != isContainer) {
                        s.isLayoutContainer = isContainer;
                        if (!isContainer && s.layoutManager != nullptr) { s.layoutManager.reset(); }
                        else if (isContainer && s.layoutManager == nullptr) {}
                        MarkSceneUpdated();
                    }
                }
                ImGui::BeginDisabled(!s.isLayoutContainer);
                const char* layoutTypes[] = { "None", "Vertical", "Horizontal", "FlexLayout", "GridLayout" };
                int currentLayoutIndex = 0;
                if (s.layoutManager != nullptr) {
                    const char* typeName = s.layoutManager->getTypeName();
                    if (strcmp(typeName, "Vertical") == 0) currentLayoutIndex = 1;
                    else if (strcmp(typeName, "Horizontal") == 0) currentLayoutIndex = 2;
                    else if (strcmp(typeName, "FlexLayout") == 0) currentLayoutIndex = 3;
                    else if (strcmp(typeName, "GridLayout") == 0) currentLayoutIndex = 4;
                }
                ImGui::SetNextItemWidth(-FLT_MIN * 0.6f);
                if (ImGui::Combo("Layout Type", &currentLayoutIndex, layoutTypes, IM_ARRAYSIZE(layoutTypes))) {
                    std::unique_ptr<LayoutManager> newManager = nullptr;
                    switch (currentLayoutIndex) {
                    case 1: newManager = std::make_unique<VerticalLayout>(); break;
                    case 2: newManager = std::make_unique<HorizontalLayout>(); break;
                    case 3: newManager = std::make_unique<FlexLayout>(); break;
                    case 4: newManager = std::make_unique<GridLayout>(); break;
                    default: break;
                    }
                    
                    bool typeChanged = (!s.layoutManager && newManager) || (s.layoutManager && !newManager) ||
                        (s.layoutManager && newManager && strcmp(s.layoutManager->getTypeName(), newManager->getTypeName()) != 0);

                    if (typeChanged) {
                        s.layoutManager = std::move(newManager);
                        MarkSceneUpdated();
                    }
                }
                if (s.layoutManager != nullptr) {
                    ImGui::Separator();
                    if (FlexLayout* flex = dynamic_cast<FlexLayout*>(s.layoutManager.get())) {
                        ImGui::TextUnformatted("Flex Options:"); ImGui::Indent();
                        const char* flexDirs[] = { "Row", "RowReverse", "Column", "ColumnReverse" }; int currentDir = static_cast<int>(flex->direction); if (ImGui::Combo("Direction", &currentDir, flexDirs, IM_ARRAYSIZE(flexDirs))) { flex->direction = static_cast<FlexDirection>(currentDir); MarkSceneUpdated(); }
                        const char* flexWraps[] = { "NoWrap", "Wrap", "WrapReverse" }; int currentWrap = static_cast<int>(flex->wrap); if (ImGui::Combo("Wrap", &currentWrap, flexWraps, IM_ARRAYSIZE(flexWraps))) { flex->wrap = static_cast<FlexWrap>(currentWrap); MarkSceneUpdated(); }
                        const char* flexJustify[] = { "FlexStart", "FlexEnd", "Center", "SpaceBetween", "SpaceAround", "SpaceEvenly" }; int currentJustify = static_cast<int>(flex->justifyContent); if (ImGui::Combo("Justify Content", &currentJustify, flexJustify, IM_ARRAYSIZE(flexJustify))) { flex->justifyContent = static_cast<JustifyContent>(currentJustify); MarkSceneUpdated(); }
                        const char* flexAlignItems[] = { "Stretch", "FlexStart", "FlexEnd", "Center", "Baseline" }; int currentAlignItems = static_cast<int>(flex->alignItems); if (ImGui::Combo("Align Items", &currentAlignItems, flexAlignItems, IM_ARRAYSIZE(flexAlignItems))) { flex->alignItems = static_cast<AlignItems>(currentAlignItems); MarkSceneUpdated(); }
                        const char* flexAlignContent[] = { "Stretch", "FlexStart", "FlexEnd", "Center", "SpaceBetween", "SpaceAround", "SpaceEvenly" }; int currentAlignContent = static_cast<int>(flex->alignContent); ImGui::BeginDisabled(flex->wrap == FlexWrap::NoWrap); if (ImGui::Combo("Align Content", &currentAlignContent, flexAlignContent, IM_ARRAYSIZE(flexAlignContent))) { flex->alignContent = static_cast<AlignContent>(currentAlignContent); MarkSceneUpdated(); } ImGui::EndDisabled();
                        if (ImGui::DragFloat("Gap", &flex->gap, 0.5f, 0.0f, 1000.0f, "%.1f")) MarkSceneUpdated();
                        ImGui::Unindent();
                    }
                    else if (GridLayout* grid = dynamic_cast<GridLayout*>(s.layoutManager.get())) {
                        ImGui::TextUnformatted("Grid Options:"); ImGui::Indent();
                        bool gridChanged = false;
                        auto EditGridTracksUI = [&](const char* section_label, std::vector<GridTrackSize>& tracks) {
                            ImGui::SeparatorText(section_label);
                            int track_to_delete = -1;
                            for (int i = 0; i < tracks.size(); ++i) {
                                ImGui::PushID((section_label + std::to_string(i)).c_str());
                                auto& track = tracks[i];
                                const char* modes[] = { "Px", "Fr", "%", "Auto" };
                                int currentModeIndex = -1;
                    switch (track.mode) { case GridTrackSize::Mode::Fixed: currentModeIndex = 0; break; case GridTrackSize::Mode::Fraction: currentModeIndex = 1; break; case GridTrackSize::Mode::Percentage: currentModeIndex = 2; break; case GridTrackSize::Mode::Auto: currentModeIndex = 3; break; default: currentModeIndex = 3; break; }
                                                                         ImGui::SetNextItemWidth(ImGui::GetFontSize() * 4.5f);
                                                                         std::string combo_label = "##Mode_" + std::string(section_label) + "_" + std::to_string(i);
                                                                         if (ImGui::Combo(combo_label.c_str(), &currentModeIndex, modes, IM_ARRAYSIZE(modes))) {
                    switch (currentModeIndex) { case 0: track.mode = GridTrackSize::Mode::Fixed; if (track.value < 1) track.value = 100.0f; break; case 1: track.mode = GridTrackSize::Mode::Fraction; if (track.value < 0.1f) track.value = 1.0f; break; case 2: track.mode = GridTrackSize::Mode::Percentage; if (track.value < 1) track.value = 50.0f; break; case 3: track.mode = GridTrackSize::Mode::Auto; track.value = 0; break; }
                                                      gridChanged = true; MarkSceneUpdated();
                                                                         } ImGui::SameLine();
                                                                         ImGui::BeginDisabled(track.mode == GridTrackSize::Mode::Auto);
                                                                         ImGui::SetNextItemWidth(ImGui::GetFontSize() * 7);
                                                                         const char* format = (track.mode == GridTrackSize::Mode::Fraction) ? "%.2f" : "%.1f";
                                                                         float step = (track.mode == GridTrackSize::Mode::Fraction) ? 0.1f : 1.0f;
                                                                         float minVal = (track.mode == GridTrackSize::Mode::Fraction) ? 0.01f : 0.0f; float maxVal = (track.mode == GridTrackSize::Mode::Percentage) ? 100.0f : 99999.0f;
                                                                         std::string drag_label = "##Value_" + std::string(section_label) + "_" + std::to_string(i);
                                                                         if (ImGui::DragFloat(drag_label.c_str(), &track.value, step, minVal, maxVal, format)) {
                                                                             track.value = std::max(minVal, std::min(track.value, maxVal)); gridChanged = true; MarkSceneUpdated();
                                                                         } ImGui::EndDisabled(); ImGui::SameLine();
                                                                         std::string delete_label = "[X]##DeleteTrack_" + std::string(section_label) + "_" + std::to_string(i);
                                                                         if (ImGui::Button(delete_label.c_str())) { track_to_delete = i; gridChanged = true; } ImGui::SameLine(); ImGui::Text("Track %d", i + 1);
                                                                         ImGui::PopID();
                            }
                            if (track_to_delete != -1 && tracks.size() > 0) { tracks.erase(tracks.begin() + track_to_delete); MarkSceneUpdated(); }
                            std::string add_button_label = "+ Add " + std::string(section_label) + " Track";
                            if (ImGui::Button(add_button_label.c_str())) {
                                GridTrackSize newTrack;
                                newTrack.mode = GridTrackSize::Mode::Fraction;
                                newTrack.value = 1.0f;
                                tracks.push_back(newTrack);
                                gridChanged = true;
                                MarkSceneUpdated();
                            }
                            };
                        EditGridTracksUI("Columns", grid->templateColumns);
                        EditGridTracksUI("Rows", grid->templateRows);
                        ImGui::Separator();
                        auto EditLengthUnitUI = [&](const char* label, LengthUnit& lengthUnit) {
                            ImGui::PushID(label);
                            ImGui::AlignTextToFramePadding(); ImGui::TextUnformatted(label); ImGui::SameLine();
                            float availableWidth = ImGui::GetContentRegionAvail().x;
                            float comboWidth = ImGui::GetFontSize() * 4.5f;
                            float dragWidth = availableWidth - comboWidth - ImGui::GetStyle().ItemSpacing.x;
                            int currentUnitIndex = static_cast<int>(lengthUnit.unit);
                            const char* units[] = { "Px", "%" };
                            ImGui::SetNextItemWidth(comboWidth);
                            if (ImGui::Combo("##Unit", &currentUnitIndex, units, IM_ARRAYSIZE(units))) {
                                lengthUnit.unit = static_cast<LengthUnit::Unit>(currentUnitIndex);
                                if (lengthUnit.unit == LengthUnit::Unit::Percent && (lengthUnit.value < 0 || lengthUnit.value > 100)) lengthUnit.value = std::clamp(lengthUnit.value, 0.0f, 100.0f);
                                else if (lengthUnit.unit == LengthUnit::Unit::Px && lengthUnit.value < 0) lengthUnit.value = 0;
                                MarkSceneUpdated();
                            }
                            ImGui::SameLine();
                            ImGui::SetNextItemWidth(dragWidth > 0 ? dragWidth : -FLT_MIN);
                            float minVal = 0.0f; float maxVal = (lengthUnit.unit == LengthUnit::Unit::Percent) ? 100.0f : 99999.0f;
                            if (ImGui::DragFloat("##Value", &lengthUnit.value, 0.5f, minVal, maxVal, "%.1f")) {
                                lengthUnit.value = std::max(minVal, std::min(lengthUnit.value, maxVal));
                                MarkSceneUpdated();
                            }
                            ImGui::PopID();
                            };
                        ImGui::SeparatorText("Implicit & Gaps");
                        EditLengthUnitUI("Implicit Col Size", grid->implicitTrackColSize);
                        EditLengthUnitUI("Implicit Row Size", grid->implicitTrackRowSize);
                        EditLengthUnitUI("Column Gap", grid->columnGap);
                        EditLengthUnitUI("Row Gap", grid->rowGap);
                        ImGui::SeparatorText("Padding");
                        ImGui::SeparatorText("Alignment");
                        const char* gridFlows[] = { "Row", "Column", "RowDense", "ColumnDense" };
                        int currentFlow = static_cast<int>(grid->autoFlow);
                        if (ImGui::Combo("Auto Flow", &currentFlow, gridFlows, IM_ARRAYSIZE(gridFlows))) {
                            grid->autoFlow = static_cast<GridAutoFlow>(currentFlow);
                            MarkSceneUpdated();
                        }
                        const char* flexJustify[] = { "Start", "End", "Center", "SpaceBetween", "SpaceAround", "SpaceEvenly" }; int currentJustify = static_cast<int>(grid->justifyContent); if (ImGui::Combo("Justify Content (Grid)", &currentJustify, flexJustify, IM_ARRAYSIZE(flexJustify))) { grid->justifyContent = static_cast<JustifyContent>(currentJustify); MarkSceneUpdated(); }
                        if (ImGui::IsItemHovered()) ImGui::SetTooltip("How the entire grid is aligned horizontally if smaller than the container.");
                        const char* flexAlignContent[] = { "Start", "End", "Center", "Stretch", "SpaceBetween", "SpaceAround", "SpaceEvenly" }; int currentAlignContent = static_cast<int>(grid->alignContent); if (ImGui::Combo("Align Content (Grid)", &currentAlignContent, flexAlignContent, IM_ARRAYSIZE(flexAlignContent))) { grid->alignContent = static_cast<AlignContent>(currentAlignContent); MarkSceneUpdated(); }
                        if (ImGui::IsItemHovered()) ImGui::SetTooltip("How the entire grid is aligned vertically if smaller than the container.");
                        const char* gridAligns[] = { "Start", "End", "Center", "Stretch" };
                        int currentJustifyItems = static_cast<int>(grid->defaultCellContentJustify); if (ImGui::Combo("Default Cell Justify", &currentJustifyItems, gridAligns, IM_ARRAYSIZE(gridAligns))) { grid->defaultCellContentJustify = static_cast<GridAxisAlignment>(currentJustifyItems); MarkSceneUpdated(); }
                        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Default horizontal alignment of content within each grid cell.");
                        int currentAlignItems = static_cast<int>(grid->defaultCellContentAlign); if (ImGui::Combo("Default Cell Align", &currentAlignItems, gridAligns, IM_ARRAYSIZE(gridAligns))) { grid->defaultCellContentAlign = static_cast<GridAxisAlignment>(currentAlignItems); MarkSceneUpdated(); }
                        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Default vertical alignment of content within each grid cell.");
                        ImGui::Unindent();
                    }
                    else if (dynamic_cast<VerticalLayout*>(s.layoutManager.get()) || dynamic_cast<HorizontalLayout*>(s.layoutManager.get())) {
                        if (ImGui::DragFloat("Spacing", &s.layoutManager->spacing, 0.5f, 0.0f, 1000.0f, "%.1f")) MarkSceneUpdated();
                    }
                    if (ImGui::CollapsingHeader("Padding & Margin", ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        ImGui::BeginDisabled(shape_effectively_locked);
                        float padding_arr[4] = { s.padding.x, s.padding.y, s.padding.z, s.padding.w };
                        if (ImGui::DragFloat4("Padding (L,T,R,B)", padding_arr, 0.5f, 0.0f, 9999.0f, "%.1f")) {
                            s.padding = ImVec4(padding_arr[0], padding_arr[1], padding_arr[2], padding_arr[3]);
                            MarkSceneUpdated();
                        }
                        float margin_arr[4] = { s.margin.x, s.margin.y, s.margin.z, s.margin.w };
                        if (ImGui::DragFloat4("Margin (L,T,R,B)", margin_arr, 0.5f, -9999.0f, 9999.0f, "%.1f")) {
                            s.margin = ImVec4(margin_arr[0], margin_arr[1], margin_arr[2], margin_arr[3]);
                            MarkSceneUpdated();
                        }
                        ImGui::EndDisabled();
                    }
                }
                ImGui::EndDisabled();
                ImGui::EndDisabled();
            }

            bool parentIsFlex = s.parent && s.parent->isLayoutContainer && dynamic_cast<FlexLayout*>(s.parent->layoutManager.get());
            bool parentIsGrid = s.parent && s.parent->isLayoutContainer && dynamic_cast<GridLayout*>(s.parent->layoutManager.get());
            bool parentIsOldLayout = s.parent && s.parent->isLayoutContainer && (dynamic_cast<VerticalLayout*>(s.parent->layoutManager.get()) || dynamic_cast<HorizontalLayout*>(s.parent->layoutManager.get()));

            if (parentIsFlex || parentIsGrid || parentIsOldLayout)
            {
                if (ImGui::CollapsingHeader("Layout Item Settings", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    ImGui::BeginDisabled(shape_effectively_locked);
                    if (parentIsFlex) {
                        ImGui::TextUnformatted("Flex Item Options:"); ImGui::Indent();
                        if (ImGui::DragFloat("Flex Grow", &s.flexGrow, 0.1f, 0.0f, 100.0f, "%.1f")) MarkSceneUpdated();
                        if (ImGui::DragFloat("Flex Shrink", &s.flexShrink, 0.1f, 0.0f, 100.0f, "%.1f")) MarkSceneUpdated();
                        const char* basisModes[] = { "Auto", "Pixels", "Content", "Percentage" };
                        int currentModeIndex = -1;
                        switch (s.flexBasisMode) {
                        case ShapeItem::FlexBasisMode::Auto:        currentModeIndex = 0; break;
                        case ShapeItem::FlexBasisMode::Pixels:      currentModeIndex = 1; break;
                        case ShapeItem::FlexBasisMode::Content:     currentModeIndex = 2; break;
                        case ShapeItem::FlexBasisMode::Percentage:  currentModeIndex = 3; break;
                        }
                        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.5f - ImGui::GetStyle().ItemSpacing.x * 0.5f);
                        if (ImGui::Combo("Flex Basis Mode", &currentModeIndex, basisModes, IM_ARRAYSIZE(basisModes))) {
                            switch (currentModeIndex) {
                            case 0: s.flexBasisMode = ShapeItem::FlexBasisMode::Auto; break;
                            case 1: s.flexBasisMode = ShapeItem::FlexBasisMode::Pixels; break;
                            case 2: s.flexBasisMode = ShapeItem::FlexBasisMode::Content; break;
                            case 3: s.flexBasisMode = ShapeItem::FlexBasisMode::Percentage; break;
                            }
                            MarkSceneUpdated();
                        }
                        ImGui::SameLine();
                        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                        if (s.flexBasisMode == ShapeItem::FlexBasisMode::Pixels) {
                            if (ImGui::DragFloat("Pixels##Basis", &s.flexBasisPixels, 1.0f, 0.0f, 10000.0f, "%.0f px")) {
                                s.flexBasisPixels = std::max(0.0f, s.flexBasisPixels);
                                MarkSceneUpdated();
                            }
                        }
                        else if (s.flexBasisMode == ShapeItem::FlexBasisMode::Percentage) {
                            if (ImGui::DragFloat("Percent##Basis", &s.flexBasisPixels, 0.5f, 0.0f, 1000.0f, "%.1f %%")) {
                                s.flexBasisPixels = std::max(0.0f, s.flexBasisPixels);
                                MarkSceneUpdated();
                            }
                        }
                        else {
                            ImGui::BeginDisabled(true);
                            float dummy = 0.0f;
                            const char* label = (s.flexBasisMode == ShapeItem::FlexBasisMode::Content) ? "Value (Content)" : "Value (Auto)";
                            ImGui::DragFloat(label, &dummy, 1.0f);
                            if (ImGui::IsItemHovered()) ImGui::SetTooltip("Value is calculated automatically.");
                            ImGui::EndDisabled();
                        }
                        const char* flexAlignSelf[] = { "Auto", "Stretch", "FlexStart", "FlexEnd", "Center", "Baseline" };
                        int currentAlignSelf = static_cast<int>(s.alignSelf);
                        if (ImGui::Combo("Align Self", &currentAlignSelf, flexAlignSelf, IM_ARRAYSIZE(flexAlignSelf))) {
                            s.alignSelf = static_cast<AlignSelf>(currentAlignSelf); MarkSceneUpdated();
                        }
                        if (ImGui::InputInt("Order", &s.order)) {
                            MarkSceneUpdated();
                        }
                        if (ImGui::IsItemHovered()) {
                            ImGui::SetTooltip("Visual order of the item. Lower numbers appear first.");
                        }
                        ImGui::Unindent();
                    }
                    else if (parentIsGrid) {
                        ImGui::TextUnformatted("Grid Item Options:"); ImGui::Indent();
                        if (ImGui::InputInt("Column Start (1-based, -1=auto)", &s.gridColumnStart)) MarkSceneUpdated();
                        if (ImGui::InputInt("Column End (Line or Span>0, -1=auto)", &s.gridColumnEnd)) MarkSceneUpdated();
                        if (ImGui::InputInt("Row Start (1-based, -1=auto)", &s.gridRowStart)) MarkSceneUpdated();
                        if (ImGui::InputInt("Row End (Line or Span>0, -1=auto)", &s.gridRowEnd)) MarkSceneUpdated();
                        const char* gridAligns[] = { "Stretch", "Start", "End", "Center" }; int currentJustifySelf = static_cast<int>(s.justifySelf); if (ImGui::Combo("Justify Self", &currentJustifySelf, gridAligns, IM_ARRAYSIZE(gridAligns))) { s.justifySelf = static_cast<GridAxisAlignment>(currentJustifySelf); MarkSceneUpdated(); }
                        int currentAlignSelfGrid = static_cast<int>(s.alignSelfGrid); if (ImGui::Combo("Align Self", &currentAlignSelfGrid, gridAligns, IM_ARRAYSIZE(gridAligns))) { s.alignSelfGrid = static_cast<GridAxisAlignment>(currentAlignSelfGrid); MarkSceneUpdated(); }
                        ImGui::Unindent();
                    }
                    else if (parentIsOldLayout) {
                        ImGui::TextUnformatted("Legacy Layout Item Options:"); ImGui::Indent();
                        if (ImGui::DragFloat("Stretch Factor", &s.stretchFactor, 0.1f, 0.0f, 10000.0f)) { s.stretchFactor = std::max(0.0f, s.stretchFactor); MarkSceneUpdated(); }
                        const char* hAlignItems[] = { "Fill", "Left", "Center", "Right" }; int currentHAlign = static_cast<int>(s.horizontalAlignment); if (ImGui::Combo("Horizontal Align", &currentHAlign, hAlignItems, IM_ARRAYSIZE(hAlignItems))) { s.horizontalAlignment = static_cast<HAlignment>(currentHAlign); MarkSceneUpdated(); }
                        const char* vAlignItems[] = { "Fill", "Top", "Center", "Bottom" }; int currentVAlign = static_cast<int>(s.verticalAlignment); if (ImGui::Combo("Vertical Align", &currentVAlign, vAlignItems, IM_ARRAYSIZE(vAlignItems))) { s.verticalAlignment = static_cast<VAlignment>(currentVAlign); MarkSceneUpdated(); }
                        ImGui::Unindent();
                    }
                    ImGui::EndDisabled();
                }
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
                        shape->position.x += multiDeltaPos[0]; 
                        shape->position.y += multiDeltaPos[1];
                    }
                    MarkSceneUpdated();
                    multiDeltaPos[0] = multiDeltaPos[1] = 0.0f;
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
                        shape->size = targetBaseSize; 
                    }
                    MarkSceneUpdated();
                    multiDeltaSize[0] = multiDeltaSize[1] = 0.0f;
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

    void ShowUI_CodeGenerationPanel() {
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

    void ShowUI_ComponentPanel() {
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

    void ShowUI_LayerShapeManager_ChildWindowMappings()
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
                std::vector<ShapeItem*> allShapes_ptrs = GetAllShapes();
                int currentShapeIndex = -1;
                for (int i = 0; i < (int)allShapes_ptrs.size(); i++) {
                    if (allShapes_ptrs[i]->id == mapping.shapeId) {
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
                    }, static_cast<void*>(&allShapes_ptrs), (int)allShapes_ptrs.size()))
                {
                    if (currentShapeIndex >= 0) {
                        mapping.shapeId = allShapes_ptrs[currentShapeIndex]->id;
                        allShapes_ptrs[currentShapeIndex]->isChildWindow = true;
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
                    for (auto sh_ptr : GetAllShapes()) {
                        if (sh_ptr && std::find(availableChildWindows.begin(), availableChildWindows.end(), sh_ptr->name) == availableChildWindows.end()) {
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
                std::vector<ShapeItem*> allShapes_ptrs = GetAllShapes();
                int defaultShapeId = (!allShapes_ptrs.empty()) ? allShapes_ptrs[0]->id : 0;
                if (!allShapes_ptrs.empty()) allShapes_ptrs[0]->isChildWindow = true;
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

    bool VerticalSplitter(const char* str_id, float thickness, float height, float* size_left, float* size_right, float min_left, float min_right)
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

    void ShowUI(GLFWwindow* window) {
        if (ImGui::IsKeyPressed(ImGuiKey_Insert, false)) {
            DesignManager::g_IsInEditMode = !DesignManager::g_IsInEditMode;
            if (!DesignManager::g_IsInEditMode) {
                DesignManager::g_InteractionState = {};
            }
            DesignManager::MarkSceneUpdated();
        }
        DesignManager::ProcessCanvasInteractions();
        if (DesignManager::g_IsInEditMode) {
            ImDrawList* bgDrawList = ImGui::GetBackgroundDrawList();
            ImU32 dimColor = IM_COL32(0, 0, 0, 90);
            bgDrawList->AddRectFilled(ImVec2(0, 0), ImGui::GetIO().DisplaySize, dimColor);
        }
        if (DesignManager::g_IsInEditMode) {
            ImDrawList* fgDrawList = ImGui::GetForegroundDrawList();
            DesignManager::DrawInteractionGizmos(fgDrawList);
        }
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

    std::string SaveConfiguration() {
        return GenerateCodeForWindow(DesignManager::selectedGuiWindow);
    }

    void DrawAllForWindow(const std::string& windowName, const Layer& layer) {
        if (windowName == ImGui::GetCurrentWindow()->Name) {
            std::vector<Layer> sortedLayers = { layer };
            std::stable_sort(sortedLayers.begin(), sortedLayers.end(), CompareLayersByZOrder);
            for (auto& layer_ : sortedLayers) {
                if (!layer_.visible)
                    continue;
                for (auto& shape_uptr : layer_.shapes) {
                    if (shape_uptr && shape_uptr->parent == nullptr) {
                        DrawShape(ImGui::GetWindowDrawList(), *shape_uptr, ImGui::GetWindowPos());
                    }
                }
            }
        }
    }

    void RenderChildWindowForShape1()
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

    void RenderChildWindowForShape2()
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
    void RenderChildWindowForShape3()
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

    ShapeItem ShapeBuilder::build() {
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
        if (!shape.isLayoutContainer && shape.layoutManager != nullptr) {
            shape.layoutManager.reset();
        }
        return shape;
    }

/*
    void RenderIfAny_ButtonShapeWindow() {
        ::RenderIfAny_ButtonShapeWindow();
    }
*/

   
    void Init(int width, int height, GLFWwindow* window) {
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

    void GeneratedCode()
    {
        // Initialize the design before the main loop (while()). This enables real-time UI design modifications. Conversely, SetupProgrammaticUI_UltimateFreedom initializes after the loop, making its design non-adjustable during UI runtime.
    }











// Experimantal part for making these changes ::
/*






*/







    struct Vec3 {
        float x, y, z;
        Vec3(float _x = 0, float _y = 0, float _z = 0) : x(_x), y(_y), z(_z) {}
        Vec3 operator+(const Vec3& other) const { return Vec3(x + other.x, y + other.y, z + other.z); }
        Vec3 operator-(const Vec3& other) const { return Vec3(x - other.x, y - other.y, z - other.z); }
        Vec3 operator*(float s) const { return Vec3(x * s, y * s, z * s); }
    };

    struct Mat3x3 {
        float m[3][3];
        Mat3x3() { for (int i = 0; i < 3; ++i) for (int j = 0; j < 3; ++j) m[i][j] = (i == j) ? 1.0f : 0.0f; }
        static Mat3x3 RotationX(float angleRad) { Mat3x3 R; R.m[1][1] = cosf(angleRad); R.m[1][2] = -sinf(angleRad); R.m[2][1] = sinf(angleRad); R.m[2][2] = cosf(angleRad); return R; }
        static Mat3x3 RotationY(float angleRad) { Mat3x3 R; R.m[0][0] = cosf(angleRad); R.m[0][2] = sinf(angleRad); R.m[2][0] = -sinf(angleRad); R.m[2][2] = cosf(angleRad); return R; }
        static Mat3x3 RotationZ(float angleRad) { Mat3x3 R; R.m[0][0] = cosf(angleRad); R.m[0][1] = -sinf(angleRad); R.m[1][0] = sinf(angleRad); R.m[1][1] = cosf(angleRad); return R; }
        Mat3x3 operator*(const Mat3x3& other) const { Mat3x3 result; for (int i = 0; i < 3; ++i) { for (int j = 0; j < 3; ++j) { result.m[i][j] = 0; for (int k = 0; k < 3; ++k) { result.m[i][j] += m[i][k] * other.m[k][j]; } } }return result; }
        Vec3 operator*(const Vec3& v) const { return Vec3(m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z, m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z, m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z); }
    };

    float HueToRGB(float p, float q, float t) {
        if (t < 0.0f) t += 1.0f;
        if (t > 1.0f) t -= 1.0f;
        if (t < 1.0f / 6.0f) return p + (q - p) * 6.0f * t;
        if (t < 1.0f / 2.0f) return q;
        if (t < 2.0f / 3.0f) return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
        return p;
    }

    ImVec4 HSLtoRGB_custom(float h_degrees, float s_percent, float l_percent, float a = 1.0f) {
        float r, g, b;
        float h = h_degrees / 360.0f;
        float s = s_percent / 100.0f;
        float l = l_percent / 100.0f;

        if (s == 0.0f) {
            r = g = b = l;
        }
        else {
            float q = l < 0.5f ? l * (1.0f + s) : l + s - l * s;
            float p = 2.0f * l - q;
            r = HueToRGB(p, q, h + 1.0f / 3.0f);
            g = HueToRGB(p, q, h);
            b = HueToRGB(p, q, h - 1.0f / 3.0f);
        }
        return ImVec4(r, g, b, a);
    }

    ImVec4 HSLtoRGB(float h_degrees, float s_percent, float l_percent, float a = 1.0f) {
#if defined(IMGUI_VERSION_NUM) && IMGUI_VERSION_NUM >= 18200
        return HSLtoRGB_custom(h_degrees, s_percent, l_percent, a);
#else
        return HSLtoRGB_custom(h_degrees, s_percent, l_percent, a);
#endif
    }


    struct CubeFaceShapeData { 
        std::vector<ImVec2> screenVertices;
        ImVec4 color;
        float avgZ;
        int id;
        bool operator<(const CubeFaceShapeData& other) const { return avgZ > other.avgZ; }
    };

    
    void SetupProgrammaticUI_UltimateFreedom(float deltaTime, float totalTime) {
        using namespace DesignManager;

        
        ImVec2 mainWindowScreenPos = ImVec2(0, 0);
        ImVec2 mainWindowSize = ImGui::GetIO().DisplaySize;
        ImGuiWindow* pMainWindowImGui = ImGui::FindWindowByName("Main");
        if (pMainWindowImGui) {
            mainWindowScreenPos = pMainWindowImGui->Pos;
            mainWindowSize = pMainWindowImGui->Size;
        }

        WindowData& mainWindowData = GetOrCreateWindow("Main", true);
        Layer* myCustomLayer = GetOrCreateLayer(mainWindowData, "3DKupKatmani", 5);
        if (!myCustomLayer) {
            return;
        }
        Layer* myCustomLayer2 = GetOrCreateLayer(mainWindowData, "Yokaa", 5);
        if (!myCustomLayer2) {
            return;
        }

        static float currentHue = 170.0f;
        static bool isDraggingSliderKnob = false;

        
        float sliderRelativeOffsetX = 50.0f;
        float sliderRelativeOffsetY = mainWindowSize.y - 100.0f;
        ImVec2 sliderBaseRelativePos = ImVec2(sliderRelativeOffsetX, sliderRelativeOffsetY);
        float sliderWidth = 300.0f;
        ImVec2 sliderTrackSize = ImVec2(sliderWidth, 25.0f);
        float sliderTrackCornerRadius = sliderTrackSize.y / 2.0f;

        
        ShapeItem titleTextTemplate = ShapeBuilder()
            .setId(3001)
            .setName("TitleText_ColorPicker")
            .setOwnerWindow("Main")
            .setPosition(ImVec2(sliderBaseRelativePos.x, sliderBaseRelativePos.y - 40.0f))
            .setSize(ImVec2(sliderWidth, 30.0f))
            .setCornerRadius(0.0f)
            .setFillColor(ImVec4(0.0f, 0.0f, 0.0f, 0.0f))
            .setBorderThickness(0.0f)
            .setHasText(true)
            .setText("KÃ¼p Rotasyon & Renk")
            .setTextColor(ImVec4(0.9f, 0.9f, 0.9f, 1.0f))
            .setTextSize(20.0f)
            .setTextAlignment(0)
            .setZOrder(100)
            .build();
        ShapeItem* pTitleText = GetOrCreateShapeInLayer(*myCustomLayer, titleTextTemplate);
        if (pTitleText) {
            pTitleText->position = titleTextTemplate.position;
            pTitleText->basePosition = titleTextTemplate.position;
            pTitleText->textColor = titleTextTemplate.textColor;
            pTitleText->text = titleTextTemplate.text;
        }

        
        ShapeItem sliderTrackTemplate = ShapeBuilder()
            .setId(2001)
            .setName("HueSliderTrack_ColorPicker")
            .setOwnerWindow("Main")
            .setPosition(sliderBaseRelativePos)
            .setSize(sliderTrackSize)
            .setCornerRadius(sliderTrackCornerRadius)
            .setFillColor(HSLtoRGB(currentHue, 90.0f, 55.0f, 1.0f))
            .setBorderThickness(0.0f)
            .setZOrder(101)
            .build();
        ShapeItem* pSliderTrack = GetOrCreateShapeInLayer(*myCustomLayer, sliderTrackTemplate);
        if (pSliderTrack) {
            pSliderTrack->position = sliderTrackTemplate.position;
            pSliderTrack->basePosition = sliderTrackTemplate.position;
            pSliderTrack->fillColor = HSLtoRGB(currentHue, 90.0f, 55.0f, 1.0f);
        }

        
        ImVec2 sliderKnobSize = ImVec2(sliderTrackSize.y - 4.0f, sliderTrackSize.y - 4.0f);
        float knobMinX_relative_to_window = sliderBaseRelativePos.x + sliderKnobSize.x / 2.0f;
        float knobMaxX_relative_to_window = sliderBaseRelativePos.x + sliderTrackSize.x - sliderKnobSize.x / 2.0f;
        float draggableRange_relative = knobMaxX_relative_to_window - knobMinX_relative_to_window;
        if (draggableRange_relative < 1.0f) draggableRange_relative = 1.0f;

        float currentKnobCenterX_relative_to_window = knobMinX_relative_to_window + (currentHue / 360.0f) * draggableRange_relative;
        float currentKnobPosX_relative_to_window = currentKnobCenterX_relative_to_window - sliderKnobSize.x / 2.0f;
        float knobPosY_relative_to_window = sliderBaseRelativePos.y + (sliderTrackSize.y - sliderKnobSize.y) / 2.0f;

        ShapeItem sliderKnobTemplate = ShapeBuilder()
            .setId(2002)
            .setName("HueSliderKnob_ColorPicker")
            .setOwnerWindow("Main")
            .setPosition(ImVec2(currentKnobPosX_relative_to_window, knobPosY_relative_to_window))
            .setSize(sliderKnobSize)
            .setCornerRadius(sliderKnobSize.x / 2.0f)
            .setFillColor(ImVec4(1.0f, 1.0f, 1.0f, 1.0f))
            .setBorderColor(ImVec4(0.6f, 0.6f, 0.6f, 0.8f))
            .setBorderThickness(1.0f)
            .setZOrder(102)
            .build();
        ShapeItem* pSliderKnob = GetOrCreateShapeInLayer(*myCustomLayer, sliderKnobTemplate);
        if (pSliderKnob) {
            pSliderKnob->position = sliderKnobTemplate.position;
            pSliderKnob->basePosition = sliderKnobTemplate.position;
        }

        
        ImVec2 screenMousePos = ImGui::GetMousePos();
        if (pSliderTrack && pSliderKnob) {
            ImVec2 knobScreenPos_TL = ImVec2(mainWindowScreenPos.x + pSliderKnob->position.x,
                mainWindowScreenPos.y + pSliderKnob->position.y);
            ImVec2 trackScreenPos_TL = ImVec2(mainWindowScreenPos.x + pSliderTrack->position.x,
                mainWindowScreenPos.y + pSliderTrack->position.y);

            bool isMouseOverKnob = screenMousePos.x >= knobScreenPos_TL.x &&
                screenMousePos.x <= knobScreenPos_TL.x + pSliderKnob->size.x &&
                screenMousePos.y >= knobScreenPos_TL.y &&
                screenMousePos.y <= knobScreenPos_TL.y + pSliderKnob->size.y;

            bool isMouseOverTrack = screenMousePos.x >= trackScreenPos_TL.x &&
                screenMousePos.x <= trackScreenPos_TL.x + pSliderTrack->size.x &&
                screenMousePos.y >= trackScreenPos_TL.y - 5.0f &&
                screenMousePos.y <= trackScreenPos_TL.y + pSliderTrack->size.y + 5.0f;

            if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                if (isDraggingSliderKnob) {
                    float mouseX_clamped_to_draggable_range_abs_screen = ImClamp(screenMousePos.x,
                        mainWindowScreenPos.x + knobMinX_relative_to_window,
                        mainWindowScreenPos.x + knobMaxX_relative_to_window
                    );
                    float hue_val = ((mouseX_clamped_to_draggable_range_abs_screen - (mainWindowScreenPos.x + knobMinX_relative_to_window)) / draggableRange_relative) * 360.0f;
                    currentHue = ImClamp(hue_val, 0.0f, 359.9f);
                }
                else if (isMouseOverKnob || isMouseOverTrack) {
                    isDraggingSliderKnob = true;
                    float mouseX_clamped_to_draggable_range_abs_screen = ImClamp(screenMousePos.x,
                        mainWindowScreenPos.x + knobMinX_relative_to_window,
                        mainWindowScreenPos.x + knobMaxX_relative_to_window
                    );
                    float hue_val = ((mouseX_clamped_to_draggable_range_abs_screen - (mainWindowScreenPos.x + knobMinX_relative_to_window)) / draggableRange_relative) * 360.0f;
                    currentHue = ImClamp(hue_val, 0.0f, 359.9f);
                }
            }
            else {
                isDraggingSliderKnob = false;
            }
        }

        
        static std::vector<Vec3> cubeVerticesModel = {
            {-0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, -0.5f}, {0.5f, 0.5f, -0.5f}, {-0.5f, 0.5f, -0.5f},
            {-0.5f, -0.5f, 0.5f},  {0.5f, -0.5f, 0.5f},  {0.5f, 0.5f, 0.5f},  {-0.5f, 0.5f, 0.5f}
        };
        static const std::vector<std::vector<int>> cubeFaceIndices = {
            {0, 1, 2, 3}, {4, 5, 6, 7}, {0, 4, 7, 3}, {1, 5, 6, 2}, {3, 2, 6, 7}, {0, 1, 5, 4}
        };
        static const std::vector<ImVec4> cubeFaceBaseColors = {
            HSLtoRGB(0.0f, 70.0f, 50.0f, 0.8f), HSLtoRGB(120.0f, 70.0f, 50.0f, 0.8f), HSLtoRGB(240.0f, 70.0f, 50.0f, 0.8f),
            HSLtoRGB(60.0f, 70.0f, 50.0f, 0.8f), HSLtoRGB(300.0f, 70.0f, 50.0f, 0.8f), HSLtoRGB(180.0f, 70.0f, 50.0f, 0.8f)
        };

        
        float angleX_from_hue = DegToRad(currentHue * 0.5f);
        float angleY_from_hue = DegToRad(currentHue);
        float angleZ_dynamic = totalTime * 0.1f;

        Mat3x3 rotXMat = Mat3x3::RotationX(angleX_from_hue);
        Mat3x3 rotYMat = Mat3x3::RotationY(angleY_from_hue);
        Mat3x3 rotZMat = Mat3x3::RotationZ(angleZ_dynamic);
        Mat3x3 rotationMatrix = rotZMat * rotYMat * rotXMat;

        
        std::vector<Vec3> transformedVertices(8);
        for (size_t i = 0; i < cubeVerticesModel.size(); ++i) {
            transformedVertices[i] = rotationMatrix * cubeVerticesModel[i];
        }

        
        static ImVec2 FIXED_CUBE_CENTER_OFFSET = ImVec2(0.5f, 0.4f); 
        ImVec2 cubeCenterWindowRelative = ImVec2(
            mainWindowSize.x * FIXED_CUBE_CENTER_OFFSET.x,
            mainWindowSize.y * FIXED_CUBE_CENTER_OFFSET.y
        );

        
        std::vector<ImVec2> projectedVerticesWindowRelative(8);
        float visualScreenSizeFactor = 150.0f;
        float viewDistance = 2.5f;

        for (size_t i = 0; i < transformedVertices.size(); ++i) {
            Vec3 v = transformedVertices[i];
            float z_dist = v.z + viewDistance;
            if (z_dist <= 0.2f) z_dist = 0.2f;

            
            projectedVerticesWindowRelative[i].x = (v.x * visualScreenSizeFactor) / z_dist + cubeCenterWindowRelative.x;
            projectedVerticesWindowRelative[i].y = (v.y * visualScreenSizeFactor) / z_dist + cubeCenterWindowRelative.y;
        }

        
        std::vector<CubeFaceShapeData> facesToDraw;
        int baseCubeFaceId = 4000;

        for (size_t i = 0; i < cubeFaceIndices.size(); ++i) {
            CubeFaceShapeData faceData;
            faceData.id = baseCubeFaceId + static_cast<int>(i);
            float sumZ = 0.0f;

            for (int vertexIndex : cubeFaceIndices[i]) {
                faceData.screenVertices.push_back(projectedVerticesWindowRelative[vertexIndex]);
                sumZ += (transformedVertices[vertexIndex].z + viewDistance);
            }

            faceData.avgZ = sumZ / (float)faceData.screenVertices.size();
            faceData.color = HSLtoRGB(currentHue, 70.0f, 50.0f, cubeFaceBaseColors[i].w);
            facesToDraw.push_back(faceData);
        }

        
        std::sort(facesToDraw.begin(), facesToDraw.end());

        
        for (const auto& face : facesToDraw) {
            if (face.screenVertices.size() >= 3) {
                ShapeItem faceShapeTemplate;
                faceShapeTemplate.type = ShapeType::Rectangle;
                faceShapeTemplate.id = face.id;
                faceShapeTemplate.name = "CubeFace_" + std::to_string(face.id);
                faceShapeTemplate.ownerWindow = "Main";
                faceShapeTemplate.isPolygon = true;

                
                faceShapeTemplate.polygonVertices = face.screenVertices;

                faceShapeTemplate.fillColor = face.color;
                faceShapeTemplate.borderColor = ImVec4(0.05f, 0.05f, 0.05f, 0.7f);
                faceShapeTemplate.borderThickness = 1.0f;
                faceShapeTemplate.zOrder = (int)(&face - &facesToDraw[0]);

                
                if (!face.screenVertices.empty()) {
                    ImVec2 min_v = face.screenVertices[0], max_v = face.screenVertices[0];
                    for (size_t v_idx = 1; v_idx < face.screenVertices.size(); ++v_idx) {
                        min_v.x = ImMin(min_v.x, face.screenVertices[v_idx].x);
                        min_v.y = ImMin(min_v.y, face.screenVertices[v_idx].y);
                        max_v.x = ImMax(max_v.x, face.screenVertices[v_idx].x);
                        max_v.y = ImMax(max_v.y, face.screenVertices[v_idx].y);
                    }

                    
                    faceShapeTemplate.position = min_v;
                    faceShapeTemplate.size = ImVec2(max_v.x - min_v.x, max_v.y - min_v.y);
                }
                else {
                    faceShapeTemplate.position = ImVec2(0, 0);
                    faceShapeTemplate.size = ImVec2(0, 0);
                }

                
                faceShapeTemplate.basePosition = faceShapeTemplate.position;
                faceShapeTemplate.baseSize = faceShapeTemplate.size;

                ShapeItem* pFaceShape = GetOrCreateShapeInLayer(*myCustomLayer, faceShapeTemplate);
                if (pFaceShape) {
                    
                    pFaceShape->isPolygon = true;
                    pFaceShape->polygonVertices = face.screenVertices; 
                    pFaceShape->fillColor = face.color;
                    pFaceShape->borderColor = faceShapeTemplate.borderColor;
                    pFaceShape->borderThickness = faceShapeTemplate.borderThickness;
                    pFaceShape->zOrder = faceShapeTemplate.zOrder;

                    
                    pFaceShape->position = faceShapeTemplate.position;        
                    pFaceShape->size = faceShapeTemplate.size;
                    pFaceShape->basePosition = faceShapeTemplate.position;    
                    pFaceShape->baseSize = faceShapeTemplate.size;

                    
                    
                }
            }
        }

        ShapeBuilder()
            .setId(3861)
            .setName("New Shape")
            .setOwnerWindow("Main")
            .setGroupId(0)
            .setBasePosition(ImVec2(150.000000f, 150.000000f))
            .setBaseSize(ImVec2(150.000000f, 150.000000f))
            .setBaseRotation(0.000000f)
            .setPosition(ImVec2(150.000000f, 150.000000f))
            .setSize(ImVec2(150.000000f, 150.000000f))
            .setRotation(0.000000f)
            .setAnchorMode(DesignManager::ShapeItem::AnchorMode::None)
            .setAnchorMargin(ImVec2(0.000000f, 0.000000f))
            .setUsePercentagePos(false)
            .setPercentagePos(ImVec2(0.000000f, 0.000000f))
            .setUsePercentageSize(false)
            .setPercentageSize(ImVec2(10.000000f, 10.000000f))
            .setMinSize(ImVec2(0.000000f, 0.000000f))
            .setMaxSize(ImVec2(99999.000000f, 99999.000000f))
            .setCornerRadius(0.000000f)
            .setBorderThickness(0.000000f) 
            .setFillColor(ImVec4(0.933333f, 0.933333f, 0.933333f, 1.000000f))
            .setBorderColor(ImVec4(0.000000f, 0.000000f, 0.000000f, 0.000000f)) 
            .setUsePerSideBorderColors(true)
            .setBorderSidesColor(ImVec4(0.000000f, 0.000000f, 0.000000f, 0.800000f), ImVec4(0.000000f, 0.000000f, 0.000000f, 0.800000f), ImVec4(0.000000f, 0.000000f, 0.000000f, 0.800000f), ImVec4(0.000000f, 0.000000f, 0.000000f, 0.800000f))
            .setUsePerSideBorderThicknesses(true)
            .setBorderSidesThickness(63.200001f, 43.799999f, 20.500000f, 38.099998f)
            .setShadowColor(ImVec4(0.000000f, 0.000000f, 0.000000f, 0.000000f))
            .setShadowSpread(ImVec4(2.000000f, 2.000000f, 2.000000f, 2.000000f))
            .setShadowOffset(ImVec2(2.000000f, 2.000000f))
            .setShadowUseCornerRadius(true)
            .setShadowInset(false)
            .setShadowRotation(0.000000f)
            .setBlurAmount(0.000000f)
            .setVisible(true)
            .setLocked(false)
            .setUseGradient(false)
            .setGradientRotation(0.000000f)
            .setGradientInterpolation(DesignManager::ShapeItem::GradientInterpolation::Linear)
            .setColorRamp({
                {0.000000f, ImVec4(1.000000f, 1.000000f, 1.000000f, 1.000000f)},
                {1.000000f, ImVec4(0.500000f, 0.500000f, 0.500000f, 1.000000f)},
                })
                .setUseGlass(false)
            .setGlassBlur(10.000000f)
            .setGlassAlpha(0.700000f)
            .setGlassColor(ImVec4(1.000000f, 1.000000f, 1.000000f, 0.300000f))
            .setZOrder(0)
            .setIsChildWindow(false)
            .setChildWindowSync(false)
            .setToggleChildWindow(false)
            .setToggleBehavior(DesignManager::ChildWindowToggleBehavior::WindowOnly)
            .setChildWindowGroupId(-1)
            .setTargetShapeID(0)
            .setTriggerGroupID(0)
            .setIsImGuiContainer(false)
            .setIsButton(false)
            .setButtonBehavior(DesignManager::ShapeItem::ButtonBehavior::SingleClick)
            .setUseOnClick(false)
            .setHoverColor(ImVec4(0.800000f, 0.800000f, 0.800000f, 1.000000f))
            .setClickedColor(ImVec4(0.600000f, 0.600000f, 0.600000f, 1.000000f))
            .setToggledStatePositionOffset(ImVec2(0.000000f, 0.000000f))
            .setToggledStateSizeOffset(ImVec2(0.000000f, 0.000000f))
            .setToggledStateRotationOffset(0.000000f)
            .setHasText(false)
            .setText("")
            .setTextColor(ImVec4(0.000000f, 0.000000f, 0.000000f, 1.000000f))
            .setTextSize(16.000000f)
            .setTextFont(0)
            .setTextPosition(ImVec2(0.000000f, 0.000000f))
            .setTextRotation(0.000000f)
            .setTextAlignment(0)
            .setDynamicTextSize(true)
            .setBaseTextSize(0.000000f)
            .setMinTextSize(8.000000f)
            .setMaxTextSize(72.000000f)
            .setUpdateAnimBaseOnResize(false)
            .setHasEmbeddedImage(false)
            .setEmbeddedImageIndex(-1)
            .setAllowItemOverlap(false)
            .setForceOverlap(false)
            .setBlockUnderlying(true)
            .setType(ShapeType::Rectangle)
            .setPositioningMode(PositioningMode::Relative)
            .setFlexGrow(0.000000f)
            .setFlexShrink(1.000000f)
            .setFlexBasisMode(DesignManager::ShapeItem::FlexBasisMode::Auto)
            .setFlexBasisPixels(0.000000f)
            .setAlignSelf(DesignManager::AlignSelf::Auto)
            .setOrder(0)
            .setGridColumnStart(-1)
            .setGridColumnEnd(-1)
            .setGridRowStart(-1)
            .setGridRowEnd(-1)
            .setJustifySelf(DesignManager::GridAxisAlignment::Stretch)
            .setAlignSelfGrid(DesignManager::GridAxisAlignment::Stretch)
            .setIsLayoutContainer(false)
            .setStretchFactor(0.000000f)
            .setHorizontalAlignment(DesignManager::HAlignment::Fill)
            .setVerticalAlignment(DesignManager::VAlignment::Fill)
            .setBoxSizing(DesignManager::ShapeItem::BoxSizing::StrokeBox) 
            .setPadding(ImVec4(0.000000f, 0.000000f, 0.000000f, 0.000000f))
            .setMargin(ImVec4(0.000000f, 0.000000f, 0.000000f, 0.000000f))
            .setIsPolygon(false)
            .build() 
            ; 

    
        float requestedAlpha = 150.0f / 255.0f; 
        ShapeItem colorfulBorderShapeTemplate = ShapeBuilder()
            .setId(3862) 
            .setName("ColorfulBorderShape")
            .setOwnerWindow("Main") 
            .setPosition(ImVec2(mainWindowSize.x * 0.1f, mainWindowSize.y * 0.1f)) 
            .setSize(ImVec2(200.0f, 120.0f))
            .setCornerRadius(0.0f) 
            .setFillColor(ImVec4(0.25f, 0.25f, 0.3f, 0.6f)) 
            .setUsePerSideBorderThicknesses(true)
            .setBorderSidesThickness(5.0f, 10.0f, 15.0f, 20.0f) 
            .setUsePerSideBorderColors(true)
            .setBorderSidesColor(
                ImVec4(1.0f, 0.1f, 0.1f, requestedAlpha), 
                ImVec4(0.1f, 1.0f, 0.1f, requestedAlpha), 
                ImVec4(0.1f, 0.1f, 1.0f, requestedAlpha), 
                ImVec4(1.0f, 1.0f, 0.1f, requestedAlpha)  
            )
            
            .setBorderThickness(1.0f)
            .setBorderColor(ImVec4(0.0f, 0.0f, 0.0f, 0.0f))
            .setZOrder(120) 
            .setVisible(true)
            .setBoxSizing(DesignManager::ShapeItem::BoxSizing::BorderBox) 
            .build();

        ShapeItem* pColorfulBorderShape = GetOrCreateShapeInLayer(*myCustomLayer2, colorfulBorderShapeTemplate);
        if (pColorfulBorderShape) {
            
            *pColorfulBorderShape = colorfulBorderShapeTemplate;
        }
        
        
        MarkSceneUpdated();
    }
}
