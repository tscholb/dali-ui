#include "lottie-animation-view-impl.h"
#include <dali-toolkit/public-api/visuals/visual-properties.h>
#include <dali-ui-foundation/devel-api/visual-factory/visual-factory.h>
#include <dali/public-api/actors/custom-actor-impl.h>
#include <dali/public-api/actors/custom-actor.h>
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/object/type-registry.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

namespace
{
BaseHandle Create()
{
  return LottieAnimationView::New();
}
// clang-format off
DALI_TYPE_REGISTRATION_BEGIN(LottieAnimationViewImpl, ViewImpl, Create)

DALI_PROPERTY_REGISTRATION(Ui::Integration, LottieAnimationViewImpl, "image", STRING, IMAGE)

DALI_TYPE_REGISTRATION_END()
// clang-format on

} // namespace

LottieAnimationViewImpl::LottieAnimationViewImpl()
: ViewImpl(),
  mUrl(),
  mLoopCount(-1)
{
}

LottieAnimationViewImpl::~LottieAnimationViewImpl()
{
}

Dali::String LottieAnimationViewImpl::GetUrl() const
{
  return mUrl;
}

Ui::LottieAnimationView LottieAnimationViewImpl::New()
{
  IntrusivePtr<LottieAnimationViewImpl> impl = new LottieAnimationViewImpl();
  Ui::LottieAnimationView               handle(*impl);
  impl->Initialize();
  return handle;
}

void LottieAnimationViewImpl::SetProperty(Dali::BaseObject* object, Dali::Property::Index index, const Dali::Property::Value& value)
{
  Ui::LottieAnimationView view = Ui::LottieAnimationView::DownCast(Dali::BaseHandle(object));
  if(view)
  {
    LottieAnimationViewImpl& impl = static_cast<LottieAnimationViewImpl&>(view.GetImplementation());
    switch(index)
    {
      case LottieAnimationViewImpl::Property::IMAGE:
      {
        Dali::String url;
        if(value.Get(url))
        {
          impl.SetImage(url);
        }
        break;
      }
    }
  }
}

Dali::Property::Value LottieAnimationViewImpl::GetProperty(Dali::BaseObject* object, Dali::Property::Index index)
{
  Dali::Property::Value   value;
  Ui::LottieAnimationView view = Ui::LottieAnimationView::DownCast(Dali::BaseHandle(object));
  if(view)
  {
    LottieAnimationViewImpl& impl = static_cast<LottieAnimationViewImpl&>(view.GetImplementation());
    switch(index)
    {
      case LottieAnimationViewImpl::Property::IMAGE:
      {
        value = impl.GetUrl();
        break;
      }
    }
  }
  return value;
}

void LottieAnimationViewImpl::OnInitialize()
{
  ViewImpl::OnInitialize();
}

MeasuredSize LottieAnimationViewImpl::OnMeasure(float widthConstraint, float heightConstraint)
{
  // TODO: return natural size from lottie visual
  return ViewImpl::OnMeasure(widthConstraint, heightConstraint);
}

void LottieAnimationViewImpl::SetImage(const Dali::String& url)
{
  if(mUrl != url)
  {
    mUrl = url;
    UpdateVisual();
  }
}

void LottieAnimationViewImpl::Play()
{
  // TODO: trigger lottie play action
}

void LottieAnimationViewImpl::Pause()
{
  // TODO: trigger lottie pause action
}

void LottieAnimationViewImpl::Stop()
{
  // TODO: trigger lottie stop action
}

void LottieAnimationViewImpl::SetLoopCount(int count)
{
  mLoopCount = count;
}

int LottieAnimationViewImpl::GetLoopCount() const
{
  return mLoopCount;
}

void LottieAnimationViewImpl::UpdateVisual()
{
  // TODO: register a lottie visual
}

} // namespace Integration
} // namespace Ui
} // namespace Dali
