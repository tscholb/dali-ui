#pragma once

#include <dali-ui-foundation/public-api/view.h>
#include <dali/public-api/common/dali-string.h>
namespace Dali
{
namespace Ui
{
namespace Integration DALI_INTERNAL
{
class LottieAnimationViewImpl;
}
class DALI_UI_API LottieAnimationView : public View
{
public:
  // Properties are managed internally in LottieAnimationViewImpl (integration-api)

  LottieAnimationView();
  static LottieAnimationView New(const Dali::String& url = "");
  ~LottieAnimationView();
  LottieAnimationView(const LottieAnimationView& rhs);
  LottieAnimationView(LottieAnimationView&& rhs) noexcept;
  LottieAnimationView&       operator=(const LottieAnimationView& rhs);
  LottieAnimationView&       operator=(LottieAnimationView&& rhs) noexcept;
  static LottieAnimationView DownCast(BaseHandle handle);
  void                       SetImage(const Dali::String& url);
  void                       Play();
  void                       Pause();
  void                       Stop();

public:
  explicit DALI_INTERNAL LottieAnimationView(Integration::LottieAnimationViewImpl& implementation);
  explicit DALI_INTERNAL LottieAnimationView(Dali::Internal::CustomActor* internal);
};
} //namespace Ui
} //namespace Dali
