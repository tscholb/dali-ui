#include <dali-ui-foundation/integration-api/image-view/lottie-animation-view-impl.h>
#include <dali-ui-foundation/public-api/image-view/lottie-animation-view.h>
namespace Dali
{
namespace Ui
{
LottieAnimationView::LottieAnimationView()                                              = default;
LottieAnimationView::~LottieAnimationView()                                             = default;
LottieAnimationView::LottieAnimationView(const LottieAnimationView& rhs)                = default;
LottieAnimationView::LottieAnimationView(LottieAnimationView&& rhs) noexcept            = default;
LottieAnimationView& LottieAnimationView::operator=(const LottieAnimationView& rhs)     = default;
LottieAnimationView& LottieAnimationView::operator=(LottieAnimationView&& rhs) noexcept = default;
LottieAnimationView  LottieAnimationView::New(const Dali::String& url)
{
  LottieAnimationView view = Integration::LottieAnimationViewImpl::New();
  if(!url.Empty())
  {
    view.SetImage(url);
  }
  return view;
}
LottieAnimationView LottieAnimationView::DownCast(BaseHandle handle)
{
  return Ui::View::DownCast<LottieAnimationView, Ui::Integration::LottieAnimationViewImpl>(handle);
}
void LottieAnimationView::SetImage(const Dali::String& url)
{
  Ui::GetImpl(*this).SetImage(url);
}
void LottieAnimationView::Play()
{
  Ui::GetImpl(*this).Play();
}
void LottieAnimationView::Pause()
{
  Ui::GetImpl(*this).Pause();
}
void LottieAnimationView::Stop()
{
  Ui::GetImpl(*this).Stop();
}
LottieAnimationView::LottieAnimationView(Integration::LottieAnimationViewImpl& implementation)
: View(implementation)
{
}
LottieAnimationView::LottieAnimationView(Dali::Internal::CustomActor* internal)
: View(internal)
{
  VerifyCustomActorPointer<Integration::LottieAnimationViewImpl>(internal);
}
} //namespace Ui
} //namespace Dali
