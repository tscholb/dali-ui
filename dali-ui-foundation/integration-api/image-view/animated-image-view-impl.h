#pragma once

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

#include <dali-ui-foundation/devel-api/view-depth-index-ranges.h>
#include <dali-ui-foundation/devel-api/visual-factory/visual-base.h>
#include <dali-ui-foundation/devel-api/visuals/image-visual-properties-devel.h>
#include <dali-ui-foundation/integration-api/view-impl.h>
#include <dali-ui-foundation/public-api/image-view/animated-image-view-properties.h>
#include <dali-ui-foundation/public-api/image-view/animated-image-view.h>
#include <dali-ui-foundation/public-api/ui-color.h>
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/signals/dali-signal.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

class AnimatedImageViewImpl;
using AnimatedImageViewImplPtr = IntrusivePtr<AnimatedImageViewImpl>;

/**
 * @brief This is the internal implementation class for AnimatedImageView.
 *
 * @see Dali::Ui::AnimatedImageView
 */
class DALI_UI_API AnimatedImageViewImpl : public ViewImpl
{
public: // Properties
  /**
   * @brief Property indices — aliased from the shared AnimatedImageViewPropertyIndex.
   *
   * Keeping a local Property struct means existing impl.cpp code (e.g.
   * AnimatedImageViewImpl::Property::IMAGE) continues to compile unchanged,
   * while the public header (animated-image-view.h) exposes the same values
   * through AnimatedImageView::Property.
   */
  struct Property
  {
    enum
    {
      IMAGE                     = Ui::AnimatedImageViewPropertyIndex::IMAGE,
      LOOP_COUNT                = Ui::AnimatedImageViewPropertyIndex::LOOP_COUNT,
      IMAGE_COLOR               = Ui::AnimatedImageViewPropertyIndex::IMAGE_COLOR,
      STOP_BEHAVIOR             = Ui::AnimatedImageViewPropertyIndex::STOP_BEHAVIOR,
      FRAME_SPEED_FACTOR        = Ui::AnimatedImageViewPropertyIndex::FRAME_SPEED_FACTOR,
      BATCH_SIZE                = Ui::AnimatedImageViewPropertyIndex::BATCH_SIZE,
      CACHE_SIZE                = Ui::AnimatedImageViewPropertyIndex::CACHE_SIZE,
      FRAME_DELAY               = Ui::AnimatedImageViewPropertyIndex::FRAME_DELAY,
      DESIRED_WIDTH             = Ui::AnimatedImageViewPropertyIndex::DESIRED_WIDTH,
      DESIRED_HEIGHT            = Ui::AnimatedImageViewPropertyIndex::DESIRED_HEIGHT,
      RELEASE_POLICY            = Ui::AnimatedImageViewPropertyIndex::RELEASE_POLICY,
      SYNCHRONOUS_LOADING       = Ui::AnimatedImageViewPropertyIndex::SYNCHRONOUS_LOADING,
      PRE_MULTIPLIED_ALPHA      = Ui::AnimatedImageViewPropertyIndex::PRE_MULTIPLIED_ALPHA,
      FITTING_MODE              = Ui::AnimatedImageViewPropertyIndex::FITTING_MODE,
      SAMPLING_MODE             = Ui::AnimatedImageViewPropertyIndex::SAMPLING_MODE,
      IMAGE_LOAD_WITH_VIEW_SIZE = Ui::AnimatedImageViewPropertyIndex::IMAGE_LOAD_WITH_VIEW_SIZE,
      ALPHA_MASK_URL            = Ui::AnimatedImageViewPropertyIndex::ALPHA_MASK_URL,
      CROP_TO_MASK              = Ui::AnimatedImageViewPropertyIndex::CROP_TO_MASK,
      MASKING_MODE              = Ui::AnimatedImageViewPropertyIndex::MASKING_MODE,
      PLACEHOLDER_IMAGE         = Ui::AnimatedImageViewPropertyIndex::PLACEHOLDER_IMAGE,
      IMAGE_URLS                = Ui::AnimatedImageViewPropertyIndex::IMAGE_URLS,
    };
  };

protected: // Construction & Destruction
  /**
   * @brief AnimatedImageViewImpl constructor.
   */
  AnimatedImageViewImpl();

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  virtual ~AnimatedImageViewImpl();

public: // Creation
  /**
   * @brief Creates a new AnimatedImageViewImpl.
   *
   * @return An intrusive pointer to the newly allocated AnimatedImageViewImpl
   */
  static AnimatedImageViewImplPtr New();

public: // Properties (required by DALI_PROPERTY_REGISTRATION)
  /// @cond internal
  static void                  SetProperty(Dali::BaseObject* object, Dali::Property::Index index, const Dali::Property::Value& value);
  static Dali::Property::Value GetProperty(Dali::BaseObject* object, Dali::Property::Index index);
  /// @endcond

public: // API
  /**
   * @copydoc Dali::Ui::AnimatedImageView::SetResourceUrl
   */
  void SetResourceUrl(const Dali::String& url);

  /**
   * @copydoc Dali::Ui::AnimatedImageView::GetResourceUrl
   */
  Dali::String GetResourceUrl() const;

  /**
   * @copydoc Dali::Ui::AnimatedImageView::Play
   */
  void Play();

  /**
   * @copydoc Dali::Ui::AnimatedImageView::Pause
   */
  void Pause();

  /**
   * @copydoc Dali::Ui::AnimatedImageView::Stop
   */
  void Stop();

  /**
   * @copydoc Dali::Ui::AnimatedImageView::SetLoopCount
   */
  void SetLoopCount(int count);

  /**
   * @copydoc Dali::Ui::AnimatedImageView::GetLoopCount
   */
  int GetLoopCount() const;

  /**
   * @copydoc Dali::Ui::AnimatedImageView::SetImageColor
   */
  void SetImageColor(const UiColor& color);

  /**
   * @copydoc Dali::Ui::AnimatedImageView::GetImageColor
   */
  UiColor GetImageColor() const;

  /// @copydoc Dali::Ui::AnimatedImageView::SetFittingMode
  void SetFittingMode(Ui::FittingMode::Type fittingMode);

  /// @copydoc Dali::Ui::AnimatedImageView::GetFittingMode
  Ui::FittingMode::Type GetFittingMode() const;

  /// @copydoc Dali::Ui::AnimatedImageView::SetSamplingMode
  void SetSamplingMode(Ui::SamplingMode::Type samplingMode);

  /// @copydoc Dali::Ui::AnimatedImageView::GetSamplingMode
  Ui::SamplingMode::Type GetSamplingMode() const;

  /// @copydoc Dali::Ui::AnimatedImageView::SetImageLoadWithViewSize
  void SetImageLoadWithViewSize(bool enabled);

  /// @copydoc Dali::Ui::AnimatedImageView::IsImageLoadWithViewSize
  bool IsImageLoadWithViewSize() const;

  /// @copydoc Dali::Ui::AnimatedImageView::SetAlphaMaskUrl
  void SetAlphaMaskUrl(const Dali::String& maskUrl);

  /// @copydoc Dali::Ui::AnimatedImageView::GetAlphaMaskUrl
  Dali::String GetAlphaMaskUrl() const;

  /// @copydoc Dali::Ui::AnimatedImageView::SetCropToMask
  void SetCropToMask(bool cropToMask);

  /// @copydoc Dali::Ui::AnimatedImageView::IsCropToMask
  bool IsCropToMask() const;

  /// @copydoc Dali::Ui::AnimatedImageView::SetMaskingMode
  void SetMaskingMode(Ui::MaskingType::Type maskingMode);

  /// @copydoc Dali::Ui::AnimatedImageView::GetMaskingMode
  Ui::MaskingType::Type GetMaskingMode() const;

  /// @copydoc Dali::Ui::AnimatedImageView::SetPlaceholderUrl
  void SetPlaceholderUrl(const Dali::String& url);

  /// @copydoc Dali::Ui::AnimatedImageView::GetPlaceholderUrl
  Dali::String GetPlaceholderUrl() const;

  /// @copydoc Dali::Ui::AnimatedImageView::SetImageUrls
  void SetImageUrls(const Dali::Vector<Dali::String>& urls);

  /// @copydoc Dali::Ui::AnimatedImageView::GetImageUrls
  Dali::Vector<Dali::String> GetImageUrls() const;

  /// @copydoc Dali::Ui::AnimatedImageView::JumpToFrame
  void JumpToFrame(int frame);

  /// @copydoc Dali::Ui::AnimatedImageView::SetStopBehavior
  void SetStopBehavior(AnimatedImageView::StopBehavior::Type behavior);

  /// @copydoc Dali::Ui::AnimatedImageView::GetStopBehavior
  AnimatedImageView::StopBehavior::Type GetStopBehavior() const;

  /// @copydoc Dali::Ui::AnimatedImageView::SetFrameSpeedFactor
  void SetFrameSpeedFactor(float factor);

  /// @copydoc Dali::Ui::AnimatedImageView::GetFrameSpeedFactor
  float GetFrameSpeedFactor() const;

  /// @copydoc Dali::Ui::AnimatedImageView::SetBatchSize
  void SetBatchSize(int size);

  /// @copydoc Dali::Ui::AnimatedImageView::GetBatchSize
  int GetBatchSize() const;

  /// @copydoc Dali::Ui::AnimatedImageView::SetCacheSize
  void SetCacheSize(int size);

  /// @copydoc Dali::Ui::AnimatedImageView::GetCacheSize
  int GetCacheSize() const;

  /// @copydoc Dali::Ui::AnimatedImageView::SetFrameDelay
  void SetFrameDelay(int milliseconds);

  /// @copydoc Dali::Ui::AnimatedImageView::GetFrameDelay
  int GetFrameDelay() const;

  /// @copydoc Dali::Ui::AnimatedImageView::GetPlayState
  AnimatedImageView::PlayState::Type GetPlayState() const;

  /// @copydoc Dali::Ui::AnimatedImageView::GetCurrentFrame
  int GetCurrentFrame() const;

  /// @copydoc Dali::Ui::AnimatedImageView::GetTotalFrame
  int GetTotalFrame() const;

  /// @copydoc Dali::Ui::AnimatedImageView::SetDesiredWidth
  void SetDesiredWidth(int width);

  /// @copydoc Dali::Ui::AnimatedImageView::GetDesiredWidth
  int GetDesiredWidth() const;

  /// @copydoc Dali::Ui::AnimatedImageView::SetDesiredHeight
  void SetDesiredHeight(int height);

  /// @copydoc Dali::Ui::AnimatedImageView::GetDesiredHeight
  int GetDesiredHeight() const;

  /// @copydoc Dali::Ui::AnimatedImageView::SetReleasePolicy
  void SetReleasePolicy(Ui::ReleasePolicy::Type releasePolicy);

  /// @copydoc Dali::Ui::AnimatedImageView::GetReleasePolicy
  Ui::ReleasePolicy::Type GetReleasePolicy() const;

  /// @copydoc Dali::Ui::AnimatedImageView::SetSynchronousLoading
  void SetSynchronousLoading(bool synchronous);

  /// @copydoc Dali::Ui::AnimatedImageView::IsSynchronousLoading
  bool IsSynchronousLoading() const;

  /// @copydoc Dali::Ui::AnimatedImageView::SetPreMultipliedAlpha
  void SetPreMultipliedAlpha(bool preMultiplied);

  /// @copydoc Dali::Ui::AnimatedImageView::IsPreMultipliedAlpha
  bool IsPreMultipliedAlpha() const;

public: // Loading Status & Signals
  /**
   * @copydoc Dali::Ui::AnimatedImageView::GetLoadingStatus
   */
  Ui::Visual::ResourceStatus GetLoadingStatus() const;

  /// @copydoc Dali::Ui::AnimatedImageView::AnimationFinishedSignal
  AnimatedImageView::AnimationFinishedSignalType& AnimationFinishedSignal();

private: // From ViewImpl
  /**
   * @copydoc Integration::ViewImpl::OnInitialize
   */
  void OnInitialize() override;

  /**
   * @copydoc Integration::ViewImpl::OnMeasure
   */
  MeasuredSize OnMeasure(float widthConstraint, float heightConstraint) override;

  /**
   * @copydoc Integration::ViewImpl::OnArrange
   */
  MeasuredSize OnArrange(const LayoutRect& bounds) override;

private: // Internal methods
  /**
   * @brief Rebuilds and re-registers the image visual from current property values.
   */
  void UpdateVisual();

  /**
   * @brief Rebuilds and re-registers the placeholder visual.
   */
  void UpdatePlaceholderVisual();

  /**
   * @brief Applies the current fitting mode to the visual.
   *
   * @param[in] size The size of the view
   */
  void ApplyFittingMode(const Vector2& size);

  /**
   * @brief Called when a visual event signal is emitted.
   *
   * Used to intercept ANIMATION_FINISHED and forward it as AnimationFinishedSignal.
   */
  void OnVisualEvent(View view, Dali::Property::Index visualIndex, Dali::Property::Index signalId);

  /**
   * @brief Called when the view's resource becomes ready.
   *
   * Used to remove placeholder when main image is loaded.
   */
  void OnViewResourceReady(Ui::View view);

private:
  // Not copyable or movable
  AnimatedImageViewImpl(const AnimatedImageViewImpl&)            = delete;
  AnimatedImageViewImpl(AnimatedImageViewImpl&&)                 = delete;
  AnimatedImageViewImpl& operator=(const AnimatedImageViewImpl&) = delete;
  AnimatedImageViewImpl& operator=(AnimatedImageViewImpl&&)      = delete;

private: // Data
  Dali::String mUrl;
  int          mLoopCount;
  UiColor      mImageColor;

  AnimatedImageView::StopBehavior::Type mStopBehavior;
  float                                 mFrameSpeedFactor;
  int                                   mBatchSize;
  int                                   mCacheSize;
  int                                   mFrameDelay;

  int                     mDesiredWidth;
  int                     mDesiredHeight;
  Ui::ReleasePolicy::Type mReleasePolicy;
  bool                    mSynchronousLoading;
  bool                    mPreMultipliedAlpha;

  Ui::FittingMode::Type  mFittingMode;
  Ui::SamplingMode::Type mSamplingMode;
  bool                   mImageLoadWithViewSize;

  Dali::String          mAlphaMaskUrl;
  bool                  mCropToMask;
  Ui::MaskingType::Type mMaskingMode;
  Dali::String          mPlaceholderUrl;

  Dali::Vector<Dali::String> mImageUrls;

  Ui::Visual::Base mVisual;      ///< The main image visual
  bool             mVisualDirty; ///< True when a property changed and the visual needs rebuilding on the next measure pass

  AnimatedImageView::AnimationFinishedSignalType mAnimationFinishedSignal;
};

} // namespace Integration
} // namespace Ui
} // namespace Dali
