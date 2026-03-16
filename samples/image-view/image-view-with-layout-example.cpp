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
#include <dali-ui-foundation/public-api/layout-types.h>
#include <dali-ui-foundation/public-api/stack-layout.h>
#include <dali-ui-foundation/public-api/stack-layout-params.h>
#include <dali/integration-api/debug.h>

using namespace Dali;
using namespace Dali::Ui;

/**
 * ImageView with Layout sample: demonstrates 4 ImageViews arranged in a 2x2
 * grid using nested StackLayouts (vertical root -> 2 horizontal rows).
 *
 * Layout structure:
 *   root (vertical StackLayout, MATCH_PARENT x MATCH_PARENT)
 *   ├── topRow (horizontal StackLayout, MATCH_PARENT x weight 1)
 *   │   ├── mFitImage    (weight 1 x MATCH_PARENT)
 *   │   └── mFillImage   (weight 1 x MATCH_PARENT)
 *   └── bottomRow (horizontal StackLayout, MATCH_PARENT x weight 1)
 *       ├── mTintedImage (weight 1 x MATCH_PARENT)
 *       └── mWrapImage   (weight 1 x MATCH_PARENT)
 *
 * Press Escape or Back to quit.
 */
class ImageViewWithLayoutController : public ConnectionTracker
{
public:
  explicit ImageViewWithLayoutController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &ImageViewWithLayoutController::OnInit);
  }

private:
  void OnInit(Application& application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::DARK_GRAY);

    window.Add(CreateContents());

    mFitImage.ResourceReadySignal().Connect(this, &ImageViewWithLayoutController::OnResourceReady);
    window.KeyEventSignal().Connect(this, &ImageViewWithLayoutController::OnKeyEvent);
  }

  View CreateContents()
  {
    return StackLayout::New(StackOrientation::VERTICAL)
      .Spacing(8.0f)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(MATCH_PARENT)
      .Children({
        CreateRow(StackOrientation::HORIZONTAL)
          .Children({
            ImageView::New(RESOURCES_DIR "gallery-large-3.jpg")
              .SetRequestedWidth(WRAP_CONTENT)
              .SetRequestedHeight(MATCH_PARENT)
              .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
              .As(mFitImage),
            ImageView::New(RESOURCES_DIR "gallery-medium-3.jpg")
              .SetRequestedWidth(WRAP_CONTENT)
              .SetRequestedHeight(MATCH_PARENT)
              .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
              .As(mFillImage),
          }),
        CreateRow(StackOrientation::HORIZONTAL)
          .Children({
            ImageView::New(RESOURCES_DIR "gallery-medium-49.jpg")
              .SetRequestedWidth(WRAP_CONTENT)
              .SetRequestedHeight(MATCH_PARENT)
              .SetImageColor(Vector4(1.0f, 0.5f, 0.5f, 1.0f))
              .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
              .As(mTintedImage),
            ImageView::New(RESOURCES_DIR "people-small-10.jpg")
              .SetRequestedWidth(WRAP_CONTENT)
              .SetRequestedHeight(MATCH_PARENT)
              .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
              .As(mWrapImage),
          }),
      });
  }

  StackLayout CreateRow(StackOrientation orientation)
  {
    return StackLayout::New(orientation)
      .Spacing(8.0f)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
  }

  void OnResourceReady(ImageView view)
  {
    DALI_LOG_RELEASE_INFO("ImageView: resource ready! status=%d\n", static_cast<int>(view.GetLoadingStatus()));
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

private:
  Application&  mApplication;
  Ui::ImageView mFitImage;
  Ui::ImageView mFillImage;
  Ui::ImageView mTintedImage;
  Ui::ImageView mWrapImage;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  ImageViewWithLayoutController controller(application);
  application.MainLoop();
  return 0;
}
