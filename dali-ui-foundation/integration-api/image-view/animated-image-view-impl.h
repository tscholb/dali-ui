#pragma once

#include <dali-ui-foundation/integration-api/view-impl.h>
#include <dali-ui-foundation/public-api/image-view/animated-image-view.h>
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/signals/dali-signal.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

class DALI_UI_API AnimatedImageViewImpl : public ViewImpl
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
      IMAGE = PROPERTY_START_INDEX,
    };
  };

protected: // Construction & Destruction
  AnimatedImageViewImpl();
  virtual ~AnimatedImageViewImpl();

public: // Creation
  static Ui::AnimatedImageView New();

public: // Properties (required by DALI_PROPERTY_REGISTRATION)
  static void                  SetProperty(Dali::BaseObject* object, Dali::Property::Index index, const Dali::Property::Value& value);
  static Dali::Property::Value GetProperty(Dali::BaseObject* object, Dali::Property::Index index);

public: // API
  void         SetImage(const Dali::String& url);
  Dali::String GetUrl() const;
  void         Play();
  void         Pause();
  void         Stop();
  void         SetLoopCount(int count);
  int          GetLoopCount() const;

  void    SetImageColor(const Vector4& color);
  Vector4 GetImageColor() const;

public: // Loading Status & Signals
  Ui::Visual::ResourceStatus GetLoadingStatus() const;

  Ui::AnimatedImageView::ImageViewSignal& ResourceReadySignal();
  Ui::AnimatedImageView::ImageViewSignal& ResourceLoadedSignal();

private: // From ViewImpl
  void         OnInitialize() override;
  MeasuredSize OnMeasure(float widthConstraint, float heightConstraint) override;

private: // Internal methods
  void UpdateVisual();

private:
  // Not copyable or movable
  AnimatedImageViewImpl(const AnimatedImageViewImpl&)            = delete;
  AnimatedImageViewImpl(AnimatedImageViewImpl&&)                 = delete;
  AnimatedImageViewImpl& operator=(const AnimatedImageViewImpl&) = delete;
  AnimatedImageViewImpl& operator=(AnimatedImageViewImpl&&)      = delete;

private: // Data
  Dali::String                           mUrl;
  int                                    mLoopCount;
  Vector4                                mImageColor;
  Ui::AnimatedImageView::ImageViewSignal mResourceReadySignal;
  Ui::AnimatedImageView::ImageViewSignal mResourceLoadedSignal;
};

} // namespace Integration

inline Integration::AnimatedImageViewImpl& GetImpl(Ui::AnimatedImageView& obj)
{
  DALI_ASSERT_ALWAYS(obj);
  return static_cast<Integration::AnimatedImageViewImpl&>(obj.GetImplementation());
}
inline const Integration::AnimatedImageViewImpl& GetImpl(const Ui::AnimatedImageView& obj)
{
  DALI_ASSERT_ALWAYS(obj);
  return static_cast<const Integration::AnimatedImageViewImpl&>(obj.GetImplementation());
}

} // namespace Ui
} // namespace Dali
