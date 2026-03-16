#pragma once

#include <dali-ui-foundation/public-api/view.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/signals/dali-signal.h>

namespace Dali
{
namespace Ui
{
namespace Integration DALI_INTERNAL
{
class AnimatedImageViewImpl;
}
class DALI_UI_API AnimatedImageView : public View
{
public:
  DALI_UI_CHAIN_VIEW_METHODS(AnimatedImageView)

  // Properties are managed internally in AnimatedImageViewImpl (integration-api)

  AnimatedImageView();
  static AnimatedImageView New(const Dali::String& url = "");
  ~AnimatedImageView();
  AnimatedImageView(const AnimatedImageView& rhs);
  AnimatedImageView(AnimatedImageView&& rhs) noexcept;
  AnimatedImageView&       operator=(const AnimatedImageView& rhs);
  AnimatedImageView&       operator=(AnimatedImageView&& rhs) noexcept;
  static AnimatedImageView DownCast(BaseHandle handle);
  AnimatedImageView&       SetImage(const Dali::String& url);
  Dali::String             GetUrl() const;
  AnimatedImageView&       Play();
  AnimatedImageView&       Pause();
  AnimatedImageView&       Stop();
  AnimatedImageView&       SetLoopCount(int count);
  int                      GetLoopCount() const;

  AnimatedImageView& SetImageColor(const Vector4& color);
  Vector4            GetImageColor() const;

public: // Loading Status & Signals
  using ImageViewSignal = Signal<void(AnimatedImageView)>;

  Ui::Visual::ResourceStatus GetLoadingStatus() const;

  ImageViewSignal& ResourceReadySignal();
  ImageViewSignal& ResourceLoadedSignal();

public:
  explicit DALI_INTERNAL AnimatedImageView(Integration::AnimatedImageViewImpl& implementation);
  explicit DALI_INTERNAL AnimatedImageView(Dali::Internal::CustomActor* internal);
};
} //namespace Ui
} //namespace Dali
