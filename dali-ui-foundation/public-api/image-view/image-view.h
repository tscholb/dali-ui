#pragma once

#include <dali-ui-foundation/public-api/image-view/image-view-types.h>
#include <dali-ui-foundation/public-api/view.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/object/property-map.h>
#include <dali/public-api/signals/dali-signal.h>

namespace Dali
{
namespace Ui
{
namespace Integration DALI_INTERNAL
{
class ImageViewImpl;
}

class DALI_UI_API ImageView : public View
{
public:
  DALI_UI_CHAIN_VIEW_METHODS(ImageView)

  /**
   * @brief Assigns this ImageView instance to a target variable.
   * @param[out] self The target variable to assign this to
   * @return Reference to this ImageView for fluent chaining
   */
  ImageView& As(ImageView& self)
  {
    self = *this;
    return *this;
  }

  // Properties are managed internally in ImageViewImpl (integration-api)

  ImageView();
  static ImageView New();
  static ImageView New(const Dali::String& url);
  ~ImageView();
  ImageView(const ImageView& imageView);
  ImageView(ImageView&& rhs) noexcept;
  ImageView&       operator=(const ImageView& imageView);
  ImageView&       operator=(ImageView&& rhs) noexcept;
  static ImageView DownCast(BaseHandle handle);

  ImageView&   SetImage(const Dali::String& url);
  Dali::String GetUrl() const;
  ImageView&   SetPixelArea(const Vector4& pixelArea);
  Vector4      GetPixelArea() const;
  ImageView&   SetPreMultipliedAlpha(bool preMultipled);
  bool         GetPreMultipliedAlpha() const;

  ImageView&   SetPlaceholderImage(const Dali::String& url);
  Dali::String GetPlaceholderImage() const;

  // Size & Fitting Control
  ImageView&            SetFittingMode(Ui::FittingMode::Type fittingMode);
  Ui::FittingMode::Type GetFittingMode() const;
  ImageView&            SetDesiredSize(Ui::ImageDimensions size);
  Ui::ImageDimensions   GetDesiredSize() const;
  ImageView&            SetWrapModeU(Ui::WrapMode::Type wrapMode);
  Ui::WrapMode::Type    GetWrapModeU() const;
  ImageView&            SetWrapModeV(Ui::WrapMode::Type wrapMode);
  Ui::WrapMode::Type    GetWrapModeV() const;
  ImageView&            SetSynchronousSizing(bool synchronous);
  bool                  GetSynchronousSizing() const;

  // Advanced Rendering & Masking
  ImageView&            SetAlphaMaskUrl(const Dali::String& maskUrl);
  Dali::String          GetAlphaMaskUrl() const;
  ImageView&            SetCropToMask(bool cropToMask);
  bool                  GetCropToMask() const;
  ImageView&            SetMaskingMode(Ui::MaskingType::Type maskingMode);
  Ui::MaskingType::Type GetMaskingMode() const;
  ImageView&            SetImageColor(const Vector4& color);
  Vector4               GetImageColor() const;

  // Loading Behavior & Synchronous
  ImageView& SetSynchronousLoading(bool synchronous);
  bool       GetSynchronousLoading() const;
  ImageView& SetFastTrackUploading(bool fastTrack);
  bool       GetFastTrackUploading() const;
  ImageView& SetOrientationCorrection(bool orientationCorrection);
  bool       GetOrientationCorrection() const;

  // N-Patch Border
  ImageView& SetBorder(const Vector4& border);
  Vector4    GetBorder() const;
  ImageView& SetBorderOnly(bool borderOnly);
  bool       GetBorderOnly() const;

public: // Loading Status & Signals
  using ImageViewSignal = Signal<void(ImageView)>;

  Ui::Visual::ResourceStatus GetLoadingStatus() const;

  ImageViewSignal& ResourceReadySignal();
  ImageViewSignal& ResourceLoadedSignal();

public:
  explicit DALI_INTERNAL ImageView(Integration::ImageViewImpl& implementation);
  explicit DALI_INTERNAL ImageView(Dali::Internal::CustomActor* internal);
};

} // namespace Ui
} // namespace Dali
