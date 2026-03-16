#pragma once

#include <dali-ui-foundation/integration-api/view-impl.h>
#include <dali-ui-foundation/public-api/image-view/lottie-animation-view.h>
#include <dali/public-api/common/dali-string.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

class DALI_UI_API LottieAnimationViewImpl : public ViewImpl
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
  LottieAnimationViewImpl();
  virtual ~LottieAnimationViewImpl();

public: // Creation
  static Ui::LottieAnimationView New();

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

private: // From ViewImpl
  void         OnInitialize() override;
  MeasuredSize OnMeasure(float widthConstraint, float heightConstraint) override;

private: // Internal methods
  void UpdateVisual();

private:
  // Not copyable or movable
  LottieAnimationViewImpl(const LottieAnimationViewImpl&)            = delete;
  LottieAnimationViewImpl(LottieAnimationViewImpl&&)                 = delete;
  LottieAnimationViewImpl& operator=(const LottieAnimationViewImpl&) = delete;
  LottieAnimationViewImpl& operator=(LottieAnimationViewImpl&&)      = delete;

private: // Data
  Dali::String mUrl;
  int          mLoopCount;
};

} // namespace Integration

inline Integration::LottieAnimationViewImpl& GetImpl(Ui::LottieAnimationView& obj)
{
  DALI_ASSERT_ALWAYS(obj);
  return static_cast<Integration::LottieAnimationViewImpl&>(obj.GetImplementation());
}
inline const Integration::LottieAnimationViewImpl& GetImpl(const Ui::LottieAnimationView& obj)
{
  DALI_ASSERT_ALWAYS(obj);
  return static_cast<const Integration::LottieAnimationViewImpl&>(obj.GetImplementation());
}

} // namespace Ui
} // namespace Dali
