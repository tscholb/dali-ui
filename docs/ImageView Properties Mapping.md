# TizenFX (NUI) ImageView Properties Overview

The following properties have been comprehensively identified from `Tizen.NUI/BaseComponents/ImageView.cs` and mapped to their corresponding `dali-toolkit` targets. This serves as the functional baseline for the new `dali-ui-elements` explicit API design.

## 1. Core Properties
| NUI Property | C++ Property (`Toolkit::`) | Header File |
|---|---|---|
| `ResourceUrl` | `ImageVisual::Property::URL` | `public-api/visuals/image-visual-properties.h` |
| `PlaceHolderUrl` | `ImageView::Property::PLACEHOLDER_IMAGE` | `public-api/controls/image-view/image-view.h` |
| `PreMultipliedAlpha` | `ImageView::Property::PRE_MULTIPLIED_ALPHA` | `public-api/controls/image-view/image-view.h` |
| `PixelArea` | `ImageVisual::Property::PIXEL_AREA` | `public-api/visuals/image-visual-properties.h` |

## 2. Size & Fitting Control
| NUI Property | C++ Property (`Toolkit::`) | Header File |
|---|---|---|
| `FittingMode` | `ImageVisual::Property::FITTING_MODE` | `public-api/visuals/image-visual-properties.h` |
| `DesiredWidth`| `ImageVisual::Property::DESIRED_WIDTH` | `public-api/visuals/image-visual-properties.h` |
| `DesiredHeight`| `ImageVisual::Property::DESIRED_HEIGHT`| `public-api/visuals/image-visual-properties.h` |
| `WrapModeU` | `ImageVisual::Property::WRAP_MODE_U` | `public-api/visuals/image-visual-properties.h` |
| `WrapModeV` | `ImageVisual::Property::WRAP_MODE_V` | `public-api/visuals/image-visual-properties.h` |
| `SynchronousSizing`| `DevelImageVisual::Property::SYNCHRONOUS_SIZING`| `devel-api/visuals/image-visual-properties-devel.h` |
| `AdjustViewSize`| (Automatically resizes View based on original image's aspect ratio) | (NUI specific logic) |

## 3. Advanced Rendering & Masking
| NUI Property | C++ Property (`Toolkit::`) | Header File |
|---|---|---|
| `AlphaMaskURL` | `ImageVisual::Property::ALPHA_MASK_URL` | `public-api/visuals/image-visual-properties.h` |
| `ImageColor` | `Visual::Property::MIX_COLOR` | `public-api/visuals/visual-properties.h` |
| `CropToMask` | `ImageVisual::Property::CROP_TO_MASK` | `public-api/visuals/image-visual-properties.h` |
| `MaskingMode` | `DevelImageVisual::Property::MASKING_TYPE`| `devel-api/visuals/image-visual-properties-devel.h` |

## 4. Loading Behavior & Orientation
| NUI Property | C++ Property (`Toolkit::`) | Header File |
|---|---|---|
| `SynchronousLoading`| `ImageVisual::Property::SYNCHRONOUS_LOADING` | `public-api/visuals/image-visual-properties.h` |
| `FastTrackUploading`| `DevelImageVisual::Property::FAST_TRACK_UPLOADING` | `devel-api/visuals/image-visual-properties-devel.h` |
| `OrientationCorrection`| `ImageVisual::Property::ORIENTATION_CORRECTION` | `public-api/visuals/image-visual-properties.h` |

## 5. Loading Status / Events
*Note: These are handled via signals or getters, rather than straightforward Property sets.*
- `ResourceReady` (Event/Signal)
- `ResourceLoaded` (Event/Signal)
- `LoadingStatus` (Enum: Preparing, Ready, Failed) -> corresponds to `Visual::ResourceStatus`

## 6. Special Effects
| NUI Property | C++ Property (`Toolkit::`) | Header File |
|---|---|---|
| `TransitionEffect` | `ImageView::Property::ENABLE_TRANSITION_EFFECT` | `public-api/controls/image-view/image-view.h` |
| `TransitionEffectOption`| `ImageView::Property::TRANSITION_EFFECT_OPTION` | `public-api/controls/image-view/image-view.h` |
| `Border` | `ImageVisual::Property::BORDER` (N-patch only) | `public-api/visuals/image-visual-properties.h` |
| `BorderOnly` | `ImageVisual::Property::BORDER_ONLY` (N-patch only) | `public-api/visuals/image-visual-properties.h` |
