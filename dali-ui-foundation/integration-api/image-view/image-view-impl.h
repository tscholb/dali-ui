#pragma once

#include <dali-ui-foundation/devel-api/visual-factory/visual-base.h>
#include <dali-ui-foundation/integration-api/view-impl.h>
#include <dali-ui-foundation/public-api/image-view/image-view-types.h>
#include <dali-ui-foundation/public-api/image-view/image-view.h>
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/object/property-map.h>
#include <dali/public-api/signals/dali-signal.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

class DALI_UI_API ImageViewImpl : public ViewImpl
{
public: // Properties
  /// @brief The start and end property ranges for this impl.
  enum
  {
    PROPERTY_START_INDEX = Dali::PROPERTY_REGISTRATION_START_INDEX,
  };

  struct Property
  {
    enum
    {
      ///////////////////////////////////////////////////////////////////////////////
      // Event side (non-animatable) properties
      ///////////////////////////////////////////////////////////////////////////////

      IMAGE = PROPERTY_START_INDEX,
      FITTING_MODE,
      DESIRED_WIDTH,
      DESIRED_HEIGHT,
      IMAGE_COLOR,
      PRE_MULTIPLIED_ALPHA,
      PLACEHOLDER_IMAGE,
      WRAP_MODE_U,
      WRAP_MODE_V,
      SYNCHRONOUS_SIZING,
      ALPHA_MASK_URL,
      CROP_TO_MASK,
      MASKING_MODE,
      SYNCHRONOUS_LOADING,
      FAST_TRACK_UPLOADING,
      ORIENTATION_CORRECTION,
      BORDER,
      BORDER_ONLY,

      ///////////////////////////////////////////////////////////////////////////////
      // Animatable Properties
      ///////////////////////////////////////////////////////////////////////////////

      PIXEL_AREA = ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX,
    };
  };

protected: // Construction & Destruction
  ImageViewImpl();
  virtual ~ImageViewImpl();

public: // Creation
  static Ui::ImageView New();

public: // Properties (required by DALI_PROPERTY_REGISTRATION)
  static void                  SetProperty(Dali::BaseObject* object, Dali::Property::Index index, const Dali::Property::Value& value);
  static Dali::Property::Value GetProperty(Dali::BaseObject* object, Dali::Property::Index index);

public: // API
  void         SetImage(const Dali::String& url);
  Dali::String GetUrl() const;
  void         SetPixelArea(const Vector4& pixelArea);
  Vector4      GetPixelArea() const;
  void         SetPreMultipliedAlpha(bool preMultipled);
  bool         GetPreMultipliedAlpha() const;

  void         SetPlaceholderImage(const Dali::String& url);
  Dali::String GetPlaceholderImage() const;

  void                  SetFittingMode(Ui::FittingMode::Type fittingMode);
  Ui::FittingMode::Type GetFittingMode() const;
  void                  SetDesiredSize(Ui::ImageDimensions size);
  Ui::ImageDimensions   GetDesiredSize() const;
  void                  SetWrapModeU(Ui::WrapMode::Type wrapMode);
  Ui::WrapMode::Type    GetWrapModeU() const;
  void                  SetWrapModeV(Ui::WrapMode::Type wrapMode);
  Ui::WrapMode::Type    GetWrapModeV() const;
  void                  SetSynchronousSizing(bool synchronous);
  bool                  GetSynchronousSizing() const;

  void                  SetAlphaMaskUrl(const Dali::String& maskUrl);
  Dali::String          GetAlphaMaskUrl() const;
  void                  SetCropToMask(bool cropToMask);
  bool                  GetCropToMask() const;
  void                  SetMaskingMode(Ui::MaskingType::Type maskingMode);
  Ui::MaskingType::Type GetMaskingMode() const;
  void                  SetImageColor(const Vector4& color);
  Vector4               GetImageColor() const;

  void SetSynchronousLoading(bool synchronous);
  bool GetSynchronousLoading() const;
  void SetFastTrackUploading(bool fastTrack);
  bool GetFastTrackUploading() const;
  void SetOrientationCorrection(bool orientationCorrection);
  bool GetOrientationCorrection() const;

  void    SetBorder(const Vector4& border);
  Vector4 GetBorder() const;
  void    SetBorderOnly(bool borderOnly);
  bool    GetBorderOnly() const;

  void SetDepthIndex(int depthIndex);

public: // Loading Status & Signals
  Ui::Visual::ResourceStatus GetLoadingStatus() const;

  Ui::ImageView::ImageViewSignal& ResourceReadySignal();
  Ui::ImageView::ImageViewSignal& ResourceLoadedSignal();

private: // From ViewImpl
  void         OnInitialize() override;
  MeasuredSize OnMeasure(float widthConstraint, float heightConstraint) override;
  MeasuredSize OnArrange(const LayoutRect& bounds) override;

private: // Internal methods
  void UpdateVisual();
  void UpdatePlaceholderVisual();
  void ApplyFittingMode(const Vector2& size);
  void OnViewResourceReady(Ui::View view);

private:
  // Not copyable or movable
  ImageViewImpl(const ImageViewImpl&)            = delete;
  ImageViewImpl(ImageViewImpl&&)                 = delete;
  ImageViewImpl& operator=(const ImageViewImpl&) = delete;
  ImageViewImpl& operator=(ImageViewImpl&&)      = delete;

private: // Data
  Dali::String          mUrl;
  Vector4               mPixelArea;
  bool                  mPreMultipliedAlpha;
  Dali::String          mPlaceholderImageUrl;
  Ui::FittingMode::Type mFittingMode;
  Ui::ImageDimensions   mDesiredSize;
  Ui::WrapMode::Type    mWrapModeU;
  Ui::WrapMode::Type    mWrapModeV;
  bool                  mSynchronousSizing;
  Dali::String          mAlphaMaskUrl;
  bool                  mCropToMask;
  Ui::MaskingType::Type mMaskingMode;
  Vector4               mImageColor;
  bool                  mSynchronousLoading;
  bool                  mFastTrackUploading;
  bool                  mOrientationCorrection;
  Vector4               mBorder;
  bool                  mBorderOnly;
  int                   mDepthIndex;
  bool                  mVisualDirty;

  Ui::ImageView::ImageViewSignal mResourceReadySignal;
  Ui::ImageView::ImageViewSignal mResourceLoadedSignal;

  Ui::Visual::Base mVisual;
};

typedef IntrusivePtr<ImageViewImpl> ImageViewImplPtr;

} // namespace Integration

inline Integration::ImageViewImpl& GetImpl(Ui::ImageView& obj)
{
  DALI_ASSERT_ALWAYS(obj);
  return static_cast<Integration::ImageViewImpl&>(obj.GetImplementation());
}

inline const Integration::ImageViewImpl& GetImpl(const Ui::ImageView& obj)
{
  DALI_ASSERT_ALWAYS(obj);
  return static_cast<const Integration::ImageViewImpl&>(obj.GetImplementation());
}

} // namespace Ui
} // namespace Dali
