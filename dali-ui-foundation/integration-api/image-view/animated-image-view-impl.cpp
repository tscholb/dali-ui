#include "animated-image-view-impl.h"
#include <dali-toolkit/devel-api/visuals/animated-image-visual-actions-devel.h>
#include <dali-toolkit/devel-api/visuals/image-visual-properties-devel.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>
#include <dali-ui-foundation/devel-api/visual-factory/visual-factory.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali/public-api/actors/custom-actor.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/object/base-handle.h>
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
  return AnimatedImageView::New();
}
// clang-format off
DALI_TYPE_REGISTRATION_BEGIN(AnimatedImageViewImpl, ViewImpl, Create)

DALI_PROPERTY_REGISTRATION(Ui::Integration, AnimatedImageViewImpl, "image", STRING, IMAGE)

DALI_TYPE_REGISTRATION_END()
// clang-format on

} // namespace

AnimatedImageViewImpl::AnimatedImageViewImpl()
: ViewImpl(),
  mUrl(),
  mLoopCount(-1),
  mImageColor(Color::WHITE)
{
}

AnimatedImageViewImpl::~AnimatedImageViewImpl()
{
}

Dali::String AnimatedImageViewImpl::GetUrl() const
{
  return mUrl;
}

Ui::AnimatedImageView AnimatedImageViewImpl::New()
{
  IntrusivePtr<AnimatedImageViewImpl> impl = new AnimatedImageViewImpl();
  Ui::AnimatedImageView               handle(*impl);
  impl->Initialize();
  return handle;
}

void AnimatedImageViewImpl::SetProperty(Dali::BaseObject* object, Dali::Property::Index index, const Dali::Property::Value& value)
{
  Ui::AnimatedImageView view = Ui::AnimatedImageView::DownCast(Dali::BaseHandle(object));
  if(view)
  {
    AnimatedImageViewImpl& impl = static_cast<AnimatedImageViewImpl&>(view.GetImplementation());
    switch(index)
    {
      case AnimatedImageViewImpl::Property::IMAGE:
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

Dali::Property::Value AnimatedImageViewImpl::GetProperty(Dali::BaseObject* object, Dali::Property::Index index)
{
  Dali::Property::Value value;
  Ui::AnimatedImageView view = Ui::AnimatedImageView::DownCast(Dali::BaseHandle(object));
  if(view)
  {
    AnimatedImageViewImpl& impl = static_cast<AnimatedImageViewImpl&>(view.GetImplementation());
    switch(index)
    {
      case AnimatedImageViewImpl::Property::IMAGE:
        value = impl.GetUrl();
        break;
    }
  }
  return value;
}

void AnimatedImageViewImpl::OnInitialize()
{
  ViewImpl::OnInitialize();
}

MeasuredSize AnimatedImageViewImpl::OnMeasure(float widthConstraint, float heightConstraint)
{
  // TODO: return natural size from animated image visual
  return ViewImpl::OnMeasure(widthConstraint, heightConstraint);
}

void AnimatedImageViewImpl::SetImage(const Dali::String& url)
{
  if(mUrl != url)
  {
    mUrl = url;
    UpdateVisual();
  }
}

void AnimatedImageViewImpl::Play()
{
  auto& viewData = Internal::ViewDataImpl::Get(*this);
  if(viewData.GetVisual(AnimatedImageViewImpl::Property::IMAGE))
  {
    viewData.DoAction(AnimatedImageViewImpl::Property::IMAGE, Toolkit::DevelAnimatedImageVisual::Action::PLAY, Dali::Property::Map());
  }
}

void AnimatedImageViewImpl::Pause()
{
  auto& viewData = Internal::ViewDataImpl::Get(*this);
  if(viewData.GetVisual(AnimatedImageViewImpl::Property::IMAGE))
  {
    viewData.DoAction(AnimatedImageViewImpl::Property::IMAGE, Toolkit::DevelAnimatedImageVisual::Action::PAUSE, Dali::Property::Map());
  }
}

void AnimatedImageViewImpl::Stop()
{
  auto& viewData = Internal::ViewDataImpl::Get(*this);
  if(viewData.GetVisual(AnimatedImageViewImpl::Property::IMAGE))
  {
    viewData.DoAction(AnimatedImageViewImpl::Property::IMAGE, Toolkit::DevelAnimatedImageVisual::Action::STOP, Dali::Property::Map());
  }
}

void AnimatedImageViewImpl::SetLoopCount(int count)
{
  if(mLoopCount != count)
  {
    mLoopCount = count;
    UpdateVisual();
  }
}

int AnimatedImageViewImpl::GetLoopCount() const
{
  return mLoopCount;
}

void AnimatedImageViewImpl::SetImageColor(const Vector4& color)
{
  if(mImageColor != color)
  {
    mImageColor = color;
    UpdateVisual();
  }
}

Vector4 AnimatedImageViewImpl::GetImageColor() const
{
  return mImageColor;
}

Ui::Visual::ResourceStatus AnimatedImageViewImpl::GetLoadingStatus() const
{
  return Internal::ViewDataImpl::Get(*this).GetVisualResourceStatus(AnimatedImageViewImpl::Property::IMAGE);
}

Ui::AnimatedImageView::ImageViewSignal& AnimatedImageViewImpl::ResourceReadySignal()
{
  return mResourceReadySignal;
}

Ui::AnimatedImageView::ImageViewSignal& AnimatedImageViewImpl::ResourceLoadedSignal()
{
  return mResourceLoadedSignal;
}

void AnimatedImageViewImpl::UpdateVisual()
{
  auto& viewData = Internal::ViewDataImpl::Get(*this);
  viewData.UnregisterVisual(AnimatedImageViewImpl::Property::IMAGE);
  if(mUrl.Empty()) return;

  Dali::Property::Map map;
  map.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::ANIMATED_IMAGE);
  map.Insert(Toolkit::ImageVisual::Property::URL, mUrl.CStr());

  if(mLoopCount != -1)
  {
    map.Insert(Toolkit::DevelImageVisual::Property::LOOP_COUNT, mLoopCount);
  }

  if(mImageColor != Color::WHITE)
  {
    map.Insert(Toolkit::Visual::Property::MIX_COLOR, mImageColor);
  }

  auto visualFactory = Ui::VisualFactory::Get();
  if(visualFactory)
  {
    auto visual = visualFactory.CreateVisual(map);
    if(visual)
    {
      viewData.RegisterVisual(AnimatedImageViewImpl::Property::IMAGE, visual);
    }
  }
}

} // namespace Integration
} // namespace Ui
} // namespace Dali
