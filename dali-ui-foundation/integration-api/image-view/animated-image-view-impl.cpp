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
#include "animated-image-view-impl.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/devel-api/object/type-registry.h>
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/object/property-array.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/visual-factory/visual-factory.h>
#include <dali-ui-foundation/devel-api/visuals/animated-image-visual-actions-devel.h>
#include <dali-ui-foundation/devel-api/visuals/animated-image-visual-signals-devel.h>
#include <dali-ui-foundation/devel-api/visuals/image-visual-properties-devel.h>
#include <dali-ui-foundation/devel-api/visuals/visual-actions-devel.h>
#include <dali-ui-foundation/devel-api/visuals/visual-properties-devel.h>
#include <dali-ui-foundation/integration-api/property-registration-helper.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/internal/visuals/visual-base-impl.h>
#include <dali-ui-foundation/public-api/align-enumerations.h>
#include <dali-ui-foundation/public-api/ui-color.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>
#include <algorithm>

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
  AnimatedImageViewImplPtr impl = AnimatedImageViewImpl::New();
  Ui::View                 view(*impl);
  impl->Initialize();
  return view;
}
// clang-format off
#define ANIMATED_IMAGE_VIEW_PROPERTY_REGISTRATION(text, valueType, enumIndex) \
  DALI_PROPERTY_REGISTRATION_EXTERNAL(Ui, AnimatedImageViewPropertyIndex, Ui::Integration, AnimatedImageViewImpl, text, valueType, enumIndex)

DALI_TYPE_REGISTRATION_BEGIN(AnimatedImageViewImpl, ViewImpl, Create)

ANIMATED_IMAGE_VIEW_PROPERTY_REGISTRATION("image",                 STRING,  IMAGE)
ANIMATED_IMAGE_VIEW_PROPERTY_REGISTRATION("loopCount",             INTEGER, LOOP_COUNT)
ANIMATED_IMAGE_VIEW_PROPERTY_REGISTRATION("imageColor",            VECTOR4, IMAGE_COLOR)
ANIMATED_IMAGE_VIEW_PROPERTY_REGISTRATION("stopBehavior",          INTEGER, STOP_BEHAVIOR)
ANIMATED_IMAGE_VIEW_PROPERTY_REGISTRATION("frameSpeedFactor",      FLOAT,   FRAME_SPEED_FACTOR)
ANIMATED_IMAGE_VIEW_PROPERTY_REGISTRATION("batchSize",             INTEGER, BATCH_SIZE)
ANIMATED_IMAGE_VIEW_PROPERTY_REGISTRATION("cacheSize",             INTEGER, CACHE_SIZE)
ANIMATED_IMAGE_VIEW_PROPERTY_REGISTRATION("frameDelay",            INTEGER, FRAME_DELAY)
ANIMATED_IMAGE_VIEW_PROPERTY_REGISTRATION("desiredWidth",          INTEGER, DESIRED_WIDTH)
ANIMATED_IMAGE_VIEW_PROPERTY_REGISTRATION("desiredHeight",         INTEGER, DESIRED_HEIGHT)
ANIMATED_IMAGE_VIEW_PROPERTY_REGISTRATION("releasePolicy",         INTEGER, RELEASE_POLICY)
ANIMATED_IMAGE_VIEW_PROPERTY_REGISTRATION("synchronousLoading",    BOOLEAN, SYNCHRONOUS_LOADING)
ANIMATED_IMAGE_VIEW_PROPERTY_REGISTRATION("preMultipliedAlpha",    BOOLEAN, PRE_MULTIPLIED_ALPHA)
ANIMATED_IMAGE_VIEW_PROPERTY_REGISTRATION("fittingMode",           INTEGER, FITTING_MODE)
ANIMATED_IMAGE_VIEW_PROPERTY_REGISTRATION("samplingMode",          INTEGER, SAMPLING_MODE)
ANIMATED_IMAGE_VIEW_PROPERTY_REGISTRATION("imageLoadWithViewSize", BOOLEAN, IMAGE_LOAD_WITH_VIEW_SIZE)
ANIMATED_IMAGE_VIEW_PROPERTY_REGISTRATION("alphaMaskUrl",          STRING,  ALPHA_MASK_URL)
ANIMATED_IMAGE_VIEW_PROPERTY_REGISTRATION("cropToMask",            BOOLEAN, CROP_TO_MASK)
ANIMATED_IMAGE_VIEW_PROPERTY_REGISTRATION("maskingMode",           INTEGER, MASKING_MODE)
ANIMATED_IMAGE_VIEW_PROPERTY_REGISTRATION("placeholderImage",      STRING,  PLACEHOLDER_IMAGE)
ANIMATED_IMAGE_VIEW_PROPERTY_REGISTRATION("imageUrls",             ARRAY,   IMAGE_URLS)

DALI_TYPE_REGISTRATION_END()
#undef ANIMATED_IMAGE_VIEW_PROPERTY_REGISTRATION
// clang-format on
} // namespace

AnimatedImageViewImpl::AnimatedImageViewImpl()
: ViewImpl(),
  mUrl(),
  mLoopCount(-1),
  mImageColor(Color::WHITE),
  mStopBehavior(AnimatedImageView::StopBehavior::CURRENT_FRAME),
  mFrameSpeedFactor(1.0f),
  mBatchSize(1),
  mCacheSize(1),
  mFrameDelay(-1),
  mDesiredWidth(0),
  mDesiredHeight(0),
  mReleasePolicy(Ui::ReleasePolicy::DETACHED),
  mSynchronousLoading(false),
  mPreMultipliedAlpha(false),
  mFittingMode(Ui::FittingMode::FILL),
  mSamplingMode(Ui::SamplingMode::LINEAR),
  mImageLoadWithViewSize(false),
  mAlphaMaskUrl(),
  mCropToMask(false),
  mMaskingMode(Ui::MaskingType::MASKING_ON_LOADING),
  mPlaceholderUrl(),
  mVisual(),
  mVisualDirty(false),
  mAnimationFinishedSignal()
{
}

AnimatedImageViewImpl::~AnimatedImageViewImpl()
{
}

Dali::String AnimatedImageViewImpl::GetResourceUrl() const
{
  return mUrl;
}

AnimatedImageViewImplPtr AnimatedImageViewImpl::New()
{
  return new AnimatedImageViewImpl();
}

void AnimatedImageViewImpl::SetProperty(Dali::BaseObject* object, Dali::Property::Index index, const Dali::Property::Value& value)
{
  Ui::View view = Ui::View::DownCast(Dali::BaseHandle(object));
  if(view)
  {
    AnimatedImageViewImpl& impl = static_cast<AnimatedImageViewImpl&>(GetImpl(view));
    switch(index)
    {
      case AnimatedImageViewImpl::Property::IMAGE:
      {
        Dali::String url;
        if(value.Get(url))
        {
          impl.SetResourceUrl(url);
        }
        break;
      }
      case AnimatedImageViewImpl::Property::LOOP_COUNT:
      {
        int count;
        if(value.Get(count))
        {
          impl.SetLoopCount(count);
        }
        break;
      }
      case AnimatedImageViewImpl::Property::IMAGE_COLOR:
      {
        Vector4 color;
        if(value.Get(color))
        {
          impl.SetImageColor(UiColor(color));
        }
        break;
      }
      case AnimatedImageViewImpl::Property::STOP_BEHAVIOR:
      {
        int behavior;
        if(value.Get(behavior))
        {
          impl.SetStopBehavior(static_cast<AnimatedImageView::StopBehavior::Type>(behavior));
        }
        break;
      }
      case AnimatedImageViewImpl::Property::FRAME_SPEED_FACTOR:
      {
        float factor;
        if(value.Get(factor))
        {
          impl.SetFrameSpeedFactor(factor);
        }
        break;
      }
      case AnimatedImageViewImpl::Property::BATCH_SIZE:
      {
        int size;
        if(value.Get(size))
        {
          impl.SetBatchSize(size);
        }
        break;
      }
      case AnimatedImageViewImpl::Property::CACHE_SIZE:
      {
        int size;
        if(value.Get(size))
        {
          impl.SetCacheSize(size);
        }
        break;
      }
      case AnimatedImageViewImpl::Property::FRAME_DELAY:
      {
        int delay;
        if(value.Get(delay))
        {
          impl.SetFrameDelay(delay);
        }
        break;
      }
      case AnimatedImageViewImpl::Property::DESIRED_WIDTH:
      {
        int width;
        if(value.Get(width))
        {
          impl.SetDesiredWidth(width);
        }
        break;
      }
      case AnimatedImageViewImpl::Property::DESIRED_HEIGHT:
      {
        int height;
        if(value.Get(height))
        {
          impl.SetDesiredHeight(height);
        }
        break;
      }
      case AnimatedImageViewImpl::Property::RELEASE_POLICY:
      {
        int policy;
        if(value.Get(policy))
        {
          impl.SetReleasePolicy(static_cast<Ui::ReleasePolicy::Type>(policy));
        }
        break;
      }
      case AnimatedImageViewImpl::Property::SYNCHRONOUS_LOADING:
      {
        bool sync;
        if(value.Get(sync))
        {
          impl.SetSynchronousLoading(sync);
        }
        break;
      }
      case AnimatedImageViewImpl::Property::PRE_MULTIPLIED_ALPHA:
      {
        bool preMultiplied;
        if(value.Get(preMultiplied))
        {
          impl.SetPreMultipliedAlpha(preMultiplied);
        }
        break;
      }
      case AnimatedImageViewImpl::Property::FITTING_MODE:
      {
        int mode;
        if(value.Get(mode))
        {
          impl.SetFittingMode(static_cast<Ui::FittingMode::Type>(mode));
        }
        break;
      }
      case AnimatedImageViewImpl::Property::SAMPLING_MODE:
      {
        int mode;
        if(value.Get(mode))
        {
          impl.SetSamplingMode(static_cast<Ui::SamplingMode::Type>(mode));
        }
        break;
      }
      case AnimatedImageViewImpl::Property::IMAGE_LOAD_WITH_VIEW_SIZE:
      {
        bool enabled;
        if(value.Get(enabled))
        {
          impl.SetImageLoadWithViewSize(enabled);
        }
        break;
      }
      case AnimatedImageViewImpl::Property::ALPHA_MASK_URL:
      {
        Dali::String url;
        if(value.Get(url))
        {
          impl.SetAlphaMaskUrl(url);
        }
        break;
      }
      case AnimatedImageViewImpl::Property::CROP_TO_MASK:
      {
        bool crop;
        if(value.Get(crop))
        {
          impl.SetCropToMask(crop);
        }
        break;
      }
      case AnimatedImageViewImpl::Property::MASKING_MODE:
      {
        int mode;
        if(value.Get(mode))
        {
          impl.SetMaskingMode(static_cast<Ui::MaskingType::Type>(mode));
        }
        break;
      }
      case AnimatedImageViewImpl::Property::PLACEHOLDER_IMAGE:
      {
        Dali::String url;
        if(value.Get(url))
        {
          impl.SetPlaceholderUrl(url);
        }
        break;
      }
      case AnimatedImageViewImpl::Property::IMAGE_URLS:
      {
        Dali::Property::Array array;
        if(value.Get(array))
        {
          Dali::Vector<Dali::String> urls;
          for(auto i = 0u; i < array.Count(); ++i)
          {
            Dali::String url;
            if(array[i].Get(url))
            {
              urls.PushBack(url);
            }
          }
          impl.SetImageUrls(urls);
        }
        break;
      }
    }
  }
}

Dali::Property::Value AnimatedImageViewImpl::GetProperty(Dali::BaseObject* object, Dali::Property::Index index)
{
  Dali::Property::Value value;
  Ui::View              view = Ui::View::DownCast(Dali::BaseHandle(object));
  if(view)
  {
    AnimatedImageViewImpl& impl = static_cast<AnimatedImageViewImpl&>(GetImpl(view));
    switch(index)
    {
      case AnimatedImageViewImpl::Property::IMAGE:
        value = impl.GetResourceUrl();
        break;
      case AnimatedImageViewImpl::Property::LOOP_COUNT:
        value = impl.GetLoopCount();
        break;
      case AnimatedImageViewImpl::Property::IMAGE_COLOR:
        value = impl.GetImageColor().Resolve();
        break;
      case AnimatedImageViewImpl::Property::STOP_BEHAVIOR:
        value = static_cast<int>(impl.GetStopBehavior());
        break;
      case AnimatedImageViewImpl::Property::FRAME_SPEED_FACTOR:
        value = impl.GetFrameSpeedFactor();
        break;
      case AnimatedImageViewImpl::Property::BATCH_SIZE:
        value = impl.GetBatchSize();
        break;
      case AnimatedImageViewImpl::Property::CACHE_SIZE:
        value = impl.GetCacheSize();
        break;
      case AnimatedImageViewImpl::Property::FRAME_DELAY:
        value = impl.GetFrameDelay();
        break;
      case AnimatedImageViewImpl::Property::DESIRED_WIDTH:
        value = impl.GetDesiredWidth();
        break;
      case AnimatedImageViewImpl::Property::DESIRED_HEIGHT:
        value = impl.GetDesiredHeight();
        break;
      case AnimatedImageViewImpl::Property::RELEASE_POLICY:
        value = static_cast<int>(impl.GetReleasePolicy());
        break;
      case AnimatedImageViewImpl::Property::SYNCHRONOUS_LOADING:
        value = impl.IsSynchronousLoading();
        break;
      case AnimatedImageViewImpl::Property::PRE_MULTIPLIED_ALPHA:
        value = impl.IsPreMultipliedAlpha();
        break;
      case AnimatedImageViewImpl::Property::FITTING_MODE:
        value = static_cast<int>(impl.GetFittingMode());
        break;
      case AnimatedImageViewImpl::Property::SAMPLING_MODE:
        value = static_cast<int>(impl.GetSamplingMode());
        break;
      case AnimatedImageViewImpl::Property::IMAGE_LOAD_WITH_VIEW_SIZE:
        value = impl.IsImageLoadWithViewSize();
        break;
      case AnimatedImageViewImpl::Property::ALPHA_MASK_URL:
        value = impl.GetAlphaMaskUrl();
        break;
      case AnimatedImageViewImpl::Property::CROP_TO_MASK:
        value = impl.IsCropToMask();
        break;
      case AnimatedImageViewImpl::Property::MASKING_MODE:
        value = static_cast<int>(impl.GetMaskingMode());
        break;
      case AnimatedImageViewImpl::Property::PLACEHOLDER_IMAGE:
        value = impl.GetPlaceholderUrl();
        break;
      case AnimatedImageViewImpl::Property::IMAGE_URLS:
      {
        Dali::Property::Array array;
        const auto&           urls = impl.GetImageUrls();
        for(auto i = 0u; i < urls.Size(); ++i)
        {
          array.PushBack(urls[i]);
        }
        value = array;
        break;
      }
    }
  }
  return value;
}

void AnimatedImageViewImpl::OnInitialize()
{
  ViewImpl::OnInitialize();
  Internal::ViewDataImpl::Get(*this).VisualEventSignal().Connect(this, &AnimatedImageViewImpl::OnVisualEvent);

  // Connect to View::ResourceReadySignal to handle placeholder removal
  Ui::View::DownCast(Self()).ResourceReadySignal().Connect(this, &AnimatedImageViewImpl::OnViewResourceReady);
}

MeasuredSize AnimatedImageViewImpl::OnMeasure(float widthConstraint, float heightConstraint)
{
  if(mVisualDirty)
  {
    mVisualDirty = false;
    UpdateVisual();
  }

  Vector2 naturalSize;
  if(mVisual)
  {
    mVisual.GetNaturalSize(naturalSize);
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

  if(naturalSize.width > 0.0f && naturalSize.height > 0.0f)
  {
    float aspectRatio = naturalSize.height / naturalSize.width;
    bool  widthFixed  = (layoutW == MATCH_PARENT || layoutW > 0);
    bool  heightFixed = (layoutH == MATCH_PARENT || layoutH > 0);
    if(widthFixed && !heightFixed)
    {
      h = w * aspectRatio;
    }
    else if(!widthFixed && heightFixed)
    {
      w = h / aspectRatio;
    }
  }

  return MeasuredSize(w, h);
}

MeasuredSize AnimatedImageViewImpl::OnArrange(const LayoutRect& bounds)
{
  MeasuredSize result = ViewImpl::OnArrange(bounds);
  ApplyFittingMode(Vector2(bounds.width, bounds.height));
  return result;
}

void AnimatedImageViewImpl::SetResourceUrl(const Dali::String& url)
{
  if(mUrl != url)
  {
    mUrl = url;
    mImageUrls.Clear();
    mVisualDirty = true;
    InvalidateMeasure();
  }
}

void AnimatedImageViewImpl::Play()
{
  auto& viewData = Internal::ViewDataImpl::Get(*this);
  if(viewData.GetVisual(AnimatedImageViewImpl::Property::IMAGE))
  {
    viewData.DoAction(AnimatedImageViewImpl::Property::IMAGE, Ui::DevelAnimatedImageVisual::Action::PLAY, Dali::Property::Map());
  }
}

void AnimatedImageViewImpl::Pause()
{
  auto& viewData = Internal::ViewDataImpl::Get(*this);
  if(viewData.GetVisual(AnimatedImageViewImpl::Property::IMAGE))
  {
    viewData.DoAction(AnimatedImageViewImpl::Property::IMAGE, Ui::DevelAnimatedImageVisual::Action::PAUSE, Dali::Property::Map());
  }
}

void AnimatedImageViewImpl::Stop()
{
  auto& viewData = Internal::ViewDataImpl::Get(*this);
  if(viewData.GetVisual(AnimatedImageViewImpl::Property::IMAGE))
  {
    viewData.DoAction(AnimatedImageViewImpl::Property::IMAGE, Ui::DevelAnimatedImageVisual::Action::STOP, Dali::Property::Map());
  }
}

void AnimatedImageViewImpl::JumpToFrame(int frame)
{
  auto& viewData = Internal::ViewDataImpl::Get(*this);
  if(viewData.GetVisual(AnimatedImageViewImpl::Property::IMAGE))
  {
    viewData.DoAction(AnimatedImageViewImpl::Property::IMAGE, Ui::DevelAnimatedImageVisual::Action::JUMP_TO, frame);
  }
}

void AnimatedImageViewImpl::SetStopBehavior(AnimatedImageView::StopBehavior::Type behavior)
{
  if(mStopBehavior != behavior)
  {
    mStopBehavior = behavior;
    mVisualDirty  = true;
    InvalidateMeasure();
  }
}

AnimatedImageView::StopBehavior::Type AnimatedImageViewImpl::GetStopBehavior() const
{
  return mStopBehavior;
}

void AnimatedImageViewImpl::SetFrameSpeedFactor(float factor)
{
  if(mFrameSpeedFactor != factor)
  {
    mFrameSpeedFactor = factor;
    mVisualDirty      = true;
    InvalidateMeasure();
  }
}

float AnimatedImageViewImpl::GetFrameSpeedFactor() const
{
  return mFrameSpeedFactor;
}

void AnimatedImageViewImpl::SetBatchSize(int size)
{
  if(mBatchSize != size)
  {
    mBatchSize   = size;
    mVisualDirty = true;
    InvalidateMeasure();
  }
}

int AnimatedImageViewImpl::GetBatchSize() const
{
  return mBatchSize;
}

void AnimatedImageViewImpl::SetCacheSize(int size)
{
  if(mCacheSize != size)
  {
    mCacheSize   = size;
    mVisualDirty = true;
    InvalidateMeasure();
  }
}

int AnimatedImageViewImpl::GetCacheSize() const
{
  return mCacheSize;
}

void AnimatedImageViewImpl::SetFrameDelay(int milliseconds)
{
  if(mFrameDelay != milliseconds)
  {
    mFrameDelay  = milliseconds;
    mVisualDirty = true;
    InvalidateMeasure();
  }
}

int AnimatedImageViewImpl::GetFrameDelay() const
{
  return mFrameDelay;
}

AnimatedImageView::PlayState::Type AnimatedImageViewImpl::GetPlayState() const
{
  if(mVisual)
  {
    Dali::Property::Map map;
    mVisual.CreatePropertyMap(map);
    if(auto* value = map.Find(Ui::DevelImageVisual::Property::PLAY_STATE))
    {
      return static_cast<AnimatedImageView::PlayState::Type>(value->Get<int>());
    }
  }
  return AnimatedImageView::PlayState::STOPPED;
}

int AnimatedImageViewImpl::GetCurrentFrame() const
{
  if(mVisual)
  {
    Dali::Property::Map map;
    mVisual.CreatePropertyMap(map);
    if(auto* value = map.Find(Ui::DevelImageVisual::Property::CURRENT_FRAME_NUMBER))
    {
      return value->Get<int>();
    }
  }
  return 0;
}

int AnimatedImageViewImpl::GetTotalFrame() const
{
  if(mVisual)
  {
    Dali::Property::Map map;
    mVisual.CreatePropertyMap(map);
    if(auto* value = map.Find(Ui::DevelImageVisual::Property::TOTAL_FRAME_NUMBER))
    {
      return value->Get<int>();
    }
  }
  return 0;
}

AnimatedImageView::AnimationFinishedSignalType& AnimatedImageViewImpl::AnimationFinishedSignal()
{
  return mAnimationFinishedSignal;
}

void AnimatedImageViewImpl::OnVisualEvent(View view, Dali::Property::Index visualIndex, Dali::Property::Index signalId)
{
  if(visualIndex == AnimatedImageViewImpl::Property::IMAGE &&
     signalId == Ui::DevelAnimatedImageVisual::Signal::ANIMATION_FINISHED)
  {
    AnimatedImageView handle = AnimatedImageView::DownCast(view);
    mAnimationFinishedSignal.Emit(handle);
  }
}

void AnimatedImageViewImpl::SetLoopCount(int count)
{
  if(mLoopCount != count)
  {
    mLoopCount   = count;
    mVisualDirty = true;
    InvalidateMeasure();
  }
}

int AnimatedImageViewImpl::GetLoopCount() const
{
  return mLoopCount;
}

void AnimatedImageViewImpl::SetImageColor(const UiColor& color)
{
  if(mImageColor.Resolve() != color.Resolve())
  {
    mImageColor = color;
    if(mVisual)
    {
      // Update MIX_COLOR directly on the existing visual without rebuilding it,
      // so the animation continues uninterrupted.
      Dali::Property::Map map;
      map.Insert(Ui::Visual::Property::MIX_COLOR, mImageColor.Resolve());
      mVisual.DoAction(DevelVisual::Action::UPDATE_PROPERTY, map);
    }
    else
    {
      mVisualDirty = true;
      InvalidateMeasure();
    }
  }
}

UiColor AnimatedImageViewImpl::GetImageColor() const
{
  return mImageColor;
}

void AnimatedImageViewImpl::SetDesiredWidth(int width)
{
  if(mDesiredWidth != width)
  {
    mDesiredWidth = width;
    mVisualDirty  = true;
    InvalidateMeasure();
  }
}

int AnimatedImageViewImpl::GetDesiredWidth() const
{
  return mDesiredWidth;
}

void AnimatedImageViewImpl::SetDesiredHeight(int height)
{
  if(mDesiredHeight != height)
  {
    mDesiredHeight = height;
    mVisualDirty   = true;
    InvalidateMeasure();
  }
}

int AnimatedImageViewImpl::GetDesiredHeight() const
{
  return mDesiredHeight;
}

void AnimatedImageViewImpl::SetReleasePolicy(Ui::ReleasePolicy::Type releasePolicy)
{
  if(mReleasePolicy != releasePolicy)
  {
    mReleasePolicy = releasePolicy;
    mVisualDirty   = true;
    InvalidateMeasure();
  }
}

Ui::ReleasePolicy::Type AnimatedImageViewImpl::GetReleasePolicy() const
{
  return mReleasePolicy;
}

void AnimatedImageViewImpl::SetSynchronousLoading(bool synchronous)
{
  if(mSynchronousLoading != synchronous)
  {
    mSynchronousLoading = synchronous;
    mVisualDirty        = true;
    InvalidateMeasure();
  }
}

bool AnimatedImageViewImpl::IsSynchronousLoading() const
{
  return mSynchronousLoading;
}

void AnimatedImageViewImpl::SetPreMultipliedAlpha(bool preMultiplied)
{
  if(mPreMultipliedAlpha != preMultiplied)
  {
    mPreMultipliedAlpha = preMultiplied;
    mVisualDirty        = true;
    InvalidateMeasure();
  }
}

bool AnimatedImageViewImpl::IsPreMultipliedAlpha() const
{
  return mPreMultipliedAlpha;
}

void AnimatedImageViewImpl::SetFittingMode(Ui::FittingMode::Type fittingMode)
{
  if(mFittingMode != fittingMode)
  {
    mFittingMode = fittingMode;
    // FittingMode only changes the visual transform, not the visual content itself.
    // Apply immediately if the actor already has a size; otherwise defer to OnArrange.
    Actor self = Self();
    float w    = self.GetProperty<float>(Actor::Property::SIZE_WIDTH);
    float h    = self.GetProperty<float>(Actor::Property::SIZE_HEIGHT);
    if(w > 0.0f && h > 0.0f)
    {
      ApplyFittingMode(Vector2(w, h));
    }
    else
    {
      InvalidateMeasure();
    }
  }
}

Ui::FittingMode::Type AnimatedImageViewImpl::GetFittingMode() const
{
  return mFittingMode;
}

void AnimatedImageViewImpl::SetSamplingMode(Ui::SamplingMode::Type samplingMode)
{
  if(mSamplingMode != samplingMode)
  {
    mSamplingMode = samplingMode;
    mVisualDirty  = true;
    InvalidateMeasure();
  }
}

Ui::SamplingMode::Type AnimatedImageViewImpl::GetSamplingMode() const
{
  return mSamplingMode;
}

void AnimatedImageViewImpl::SetImageLoadWithViewSize(bool enabled)
{
  if(mImageLoadWithViewSize != enabled)
  {
    mImageLoadWithViewSize = enabled;
    mVisualDirty           = true;
    InvalidateMeasure();
  }
}

bool AnimatedImageViewImpl::IsImageLoadWithViewSize() const
{
  return mImageLoadWithViewSize;
}

void AnimatedImageViewImpl::SetAlphaMaskUrl(const Dali::String& maskUrl)
{
  if(mAlphaMaskUrl != maskUrl)
  {
    mAlphaMaskUrl = maskUrl;
    mVisualDirty  = true;
    InvalidateMeasure();
  }
}

Dali::String AnimatedImageViewImpl::GetAlphaMaskUrl() const
{
  return mAlphaMaskUrl;
}

void AnimatedImageViewImpl::SetCropToMask(bool cropToMask)
{
  if(mCropToMask != cropToMask)
  {
    mCropToMask  = cropToMask;
    mVisualDirty = true;
    InvalidateMeasure();
  }
}

bool AnimatedImageViewImpl::IsCropToMask() const
{
  return mCropToMask;
}

void AnimatedImageViewImpl::SetMaskingMode(Ui::MaskingType::Type maskingMode)
{
  if(mMaskingMode != maskingMode)
  {
    mMaskingMode = maskingMode;
    mVisualDirty = true;
    InvalidateMeasure();
  }
}

Ui::MaskingType::Type AnimatedImageViewImpl::GetMaskingMode() const
{
  return mMaskingMode;
}

void AnimatedImageViewImpl::SetPlaceholderUrl(const Dali::String& url)
{
  if(mPlaceholderUrl != url)
  {
    mPlaceholderUrl = url;
    UpdatePlaceholderVisual();
  }
}

Dali::String AnimatedImageViewImpl::GetPlaceholderUrl() const
{
  return mPlaceholderUrl;
}

void AnimatedImageViewImpl::SetImageUrls(const Dali::Vector<Dali::String>& urls)
{
  mImageUrls   = urls;
  mUrl         = Dali::String();
  mVisualDirty = true;
  InvalidateMeasure();
}

Dali::Vector<Dali::String> AnimatedImageViewImpl::GetImageUrls() const
{
  return mImageUrls;
}

Ui::Visual::ResourceStatus AnimatedImageViewImpl::GetLoadingStatus() const
{
  return Internal::ViewDataImpl::Get(*this).GetVisualResourceStatus(AnimatedImageViewImpl::Property::IMAGE);
}

void AnimatedImageViewImpl::UpdateVisual()
{
  auto& viewData = Internal::ViewDataImpl::Get(*this);
  viewData.UnregisterVisual(AnimatedImageViewImpl::Property::IMAGE);

  const bool hasUrlArray = !mImageUrls.Empty();
  if(!hasUrlArray && mUrl.Empty())
  {
    return;
  }

  Dali::Property::Map map;
  map.Insert(Visual::Property::TYPE, Visual::ANIMATED_IMAGE);

  if(hasUrlArray)
  {
    Dali::Property::Array urlArray;
    for(auto i = 0u; i < mImageUrls.Size(); ++i)
    {
      urlArray.PushBack(mImageUrls[i]);
    }
    map.Insert(Ui::ImageVisual::Property::URL, urlArray);
  }
  else
  {
    map.Insert(Ui::ImageVisual::Property::URL, mUrl);
  }

  if(mLoopCount != -1)
  {
    map.Insert(Ui::DevelImageVisual::Property::LOOP_COUNT, mLoopCount);
  }

  if(mStopBehavior != AnimatedImageView::StopBehavior::CURRENT_FRAME)
  {
    map.Insert(Ui::DevelImageVisual::Property::STOP_BEHAVIOR, static_cast<int>(mStopBehavior));
  }

  if(mFrameSpeedFactor != 1.0f)
  {
    map.Insert(Ui::DevelImageVisual::Property::FRAME_SPEED_FACTOR, mFrameSpeedFactor);
  }

  map.Insert(Ui::ImageVisual::Property::BATCH_SIZE, mBatchSize);
  map.Insert(Ui::ImageVisual::Property::CACHE_SIZE, mCacheSize);

  if(mFrameDelay >= 0)
  {
    map.Insert(Ui::ImageVisual::Property::FRAME_DELAY, mFrameDelay);
  }

  if(mImageColor.Resolve() != Color::WHITE)
  {
    map.Insert(Visual::Property::MIX_COLOR, mImageColor.Resolve());
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

  if(mFittingMode != Ui::FittingMode::DONT_CARE)
  {
    map.Insert(Ui::ImageVisual::Property::FITTING_MODE, static_cast<int>(mFittingMode));
  }

  if(mSamplingMode != Ui::SamplingMode::LINEAR)
  {
    map.Insert(Ui::ImageVisual::Property::SAMPLING_MODE, static_cast<int>(mSamplingMode));
  }

  if(mImageLoadWithViewSize)
  {
    map.Insert(Ui::DevelImageVisual::Property::SYNCHRONOUS_SIZING, true);
  }

  if(!mAlphaMaskUrl.Empty())
  {
    map.Insert(Ui::ImageVisual::Property::ALPHA_MASK_URL, mAlphaMaskUrl);
  }

  if(mCropToMask)
  {
    map.Insert(Ui::DevelImageVisual::Property::CROP_TO_MASK, true);
  }

  if(mMaskingMode != Ui::MaskingType::MASKING_ON_LOADING)
  {
    map.Insert(Ui::DevelImageVisual::Property::MASKING_TYPE, static_cast<int>(mMaskingMode));
  }

  auto visualFactory = Ui::VisualFactory::Get();
  if(visualFactory)
  {
    mVisual = visualFactory.CreateVisual(map);
    if(mVisual)
    {
      viewData.RegisterVisual(AnimatedImageViewImpl::Property::IMAGE, mVisual);
    }
  }
}

void AnimatedImageViewImpl::UpdatePlaceholderVisual()
{
  auto& viewData = Internal::ViewDataImpl::Get(*this);
  viewData.UnregisterVisual(AnimatedImageViewImpl::Property::PLACEHOLDER_IMAGE);

  if(mPlaceholderUrl.Empty())
  {
    return;
  }

  // Don't show placeholder if main image is already loaded
  if(viewData.GetVisualResourceStatus(AnimatedImageViewImpl::Property::IMAGE) == Ui::Visual::ResourceStatus::READY)
  {
    return;
  }

  auto visualFactory = Ui::VisualFactory::Get();
  if(!visualFactory)
  {
    return;
  }

  Dali::Property::Map map;
  map.Insert(Visual::Property::TYPE, Visual::IMAGE);
  map.Insert(Ui::ImageVisual::Property::URL, mPlaceholderUrl);

  auto visual = visualFactory.CreateVisual(map);
  if(visual)
  {
    viewData.RegisterVisual(AnimatedImageViewImpl::Property::PLACEHOLDER_IMAGE, visual, DepthIndex::CONTENT + 1);
  }
}

void AnimatedImageViewImpl::ApplyFittingMode(const Vector2& size)
{
  if(!mVisual)
  {
    return;
  }

  auto& visualImpl = Ui::GetImplementation(mVisual);

  if(visualImpl.IsIgnoreFittingMode())
  {
    return;
  }

  if(mFittingMode == Ui::FittingMode::DONT_CARE)
  {
    mVisual.SetTransformAndSize(Dali::Property::Map(), size);
    return;
  }

  Extents padding = GetViewPadding();

  // Reset PIXEL_AREA after using OVER_FIT_KEEP_ASPECT_RATIO
  if(visualImpl.IsPixelAreaSetForFittingMode())
  {
    static const Vector4 FULL_TEXTURE_RECT(0.f, 0.f, 1.f, 1.f);
    visualImpl.SetPixelAreaForFittingMode(FULL_TEXTURE_RECT);
  }

  Vector2 finalSize   = size - Vector2(padding.start + padding.end, padding.top + padding.bottom);
  Vector2 finalOffset = Vector2(padding.start, padding.top);

  bool                zeroPadding = (padding == Extents());
  Dali::Property::Map transformMap;

  auto fittingMode = static_cast<DevelVisual::FittingMode>(mFittingMode);

  // FIT_WIDTH/FIT_HEIGHT: resolve to FIT_KEEP or OVER_FIT based on aspect ratio
  if(fittingMode == DevelVisual::FIT_WIDTH || fittingMode == DevelVisual::FIT_HEIGHT)
  {
    Vector2 naturalSize;
    visualImpl.GetNaturalSize(naturalSize);
    const float widthRatio  = !Dali::EqualsZero(naturalSize.width) ? (finalSize.width / naturalSize.width) : 0.0f;
    const float heightRatio = !Dali::EqualsZero(naturalSize.height) ? (finalSize.height / naturalSize.height) : 0.0f;
    if(widthRatio < heightRatio)
    {
      fittingMode = (fittingMode == DevelVisual::FIT_WIDTH) ? DevelVisual::FIT_KEEP_ASPECT_RATIO : DevelVisual::OVER_FIT_KEEP_ASPECT_RATIO;
    }
    else
    {
      fittingMode = (fittingMode == DevelVisual::FIT_WIDTH) ? DevelVisual::OVER_FIT_KEEP_ASPECT_RATIO : DevelVisual::FIT_KEEP_ASPECT_RATIO;
    }
  }

  if((!zeroPadding) || (fittingMode != DevelVisual::FILL))
  {
    visualImpl.SetTransformMapUsageForFittingMode(true);

    Vector2 naturalSize;
    if(fittingMode != DevelVisual::FILL)
    {
      visualImpl.GetNaturalSize(naturalSize);
    }

    switch(fittingMode)
    {
      case DevelVisual::FIT_KEEP_ASPECT_RATIO:
      {
        auto availableVisualSize = finalSize;
        finalSize                = naturalSize * std::min((!Dali::EqualsZero(naturalSize.width) ? (availableVisualSize.width / naturalSize.width) : 0.0f),
                                                          (!Dali::EqualsZero(naturalSize.height) ? (availableVisualSize.height / naturalSize.height) : 0.0f));
        finalOffset += (availableVisualSize - finalSize) * 0.5f;
        transformMap.Add(Visual::Transform::Property::OFFSET, finalOffset)
          .Add(Visual::Transform::Property::SIZE, finalSize);
        break;
      }
      case DevelVisual::OVER_FIT_KEEP_ASPECT_RATIO:
      {
        auto availableVisualSize = finalSize;
        finalSize                = naturalSize * std::max((!Dali::EqualsZero(naturalSize.width) ? (availableVisualSize.width / naturalSize.width) : 0.0f),
                                                          (!Dali::EqualsZero(naturalSize.height) ? (availableVisualSize.height / naturalSize.height) : 0.0f));
        auto originalOffset      = finalOffset;
        if(!visualImpl.IsPixelAreaSetForFittingMode() && !Dali::EqualsZero(finalSize.width) && !Dali::EqualsZero(finalSize.height))
        {
          float   x           = std::abs((availableVisualSize.width - finalSize.width) / finalSize.width) * 0.5f;
          float   y           = std::abs((availableVisualSize.height - finalSize.height) / finalSize.height) * 0.5f;
          float   widthRatio  = 1.f - std::abs((availableVisualSize.width - finalSize.width) / finalSize.width);
          float   heightRatio = 1.f - std::abs((availableVisualSize.height - finalSize.height) / finalSize.height);
          Vector4 pixelArea(x, y, widthRatio, heightRatio);
          visualImpl.SetPixelAreaForFittingMode(pixelArea);
        }
        transformMap.Add(Visual::Transform::Property::OFFSET, originalOffset)
          .Add(Visual::Transform::Property::SIZE, availableVisualSize);
        break;
      }
      case DevelVisual::CENTER:
      {
        auto availableVisualSize = finalSize;
        if(availableVisualSize.width > naturalSize.width && availableVisualSize.height > naturalSize.height)
        {
          finalSize = naturalSize;
        }
        else
        {
          finalSize = naturalSize * std::min((!Dali::EqualsZero(naturalSize.width) ? (availableVisualSize.width / naturalSize.width) : 0.0f),
                                             (!Dali::EqualsZero(naturalSize.height) ? (availableVisualSize.height / naturalSize.height) : 0.0f));
        }
        finalOffset += (availableVisualSize - finalSize) * 0.5f;
        transformMap.Add(Visual::Transform::Property::OFFSET, finalOffset)
          .Add(Visual::Transform::Property::SIZE, finalSize);
        break;
      }
      case DevelVisual::FILL:
      default:
      {
        transformMap.Add(Visual::Transform::Property::OFFSET, finalOffset)
          .Add(Visual::Transform::Property::SIZE, finalSize);
        break;
      }
    }

    transformMap.Add(Visual::Transform::Property::OFFSET_POLICY,
                     Vector2(Visual::Transform::Policy::ABSOLUTE, Visual::Transform::Policy::ABSOLUTE))
      .Add(Visual::Transform::Property::ORIGIN, Align::TOP_BEGIN)
      .Add(Visual::Transform::Property::ANCHOR_POINT, Align::TOP_BEGIN)
      .Add(Visual::Transform::Property::SIZE_POLICY,
           Vector2(Visual::Transform::Policy::ABSOLUTE, Visual::Transform::Policy::ABSOLUTE));
  }
  else if(visualImpl.IsTransformMapSetForFittingMode() && zeroPadding)
  {
    visualImpl.SetTransformMapUsageForFittingMode(false);
    transformMap.Add(Visual::Transform::Property::OFFSET, Vector2::ZERO)
      .Add(Visual::Transform::Property::OFFSET_POLICY,
           Vector2(Visual::Transform::Policy::RELATIVE, Visual::Transform::Policy::RELATIVE))
      .Add(Visual::Transform::Property::SIZE, Vector2::ONE)
      .Add(Visual::Transform::Property::SIZE_POLICY,
           Vector2(Visual::Transform::Policy::RELATIVE, Visual::Transform::Policy::RELATIVE));
  }

  mVisual.SetTransformAndSize(transformMap, size);
}

void AnimatedImageViewImpl::OnViewResourceReady(Ui::View view)
{
  auto& viewData = Internal::ViewDataImpl::Get(*this);
  if(viewData.GetVisualResourceStatus(AnimatedImageViewImpl::Property::IMAGE) != Ui::Visual::ResourceStatus::READY)
  {
    return;
  }

  // Main image is ready: remove placeholder
  viewData.UnregisterVisual(AnimatedImageViewImpl::Property::PLACEHOLDER_IMAGE);

  // Request a re-layout now that the natural size is known
  InvalidateMeasure();
}

} // namespace Integration
} // namespace Ui
} // namespace Dali
