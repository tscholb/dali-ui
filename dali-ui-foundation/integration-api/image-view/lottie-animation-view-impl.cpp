/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

// CLASS HEADER
#include "lottie-animation-view-impl.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/devel-api/object/type-registry.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/object/property-array.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/visual-factory/visual-factory.h>
#include <dali-ui-foundation/devel-api/visuals/animated-vector-image-visual-actions-devel.h>
#include <dali-ui-foundation/devel-api/visuals/animated-vector-image-visual-signals-devel.h>
#include <dali-ui-foundation/devel-api/visuals/image-visual-properties-devel.h>
#include <dali-ui-foundation/devel-api/visuals/visual-actions-devel.h>
#include <dali-ui-foundation/devel-api/visuals/visual-properties-devel.h>
#include <dali-ui-foundation/integration-api/property-registration-helper.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/public-api/ui-color.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>

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
  LottieAnimationViewImplPtr impl = LottieAnimationViewImpl::New();
  Ui::View                   view(*impl);
  impl->Initialize();
  return view;
}
// clang-format off
#define LOTTIE_ANIMATION_VIEW_PROPERTY_REGISTRATION(text, valueType, enumIndex) \
  DALI_PROPERTY_REGISTRATION_EXTERNAL(Ui, LottieAnimationViewPropertyIndex, Ui::Integration, LottieAnimationViewImpl, text, valueType, enumIndex)

DALI_TYPE_REGISTRATION_BEGIN(LottieAnimationViewImpl, ViewImpl, Create)

LOTTIE_ANIMATION_VIEW_PROPERTY_REGISTRATION("image",                   STRING,  IMAGE)
LOTTIE_ANIMATION_VIEW_PROPERTY_REGISTRATION("loopCount",               INTEGER, LOOP_COUNT)
LOTTIE_ANIMATION_VIEW_PROPERTY_REGISTRATION("loopingMode",             INTEGER, LOOPING_MODE)
LOTTIE_ANIMATION_VIEW_PROPERTY_REGISTRATION("stopBehavior",            INTEGER, STOP_BEHAVIOR)
LOTTIE_ANIMATION_VIEW_PROPERTY_REGISTRATION("frameSpeedFactor",        FLOAT,   FRAME_SPEED_FACTOR)
LOTTIE_ANIMATION_VIEW_PROPERTY_REGISTRATION("imageColor",              VECTOR4, IMAGE_COLOR)
LOTTIE_ANIMATION_VIEW_PROPERTY_REGISTRATION("desiredWidth",            INTEGER, DESIRED_WIDTH)
LOTTIE_ANIMATION_VIEW_PROPERTY_REGISTRATION("desiredHeight",           INTEGER, DESIRED_HEIGHT)
LOTTIE_ANIMATION_VIEW_PROPERTY_REGISTRATION("releasePolicy",           INTEGER, RELEASE_POLICY)
LOTTIE_ANIMATION_VIEW_PROPERTY_REGISTRATION("synchronousLoading",      BOOLEAN, SYNCHRONOUS_LOADING)
LOTTIE_ANIMATION_VIEW_PROPERTY_REGISTRATION("preMultipliedAlpha",      BOOLEAN, PRE_MULTIPLIED_ALPHA)
LOTTIE_ANIMATION_VIEW_PROPERTY_REGISTRATION("redrawInScalingDown",     BOOLEAN, REDRAW_IN_SCALING_DOWN)
LOTTIE_ANIMATION_VIEW_PROPERTY_REGISTRATION("redrawInScalingUp",       BOOLEAN, REDRAW_IN_SCALING_UP)
LOTTIE_ANIMATION_VIEW_PROPERTY_REGISTRATION("enableFrameCache",        BOOLEAN, ENABLE_FRAME_CACHE)
LOTTIE_ANIMATION_VIEW_PROPERTY_REGISTRATION("notifyAfterRasterization",BOOLEAN, NOTIFY_AFTER_RASTERIZATION)

DALI_TYPE_REGISTRATION_END()
#undef LOTTIE_ANIMATION_VIEW_PROPERTY_REGISTRATION
// clang-format on
} // namespace

LottieAnimationViewImpl::LottieAnimationViewImpl()
: ViewImpl(),
  mUrl(),
  mLoopCount(-1),
  mStopBehavior(LottieAnimationView::StopBehavior::CURRENT_FRAME),
  mLoopingMode(LottieAnimationView::LoopingMode::RESTART),
  mFrameSpeedFactor(1.0f),
  mPlayRangeType(PlayRangeType::NONE),
  mMinFrame(0),
  mMaxFrame(0),
  mMarker1(),
  mMarker2(),
  mRedrawInScalingDown(true),
  mRedrawInScalingUp(true),
  mEnableFrameCache(false),
  mNotifyAfterRasterization(false),
  mDesiredWidth(0),
  mDesiredHeight(0),
  mReleasePolicy(Ui::ReleasePolicy::DETACHED),
  mSynchronousLoading(false),
  mPreMultipliedAlpha(false),
  mImageColor(Color::WHITE),
  mAnimationFinishedSignal()
{
}

LottieAnimationViewImpl::~LottieAnimationViewImpl()
{
}

LottieAnimationViewImplPtr LottieAnimationViewImpl::New()
{
  return new LottieAnimationViewImpl();
}

void LottieAnimationViewImpl::SetProperty(Dali::BaseObject* object, Dali::Property::Index index, const Dali::Property::Value& value)
{
  Ui::View view = Ui::View::DownCast(Dali::BaseHandle(object));
  if(view)
  {
    LottieAnimationViewImpl& impl = static_cast<LottieAnimationViewImpl&>(GetImpl(view));
    switch(index)
    {
      case LottieAnimationViewImpl::Property::IMAGE:
      {
        Dali::String url;
        if(value.Get(url))
        {
          impl.SetResourceUrl(url);
        }
        break;
      }
      case LottieAnimationViewImpl::Property::LOOP_COUNT:
      {
        int count;
        if(value.Get(count))
        {
          impl.SetLoopCount(count);
        }
        break;
      }
      case LottieAnimationViewImpl::Property::LOOPING_MODE:
      {
        int mode;
        if(value.Get(mode))
        {
          impl.SetLoopingMode(static_cast<LottieAnimationView::LoopingMode::Type>(mode));
        }
        break;
      }
      case LottieAnimationViewImpl::Property::STOP_BEHAVIOR:
      {
        int behavior;
        if(value.Get(behavior))
        {
          impl.SetStopBehavior(static_cast<LottieAnimationView::StopBehavior::Type>(behavior));
        }
        break;
      }
      case LottieAnimationViewImpl::Property::FRAME_SPEED_FACTOR:
      {
        float factor;
        if(value.Get(factor))
        {
          impl.SetFrameSpeedFactor(factor);
        }
        break;
      }
      case LottieAnimationViewImpl::Property::IMAGE_COLOR:
      {
        Vector4 color;
        if(value.Get(color))
        {
          impl.SetImageColor(UiColor(color));
        }
        break;
      }
      case LottieAnimationViewImpl::Property::DESIRED_WIDTH:
      {
        int width;
        if(value.Get(width))
        {
          impl.SetDesiredWidth(width);
        }
        break;
      }
      case LottieAnimationViewImpl::Property::DESIRED_HEIGHT:
      {
        int height;
        if(value.Get(height))
        {
          impl.SetDesiredHeight(height);
        }
        break;
      }
      case LottieAnimationViewImpl::Property::RELEASE_POLICY:
      {
        int policy;
        if(value.Get(policy))
        {
          impl.SetReleasePolicy(static_cast<Ui::ReleasePolicy::Type>(policy));
        }
        break;
      }
      case LottieAnimationViewImpl::Property::SYNCHRONOUS_LOADING:
      {
        bool sync;
        if(value.Get(sync))
        {
          impl.SetSynchronousLoading(sync);
        }
        break;
      }
      case LottieAnimationViewImpl::Property::PRE_MULTIPLIED_ALPHA:
      {
        bool preMultiplied;
        if(value.Get(preMultiplied))
        {
          impl.SetPreMultipliedAlpha(preMultiplied);
        }
        break;
      }
      case LottieAnimationViewImpl::Property::REDRAW_IN_SCALING_DOWN:
      {
        bool redraw;
        if(value.Get(redraw))
        {
          impl.SetRedrawInScalingDown(redraw);
        }
        break;
      }
      case LottieAnimationViewImpl::Property::REDRAW_IN_SCALING_UP:
      {
        bool redraw;
        if(value.Get(redraw))
        {
          impl.SetRedrawInScalingUp(redraw);
        }
        break;
      }
      case LottieAnimationViewImpl::Property::ENABLE_FRAME_CACHE:
      {
        bool enable;
        if(value.Get(enable))
        {
          impl.SetEnableFrameCache(enable);
        }
        break;
      }
      case LottieAnimationViewImpl::Property::NOTIFY_AFTER_RASTERIZATION:
      {
        bool notify;
        if(value.Get(notify))
        {
          impl.SetNotifyAfterRasterization(notify);
        }
        break;
      }
    }
  }
}

Dali::Property::Value LottieAnimationViewImpl::GetProperty(Dali::BaseObject* object, Dali::Property::Index index)
{
  Dali::Property::Value value;
  Ui::View              view = Ui::View::DownCast(Dali::BaseHandle(object));
  if(view)
  {
    LottieAnimationViewImpl& impl = static_cast<LottieAnimationViewImpl&>(GetImpl(view));
    switch(index)
    {
      case LottieAnimationViewImpl::Property::IMAGE:
        value = impl.GetResourceUrl();
        break;
      case LottieAnimationViewImpl::Property::LOOP_COUNT:
        value = impl.GetLoopCount();
        break;
      case LottieAnimationViewImpl::Property::LOOPING_MODE:
        value = static_cast<int>(impl.GetLoopingMode());
        break;
      case LottieAnimationViewImpl::Property::STOP_BEHAVIOR:
        value = static_cast<int>(impl.GetStopBehavior());
        break;
      case LottieAnimationViewImpl::Property::FRAME_SPEED_FACTOR:
        value = impl.GetFrameSpeedFactor();
        break;
      case LottieAnimationViewImpl::Property::IMAGE_COLOR:
        value = impl.GetImageColor().Resolve();
        break;
      case LottieAnimationViewImpl::Property::DESIRED_WIDTH:
        value = impl.GetDesiredWidth();
        break;
      case LottieAnimationViewImpl::Property::DESIRED_HEIGHT:
        value = impl.GetDesiredHeight();
        break;
      case LottieAnimationViewImpl::Property::RELEASE_POLICY:
        value = static_cast<int>(impl.GetReleasePolicy());
        break;
      case LottieAnimationViewImpl::Property::SYNCHRONOUS_LOADING:
        value = impl.IsSynchronousLoading();
        break;
      case LottieAnimationViewImpl::Property::PRE_MULTIPLIED_ALPHA:
        value = impl.IsPreMultipliedAlpha();
        break;
      case LottieAnimationViewImpl::Property::REDRAW_IN_SCALING_DOWN:
        value = impl.IsRedrawInScalingDown();
        break;
      case LottieAnimationViewImpl::Property::REDRAW_IN_SCALING_UP:
        value = impl.IsRedrawInScalingUp();
        break;
      case LottieAnimationViewImpl::Property::ENABLE_FRAME_CACHE:
        value = impl.IsFrameCacheEnabled();
        break;
      case LottieAnimationViewImpl::Property::NOTIFY_AFTER_RASTERIZATION:
        value = impl.IsNotifyAfterRasterizationEnabled();
        break;
    }
  }
  return value;
}

void LottieAnimationViewImpl::OnInitialize()
{
  ViewImpl::OnInitialize();
  Internal::ViewDataImpl::Get(*this).VisualEventSignal().Connect(this, &LottieAnimationViewImpl::OnVisualEvent);
}

MeasuredSize LottieAnimationViewImpl::OnMeasure(float widthConstraint, float heightConstraint)
{
  auto& viewData = Internal::ViewDataImpl::Get(*this);

  Vector2 naturalSize;
  if(auto visual = viewData.GetVisual(LottieAnimationViewImpl::Property::IMAGE))
  {
    visual.GetNaturalSize(naturalSize);
  }

  float w = naturalSize.width;
  float h = naturalSize.height;

  float layoutW = GetRequestedWidth();
  float layoutH = GetRequestedHeight();

  if(layoutW == MATCH_PARENT)
  {
    w = widthConstraint;
  }
  else if(layoutW > 0)
  {
    w = layoutW;
  }

  if(layoutH == MATCH_PARENT)
  {
    h = heightConstraint;
  }
  else if(layoutH > 0)
  {
    h = layoutH;
  }

  return MeasuredSize(w, h);
}

void LottieAnimationViewImpl::SetResourceUrl(const Dali::String& url)
{
  if(mUrl != url)
  {
    mUrl = url;
    UpdateVisual();
  }
}

Dali::String LottieAnimationViewImpl::GetResourceUrl() const
{
  return mUrl;
}

void LottieAnimationViewImpl::Play()
{
  auto& viewData = Internal::ViewDataImpl::Get(*this);
  if(viewData.GetVisual(LottieAnimationViewImpl::Property::IMAGE))
  {
    viewData.DoAction(LottieAnimationViewImpl::Property::IMAGE, Ui::DevelAnimatedVectorImageVisual::Action::PLAY, Dali::Property::Map());
  }
}

void LottieAnimationViewImpl::Pause()
{
  auto& viewData = Internal::ViewDataImpl::Get(*this);
  if(viewData.GetVisual(LottieAnimationViewImpl::Property::IMAGE))
  {
    viewData.DoAction(LottieAnimationViewImpl::Property::IMAGE, Ui::DevelAnimatedVectorImageVisual::Action::PAUSE, Dali::Property::Map());
  }
}

void LottieAnimationViewImpl::Stop()
{
  auto& viewData = Internal::ViewDataImpl::Get(*this);
  if(viewData.GetVisual(LottieAnimationViewImpl::Property::IMAGE))
  {
    viewData.DoAction(LottieAnimationViewImpl::Property::IMAGE, Ui::DevelAnimatedVectorImageVisual::Action::STOP, Dali::Property::Map());
  }
}

void LottieAnimationViewImpl::SetLoopCount(int count)
{
  if(mLoopCount != count)
  {
    mLoopCount = count;
    UpdateVisual();
  }
}

int LottieAnimationViewImpl::GetLoopCount() const
{
  return mLoopCount;
}

void LottieAnimationViewImpl::JumpToFrame(int frame)
{
  auto& viewData = Internal::ViewDataImpl::Get(*this);
  if(viewData.GetVisual(LottieAnimationViewImpl::Property::IMAGE))
  {
    viewData.DoAction(LottieAnimationViewImpl::Property::IMAGE, Ui::DevelAnimatedVectorImageVisual::Action::JUMP_TO, frame);
  }
}

void LottieAnimationViewImpl::SetMinMaxFrame(int minFrame, int maxFrame)
{
  mPlayRangeType = PlayRangeType::FRAME;
  mMinFrame      = minFrame;
  mMaxFrame      = maxFrame;
  UpdateVisual();
}

void LottieAnimationViewImpl::SetMinMaxFrameByMarker(const Dali::String& marker1, const Dali::String& marker2)
{
  mPlayRangeType = PlayRangeType::MARKER;
  mMarker1       = marker1;
  mMarker2       = marker2;
  UpdateVisual();
}

void LottieAnimationViewImpl::SetStopBehavior(LottieAnimationView::StopBehavior::Type behavior)
{
  if(mStopBehavior != behavior)
  {
    mStopBehavior = behavior;
    UpdateVisual();
  }
}

LottieAnimationView::StopBehavior::Type LottieAnimationViewImpl::GetStopBehavior() const
{
  return mStopBehavior;
}

void LottieAnimationViewImpl::SetLoopingMode(LottieAnimationView::LoopingMode::Type mode)
{
  if(mLoopingMode != mode)
  {
    mLoopingMode = mode;
    UpdateVisual();
  }
}

LottieAnimationView::LoopingMode::Type LottieAnimationViewImpl::GetLoopingMode() const
{
  return mLoopingMode;
}

void LottieAnimationViewImpl::SetFrameSpeedFactor(float factor)
{
  if(mFrameSpeedFactor != factor)
  {
    mFrameSpeedFactor = factor;
    UpdateVisual();
  }
}

float LottieAnimationViewImpl::GetFrameSpeedFactor() const
{
  return mFrameSpeedFactor;
}

LottieAnimationView::PlayState::Type LottieAnimationViewImpl::GetPlayState() const
{
  auto& viewData = Internal::ViewDataImpl::Get(*this);
  if(auto visual = viewData.GetVisual(LottieAnimationViewImpl::Property::IMAGE))
  {
    Dali::Property::Map map;
    visual.CreatePropertyMap(map);
    if(auto* value = map.Find(Ui::DevelImageVisual::Property::PLAY_STATE))
    {
      return static_cast<LottieAnimationView::PlayState::Type>(value->Get<int>());
    }
  }
  return LottieAnimationView::PlayState::STOPPED;
}

int LottieAnimationViewImpl::GetCurrentFrame() const
{
  auto& viewData = Internal::ViewDataImpl::Get(*this);
  if(auto visual = viewData.GetVisual(LottieAnimationViewImpl::Property::IMAGE))
  {
    Dali::Property::Map map;
    visual.CreatePropertyMap(map);
    if(auto* value = map.Find(Ui::DevelImageVisual::Property::CURRENT_FRAME_NUMBER))
    {
      return value->Get<int>();
    }
  }
  return 0;
}

int LottieAnimationViewImpl::GetTotalFrame() const
{
  auto& viewData = Internal::ViewDataImpl::Get(*this);
  if(auto visual = viewData.GetVisual(LottieAnimationViewImpl::Property::IMAGE))
  {
    Dali::Property::Map map;
    visual.CreatePropertyMap(map);
    if(auto* value = map.Find(Ui::DevelImageVisual::Property::TOTAL_FRAME_NUMBER))
    {
      return value->Get<int>();
    }
  }
  return 0;
}

void LottieAnimationViewImpl::SetRedrawInScalingDown(bool redraw)
{
  if(mRedrawInScalingDown != redraw)
  {
    mRedrawInScalingDown = redraw;
    UpdateVisual();
  }
}

bool LottieAnimationViewImpl::IsRedrawInScalingDown() const
{
  return mRedrawInScalingDown;
}

void LottieAnimationViewImpl::SetRedrawInScalingUp(bool redraw)
{
  if(mRedrawInScalingUp != redraw)
  {
    mRedrawInScalingUp = redraw;
    UpdateVisual();
  }
}

bool LottieAnimationViewImpl::IsRedrawInScalingUp() const
{
  return mRedrawInScalingUp;
}

void LottieAnimationViewImpl::SetEnableFrameCache(bool enable)
{
  if(mEnableFrameCache != enable)
  {
    mEnableFrameCache = enable;
    UpdateVisual();
  }
}

bool LottieAnimationViewImpl::IsFrameCacheEnabled() const
{
  return mEnableFrameCache;
}

void LottieAnimationViewImpl::SetNotifyAfterRasterization(bool notify)
{
  if(mNotifyAfterRasterization != notify)
  {
    mNotifyAfterRasterization = notify;
    UpdateVisual();
  }
}

bool LottieAnimationViewImpl::IsNotifyAfterRasterizationEnabled() const
{
  return mNotifyAfterRasterization;
}

Dali::Property::Map LottieAnimationViewImpl::GetContentInfo()
{
  Dali::Property::Map result;
  auto&               viewData = Internal::ViewDataImpl::Get(*this);
  if(auto visual = viewData.GetVisual(LottieAnimationViewImpl::Property::IMAGE))
  {
    Dali::Property::Map map;
    visual.CreatePropertyMap(map);
    if(auto* value = map.Find(Ui::DevelImageVisual::Property::CONTENT_INFO))
    {
      value->Get(result);
    }
  }
  return result;
}

Dali::Property::Map LottieAnimationViewImpl::GetMarkerInfo()
{
  Dali::Property::Map result;
  auto&               viewData = Internal::ViewDataImpl::Get(*this);
  if(auto visual = viewData.GetVisual(LottieAnimationViewImpl::Property::IMAGE))
  {
    Dali::Property::Map map;
    visual.CreatePropertyMap(map);
    if(auto* value = map.Find(Ui::DevelImageVisual::Property::MARKER_INFO))
    {
      value->Get(result);
    }
  }
  return result;
}

void LottieAnimationViewImpl::SetDynamicProperty(const LottieAnimationView::DynamicPropertyInfo& info)
{
  auto& viewData = Internal::ViewDataImpl::Get(*this);
  if(viewData.GetVisual(LottieAnimationViewImpl::Property::IMAGE))
  {
    Ui::DevelAnimatedVectorImageVisual::DynamicPropertyInfo dynamicInfo;
    dynamicInfo.id       = info.id;
    dynamicInfo.keyPath  = info.keyPath.CStr();
    dynamicInfo.property = info.property;
    dynamicInfo.callback = info.callback;
    viewData.DoActionExtension(LottieAnimationViewImpl::Property::IMAGE,
                               Ui::DevelAnimatedVectorImageVisual::Action::SET_DYNAMIC_PROPERTY,
                               Dali::Any(dynamicInfo));
  }
}

void LottieAnimationViewImpl::Flush()
{
  auto& viewData = Internal::ViewDataImpl::Get(*this);
  if(viewData.GetVisual(LottieAnimationViewImpl::Property::IMAGE))
  {
    viewData.DoAction(LottieAnimationViewImpl::Property::IMAGE, Ui::DevelAnimatedVectorImageVisual::Action::FLUSH, Dali::Property::Map());
  }
}

LottieAnimationView::AnimationFinishedSignalType& LottieAnimationViewImpl::AnimationFinishedSignal()
{
  return mAnimationFinishedSignal;
}

Ui::Visual::ResourceStatus LottieAnimationViewImpl::GetLoadingStatus() const
{
  return Internal::ViewDataImpl::Get(*this).GetVisualResourceStatus(LottieAnimationViewImpl::Property::IMAGE);
}

void LottieAnimationViewImpl::OnVisualEvent(View view, Dali::Property::Index visualIndex, Dali::Property::Index signalId)
{
  if(visualIndex == LottieAnimationViewImpl::Property::IMAGE &&
     signalId == static_cast<Dali::Property::Index>(Ui::DevelAnimatedVectorImageVisual::Signal::ANIMATION_FINISHED))
  {
    LottieAnimationView handle = LottieAnimationView::DownCast(view);
    mAnimationFinishedSignal.Emit(handle);
  }
}

void LottieAnimationViewImpl::UpdateVisual()
{
  auto& viewData = Internal::ViewDataImpl::Get(*this);
  viewData.UnregisterVisual(LottieAnimationViewImpl::Property::IMAGE);
  if(mUrl.Empty())
  {
    return;
  }

  Dali::Property::Map map;
  map.Insert(Visual::Property::TYPE, static_cast<int>(Ui::DevelVisual::ANIMATED_VECTOR_IMAGE));
  map.Insert(Ui::ImageVisual::Property::URL, mUrl);

  if(mLoopCount != -1)
  {
    map.Insert(Ui::DevelImageVisual::Property::LOOP_COUNT, mLoopCount);
  }

  if(mStopBehavior != LottieAnimationView::StopBehavior::CURRENT_FRAME)
  {
    map.Insert(Ui::DevelImageVisual::Property::STOP_BEHAVIOR, static_cast<int>(mStopBehavior));
  }

  if(mLoopingMode != LottieAnimationView::LoopingMode::RESTART)
  {
    map.Insert(Ui::DevelImageVisual::Property::LOOPING_MODE, static_cast<int>(mLoopingMode));
  }

  if(mFrameSpeedFactor != 1.0f)
  {
    map.Insert(Ui::DevelImageVisual::Property::FRAME_SPEED_FACTOR, mFrameSpeedFactor);
  }

  if(mPlayRangeType == PlayRangeType::FRAME)
  {
    Dali::Property::Array range;
    range.PushBack(mMinFrame);
    range.PushBack(mMaxFrame);
    map.Insert(Ui::DevelImageVisual::Property::PLAY_RANGE, range);
  }
  else if(mPlayRangeType == PlayRangeType::MARKER)
  {
    Dali::Property::Array range;
    range.PushBack(mMarker1);
    if(!mMarker2.Empty())
    {
      range.PushBack(mMarker2);
    }
    map.Insert(Ui::DevelImageVisual::Property::PLAY_RANGE, range);
  }

  if(!mRedrawInScalingDown)
  {
    map.Insert(Ui::DevelImageVisual::Property::REDRAW_IN_SCALING_DOWN, false);
  }

  if(!mRedrawInScalingUp)
  {
    map.Insert(Ui::DevelImageVisual::Property::REDRAW_IN_SCALING_UP, false);
  }

  if(mEnableFrameCache)
  {
    map.Insert(Ui::DevelImageVisual::Property::ENABLE_FRAME_CACHE, true);
  }

  if(mNotifyAfterRasterization)
  {
    map.Insert(Ui::DevelImageVisual::Property::NOTIFY_AFTER_RASTERIZATION, true);
  }

  if(mDesiredWidth > 0)
  {
    map.Insert(Ui::ImageVisual::Property::DESIRED_WIDTH, mDesiredWidth);
  }

  if(mDesiredHeight > 0)
  {
    map.Insert(Ui::ImageVisual::Property::DESIRED_HEIGHT, mDesiredHeight);
  }

  if(mReleasePolicy != Ui::ReleasePolicy::DETACHED)
  {
    map.Insert(Ui::DevelImageVisual::Property::RELEASE_POLICY, static_cast<int>(mReleasePolicy));
  }

  if(mSynchronousLoading)
  {
    map.Insert(Ui::ImageVisual::Property::SYNCHRONOUS_LOADING, true);
  }

  if(mPreMultipliedAlpha)
  {
    map.Insert(Visual::Property::PREMULTIPLIED_ALPHA, true);
  }

  if(mImageColor.Resolve() != Color::WHITE)
  {
    map.Insert(Visual::Property::MIX_COLOR, mImageColor.Resolve());
  }

  auto visualFactory = Ui::VisualFactory::Get();
  if(visualFactory)
  {
    auto visual = visualFactory.CreateVisual(map);
    if(visual)
    {
      viewData.RegisterVisual(LottieAnimationViewImpl::Property::IMAGE, visual);
    }
  }
}

void LottieAnimationViewImpl::SetDesiredWidth(int width)
{
  if(mDesiredWidth != width)
  {
    mDesiredWidth = width;
    UpdateVisual();
  }
}

int LottieAnimationViewImpl::GetDesiredWidth() const
{
  return mDesiredWidth;
}

void LottieAnimationViewImpl::SetDesiredHeight(int height)
{
  if(mDesiredHeight != height)
  {
    mDesiredHeight = height;
    UpdateVisual();
  }
}

int LottieAnimationViewImpl::GetDesiredHeight() const
{
  return mDesiredHeight;
}

void LottieAnimationViewImpl::SetReleasePolicy(Ui::ReleasePolicy::Type releasePolicy)
{
  if(mReleasePolicy != releasePolicy)
  {
    mReleasePolicy = releasePolicy;
    UpdateVisual();
  }
}

Ui::ReleasePolicy::Type LottieAnimationViewImpl::GetReleasePolicy() const
{
  return mReleasePolicy;
}

void LottieAnimationViewImpl::SetSynchronousLoading(bool synchronous)
{
  if(mSynchronousLoading != synchronous)
  {
    mSynchronousLoading = synchronous;
    UpdateVisual();
  }
}

bool LottieAnimationViewImpl::IsSynchronousLoading() const
{
  return mSynchronousLoading;
}

void LottieAnimationViewImpl::SetPreMultipliedAlpha(bool preMultiplied)
{
  if(mPreMultipliedAlpha != preMultiplied)
  {
    mPreMultipliedAlpha = preMultiplied;
    UpdateVisual();
  }
}

bool LottieAnimationViewImpl::IsPreMultipliedAlpha() const
{
  return mPreMultipliedAlpha;
}

void LottieAnimationViewImpl::SetImageColor(const UiColor& color)
{
  if(mImageColor.Resolve() != color.Resolve())
  {
    mImageColor    = color;
    auto& viewData = Internal::ViewDataImpl::Get(*this);
    if(viewData.GetVisual(LottieAnimationViewImpl::Property::IMAGE))
    {
      // Update MIX_COLOR directly on the existing visual without rebuilding it,
      // so the animation continues uninterrupted.
      Dali::Property::Map map;
      map.Insert(Visual::Property::MIX_COLOR, mImageColor.Resolve());
      viewData.DoAction(LottieAnimationViewImpl::Property::IMAGE, DevelVisual::Action::UPDATE_PROPERTY, map);
    }
    else
    {
      UpdateVisual();
    }
  }
}

UiColor LottieAnimationViewImpl::GetImageColor() const
{
  return mImageColor;
}

} // namespace Integration
} // namespace Ui
} // namespace Dali
