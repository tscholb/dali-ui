/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
 */

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/image-view/image-view.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout-params.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>
#include <dali/integration-api/debug.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
const char* const IMAGES[] = {
  RESOURCES_DIR "gallery-large-3.jpg",    // landscape
  RESOURCES_DIR "people-small-10.jpg",    // portrait
  RESOURCES_DIR "gallery-medium-3.jpg",   // square-ish
  RESOURCES_DIR "gallery-medium-49.jpg",  // landscape
};
constexpr int IMAGE_COUNT = 4;

// Color palette for mode labels (dark backgrounds)
constexpr uint32_t COLOR_FIT_KEEP   = 0x1565C0; // blue   - FIT_KEEP_ASPECT_RATIO
constexpr uint32_t COLOR_FILL       = 0xAD1457; // pink   - FILL
constexpr uint32_t COLOR_OVER_FIT   = 0x00695C; // teal   - OVER_FIT_KEEP_ASPECT_RATIO
constexpr uint32_t COLOR_CENTER     = 0x6A1B9A; // purple - CENTER
constexpr uint32_t COLOR_IMAGE_BG   = 0x263238; // dark slate - shows empty ImageView area
constexpr uint32_t COLOR_DIVIDER    = 0x0D0D0D; // near-black divider between cells
} // namespace

/**
 * @brief ImageView FittingMode Comparison sample
 *
 * Layout: 2×2 grid with all four FittingModes shown simultaneously.
 * Each cell has a color-coded label (mode name) on top and an ImageView
 * below so you can immediately compare how the same image is placed.
 *
 * Background color of the ImageView area (dark slate) makes it easy to see
 * exactly which pixels are covered by the image for each mode:
 *
 *   FIT_KEEP_ASPECT_RATIO  |  FILL
 *   -----------------------+------------------
 *   OVER_FIT_KEEP_ASPECT   |  CENTER
 *
 * [SWAP IMAGE] button cycles through test images (landscape / portrait / etc.)
 *
 * Press Escape or Back to quit.
 */
class FittingModeComparisonController : public ConnectionTracker
{
  static constexpr int MODE_COUNT = 4;

  struct ModeInfo
  {
    Ui::FittingMode::Type mode;
    const char*           label;
    uint32_t              color;
  };

  static constexpr ModeInfo MODES[MODE_COUNT] = {
    {Ui::FittingMode::FIT_KEEP_ASPECT_RATIO,      "FIT KEEP\nASPECT RATIO",  COLOR_FIT_KEEP},
    {Ui::FittingMode::FILL,                        "FILL",                    COLOR_FILL},
    {Ui::FittingMode::OVER_FIT_KEEP_ASPECT_RATIO,  "OVER FIT\nKEEP ASPECT",  COLOR_OVER_FIT},
    {Ui::FittingMode::CENTER,                      "CENTER\n(original size)", COLOR_CENTER},
  };

public:
  explicit FittingModeComparisonController(Application& application)
  : mApplication(application),
    mImageIndex(0)
  {
    mApplication.InitSignal().Connect(this, &FittingModeComparisonController::OnInit);
  }

private:
  // -------------------------------------------------------------------------
  // Initialisation
  // -------------------------------------------------------------------------
  void OnInit(Application& application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(COLOR_DIVIDER));

    window.Add(
      StackLayout::New(StackOrientation::VERTICAL)
        .SetRequestedWidth(MATCH_PARENT)
        .SetRequestedHeight(MATCH_PARENT)
        .Children({
          CreateTitleBar(),
          CreateGrid(),
          CreateBottomBar(),
        }));

    window.KeyEventSignal().Connect(this, &FittingModeComparisonController::OnKeyEvent);
  }

  // -------------------------------------------------------------------------
  // Title bar
  // -------------------------------------------------------------------------
  View CreateTitleBar()
  {
    return StackLayout::New(StackOrientation::VERTICAL)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(50.0f)
      .SetBackgroundColor(UiColor(0x37474F))
      .Children({
        Label::New("FittingMode Comparison")
          .SetRequestedWidth(MATCH_PARENT)
          .SetRequestedHeight(MATCH_PARENT)
          .SetFontSize(18.0f)
          .SetTextColor(UiColor(0xFFFFFF))
          .SetHorizontalTextAlignment(Text::Alignment::CENTER)
          .SetVerticalTextAlignment(Text::Alignment::CENTER),
      });
  }

  // -------------------------------------------------------------------------
  // 2×2 grid (occupies all remaining space via weight)
  // -------------------------------------------------------------------------
  View CreateGrid()
  {
    return StackLayout::New(StackOrientation::VERTICAL)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .Spacing(2.0f)
      .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
      .Children({
        CreateRow(0, 1), // row 0: FIT_KEEP_ASPECT_RATIO | FILL
        CreateRow(2, 3), // row 1: OVER_FIT_KEEP_ASPECT  | CENTER
      });
  }

  // One horizontal row containing two mode cells
  View CreateRow(int leftIndex, int rightIndex)
  {
    return StackLayout::New(StackOrientation::HORIZONTAL)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .Spacing(2.0f)
      .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
      .Children({
        CreateCell(leftIndex),
        CreateCell(rightIndex),
      });
  }

  // One cell: colored label on top + ImageView filling remaining space
  View CreateCell(int index)
  {
    StackLayout cell = StackLayout::New(StackOrientation::VERTICAL)
                         .SetRequestedWidth(WRAP_CONTENT)
                         .SetRequestedHeight(MATCH_PARENT)
                         .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
                         .Children({
                           CreateModeLabel(index),
                           CreateModeImage(index),
                         });
    return cell;
  }

  View CreateModeLabel(int index)
  {
    return StackLayout::New(StackOrientation::VERTICAL)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(52.0f)
      .SetBackgroundColor(UiColor(MODES[index].color))
      .Children({
        Label::New(MODES[index].label)
          .SetRequestedWidth(MATCH_PARENT)
          .SetRequestedHeight(MATCH_PARENT)
          .SetFontSize(13.0f)
          .SetMultiLine(true)
          .SetTextColor(UiColor(0xFFFFFF))
          .SetHorizontalTextAlignment(Text::Alignment::CENTER)
          .SetVerticalTextAlignment(Text::Alignment::CENTER),
      });
  }

  View CreateModeImage(int index)
  {
    return ImageView::New(IMAGES[mImageIndex])
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetBackgroundColor(UiColor(COLOR_IMAGE_BG))
      .SetFittingMode(MODES[index].mode)
      .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
      .As(mImages[index]);
  }

  // -------------------------------------------------------------------------
  // Bottom bar
  // -------------------------------------------------------------------------
  View CreateBottomBar()
  {
    StackLayout swapButton =
      StackLayout::New(StackOrientation::VERTICAL)
        .SetRequestedWidth(WRAP_CONTENT)
        .SetRequestedHeight(MATCH_PARENT)
        .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
        .SetBackgroundColor(UiColor(0x2E7D32))
        .Children({
          Label::New("SWAP\nIMAGE")
            .SetRequestedWidth(MATCH_PARENT)
            .SetRequestedHeight(MATCH_PARENT)
            .SetFontSize(13.0f)
            .SetMultiLine(true)
            .SetTextColor(UiColor(0xFFFFFF))
            .SetHorizontalTextAlignment(Text::Alignment::CENTER)
            .SetVerticalTextAlignment(Text::Alignment::CENTER),
        });
    swapButton.EnsureInteractiveTrait().ClickedSignal().Connect(
      this, &FittingModeComparisonController::OnSwapClicked);

    return StackLayout::New(StackOrientation::HORIZONTAL)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(60.0f)
      .SetViewPadding(Extents(4, 4, 4, 4))
      .Spacing(4.0f)
      .SetBackgroundColor(UiColor(0x212121))
      .Children({swapButton});
  }

  // -------------------------------------------------------------------------
  // Event handlers
  // -------------------------------------------------------------------------
  void OnSwapClicked(View /*view*/, const InputEvent& /*event*/)
  {
    mImageIndex = (mImageIndex + 1) % IMAGE_COUNT;
    const char* url = IMAGES[mImageIndex];
    for(int i = 0; i < MODE_COUNT; ++i)
    {
      mImages[i].SetResourceUrl(url);
    }
    DALI_LOG_RELEASE_INFO("[FittingModeComparison] Image swapped to: %s\n", url);
  }

  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.GetState() == KeyEvent::DOWN)
    {
      if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
      {
        mApplication.Quit();
      }
    }
  }

  // -------------------------------------------------------------------------
  // Members
  // -------------------------------------------------------------------------
  Application&  mApplication;
  Ui::ImageView mImages[MODE_COUNT];
  int           mImageIndex;
};

constexpr FittingModeComparisonController::ModeInfo FittingModeComparisonController::MODES[FittingModeComparisonController::MODE_COUNT];

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  FittingModeComparisonController controller(application);
  application.MainLoop();
  return 0;
}
