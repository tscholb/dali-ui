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
#include <dali-ui-foundation/public-api/layouts/stack-layout-params.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>
#include <dali-ui-foundation/public-api/views/image/image-view.h>
#include <dali/devel-api/adaptor-framework/application.h>
#include <dali/public-api/adaptor-framework/timer.h>

#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>

using namespace Dali;
using namespace Dali::Ui;

/**
 * SVG reload sample:
 * - Writes an SVG file to a temporary path and shows it in two ImageViews
 * - "Rewrite file" overwrites the same path with a document of another size and color
 * - "Reload" calls ImageView::Reload() so both views read the file again
 * - The left view wraps its content, so its size follows the SVG's own width and height
 * - The right view has a fixed size, so only its content changes
 * - Keys: W rewrites the file, R reloads, Escape or Back quits
 * - "--auto" runs rewrite and reload once, prints the results and quits
 */
class SvgReloadController : public ConnectionTracker
{
  struct SvgVariant
  {
    const char* name;
    uint32_t    width;
    uint32_t    height;
    const char* color;
  };

  static constexpr int    VARIANT_COUNT = 3;
  static const SvgVariant VARIANTS[VARIANT_COUNT];

public:
  SvgReloadController(Application& application, bool autoRun)
  : mApplication(application),
    mPath(std::filesystem::temp_directory_path() / "dali-ui-svg-reload-sample.svg"),
    mAutoRun(autoRun)
  {
    mApplication.InitSignal().Connect(this, &SvgReloadController::OnInit);
  }

  ~SvgReloadController()
  {
    std::error_code error;
    std::filesystem::remove(mPath, error);
  }

private:
  void OnInit(Application application)
  {
    WriteVariant(0);

    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0x20242A));
    window.Add(CreateContents());
    window.KeyEventSignal().Connect(this, &SvgReloadController::OnKeyEvent);

    if(mAutoRun)
    {
      mAutoTimer = Timer::New(700u);
      mAutoTimer.TickSignal().Connect(this, &SvgReloadController::OnAutoTick);
      mAutoTimer.Start();
    }
  }

  View CreateContents()
  {
    StackLayout contents = StackLayout::New(StackOrientation::VERTICAL);
    contents.SetRequestedWidth(MATCH_PARENT);
    contents.SetRequestedHeight(MATCH_PARENT);
    contents.SetPadding(Insets(12.0f, 12.0f, 12.0f, 12.0f));
    contents.SetSpacing(8.0f);

    mInfoLabel = CreateLabel(16.0f);
    mInfoLabel.SetRequestedHeight(96.0f);
    mInfoLabel.SetMultiLine(true);
    contents.Add(mInfoLabel);

    StackLayout images = StackLayout::New(StackOrientation::HORIZONTAL);
    images.SetRequestedWidth(MATCH_PARENT);
    images.SetRequestedHeight(WRAP_CONTENT);
    images.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    images.SetSpacing(12.0f);

    mWrapView = CreateImageView();
    mWrapView.SetRequestedWidth(WRAP_CONTENT);
    mWrapView.SetRequestedHeight(WRAP_CONTENT);
    images.Add(mWrapView);

    mFixedView = CreateImageView();
    mFixedView.SetRequestedWidth(160.0f);
    mFixedView.SetRequestedHeight(160.0f);
    mFixedView.SetFittingMode(Ui::Image::FittingMode::FIT_KEEP_ASPECT_RATIO);
    images.Add(mFixedView);
    contents.Add(images);

    StackLayout buttons = StackLayout::New(StackOrientation::HORIZONTAL);
    buttons.SetRequestedWidth(MATCH_PARENT);
    buttons.SetRequestedHeight(64.0f);
    buttons.SetSpacing(8.0f);
    View rewrite = CreateButton("Rewrite file (W)");
    rewrite.AsInteractive().ClickedSignal().Connect(this, &SvgReloadController::OnRewriteClicked);
    buttons.Add(rewrite);
    View reload = CreateButton("Reload (R)");
    reload.AsInteractive().ClickedSignal().Connect(this, &SvgReloadController::OnReloadClicked);
    buttons.Add(reload);
    contents.Add(buttons);

    UpdateInfo();
    return contents;
  }

  ImageView CreateImageView()
  {
    ImageView view = ImageView::New(mPath.string().c_str());
    view.SetImageLoadWithViewSizeEnabled(false);
    view.SetBackgroundColor(UiColor(0x30343B));
    view.ResourceReadySignal().Connect(this, &SvgReloadController::OnResourceReady);
    return view;
  }

  Label CreateLabel(float fontSize)
  {
    Label label = Label::New("");
    label.SetRequestedWidth(MATCH_PARENT);
    label.SetFontSize(fontSize);
    label.SetTextColor(UiColor(0xDDDDDD));
    label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    label.SetVerticalTextAlignment(Text::Alignment::CENTER);
    return label;
  }

  View CreateButton(const char* text)
  {
    StackLayout button = StackLayout::New(StackOrientation::VERTICAL);
    button.SetRequestedWidth(WRAP_CONTENT);
    button.SetRequestedHeight(MATCH_PARENT);
    button.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    button.SetBackgroundColor(UiColor(0x1565C0));

    Label label = CreateLabel(14.0f);
    label.SetText(text);
    label.SetRequestedHeight(MATCH_PARENT);
    label.SetTextColor(UiColor(0xFFFFFF));
    button.Add(label);
    return button;
  }

  void WriteVariant(int index)
  {
    const SvgVariant& variant = VARIANTS[index];
    std::ofstream     stream(mPath, std::ios::binary | std::ios::trunc);
    stream << "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"" << variant.width << "\" height=\"" << variant.height << "\">"
           << "<rect width=\"" << variant.width << "\" height=\"" << variant.height << "\" rx=\"12\" fill=\"" << variant.color << "\"/>"
           << "<circle cx=\"" << variant.width / 2u << "\" cy=\"" << variant.height / 2u << "\" r=\"" << std::min(variant.width, variant.height) / 4u
           << "\" fill=\"#FFFFFF\"/></svg>";
    mFileVariant = index;
    Log("file rewritten");
  }

  void Rewrite()
  {
    WriteVariant((mFileVariant + 1) % VARIANT_COUNT);
    UpdateInfo();
  }

  void Reload()
  {
    mReadyCount = 0;
    mWrapView.Reload();
    mFixedView.Reload();
    Log("reload requested");
    UpdateInfo();
  }

  void OnRewriteClicked(View, InputEvent)
  {
    Rewrite();
  }

  void OnReloadClicked(View, InputEvent)
  {
    Reload();
  }

  void OnResourceReady(View)
  {
    ++mReadyCount;
    UpdateInfo();
  }

  // Auto mode: show the first file, rewrite it, reload, then report and quit.
  bool OnAutoTick()
  {
    switch(mAutoStep++)
    {
      case 0:
        Log("initial");
        Rewrite();
        return true;
      case 1:
        Log("after rewrite, before reload");
        Reload();
        return true;
      case 2:
        Log("after reload");
        mApplication.Quit();
        return false;
    }
    return false;
  }

  void Log(const char* step) const
  {
    if(!mWrapView)
    {
      std::printf("[SvgReload] %s: file=%s\n", step, VARIANTS[mFileVariant].name);
      std::fflush(stdout);
      return;
    }
    const Vector3 natural = mWrapView.GetNaturalSize();
    std::printf("[SvgReload] %s: file=%s natural=%.0fx%.0f status=%d/%d readySignals=%d\n",
                step,
                VARIANTS[mFileVariant].name,
                natural.width,
                natural.height,
                static_cast<int>(mWrapView.GetLoadingStatus()),
                static_cast<int>(mFixedView.GetLoadingStatus()),
                mReadyCount);
    std::fflush(stdout);
  }

  void UpdateInfo()
  {
    if(!mInfoLabel || !mWrapView)
    {
      return;
    }
    const SvgVariant& variant = VARIANTS[mFileVariant];
    const Vector3     natural = mWrapView.GetNaturalSize();
    char              text[256];
    std::snprintf(text,
                  sizeof(text),
                  "File on disk: %s (%ux%u)\nShown natural size: %.0fx%.0f\nResourceReady signals since last reload: %d",
                  variant.name,
                  variant.width,
                  variant.height,
                  natural.width,
                  natural.height,
                  mReadyCount);
    mInfoLabel.SetText(text);
  }

  void OnKeyEvent(Window, KeyEvent event)
  {
    if(event.GetState() != KeyEvent::DOWN)
    {
      return;
    }
    if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
    {
      mApplication.Quit();
    }
    else if(event.GetKeyName() == "w")
    {
      Rewrite();
    }
    else if(event.GetKeyName() == "r")
    {
      Reload();
    }
  }

private:
  Application&          mApplication;
  std::filesystem::path mPath;
  ImageView             mWrapView;
  ImageView             mFixedView;
  Label                 mInfoLabel;
  Timer                 mAutoTimer;
  int                   mFileVariant{0};
  int                   mReadyCount{0};
  int                   mAutoStep{0};
  bool                  mAutoRun;
};

const SvgReloadController::SvgVariant SvgReloadController::VARIANTS[SvgReloadController::VARIANT_COUNT] = {
  {"blue", 120u, 60u, "#1E88E5"},
  {"green", 240u, 120u, "#43A047"},
  {"orange", 90u, 180u, "#FB8C00"},
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  bool autoRun = false;
  for(int i = 1; i < argc; ++i)
  {
    if(std::string(argv[i]) == "--auto")
    {
      autoRun = true;
    }
  }

  Application application = Application::New(&argc, &argv);
  UiConfig    config      = UiConfig::New();
  config.SetDefaultStateEffectForInteractive(OverlayEffect::Plain());
  config.Apply();
  SvgReloadController controller(application, autoRun);
  application.MainLoop();
  return 0;
}
