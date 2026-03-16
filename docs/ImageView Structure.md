# ImageView Structure

## 1. Before (dali-toolkit Architecture)

기존 `dali-toolkit` 구조에서는 베이스가 되는 `ImageView` 클래스가 `ImageVisual`, `AnimatedImageVisual`, `SvgVisual` 등 다양한 Visual들을 Property Map 값(`URL`이나 `TYPE`)에 따라 내부에서 동적으로 생성하고 범용적으로 처리하는 무거운 역할을 담당했습니다.

```mermaid
flowchart TD
    %% TizenFX (C#) Layer
    subgraph Tizen_NUI["Tizen.NUI (C#)"]
        direction TB
        NUI_View["View"]
        NUI_ImageView["ImageView"]
        
        subgraph NUI_Derived["Derived Classes"]
            direction LR
            NUI_AnimatedImageView["AnimatedImageView"]
            NUI_LottieAnimationView["LottieAnimationView"]
        end
        
        NUI_View -.->|inherits| NUI_ImageView
        NUI_ImageView -.->|inherits| NUI_AnimatedImageView
        NUI_ImageView -.->|inherits| NUI_LottieAnimationView
    end

    %% C# Binder & Property Layer
    subgraph Binder["dali-csharp-binder"]
        Interop["Interop Layer<br/>(Maps C# API to ToolKit Properties)"]
    end

    %% Toolkit (C++) Layer
    subgraph Toolkit["dali-toolkit (C++)"]
        direction TB
        TK_Control["Control (Base)"]
        TK_ImageView["ImageView (Derived)"]
        
        TK_Control -.->|inherits| TK_ImageView

        TK_Property["Property System<br/>(Mixed Control & Visual Data)"]
        
        subgraph Impl["Internal Impl Layer"]
            direction TB
            TK_Impl["ImageView::Impl<br/>(All-in-one Visual Manager)"]
        end
        
        %% Visuals managed by the single ImageView Impl
        subgraph Visuals["Dali Visuals"]
            direction LR
            V_Image["ImageVisual"]
            V_AnimImage["AnimatedImageVisual"]
            V_Vector["AnimatedVectorImageVisual"]
            V_Svg["SvgVisual"]
        end
    end

    NUI_ImageView --> Interop
    NUI_Derived --> Interop
    
    Interop --> TK_Property
    TK_Property -->|Apply PropertyMap| TK_ImageView
    TK_ImageView --> TK_Impl
    
    %% The monolithic ImageView Impl manages all visual types
    TK_Impl -->|Creates dynamically based on URL/Type| V_Image
    TK_Impl -->|Creates dynamically| V_AnimImage
    TK_Impl -->|Creates dynamically| V_Vector
    TK_Impl -->|Creates dynamically| V_Svg
```

---

## 2. After (dali-ui Architecture Plan)

새로운 `dali-ui` 구조에서는 애플리케이션 개발자가 C++ 단에서 명시적으로 사용하는 뷰(`ImageView`, `AnimatedImageView` 등)들이 1:1로 매칭되는 `Visual` 타입만을 전문적으로 취급하도록 `Impl` 클래스들이 확실하게 역할을 분담합니다.

```mermaid
flowchart TD
    %% Application Layer (Uses Dali-UI Directly)
    subgraph Application["Application (C++)"]
        App_UI["App UI Layer"]
    end

    %% Dali-UI Elements (Public Layer)
    subgraph Elements["dali-ui-elements (Public API)"]
        direction TB
        UI_View["View (Base)"]
        
        UI_ImageView["ImageView"]
        UI_AnimatedImageView["AnimatedImageView"]
        UI_LottieAnimationView["LottieAnimationView"]

        UI_View -.->|inherits| UI_ImageView
        UI_View -.->|inherits| UI_AnimatedImageView
        UI_View -.->|inherits| UI_LottieAnimationView
        
        UI_ExplicitAPI["Explicit C++ API<br/>(SetPixelArea, SetImage, etc.)"]
        
        UI_ImageView -.-> UI_ExplicitAPI
        UI_AnimatedImageView -.-> UI_ExplicitAPI
        UI_LottieAnimationView -.-> UI_ExplicitAPI
    end

    %% Dali-UI Foundation (Internal Core Layer)
    subgraph Foundation["dali-ui-foundation (Internal)"]
        direction TB
        F_ViewImpl["ViewImpl<br/>(Base Impl & Integration API)"]
        
        F_ImageViewImpl["ImageViewImpl"]
        F_AnimViewImpl["AnimatedImageViewImpl"]
        F_LottieImpl["LottieAnimationViewImpl"]
        
        F_ViewImpl -.->|inherits| F_ImageViewImpl
        F_ViewImpl -.->|inherits| F_AnimViewImpl
        F_ViewImpl -.->|inherits| F_LottieImpl

        F_PropertyUse["PropertyMap Builder<br/>(Engine Setup)"]
    end

    %% Engine Layer (Core / Adaptor / Visuals)
    subgraph Engine["Dali Core Engine & Visuals"]
        direction LR
        Core["dali-core / adaptor"]
        
        %% Specific 1:1 Visual Mapping
        V_Image["ImageVisual"]
        V_AnimImage["AnimatedImageVisual"]
        V_Vector["AnimatedVectorImageVisual"]
    end

    App_UI --> UI_View
    App_UI --> UI_ImageView
    App_UI --> UI_AnimatedImageView
    App_UI --> UI_LottieAnimationView

    %% Elements calls specific Impl directly
    UI_ExplicitAPI == "Delegates (Direct Call)" ==> F_ImageViewImpl
    UI_ExplicitAPI == "Delegates (Direct Call)" ==> F_AnimViewImpl
    UI_ExplicitAPI == "Delegates (Direct Call)" ==> F_LottieImpl
    
    %% Specific Impl manages specific Visual via PropertyMap
    F_ImageViewImpl -->|1:1 Specific Management| V_Image
    F_AnimViewImpl -->|1:1 Specific Management| V_AnimImage
    F_LottieImpl -->|1:1 Specific Management| V_Vector
    
    F_ImageViewImpl -.-> F_PropertyUse
    F_AnimViewImpl -.-> F_PropertyUse
    F_LottieImpl -.-> F_PropertyUse
    
    F_PropertyUse --> Core
```
