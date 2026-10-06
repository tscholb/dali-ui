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

#include <dali-ui-foundation/integration-api/visuals/image-visual-properties-integ.h>
#include <dali-ui-foundation/integration-api/visuals/visual-properties-integ.h>
#include <dali-ui-foundation/integration-api/visuals/visual-transform.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/internal/visuals/visual-base-impl.h>
#include <dali-ui-foundation/public-api/configuration/ui-scale-manager.h>
#include <dali-ui-foundation/public-api/image-loader/image-url.h>
#include <dali-ui-foundation/public-api/views/image/image-view.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali-ui-foundation/public-api/visuals/visual-types.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>
#include <ui-event-thread-callback.h>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include "resources/image-span-resolution-png.h"

using namespace Dali;
using namespace Dali::Ui;

namespace
{
Dali::Ui::Internal::ViewDataImpl& DataOf(ImageView view)
{
  return Dali::Ui::Internal::ViewDataImpl::Get(GetImpl(view));
}

/**
 * @brief Counts how often the layout system asks this view to measure.
 *
 * ViewDataImpl::Measure() returns the cached size without reaching the measure
 * step while the cache is valid, so an unchanged count across a re-measure with
 * identical constraints means the cache survived. The callback must be installed
 * before the first Measure, because installing one invalidates the cache.
 */
struct MeasureCounter
{
  MeasuredSize OnMeasure(Ui::View view, float, float)
  {
    ++count;
    return MeasuredSize(view.GetRequestedWidth(), view.GetRequestedHeight());
  }

  uint32_t count{0u};
};
} // namespace

void utc_dali_image_view_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_image_view_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliImageViewFixedSizeSetUrlSkipsMeasureInvalidation(void)
{
  UiTestApplication application;

  // Installing a measure callback invalidates the cache, so it must be in place
  // before the first Measure() establishes the cached entry under test.
  MeasureCounter counter;

  ImageView view = ImageView::New();
  view.SetRequestedWidth(200.0f);
  view.SetRequestedHeight(100.0f);
  view.SetMeasureCallback(MeasureCallback::New(&counter, &MeasureCounter::OnMeasure));
  view.Measure(500.0f, 500.0f);
  DALI_TEST_EQUALS(counter.count, 1u, TEST_LOCATION);

  auto& viewData = DataOf(view);
  DALI_TEST_CHECK(!viewData.GetVisual(ImageView::Property::IMAGE));

  view.SetResourceUrl("image.png");

  // Both measured axes are independent of the image's natural size. The visual is
  // rebuilt immediately, without retracting the still-correct measure cache: the
  // repeated Measure() with identical constraints is served from the cache and
  // never reaches the measure step again.
  DALI_TEST_CHECK(viewData.GetVisual(ImageView::Property::IMAGE));
  view.Measure(500.0f, 500.0f);
  DALI_TEST_EQUALS(counter.count, 1u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageViewZeroRequestedSizeSetUrlInvalidatesMeasure(void)
{
  UiTestApplication application;

  // Installing a measure callback invalidates the cache, so it must be in place
  // before the first Measure() establishes the cached entry under test.
  MeasureCounter counter;

  ImageView view = ImageView::New();
  view.SetRequestedWidth(0.0f);
  view.SetRequestedHeight(100.0f);
  view.SetMeasureCallback(MeasureCallback::New(&counter, &MeasureCounter::OnMeasure));
  view.Measure(500.0f, 500.0f);
  DALI_TEST_EQUALS(counter.count, 1u, TEST_LOCATION);

  auto& viewData = DataOf(view);
  DALI_TEST_CHECK(!viewData.GetVisual(ImageView::Property::IMAGE));

  view.SetResourceUrl("image.png");

  // Preserve ImageView's existing strictly-positive fixed-size rule: zero does
  // not enter the SetUrl fast path.
  DALI_TEST_CHECK(!viewData.GetVisual(ImageView::Property::IMAGE));

  // A repeated Measure() with identical constraints is served from the cache
  // unless SetResourceUrl() retracted it, so reaching the measure step a second
  // time is proof that the cached measurement was invalidated.
  view.Measure(500.0f, 500.0f);
  DALI_TEST_EQUALS(counter.count, 2u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageViewMatchParentSetUrlInvalidatesMeasure(void)
{
  UiTestApplication application;

  // Installing a measure callback invalidates the cache, so it must be in place
  // before the first Measure() establishes the cached entry under test.
  MeasureCounter counter;

  ImageView view = ImageView::New();
  view.SetRequestedWidth(MATCH_PARENT);
  view.SetRequestedHeight(100.0f);
  view.SetMeasureCallback(MeasureCallback::New(&counter, &MeasureCounter::OnMeasure));
  view.Measure(500.0f, 500.0f);
  DALI_TEST_EQUALS(counter.count, 1u, TEST_LOCATION);

  auto& viewData = DataOf(view);
  DALI_TEST_CHECK(!viewData.GetVisual(ImageView::Property::IMAGE));

  view.SetResourceUrl("image.png");

  // Keep MATCH_PARENT out of the explicit-size optimization. Its final size is
  // supplied by the parent constraint, so preserve the existing deferred path.
  DALI_TEST_CHECK(!viewData.GetVisual(ImageView::Property::IMAGE));

  // A repeated Measure() with identical constraints is served from the cache
  // unless SetResourceUrl() retracted it, so reaching the measure step a second
  // time is proof that the cached measurement was invalidated.
  view.Measure(500.0f, 500.0f);
  DALI_TEST_EQUALS(counter.count, 2u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageViewWrapContentSetUrlInvalidatesMeasure(void)
{
  UiTestApplication application;

  // Installing a measure callback invalidates the cache, so it must be in place
  // before the first Measure() establishes the cached entry under test.
  MeasureCounter counter;

  ImageView view = ImageView::New();
  view.SetRequestedWidth(200.0f);
  view.SetRequestedHeight(WRAP_CONTENT);
  view.SetMeasureCallback(MeasureCallback::New(&counter, &MeasureCounter::OnMeasure));
  view.Measure(500.0f, 500.0f);
  DALI_TEST_EQUALS(counter.count, 1u, TEST_LOCATION);

  auto& viewData = DataOf(view);
  DALI_TEST_CHECK(!viewData.GetVisual(ImageView::Property::IMAGE));

  view.SetResourceUrl("image.png");

  // A WRAP_CONTENT axis can change from the image aspect ratio, so this path keeps
  // the existing deferred visual rebuild and measure invalidation.
  DALI_TEST_CHECK(!viewData.GetVisual(ImageView::Property::IMAGE));

  // A repeated Measure() with identical constraints is served from the cache
  // unless SetResourceUrl() retracted it, so reaching the measure step a second
  // time is proof that the cached measurement was invalidated.
  view.Measure(500.0f, 500.0f);
  DALI_TEST_EQUALS(counter.count, 2u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageViewFixedSizeSetUrlPreservesLayoutFinishedFitting(void)
{
  UiTestApplication application;

  // Installing a measure callback invalidates the cache, so it must be in place
  // before the first Measure() establishes the cached entry under test.
  MeasureCounter counter;

  ImageView view = ImageView::New();
  view.SetRequestedWidth(200.0f);
  view.SetRequestedHeight(200.0f);
  view.SetDesiredWidth(100);
  view.SetDesiredHeight(50);
  view.SetFittingMode(Image::FittingMode::FIT_KEEP_ASPECT_RATIO);
  view.SetMeasureCallback(MeasureCallback::New(&counter, &MeasureCounter::OnMeasure));
  view.Measure(500.0f, 500.0f);

  auto& viewData = DataOf(view);
  DALI_TEST_EQUALS(counter.count, 1u, TEST_LOCATION);

  view.SetResourceUrl("image.png");

  auto visual = viewData.GetVisual(ImageView::Property::IMAGE);
  DALI_TEST_CHECK(visual);
  view.Measure(500.0f, 500.0f);
  DALI_TEST_EQUALS(counter.count, 1u, TEST_LOCATION);

  // The fast path does not request a layout itself. Fitting-required visuals must
  // still be connected to the View's LayoutFinished path at registration time, so
  // the final bounds are applied when the layout pass completes.
  viewData.EmitLayoutFinishedSignal(LayoutRect(0.0f, 0.0f, 200.0f, 200.0f));

  Property::Map visualMap;
  visual.CreatePropertyMap(visualMap);

  Property::Map transform;
  DALI_TEST_CHECK(visualMap.Find(Ui::Integration::Visual::Property::TRANSFORM)->Get(transform));

  Vector2 fittedSize;
  Vector2 fittedOffset;
  DALI_TEST_CHECK(transform.Find(Ui::Integration::Visual::Transform::Property::SIZE)->Get(fittedSize));
  DALI_TEST_CHECK(transform.Find(Ui::Integration::Visual::Transform::Property::OFFSET)->Get(fittedOffset));
  DALI_TEST_EQUALS(fittedSize, Vector2(200.0f, 100.0f), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(fittedOffset, Vector2(0.0f, 50.0f), 0.01f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageViewAutoNPatchBorderOnlyReachesVisual(void)
{
  UiTestApplication application;

  ImageView view = ImageView::New();
  view.SetRequestedWidth(200.0f);
  view.SetRequestedHeight(100.0f);
  view.SetNPatchBorderOnly(true);
  view.SetResourceUrl("image.9.png");

  auto visual = DataOf(view).GetVisual(ImageView::Property::IMAGE);
  DALI_TEST_CHECK(visual);

  Property::Map visualMap;
  visual.CreatePropertyMap(visualMap);

  bool borderOnly = false;
  auto borderOnlyValue = visualMap.Find(Ui::Integration::ImageVisual::Property::BORDER_ONLY);
  DALI_TEST_CHECK(borderOnlyValue);
  DALI_TEST_CHECK(borderOnlyValue->Get(borderOnly));
  DALI_TEST_EQUALS(borderOnly, true, TEST_LOCATION);

  END_TEST;
}

namespace
{
struct ArrangeSvgPanel : ConnectionTracker
{
  ImageView    arrow;
  bool         delaySettlement{true};
  int          measures{0};
  int          finishes{0};
  MeasuredSize Measure(View view, float, float)
  {
    ++measures;
    if(!arrow)
    {
      arrow = ImageView::New(DALI_UI_FOUNDATION_INTERNAL_TEST_RESOURCE_DIR "/initial-layout.svg");
      arrow.SetRequestedWidth(40.0f);
      arrow.SetRequestedHeight(40.0f);
      arrow.SetBackgroundColor(UiColor(0x303030));
      view.Add(arrow);
      arrow.LayoutFinishedSignal().Connect(this, &ArrangeSvgPanel::Finished);
    }
    arrow.Measure(40.0f, 40.0f);
    // Bound the unsettled interval without changing any image property.
    if(delaySettlement && measures < 12)
    {
      Dali::Ui::Internal::ViewDataImpl::Get(GetImpl(view)).InvalidateMeasure();
    }
    return MeasuredSize(100.0f, 100.0f);
  }
  LayoutRect Arrange(View, const LayoutRect& bounds)
  {
    arrow.Arrange(LayoutRect(4.0f, 4.0f, 40.0f, 40.0f));
    return bounds;
  }
  void Finished(View, LayoutRect)
  {
    ++finishes;
  }
};
} // namespace

// Drive event processing explicitly: this tests SVG size delivery while layout
// remains pending, independently of whether a follow-up idle wake is requested.
int UtcDaliImageViewSvgReadyBeforeLayoutSettles(void)
{
  UiTestApplication application;
  ArrangeSvgPanel   panel;
  View              root = View::New();
  root.SetRequestedWidth(100.0f);
  root.SetRequestedHeight(100.0f);
  root.SetMeasureCallback(MeasureCallback::New(&panel, &ArrangeSvgPanel::Measure));
  root.SetArrangeCallback(ArrangeCallback::New(&panel, &ArrangeSvgPanel::Arrange));
  application.GetWindow().Add(root);

  for(int i = 0; i < 8 && (!panel.arrow || !panel.arrow.IsResourceReady()); ++i)
  {
    application.SendNotification();
    application.Render();
    if(!panel.arrow.IsResourceReady())
    {
      Test::WaitForEventThreadTrigger(1, 1);
    }
  }

  DALI_TEST_CHECK(panel.arrow.IsResourceReady());
  DALI_TEST_EQUALS(panel.finishes, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(panel.arrow.GetProperty<float>(Actor::Property::SIZE_WIDTH), 40.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(panel.arrow.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 40.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(panel.arrow.GetRendererCount(), 2u, TEST_LOCATION);
  Texture texture = panel.arrow.GetRendererAt(1u).GetTextures().GetTexture(0u);

  panel.delaySettlement = false;
  Dali::Ui::Internal::ViewDataImpl::Get(GetImpl(root)).InvalidateMeasure();
  for(int i = 0; i < 5 && panel.finishes == 0; ++i)
  {
    application.SendNotification();
    application.Render();
  }

  DALI_TEST_CHECK(panel.finishes > 0);
  DALI_TEST_CHECK(panel.arrow.IsResourceReady());
  DALI_TEST_EQUALS(panel.arrow.GetRendererCount(), 2u, TEST_LOCATION);
  DALI_TEST_CHECK(panel.arrow.GetRendererAt(1u).GetTextures().GetTexture(0u) == texture);
  END_TEST;
}

namespace
{
struct SizingPngFixture
{
  SizingPngFixture()
  : path(std::filesystem::temp_directory_path() /
         ("dali-ui-sizing-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()) + ".png"))
  {
    std::ofstream stream(path, std::ios::binary);
    stream.write(reinterpret_cast<const char*>(IMAGE_SPAN_RESOLUTION_PNG), sizeof(IMAGE_SPAN_RESOLUTION_PNG));
  }
  ~SizingPngFixture()
  {
    std::error_code error;
    std::filesystem::remove(path, error);
  }
  std::filesystem::path path;
};

Texture ImageTexture(ImageView view)
{
  auto visual = DataOf(view).GetVisual(ImageView::Property::IMAGE);
  return Ui::GetImplementation(visual).GetRenderer().GetTextures().GetTexture(0);
}
} // namespace

int UtcDaliImageViewExplicitSizeFirstUpload(void)
{
  UiTestApplication        application;
  SizingPngFixture         fixture;
  std::vector<std::string> urls{fixture.path.string()};
  if(const char* remote = std::getenv("DALI_TEST_SIZING_REMOTE_URL"))
  {
    urls.emplace_back(remote); // Optional loopback HTTP fixture for integration runs.
  }
  struct ScaleRestorer
  {
    float original = UiScaleManager::Get().GetScale();
    ~ScaleRestorer()
    {
      UiScaleManager::Get().SetScale(original);
    }
  } restoreScale;
  for(float scale : {1.0f, 1.5f})
  {
    UiScaleManager::Get().SetScale(scale);
    for(const auto& url : urls)
    {
      for(bool synchronous : {false, true})
      {
        auto view = ImageView::New();
        view.SetImageLoadWithViewSizeEnabled(true);
        view.SetSynchronousLoading(synchronous);
        view.SetDesiredWidth(16);
        view.SetDesiredHeight(16);
        view.SetResourceUrl(url.c_str());
        // Read the current explicit size at scene attachment, including constraints.
        view.SetRequestedWidth(synchronous ? 100 : 40);
        view.SetRequestedHeight(synchronous ? 100 : 40);
        view.SetMinimumWidth(80);
        view.SetMinimumHeight(80);
        view.SetMaximumWidth(80);
        view.SetMaximumHeight(80);
        view.SetPadding(8, 8, 8, 8);
        // With URL assigned first, ImageView creates its deferred visual during measure.
        view.Measure(80 * scale, 80 * scale);
        DALI_TEST_CHECK(!DataOf(view).HasArrangeResult());
        DALI_TEST_EQUALS(view.GetLoadingStatus(), Ui::Visual::ResourceStatus::PREPARING, TEST_LOCATION);
        application.GetWindow().Add(view);
        if(!synchronous || url.find("http") == 0)
        {
          DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1));
        }
        // No Arrange/Render has run: the first texture must already be reduced.
        DALI_TEST_CHECK(!DataOf(view).HasArrangeResult());
        auto texture = ImageTexture(view);
        DALI_TEST_EQUALS(texture.GetWidth(), static_cast<uint32_t>(64 * scale), TEST_LOCATION);
        DALI_TEST_EQUALS(texture.GetHeight(), static_cast<uint32_t>(64 * scale), TEST_LOCATION);
        DALI_TEST_EQUALS(view.GetDesiredWidth(), 16, TEST_LOCATION);
        DALI_TEST_EQUALS(view.GetDesiredHeight(), 16, TEST_LOCATION);
        view.Arrange(LayoutRect(0, 0, 80 * scale, 80 * scale));
        DataOf(view).EmitLayoutFinishedSignal(LayoutRect(0, 0, 80 * scale, 80 * scale));
        application.SendNotification();
        application.Render();
        DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, 1), false, TEST_LOCATION);
        DALI_TEST_CHECK(ImageTexture(view) == texture);
        // Both loading modes must apply later size changes.
        view.SetMinimumWidth(0);
        view.SetMinimumHeight(0);
        view.SetRequestedWidth(64);
        view.SetRequestedHeight(64);
        view.Arrange(LayoutRect(0, 0, 64 * scale, 64 * scale));
        DataOf(view).EmitLayoutFinishedSignal(LayoutRect(0, 0, 64 * scale, 64 * scale));
        if(!synchronous || url.find("http") == 0)
        {
          DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1));
        }
        application.SendNotification();
        application.Render();
        texture = ImageTexture(view);
        DALI_TEST_EQUALS(texture.GetWidth(), static_cast<uint32_t>(48 * scale), TEST_LOCATION);
        DALI_TEST_EQUALS(texture.GetHeight(), static_cast<uint32_t>(48 * scale), TEST_LOCATION);
        application.GetWindow().Remove(view);
        application.SendNotification();
        application.Render();
      }
    }
  }
  END_TEST;
}

int UtcDaliImageViewExplicitSizeFallbacks(void)
{
  UiTestApplication application;
  SizingPngFixture  fixture;
  for(int scenario = 0; scenario < 6; ++scenario)
  {
    auto view = ImageView::New();
    view.SetRequestedWidth(scenario == 3 ? MATCH_PARENT : scenario == 4 ? 0
                                                                        : 64);
    view.SetRequestedHeight(64);
    view.SetSynchronousLoading(true);
    view.SetImageLoadWithViewSizeEnabled(scenario != 0);
    if(scenario == 0)
    {
      view.SetDesiredWidth(32);
      view.SetDesiredHeight(32);
    }
    if(scenario == 1)
    {
      view.SetLoadPolicy(Image::LoadPolicy::IMMEDIATE);
    }
    if(scenario == 2)
    {
      view.SetFittingMode(Image::FittingMode::CENTER);
    }
    view.SetResourceUrl(fixture.path.string().c_str());
    view.Measure(64, 64);
    if(scenario == 5)
    {
      Property::Map transform;
      transform.Add(Ui::Integration::Visual::Transform::Property::SIZE, Vector2(0.5f, 0.5f));
      Property::Map properties;
      properties.Add(Ui::Integration::Visual::Property::TRANSFORM, transform);
      auto visual = DataOf(view).GetVisual(ImageView::Property::IMAGE);
      Ui::GetImplementation(visual).SetProperties(properties);
    }
    application.GetWindow().Add(view);
    auto texture = ImageTexture(view);
    DALI_TEST_EQUALS(texture.GetWidth(), scenario == 0 ? 32u : 192u, TEST_LOCATION);
    DALI_TEST_EQUALS(texture.GetHeight(), scenario == 0 ? 32u : 192u, TEST_LOCATION);
    application.GetWindow().Remove(view);
  }
  END_TEST;
}

int UtcDaliImageViewExplicitSizeShaderAndMask(void)
{
  UiTestApplication application;
  SizingPngFixture  fixture;
  for(int scenario = 0; scenario < 3; ++scenario)
  {
    for(bool synchronous : {false, true})
    {
      auto view = ImageView::New();
      view.SetRequestedWidth(64);
      view.SetRequestedHeight(64);
      view.SetImageLoadWithViewSizeEnabled(true);
      view.SetSynchronousLoading(synchronous);
      if(scenario != 0)
      {
        view.SetAlphaMaskUrl(fixture.path.string().c_str());
        view.SetCropToMask(true);
        view.SetMaskingPolicy(scenario == 1 ? Image::MaskingPolicy::ON_LOADING : Image::MaskingPolicy::ON_RENDERING);
      }
      view.SetResourceUrl(fixture.path.string().c_str());
      if(scenario == 0)
      {
        Property::Map shader;
        shader.Add(Ui::Integration::Visual::Shader::Property::FRAGMENT_SHADER, "Foobar");
        Property::Map properties;
        properties.Add(Ui::Integration::Visual::Property::SHADER, shader);
        auto visual = DataOf(view).GetVisual(ImageView::Property::IMAGE);
        Ui::GetImplementation(visual).SetProperties(properties);
      }
      application.GetWindow().Add(view);
      if(!synchronous)
      {
        DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(scenario == 0 ? 1 : scenario == 1 ? 3
                                                                                          : 2));
      }
      auto texture = ImageTexture(view);
      DALI_TEST_EQUALS(texture.GetWidth(), 64u, TEST_LOCATION);
      DALI_TEST_EQUALS(texture.GetHeight(), 64u, TEST_LOCATION);
      auto textures = Ui::GetImplementation(DataOf(view).GetVisual(ImageView::Property::IMAGE)).GetRenderer().GetTextures();
      DALI_TEST_EQUALS(textures.GetTextureCount(), scenario == 2 ? 2u : 1u, TEST_LOCATION);
      view.Arrange(LayoutRect(0, 0, 64, 64));
      DataOf(view).EmitLayoutFinishedSignal(LayoutRect(0, 0, 64, 64));
      application.SendNotification();
      application.Render();
      DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, 1), false, TEST_LOCATION);
      DALI_TEST_CHECK(ImageTexture(view) == texture);
      if(synchronous)
      {
        for(uint32_t size : {32u, 96u, 48u})
        {
          view.SetRequestedWidth(size);
          view.SetRequestedHeight(size);
          view.Arrange(LayoutRect(0, 0, size, size));
          DataOf(view).EmitLayoutFinishedSignal(LayoutRect(0, 0, size, size));
          texture = ImageTexture(view);
          DALI_TEST_EQUALS(texture.GetWidth(), size, TEST_LOCATION);
          DALI_TEST_EQUALS(texture.GetHeight(), size, TEST_LOCATION);
          textures = Ui::GetImplementation(DataOf(view).GetVisual(ImageView::Property::IMAGE)).GetRenderer().GetTextures();
          DALI_TEST_EQUALS(textures.GetTextureCount(), scenario == 2 ? 2u : 1u, TEST_LOCATION);
          application.SendNotification();
          application.Render();
        }
      }
      application.GetWindow().Remove(view);
      application.SendNotification();
      application.Render();
    }
  }
  END_TEST;
}

int UtcDaliImageViewSynchronousSizingResizeReleasesTextures(void)
{
  UiTestApplication application;
  SizingPngFixture  fixture;
  auto&             gl   = application.GetGlAbstraction();
  auto              view = ImageView::New();
  view.SetRequestedWidth(64);
  view.SetRequestedHeight(64);
  view.SetImageLoadWithViewSizeEnabled(true);
  view.SetSynchronousLoading(true);
  view.SetResourceUrl(fixture.path.string().c_str());
  application.GetWindow().Add(view);
  view.Arrange(LayoutRect(0, 0, 64, 64));
  DataOf(view).EmitLayoutFinishedSignal(LayoutRect(0, 0, 64, 64));
  application.SendNotification();
  application.Render();
  const auto liveTextures = gl.GetNumGeneratedTextures();

  for(uint32_t size : {48u, 32u, 80u, 48u})
  {
    auto           texture = ImageTexture(view);
    WeakHandleBase previous(texture);
    texture.Reset(); // The test must not keep the old texture alive.
    const auto previousId = gl.GetLastGenTextureId();
    gl.ClearDeletedTextures();

    view.SetRequestedWidth(size);
    view.SetRequestedHeight(size);
    view.Arrange(LayoutRect(0, 0, size, size));
    DataOf(view).EmitLayoutFinishedSignal(LayoutRect(0, 0, size, size));
    texture = ImageTexture(view);
    DALI_TEST_EQUALS(texture.GetWidth(), size, TEST_LOCATION);
    DALI_TEST_EQUALS(texture.GetHeight(), size, TEST_LOCATION);
    DALI_TEST_CHECK(texture != previous.GetBaseHandle());
    texture.Reset();

    // Flush queued cache removal and render-side resource destruction.
    for(int frame = 0; frame < 3; ++frame)
    {
      application.SendNotification();
      application.Render();
    }
    DALI_TEST_CHECK(!previous.GetBaseHandle());
    DALI_TEST_CHECK(gl.CheckTextureDeleted(previousId));
    DALI_TEST_EQUALS(gl.GetNumGeneratedTextures(), liveTextures, TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliImageViewExplicitSizeFittingAndBuffer(void)
{
  UiTestApplication                       application;
  SizingPngFixture                        fixture;
  Dali::EncodedImageBuffer::RawBufferType bytes;
  bytes.Resize(sizeof(IMAGE_SPAN_RESOLUTION_PNG));
  std::copy(std::begin(IMAGE_SPAN_RESOLUTION_PNG), std::end(IMAGE_SPAN_RESOLUTION_PNG), bytes.Begin());
  auto              bufferUrl = Dali::Ui::ImageUrl::New(Dali::EncodedImageBuffer::New(std::move(bytes)));
  const std::string localUrl  = fixture.path.string();
  for(const auto& url : {localUrl, std::string(bufferUrl.GetUrl().CStr())})
  {
    for(auto mode : {Image::FittingMode::FILL, Image::FittingMode::FIT_KEEP_ASPECT_RATIO, Image::FittingMode::OVER_FIT_KEEP_ASPECT_RATIO})
    {
      for(bool synchronous : {false, true})
      {
        auto view = ImageView::New();
        view.SetRequestedWidth(80);
        view.SetRequestedHeight(40);
        view.SetSynchronousLoading(synchronous);
        view.SetImageLoadWithViewSizeEnabled(true);
        view.SetFittingMode(mode);
        view.SetResourceUrl(url.c_str());
        application.GetWindow().Add(view);
        if(!synchronous)
        {
          DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1));
        }
        // The square source covers the rectangular request without losing its aspect ratio.
        auto texture = ImageTexture(view);
        DALI_TEST_EQUALS(texture.GetWidth(), 80u, TEST_LOCATION);
        DALI_TEST_EQUALS(texture.GetHeight(), 80u, TEST_LOCATION);
        view.Arrange(LayoutRect(0, 0, 80, 40));
        DataOf(view).EmitLayoutFinishedSignal(LayoutRect(0, 0, 80, 40));
        if(!synchronous && mode == Image::FittingMode::FIT_KEEP_ASPECT_RATIO)
        {
          DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1));
        }
        application.SendNotification();
        application.Render();
        texture                 = ImageTexture(view);
        const uint32_t expected = mode == Image::FittingMode::FIT_KEEP_ASPECT_RATIO ? 40u : 80u;
        DALI_TEST_EQUALS(texture.GetWidth(), expected, TEST_LOCATION);
        DALI_TEST_EQUALS(texture.GetHeight(), expected, TEST_LOCATION);
        application.GetWindow().Remove(view);
        application.SendNotification();
        application.Render();
      }
    }
  }
  END_TEST;
}

int UtcDaliImageViewInitialSizeOnEnableAndReplacement(void)
{
  UiTestApplication application;
  SizingPngFixture  fixture;
  for(bool synchronous : {false, true})
  {
    // The first scene attachment is delayed until after the size changes.
    auto view = ImageView::New();
    view.SetRequestedWidth(64);
    view.SetRequestedHeight(64);
    view.SetImageLoadWithViewSizeEnabled(true);
    view.SetSynchronousLoading(synchronous);
    view.SetResourceUrl(fixture.path.string().c_str());
    DataOf(view).EnableVisual(ImageView::Property::IMAGE, false);
    application.GetWindow().Add(view);
    view.SetRequestedWidth(40);
    view.SetRequestedHeight(40);
    DataOf(view).EnableVisual(ImageView::Property::IMAGE, true);
    if(!synchronous)
    {
      DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1));
    }
    auto texture = ImageTexture(view);
    DALI_TEST_EQUALS(texture.GetWidth(), 40u, TEST_LOCATION);
    DALI_TEST_EQUALS(texture.GetHeight(), 40u, TEST_LOCATION);

    // Replacing a visual on scene must also use the latest size before relayout.
    view.SetRequestedWidth(80);
    view.SetRequestedHeight(80);
    view.SetDesiredWidth(16);
    view.SetDesiredHeight(16);
    view.Measure(80, 80);
    if(!synchronous)
    {
      DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1));
    }
    texture = ImageTexture(view);
    DALI_TEST_EQUALS(texture.GetWidth(), 80u, TEST_LOCATION);
    DALI_TEST_EQUALS(texture.GetHeight(), 80u, TEST_LOCATION);
    application.GetWindow().Remove(view);
    application.SendNotification();
    application.Render();
  }
  END_TEST;
}
