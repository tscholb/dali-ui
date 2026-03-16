#include <dali-ui-foundation/integration-api/image-view/animated-image-view-impl.h>
#include <dali-ui-foundation/public-api/image-view/animated-image-view.h>
#include <dali-ui-foundation/public-api/ui-color.h>
namespace Dali
{
namespace Ui
{
AnimatedImageView::AnimatedImageView()                                            = default;
AnimatedImageView::~AnimatedImageView()                                           = default;
AnimatedImageView::AnimatedImageView(const AnimatedImageView& rhs)                = default;
AnimatedImageView::AnimatedImageView(AnimatedImageView&& rhs) noexcept            = default;
AnimatedImageView& AnimatedImageView::operator=(const AnimatedImageView& rhs)     = default;
AnimatedImageView& AnimatedImageView::operator=(AnimatedImageView&& rhs) noexcept = default;
AnimatedImageView  AnimatedImageView::New(const Dali::String& url)
{
  AnimatedImageView view = Integration::AnimatedImageViewImpl::New();
  if(!url.Empty())
  {
    view.SetImage(url);
  }
  return view;
}
AnimatedImageView AnimatedImageView::DownCast(BaseHandle handle)
{
  return Ui::View::DownCast<AnimatedImageView, Ui::Integration::AnimatedImageViewImpl>(handle);
}
AnimatedImageView& AnimatedImageView::SetImage(const Dali::String& url)
{
  Ui::GetImpl(*this).SetImage(url);
  return *this;
}
Dali::String AnimatedImageView::GetUrl() const
{
  return Ui::GetImpl(*this).GetUrl();
}
AnimatedImageView& AnimatedImageView::Play()
{
  Ui::GetImpl(*this).Play();
  return *this;
}
AnimatedImageView& AnimatedImageView::Pause()
{
  Ui::GetImpl(*this).Pause();
  return *this;
}
AnimatedImageView& AnimatedImageView::Stop()
{
  Ui::GetImpl(*this).Stop();
  return *this;
}
AnimatedImageView& AnimatedImageView::SetLoopCount(int count)
{
  Ui::GetImpl(*this).SetLoopCount(count);
  return *this;
}
int AnimatedImageView::GetLoopCount() const
{
  return Ui::GetImpl(*this).GetLoopCount();
}
AnimatedImageView& AnimatedImageView::SetImageColor(const UiColor& color)
{
  Ui::GetImpl(*this).SetImageColor(color);
  return *this;
}
UiColor AnimatedImageView::GetImageColor() const
{
  return Ui::GetImpl(*this).GetImageColor();
}
Ui::Visual::ResourceStatus AnimatedImageView::GetLoadingStatus() const
{
  return Ui::GetImpl(*this).GetLoadingStatus();
}
AnimatedImageView::ImageViewSignal& AnimatedImageView::ResourceReadySignal()
{
  return Ui::GetImpl(*this).ResourceReadySignal();
}
AnimatedImageView::ImageViewSignal& AnimatedImageView::ResourceLoadedSignal()
{
  return Ui::GetImpl(*this).ResourceLoadedSignal();
}
AnimatedImageView::AnimatedImageView(Integration::AnimatedImageViewImpl& implementation)
: View(implementation)
{
}
AnimatedImageView::AnimatedImageView(Dali::Internal::CustomActor* internal)
: View(internal)
{
  VerifyCustomActorPointer<Integration::AnimatedImageViewImpl>(internal);
}
} //namespace Ui
} //namespace Dali
