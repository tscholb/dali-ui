# ImageView Architecture: NUI → dali-ui Migration

## 1. Class Hierarchy

### Before — NUI + dali-toolkit

```mermaid
classDiagram
    direction TB

    namespace TizenFX_NUI {
        class BaseHandle["BaseHandle (C#)"]
        class Container["Container (C#)"]
        class View_NUI["View (C#)"]
        class ImageView_NUI["ImageView (C#)"]
        class AnimatedImageView_NUI["AnimatedImageView (C#)\n⚠ ImageView 전체 API 상속"]
        class LottieAnimationView_NUI["LottieAnimationView (C#)"]
        class AnimatedVectorImageView_NUI["AnimatedVectorImageView (C#)\n⚠ LottieAnimationView 상속"]
    }

    BaseHandle <|-- Container
    Container <|-- View_NUI
    View_NUI <|-- ImageView_NUI
    ImageView_NUI <|-- AnimatedImageView_NUI
    View_NUI <|-- LottieAnimationView_NUI
    LottieAnimationView_NUI <|-- AnimatedVectorImageView_NUI

    namespace dali_toolkit {
        class Control["Control (C++)"]
        class TK_ImageView["ImageView (C++)"]
        class ImageVisual["ImageVisual"]
        class NPatchVisual["NPatchVisual"]
        class AnimatedImageVisual["AnimatedImageVisual"]
    }

    Control <|-- TK_ImageView
    TK_ImageView ..> ImageVisual : uses
    TK_ImageView ..> NPatchVisual : uses
    TK_ImageView ..> AnimatedImageVisual : uses

    ImageView_NUI ..> TK_ImageView : P/Invoke
    AnimatedImageView_NUI ..> TK_ImageView : P/Invoke
    LottieAnimationView_NUI ..> TK_ImageView : P/Invoke
```

---

### After — dali-ui-foundation

```mermaid
classDiagram
    direction TB

    namespace public_api {
        class View["View"]
        class ImageView_UI["ImageView"]
        class AnimatedImageView_UI["AnimatedImageView"]
        class LottieAnimationView_UI["LottieAnimationView"]
    }

    namespace integration_api {
        class ViewImpl["ViewImpl"]
        class ImageViewImpl["ImageViewImpl\n─────────────\nuses: ImageVisual\n      NPatchVisual"]
        class AnimatedImageViewImpl["AnimatedImageViewImpl\n─────────────\nuses: AnimatedImageVisual"]
        class LottieAnimationViewImpl["LottieAnimationViewImpl\n─────────────\nuses: LottieVisual"]
    }

    View <|-- ImageView_UI
    View <|-- AnimatedImageView_UI
    View <|-- LottieAnimationView_UI

    ViewImpl <|-- ImageViewImpl
    ViewImpl <|-- AnimatedImageViewImpl
    ViewImpl <|-- LottieAnimationViewImpl

    ImageView_UI *-- ImageViewImpl : impl
    AnimatedImageView_UI *-- AnimatedImageViewImpl : impl
    LottieAnimationView_UI *-- LottieAnimationViewImpl : impl
```

| | NUI + dali-toolkit | dali-ui-foundation |
|--|--------------------|--------------------|
| `AnimatedImageView` 부모 | `ImageView` (불필요한 API 전체 노출) | `View` (필요한 API만) |
| `AnimatedVectorImageView` 부모 | `LottieAnimationView` (중첩 상속) | `View` (독립) |
| Visual 사용 | toolkit ImageView 하나가 모든 Visual 담당 | 각 Impl이 전담 Visual만 사용 |
| 언어 경계 | C# / C / C++ (3개 언어) | C++ 단일 |

---

## 2. Before / After Architecture

```mermaid
flowchart LR
    subgraph BEFORE["❌ Before — NUI (C#) 기반"]
        direction TB
        A1["App (C#)"]
        A2["TizenFX / NUI\nImageView.cs\nBindableProperty\nPropertyMap cache"]
        A3["dali-csharp-binder\nP/Invoke wrapper (C ABI)"]
        A4["dali-toolkit\nImageViewImpl (C++)"]
        A5["dali-core\nActor / Visual / Loader"]

        A1-->|"C# 호출\n(GC, boxing)"| A2
        A2-->|"P/Invoke\n(marshalling)"| A3
        A3-->|"C ABI → C++"| A4
        A4-->|"PropertyMap\n(dictionary 탐색)"| A5
    end

    subgraph AFTER["✅ After — dali-ui (C++) 기반"]
        direction TB
        B1["App (C++)"]
        B2["dali-ui\nImageView (C++)\nFluent API / autogen.h\ntyped member variables"]
        B3["dali-toolkit\nVisual 레이어만 활용"]
        B4["dali-core\nActor / Visual / Loader"]

        B1-->|"직접 C++ 호출\n(zero overhead)"| B2
        B2-->|"Visual 생성 시점에만"| B3
        B3-->B4
    end

    style BEFORE fill:#F8D7DA,color:#333,stroke:#DC3545
    style AFTER fill:#D4EDDA,color:#333,stroke:#28A745
```

| | NUI | dali-ui |
|--|-----|---------|
| 호출 레이어 | 5단계 (App → NUI → Binder → Toolkit → Core) | 3단계 (App → dali-ui → Core) |
| Property 설정 | BindableProperty → P/Invoke → PropertyMap 딕셔너리 | 직접 typed 멤버 변수 할당 |
| 타입 안전성 | 런타임 (object boxing/unboxing) | 컴파일타임 |
| Fluent API | ❌ | ✅ `.SetFittingMode().SetSamplingMode().SetResourceUrl()` |
| C# 런타임 의존 | ✅ (필수) | ❌ (불필요) |

---

## 3. NUI → dali-ui 기능 대응표

### ImageView

| NUI API | dali-ui API | 상태 |
|---------|------------|------|
| `ResourceUrl` | `SetResourceUrl()` / `Reload()` | ✅ |
| `Image` (PropertyMap) | 개별 Set 메서드로 분리 | ✅ 타입 안전하게 개선 |
| `FittingMode` | `SetFittingMode()` | ✅ |
| `SamplingMode` | `SetSamplingMode()` | ✅ |
| `DesiredWidth/Height` | `SetDesiredSize()` | ✅ |
| `PixelArea` | `SetPixelArea()` | ✅ |
| `PlaceHolderUrl` | `SetPlaceholderUrl()` | ✅ |
| `AlphaMaskURL` | `SetAlphaMaskUrl()` | ✅ |
| `CropToMask` | `SetCropToMask()` | ✅ |
| `MaskingMode` | `SetMaskingMode()` | ✅ |
| `Border` | `SetBorder()` | ✅ N-Patch |
| `BorderOnly` | `SetBorderOnly()` | ✅ N-Patch |
| `ReleasePolicy` | `SetReleasePolicy()` | ✅ |
| `SynchronousLoading` | `SetSynchronousLoading()` | ✅ |
| `FastTrackUploading` | `SetFastTrackUploading()` | ✅ |
| `OrientationCorrection` | `SetOrientationCorrection()` | ✅ |
| `AdjustViewSize` | `SetFitSizeToImage()` | ✅ 명칭 개선 |
| `PreMultipliedAlpha` | `SetPreMultipliedAlpha()` | ✅ |
| `ImageColor` | `SetImageColor()` | ✅ UiColor 지원 추가 |
| `ResourceReadyEventHandler` | `ResourceReadySignal()` | ✅ |
| `ResourceLoadedEventHandler` | `ResourceLoadedSignal()` | ✅ |
| `WrapModeU/V` | — | ❌ 미지원 |
| `TransitionEffect` | — | ❌ 미지원 |

### AnimatedImageView

| NUI API | dali-ui API | 상태 |
|---------|------------|------|
| `ResourceUrl` | `SetResourceUrl()` | ✅ |
| `Play()` | `Play()` | ✅ |
| `Pause()` | `Pause()` | ✅ |
| `Stop()` | `Stop()` | ✅ |
| `LoopCount` | `SetLoopCount()` | ✅ |
| `ImageColor` | `SetImageColor()` | ✅ |
| `ResourceReadyEventHandler` | `ResourceReadySignal()` | ✅ |
| `CurrentFrameNumber` | — | ❌ 미지원 |
| `TotalFrameNumber` | — | ❌ 미지원 |

### LottieAnimationView (AnimatedVectorImageView)

| NUI API | dali-ui API | 상태 |
|---------|------------|------|
| `ResourceUrl` | `SetResourceUrl()` | ✅ |
| `Play()` | `Play()` | ✅ |
| `Pause()` | `Pause()` | ✅ |
| `Stop()` | `Stop()` | ✅ |
| `LoopCount` | `SetLoopCount()` | ✅ |
| `CurrentFrameNumber` | — | ❌ 미지원 |
| `TotalFrameNumber` | — | ❌ 미지원 |
| `SetMinMaxFrame()` | — | ❌ 미지원 |

---

## 4. Property 업데이트 흐름 비교 (ImageView 관점)

NUI와 dali-ui 모두 배칭(batching) 구조를 가지지만, 배칭 위치와 레이아웃 계산 주체가 다르다.

### Before — NUI ImageView

```mermaid
sequenceDiagram
    participant App
    participant NIV as NUI ImageView (C#)
    participant PC as ProcessorController
    participant Dali as Dali (C++)

    App->>NIV: resourceUrl = "cat.png"
    Note right of NIV: changedPropertyMap[URL]="cat.png"<br/>visualCreationRequiredFlag=true
    App->>NIV: fittingMode = SCALE_TO_FILL
    Note right of NIV: changedPropertyMap[FITTING]=SCALE_TO_FILL
    App->>NIV: samplingMode = BOX
    Note right of NIV: changedPropertyMap[SAMPLING]=BOX
    NIV->>PC: ReqeustProcessorOnceEvent()

    Note over App,Dali: ── 다음 프레임 ──

    PC->>NIV: UpdateVisualPropertyMap()
    NIV->>Dali: Interop: CreateVisual(cachedPropertyMap)
    Note right of Dali: 레이아웃 크기/위치는<br/>NUI Layout이 별도 처리
```

### After — dali-ui ImageView

```mermaid
sequenceDiagram
    participant App
    participant DIV as Dali-UI ImageView (C++)
    participant LC as LayoutController
    participant Dali as Dali (C++)

    App->>DIV: SetResourceUrl("cat.png")
    Note right of DIV: mVisualDirty=true
    App->>DIV: SetFittingMode(SCALE_TO_FILL)
    Note right of DIV: mVisualDirty=true
    App->>DIV: SetSamplingMode(BOX)
    Note right of DIV: mVisualDirty=true
    DIV->>LC: InvalidateMeasure()<br/>→ RequestLayout(this)

    Note over App,Dali: ── 다음 프레임 ──

    LC->>DIV: Measure(parentW, parentH)
    Note right of DIV: mVisualDirty==true<br/>→ UpdateVisual() 1회<br/>(natural size 계산 포함)
    DIV-->>LC: MeasuredSize(w, h)
    LC->>DIV: Arrange(bounds)
    DIV->>Dali: SetProperty(SIZE / POSITION)
    DIV->>Dali: ApplyFittingMode(visual transform)
```

| | NUI ImageView | dali-ui ImageView |
|---|---|---|
| **API** | property setter (index 기반, object boxing) | 타입 안전한 메서드 |
| **배칭 레이어** | C# (`ProcessorController`) | C++ (`LayoutController`) |
| **레이아웃 계산** | NUI Layout이 별도 처리 | `Measure` / `Arrange` 패스에서 natural size, aspect ratio 포함 처리 |
| **Visual 갱신** | `CreateVisual(PropertyMap)` | `UpdateVisual()` → `OnMeasure` 내에서 1회 |

> **참고:** 배칭 타이밍(다음 프레임 defer)은 양쪽 모두 동일하며 성능 차이는 미미하다.
> dali-ui의 실질적인 차이는 **레이아웃 계산 주체가 dali-ui로 내재화**된 것과 **타입 안전한 C++ API** 제공에 있다.
