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

#include <dali-ui-test-suite-utils.h>
#include <dali.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/image-view/animated-image-view.h>

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_animatedimageview_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_animatedimageview_cleanup(void)
{
  test_return_value = TET_PASS;
}

// Construction

int UtcDaliAnimatedImageViewConstructorP(void)
{
  UiTestApplication application;
  AnimatedImageView view;
  DALI_TEST_CHECK(!view);
  END_TEST;
}

int UtcDaliAnimatedImageViewNewP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliAnimatedImageViewNewWithUrlP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New("test.gif");
  DALI_TEST_CHECK(view);
  DALI_TEST_EQUALS(view.GetResourceUrl(), Dali::String("test.gif"), TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimatedImageViewCopyConstructorP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();
  AnimatedImageView copy(view);
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(view == copy);
  END_TEST;
}

int UtcDaliAnimatedImageViewMoveConstructorP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();
  AnimatedImageView moved(std::move(view));
  DALI_TEST_CHECK(moved);
  END_TEST;
}

int UtcDaliAnimatedImageViewAssignmentOperatorP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();
  AnimatedImageView copy;
  copy = view;
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(view == copy);
  END_TEST;
}

int UtcDaliAnimatedImageViewMoveAssignmentOperatorP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();
  AnimatedImageView moved;
  moved = std::move(view);
  DALI_TEST_CHECK(moved);
  END_TEST;
}

// DownCast

int UtcDaliAnimatedImageViewDownCastP(void)
{
  UiTestApplication application;
  AnimatedImageView view     = AnimatedImageView::New();
  View              baseView = view;
  AnimatedImageView downcast = AnimatedImageView::DownCast(baseView);
  DALI_TEST_CHECK(downcast);
  END_TEST;
}

int UtcDaliAnimatedImageViewDownCastN(void)
{
  UiTestApplication application;
  View              baseView = View::New();
  AnimatedImageView downcast = AnimatedImageView::DownCast(baseView);
  DALI_TEST_CHECK(!downcast);
  END_TEST;
}

// SetResourceUrl / GetResourceUrl

int UtcDaliAnimatedImageViewSetGetImageP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();
  view.SetResourceUrl("animation.gif");
  DALI_TEST_EQUALS(view.GetResourceUrl(), Dali::String("animation.gif"), TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimatedImageViewSetImageNoChangeP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New("animation.gif");
  view.SetResourceUrl("animation.gif"); // same url — no update
  DALI_TEST_EQUALS(view.GetResourceUrl(), Dali::String("animation.gif"), TEST_LOCATION);
  END_TEST;
}

// LoopCount

int UtcDaliAnimatedImageViewSetGetLoopCountP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();
  DALI_TEST_EQUALS(view.GetLoopCount(), -1, TEST_LOCATION); // default: infinite

  view.SetLoopCount(3);
  DALI_TEST_EQUALS(view.GetLoopCount(), 3, TEST_LOCATION);

  view.SetLoopCount(0);
  DALI_TEST_EQUALS(view.GetLoopCount(), 0, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimatedImageViewSetLoopCountNoChangeP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();
  view.SetLoopCount(5);
  view.SetLoopCount(5); // same value — no update
  DALI_TEST_EQUALS(view.GetLoopCount(), 5, TEST_LOCATION);
  END_TEST;
}

// ImageColor

int UtcDaliAnimatedImageViewSetGetImageColorP(void)
{
  UiTestApplication application;
  AnimatedImageView view  = AnimatedImageView::New();
  UiColor           color(1.0f, 0.5f, 0.5f, 1.0f);
  view.SetImageColor(color);
  DALI_TEST_EQUALS(view.GetImageColor().Resolve(), color.Resolve(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimatedImageViewSetImageColorNoChangeP(void)
{
  UiTestApplication application;
  AnimatedImageView view  = AnimatedImageView::New();
  UiColor           white(Color::WHITE);
  view.SetImageColor(white); // same as default — no update
  DALI_TEST_EQUALS(view.GetImageColor().Resolve(), white.Resolve(), TEST_LOCATION);
  END_TEST;
}

// Play / Pause / Stop (no visual — should not crash)

int UtcDaliAnimatedImageViewPlayNoVisualP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();
  view.Play();
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliAnimatedImageViewPauseNoVisualP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();
  view.Pause();
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliAnimatedImageViewStopNoVisualP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();
  view.Stop();
  DALI_TEST_CHECK(view);
  END_TEST;
}

// GetLoadingStatus

int UtcDaliAnimatedImageViewGetLoadingStatusP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New("animation.gif");
  // Before scene add, status may be PREPARING, READY, or FAILED
  Ui::Visual::ResourceStatus status = view.GetLoadingStatus();
  DALI_TEST_CHECK(status == Ui::Visual::ResourceStatus::PREPARING ||
                  status == Ui::Visual::ResourceStatus::READY ||
                  status == Ui::Visual::ResourceStatus::FAILED);
  END_TEST;
}

// ResourceReadySignal

int UtcDaliAnimatedImageViewSignalsP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();
  // ResourceReadySignal is inherited from View — just call, don't store
  view.ResourceReadySignal();
  DALI_TEST_CHECK(true);
  END_TEST;
}

// Fluent chaining

// Property IMAGE set/get via Property::Index

int UtcDaliAnimatedImageViewPropertyImageP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  view.SetProperty(Dali::Property::Index(Dali::PROPERTY_REGISTRATION_START_INDEX), // IMAGE
                   Dali::String("animation.gif"));
  Dali::Property::Value value = view.GetProperty(Dali::Property::Index(Dali::PROPERTY_REGISTRATION_START_INDEX));
  Dali::String url;
  DALI_TEST_CHECK(value.Get(url));
  DALI_TEST_EQUALS(url, Dali::String("animation.gif"), TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimatedImageViewChainingP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New()
                             .SetResourceUrl("animation.gif")
                             .SetLoopCount(2)
                             .SetImageColor(UiColor(1.0f, 1.0f, 1.0f, 0.5f));
  DALI_TEST_CHECK(view);
  DALI_TEST_EQUALS(view.GetResourceUrl(), Dali::String("animation.gif"), TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetLoopCount(), 2, TEST_LOCATION);
  END_TEST;
}

// =============================================================================
// Validation Tests - New Functionality
// =============================================================================

// StopBehavior

int UtcDaliAnimatedImageViewSetGetStopBehaviorP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  // Default should be CURRENT_FRAME
  DALI_TEST_EQUALS(view.GetStopBehavior(), AnimatedImageView::StopBehavior::CURRENT_FRAME, TEST_LOCATION);

  view.SetStopBehavior(AnimatedImageView::StopBehavior::FIRST_FRAME);
  DALI_TEST_EQUALS(view.GetStopBehavior(), AnimatedImageView::StopBehavior::FIRST_FRAME, TEST_LOCATION);

  view.SetStopBehavior(AnimatedImageView::StopBehavior::LAST_FRAME);
  DALI_TEST_EQUALS(view.GetStopBehavior(), AnimatedImageView::StopBehavior::LAST_FRAME, TEST_LOCATION);

  // Set back to default
  view.SetStopBehavior(AnimatedImageView::StopBehavior::CURRENT_FRAME);
  DALI_TEST_EQUALS(view.GetStopBehavior(), AnimatedImageView::StopBehavior::CURRENT_FRAME, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimatedViewSetStopBehaviorNoChangeP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();
  view.SetStopBehavior(AnimatedImageView::StopBehavior::LAST_FRAME);
  view.SetStopBehavior(AnimatedImageView::StopBehavior::LAST_FRAME); // same value
  DALI_TEST_EQUALS(view.GetStopBehavior(), AnimatedImageView::StopBehavior::LAST_FRAME, TEST_LOCATION);
  END_TEST;
}

// FrameSpeedFactor

int UtcDaliAnimatedViewSetGetFrameSpeedFactorP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  // Default should be 1.0f
  DALI_TEST_EQUALS(view.GetFrameSpeedFactor(), 1.0f, TEST_LOCATION);

  view.SetFrameSpeedFactor(0.5f);
  DALI_TEST_EQUALS(view.GetFrameSpeedFactor(), 0.5f, TEST_LOCATION);

  view.SetFrameSpeedFactor(2.0f);
  DALI_TEST_EQUALS(view.GetFrameSpeedFactor(), 2.0f, TEST_LOCATION);

  // Reset to default
  view.SetFrameSpeedFactor(1.0f);
  DALI_TEST_EQUALS(view.GetFrameSpeedFactor(), 1.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimatedViewSetFrameSpeedFactorNoChangeP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();
  view.SetFrameSpeedFactor(1.5f);
  view.SetFrameSpeedFactor(1.5f); // same value
  DALI_TEST_EQUALS(view.GetFrameSpeedFactor(), 1.5f, TEST_LOCATION);
  END_TEST;
}

// BatchSize

int UtcDaliAnimatedViewSetGetBatchSizeP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  // Default should be 1
  DALI_TEST_EQUALS(view.GetBatchSize(), 1, TEST_LOCATION);

  view.SetBatchSize(3);
  DALI_TEST_EQUALS(view.GetBatchSize(), 3, TEST_LOCATION);

  view.SetBatchSize(10);
  DALI_TEST_EQUALS(view.GetBatchSize(), 10, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimatedViewSetBatchSizeNoChangeP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();
  view.SetBatchSize(5);
  view.SetBatchSize(5); // same value
  DALI_TEST_EQUALS(view.GetBatchSize(), 5, TEST_LOCATION);
  END_TEST;
}

// CacheSize

int UtcDaliAnimatedViewSetGetCacheSizeP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  // Default should be 1
  DALI_TEST_EQUALS(view.GetCacheSize(), 1, TEST_LOCATION);

  view.SetCacheSize(5);
  DALI_TEST_EQUALS(view.GetCacheSize(), 5, TEST_LOCATION);

  view.SetCacheSize(20);
  DALI_TEST_EQUALS(view.GetCacheSize(), 20, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimatedViewSetCacheSizeNoChangeP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();
  view.SetCacheSize(10);
  view.SetCacheSize(10); // same value
  DALI_TEST_EQUALS(view.GetCacheSize(), 10, TEST_LOCATION);
  END_TEST;
}

// FrameDelay

int UtcDaliAnimatedViewSetGetFrameDelayP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  // Default should be -1 (use file default)
  DALI_TEST_EQUALS(view.GetFrameDelay(), -1, TEST_LOCATION);

  view.SetFrameDelay(33); // 30 FPS
  DALI_TEST_EQUALS(view.GetFrameDelay(), 33, TEST_LOCATION);

  view.SetFrameDelay(100); // 10 FPS
  DALI_TEST_EQUALS(view.GetFrameDelay(), 100, TEST_LOCATION);

  // Reset to default
  view.SetFrameDelay(-1);
  DALI_TEST_EQUALS(view.GetFrameDelay(), -1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimatedViewSetFrameDelayNoChangeP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();
  view.SetFrameDelay(50);
  view.SetFrameDelay(50); // same value
  DALI_TEST_EQUALS(view.GetFrameDelay(), 50, TEST_LOCATION);
  END_TEST;
}

// DesiredWidth / DesiredHeight

int UtcDaliAnimatedViewSetGetDesiredWidthP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  // Default should be 0 (use natural size)
  DALI_TEST_EQUALS(view.GetDesiredWidth(), 0, TEST_LOCATION);

  view.SetDesiredWidth(1920);
  DALI_TEST_EQUALS(view.GetDesiredWidth(), 1920, TEST_LOCATION);

  view.SetDesiredWidth(640);
  DALI_TEST_EQUALS(view.GetDesiredWidth(), 640, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimatedViewSetGetDesiredHeightP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  // Default should be 0 (use natural size)
  DALI_TEST_EQUALS(view.GetDesiredHeight(), 0, TEST_LOCATION);

  view.SetDesiredHeight(1080);
  DALI_TEST_EQUALS(view.GetDesiredHeight(), 1080, TEST_LOCATION);

  view.SetDesiredHeight(480);
  DALI_TEST_EQUALS(view.GetDesiredHeight(), 480, TEST_LOCATION);
  END_TEST;
}

// ReleasePolicy

int UtcDaliAnimatedViewSetGetReleasePolicyP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  // Default should be DETACHED
  DALI_TEST_EQUALS(view.GetReleasePolicy(), Ui::ReleasePolicy::DETACHED, TEST_LOCATION);

  view.SetReleasePolicy(Ui::ReleasePolicy::NEVER);
  DALI_TEST_EQUALS(view.GetReleasePolicy(), Ui::ReleasePolicy::NEVER, TEST_LOCATION);

  view.SetReleasePolicy(Ui::ReleasePolicy::DESTROYED);
  DALI_TEST_EQUALS(view.GetReleasePolicy(), Ui::ReleasePolicy::DESTROYED, TEST_LOCATION);

  // Reset to default
  view.SetReleasePolicy(Ui::ReleasePolicy::DETACHED);
  DALI_TEST_EQUALS(view.GetReleasePolicy(), Ui::ReleasePolicy::DETACHED, TEST_LOCATION);
  END_TEST;
}

// SynchronousLoading

int UtcDaliAnimatedViewSetGetSynchronousLoadingP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  // Default should be false
  DALI_TEST_CHECK(!view.GetSynchronousLoading());

  view.SetSynchronousLoading(true);
  DALI_TEST_CHECK(view.GetSynchronousLoading());

  view.SetSynchronousLoading(false);
  DALI_TEST_CHECK(!view.GetSynchronousLoading());
  END_TEST;
}

// PreMultipliedAlpha

int UtcDaliAnimatedViewSetGetPreMultipliedAlphaP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  // Default should be false
  DALI_TEST_CHECK(!view.GetPreMultipliedAlpha());

  view.SetPreMultipliedAlpha(true);
  DALI_TEST_CHECK(view.GetPreMultipliedAlpha());

  view.SetPreMultipliedAlpha(false);
  DALI_TEST_CHECK(!view.GetPreMultipliedAlpha());
  END_TEST;
}

// JumpToFrame

int UtcDaliAnimatedViewJumpToFrameP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  // Should not crash even without visual
  view.JumpToFrame(5);
  DALI_TEST_CHECK(view);

  view.JumpToFrame(0);
  DALI_TEST_CHECK(view);

  view.JumpToFrame(100);
  DALI_TEST_CHECK(view);
  END_TEST;
}

// GetPlayState

int UtcDaliAnimatedViewGetPlayStateP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  // Without visual, should return STOPPED
  DALI_TEST_EQUALS(view.GetPlayState(), AnimatedImageView::PlayState::STOPPED, TEST_LOCATION);

  view.Play();
  DALI_TEST_CHECK(view);

  view.Pause();
  DALI_TEST_CHECK(view);

  view.Stop();
  DALI_TEST_EQUALS(view.GetPlayState(), AnimatedImageView::PlayState::STOPPED, TEST_LOCATION);
  END_TEST;
}

// GetCurrentFrame / GetTotalFrame

int UtcDaliAnimatedViewGetCurrentFrameP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  // Without visual, should return 0
  DALI_TEST_EQUALS(view.GetCurrentFrame(), 0, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimatedViewGetTotalFrameP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  // Without visual, should return 0
  DALI_TEST_EQUALS(view.GetTotalFrame(), 0, TEST_LOCATION);
  END_TEST;
}

// AnimationFinishedSignal

int UtcDaliAnimatedViewAnimationFinishedSignalP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  // Just call to ensure it doesn't crash
  view.AnimationFinishedSignal();
  DALI_TEST_CHECK(view);
  END_TEST;
}

// Property registration - all new properties

int UtcDaliAnimatedViewPropertyLoopCountP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  const int index = AnimatedImageView::Property::LOOP_COUNT;

  view.SetProperty(index, 5);
  DALI_TEST_EQUALS(view.GetProperty(index).Get<int>(), 5, TEST_LOCATION);

  view.SetProperty(index, -1);
  DALI_TEST_EQUALS(view.GetProperty(index).Get<int>(), -1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimatedViewPropertyStopBehaviorP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  const int index = AnimatedImageView::Property::STOP_BEHAVIOR;

  view.SetProperty(index, static_cast<int>(AnimatedImageView::StopBehavior::LAST_FRAME));
  DALI_TEST_EQUALS(view.GetProperty(index).Get<int>(),
                   static_cast<int>(AnimatedImageView::StopBehavior::LAST_FRAME), TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimatedViewPropertyFrameSpeedFactorP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  const int index = AnimatedImageView::Property::FRAME_SPEED_FACTOR;

  view.SetProperty(index, 1.5f);
  DALI_TEST_EQUALS(view.GetProperty(index).Get<float>(), 1.5f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimatedViewPropertyBatchSizeP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  const int index = AnimatedImageView::Property::BATCH_SIZE;

  view.SetProperty(index, 3);
  DALI_TEST_EQUALS(view.GetProperty(index).Get<int>(), 3, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimatedViewPropertyCacheSizeP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  const int index = AnimatedImageView::Property::CACHE_SIZE;

  view.SetProperty(index, 5);
  DALI_TEST_EQUALS(view.GetProperty(index).Get<int>(), 5, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimatedViewPropertyFrameDelayP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  const int index = AnimatedImageView::Property::FRAME_DELAY;

  view.SetProperty(index, 33);
  DALI_TEST_EQUALS(view.GetProperty(index).Get<int>(), 33, TEST_LOCATION);

  view.SetProperty(index, -1);
  DALI_TEST_EQUALS(view.GetProperty(index).Get<int>(), -1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimatedViewPropertyDesiredWidthP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  const int index = AnimatedImageView::Property::DESIRED_WIDTH;

  view.SetProperty(index, 1920);
  DALI_TEST_EQUALS(view.GetProperty(index).Get<int>(), 1920, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimatedViewPropertyDesiredHeightP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  const int index = AnimatedImageView::Property::DESIRED_HEIGHT;

  view.SetProperty(index, 1080);
  DALI_TEST_EQUALS(view.GetProperty(index).Get<int>(), 1080, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimatedViewPropertyReleasePolicyP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  const int index = AnimatedImageView::Property::RELEASE_POLICY;

  view.SetProperty(index, static_cast<int>(Ui::ReleasePolicy::NEVER));
  DALI_TEST_EQUALS(view.GetProperty(index).Get<int>(),
                   static_cast<int>(Ui::ReleasePolicy::NEVER), TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimatedViewPropertySynchronousLoadingP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  const int index = AnimatedImageView::Property::SYNCHRONOUS_LOADING;

  view.SetProperty(index, true);
  DALI_TEST_CHECK(view.GetProperty(index).Get<bool>());

  view.SetProperty(index, false);
  DALI_TEST_CHECK(!view.GetProperty(index).Get<bool>());
  END_TEST;
}

int UtcDaliAnimatedViewPropertyPreMultipliedAlphaP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  const int index = AnimatedImageView::Property::PRE_MULTIPLIED_ALPHA;

  view.SetProperty(index, true);
  DALI_TEST_CHECK(view.GetProperty(index).Get<bool>());

  view.SetProperty(index, false);
  DALI_TEST_CHECK(!view.GetProperty(index).Get<bool>());
  END_TEST;
}
